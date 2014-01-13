/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SCALEPILEUP_H
#define ANP_SCALEPILEUP_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : ScalePileup
 * @Author : Aidan Randle-Conde, Elliot Lipeles
 *
 * @Brief  : Algorithm for scaling event weight by pileup conditions
 *           - use true MC pileup informartion
 *           - use number of vertexes per event
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/ScaleABC.h"

// ROOT
#include <TFile.h>
#include <TTree.h>

namespace Anp
{
  class ScalePileup: public virtual AlgEvent, public virtual ScaleABC {
  public:

    ScalePileup();
    virtual ~ScalePileup();

    void Config(const Registry &reg);

    bool Init();

    void Exec();

    void Done();

    long double GetScaleCorr()   const { return 0.0;    }
    long double GetPredLumi()    const { return 0.0;    }
    long double GetScalePileup() const { return fScale; }

  private:    

    long double ComputeCorrFromMC(const TrigMonEvent &event) const;
    long double ComputeCorrFromVx(const TrigMonEvent &event) ;

  private:

    Branch<TrigMonEvent> fEntry;            // Trigger events
			 
    // Properties:	 
    bool        fDebug;            // Print debugging info
    bool        fUseTruth;         // Default is to not use truth
    bool        fIsValid;          // Algorithm has valid input parameters
     	        
    double      fPredPileup;       // Pile-up for prediction sample
    long double fScale;            // Scale factor calculated for this event
    float       fMinProbCut;       // Cut on probability of NV given mu_in
    float       fVtxEffA;          // Efficiency of reconstructing vertex (a mu + b mu^2)
    float       fVtxEffB;          // Efficiency of reconstructing vertex (a mu + b mu^2)
    float       fPowerOffset;      // The power to raise mu to (ie (mu1/mu0)^(NV-offset))
    
    // Stored weights
    std::string fWeightNtupleName;
    TFile*      fFileWeights;      //
    TTree*      fTreeWeights;      //
    int         fRunNumber;        //
    int         fEventNumber;      //
    int         fLumiblock;        //
    int         fNV;               //
    float       fMuIn;             //
    float       fTotalScale;       //

    bool        fNullDenomMsg;     // true if null denominator error message issued
    int         fNNullDenom;       // Number of null denominator events
  };
}

#endif
