/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if 0 // doesn't compile in devval
//////////////////////// -*- C++ -*- /////////////////////////////
// PAUtruthTrackClassifier.cxx 
// Implementation file for class PAUtruthTrackClassifier
// Author: H.Abreu<abreu@lal.in2p3.fr>
/////////////////////////////////////////////////////////////////// 

// PhotonAnalysisUtils includes
#include "PhotonAnalysisUtils/PAUtruthTrackClassifier.h"

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

// StoreGate
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/TrkParticleBaseDict.h"

#include "HepMC/GenParticle.h"

#define UNKNOWN -99999


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
PAUtruthTrackClassifier::PAUtruthTrackClassifier( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_TruthClassifierTool( "MCTruthClassifier" ),
  m_storeGate( "StoreGateSvc", name )
{
  declareInterface<IPAUtruthTrackClassifier>(this);
  declareProperty( "EgammaTruthClassifierTool", m_TruthClassifierTool, "the egammaTruthClassifierTool" ); 
 

}

// Destructor
///////////////
PAUtruthTrackClassifier::~PAUtruthTrackClassifier()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode PAUtruthTrackClassifier::initialize()
{
  
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;

  if ( m_TruthClassifierTool.retrieve().isFailure() ) { 
    log << MSG::FATAL << "Failed to retrieve tool " << m_TruthClassifierTool << endreq;
    return StatusCode::FAILURE;
  } else
    log << MSG::INFO << "Retrieved tool " << m_TruthClassifierTool << endreq;
 
  return StatusCode::SUCCESS;

}

StatusCode PAUtruthTrackClassifier::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

// ======================================================================================
void PAUtruthTrackClassifier::ConvPhotClassifier( PAU::egamma* g ) {
  
  using namespace MCTruthPartClassifier;
  
  bool isConv = ( g->convFlag()>0 && (g->convFlag())%10>0 ) ;

  vector_pdgID[0]= vector_pdgID[1]=UNKNOWN;
  vector_pdgMOTHER[0]= vector_pdgMOTHER[1]=UNKNOWN;
  vector_barcodeMOTHER[0]= vector_barcodeMOTHER[1]=UNKNOWN;
  vector_pdgGrandMOTHER[0]=  vector_pdgGrandMOTHER[1]=UNKNOWN;
  vector_Type[0]= vector_Type[1]=UNKNOWN;
  vector_phtType[0]= vector_phtType[1]=UNKNOWN;
  vector_Origin[0]= vector_Origin[1]=UNKNOWN; 
  vector_phtOrigin[0]= vector_phtOrigin[1]=UNKNOWN;
  
  if ( isConv ) {  // recover all conversion info:

    std::pair< unsigned int, unsigned int> tpTruthResult;

    if (g->convVx()) {  

      int pdgID = UNKNOWN;
      int pdgMOTHER = UNKNOWN;
      //int barcodeMOTHER = UNKNOWN;
      int pdgGrandMOTHER = UNKNOWN;


      /*std::pair<ParticleType,ParticleOrigin> photon=*/m_TruthClassifierTool->particleTruthClassifier(g->photon());
      std::vector<const HepMC::GenParticle*> vecTruthPart=m_TruthClassifierTool->getCnvPhotTrkToTruthPart();
      std::vector<ParticleType>   vecPartType  = m_TruthClassifierTool->getCnvPhotPartType(); 
      std::vector<ParticleOrigin> vecPartOrig = m_TruthClassifierTool->getCnvPhotPartOrig();
 
      for (unsigned int i=0; i<vecTruthPart.size();i++) {

	if ( i>1 ) continue;	  

	vector_Type[i] = vecPartType[i];
	vector_Origin[i] = vecPartOrig[i];

	if(vector_Type[i]==MCTruthPartClassifier::BkgElectron && vector_Origin[i]==MCTruthPartClassifier::PhotonConv){
	  g->matchedTrackGenParticle.push_back(vecTruthPart[i]);
	  std::pair<ParticleType,ParticleOrigin> electron= m_TruthClassifierTool->checkOrigOfBkgElec(vecTruthPart[i]);
	  vector_phtType[i] = electron.first;
	  vector_phtOrigin[i] = electron.second;
	} 

	if (vecTruthPart[i]){
	  //needed to get rigth mother pointer 
          /*std::pair<ParticleType,ParticleOrigin> truthPart=*/ m_TruthClassifierTool->particleTruthClassifier(vecTruthPart[i]);
	  vector_pdgID[i] = vecTruthPart[i]->pdg_id(); 
	  vector_pdgMOTHER[i]  = m_TruthClassifierTool->getMotherPDG();
	  //vector_barcodeMOTHER[i]  = m_TruthClassifierTool->getMotherBarcode();  
	  vector_barcodeMOTHER[i]  = m_TruthClassifierTool->getMotherPDG();  //??????!
	  vector_pdgGrandMOTHER[i] = m_TruthClassifierTool->getPhotonMotherPDG();  // this is work only  for conversion electons 	
	} else {
	  vector_pdgID[i] = pdgID;
	  vector_pdgMOTHER[i] = pdgMOTHER;
	  vector_barcodeMOTHER[i] = pdgMOTHER;
	  vector_pdgGrandMOTHER[i] = pdgGrandMOTHER;
	}

      }  //end loop on tracks


    } // end if g->convVx
    else { // recover information for single track conversions 
      if (g->trackParticle()) { 

	// Using trackClassifier from Oleg's Tool One single track conversion
	tpTruthResult=m_TruthClassifierTool->particleTruthClassifier( g->trackParticle() );
	const HepMC::GenParticle* thePart1= m_TruthClassifierTool->getGenPart();
	vector_Type[0] = tpTruthResult.first;
	vector_Origin[0] = tpTruthResult.second;
	
	if( vector_Type[0]==MCTruthPartClassifier::BkgElectron && vector_Origin[0]==MCTruthPartClassifier::PhotonConv ){
	  std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> electron= m_TruthClassifierTool->checkOrigOfBkgElec(thePart1);
	  vector_phtType[0] = electron.first;
	  vector_phtOrigin[0] = electron.second;  	    
        }
   
	if ( thePart1 ){
	  vector_pdgID[0] = thePart1->pdg_id();
	  vector_pdgMOTHER[0] = m_TruthClassifierTool->getMotherPDG();
	  vector_barcodeMOTHER[0] = m_TruthClassifierTool->getMotherBarcode();
	  vector_pdgGrandMOTHER[0] = m_TruthClassifierTool->getPhotonMotherPDG();
	}

    }  // end g->trackParticle
      
    } // else 1-track conversion
  }// end if isConv
  
}
// ====================================================================================== 
#endif
