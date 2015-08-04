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

#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODJet/JetAuxContainer.h"
#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "TagEvent/MissingETAttributeNames.h"
#include "TagEvent/ParticleJetAttributeNames.h"

// JetCalibTools includes
#include "JetCalibTools/IJetCalibrationTool.h"

#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"


#include <sstream>
#include <vector>



using xAOD::Jet;
using xAOD::MissingET;
using xAOD::MissingETContainer;

// define some global/static selectors
// veryLooseBadTool, looseBadTool, etc... are defined here
#include "JetSelectorDefs.h"

/** the constructor */
JetMetTagTool::JetMetTagTool (const std::string& type, const std::string& name, 
                                          const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_jetCalibrationTool("") {

  /** JetMissingEt AOD Container Name */
  declareProperty("JetCalibrationTool",    m_jetCalibrationTool);

  /** JetMissingEt AOD Container Name */
  declareProperty("JetContainer",          m_containerName           = "AntiKt4EMTopo");
  declareProperty("METContainer",          m_metContainerName        = "MET_Reference_AntiKt4Topo_TAGcalib");
  declareProperty("METFinalName",          m_metRefFinalName         = "FinalClus");
  // declareProperty("METSoftJetName",        m_metSoftJetName          = "MET_SoftJets");
  // declareProperty("METRefMuonInName",      m_metRefMuonInName        = "MET_RefMuons");
  declareProperty("METMuonsName",          m_metMuonsName            = "Muons");
  declareProperty("METSoftTermName",       m_metSoftTermName         = "SoftClus");
  declareProperty("METRefTauName",         m_metRefTauName           = "RefTau");
  declareProperty("JetCalibContainer",     m_jetCalibcontainerName   = "AntiKt4TopoJets_TAGcalib");
  /** selection cut of Pt */
  declareProperty("EtCut",                 m_jetPtCut                = 40.0*CLHEP::GeV);

  declareProperty("UseEMScale",            m_useEMScale              = false);
  declareProperty("isSimulation",          m_isSimulation            = false);
  
  declareInterface<JetMetTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  JetMetTagTool::initialize() {
  ATH_MSG_DEBUG(  "in intialize()" );

  // retrieve the jet calibration tool
  CHECK(m_jetCalibrationTool.retrieve());

  // init selectors as defined in JetSelectorDefs.h
  CHECK(initJetSelectors());

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap) {

  ATH_MSG_DEBUG(  "in attributeSpecification() for missingET" );

  /** MissingET Ref_final attributes */
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalX] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefFinalX], MissingEtAttributeGroupNames[EtMiss::MET_RefFinalX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalY] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefFinalY], MissingEtAttributeGroupNames[EtMiss::MET_RefFinalY]);
  attrMap[ MissingEtAttributeNames[EtMiss::SumET] ]        = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::SumET],        MissingEtAttributeGroupNames[EtMiss::SumET]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_CellOutX] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_CellOutX], MissingEtAttributeGroupNames[EtMiss::MET_CellOutX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_CellOutY] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_CellOutY], MissingEtAttributeGroupNames[EtMiss::MET_CellOutY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauX] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefTauX], MissingEtAttributeGroupNames[EtMiss::MET_RefTauX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauY] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefTauY], MissingEtAttributeGroupNames[EtMiss::MET_RefTauY]);

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMetTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                       const int max) {
  ATH_MSG_DEBUG(  "in attributeSpecification() for jet" );

  std::ostringstream os;

  /** Jet Attributes */
  attrMap[ JetAttributeNames[PJet::NJet] ]      = AthenaAttributeType("unsigned int",  JetAttributeUnitNames[PJet::NJet],  JetAttributeGroupNames[PJet::NJet]);
  attrMap[ JetAttributeNames[PJet::JetSumET] ]  = AthenaAttributeType("float", JetAttributeUnitNames[PJet::JetSumET], JetAttributeGroupNames[PJet::JetSumET]);

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
  
  for ( xAOD::Jet *shallowCopyJet : * jetContainerShallowCopy ) {
    
    if( m_jetCalibrationTool->applyCalibration(*shallowCopyJet).isFailure() ){
      ATH_MSG_WARNING( "Failed to apply calibration to the jet container"); 
      return 0;
    }
    const xAOD::IParticleLink originLink( *jetContainer, shallowCopyJet->index() );
    accSetOriginLink(*shallowCopyJet) = originLink;
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
 
  /** retrieve the AOD Jet container */
  const xAOD::JetContainer *jetContainer=0;
  StatusCode sc = evtStore()->retrieve( jetContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "No AOD Jet container ("<<m_containerName<<") found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "AOD Jet container ("<<m_containerName<<") successfully retrieved" );

  // Calibrate and record a shallow copy of the jet container 
  const xAOD::JetContainer * jetContainerShallowCopy = calibrateAndRecordShallowCopyJetCollection(jetContainer);
  if(!jetContainerShallowCopy){
    ATH_MSG_WARNING(  "Unable to create calibrated jet shallow copy container" );
    return StatusCode::SUCCESS;
  }
  // vector to store all jets that pass selection cuts
  std::vector<const xAOD::Jet *> selectedJetsVector;
  selectedJetsVector.reserve(jetContainerShallowCopy->size() );

  // determine jet scale to use
  xAOD::JetScale scale = m_useEMScale ? xAOD::JetEMScaleMomentum : xAOD::JetAssignedScaleMomentum ;

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
  int i=0;
  /** for counting different pT jets and b-jets */
  int ij40=0;
  int ij50=0;
  int ij55=0;
  int ij80=0;
  int ij100=0;
  int iBj40=0;
  int iBj50=0;
  int iBj55=0;
  int iBj80=0;
  int iBj100=0;

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
      if (jet::JetCaloQualityUtils::isUgly(selectedJet))
	pid |= 1 << 0;
      if (!looseBadTool->accept(*selectedJet))
	pid |= 1 << 2;
      if (!tightBadTool->accept(*selectedJet))
	pid |= 1 << 4;
      
      /** get JVT */
      bool hasjvt = selectedJet->isAvailable<float>("Jvt");
      if (hasjvt) {
	float jvt = selectedJet->auxdata<float>("Jvt");
	if (fabs(jvt) > 0.2)
	  pid |= 1 << 6;
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

      /** B-tagging */
      const xAOD::BTagging* btag = selectedJet->btagging();
      if (bool(btag)) {
	double mvx;
	btag->MVx_discriminant("MV2c20", mvx);
	
	if (mvx > 0.473)
	  pid |= 1 << 12; 	 // MV2c20 @ 60%
	if (mvx > -0.046)
	  pid |= 1 << 13; 	 // MV2c20 @ 70%
	if (mvx > -0.819)
	  pid |= 1 << 14; 	 // MV2c20 @ 85%
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



/** execute - called on every event for missing Et */
StatusCode JetMetTagTool::execute(TagFragmentCollection& missingEtTagColl) {

  ATH_MSG_DEBUG(  "in execute() - missing Et" );
  
  const MissingETContainer *met(0); //comment out const
  StatusCode sc = evtStore()->retrieve( met, m_metContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "No MissingET container found in SG" ); 
    return StatusCode::SUCCESS;
  }
  
  const MissingET* metfinal = (*met)[m_metRefFinalName]; //comment out const
  if (!metfinal) {
    ATH_MSG_WARNING(  "No total MissingET object found in container with name " << m_metRefFinalName ); 
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG(  "MET Final object successfully retrieved " );
  
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalX], metfinal->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalY], metfinal->mpy());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::SumET], metfinal->sumet());
  
  const MissingET *metSoftTerm = (*met)[m_metSoftTermName];
  if ( !metSoftTerm ) {
    ATH_MSG_WARNING(  "No soft MissingET found in container with name " << m_metSoftTermName );
    return StatusCode::SUCCESS;
  } 

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_CellOutX], metSoftTerm->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_CellOutY], metSoftTerm->mpy());

  const MissingET *metRefTau = (*met)[m_metRefTauName];
  if ( !metRefTau ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefTauName );
    return StatusCode::SUCCESS;
  } 

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauX], metRefTau->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauY], metRefTau->mpy());


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


