/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : JetMissingEtTagTool.cxx
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a collection of JetMissingEtJetTag

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "StoreGate/StoreGateSvc.h"

//#include "JetEvent/JetCollection.h"
#include "xAODJet/JetContainer.h"
//#include "JetUtils/JetCaloHelper.h"
#include "JetUtils/JetCaloQualityUtils.h"

// #include "MissingETEvent/MissingET.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"
#include "TagEvent/MissingETAttributeNames.h"
#include "TagEvent/ParticleJetAttributeNames.h"

#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>



using xAOD::Jet;
using xAOD::MissingET;
using xAOD::MissingETContainer;

// define some global/static selectors
// veryLooseBadTool, looseBadTool, etc... are defined here
#include "JetSelectorDefs.h"

/** the constructor */
JetMissingEtTagTool::JetMissingEtTagTool (const std::string& type, const std::string& name, 
					  const IInterface* parent) : 
  AthAlgTool( type, name, parent ) {

  /** JetMissingEt AOD Container Name */
  declareProperty("JetContainer",          m_containerName           = "AntiKt4LCTopoJets");
  declareProperty("METContainer",          m_metContainerName        = "MET_RefFinal");
  declareProperty("METFinalName",          m_metRefFinalName         = "Final");
  // declareProperty("METSoftJetName",        m_metSoftJetName          = "MET_SoftJets");
  // declareProperty("METRefMuonInName",      m_metRefMuonInName        = "MET_RefMuons");
  declareProperty("METMuonsName",          m_metMuonsName            = "Muons");
  declareProperty("METSoftTermName",       m_metSoftTermName         = "SoftClus");
  declareProperty("METRefTauName",         m_metRefTauName           = "RefTau");

  /** selection cut of Pt */
  declareProperty("EtCut",                 m_jetPtCut                = 40.0*CLHEP::GeV);

  declareProperty("UseEMScale",            m_useEMScale              = false);
  declareProperty("isSimulation",          m_isSimulation            = false);
  
  declareInterface<JetMissingEtTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  JetMissingEtTagTool::initialize() {
  ATH_MSG_DEBUG(  "in intialize()" );

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
    return sc;
  }

  // init selectors as defined in JetSelectorDefs.h
  CHECK(initJetSelectors());

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMissingEtTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap) {

  ATH_MSG_DEBUG(  "in attributeSpecification() for missingET" );

  /** MissingET Ref_final attributes */
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalX] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefFinalX], MissingEtAttributeGroupNames[EtMiss::MET_RefFinalX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefFinalY] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefFinalY], MissingEtAttributeGroupNames[EtMiss::MET_RefFinalY]);
  attrMap[ MissingEtAttributeNames[EtMiss::SumET] ]        = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::SumET],        MissingEtAttributeGroupNames[EtMiss::SumET]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_CellOutX] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_CellOutX], MissingEtAttributeGroupNames[EtMiss::MET_CellOutX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_CellOutY] ] = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_CellOutY], MissingEtAttributeGroupNames[EtMiss::MET_CellOutY]);
  // attrMap[ MissingEtAttributeNames[EtMiss::MET_SoftJetsX] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_SoftJetsX], MissingEtAttributeGroupNames[EtMiss::MET_SoftJetsX]);
  // attrMap[ MissingEtAttributeNames[EtMiss::MET_SoftJetsY] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_SoftJetsY], MissingEtAttributeGroupNames[EtMiss::MET_SoftJetsY]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauX] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefTauX], MissingEtAttributeGroupNames[EtMiss::MET_RefTauX]);
  attrMap[ MissingEtAttributeNames[EtMiss::MET_RefTauY] ]     = AthenaAttributeType("float", MissingEtAttributeUnitNames[EtMiss::MET_RefTauY], MissingEtAttributeGroupNames[EtMiss::MET_RefTauY]);

  return StatusCode::SUCCESS;
}

/** build the attribute list - called in initialize */
StatusCode JetMissingEtTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
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
  attrMap[ JetAttributeNames[PJet::NpTBJet] ]      = AthenaAttributeType("unsigned int",  JetAttributeUnitNames[PJet::NpTBJet],  JetAttributeGroupNames[PJet::NpTBJet]);

  return StatusCode::SUCCESS;
}

/** execute - called on every event for jetTags */
StatusCode JetMissingEtTagTool::execute(TagFragmentCollection& jetMissingEtTagColl, const int max) {

  ATH_MSG_DEBUG(  "in execute() - jet" );

  /** retrieve the AOD Jet container */
  const xAOD::JetContainer * jetContainer=0;
  StatusCode sc = m_storeGate->retrieve( jetContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "No AOD Jet container ("<<m_containerName<<") found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "AOD Jet container ("<<m_containerName<<") successfully retrieved" );

  std::vector<const Jet*> userContainer (jetContainer->begin(),
                                         jetContainer->end());

  //Jet should be sorted by pt

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
  float jetSumEt = 0;

  for (auto jetItr=jetContainer->begin(); jetItr != jetContainer->end(); ++jetItr){

    ATH_MSG_DEBUG(  " Before touching signal state:" << (*jetItr)->pt() );
    const xAOD::Jet* jet = *jetItr;

    xAOD::JetScale s = m_useEMScale ? xAOD::JetEMScaleMomentum : xAOD::JetAssignedScaleMomentum ;
    const xAOD::JetFourMom_t & jetP4 = jet->jetP4(s);
      
    /** select  Jets */
    double pt      = jetP4.Pt();
    bool select =  ( pt > m_jetPtCut ) && (  veryLooseBadTool->accept( *jet) );

    if ( select ) { 
      ATH_MSG_DEBUG(  " Selected jet with pt=" << pt);
      /** jet summed Et - to be implemented correctly */ 
      jetSumEt += pt;

      if ( i<max ) { 

         /** pt */
         jetMissingEtTagColl.insert( m_ptStr[i], (*jetItr)->pt() );
       
         /** eta */
         jetMissingEtTagColl.insert( m_etaStr[i], (*jetItr)->eta() );

         /** phi */
         jetMissingEtTagColl.insert( m_phiStr[i], (*jetItr)->phi() );

         /** add PID information */
         unsigned int pid = 0;
         
         /** isBadJet */
         if (jet::JetCaloQualityUtils::isUgly( *jetItr ))  pid |= 1<<0;
         
         if (!veryLooseBadTool->accept( *jet))               pid |= 1<<1;
         if (!looseBadTool->accept( *jet))                   pid |= 1<<2;
         if (!mediumBadTool->accept( *jet))                  pid |= 1<<3;
         if (!tightBadTool->accept( *jet))                   pid |= 1<<4;
         
         
         /** get JVF */
         std::vector<float> jvf_v = jet->getAttribute<std::vector<float> >(xAOD::JetAttribute::JVF);
         double jetVertFrac = -1;
         if(!jvf_v.empty()) jetVertFrac= jvf_v[0];
         if (jetVertFrac > 0.0)  pid |= 1<<8;
         if (jetVertFrac > 0.5)  pid |= 1<<9;
         if (jetVertFrac > 0.75) pid |= 1<<10;
         if (jetVertFrac > 0.9)  pid |= 1<<11;
         
         /** B-tagging */
         const xAOD::BTagging* btag =  (*jetItr)->btagging();
         double mv1 = btag->MV1_discriminant();

         if (mv1 >  0.9827)  pid |= 1<< 12; 	 // MV1 @ 60% 
         if (mv1 >  0.7892)  pid |= 1<< 13; 	 // MV1 @ 70% 
         if (mv1 >  0.6073)  pid |= 1<< 14; 	 // MV1 @ 75% 
         if (mv1 >  0.1340)  pid |= 1<< 15; 	 // MV1 @ 85% 
         
         jetMissingEtTagColl.insert( m_pidStr[i], pid);
         
      }
      
      /** count jets |eta|<2.8 with different pT cuts */
      if (fabs(jetP4.eta())<2.8){
        if (jetP4.pt() > 40.0*CLHEP::GeV) ij40++;
        if (jetP4.pt() > 50.0*CLHEP::GeV) ij50++;
        if (jetP4.pt() > 55.0*CLHEP::GeV) ij55++;
        if (jetP4.pt() > 80.0*CLHEP::GeV) ij80++;
        if (jetP4.pt() > 100.0*CLHEP::GeV) ij100++;

        const xAOD::BTagging* btag =  (*jetItr)->btagging();
        double mv1 = btag->MV1_discriminant();
        if (fabs(jetP4.eta()) < 2.5 &&  mv1 >  0.6073){
          if (jetP4.pt() > 40.0*CLHEP::GeV) iBj40++;
          if (jetP4.pt() > 50.0*CLHEP::GeV) iBj50++;
          if (jetP4.pt() > 55.0*CLHEP::GeV) iBj55++;
          if (jetP4.pt() > 80.0*CLHEP::GeV) iBj80++;
          if (jetP4.pt() > 100.0*CLHEP::GeV) iBj100++;
        }
      }

      /** count the total number of jets */
      i++;

    }
    else
      ATH_MSG_DEBUG(  "Did not select jet with pt=" << jetP4.pt() );
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
StatusCode JetMissingEtTagTool::execute(TagFragmentCollection& missingEtTagColl) {

  ATH_MSG_DEBUG(  "in execute() - missing Et" );

  const MissingETContainer *met(0);
  StatusCode sc = m_storeGate->retrieve( met, m_metContainerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "No MissingET container found in SG" ); 
    return StatusCode::SUCCESS;
  }

  const MissingET* metfinal = (*met)[m_metRefFinalName];
  if (!metfinal) {
    ATH_MSG_WARNING(  "No total MissingET object found in container with name " << m_metRefFinalName ); 
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG(  "MET Final object successfully retrieved " );

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalX], metfinal->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefFinalY], metfinal->mpy());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::SumET], metfinal->sumet());

  const MissingET *metSoftTerm = (*met)[m_metSoftTermName];
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING(  "No soft MissingET found in container with name " << m_metSoftTermName );
    return StatusCode::SUCCESS;
  } 

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_CellOutX], metSoftTerm->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_CellOutY], metSoftTerm->mpy());

  const MissingET *metRefTau = (*met)[m_metRefTauName];
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING(  "No tau MissingET found in container with name " << m_metRefTauName );
    return StatusCode::SUCCESS;
  } 

  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauX], metRefTau->mpx());
  missingEtTagColl.insert(MissingEtAttributeNames[EtMiss::MET_RefTauY], metRefTau->mpy());


  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode JetMissingEtTagTool::finalize() {
  ATH_MSG_DEBUG(  "in finalize()" );
  return StatusCode::SUCCESS;
}

/** destructor */
JetMissingEtTagTool::~JetMissingEtTagTool() {}


