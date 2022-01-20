/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @file   BdKstarMuMu_metadata.cxx
 *  @author Pavel Reznicek <pavel.reznicek@cern.ch>
 */

#include "DerivationFrameworkBPhys/BdKstarMuMu_metadata.h"

namespace DerivationFramework {

  BdKstarMuMu_metadata::BdKstarMuMu_metadata(const std::string& t,
                                             const std::string& n,
                                             const IInterface*  p):
    AthAlgTool(t,n,p), BPhysMetadataBase(t,n,p) {

      recordPropertyI("verbose"      , 0);          // verbose athena output
      recordPropertyB("isSimulation" , false);      // input data is MC simulation or real data
      recordPropertyS("projectTag"   , "__NONE__"); // copy reconstruction project tag
      recordPropertyB("isRelease21"  , true);       // is processed in release 21
      recordPropertyS("mcCampaign"   , "__NONE__"); // what MC campaign is used
      recordPropertyS("triggerStream", "__NONE__"); // what data stream is analyzed
      recordPropertyI("runNumber"    , -1);         // run number from the input file

      recordPropertyB("looseCuts"   , false); // apply loose cuts (debugging only)
      recordPropertyB("skimTrig"    , false); // skim data by selected triggers
      recordPropertyB("skimData"    , false); // skim data by passed B-candidates
      recordPropertyB("thinData"    , false); // thin ID tracks, muons and PVs
      recordPropertyB("slimData"    , false); // TODO: data slimming
      recordPropertyB("thinMC"      , false); // thin MC-truth (keep wide range of heavy hadrons)
      recordPropertyB("thinMCsignal", false); // thin MC-truth to signal-only (keep only signal PDG b-hadrons)
      recordPropertyB("trigObjects" , false); // store trigger objects for B-physics and muons

      recordPropertyS("version", "v1.0"); // derivation version (update with every update of the derivation)

      /*
        Constants (in sync with the JpsiUpsilonTools)
      */
      recordPropertyD("mass_mu"   ,  105.658); // PDG: 105.6583745
      recordPropertyD("mass_e"    ,    0.511); // PDG:   0.5109989461
      recordPropertyD("mass_K"    ,  493.677); // PDG: 493.677
      recordPropertyD("mass_pi"   ,  139.57 ); // PDG: 139.57039
      recordPropertyD("mass_p"    ,  938.272); // PDG: 938.272081
      recordPropertyD("mass_Jpsi" , 3096.916); // PDG:3096.900
      recordPropertyD("mass_Kstar",  891.66 ); // PDG: 891.66
      recordPropertyD("mass_Bd"   , 5279.65 ); // PDG:5279.65
      recordPropertyD("mass_Bs"   , 5366.88 ); // PDG:5366.88

    } // AthAlgTool
} // namespace
