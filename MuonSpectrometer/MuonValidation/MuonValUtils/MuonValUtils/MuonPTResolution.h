/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   T.A.Mueller (LMU) thomas(at)cern.ch
// Feb. 2007
//
// DESCRIPTION:
// Subject: These functions allow a common agreed way to define the muon pT-Resolution
///////////////////////////////////////////////////////////////////////////


#include "TF1.h"
#include "TH1F.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
using namespace std;

double	getContentInRange(TH1F *hist, double centralX, double halfWidth);

// Uses n_max_runs = 3 and does not give the alternative HalfWidth for non-gaussian
//distributions
void	getMuonPTResolution(	TH1F *hist,
				double &PTResMean,
				double &ErrMean,
				double &PTSigma,
				double &ErrSigma,
				double &TailFraction);

// Gives Number of Entries in Range centralX +/- halfWidth
// Fits a gaussian to the given histogramm, then discards all entries more than 2 sigmas
// away from PTResMean. Iterates n_max_runs times. Sigma of the last fitted gaussian is
// returned as &PTSigma. Also goes from PTResMean and increases HalfWidth until
// PerCentageOfEntries % are within the range PTResMean +/- Halfwidth. This can be used as
// an alternative method to calculate the width of a non-gaussian pt-distribution
void	getMuonPTResolution(	TH1F *hist,
				int n_max_runs,
				double &PTResMean,
				double &ErrMean,
				double &PTSigma,
				double &ErrSigma,
				double &TailFraction,
				double PercentageOfEntriesInWidth,
				double &HalfWidth);
