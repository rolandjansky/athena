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

// STL includes
#include <vector>
#include <string>
// Include the return object and the base class
#include "PATCore/PATCoreEnums.h"
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"

namespace Root {
    class TPhotonEfficiencyCorrectionTool : public Root::TElectronEfficiencyCorrectionTool{
    
    public:
        struct Result{
            double scaleFactor=-999.0;
            double totalUncertainty=-999.0;
        };

        TPhotonEfficiencyCorrectionTool(const char* name="TPhotonEfficiencyCorrectionTool");
        ~TPhotonEfficiencyCorrectionTool();
        int initialize();
        // Additional public methods
        const Result calculate( const PATCore::ParticleDataType::DataType dataType,
                const unsigned int runnumber,
                const double cluster_eta,
                const double et /* in MeV */
                );

    private :
    };

} // End: namespace Root

#endif

