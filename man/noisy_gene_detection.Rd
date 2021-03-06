% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/noisygene_detection.r
\name{noisy_gene_detection}
\alias{noisy_gene_detection}
\title{A wrapper function for noisy gene detection from raw data.
his produces synthetic control, performs bayNorm on both real
cell data and synthetic controls and does noisy gene detection.}
\usage{
noisy_gene_detection(
  Data,
  BETA_vec = NULL,
  mode_version = FALSE,
  mean_version = FALSE,
  S = 20,
  parallel = TRUE,
  NCores = 5,
  FIX_MU = TRUE,
  GR = FALSE,
  BB_SIZE = TRUE,
  verbose = TRUE,
  plot.out = FALSE,
  PRIORS = NULL,
  input_params = NULL
)
}
\arguments{
\item{Data}{A matrix of single-cell expression where rows
are genes and columns are samples (cells). \code{Data}
can be of class \code{SummarizedExperiment} (the
assays slot contains the expression matrix,
is named "Counts"), just \code{matrix} or sparse matrix.}

\item{BETA_vec}{A vector of capture efficiencies
of cells.}

\item{mode_version}{If TRUE, bayNorm return mode version
normalized data which is of 2D matrix instead of 3D array.
Default is FALSE.}

\item{mean_version}{If TRUE, bayNorm return
mean version normalized data which
is of 2D matrix instead of 3D array.
Default is FALSE.}

\item{S}{The number of samples you would like
to generate from estimated posterior distribution
(The third dimension of 3D array).
Default is 20. S needs to be specified if
\code{mode_version}=FALSE.}

\item{parallel}{If TRUE, \code{NCores} cores will be
used for parallelization. Default is TRUE.}

\item{NCores}{number of cores to use, default is 5.
This will be used to set up a parallel environment
using either MulticoreParam
(Linux, Mac) or SnowParam (Windows) with
NCores using the package BiocParallel.}

\item{FIX_MU}{Whether fix mu when estimating
parameters by
maximizing marginal distribution.
If TRUE, then 1D optimization,
otherwise 2D optimization (slow).}

\item{GR}{If TRUE, the gradient function will
be used in optimization. However since
the gradient function itself is very complicated,
it does not help too much in speeding up.
Default is FALSE.}

\item{BB_SIZE}{If TRUE, estimate BB size,
and then use it for
adjusting MME SIZE. Use the adjusted MME size
for bayNorm. Default is TRUE.}

\item{verbose}{Print out status messages.
Default is TRUE.}

\item{plot.out}{If TRUE, show CV^2 vs Mean
expression plot.
Default is FALSE.}

\item{PRIORS}{(Need to be specified for efficiency
if \code{bayNorm} has already been applied)
A list of estimated prior
parameters obtained from bayNorm. Default is NULL.}

\item{input_params}{(Need to be specified for efficiency
if \code{bayNorm} has already been applied)
A list of input parameters which have been used:
\code{BETA_vec}, \code{Conditions},
\code{UMI_sffl}, \code{Prior_type},
\code{FIX_MU}, \code{BB_SIZE} and \code{GR}.}
}
\value{
A list of objects.
}
\description{
A wrapper function for noisy gene detection from raw data.
his produces synthetic control, performs bayNorm on both real
cell data and synthetic controls and does noisy gene detection.
}
\details{
A wrapper function for noisy gene detection
from raw scRNA-seq data.
}
\examples{
data("EXAMPLE_DATA_list")
noisy_out<-noisy_gene_detection(Data=
EXAMPLE_DATA_list$inputdata[,seq(1,30)],BETA_vec
=EXAMPLE_DATA_list$inputbeta[seq(1,30)], mode_version = FALSE,
mean_version=FALSE,
S = 20,parallel = FALSE, NCores = 5,
FIX_MU = TRUE, GR = FALSE,
PRIORS=NULL,
BB_SIZE = TRUE,
verbose = TRUE, plot.out = TRUE)

}
