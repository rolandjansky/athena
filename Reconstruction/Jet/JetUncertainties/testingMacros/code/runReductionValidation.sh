#!/bin/bash

runOne () \
{
  if test $# -ne 3 ; then
    echo -e "ERROR: Requires three arguments"
    exit 1
  fi
  
  mkdir Reduction_Plots/$3

  ./runCompare.sh $1 $2
  mv Correlation_Plots/*.p* Reduction_Plots/$3/.

  ./runReducedParamPlots.sh $2
  mv *Reduction*.p* Reduction_Plots/$3/.
}

runOne AllNuisanceParameters.config 12NP.config AllNuisanceParameters-GlobalReduction
runOne AllNuisanceParameters.config 17NP_ByCategory.config AllNuisanceParameters-CategoryReduction

runOne StrongerCorrelations.config StrongerCorrelations_12NP.config StrongerCorrelations-GlobalReduction
runOne StrongerCorrelations.config StrongerCorrelations_17NP_ByCategory.config StrongerCorrelations-CategoryReduction

runOne WeakerCorrelations.config WeakerCorrelations_12NP.config WeakerCorrelations-GlobalReduction
runOne WeakerCorrelations.config WeakerCorrelations_18NP_ByCategory.config WeakerCorrelations-CategoryReduction


