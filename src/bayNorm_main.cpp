#define ARMA_64BIT_WORD 1
#include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
#include <Rmath.h>
#include <Rcpp.h>
#include <progress.hpp>
#include <progress_bar.hpp>
// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::depends(RcppProgress)]]



using namespace Rcpp;
using namespace arma;
using namespace std;


struct add_multiple {
  int incr;
  int count;
  add_multiple(int incr)
    : incr(incr), count(0)
  {}
  inline int operator()(int d) {
    return d + incr * count++;
  }
};

Rcpp::NumericVector rcpp_seq(double from_, double to_, double by_ = 1.0) {
  int adjust = std::pow(10, std::ceil(std::log10(10 / by_)) - 1);
  int from = adjust * from_;
  int to = adjust * to_;
  int by = adjust * by_;

  std::size_t n = ((to - from) / by) + 1;
  Rcpp::IntegerVector res = Rcpp::rep(from, n);
  add_multiple ftor(by);

  std::transform(res.begin(), res.end(), res.begin(), ftor);
  return Rcpp::NumericVector(res) / adjust;
}


double chooseC(double n, double k) {
  return Rf_choose(n, k);
}




double post_binom_unif(double n, double m, double beta) {
  double prob;
  arma::vec k = arma::linspace<vec>(0, m-1, m);
  arma::vec k_vec;

  if(n<m){prob=0;}
  else{
    if(chooseC(n,m)==R_PosInf){

      k_vec= (n-k)/(m-k)*pow((1-beta),(n-m)/m)*beta;

      prob=arma::prod(k_vec)*beta;
    }
    else{
      prob = beta * chooseC(n,m) * pow(beta,m) * pow((1-beta),(n-m));
    }

  }
  return(prob);
}




double post_binom_nb(double n, double m,  double beta, double size, double m_ave) {
  double prob;
  arma::vec k = arma::linspace<vec>(0, m-1, m);
  arma::vec k_vec;

  if(n<m){prob=0;}
  else{

    if(chooseC(n,m)==R_PosInf){

      k_vec= (n-k)/(m-k)*pow((1-beta),(n-m)/m)*beta;

      prob=prod(k_vec)*beta*dnbinom_mu(n,size,m_ave,0);
    }

    else{
      prob = beta *chooseC(n,m) * pow(beta,m) * pow((1-beta),(n-m))*dnbinom_mu(n, size, m_ave, 0);
    }

  }
  return(prob);

}




NumericVector post_wrapper(IntegerVector x,double m,double beta,double size, double m_ave, int last,int Indicate) {
  NumericVector y(last+1);

  if(Indicate==1)
  {
    for(int temp=0;temp<(last+1);temp++){
      y(temp)=post_binom_nb(x(temp), m, beta, size, m_ave);
    }
  } else if(Indicate==0){
    for(int temp2=0;temp2<(last+1);temp2++){
      y(temp2)=post_binom_unif(x(temp2), m, beta);
    }
  }

  return(y);
}


NumericVector post_wrapper_norm(IntegerVector x,double m,double beta,int last, int init) {

  NumericVector y(last-init+1);

    for(int temp2=0;temp2<(last-init+1);temp2++){
      y(temp2)=R::dnorm(m,x(temp2)*beta,x(temp2)*beta*(1- beta),false);
    }
  return(y);
}


// // ' @title Main_Bay (1D grid approximation, slow)
// // '
// // ' @description
// // ' If the observed count is above 500,
// // ' then we use normal distribution to
// // ' approximate binomial distribution.
// //'
// //'
// //' @param Data raw count Data
// // //' @param BETA_vec A vector of capture efficiencies of cells
// //' @param size A vector of size
// //' @param mu A vector of mu
// //' @param S Draw S samples from posterior
// //' distribution to form 3D array
// //' @param thres For observed count greater
// //' than \code{thres}, use uniform prior.
// //' @return bayNorm normalized data
// //'
// //'
// //' @examples
// //' data("EXAMPLE_DATA_list")
// //' \dontrun{
// //' data("EXAMPLE_DATA_list")
// //' Norm_3D_array<-Main_Bay(Data=EXAMPLE_DATA_list$inputdata,
// //' BETA_vec = EXAMPLE_DATA_list$inputbeta,
// //' size=EXAMPLE_DATA_list$size,mu=EXAMPLE_DATA_list$mu,
// //' S=20,thres=10000000)
// //' }
// //' @export
// // [[Rcpp::export]]




NumericVector Main_Bay(NumericMatrix Data,
                       NumericVector BETA_vec,
                       NumericVector size,
                       Nullable<NumericVector> mu,
                       int S,int thres)
{



  arma::mat M = Rcpp::as<arma::mat>(Data);

  arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
  arma::vec M_ave;
  arma::mat M_t;

  int nrow=M.n_rows;
  int ncol=M.n_cols;
  int i;
  int j;
  int q;


  int last;
  int init;
  int NormalApproThre=500;

  IntegerVector x;
  NumericVector y;

  arma::cube Final_mat(nrow, ncol, S);


  if (mu.isNotNull())
  {

    M_ave = Rcpp::as<arma::vec>(mu);
  }
  //
  //
  // else{
  //
  //   arma::rowvec M_colmean=arma::sum(M, dim=0 );
  //
  //   M_t=M.each_row() / M_colmean;
  //   M_ave = arma::mean(M_t, dim=1 )*Mean_depth;
  // }

  Progress p(ncol*nrow, true);

  for( i=0;i<ncol;i++){
   // Rcout << "The cell is \n" << i+1 << std::endl;

    for( j=0;j<nrow;j++){

      p.increment();

      //if(debug)
      //{Rcout << "The gene is \n" << j+1 << std::endl;}

      if(M(j,i)==NA_INTEGER) {
        for( q=0;q<S;q++){Final_mat(j,i,q)=NA_INTEGER;}
      }

      else{

        if(M(j,i)<NormalApproThre){
      last=floor((M(j,i)+1)*3/Beta(i));
      x=(seq_len(last+1)-1);

        if(M(j,i)<thres){

          y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,1);
        }

        else{
          y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,0);
        }
        }else{
          init=M(j,i)/3/Beta(i);
          last=M(j,i)*3/Beta(i);
          x=rcpp_seq(init,last,1);
           y=post_wrapper_norm(x,M(j,i),Beta(i),last,init);

        }

        NumericVector z=y/sum(y);


        //Rcout << "The gene is \n" << j+1 << std::endl;
        IntegerVector S_temp=Rcpp::RcppArmadillo::sample(x, S, true, z);
        arma::vec S_input = Rcpp::as<arma::vec>(S_temp);
        Final_mat.subcube(j,i,0,j,i,S-1)=S_input;


      } //end of else


    } //j

  }  //i


  NumericVector Final_mat2=Rcpp::wrap(Final_mat);
  Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
  Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

return(Rcpp::wrap(Final_mat2));
}


// //' @title  Mode_Bay
// //'
// //' @description bayNorm (1D grid approximation, slow)
// //' If the observed count is above 500,
// //' then we use normal distribution to
// //' approximate binomial distribution.
// //'
// //'
// //' @param Data raw count Data
// //' @param BETA_vec A vector of capture efficiencies of cells
// //' @param size A vector of size
// //' @param mu A vector of mu
// //' @param S number of samples that you want to generate
// //' @param thres for observed count greater
// //' than \code{thres}, use uniform prior.
// //' @return bayNorm normalized data
// //'
// //' @examples
// //' data("EXAMPLE_DATA_list")
// //' \dontrun{
// //' data("EXAMPLE_DATA_list")
// //' Norm_2D_matrix<-Main_mode_Bay(Data=EXAMPLE_DATA_list$inputdata,
// //' BETA_vec = EXAMPLE_DATA_list$inputbeta,
// //' size=EXAMPLE_DATA_list$size,mu=EXAMPLE_DATA_list$mu,
// //' S=20,thres=10000000)
// //' }
// //' @export
// // [[Rcpp::export]]
NumericMatrix Main_mode_Bay(NumericMatrix Data,
                            NumericVector BETA_vec,
                            NumericVector size,
                            Nullable<NumericVector> mu,
                            int S,int thres)
{



  arma::mat M = Rcpp::as<arma::mat>(Data);

  arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
  arma::vec M_ave;
  arma::mat M_t;

  int nrow=M.n_rows;
  int ncol=M.n_cols;
  int i;
  int j;
  int q;
  int NormalApproThre=500;

  int last;
  int init;
  IntegerVector x;
  NumericVector y;

  arma::mat Final_mat(nrow, ncol);


  if (mu.isNotNull())
  {

    M_ave = Rcpp::as<arma::vec>(mu);
  }

  // else{
  //   arma::rowvec M_colmean=arma::sum(M, dim=0 );
  //   M_t=M.each_row() / M_colmean;
  //   M_ave = arma::mean(M_t, dim=1 )*Mean_depth;
  // }

  Progress p(ncol*nrow, true);


  for( i=0;i<ncol;i++){
    //Rcout << "The cell is \n" << i+1 << std::endl;

    for( j=0;j<nrow;j++){

      p.increment();

      if(M(j,i)==NA_INTEGER) {
        for( q=0;q<S;q++){Final_mat(j,i)=NA_INTEGER;}
      }

      else{

          if(M(j,i)<NormalApproThre){

          last=floor((M(j,i)+1)*3/Beta(i));
          x=(seq_len(last+1)-1);

          if(M(j,i)<thres){

            y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,1);
          }

          else{
            y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,0);
          }
        }else{
          init=M(j,i)/3/Beta(i);
          last=M(j,i)*3/Beta(i);
          x=rcpp_seq(init,last,1);
          y=post_wrapper_norm(x,M(j,i),Beta(i),last,init);

        }

        NumericVector z=y/sum(y);

        arma::vec x_arma= Rcpp::as<arma::vec>(x);
        arma::vec z_arma= Rcpp::as<arma::vec>(z);
        arma::uword max_index=index_max(z_arma);
        Final_mat(j,i)=mean(x_arma(max_index));

        //Rcout << "The gene is \n" << j+1 << std::endl;
        //IntegerVector S_temp=Rcpp::RcppArmadillo::sample(x, S, true, z);
        //arma::vec S_input = Rcpp::as<arma::vec>(S_temp);

        //Final_mat.subcube(j,i,0,j,i,S-1)=S_input;

      } //end of else


    } //j

  }  //i



  NumericVector Final_mat2=Rcpp::wrap(Final_mat);
  Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
  Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

  return(Rcpp::wrap(Final_mat2));
}

/*
//' @title  Mean_Bay
//'
//' @description bayNorm (1D grid approximation, slow)
//' If the observed count is above 500,
//' then we use normal distribution to
//' approximate binomial distribution.
//'
//'
//' @param Data raw count Data
//' @param BETA_vec A vector of capture efficiencies of cells
//' @param size A vector of size
//' @param mu A vector of mu
//' @param S number of samples that you want to generate
//' @param thres for observed count greater
//' than \code{thres}, use uniform prior.
//' @return bayNorm normalized data
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' \dontrun{
//' data("EXAMPLE_DATA_list")
//' Norm_2D_matrix<-Main_mean_Bay(Data=EXAMPLE_DATA_list$inputdata,
//' BETA_vec = EXAMPLE_DATA_list$inputbeta,
//' size=EXAMPLE_DATA_list$size,mu=EXAMPLE_DATA_list$mu,
//' S=20,thres=10000000)
//' }
//' @export
// [[Rcpp::export]]
*/


NumericMatrix Main_mean_Bay(NumericMatrix Data,
                            NumericVector BETA_vec,
                            NumericVector size,
                            Nullable<NumericVector> mu,
                            int S,int thres)
{



    arma::mat M = Rcpp::as<arma::mat>(Data);

    arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
    arma::vec M_ave;
    arma::mat M_t;

    int nrow=M.n_rows;
    int ncol=M.n_cols;
    int i;
    int j;
    int q;
    int NormalApproThre=500;

    int last;
    int init;
    IntegerVector x;
    NumericVector y;

    arma::mat Final_mat(nrow, ncol);


    if (mu.isNotNull())
    {

        M_ave = Rcpp::as<arma::vec>(mu);
    }

    // else{
    //   arma::rowvec M_colmean=arma::sum(M, dim=0 );
    //   M_t=M.each_row() / M_colmean;
    //   M_ave = arma::mean(M_t, dim=1 )*Mean_depth;
    // }

    Progress p(ncol*nrow, true);


    for( i=0;i<ncol;i++){
        //Rcout << "The cell is \n" << i+1 << std::endl;

        for( j=0;j<nrow;j++){

            p.increment();

            if(M(j,i)==NA_INTEGER) {
                for( q=0;q<S;q++){Final_mat(j,i)=NA_INTEGER;}
            }

            else{

                if(M(j,i)<NormalApproThre){

                    last=floor((M(j,i)+1)*3/Beta(i));
                    x=(seq_len(last+1)-1);

                    if(M(j,i)<thres){

                        y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,1);
                    }

                    else{
                        y=post_wrapper(x,M(j,i),Beta(i),size(j), M_ave(j), last,0);
                    }
                }else{
                    init=M(j,i)/3/Beta(i);
                    last=M(j,i)*3/Beta(i);
                    x=rcpp_seq(init,last,1);
                    y=post_wrapper_norm(x,M(j,i),Beta(i),last,init);

                }

                NumericVector z=y/sum(y);

                arma::vec x_arma= Rcpp::as<arma::vec>(x);
                arma::vec z_arma= Rcpp::as<arma::vec>(z);
                //arma::uword max_index=index_max(z_arma);
                IntegerVector S_temp=Rcpp::RcppArmadillo::sample(x, S, true, z);
                //arma::vec S_input = Rcpp::as<arma::vec>(S_temp);




                Final_mat(j,i)=mean(S_temp);

                //Rcout << "The gene is \n" << j+1 << std::endl;


                //Final_mat.subcube(j,i,0,j,i,S-1)=S_input;

            } //end of else


        } //j

    }  //i



    NumericVector Final_mat2=Rcpp::wrap(Final_mat);
    Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
    Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

    return(Rcpp::wrap(Final_mat2));
}



double D_SIZE(double SIZE,double MU, NumericVector m_observed,NumericVector BETA) {

  NumericVector m=m_observed;
  int nCells=m.size();

  NumericVector temp_vec_2(nCells);


  double MarginalVal;

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;
    int n_length=n.size();
    NumericVector numerator_1(n_length);
    NumericVector numerator_2(n_length);
    NumericVector kernel_vec(n_length);

    for(int j=0;j<n_length;j++){

      kernel_vec(j)=R::dbinom(m(i),n(j),BETA(i),false)*R::dnbinom_mu(n(j),SIZE,MU,false);
      numerator_1(j)=R::digamma(n(j)+SIZE)-R::digamma(SIZE)+log(SIZE/(SIZE+MU))+(MU-n(j))/(MU+SIZE);
      numerator_2(j)=kernel_vec(j)*numerator_1(j);
    }


    temp_vec_2(i)=sum(numerator_2)/sum(kernel_vec);


  }

  MarginalVal=sum(temp_vec_2);
  return MarginalVal;
}




double D_MU(double SIZE,double MU, NumericVector m_observed,NumericVector BETA) {
  NumericVector m=m_observed;
  int nCells=m.size();
  NumericVector temp_vec_2(nCells);
  double MarginalVal;

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;
    int n_length=n.size();
    NumericVector numerator_1(n_length);
    NumericVector numerator_2(n_length);
    NumericVector kernel_vec(n_length);

    for(int j=0;j<n_length;j++){

      kernel_vec(j)=R::dbinom(m(i),n(j),BETA(i),false)*R::dnbinom_mu(n(j),SIZE,MU,false);
      numerator_1(j)=SIZE*(n(j)-MU)/(MU*(MU+SIZE));
      numerator_2(j)=kernel_vec(j)*numerator_1(j);

    }
    temp_vec_2(i)=sum(numerator_2);
  }
  MarginalVal=sum(temp_vec_2);
  return MarginalVal;
}





NumericVector D_SIZE_MU_2D(NumericVector SIZE_MU, NumericVector m_observed,NumericVector BETA) {

  NumericVector m=m_observed;
  int nCells=m.size();
  NumericVector temp_vec_2_SIZE(nCells);
  NumericVector temp_vec_2_MU(nCells);

  NumericVector MarginalVal_vec(2);

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;

    int n_length=n.size();
    NumericVector numerator_SIZE(n_length);
    NumericVector numerator_MU(n_length);
    NumericVector numerator_2_SIZE(n_length);
    NumericVector numerator_2_MU(n_length);
    NumericVector kernel_vec(n_length);

    for(int j=0;j<n_length;j++){

      kernel_vec(j)=R::dbinom(m(i),n(j),BETA(i),false)*R::dnbinom_mu(n(j),SIZE_MU(0),SIZE_MU(1),false);
      numerator_SIZE(j)=R::digamma(n(j)+SIZE_MU(0))-R::digamma(SIZE_MU(0))+log(SIZE_MU(0)/(SIZE_MU(0)+SIZE_MU(1)))+(SIZE_MU(1)-n(j))/(SIZE_MU(1)+SIZE_MU(0));
      numerator_MU(j)=SIZE_MU(0)*(n(j)-SIZE_MU(1))/(SIZE_MU(1)*(SIZE_MU(1)+SIZE_MU(0)));
      numerator_2_SIZE(j)=kernel_vec(j)*numerator_SIZE(j);
      numerator_2_MU(j)=kernel_vec(j)*numerator_MU(j);

    }

    temp_vec_2_SIZE(i)=sum(numerator_2_SIZE)/sum(kernel_vec);
    temp_vec_2_MU(i)=sum(numerator_2_MU)/sum(kernel_vec);
  }
  MarginalVal_vec[0]=sum(temp_vec_2_SIZE);
  MarginalVal_vec[1]=sum(temp_vec_2_MU);
  return MarginalVal_vec;
}


/*
//' @title GradientFun_2D
//'
//' @description First derivative of marginal distribution
//' with respect to both size and mu.
//'
//' @param SIZE_MU a vector of two elements (size,mu)
//' @param m_observed a vector of observed counts
//' @param BETA The corresponding capture efficiency
//' @return GradientFun_2D
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users.
//' \dontrun{
//' }
//'
//'
//' @export
// [[Rcpp::export]]
*/
NumericVector GradientFun_2D(NumericVector SIZE_MU,
                             NumericVector m_observed,
                             NumericVector BETA){
  NumericVector m=m_observed;
  NumericVector Grad_vec(2);
  Grad_vec=D_SIZE_MU_2D(SIZE_MU,m,BETA);
  return Grad_vec;
}


/*
//' @title MarginalF_2D
//'
//' @description Mariginal distribution with
//' respect to both size and mu.
//'
//' @param SIZE_MU a vector of two elements (size,mu)
//' @param m_observed  m_observed
//' @param BETA Corresponding capture efficiency
//' @return Marginal likelihood
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]
 */
double MarginalF_2D(NumericVector SIZE_MU,
                    NumericVector m_observed,
                    NumericVector BETA) {
  NumericVector m=m_observed;
  int nCells=m.size();
  NumericVector temp_vec_2(nCells);


  double MarginalVal;

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;
    int n_length=n.size();
    NumericVector temp_vec_1(n_length);

    for(int j=0;j<n_length;j++){

      temp_vec_1(j)=R::dbinom(m(i),n(j),BETA(i),false)
      *R::dnbinom_mu(n(j),SIZE_MU(0),SIZE_MU(1),false);

    }

    arma::vec temp_vec_1arma = Rcpp::as<arma::vec>(temp_vec_1);

    temp_vec_2(i)=sum(temp_vec_1arma);

  }
  MarginalVal=sum(log(temp_vec_2));
  return MarginalVal;
}



double D_SIZE_MU_1D(double SIZE,double MU, NumericVector m_observed,NumericVector BETA) {

  NumericVector m=m_observed;
  int nCells=m.size();
  NumericVector temp_vec_2_SIZE(nCells);
  NumericVector temp_vec_2_MU(nCells);

  double MarginalVal;

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;
    int n_length=n.size();
    NumericVector numerator_SIZE(n_length);
    NumericVector numerator_MU(n_length);
    NumericVector numerator_2_SIZE(n_length);
    NumericVector numerator_2_MU(n_length);
    NumericVector kernel_vec(n_length);

    for(int j=0;j<n_length;j++){

      kernel_vec(j)=R::dbinom(m(i),n(j),BETA(i),false)*R::dnbinom_mu(n(j),SIZE,MU,false);
      numerator_SIZE(j)=R::digamma(n(j)+SIZE)-R::digamma(SIZE)+log(SIZE/(SIZE+MU))+(MU-n(j))/(MU+SIZE);

      numerator_2_SIZE(j)=kernel_vec(j)*numerator_SIZE(j);
    }
    temp_vec_2_SIZE(i)=sum(numerator_2_SIZE)/sum(kernel_vec);
  }

  MarginalVal=sum(temp_vec_2_SIZE);
  return(MarginalVal);
}


/*
//' @title GradientFun_1D
//'
//' @description First derivative of marginal distribution
//' with respect to size.
//'
//'
//'
//' @param SIZE size
//' @param MU mu
//' @param m_observed one observed count
//' @param BETA The corresponding capture efficiency
//' @return GradientFun_1D
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users.
//' \dontrun{
//' }
//' @Keywords internal
// [[Rcpp::export]]
 */
double GradientFun_1D(double SIZE,double MU,
                      NumericVector m_observed,
                      NumericVector BETA){
  NumericVector m=m_observed;
  double Gradd;
  Gradd=D_SIZE_MU_1D(SIZE, MU,m,BETA);
  return Gradd;

}


/*
//' @title MarginalF_1D
//'
//' @description Mariginal distribution with respect to size.
//'
//' @param SIZE size
//' @param MU mu
//' @param m_observed one observed count
//' @param BETA The corresponding capture efficiency
//' @return Marginal likelihood
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users.
//' \dontrun{
//' }
//' @Keywords internal
// [[Rcpp::export]]
 */
double MarginalF_1D(double SIZE,double MU,
                    NumericVector m_observed,
                    NumericVector BETA) {
  NumericVector m=m_observed;
  int nCells=m.size();
  NumericVector temp_vec_2(nCells);


  double MarginalVal;

  for(int i=0;i<nCells;i++){

    int last=floor((m(i)+1)*3/BETA(i));
    NumericVector n=rcpp_seq(0,last, 1.0) ;
    int n_length=n.size();
    NumericVector temp_vec_1(n_length);

    for(int j=0;j<n_length;j++){
      temp_vec_1(j)=R::dbinom(m(i),n(j),BETA(i),false)*
        R::dnbinom_mu(n(j),SIZE,MU,false);

    }

    arma::vec temp_vec_1arma = Rcpp::as<arma::vec>(temp_vec_1);

    temp_vec_2(i)=sum(temp_vec_1arma);

  }
  MarginalVal=sum(log(temp_vec_2));
  return MarginalVal;
}



//' @title Binomial downsampling
//'
//' @description For each element in the \code{Data},
//' randomly generate a number using Binomial distribution with
//' probability equal to the specific capture efficiency.
//'
//' @param Data raw count Data
//' @param BETA_vec A vector of capture efficiencies of cells
//'
//' @return A matrix of binomial downsampling data.
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' Downsample_data<-DownSampling(Data=EXAMPLE_DATA_list$inputdata
//' ,BETA_vec = EXAMPLE_DATA_list$inputbeta)
//' @export
//'
// [[Rcpp::export]]
NumericMatrix DownSampling(NumericMatrix Data ,
                           NumericVector BETA_vec) {

  int Nrows;
  int Ncols;
  int i;
  int j;
  Nrows=Data.nrow();
  Ncols=Data.ncol();

  NumericMatrix Counts_downsampling(Nrows,Ncols);

  for(i=0;i<Nrows;i++){
    for( j=0;j<Ncols;j++){

      Counts_downsampling(i,j)= as<double>(rbinom(1,Data(i,j),
                                           BETA_vec(j)));
    }

  }

  return(Counts_downsampling);

}











// [[Rcpp::export]]
double MarginalF_NB_1D(double SIZE,double MU,
                    NumericVector m_observed,
                    NumericVector BETA) {
    NumericVector m=m_observed;
    int nCells=m.size();
    NumericVector temp_vec_2(nCells);

    double MarginalVal;


    for(int i=0;i<nCells;i++){
        temp_vec_2(i)=R::dnbinom_mu(m_observed(i),SIZE,BETA(i)*MU,true);
    }
    MarginalVal=sum(temp_vec_2);
    return MarginalVal;
}



// [[Rcpp::export]]
double MarginalF_NB_2D(NumericVector SIZE_MU,
                    NumericVector m_observed,
                    NumericVector BETA) {
    NumericVector m=m_observed;
    int nCells=m.size();
    NumericVector temp_vec_2(nCells);


    double MarginalVal;

    for(int i=0;i<nCells;i++){

        temp_vec_2(i)=R::dnbinom_mu(m_observed(i),SIZE_MU(0),SIZE_MU(1)*BETA(i),true);

    }
    MarginalVal=sum(temp_vec_2);
    return MarginalVal;
}





// [[Rcpp::export]]
double GradientFun_NB_1D(double SIZE,double MU,
                      NumericVector m_observed,
                      NumericVector BETA){
    NumericVector m=m_observed;
    double Gradd;

    int nCells=m.size();
    NumericVector temp_vec_2_SIZE(nCells);

    for(int i=0;i<nCells;i++){
        temp_vec_2_SIZE(i)=R::digamma(m_observed(i)+SIZE)-R::digamma(SIZE)+log(SIZE/(SIZE+MU*BETA(i)))+(BETA(i)*MU-m_observed(i))/(BETA(i)*MU+SIZE);

    }
    Gradd=sum(temp_vec_2_SIZE);
    return Gradd;
}




// [[Rcpp::export]]
NumericVector GradientFun_NB_2D(NumericVector SIZE_MU,
                         NumericVector m_observed,
                         NumericVector BETA){
    NumericVector m=m_observed;
    int nCells=m.size();
    NumericVector temp_size(nCells);
    NumericVector temp_mu(nCells);
    NumericVector Gradd_vec(2);

    for(int i=0;i<nCells;i++){
        temp_mu(i)=(m_observed(i)*SIZE_MU(0)-SIZE_MU(1)*BETA(i)*SIZE_MU(0))/(SIZE_MU(1)*(SIZE_MU(1)*BETA(i)+SIZE_MU(0)));

        temp_size(i)=R::digamma(m_observed(i)+SIZE_MU(0))-R::digamma(SIZE_MU(0))+log(SIZE_MU(0)/(SIZE_MU(0)+SIZE_MU(1)*BETA(i)))+(BETA(i)*SIZE_MU(1)-m_observed(i))/(BETA(i)*SIZE_MU(1)+SIZE_MU(0));

    }
    Gradd_vec(0)=sum(temp_size);
    Gradd_vec(1)=sum(temp_mu);


    return Gradd_vec;
}



/*
//' @title GradientFun_NBmu_1D
//'
//' @description First derivative of marginal distribution
//' with respect to size.
//'
//'
//'
//' @param SIZE size
//' @param MU mu
//' @param m_observed one observed count
//' @param BETA The corresponding capture efficiency
//' @return GradientFun_NB_1D
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users, it is used in prior estimation.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]
*/
double GradientFun_NBmu_1D(double SIZE,double MU,
                         NumericVector m_observed,
                         NumericVector BETA){
    NumericVector m=m_observed;
    double Gradd;

    int nCells=m.size();
    NumericVector temp_mu(nCells);

    for(int i=0;i<nCells;i++){

        temp_mu(i)=(m_observed(i)*SIZE-MU*BETA(i)*SIZE)/(MU*(MU*BETA(i)+SIZE));

    }
    Gradd=sum(temp_mu);
    return Gradd;
}






// [[Rcpp::export]]
NumericVector Main_NB_Bay(arma::sp_mat Data,
                       NumericVector BETA_vec,
                       NumericVector size,
                       Nullable<NumericVector> mu,
                       int S,int thres)
{


    arma::sp_mat M=Data;
    //arma::mat M = Rcpp::as<arma::mat>(Data);
    //arma::mat M(Data);

    arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
    arma::vec M_ave;
    arma::mat M_t;

    int nrow=M.n_rows;
    int ncol=M.n_cols;
    int i;
    int j;
    int q;
    double tempmu;
    NumericVector S_temp;


    IntegerVector x;
    NumericVector y;

    arma::cube Final_mat(nrow, ncol, S);


    if (mu.isNotNull())
    {

        M_ave = Rcpp::as<arma::vec>(mu);
    }


    Progress p(ncol*nrow, true);

    for( i=0;i<ncol;i++){
        // Rcout << "The cell is \n" << i+1 << std::endl;

        for( j=0;j<nrow;j++){

            p.increment();

            //if(debug)
            //{Rcout << "The gene is \n" << j+1 << std::endl;}

            if(M(j,i)==NA_INTEGER) {
                for( q=0;q<S;q++){Final_mat(j,i,q)=NA_INTEGER;}
            }

            else{
                tempmu=(M(j,i)+size(j))*(M_ave(j)-M_ave(j)*Beta(i))/(size(j)+M_ave(j)*Beta(i));
                //Rcout << "The gene is \n" << j+1 << std::endl;

                S_temp=Rcpp::rnbinom_mu(S,size(j)+M(j,i),tempmu)+M(j,i);
                arma::vec S_input = Rcpp::as<arma::vec>(S_temp);
                Final_mat.subcube(j,i,0,j,i,S-1)=S_input;


            } //end of else


        } //j

    }  //i


    NumericVector Final_mat2=Rcpp::wrap(Final_mat);
    // Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
    // Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

    return(Rcpp::wrap(Final_mat2));
}



// [[Rcpp::export]]
NumericMatrix Main_mean_NB_Bay(arma::sp_mat Data,
                            NumericVector BETA_vec,
                            NumericVector size,
                            Nullable<NumericVector> mu,
                            int S,int thres)
{



    //arma::mat M = Rcpp::as<arma::mat>(Data);
    //arma::mat M(Data);
    arma::sp_mat M=Data;

    arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
    arma::vec M_ave;
    arma::mat M_t;

    int nrow=M.n_rows;
    int ncol=M.n_cols;
    int i;
    int j;

    IntegerVector x;
    NumericVector y;

    double tempmu;

    arma::mat Final_mat(nrow, ncol);


    if (mu.isNotNull())
    {

        M_ave = Rcpp::as<arma::vec>(mu);
    }


    Progress p(ncol*nrow, true);


    for( i=0;i<ncol;i++){
        //Rcout << "The cell is \n" << i+1 << std::endl;

        for( j=0;j<nrow;j++){

            p.increment();

            if(M(j,i)==NA_INTEGER) {
                Final_mat(j,i)=NA_INTEGER;
            }

            else{
                tempmu=(M(j,i)+size(j))*(M_ave(j)-M_ave(j)*Beta(i))/(size(j)+M_ave(j)*Beta(i))+M(j,i);

                Final_mat(j,i)=tempmu;

                //Rcout << "The gene is \n" << j+1 << std::endl;


                //Final_mat.subcube(j,i,0,j,i,S-1)=S_input;

            } //end of else


        } //j

    }  //i



    NumericVector Final_mat2=Rcpp::wrap(Final_mat);
    //Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
    //Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

    return(Rcpp::wrap(Final_mat2));
}





// [[Rcpp::export]]
NumericMatrix Main_mode_NB_Bay(arma::sp_mat Data,
                            NumericVector BETA_vec,
                            NumericVector size,
                            Nullable<NumericVector> mu,
                            int S,int thres)
{



    //arma::mat M = Rcpp::as<arma::mat>(Data);
    //arma::mat M(Data);
    arma::sp_mat M=Data;
    
    arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
    arma::vec M_ave;
    arma::mat M_t;

    int nrow=M.n_rows;
    int ncol=M.n_cols;
    int i;
    int j;
    // double probb;
    double meann;


    arma::mat Final_mat(nrow, ncol);


    if (mu.isNotNull())
    {

        M_ave = Rcpp::as<arma::vec>(mu);
    }
    Progress p(ncol*nrow, true);




    for( i=0;i<ncol;i++){
        //Rcout << "The cell is \n" << i+1 << std::endl;

        for( j=0;j<nrow;j++){
            p.increment();

            if(M(j,i)==NA_INTEGER) {
                Final_mat(j,i)=NA_INTEGER;
            }

            else{
                if(((size(j)+M(j,i)))<=1){
                    Final_mat(j,i)=0;
                } else if((size(j)+M(j,i))>1){
                    meann=(M(j,i)+size(j))*(M_ave(j)-M_ave(j)*Beta(i))/(size(j)+M_ave(j)*Beta(i));
                    //probb=(M_ave(j)*Beta(i)+size(j)+M(j,i))/(M(j,i)+size(j)+M(j,i));
                    //Final_mat(j,i)= floor((1-probb)*(size(j)+M(j,i)-1)/probb)+M(j,i);
                    Final_mat(j,i)= floor(meann/(size(j)+M(j,i))*((size(j)+M(j,i))-1))+M(j,i);

                }

            } //end of else


        } //j

    }  //i



    NumericVector Final_mat2=Rcpp::wrap(Final_mat);
    //Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
    //Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);

    return(Rcpp::wrap(Final_mat2));
}


// [[Rcpp::export]]
NumericVector rowMeansFast(arma::sp_mat x) {
  int nrow = x.n_rows, ncol = x.n_cols;
  
  NumericVector means(nrow);
  for (int i = 0; i < nrow; ++i)
  {
    means(i) = 0;
  }
  for(arma::sp_mat::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    means(i.row()) += *i;
  }
  for (int i = 0; i < nrow; ++i)
  {
    means(i) /= ncol;
  }
  return means;
}

// [[Rcpp::export]]
NumericVector rowVarsFast(arma::sp_mat x, NumericVector means) {
  int nrow = x.n_rows, ncol = x.n_cols;
  
  NumericVector vars(nrow);
  NumericVector nonzero_vals(nrow);
  
  for(arma::sp_mat::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    vars(i.row()) += (*i-means(i.row()))*(*i-means(i.row())); 
    nonzero_vals(i.row()) += 1;
  }
  // Add back square mean error for zero elements
  // const_iterator only loops over nonzero elements 
  for (int i = 0; i < nrow; ++i)
  {
    vars(i) += (ncol - nonzero_vals(i))*(means(i)*means(i));
    vars(i) /= (ncol-1);
  }
  return vars;
}

//' @title Estimate size and mu for Negative Binomial distribution
//' for each gene using MME method (Rcpp version, sp_mat)
//'
//' @description  Input raw data and return
//' estimated size and mu for each gene using the MME method.
//' @param Data A matrix of single-cell expression where rows
//' are genes and columns are samples (cells). \code{Data}
//' can be of class \code{SummarizedExperiment} (the
//' assays slot contains the expression matrix and
//' is named "Counts") or just matrix.
//' @details mu and size are two parameters of the prior that
//' need to be specified for each gene in bayNorm.
//' They are parameters of negative binomial distribution.
//' The variance is \eqn{mu + mu^2/size} in this parametrization.
//'
//' @return  List containing estimated mu and
//' size for each gene.
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users, it is used in prior estimation.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]

List EstPrior_sprcpp(arma::sp_mat Data) {
  //arma::mat M = Rcpp::as<arma::mat>(Data);
  //arma::mat M(Data);
  
  int Nrows;
  int Ncols;
  
  Nrows=Data.n_rows;
  Ncols=Data.n_cols;
  
  double n=Ncols;
  //double debugg=(n - 1) / n;
  NumericVector m = rowMeansFast(Data);
  NumericVector vaaa=rowVarsFast(Data, m);
  
  NumericVector v = (n - 1) / n * vaaa;
  NumericVector mme_size = pow(m,2)/(v - m);
  
  // uvec q1 = find(v <= m);
  // int ll=q1.n_elem;
  
  List L = List::create(Named("MU") = m , _["SIZE"] = mme_size,_["v"]=v);
  return(Rcpp::wrap(L));
  //return(Rcpp::wrap(debugg));
}


//' @title Estimate size and mu for Negative Binomial distribution
//' for each gene using MME method (Rcpp version)
//'
//' @description  Input raw data and return
//' estimated size and mu for each gene using the MME method.
//' @param Data A matrix of single-cell expression where rows
//' are genes and columns are samples (cells). \code{Data}
//' can be of class \code{SummarizedExperiment} (the
//' assays slot contains the expression matrix and
//' is named "Counts") or just matrix.
//' @details mu and size are two parameters of the prior that
//' need to be specified for each gene in bayNorm.
//' They are parameters of negative binomial distribution.
//' The variance is \eqn{mu + mu^2/size} in this parametrization.
//'
//' @return  List containing estimated mu and
//' size for each gene.
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users, it is used in prior estimation.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]

List EstPrior_rcpp(NumericMatrix Data) {
  arma::mat M = Rcpp::as<arma::mat>(Data);
  //arma::mat M(Data);
  
  int Nrows;
  int Ncols;
  
  Nrows=size(M)(0);
  Ncols=size(M)(1);
  
  double n=Ncols;
  int dim;
  int norm_type;
  //double debugg=(n - 1) / n;
  
  arma::vec vaaa=var( M, norm_type=0, dim=1 );
  arma::vec m = mean( M, dim=1 );
  arma::vec v = (n - 1) / n * vaaa;
  arma::vec mme_size = pow(m,2)/(v - m);
  
  // uvec q1 = find(v <= m);
  // int ll=q1.n_elem;

  List L = List::create(Named("MU") = m , _["SIZE"] = mme_size,_["v"]=v);
  return(Rcpp::wrap(L));
  //return(Rcpp::wrap(debugg));
}

//' @title Transpose of sparse matrix
//'
//' @description  Transpose of sparse matrix
//' @param Data A matrix of single-cell expression where rows
//' are genes and columns are samples (cells). \code{Data}
//' can be of class \code{SummarizedExperiment} (the
//' assays slot contains the expression matrix and
//' is named "Counts") or just matrix.
//' @details Transpose of sparse matrix.
//'
//' @return  Transpose of sparse matrix.
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users, it is used in prior estimation.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]


arma::sp_mat t_sp(arma::sp_mat Data) {
  //arma::mat M = Rcpp::as<arma::mat>(Data);
  arma::sp_mat t_mat=trans(Data);
  
  return(t_mat);
}


//' @title Rcpp version: as.matrix
//'
//' @description  Rcpp version: as.matrix
//' @param rp vector
//' @param cp vector
//' @param z vector
//' @param nrows nrows
//' @param ncols ncols
//' @details Rcpp version: as.matrix
//'
//' @return  Matrix object in R.
//'
//' @examples
//' data("EXAMPLE_DATA_list")
//' #Should not run by the users, it is used in prior estimation.
//' \dontrun{
//' }
//' @export
// [[Rcpp::export]]
IntegerMatrix asMatrix(NumericVector rp,
                       NumericVector cp,
                       NumericVector z,
                       int nrows,
                       int ncols){
  
  int k = z.size() ;
  
  IntegerMatrix  mat(nrows, ncols);
  
  for (int i = 0; i < k; i++){
    mat(rp[i],cp[i]) = z[i];
  }
  
  return mat;
}



// [[Rcpp::export]]
arma::sp_mat Main_mean_NB_spBay(arma::sp_mat Data,
                                      NumericVector BETA_vec,
                                      NumericVector size,
                                      Nullable<NumericVector> mu,
                                      int S,int thres)
{
  
  
  
  //arma::mat M = Rcpp::as<arma::mat>(Data);
  //arma::mat M(Data);
  
  
  arma::vec Beta = Rcpp::as<arma::vec>(BETA_vec);
  arma::vec M_ave;
  //arma::mat M_t;
  
  int nrow=Data.n_rows;
  int ncol=Data.n_cols;
  
  //arma::sp_mat M=Data;
  //arma::mat M=zeros(nrow,ncol);
  
  
  int i;
  int j;
  
  IntegerVector x;
  NumericVector y;
  
  double tempmu;
  
  arma::mat Final_mat(nrow, ncol);
  
  
  if (mu.isNotNull())
  {
    
    M_ave = Rcpp::as<arma::vec>(mu);
  }
  
  
  Progress p(ncol*nrow, true);
  
  
  for( i=0;i<ncol;i++){
    //Rcout << "The cell is \n" << i+1 << std::endl;
    
    for( j=0;j<nrow;j++){
      
      p.increment();
      
      if(Data(j,i)==NA_INTEGER) {
        Final_mat(j,i)=NA_INTEGER;
      }
      
      else{
        tempmu=(Data(j,i)+size(j))*(M_ave(j)-M_ave(j)*Beta(i))/(size(j)+M_ave(j)*Beta(i))+Data(j,i);
        
        Final_mat(j,i)=tempmu;
        
        //Rcout << "The gene is \n" << j+1 << std::endl;
        
        
        //Final_mat.subcube(j,i,0,j,i,S-1)=S_input;
        
      } //end of else
      
      
    } //j
    
  }  //i
  
  
  
  //NumericVector Final_mat2=Rcpp::wrap(Final_mat);
  //Rcpp::rownames(Final_mat2) = Rcpp::rownames(Data);
  //Rcpp::colnames(Final_mat2) = Rcpp::colnames(Data);
  
  arma::sp_mat Final_mat2(Final_mat);
  
  return Final_mat2;
}


