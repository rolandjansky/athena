/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TPHOTONEFFICIENCYCORRECTIONTOOL__
#define __TPHOTONEFFICIENCYCORRECTIONTOOL__

/**
   @class TPhotonEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in pure ROOT, inherit the structure from TElectronEfficiencyCorrection tool
   For each photon type declare a tool and use addFileName to add root files containing scale factors for this specific type. 
   After adding root files call initialize()
   In the event loop use calculate(const PATCore::ParticleDataType::DataType dataType, const unsigned int runnumber, const double cluster_eta, const double et)
   This returns a TResult. The scale factor and its uncertainty are obtained by calling getScaleFactor() or getTotalUncertainty (See header file of TResult in package PATCore)
   For a short documentation see the included README file

   @author Karsten Koeneke, Felix Buehrer,  Michael Pitt
   @date   January 2014
*/


#include <sstream>

// STL includes
#include <vector>
#include <string>

// ROOT includes
#include <TString.h>
#include "TKey.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

// Include the return object and the base class
#include "PATCore/TResult.h"
#include "PATCore/TCalculatorToolBase.h"
#include "PATCore/PATCoreEnums.h"
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"

namespace Root {
    class TPhotonEfficiencyCorrectionTool : public Root::TElectronEfficiencyCorrectionTool{
  
    // Public methods
    public:
	  double GetIsoSyst(bool isConv, double eta, int isolationcut);  

    };
   
} // End: namespace Root

#endif

