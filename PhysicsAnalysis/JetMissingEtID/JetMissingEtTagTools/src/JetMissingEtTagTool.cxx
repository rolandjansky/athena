/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : JetMissingEtTagTool.cxx
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Purpose : create a collection of JetMissingEtJetTag
*****************************************************************************/
#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationHelper.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include "TagEvent/MissingETAttributeNames.h"
#include "TagEvent/ParticleJetAttributeNames.h"

// JetCalibTools includes
#include "JetCalibTools/JetCalibrationTool.h"

// JetInterface includes
#include "JetInterface/IJetUpdateJvt.h"
#include "METInterface/IMETMaker.h"

#include "METUtilities/METHelpers.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>
#include <vector>

// define some global/static selectors
// veryLooseBadTool, looseBadTool, etc... are defined here
#include "JetSelectorDefs.h"

/** the constructor */
JetMetTagTool::JetMetTagTool (const std::string& type, const std::string& name,
                                          const IInterface* parent) :
  AthAlgTool( type, name, parent ),
  m_jetCalibrationTool(""), m_jetJVTUpdateTool("") {

  /** JetMissingEt tools */
  declareProperty("JetCalibrationTool",    m_jetCalibrationTool);
  declareProperty("JetVertexTaggerTool",   m_jetJVTUpdateTool);
  /** MET maker tool */
  declareProperty( "METMaker",             m_metmaker);

  /** B-tagging tools */
  declareProperty( "FixedCutBEff60",       m_FixedCutBEff_60);
  declareProperty( "FixedCutBEff70",       m_FixedCutBEff_70);
  declareProperty( "FixedCutBEff85",       m_FixedCutBEff_85);
  //  declareProperty( "FlatBEff60",           m_FlatBEff_60);
  //  declareProperty( "FlatBEff70",           m_FlatBEff_70);
  //  declareProperty( "FlatBEff77",           m_FlatBEff_77);

  /** JetMissingEt AOD Container Name */
  declareProperty("JetContainer",          m_containerName           = "AntiKt4EMTopo");
  declareProperty("JetCalibContainer",     m_jetCalibcontainerName   = "AntiKt4TopoJets_TAGcalib");

  /** selection cut of Pt */
  declareProperty("EtCut",                 m_jetPtCut                = 40.0*CLHEP::GeV);
  declareProperty("EtCutSkim",             m_jetPtCut_skim           = 20.0*CLHEP::GeV);

  declareProperty("UseEMScale",            m_useEMScale              = false);
  declareProperty("isSimulation",          m_isSimulation            = false);

  /** Have to toggle off in cosmics */
  declareProperty("DoJVT",                 m_doJVT                   = true);

  /** MET input with skims */
  declareProperty("JetCalibContainerSkim", m_jetCalibcontainerName_skim = "AntiKt4TopoJets_TAGcalibskim");
  declareProperty("ElectronsContainerSkim",m_ElectronsContainer_skim);
  declareProperty("PhotonsContainerSkim",  m_PhotonsContainer_skim);
  declareProperty("TausContainerSkim",     m_TausContainer_skim);
  declareProperty("MuonsContainerSkim",    m_MuonsContainer_skim);
  declareProperty("METMapName",            m_METMapName);
  declareProperty("METCoreName",           m_METCoreName);

  /** MET with skims */
  declareProperty("METContainer",          m_metContainerName        = "MET_Reference_AntiKt4Topo_TAGcalibskim");
  declareProperty("METFinalName",          m_metRefFinalName         = "FinalClus");
  declareProperty("METJetName",            m_metRefJetName           = "RefJet");
  declareProperty("METMuonsName",          m_metMuonsName            = "Muons");
  declareProperty("METSoftClusName",       m_metSoftClusName         = "SoftClus");
  declareProperty("METRefTauName",         m_metRefTauName           = "RefTau");
  declareProperty("METRefEleName",         m_metRefEleName           = "RefEle");
  declareProperty("METRefGammaName",       m_metRefGammaName         = "RefGamma");
  declareProperty("METPVSoftTrkName",      m_metPVSoftTrkName        = "PVSoftTrk");
  declareProperty("METFinalTrkName",       m_metFinalTrkName         = "FinalTrk");

  declareInterface<JetMetTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  JetMetTagTool::initialize() {
  ATH_MSG_DEBUG(  "in intialize()" );

  // retrieve the jet calibration tool
  CHECK(m_jetCalibrationTool.retrieve());

  // retrieve the JVT update tool
  CHECK(m_jetJVTUpdateTool.retrieve());

  // MET maker tool
  CHECK(m_metmaker.retrieve());

  // B-tagging tools
  CHECK(m_FixedCutBEff_60.retrieve());
  CHECK(m_FixedCutBEff_70.retrieve());
  CHECK(m_FixedCutBEff_85.retrieve());
  //  CHECK(m_FlatBEff_60.retrieve());
  //  CHECK(m_FlatBEff_70.retrieve());
  //  CHECK(m_FlatBEff_77.retrieve());

  // init selectors as defined in JetSelectorDefs.h
  CHECK(initJetSelectors());

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap) {

  ATH_MSG_DEBUG(  "in attributeSpecification() for missingET" );

  /** MissingET Ref_final attributes */
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalX] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefFinalX],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefFinalX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalY] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefFinalY],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefFinalY]);
  attrMap[ MissingEtAttributeNames[EtMiss::SumET] ]          = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::SumET],
										   MissingEtAttributeGroupNames[EtMiss::SumET]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_SoftClusX] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_SoftClusX],
										   MissingEtAttributeGroupNames[EtMiss::MET_SoftClusX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_SoftClusY] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_SoftClusY],
										   MissingEtAttributeGroupNames[EtMiss::MET_SoftClusY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefJetX] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefJetX],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefJetX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefJetY] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefJetY],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefJetY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauX] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefTauX],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefTauX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauY] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefTauY],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefTauY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_MuonsX] ]     = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_MuonsX],
										   MissingEtAttributeGroupNames[EtMiss::MET_MuonsX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_MuonsY] ]     = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_MuonsY],
										   MissingEtAttributeGroupNames[EtMiss::MET_MuonsY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefEleX] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefEleX],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefEleX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefEleY] ]    = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefEleY],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefEleY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefGammaX] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefGammaX],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefGammaX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefGammaY] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_RefGammaY],
										   MissingEtAttributeGroupNames[EtMiss::MET_RefGammaY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_PVSoftTrkX] ] = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_PVSoftTrkX],
										   MissingEtAttributeGroupNames[EtMiss::MET_PVSoftTrkX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_PVSoftTrkY] ] = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_PVSoftTrkY],
										   MissingEtAttributeGroupNames[EtMiss::MET_PVSoftTrkY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_FinalTrkX] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_FinalTrkX],
										   MissingEtAttributeGroupNames[EtMiss::MET_FinalTrkX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_FinalTrkY] ]  = AthenaAttributeType("float",
										   MissingEtAttributeUnitNames[EtMiss::MET_FinalTrkY],
										   MissingEtAttributeGroupNames[EtMiss::MET_FinalTrkY]);
  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                       const int max) {
  ATH_MSG_DEBUG(  "in attributeSpecification() for jet" );

  std::ostringstream os;

  /** Jet Attributes */
  attrMap[ JetAttributeNames[PJet::NJet] ]      = AthenaAttributeType("unsigned int", JetAttributeUnitNames[PJet::NJet],     JetAttributeGroupNames[PJet::NJet]);
  attrMap[ JetAttributeNames[PJet::JetSumET] ]  = AthenaAttributeType("float",        JetAttributeUnitNames[PJet::JetSumET], JetAttributeGroupNames[PJet::JetSumET]);

  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str("");
    os << JetAttributeNames[PJet::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", JetAttributeUnitNames[PJet::Pt], JetAttributeGroupNames[PJet::Pt]);
    m_ptStr.push_back( os.str() );

    /** E */
    os.str("");
    os << JetAttributeNames[PJet::E] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", JetAttributeUnitNames[PJet::E], JetAttributeGroupNames[PJet::E]);
    m_eStr.push_back( os.str() );

    /** eta */
    os.str("");
    os << JetAttributeNames[PJet::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", JetAttributeUnitNames[PJet::Eta], JetAttributeGroupNames[PJet::Eta]);
    m_etaStr.push_back( os.str() );

    /** phi */
    os.str("");
    os << JetAttributeNames[PJet::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", JetAttributeUnitNames[PJet::Phi], JetAttributeGroupNames[PJet::Phi]);
    m_phiStr.push_back( os.str() );

    /** Jet PID  */
    os.str("");
    os << JetAttributeNames[PJet::JetPID] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", JetAttributeUnitNames[PJet::JetPID], JetAttributeGroupNames[PJet::JetPID]);
    m_pidStr.push_back( os.str() );

  }

  attrMap[ JetAttributeNames[PJet::NpTJet] ]      = AthenaAttributeType("unsigned int",  JetAttributeUnitNames[PJet::NpTJet],  JetAttributeGroupNames[PJet::NpTJet]);
  attrMap[ JetAttributeNames[PJet::NpTBJet] ]     = AthenaAttributeType("unsigned int",  JetAttributeUnitNames[PJet::NpTBJet],  JetAttributeGroupNames[PJet::NpTBJet]);

  return StatusCode::SUCCESS;
}

/**Calibrate and record a shallow copy of a given jet container */
const xAOD::JetContainer* JetMetTagTool::calibrateAndRecordShallowCopyJetCollection(const xAOD::JetContainer * jetContainer) {

  // create a shallow copy of the jet container
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* >  shallowCopy = xAOD::shallowCopyContainer(*jetContainer);
  xAOD::JetContainer *jetContainerShallowCopy           = shallowCopy.first;
  xAOD::ShallowAuxContainer *jetAuxContainerShallowCopy = shallowCopy.second;

  if( evtStore()->record(jetContainerShallowCopy, m_jetCalibcontainerName).isFailure() ){
    ATH_MSG_WARNING("Unable to record JetCalibratedContainer: " << m_jetCalibcontainerName);
    return 0;
  }
  if( evtStore()->record(jetAuxContainerShallowCopy, m_jetCalibcontainerName+"Aux.").isFailure() ){
    ATH_MSG_WARNING("Unable to record JetCalibratedAuxContainer: " << m_jetCalibcontainerName+"Aux.");
    return 0;
  }

  static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");
  static SG::AuxElement::Decorator< float > decJvt("Jvt");

  if( m_jetCalibrationTool->applyCalibration(*jetContainerShallowCopy).isFailure() ){
    ATH_MSG_WARNING( "Failed to apply calibration to the jet container");
    return 0;
  }

  for ( xAOD::Jet *shallowCopyJet : * jetContainerShallowCopy ) {
    const xAOD::IParticleLink originLink( *jetContainer, shallowCopyJet->index() );
    accSetOriginLink(*shallowCopyJet) = originLink;

    if(m_doJVT) {
      decJvt(*shallowCopyJet) = m_jetJVTUpdateTool->updateJvt(*shallowCopyJet);
    }
  }

  if( evtStore()->setConst(jetContainerShallowCopy ).isFailure() ){
    ATH_MSG_WARNING( "Failed to set jetcalibCollection (" << m_jetCalibcontainerName+"Aux." << ")const in StoreGate!");
    return 0;
  }
  if( evtStore()->setConst(jetAuxContainerShallowCopy ).isFailure() ){
    ATH_MSG_WARNING( "Failed to set jetcalibCollection (" << m_jetCalibcontainerName+"Aux." << ")const in StoreGate!");
    return 0;
  }

  return jetContainerShallowCopy;
}

/** execute - called on every event for jetTags */
StatusCode JetMetTagTool::execute(TagFragmentCollection& jetMissingEtTagColl, const int max) {

  ATH_MSG_DEBUG(  "in execute() - jet" );

  static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");

  /** retrieve the AOD Jet container */
  const xAOD::JetContainer *jetContainer=0;
  StatusCode sc = evtStore()->retrieve( jetContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "No AOD Jet container ("<<m_containerName<<") found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "AOD Jet container ("<<m_containerName<<") successfully retrieved" );

  /** Calibrate and record a shallow copy of the jet container */
  const xAOD::JetContainer * jetContainerShallowCopy = calibrateAndRecordShallowCopyJetCollection(jetContainer);
  if(!jetContainerShallowCopy){
    ATH_MSG_WARNING(  "Unable to create calibrated jet shallow copy container" );
    return StatusCode::SUCCESS;
  }

  /** determine jet scale to use */
  xAOD::JetScale scale = m_useEMScale ? xAOD::JetEMScaleMomentum : xAOD::JetAssignedScaleMomentum ;

  /** create a new copy for MET calculation */
  ConstDataVector< xAOD::JetContainer >* selectedJets = new ConstDataVector< xAOD::JetContainer >( SG::VIEW_ELEMENTS );
  ATH_CHECK( evtStore()->record( selectedJets, m_jetCalibcontainerName_skim ) );

  for ( const xAOD::Jet *originalJet : * jetContainerShallowCopy ) {

    const xAOD::JetFourMom_t &jetP4 = originalJet->jetP4(scale);

    /** select and store Jets */
    double pt = jetP4.Pt();

    bool select = pt > m_jetPtCut_skim;

    if (select) {

      /**
      //retrieve jvt information
      bool hasjvt = originalJet->isAvailable<float>("JvtUpdate");

      if (hasjvt) {

	//apply pile up removal selection
	float jvt = originalJet->auxdata<float>("JvtUpdate");
	if ( !( originalJet->pt()< 60000. && fabs(originalJet->eta())<2.4 && fabs(jvt) < 0.59 ) ) {
	  selectedJets->push_back( originalJet );
	}
      }
      */

      /** we do take all jets at this point, JVT cut is done in METmaker */
      selectedJets->push_back( originalJet );

    }
  }

  /** vector to store all jets that pass selection cuts */
  std::vector<const xAOD::Jet *> selectedJetsVector;
  selectedJetsVector.reserve(jetContainerShallowCopy->size() );

  /** for counting the total energy in jets */
  float jetSumEt = 0;
  /** select and store jets that pass selection cuts into 'selectedJets' vector */
  for ( const xAOD::Jet *calibratedJet : *jetContainerShallowCopy ) {

    const xAOD::JetFourMom_t &jetP4 = calibratedJet->jetP4(scale);

    /** select and store Jets */
    double pt = jetP4.Pt();

    bool select = ( pt > m_jetPtCut ) && (  looseBadTool->accept( *calibratedJet) );

    if (select) {
      ATH_MSG_DEBUG(" Selected jet with pt=" << pt);

      /** jet summed Et - to be implemented correctly */
      jetSumEt += pt;

      selectedJetsVector.push_back(calibratedJet);
    } else {
      ATH_MSG_DEBUG( "Did not select jet with pt=" << jetP4.pt() );
    }
  }

  /** sort selected jets container by pT */
  AnalysisUtils::Sort::pT( &selectedJetsVector );

  /** make the selection */
  int i      = 0;
  /** for counting different pT jets and b-jets */
  int ij40   = 0;
  int ij50   = 0;
  int ij55   = 0;
  int ij80   = 0;
  int ij100  = 0;
  int iBj40  = 0;
  int iBj50  = 0;
  int iBj55  = 0;
  int iBj80  = 0;
  int iBj100 = 0;

  for (const xAOD::Jet *selectedJet : selectedJetsVector) {

    ATH_MSG_DEBUG(" Before touching signal state:" << selectedJet->pt());
    const xAOD::JetFourMom_t &jetP4 = selectedJet->jetP4(scale);

    if (i < max) {

      /** pt */
      jetMissingEtTagColl.insert(m_ptStr[i], selectedJet->pt());

      /** e */
      jetMissingEtTagColl.insert(m_eStr[i], selectedJet->e());

      /** eta */
      jetMissingEtTagColl.insert(m_etaStr[i], selectedJet->eta());

      /** phi */
      jetMissingEtTagColl.insert(m_phiStr[i], selectedJet->phi());

      /** add PID information */
      unsigned int pid = 0;

      /** isBadJet */
      if (!isUglyTool->accept(*selectedJet))
	pid |= 1 << 0;
      if (!looseBadTool->accept(*selectedJet))
	pid |= 1 << 2;
      if (!tightBadTool->accept(*selectedJet))
	pid |= 1 << 4;

      /** get JVT */
      bool hasjvt = selectedJet->isAvailable<float>("Jvt");
      if (hasjvt) {
      float jvt = selectedJet->auxdata<float>("Jvt");

	if (fabs(jvt) > 0.2){
	  pid |= 1 << 6;
	}
	if (fabs(jvt) > 0.64){
	  pid |= 1 << 7;
	}
	if (fabs(jvt) > 0.59){
	  pid |= 1 << 17;
	}
      }

      /** get JVF */
      std::vector<float> jvf_v;
      bool hasjvf = selectedJet->getAttribute < std::vector<float> > (xAOD::JetAttribute::JVF, jvf_v);
      if (hasjvf) {
	double jetVertFrac = -1;
	if (!jvf_v.empty())
	  jetVertFrac = jvf_v[0];
	if (jetVertFrac > 0.0)
	  pid |= 1 << 8;
	if (jetVertFrac > 0.5)
	  pid |= 1 << 9;
	if (jetVertFrac > 0.75)
	  pid |= 1 << 10;
	if (jetVertFrac > 0.9)
	  pid |= 1 << 11;
      }

      /** B-tagging using selection tools*/

      /** B-tagging */
      const xAOD::BTagging* btag = selectedJet->btagging();
      if (bool(btag)) {
	if ( m_FixedCutBEff_60->accept( selectedJet ) ) pid |= 1 << 12; 	 // MV2c20 @ 60%
	if ( m_FixedCutBEff_70->accept( selectedJet ) ) pid |= 1 << 13; 	 // MV2c20 @ 70%
	if ( m_FixedCutBEff_85->accept( selectedJet ) ) pid |= 1 << 14; 	 // MV2c20 @ 85%
	// if ( m_FlatBEff_60->accept( selectedJet ) ) pid |= 1 << 18; 	 // MV2c20 @ flat 60%
	// if ( m_FlatBEff_70->accept( selectedJet ) ) pid |= 1 << 19; 	 // MV2c20 @ flat 70%
	// if ( m_FlatBEff_77->accept( selectedJet ) ) pid |= 1 << 20; 	 // MV2c20 @ flat 77%
      }

      /** isbadCHF */
      std::vector<float> sumPtTrkvec;
      selectedJet->getAttribute( xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec );
      float sumpttrk = 0.;
      if( ! sumPtTrkvec.empty() ) sumpttrk = sumPtTrkvec[0];
      float chf = sumpttrk/selectedJet->pt();

      if (chf < 0.1) pid |= 1 << 16;

      jetMissingEtTagColl.insert(m_pidStr[i], pid);

    }

    /** count jets |eta|<2.8 with different pT cuts */
    if (fabs(jetP4.eta()) < 2.8) {
      if (jetP4.pt() > 40.0 * CLHEP::GeV)
	ij40++;
      if (jetP4.pt() > 50.0 * CLHEP::GeV)
	ij50++;
      if (jetP4.pt() > 55.0 * CLHEP::GeV)
	ij55++;
      if (jetP4.pt() > 80.0 * CLHEP::GeV)
	ij80++;
      if (jetP4.pt() > 100.0 * CLHEP::GeV)
	ij100++;

      const xAOD::BTagging* btag = selectedJet->btagging();
      if (bool(btag)) {
	double mvx;
	btag->MVx_discriminant("MV2c20", mvx);
	if (fabs(jetP4.eta()) < 2.5 && mvx > -0.046) {
	  if (jetP4.pt() > 40.0 * CLHEP::GeV)
	    iBj40++;
	  if (jetP4.pt() > 50.0 * CLHEP::GeV)
	    iBj50++;
	  if (jetP4.pt() > 55.0 * CLHEP::GeV)
	    iBj55++;
	  if (jetP4.pt() > 80.0 * CLHEP::GeV)
	    iBj80++;
	  if (jetP4.pt() > 100.0 * CLHEP::GeV)
	    iBj100++;
	}
      }
    }

    /** count the total number of jets */
    i++;
  }

  /** insert the number Jet and BJet */
  jetMissingEtTagColl.insert(JetAttributeNames[PJet::NJet], i);
  //  jetMissingEtTagColl.insert(JetAttributeNames[PJet::NBJet], nbjet);
  jetMissingEtTagColl.insert(JetAttributeNames[PJet::JetSumET], jetSumEt);

  /** add the encoded jet and b-tagged jet counting information */
  jetMissingEtTagColl.insert(JetAttributeNames[PJet::NpTJet], ij40+100*ij50+10000*ij55+1000000*ij80+100000000*ij100);
  jetMissingEtTagColl.insert(JetAttributeNames[PJet::NpTBJet], iBj40+100*iBj50+10000*iBj55+1000000*iBj80+100000000*iBj100);

  return StatusCode::SUCCESS;
}

/** recomputeMissingEt - called on every event for missing Et */
StatusCode JetMetTagTool::recomputeMissingEt() {

  ATH_MSG_DEBUG(  "in recomputeMissingEt() - missing Et" );

  // Create a MissingETContainer with its aux store

  xAOD::MissingETContainer* newMet = new xAOD::MissingETContainer();
  if( evtStore()->record(newMet, m_metContainerName).isFailure() ) {
    ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_metContainerName);
    return StatusCode::SUCCESS;
  }

  xAOD::MissingETAuxContainer* metAuxCont = new xAOD::MissingETAuxContainer();
  if( evtStore()->record(metAuxCont, m_metContainerName+"Aux.").isFailure() ) {
    ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_metContainerName+"Aux.");
    return StatusCode::SUCCESS;
  }
  newMet->setStore(metAuxCont);

  // Settings for MET calculation ***********************************************

  //  std::vector<const xAOD::IParticle*> uniques;
  //MissingETBase::UsageHandler::Policy objScale = MissingETBase::UsageHandler::PhysicsObject;

  // Retrieve containers and compute MET ****************************************

  /// MET map
  const xAOD::MissingETAssociationMap* metMap = 0;
  if( evtStore()->retrieve(metMap, m_METMapName).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_METMapName);
    return StatusCode::SUCCESS;
  }
  xAOD::MissingETAssociationHelper metHelper(metMap);

  /// MET core
  const xAOD::MissingETContainer* coreMet(0);
  if( evtStore()->retrieve(coreMet, m_METCoreName).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_METCoreName);
    return StatusCode::SUCCESS;
  }

  /// Electrons
  const xAOD::ElectronContainer* elCont(0);
  if(!m_ElectronsContainer_skim.empty()) {
    if( evtStore()->retrieve(elCont, m_ElectronsContainer_skim).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container: " << m_ElectronsContainer_skim);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved electron collection");

    // compute MET term
    //    uniques.clear();
    if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, newMet,
			       elCont,
			       &metHelper).isFailure() ) {
      ATH_MSG_WARNING("Failed to build electron term.");
    }
    //ATH_MSG_DEBUG("Selected " << elCont->size() << " MET electrons. "
    //    		              << uniques.size() << " are non-overlapping.");
  }

  /// Photons
  const xAOD::PhotonContainer* phCont(0);
  if(!m_PhotonsContainer_skim.empty()) {
    if( evtStore()->retrieve(phCont, m_PhotonsContainer_skim).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input photon container: " << m_PhotonsContainer_skim);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved photon collection");

    // compute MET term
    //uniques.clear();
    if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, newMet,
			       phCont,
			       &metHelper).isFailure() ) {
      ATH_MSG_WARNING("Failed to build photon term.");
    }
    //ATH_MSG_DEBUG("Selected " << phCont->size() << " MET photons. "
    //		              << uniques.size() << " are non-overlapping.");
  }

  /// Taus
  const xAOD::TauJetContainer* tauCont(0);
  if(!m_TausContainer_skim.empty()) {
    if( evtStore()->retrieve(tauCont, m_TausContainer_skim).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input tau container: " << m_TausContainer_skim);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved tau collection");

    // compute MET term
    //uniques.clear();
    if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, newMet,
			       tauCont,
			       &metHelper).isFailure() ){
      ATH_MSG_WARNING("Failed to build tau term.");
    }
    //ATH_MSG_DEBUG("Selected " << tauCont->size() << " MET taus. "
    //<< uniques.size() << " are non-overlapping.");
  }

  /// Muons
  const xAOD::MuonContainer* muonCont(0);
  if(!m_MuonsContainer_skim.empty()) {
    if( evtStore()->retrieve(muonCont, m_MuonsContainer_skim).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input muon container: " << m_MuonsContainer_skim);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved muon collection");

    // compute MET term
    //uniques.clear();
    if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, newMet,
			       muonCont,
			       &metHelper).isFailure() ) {
      ATH_MSG_WARNING("Failed to build muon term.");
    }
    //ATH_MSG_DEBUG("Selected " << muonCont->size() << " MET muons. "
    //<< uniques.size() << " are non-overlapping.");
  }

  /// Jets
  const xAOD::JetContainer* jetCont(0);
  if( evtStore()->retrieve(jetCont, m_jetCalibcontainerName_skim).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve input jet container: " << m_jetCalibcontainerName_skim);
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Successfully retrieved jet collection");

  // compute MET term
  //uniques.clear();
  if( m_metmaker->rebuildJetMET("RefJet", m_metSoftClusName, m_metPVSoftTrkName, newMet,
				jetCont, coreMet, &metHelper, true ).isFailure() ) {
    ATH_MSG_WARNING("Failed to build jet and soft terms.");
  }
  //ATH_MSG_DEBUG("Of " << jetCont->size() << " jets, "
  //<< uniques.size() << " are non-overlapping.");

  // compute Track soft term
  MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
  if((*newMet)[m_metPVSoftTrkName]) trksource = (*newMet)[m_metPVSoftTrkName]->source();
  if( met::buildMETSum("FinalTrk", newMet, trksource).isFailure() ){
    ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
  }

  // compute cluster soft term
  MissingETBase::Types::bitmask_t clsource = MissingETBase::Source::LCTopo;
  if((*newMet)[m_metSoftClusName]) clsource = (*newMet)[m_metSoftClusName]->source();
  if( met::buildMETSum("FinalClus", newMet, clsource).isFailure() ) {
    ATH_MSG_WARNING("Building MET FinalClus sum failed.");
  }
  return StatusCode::SUCCESS;
}

/** execute - called on every event for missing Et */
StatusCode JetMetTagTool::execute(TagFragmentCollection& missingEtTagColl) {

  ATH_MSG_DEBUG(  "in execute() - missing Et" );

  /** Calibrate and record a shallow copy of the jet container */
  StatusCode sc = recomputeMissingEt();
  if(sc.isFailure()){
    ATH_MSG_WARNING(  "Unable to recompute MissingET" );
    return StatusCode::SUCCESS;
  }

  const xAOD::MissingETContainer *met(0); //comment out const
  sc = evtStore()->retrieve( met, m_metContainerName);
  if (sc.isFailure()) {
      ATH_MSG_WARNING(  "No recomputed MissingET container found in SG" );
      return StatusCode::SUCCESS;
  }

  const xAOD::MissingET* metfinal = (*met)[m_metRefFinalName]; //comment out const
  if (!metfinal) {
    ATH_MSG_WARNING(  "No total MissingET object found in container with name " << m_metRefFinalName );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG(  "MET Final object successfully retrieved " );

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalX], metfinal->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalY], metfinal->mpy());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::SumET], metfinal->sumet());

  const xAOD::MissingET *metSoftTerm = (*met)[m_metSoftClusName];
  if ( !metSoftTerm ) {
    ATH_MSG_WARNING(  "No soft MissingET found in container with name " << m_metSoftClusName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_SoftClusX], metSoftTerm->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_SoftClusY], metSoftTerm->mpy());

  const xAOD::MissingET *metRefJet = (*met)[m_metRefJetName];
  if ( !metRefJet ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefJetName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefJetX], metRefJet->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefJetY], metRefJet->mpy());


  const xAOD::MissingET *metRefTau = (*met)[m_metRefTauName];
  if ( !metRefTau ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefTauName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauX], metRefTau->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauY], metRefTau->mpy());


  const xAOD::MissingET *metMuons = (*met)[m_metMuonsName];
  if ( !metMuons ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metMuonsName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_MuonsX], metMuons->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_MuonsY], metMuons->mpy());


  const xAOD::MissingET *metRefEle = (*met)[m_metRefEleName];
  if ( !metRefEle ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefEleName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefEleX], metRefEle->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefEleY], metRefEle->mpy());


  const xAOD::MissingET *metRefGamma = (*met)[m_metRefGammaName];
  if ( !metRefGamma ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefGammaName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefGammaX], metRefGamma->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefGammaY], metRefGamma->mpy());


  const xAOD::MissingET *metPVSoftTrk = (*met)[m_metPVSoftTrkName];
  if ( !metPVSoftTrk ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metPVSoftTrkName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_PVSoftTrkX], metPVSoftTrk->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_PVSoftTrkY], metPVSoftTrk->mpy());

  const xAOD::MissingET *metFinalTrk = (*met)[m_metFinalTrkName];
  if ( !metFinalTrk ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metFinalTrkName );
    return StatusCode::SUCCESS;
  }

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_FinalTrkX], metFinalTrk->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_FinalTrkY], metFinalTrk->mpy());

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode JetMetTagTool::finalize() {
  ATH_MSG_DEBUG(  "in finalize()" );
  CHECK(m_jetCalibrationTool.release());
  return StatusCode::SUCCESS;
}

/** destructor */
JetMetTagTool::~JetMetTagTool() {}


