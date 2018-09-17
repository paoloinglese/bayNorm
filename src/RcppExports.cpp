// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppArmadillo.h>
#include <Rcpp.h>

using namespace Rcpp;

// DownSampling
NumericMatrix DownSampling(NumericMatrix Data, NumericVector BETA_vec);
RcppExport SEXP _bayNorm_DownSampling(SEXP DataSEXP, SEXP BETA_vecSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type Data(DataSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA_vec(BETA_vecSEXP);
    rcpp_result_gen = Rcpp::wrap(DownSampling(Data, BETA_vec));
    return rcpp_result_gen;
END_RCPP
}
// MarginalF_NB_1D
double MarginalF_NB_1D(double SIZE, double MU, NumericVector m_observed, NumericVector BETA);
RcppExport SEXP _bayNorm_MarginalF_NB_1D(SEXP SIZESEXP, SEXP MUSEXP, SEXP m_observedSEXP, SEXP BETASEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type SIZE(SIZESEXP);
    Rcpp::traits::input_parameter< double >::type MU(MUSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type m_observed(m_observedSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA(BETASEXP);
    rcpp_result_gen = Rcpp::wrap(MarginalF_NB_1D(SIZE, MU, m_observed, BETA));
    return rcpp_result_gen;
END_RCPP
}
// MarginalF_NB_2D
double MarginalF_NB_2D(NumericVector SIZE_MU, NumericVector m_observed, NumericVector BETA);
RcppExport SEXP _bayNorm_MarginalF_NB_2D(SEXP SIZE_MUSEXP, SEXP m_observedSEXP, SEXP BETASEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type SIZE_MU(SIZE_MUSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type m_observed(m_observedSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA(BETASEXP);
    rcpp_result_gen = Rcpp::wrap(MarginalF_NB_2D(SIZE_MU, m_observed, BETA));
    return rcpp_result_gen;
END_RCPP
}
// GradientFun_NB_1D
double GradientFun_NB_1D(double SIZE, double MU, NumericVector m_observed, NumericVector BETA);
RcppExport SEXP _bayNorm_GradientFun_NB_1D(SEXP SIZESEXP, SEXP MUSEXP, SEXP m_observedSEXP, SEXP BETASEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type SIZE(SIZESEXP);
    Rcpp::traits::input_parameter< double >::type MU(MUSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type m_observed(m_observedSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA(BETASEXP);
    rcpp_result_gen = Rcpp::wrap(GradientFun_NB_1D(SIZE, MU, m_observed, BETA));
    return rcpp_result_gen;
END_RCPP
}
// GradientFun_NB_2D
NumericVector GradientFun_NB_2D(NumericVector SIZE_MU, NumericVector m_observed, NumericVector BETA);
RcppExport SEXP _bayNorm_GradientFun_NB_2D(SEXP SIZE_MUSEXP, SEXP m_observedSEXP, SEXP BETASEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type SIZE_MU(SIZE_MUSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type m_observed(m_observedSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA(BETASEXP);
    rcpp_result_gen = Rcpp::wrap(GradientFun_NB_2D(SIZE_MU, m_observed, BETA));
    return rcpp_result_gen;
END_RCPP
}
// GradientFun_NBmu_1D
double GradientFun_NBmu_1D(double SIZE, double MU, NumericVector m_observed, NumericVector BETA);
RcppExport SEXP _bayNorm_GradientFun_NBmu_1D(SEXP SIZESEXP, SEXP MUSEXP, SEXP m_observedSEXP, SEXP BETASEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type SIZE(SIZESEXP);
    Rcpp::traits::input_parameter< double >::type MU(MUSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type m_observed(m_observedSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA(BETASEXP);
    rcpp_result_gen = Rcpp::wrap(GradientFun_NBmu_1D(SIZE, MU, m_observed, BETA));
    return rcpp_result_gen;
END_RCPP
}
// Main_NB_Bay
NumericVector Main_NB_Bay(NumericMatrix Data, NumericVector BETA_vec, NumericVector size, Nullable<NumericVector> mu, int S, int thres);
RcppExport SEXP _bayNorm_Main_NB_Bay(SEXP DataSEXP, SEXP BETA_vecSEXP, SEXP sizeSEXP, SEXP muSEXP, SEXP SSEXP, SEXP thresSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type Data(DataSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA_vec(BETA_vecSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type size(sizeSEXP);
    Rcpp::traits::input_parameter< Nullable<NumericVector> >::type mu(muSEXP);
    Rcpp::traits::input_parameter< int >::type S(SSEXP);
    Rcpp::traits::input_parameter< int >::type thres(thresSEXP);
    rcpp_result_gen = Rcpp::wrap(Main_NB_Bay(Data, BETA_vec, size, mu, S, thres));
    return rcpp_result_gen;
END_RCPP
}
// Main_mean_NB_Bay
NumericMatrix Main_mean_NB_Bay(NumericMatrix Data, NumericVector BETA_vec, NumericVector size, Nullable<NumericVector> mu, int S, int thres);
RcppExport SEXP _bayNorm_Main_mean_NB_Bay(SEXP DataSEXP, SEXP BETA_vecSEXP, SEXP sizeSEXP, SEXP muSEXP, SEXP SSEXP, SEXP thresSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type Data(DataSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA_vec(BETA_vecSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type size(sizeSEXP);
    Rcpp::traits::input_parameter< Nullable<NumericVector> >::type mu(muSEXP);
    Rcpp::traits::input_parameter< int >::type S(SSEXP);
    Rcpp::traits::input_parameter< int >::type thres(thresSEXP);
    rcpp_result_gen = Rcpp::wrap(Main_mean_NB_Bay(Data, BETA_vec, size, mu, S, thres));
    return rcpp_result_gen;
END_RCPP
}
// Main_mode_NB_Bay
NumericMatrix Main_mode_NB_Bay(NumericMatrix Data, NumericVector BETA_vec, NumericVector size, Nullable<NumericVector> mu, int S, int thres);
RcppExport SEXP _bayNorm_Main_mode_NB_Bay(SEXP DataSEXP, SEXP BETA_vecSEXP, SEXP sizeSEXP, SEXP muSEXP, SEXP SSEXP, SEXP thresSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type Data(DataSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type BETA_vec(BETA_vecSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type size(sizeSEXP);
    Rcpp::traits::input_parameter< Nullable<NumericVector> >::type mu(muSEXP);
    Rcpp::traits::input_parameter< int >::type S(SSEXP);
    Rcpp::traits::input_parameter< int >::type thres(thresSEXP);
    rcpp_result_gen = Rcpp::wrap(Main_mode_NB_Bay(Data, BETA_vec, size, mu, S, thres));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_bayNorm_DownSampling", (DL_FUNC) &_bayNorm_DownSampling, 2},
    {"_bayNorm_MarginalF_NB_1D", (DL_FUNC) &_bayNorm_MarginalF_NB_1D, 4},
    {"_bayNorm_MarginalF_NB_2D", (DL_FUNC) &_bayNorm_MarginalF_NB_2D, 3},
    {"_bayNorm_GradientFun_NB_1D", (DL_FUNC) &_bayNorm_GradientFun_NB_1D, 4},
    {"_bayNorm_GradientFun_NB_2D", (DL_FUNC) &_bayNorm_GradientFun_NB_2D, 3},
    {"_bayNorm_GradientFun_NBmu_1D", (DL_FUNC) &_bayNorm_GradientFun_NBmu_1D, 4},
    {"_bayNorm_Main_NB_Bay", (DL_FUNC) &_bayNorm_Main_NB_Bay, 6},
    {"_bayNorm_Main_mean_NB_Bay", (DL_FUNC) &_bayNorm_Main_mean_NB_Bay, 6},
    {"_bayNorm_Main_mode_NB_Bay", (DL_FUNC) &_bayNorm_Main_mode_NB_Bay, 6},
    {NULL, NULL, 0}
};

RcppExport void R_init_bayNorm(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
