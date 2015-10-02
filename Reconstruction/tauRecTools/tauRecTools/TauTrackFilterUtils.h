/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauTrackFilterUtils.h
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-13
//
//
//-----------------------------------------------------------------------------

#ifndef TAUREC_TAUTRACKFILTERUTILS_H
#define	TAUREC_TAUTRACKFILTERUTILS_H

#include "TF1.h"
#include "TLorentzVector.h"
#include <vector>
#include <iostream>
#include <string>

namespace TauTrackFilterUtils {

    struct TrackInfo{
        TLorentzVector p4;
        int            charge;
        int            index;
    };

    struct TrackPair{
        float angle;
        float mass;
        int charge;
    };

    bool     pass3prong(std::vector<TauTrackFilterUtils::TrackInfo> combination,TLorentzVector tau);
    bool     pass2prong(std::vector<TauTrackFilterUtils::TrackInfo> pair,TLorentzVector tau);
    bool     pass1prong(TLorentzVector track,TLorentzVector tau);
    float    ComputePi0Cone(int recProngs,TLorentzVector tau);
    float    ComputeAngle(float p, float eta, float a[9][4], int npar, int npol, char eqn[] = (char*)"[0]*exp([1]*x)+[2]+[3]/(x*x)");
    float    Compute1dim(float p, float a[10], int npar, char eqn[]);

}

#endif
