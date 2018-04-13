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



/* C.A 
 * Comments for possible migration away from TResult. 
 * This tool as mentioned in the comment above WAS inheriting publicly from the TElectronEfficiencyCorrectionTool.
 * Therefore, in pracices it WAS inheriting both the TCalculatorToolBase facilities/interface and the
 * TElectronEfficiencyCorrectionTool Implementation. This kind of header inheritance from different places
 * made this a tad bit convoluted to catch.
 *
 * The main issue is  due to AsgPhotonEfficiencyCorrectionTool having public
 * methods returning a TResult. The IAsgPhotonEfficiencyCorrectionTool does not advertise these methods but this
 * was enough to make me wonder . As I could imagine clients could exist? 
 * In general not sure why we have these methods around
 * in the "Asg" part at they do not follow 100% the Electron notation.
 * 
 * Anyhow, the TElectronEfficiencyCorrectionTool still provides the actual implementation.
 * One can not really use the TElectronEfficiencyTool polymorphically (does not seem to be done anywhere).
 * After its own cleanup away from TResult it does not have common variables with the TCalculatorToolBase.
 *
 * Needed to do the ctor/dtor and initialize / finalize (both bases have these)
 * And a calculate wraping the new one from TElectronEfficiencyCorrectionTool 
 * following the older convention i.e return TResult ref.
 *
 * Hopefully, someone with a more recent view could make a more informed decison  
 * on how to avoid the "mutable" from TSelectorToolBase also for the Photon tools. Migrate this package. 
 * My impression is that we could move away but need feedback.
 *
 * When done resolving the above please remove this comment !!!
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
    class TPhotonEfficiencyCorrectionTool : public  Root::TCalculatorToolBase, 
    public Root::TElectronEfficiencyCorrectionTool{
    
    public:
        TPhotonEfficiencyCorrectionTool(const char* name="TPhotonEfficiencyCorrectionTool");
        ~TPhotonEfficiencyCorrectionTool();
        int initialize();
        int finalize(); 
        // Additional public methods
        const Root::TResult& calculate( const PATCore::ParticleDataType::DataType dataType,
                const unsigned int runnumber,
                const double cluster_eta,
                const double et /* in MeV */
                );

    private :
    };

} // End: namespace Root

#endif

