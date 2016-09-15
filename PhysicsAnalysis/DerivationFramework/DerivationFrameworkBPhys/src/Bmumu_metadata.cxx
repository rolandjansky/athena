/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Bmumu_metadata.cxx, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Store JO metadata in the output file.
//
// Store JO metadata specific to the Bmumu analysis in the output file.
// This class inherits from BPhysMetadataBase.
//
//============================================================================
//

#include "DerivationFrameworkBPhys/Bmumu_metadata.h"

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  Bmumu_metadata::Bmumu_metadata(const std::string& t,
				 const std::string& n,
				 const IInterface*  p) : 
    AthAlgTool(t,n,p), BPhysMetadataBase(t,n,p) {

    // configuration defaults etc.
    recordPropertyI("verbose", 0);
    recordPropertyB("isSimulation", false);

    // MC dataset number lists
    recordPropertyVI("mcBmumu"         , {});
    recordPropertyVI("mcBplusJpsiKplus", {});
    recordPropertyVI("mcBsJpsiPhi"     , {});

    // blind search
    recordPropertyB("doBmumuBlinding", true);

    // wide mumu mass range
    recordPropertyB("doUseWideMuMuMassRange", false);

    // use mass calculated using the combined muon track information in cuts?
    recordPropertyB("useMuCalcMass",    true);
    
    // Global mass values (in MeV, from PDG 2015)
    recordPropertyD("GlobalMuonMass" ,  105.6584);
    recordPropertyD("GlobalKaonMass" ,  493.677 );
    recordPropertyD("GlobalJpsiMass" , 3096.92  );
    recordPropertyD("GlobalBplusMass", 5279.29 );
    recordPropertyD("GlobalB0Mass"   , 5279.61 );
    recordPropertyD("GlobalBsMass"   , 5366.79 );

    // mass ranges
    recordPropertyD("GlobalBMassUpperCut"     , 7000.);
    recordPropertyD("GlobalBMassLowerCut"     , 3500.);
    recordPropertyD("GlobalDiMuonMassUpperCut", 7000.);
    recordPropertyD("GlobalDiMuonMassLowerCut", 2000.);
    recordPropertyD("GlobalJpsiMassUpperCut"  , 7000.);
    recordPropertyD("GlobalJpsiMassLowerCut"  , 2000.);
    recordPropertyD("GlobalBlindUpperCut"     , 5166.);
    recordPropertyD("GlobalBlindLowerCut"     , 5526.);
    
    // Global chi2 cut for vertexing
    recordPropertyD("GlobalChi2Cut", 15.0);

    // Cut values for kaon candidates
    recordPropertyD("GlobalKaonPtCut" , 1000.); // MeV
    recordPropertyD("GlobalKaonEtaCut",  2.5 );

    // MCP cuts for JpsiFinder
    recordPropertyB("useJpsiFinderMCPCuts", false);

    // reject muons in JpsiPlus1Track or JpsiPlus2Track finders
    recordPropertyS("GlobalMuonsUsedInJpsi", "NONE"); // turn off by default

    // run number
    recordPropertyI("runNumber", -1);

    // MC channel number
    recordPropertyI("mcChNumber", -1);
    
    // channels to be processed
    recordPropertyVS("doChannels", {});

    // vertex types to be done
    recordPropertyI("doVertexType", 7);

    // selection expression
    recordPropertyS("SelExpression", "");
    
    // MC truth decay parents
    recordPropertyVI("TruthDecayParents", {});
    
  }
  //--------------------------------------------------------------------------
} // namespace
