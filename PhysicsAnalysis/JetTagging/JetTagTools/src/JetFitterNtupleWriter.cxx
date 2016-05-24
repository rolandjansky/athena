/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterNtupleWriter.h
/// Package : BTagTools 
/// Author  : Giacinto Piacquadio
/// Created : March 2007
///
/// DESCRIPTION:
///
/// This class writes an ntuple out which is needed to calibrate the new BTagging algorithm 
/// based on JetFitter.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GaudiKernel/ITHistSvc.h"

//#include "VxJetVertex/VxVertexOnJetAxis.h"
//#include "VxVertex/VxTrackAtVertex.h"
//#include "VxJetVertex/VxJetCandidate.h"

#include <TMath.h>
#include <TTree.h>

#include "CLHEP/Vector/LorentzVector.h"

#include "JetTagTools/JetFitterNtupleWriter.h"

#include "JetTagInfo/JetFitterTagInfo.h"
//#include "JetTagInfo/IJetFitterTagInfo.h"

//#include "JetEvent/Jet.h"

namespace Analysis {
  
  
  JetFitterNtupleWriter::JetFitterNtupleWriter(const std::string& name,
					       const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p)
  {
    declareInterface<IJetFitterNtupleWriter>(this);
  }

  JetFitterNtupleWriter::~JetFitterNtupleWriter() {
    //you have to delete what you created...


    //    don't delete the trees ! you don't own them!
    /*    
    std::vector<std::pair<std::string,TTree*> >::iterator begin=m_tree.begin();
    std::vector<std::pair<std::string,TTree*> >::iterator end=m_tree.end();
    
    for (  std::vector<std::pair<std::string,TTree*> >::iterator iter=begin;iter!=end;
	   ++iter ) {
      if ((*iter).second!=0) {
	delete (*iter).second;
	(*iter).second=0;
      }
    }
    */
    
    std::vector<std::pair<std::string,JetFitterVariables*> >::iterator beginvar=m_variables.begin();
    std::vector<std::pair<std::string,JetFitterVariables*> >::iterator endvar=m_variables.end();
    
    for (  std::vector<std::pair<std::string,JetFitterVariables*> >::iterator iter=beginvar;iter!=endvar;
	   ++iter ) {
      if ((*iter).second!=0) {
	delete (*iter).second;
	(*iter).second=0;
      }
    }
    
  }

  
  StatusCode JetFitterNtupleWriter::initialize() {
    
    StatusCode sc=service( "THistSvc", m_histoSvc );
    if( sc.isSuccess() ) {
      ATH_MSG_INFO(name() << ": HistoSvc loaded successfully.");
    } else {
      ATH_MSG_ERROR(name() << ": HistoSvc could NOT bo loaded.");
      return sc; 
    }
    
    ATH_MSG_INFO(" Initialization of JetFitterNtupleWriter succesfull");
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterNtupleWriter::finalize() {    
    ATH_MSG_INFO(" Finalization of JetFitterNtupleWriter succesfull");
    return StatusCode::SUCCESS;
  }
  


 StatusCode JetFitterNtupleWriter::bookNtuple(const std::string & jetAuthor) {

   ATH_MSG_DEBUG(" Trying to register ntuple for jetAuthor " << jetAuthor);


  std::vector<std::pair<std::string,TTree*> >::iterator begin=m_tree.begin();
  std::vector<std::pair<std::string,TTree*> >::iterator end=m_tree.end();


  TTree* newTree(0);
  for (  std::vector<std::pair<std::string,TTree*> >::iterator iter=begin;iter!=end;
	 ++iter ) {
     
    if (jetAuthor==(*iter).first) {
      ATH_MSG_DEBUG(" Tree with key " << jetAuthor << " is already there. Using the same tree");
      newTree=(*iter).second;
      if (newTree==0) {
	ATH_MSG_WARNING(" The ntuple is an empty pointer ");
      } 
    }
  }

  ATH_MSG_DEBUG("Already registered with pointer " << newTree);

  if (newTree==0) {
    newTree=new TTree(("JetFitterTag"+jetAuthor).c_str(),"Calibration variables for JetFitter");
    m_tree.push_back(std::pair<std::string,TTree*>(jetAuthor,newTree));
    JetFitterVariables* newVariables=new JetFitterVariables();
    
    newTree->Branch("nVTX",&newVariables->nVTX,"nVTX/I");
    newTree->Branch("nTracksAtVtx",&newVariables->nTracksAtVtx,"nTracksAtVtx/I");
    newTree->Branch("nSingleTracks",&newVariables->nSingleTracks,"nSingleTracks/I");
    newTree->Branch("energyFraction",&newVariables->energyFraction,"energyFraction/D");
    newTree->Branch("mass",&newVariables->mass,"mass/D");
    newTree->Branch("significance3d",&newVariables->significance3d,"significance3d/D");
    newTree->Branch("flavour",&newVariables->flavour,"flavour/I");
    newTree->Branch("deltaphi",&newVariables->deltaphi,"deltaphi/D");
    newTree->Branch("deltaeta",&newVariables->deltaeta,"deltaeta/D");
    newTree->Branch("jetpt",&newVariables->jetpt,"jetpt/D");
    newTree->Branch("jeteta",&newVariables->jeteta,"jeteta/D");

   m_variables.push_back(std::pair<std::string,JetFitterVariables*>(jetAuthor,newVariables));
  }

  ATH_MSG_DEBUG("Now registered with pointer " << newTree);

  //registering the tree
  StatusCode sc = m_histoSvc->regTree("/RefFile/JetFitter/"+jetAuthor+"/JetFitterTag",newTree);
  if(!sc)
    {
      ATH_MSG_ERROR("initialize() Could not register the tree");
      return sc;
    }
  
  return StatusCode::SUCCESS;
  
 }
  
  StatusCode JetFitterNtupleWriter::fillNtuple(const std::string & hypothesis,
					       const std::string & jetAuthor,
					      const IJetFitterTagInfo & jetFitterTagInfo,
					      const xAOD::Jet & myJet) {

  if (hypothesis!="") {
    
    TTree* myTTree(0);

    std::vector<std::pair<std::string,TTree*> >::iterator begin=m_tree.begin();
    std::vector<std::pair<std::string,TTree*> >::iterator end=m_tree.end();
    
    for (  std::vector<std::pair<std::string,TTree*> >::iterator iter=begin;iter!=end;
	   ++iter ) {
      if ((*iter).first==jetAuthor) {
	myTTree=(*iter).second;
      }
    }
    
    JetFitterVariables* myVariables(0);
    std::vector<std::pair<std::string,JetFitterVariables*> >::iterator beginvar=m_variables.begin();
    std::vector<std::pair<std::string,JetFitterVariables*> >::iterator endvar=m_variables.end();
    
    for (  std::vector<std::pair<std::string,JetFitterVariables*> >::iterator iter=beginvar;iter!=endvar;
	   ++iter ) {
      if ((*iter).first==jetAuthor) {
	myVariables=(*iter).second;
      }
    }
    
    if (myVariables==0||myTTree==0) {
      ATH_MSG_WARNING(" No Tree for the jetAuthor " << jetAuthor << ". Cannot fill ntuple; skipping this jet... ");
      return StatusCode::FAILURE;
    }
    
    myVariables->nVTX=jetFitterTagInfo.nVTX();
    myVariables->nTracksAtVtx=jetFitterTagInfo.nTracksAtVtx();
    myVariables->nSingleTracks=jetFitterTagInfo.nSingleTracks();
    myVariables->energyFraction=jetFitterTagInfo.energyFraction();
    myVariables->mass=jetFitterTagInfo.mass();
    myVariables->significance3d=jetFitterTagInfo.significance3d();
    myVariables->deltaphi=jetFitterTagInfo.deltaphi();
    myVariables->deltaeta=jetFitterTagInfo.deltaeta();
    myVariables->flavour=0;
    myVariables->jetpt=myJet.pt();
    myVariables->jeteta=myJet.eta();
    
    ATH_MSG_DEBUG(" nvtx " << myVariables->nVTX << 
      " ntracksatvtx " << myVariables->nTracksAtVtx << " nsingletracks " << myVariables->nSingleTracks << 
      " energyfraction " << myVariables->energyFraction << " myVariables->mass " << myVariables->mass << 
      " myVariables->significance3d " << myVariables->significance3d << " myVariables->deltaphi " << 
      myVariables->deltaphi << " myVariables->deltaeta " << myVariables->deltaeta);

    if (hypothesis=="bottom") {
      myVariables->flavour=5;
    } else if (hypothesis=="charm") {
      myVariables->flavour=4;
    } else if (hypothesis=="light") {
      myVariables->flavour=1;
    } else {
      myVariables->flavour=-1;
    }
    

    ATH_MSG_DEBUG(" the number of entries is: " << myTTree->GetEntries());
    
    myTTree->Fill();

  }//end of hypothesis !=""

  return StatusCode::SUCCESS;

 }
  
}//end Analysis namespace
