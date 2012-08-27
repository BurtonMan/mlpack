/**
 * @file gaussian_distribution.cpp
 * @author Ryan Curtin
 *
 * Implementation of Gaussian distribution class.
 */
#include "gaussian_distribution.hpp"

using namespace mlpack;
using namespace mlpack::distribution;

arma::vec GaussianDistribution::Random() const
{
  // Should we store chol(covariance) for easier calculation later?
  return trans(chol(covariance)) * arma::randn<arma::vec>(mean.n_elem) + mean;
}

/**
 * Estimate the Gaussian distribution directly from the given observations.
 *
 * @param observations List of observations.
 */
void GaussianDistribution::Estimate(const arma::mat& observations)
{
  // Calculate the mean and covariance with each point.  Because this is a
  // std::vector and not a matrix, this is a little more difficult.
  if (observations.n_cols > 0)
  {
    mean.zeros(observations.n_rows);
    covariance.zeros(observations.n_rows, observations.n_rows);
  }
  else // This will end up just being empty.
  {
    mean.zeros(0);
    covariance.zeros(0);
  }

  // Calculate the mean.
  for (size_t i = 0; i < observations.n_cols; i++)
    mean += observations.col(i);

  // Normalize the mean.
  mean /= observations.n_cols;

  // Now calculate the covariance.
  for (size_t i = 0; i < observations.n_cols; i++)
  {
    arma::vec obsNoMean = observations.col(i) - mean;
    covariance += obsNoMean * trans(obsNoMean);
  }

  // Finish estimating the covariance by normalizing, with the (1 / (n - 1)) so
  // that it is the unbiased estimator.
  covariance /= (observations.n_cols - 1);
}

/**
 * Estimate the Gaussian distribution from the given observations, taking into
 * account the probability of each observation actually being from this
 * distribution.
 */
void GaussianDistribution::Estimate(const arma::mat& observations,
                                    const arma::vec& probabilities)
{
  if (observations.n_cols > 0)
  {
    mean.zeros(observations.n_rows);
    covariance.zeros(observations.n_rows, observations.n_rows);
  }
  else // This will end up just being empty.
  {
    mean.zeros(0);
    covariance.zeros(0);
  }

  double sumProb = 0;

  // First calculate the mean, and save the sum of all the probabilities for
  // later normalization.
  for (size_t i = 0; i < observations.n_cols; i++)
  {
    mean += probabilities[i] * observations.col(i);
    sumProb += probabilities[i];
  }

  // Normalize.
  mean /= sumProb;

  // Now find the covariance.
  for (size_t i = 0; i < observations.n_cols; i++)
  {
    arma::vec obsNoMean = observations.col(i) - mean;
    covariance += probabilities[i] * (obsNoMean * trans(obsNoMean));
  }

  // This is probably biased, but I don't know how to unbias it.
  covariance /= sumProb;

}

/**
 * Returns a string representation of this object.
 */
std::string GaussianDistribution::ToString() const
{
  std::ostringstream convert;
  convert << "GaussianDistribution: " << this << std::endl;
  convert << "mean: " << std::endl << mean << std::endl;
  convert << "covariance: " << std::endl << covariance << std::endl;
  return convert.str();
}
