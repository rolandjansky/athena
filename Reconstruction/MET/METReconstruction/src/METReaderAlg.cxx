/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METReaderAlg.cxx

#include "METReaderAlg.h"
#include "xAODJet/JetContainer.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"

using std::string;
using xAOD::MissingETContainer;
using xAOD::MissingETComponentMap;

namespace met {

  //**********************************************************************

  METReaderAlg::METReaderAlg(const std::string& name,
			     ISvcLocator* pSvcLocator )
    : ::AthReentrantAlgorithm( name, pSvcLocator ) {
    declareProperty ("MET_RefFinal", m_met_RefFinal = "MET_RefFinal");
    declareProperty ("MET_LocHadTopo", m_met_LocHadTopo = "MET_LocHadTopo");
    declareProperty ("MET_Track", m_met_Track = "MET_Track");
    declareProperty ("MET_PFlow", m_met_PFlow = "MET_PFlow");
    declareProperty ("MET_Truth", m_met_Truth = "MET_Truth");
    declareProperty ("MET_TruthRegions", m_met_TruthReg = "MET_TruthRegions");
    declareProperty ("METMap_RefFinal", m_metMap_RefFinal = "METMap_RefFinal");
    declareProperty ("METMap_LocHadTopo", m_metMap_LocHadTopo = "METMap_LocHadTopo");


  }

  //**********************************************************************

  METReaderAlg::~METReaderAlg() { }

  //**********************************************************************

  StatusCode METReaderAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_CHECK( m_met_RefFinal.initialize() );
    ATH_CHECK( m_met_LocHadTopo.initialize() );
    ATH_CHECK( m_met_Track.initialize() );
    ATH_CHECK( m_met_PFlow.initialize() );
    ATH_CHECK( m_met_Truth.initialize() );
    ATH_CHECK( m_met_TruthReg.initialize() );
    ATH_CHECK( m_metMap_RefFinal.initialize() );
    ATH_CHECK( m_metMap_LocHadTopo.initialize() );

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METReaderAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METReaderAlg::execute_r (const EventContext& ctx) const{ 
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Loop over tools.

    ATH_MSG_INFO( "Check MET rebuilding" );
    SG::ReadHandle<xAOD::MissingETContainer> met_RefFinal (m_met_RefFinal, ctx);
    SG::ReadHandle<xAOD::MissingETContainer> met_LocHadTopo (m_met_LocHadTopo, ctx);
    SG::ReadHandle<xAOD::MissingETContainer> met_Track (m_met_Track, ctx);
    SG::ReadHandle<xAOD::MissingETContainer> met_PFlow (m_met_PFlow, ctx);
    SG::ReadHandle<xAOD::MissingETContainer> met_Truth (m_met_Truth, ctx);



    bool doRefFinal( met_RefFinal.isValid() );
    bool doLHT( met_LocHadTopo.isValid() );
    bool doTrack( met_Track.isValid() );
    bool doPFlow( met_PFlow.isValid() );
    bool doTruth( met_Truth.isValid() );

    ATH_MSG_INFO( "  MET magnitude:" );
    //
    if(doRefFinal) {
      ATH_MSG_INFO( "    MET_Final = " << (*met_RefFinal)["Final"]->met() );
      ATH_MSG_INFO( "    MET_RefEle = " << (*met_RefFinal)["RefEle"]->met() );
      ATH_MSG_INFO( "    MET_RefGamma = " << (*met_RefFinal)["RefGamma"]->met() );
      ATH_MSG_INFO( "    MET_RefTau = " << (*met_RefFinal)["RefTau"]->met() );
      ATH_MSG_INFO( "    MET_RefJet = " << (*met_RefFinal)["RefJet"]->met() );
      ATH_MSG_INFO( "    MET_RefJet_JVFCut = " << (*met_RefFinal)["RefJet_JVFCut"]->met() );
      ATH_MSG_INFO( "    MET_Muons = " << (*met_RefFinal)["Muons"]->met() );
      ATH_MSG_INFO( "    MET_SoftClus = " << (*met_RefFinal)["SoftClus"]->met() );
      ATH_MSG_INFO( "    MET_PVSoftTrk = " << (*met_RefFinal)["PVSoftTrk"]->met() );
    }
    //
    if(doLHT) {
      ATH_MSG_INFO( "    MET_LocHadTopo = " << (*met_LocHadTopo)["LocHadTopo"]->met() );
    }
    if(doTrack) {
      ATH_MSG_INFO( "    MET_Track = " << (*met_Track)["Track"]->met() );
      ATH_MSG_INFO( "    MET_TrackPV = " << (*met_Track)["PVTrack"]->met() );
    }
    if(doPFlow) {
      ATH_MSG_INFO( "    MET_PFlow = " << (*met_PFlow)["PFlow"]->met() );
    }
    //
    if(doRefFinal) {
      ATH_MSG_INFO( "    MET_Truth_NonInt = " << (*met_Truth)["NonInt"]->met() );
      ATH_MSG_INFO( "    MET_Truth_Int = " << (*met_Truth)["Int"]->met() );
      ATH_MSG_INFO( "    MET_Truth_IntOut = " << (*met_Truth)["IntOut"]->met() );
      ATH_MSG_INFO( "    MET_Truth_IntMuons = " << (*met_Truth)["IntMuons"]->met() );
    }
    ATH_MSG_INFO( "  MET x-component:" );
    //
    if(doRefFinal) {
      ATH_MSG_INFO( "    MET_Final_x = " << (*met_RefFinal)["Final"]->mpx() );
      ATH_MSG_INFO( "    MET_RefEle_x = " << (*met_RefFinal)["RefEle"]->mpx() );
      ATH_MSG_INFO( "    MET_RefGamma_x = " << (*met_RefFinal)["RefGamma"]->mpx() );
      ATH_MSG_INFO( "    MET_RefTau_x = " << (*met_RefFinal)["RefTau"]->mpx() );
      ATH_MSG_INFO( "    MET_RefJet_x = " << (*met_RefFinal)["RefJet"]->mpx() );
      ATH_MSG_INFO( "    MET_RefJet_JVFCut_x = " << (*met_RefFinal)["RefJet_JVFCut"]->mpx() );
      ATH_MSG_INFO( "    MET_Muons_x = " << (*met_RefFinal)["Muons"]->mpx() );
      ATH_MSG_INFO( "    MET_SoftClus_x = " << (*met_RefFinal)["SoftClus"]->mpx() );
      ATH_MSG_INFO( "    MET_PVSoftTrk_x = " << (*met_RefFinal)["PVSoftTrk"]->mpx() );
    }
    //
    if(doLHT) {
      ATH_MSG_INFO( "    MET_LocHadTopo_x = " << (*met_LocHadTopo)["LocHadTopo"]->mpx() );
    }
    if(doTrack) {
      ATH_MSG_INFO( "    MET_Track_x = " << (*met_Track)["Track"]->mpx() );
      ATH_MSG_INFO( "    MET_TrackPV_x = " << (*met_Track)["PVTrack"]->mpx() );
    }
    if(doPFlow) {
      ATH_MSG_INFO( "    MET_PFlow_x = " << (*met_PFlow)["PFlow"]->mpx() );
    }
    //
    if(doTruth) {
      ATH_MSG_INFO( "    MET_Truth_NonInt_x = " << (*met_Truth)["NonInt"]->mpx() );
      ATH_MSG_INFO( "    MET_Truth_Int_x = " << (*met_Truth)["Int"]->mpx() );
      ATH_MSG_INFO( "    MET_Truth_IntOut_x = " << (*met_Truth)["IntOut"]->mpx() );
      ATH_MSG_INFO( "    MET_Truth_IntMuons_x = " << (*met_Truth)["IntMuons"]->mpx() );
    }
    ATH_MSG_INFO( "  MET y-component:" );
    //
    if(doRefFinal) {
      ATH_MSG_INFO( "    MET_Final_y = " << (*met_RefFinal)["Final"]->mpy() );
      ATH_MSG_INFO( "    MET_RefEle_y = " << (*met_RefFinal)["RefEle"]->mpy() );
      ATH_MSG_INFO( "    MET_RefGamma_y = " << (*met_RefFinal)["RefGamma"]->mpy() );
      ATH_MSG_INFO( "    MET_RefTau_y = " << (*met_RefFinal)["RefTau"]->mpy() );
      ATH_MSG_INFO( "    MET_RefJet_y = " << (*met_RefFinal)["RefJet"]->mpy() );
      ATH_MSG_INFO( "    MET_RefJet_JVFCut_y = " << (*met_RefFinal)["RefJet_JVFCut"]->mpy() );
      ATH_MSG_INFO( "    MET_Muons_y = " << (*met_RefFinal)["Muons"]->mpy() );
      ATH_MSG_INFO( "    MET_SoftClus_y = " << (*met_RefFinal)["SoftClus"]->mpy() );
      ATH_MSG_INFO( "    MET_PVSoftTrk_y = " << (*met_RefFinal)["PVSoftTrk"]->mpy() );
    }
    //
    if(doLHT) {
      ATH_MSG_INFO( "    MET_LocHadTopo_y = " << (*met_LocHadTopo)["LocHadTopo"]->mpy() );
    }
    if(doTrack) {
      ATH_MSG_INFO( "    MET_Track_y = " << (*met_Track)["Track"]->mpy() );
      ATH_MSG_INFO( "    MET_TrackPV_y = " << (*met_Track)["PVTrack"]->mpy() );
    }
    if(doPFlow) {
      ATH_MSG_INFO( "    MET_PFlow_y = " << (*met_PFlow)["PFlow"]->mpy() );
    }
    //
    if(doTruth) {
      ATH_MSG_INFO( "    MET_Truth_NonInt_y = " << (*met_Truth)["NonInt"]->mpy() );
      ATH_MSG_INFO( "    MET_Truth_Int_y = " << (*met_Truth)["Int"]->mpy() );
      ATH_MSG_INFO( "    MET_Truth_IntOut_y = " << (*met_Truth)["IntOut"]->mpy() );
      ATH_MSG_INFO( "    MET_Truth_IntMuons_y = " << (*met_Truth)["IntMuons"]->mpy() );
    }

    if(doTruth) {
      SG::ReadHandle<xAOD::MissingETContainer> met_TruthReg (m_met_TruthReg, ctx);

      if( met_TruthReg.isValid() ) {
	ATH_MSG_INFO( "    MET_Truth_IntCentral_y = " << (*met_TruthReg)["Int_Central"]->mpy() );
	ATH_MSG_INFO( "    MET_Truth_IntEndCap_y = " << (*met_TruthReg)["Int_EndCap"]->mpy() );
	ATH_MSG_INFO( "    MET_Truth_IntForward_y = " << (*met_TruthReg)["Int_Forward"]->mpy() );
      }
    }
    SG::ReadHandle<xAOD::MissingETComponentMap> metMap_RefFinal (m_metMap_RefFinal, ctx);
    SG::ReadHandle<xAOD::MissingETComponentMap> metMap_LocHadTopo (m_metMap_LocHadTopo, ctx);

    ATH_CHECK( metMap_RefFinal.isValid() );
    ATH_CHECK( metMap_LocHadTopo.isValid() );

    ATH_MSG_INFO( "Number of components in MET map: " << metMap_RefFinal->size() );
    ATH_MSG_INFO( "Number of components in MET map: " << metMap_LocHadTopo->size() );

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

}

