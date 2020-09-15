# CovarianceTool userguide


Table of Contents:
- [I want to construct a covariance matrix from a Yoda file with uncertainty breakdown](#i-want-to-construct-a-covariance-matrix-from-a-yoda-file-with-uncertainty-breakdown)
- [I want to manipulate a yoda file with uncertainty breakdown and make a covariance matrix](#i-want-to-manipulate-a-yoda-file-with-uncertainty-breakdown-and-make-a-covariance-matrix)
	- [I want to inspect the error breakdown for each analysis object in a yoda file](#i-want-to-inspect-the-error-breakdown-for-each-analysis-object-in-a-yoda-file)
	- [I want to comapore compare data and MC yoda files accoutinng for the covariance between bins](#i-want-to-comapore-compare-data-and-mc-yoda-files-accoutinng-for-the-covariance-between-bins)
		- [I only want to consider a subset of the analysis objects](#i-only-want-to-consider-a-subset-of-the-analysis-objects)
	- [I want to determine the global agreement between data and MC across several distributions, accounting for the correlations between their bins](#i-want-to-determine-the-global-agreement-between-data-and-mc-across-several-distributions-accounting-for-the-correlations-between-their-bins)
<br/>




# I want to construct a covariance matrix from a Yoda file with uncertainty breakdown

  The `covarianceTool.py` script is designed exactly for this purpose. It comes loaded with several useful functions for manipulating `yoda` files which are markeup up with uncertainty breakdowns. 

# I want to manipulate a yoda file with uncertainty breakdown and make a covariance matrix

  A very simple typical example would be in a python macro:
  ```
#!/usr/bin/env python

  import yoda #to manipulate the RIVET output
  import covarianceToolsLibrary as ct # This gives access to the covarianceToolsLibrary functions

  hists=yoda.read('example.yoda')
  h=hists['/EXAMPLE']
  h2=hists['/EXAMPLENOCORRINFO']

#access the uncertainty breakdown 
binErrors=ct.getCorrInfo(h)

# produce the covariance matrix from direct propagation (as a numpy.array())
# if errors are asymmetric, they are symmetrized
covMatrix=ct.makeCovarianceMatrix(h)

# For hdata and hmc two AOs with covariance info included...
# get the chi2 from two analysis objects with stored uncertainty breakdown
# by default, the cov matrices are produced using symmetrized uncertainties,
# using the direct propagation method
# the result is a list of objects :
# [chi2 value, ndof, p-value, matrix of chi2 contributions from each pair of bins]
chi2results=ct.calculateChi2(hdata,hmc)
  ```

## I want to inspect the error breakdown for each analysis object in a yoda file 

  This can be achieved using the `covarianceTool.py`:
  ```
  covarianceTool.py --data ATLAS_2017_I1514251_EL.yoda
  ```

  This command takes the input data yoda file, and checks whether the uncertainty breakdown is present as a 'Corr' annotation in the yoda.
  If so, it will produce a validation plot for each histogram showing the effect of any variation which changes any bin by more than 5%.
  If successful, the script will then produce the validation plots for each distributions e.g.:
  ```
  outputs/ATLAS_2017_I1514251_EL_corrs/d01-x01-y02/validation-plots//data-variations.pdf
  ```

## I want to comapore compare data and MC yoda files accoutinng for the covariance between bins

  If all the uncertainty breakdown information has been provided, it is then possible to do the full Data/MC comparison. E.g.:
  ```
  covarianceTool.py --mc sherpa221_corrs.yoda  --data ATLAS_2017_I1514251_EL_corrs.yoda
  ```

  For both the data and the MC, the covariance is produced from the breakdown of uncertainties in the 'Corr' annotation of each histogram.
  The script will run through each distribution where it was possible to establish a 1-1 match between the Data and MC naming convention.
  For each distribution, the following outputs are produced:
  ```
# a txt summary containing the Data, MC, and total covariance matrices, as well as the correaltion matrix, the chi-square
  outputs/ATLAS_2017_I1514251_EL_corrs/sherpa221_corrs.renamed/d01-x01-y02/summary.txt
# a visualisation of the total covariance matrix, correlation matrix and the chi2 contributions in each bin
  outputs/ATLAS_2017_I1514251_EL_corrs/sherpa221_corrs.renamed/d01-x01-y02/validation-plots/chi2-contribs.pdf
  outputs/ATLAS_2017_I1514251_EL_corrs/sherpa221_corrs.renamed/d01-x01-y02/validation-plots/covariance-matrix.pdf
  outputs/ATLAS_2017_I1514251_EL_corrs/sherpa221_corrs.renamed/d01-x01-y02/validation-plots/correlation-matrix.pdf
# and finally the usual Rivet plot, where the Chi2 value has been added to the MC in thge legend
  outputs/ATLAS_2017_I1514251_EL_corrs/sherpa221_corrs.renamed/d01-x01-y02/ATLAS_2017_I1514251_EL_d01-x01-y02.pdf
  ```
  By default, the covariance matrices are produced using the direct propagation method if the errors are symmetrical and using 1000 toys if the errors are asymmetrical.
  One can force the use of either method in all cases with the options `--use_direct` (in which case asymm errors are symmetrized) or `--use_toys` (in which case the number of toys can be controlled using `--ntoys`, default 1000).

  By default the total covariance matrix is the sum of the data and MC covariance matrices. One can choose to only consider either the data or MC covariance matrices using the options `--ignore_errs_data` and `--ignore_errs_mc`.

  When constructing the covariance matrices, any source of error containing the string 'stat' is treated as uncorrelated between bins, and otherwise treated as 100% correlated between bins. As a sanity check, it is possible to treat all sources of uncertainty as uncorrelated, in which case the chi2 should revert to the naive value of Sum_bins (data-mc)^2 /(total err)^2. This can be achieved using ` --ignore_corrs` (or just for data/mc using ` --ignore_corrs_data` / ` --ignore_corrs_mc`). 

### I only want to consider a subset of the analysis objects

  You may not want to consider all the analysis objects in the yoda file. In this case, you can specify which histogram names to use using the options `--filter <comma seprarated list of strings>`. Only analysis objects containing one of the strings in the list will be processed. E.g.:
  ```
  covarianceTool.py --mc sherpa221_corrs.yoda  --data ATLAS_2017_I1514251_EL_corrs.yoda --filter d01-x01-y02,d01-x03-y02,d01-x11-y02,d01-x04-y02
  ```

## I want to determine the global agreement between data and MC across several distributions, accounting for the correlations between their bins 

  It is possible to build 'super-analysis-objects' to calculate the agreement between data and MC over a range of measurements.
  This can be done using the `--make_super_aos` option. 
  In this case, in addition to making the usual comparisons between the analysis objects which pass the filter, an additional analysis object is processed where each bin corresponds to a bin in the individual distributions. A covariance matrix for this super-ao is built, and an overall chi2 calculated in the same way:
  ```
  covarianceTool.py --mc sherpa221_corrs.renamed.yoda  --data ATLAS_2017_I1514251_EL_corrs.yoda --filter d01-x01-y02,d01-x03-y02,d01-x11-y02,d01-x04-y02 --make_super_aos
  ```
  will return the usual comparisons in addition to 
  ```
  ATLAS_2017_I1514251_EL_corrs.yoda:superAO_d01-x01-y02_d01-x03-y02_d01-x04-y02_d01-x11-y02 vs sherpa221_corrs.renamed.renamed.yoda:superAO_d01-x01-y02_d01-x03-y02_d01-x04-y02_d01-x11-y02 --> chi2=73.70/47
  ```
  and the usual plots.




