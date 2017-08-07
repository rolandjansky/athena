/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 *  @file   Bmumu_metadata.cxx
 *  @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 */

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
    recordPropertyS("projectTag", "__NONE__");
    recordPropertyB("isRelease21" , true);
    
    // MC dataset number lists
    recordPropertyVI("mcBsmumu"         , {});
    recordPropertyVI("mcBplusJpsiKplus" , {});
    recordPropertyVI("mcBsJpsiPhi"      , {});
    recordPropertyVI("mcBplusJpsiPiplus", {});
    recordPropertyVI("mcBhh"            , {});

    // MC datasets without trigger information
    recordPropertyVI("mcNoTrigger"     , {});
    
    // blind search
    recordPropertyB("doBmumuBlinding", true);

    // include trigger
    recordPropertyB("doTriggerInfo"  , true);

    // wide mumu mass range
    recordPropertyB("doUseWideMuMuMassRange", false);

    // use mass calculated using the combined muon track information in cuts?
    recordPropertyB("useMuCalcMass",    true);

    // use calibrated muons instead of the original ones
    recordPropertyI("useCalibratedMuons", 0);

    // adjust primary track to muon kinematics for MUCALC mass
    recordPropertyB("adjustMucalcKinematics", false);

    // add MUCALC mass from non-modified muons for debugging
    recordPropertyB("addMucalcMassForDebug", false);

    // primary vertex types to consider for MinChi2ToAnyPV
    recordPropertyVI("MinChi2ToAnyPVTypes", {1, 3});
    
    // JpsiFinder: muAndMu or TrackAndTrack option?
    recordPropertyB("JfTwoMuons" , true );
    recordPropertyB("JfTwoTracks", false);

    // JpsiFinder: TrackThresholdPt
    recordPropertyD("JfTrackThresholdPt", 0.);
    
    // muon calibration and smearing tool configuration
    recordPropertyS("McstYear"                 , "Data16");
    recordPropertyS("McstRelease"              , "_NONE_");
    recordPropertyB("McstStatComb"             , true);
    recordPropertyB("McstSagittaCorr"          , true);
    recordPropertyS("McstSagittaRelease"       , "_NONE_");
    recordPropertyB("McstDoSagittaMCDistortion", false);
    
    // muon collections
    recordPropertyS("MuonCollection"     , "Muons");
    recordPropertyS("CalMuonCollection"  , "Muons");
    recordPropertyS("UsedMuonCollection" , "Muons");
    recordPropertyVS("AllMuonCollections", {}     );

    // Global mass values (in MeV, from PDG 2015)
    recordPropertyD("GlobalMuonMass" ,  105.6584);
    recordPropertyD("GlobalPionMass" ,  139.57061);
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
    recordPropertyD("GlobalChi2CutBase", 15.0);
    // Different chi2 cuts for 2-, 3- and 4-prong vertices
    recordPropertyD("Chi2Cut2Prong"    , 30.0);
    recordPropertyD("Chi2Cut3Prong"    , 45.0);
    recordPropertyD("Chi2Cut4Prong"    , 60.0);

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
    
    // minimum number of tracks in PV considered for PV association
    recordPropertyI("minNTracksInPV", 0);

    // mode of minLogChi2ToAnyPV calculation
    recordPropertyI("AddMinChi2ToAnyPVMode", 0);
    
    // thinning level
    recordPropertyI("thinLevel", 0);

    // selection expression
    recordPropertyS("SelExpression", "");
    
    // MC truth decay parents
    recordPropertyVI("TruthDecayParents", {});

    // vertex isolation properties
    recordPropertyVS("IsoTrackCategoryName", {});
    recordPropertyVS("IsoTrackCutLevel"    , {});
    recordPropertyVD("IsoTrackPtCut"       , {});
    recordPropertyVD("IsoTrackEtaCut"      , {});
    recordPropertyVI("IsoTrackPixelHits"   , {});
    recordPropertyVI("IsoTrackSCTHits"     , {});
    recordPropertyVI("IsoTrackbLayerHits"  , {});
    recordPropertyVI("IsoTrackIBLHits"     , {});
    recordPropertyVD("IsolationConeSizes"  , {});
    recordPropertyVD("IsoTrkImpLogChi2Max" , {});
    recordPropertyVI("useIsoTrackTypes"    , {});
    recordPropertyB("IsoUseOptimizedAlgo"  ,  true);

    // muon isolation properties (muons of B candidate)
    recordPropertyVS("MuIsoTrackCategoryName", {});
    recordPropertyVS("MuIsoTrackCutLevel"    , {});
    recordPropertyVD("MuIsoTrackPtCut"       , {});
    recordPropertyVD("MuIsoTrackEtaCut"      , {});
    recordPropertyVI("MuIsoTrackPixelHits"   , {});
    recordPropertyVI("MuIsoTrackSCTHits"     , {});
    recordPropertyVI("MuIsoTrackbLayerHits"  , {});
    recordPropertyVI("MuIsoTrackIBLHits"     , {});
    recordPropertyVD("MuIsolationConeSizes"  , {});
    recordPropertyVD("MuIsoTrkImpLogChi2Max" , {});
    recordPropertyVI("useMuIsoTrackTypes"    , {});

    // closest track properties
    recordPropertyVS("CloseTrackCategoryName", {});
    recordPropertyVS("CloseTrackCutLevel"    , {});
    recordPropertyVD("CloseTrackPtCut"       , {});
    recordPropertyVD("CloseTrackEtaCut"      , {});
    recordPropertyVI("CloseTrackPixelHits"   , {});
    recordPropertyVI("CloseTrackSCTHits"     , {});
    recordPropertyVI("CloseTrackbLayerHits"  , {});
    recordPropertyVI("CloseTrackIBLHits"     , {});
    recordPropertyVI("useCloseTrackTypes"    , {});
    recordPropertyB("CloseTrackMinDCAin3D"   , true);
    recordPropertyD("CloseTrackMaxLogChi2"   , 99999.);
    recordPropertyD("NCloseTrackMaxLogChi2"  , 99999.);

    // track-to-vertex association check tool
    recordPropertyI("DebugTrkToVtxMaxEvents" , 0);

    // output containers and branch prefixes
    // (mostly used for isolation tools)
    recordPropertyS("TrkPartContName", "InDetTrackParticles");
    recordPropertyS("PVContName"     , "PrimaryVertices");
    recordPropertyVS("VtxContNames"  , {} );
    recordPropertyVS("RefPVContNames", {} );
    recordPropertyVS("BranchPrefixes", {} );

  }
  //--------------------------------------------------------------------------
} // namespace
