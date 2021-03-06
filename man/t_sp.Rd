% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{t_sp}
\alias{t_sp}
\title{Transpose of sparse matrix}
\usage{
t_sp(Data)
}
\arguments{
\item{Data}{A matrix of single-cell expression where rows
are genes and columns are samples (cells). \code{Data}
can be of class \code{SummarizedExperiment} (the
assays slot contains the expression matrix and
is named "Counts") or just matrix.}
}
\value{
Transpose of sparse matrix.
}
\description{
Transpose of sparse matrix
}
\details{
Transpose of sparse matrix.
}
\examples{
data("EXAMPLE_DATA_list")
#Should not run by the users, it is used in prior estimation.
\dontrun{
}
}
