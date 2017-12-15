/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisJiveXML/TruthParticleRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "McParticleEvent/TruthParticleContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TruthParticleRetriever::TruthParticleRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("CompositeParticle")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "SpclMC","StoreGate key for TruthParticles"); 
    declareProperty("TruthStatus", m_truthStatus = 2,"Truth status cut, currently not applied"); // currently unused
    declareProperty("TruthMaximumPdgId", m_truthMaximumPdgId = 40,
        "Maximum Truth PDG-ID, default:< 40");
    declareProperty("TruthPtCut", m_truthPtCut = 10.,"pT cut on Truth, default 10 GeV");
    declareProperty("SkimTruth", m_skimTruth = true,"Some criteria to skim most important Truth info ");
  }

  //--------------------------------------------------------------------------

  StatusCode TruthParticleRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    const TruthParticleContainer *truthCont = NULL;
    
    if ( !evtStore()->contains<TruthParticleContainer>(m_sgKey)){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No TruthParticle found in SG at "
					      << m_sgKey << endmsg;
      return StatusCode::SUCCESS;
    }
    if ( evtStore()->retrieve(truthCont,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "TruthParticle retrieval from SG failed "
          << m_sgKey << endmsg;
      return StatusCode::SUCCESS;
    } 
    int nTruth = truthCont->size();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving TruthParticles with size " << nTruth << endmsg;

//code from: PhysicsAnalysis/HiggsPhys/HiggsToFourLeptons/src/H4lElectronPerformanceTool.cxx

    DataVect pt; pt.reserve(nTruth);
    DataVect phi; phi.reserve(nTruth);
    DataVect eta; eta.reserve(nTruth);
    DataVect typeEV; typeEV.reserve(nTruth);
    DataVect label; label.reserve(nTruth);
    DataVect typeLabelStr; typeLabelStr.reserve(nTruth);
    DataVect pdgId; pdgId.reserve(nTruth);
    DataVect dataType; dataType.reserve(nTruth);

    TruthParticleContainer::const_iterator mcpartItr  = truthCont->begin();
    TruthParticleContainer::const_iterator mcpartItrE = truthCont->end();

    std::string truthLabels;
    std::string statusList="_";
    std::string typeLabel="n_a";
    int pdgId2;
    int motherPdgId;
    int childPdgId;
    int countTruth = 1;
    bool samePdgIdFlag = false;
    bool initialProcessFlag = false;
    bool protectedParticleFlag = false; 

    for(; (mcpartItr != mcpartItrE) ; ++mcpartItr) { // loop over TruthParticle
     samePdgIdFlag = false;
     initialProcessFlag = false;
     protectedParticleFlag = false;
     statusList += (*mcpartItr)->status() + "_";

     if ( (*mcpartItr)->et()/CLHEP::GeV < m_truthPtCut ){ continue; }
/*
     if ((*mcpartItr)->hasPdgId() == false) { continue; }
///// status is pythia only. May fail for other MCs     
     if ( (*mcpartItr)->status() != 2 ){ continue; } 
*/     
     if ( abs( (*mcpartItr)->pdgId()) > m_truthMaximumPdgId ){ continue; }

     pdgId2 = (*mcpartItr)->pdgId();

     // important particles are protected (can come from gamma etc)
     if (( abs(pdgId2) == 24 ) || ( abs(pdgId2) == 5 ) ||  
          ( abs(pdgId2) == 6 ) || ( abs(pdgId2) == 23 ) ||  
          ( abs(pdgId2) == 36 ) || ( abs(pdgId2) == 37 ) || 
          ( abs(pdgId2) == 25 ) ){ 
	 protectedParticleFlag = true; 
     }

     // particle has to originate from b,t,W,Z,H,A,H+- only	 
     for ( unsigned int iMother = 0; iMother < (*mcpartItr)->nParents(); ++iMother ) { 
      bool motherHasPdgId = (*mcpartItr)->mother(iMother)->hasPdgId(); 
       if ( motherHasPdgId ){ 
         motherPdgId = (*mcpartItr)->mother(iMother)->pdgId();       
         if ( motherPdgId == pdgId2 ){ samePdgIdFlag = true; }
         if (( abs(motherPdgId) == 24 ) || ( abs(motherPdgId) == 5 ) ||  
          ( abs(motherPdgId) == 6 ) || ( abs(motherPdgId) == 23 ) ||  
          ( abs(motherPdgId) == 36 ) || ( abs(motherPdgId) == 37 ) || 
	  ( abs(pdgId2) == 25 ) ){ 
	      initialProcessFlag = true; 
	 }
       }
     } 
     // jump to next particle (ignore) if PdgId is same as mother 
     if (m_skimTruth){ // some skimming criteria for simpler display of Truth
       if ( !initialProcessFlag && !protectedParticleFlag ){ 
	 //	 if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Particle rejected: not initial process " << endmsg;
          continue; 
       } 
     }
     if ( samePdgIdFlag ){  
       //         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Particle rejected: Same PdgId " << endmsg;
         continue;
     }   

     pt.push_back( DataType((*mcpartItr)->et()/CLHEP::GeV ) );
     phi.push_back( DataType((*mcpartItr)->phi() ) );
     eta.push_back( DataType((*mcpartItr)->eta() ) );
     dataType.push_back( DataType( (*mcpartItr)->dataType() ));

// if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " TruthParticle No:" << countTruth << " pt:" 
//                << (*mcpartItr)->et()/CLHEP::GeV << endmsg;
	  
     pdgId.push_back( DataType( pdgId2 ));

     typeLabel = "n_a";
     if( abs(pdgId2) == 11) typeLabel = "Electron";
     if( abs(pdgId2) == 12) typeLabel = "NeutrinoElectron";
     if( abs(pdgId2) == 13) typeLabel = "Muon";
     if( abs(pdgId2) == 14) typeLabel = "NeutrinoMuon";
     if( abs(pdgId2) == 15) typeLabel = "Tau";
     if( abs(pdgId2) == 16) typeLabel = "NeutrinoTau";
     if( pdgId2 == 6) typeLabel = "Top";  
     if( pdgId2 == -6) typeLabel = "AntiTop";  
     if( pdgId2 == 5) typeLabel = "Bottom";
     if( pdgId2 == -5) typeLabel = "AntiBottom";
     if( pdgId2 == 22) typeLabel = "Photon";
     if( pdgId2 == 23) typeLabel = "Z0";
     if( pdgId2 == 224) typeLabel = "Wplus";
     if( pdgId2 == -24) typeLabel = "Wminus";
     if( pdgId2 == 36) typeLabel = "A0";
     if( pdgId2 == 25) typeLabel = "Higgs0";
     if(( abs(pdgId2) >= 1) && ( abs(pdgId2) <= 4)) typeLabel = "LightQuark"; 
   
     typeEV.push_back( DataType( typeLabel ) );

     truthLabels = "No" + DataType( countTruth ).toString() +"_Pdg="
                   + DataType( (*mcpartItr)->pdgId() ).toString()
                 + "_stat=" + DataType( (*mcpartItr)->status() ).toString(); 
                 + "_nParents=" + DataType( (*mcpartItr)->nParents() ).toString()
                 + "_nDecay=" + DataType( (*mcpartItr)->nDecay() ).toString(); 

     for ( unsigned int iChild = 0; iChild < (*mcpartItr)->nDecay(); ++iChild ) { 
        bool childHasPdgId = (*mcpartItr)->child(iChild)->hasPdgId();
        if ( childHasPdgId ){        
           childPdgId = (*mcpartItr)->child(iChild)->pdgId();
           truthLabels += "_CNo=" + DataType(iChild+1).toString() + "_CPdgId=" + 
	                  DataType( childPdgId ).toString() + "_CET=" + 
			  DataType( (*mcpartItr)->child(iChild)->et()/CLHEP::GeV ).toString();  
	   if ( (*mcpartItr)->child(iChild)->et()/CLHEP::GeV <= m_truthPtCut ){ continue; }
           if (childPdgId == -11){
              truthLabels += "_IntoElectron"; 
              typeLabel += "_IntoElectron"; 
           }
           if (childPdgId == 11){
              truthLabels += "_FromPositron"; 
              typeLabel += "_FromPositron"; 
           }
           if (childPdgId == 13){
              truthLabels += "_IntoMuonMinus"; 
              typeLabel += "_IntoMuonMinus"; 
           }
           if (childPdgId == -13){
	     truthLabels += "_IntoMuonPlus";
	     typeLabel += "_IntoMuonPlus";
           }
           if (childPdgId == 15){
	     truthLabels += "_IntoTauMinus";
	     typeLabel += "_IntoTauMinus";
           }
           if (childPdgId == -15){
             truthLabels += "_IntoTauPlus";
             typeLabel += "_IntoTauPlus";
           }
           if (childPdgId == 5){
              truthLabels += "_IntoBottom"; 
              typeLabel += "_IntoBottom"; 
           }
           if (childPdgId == -5){
              truthLabels += "_IntoAntiBottom"; 
              typeLabel += "_IntoAntiBottom"; 
           }
           if (childPdgId == 6){
              truthLabels += "_IntoTop"; 
              typeLabel += "_IntoTop"; 
           }
           if (childPdgId == -6){
              truthLabels += "_IntoAntiTop"; 
              typeLabel += "_IntoAntiTop"; 
           }
	   if ( ( abs(childPdgId) >= 1) && ( abs(childPdgId) <=4) ) {
             truthLabels += "_IntoLightQuark";
	     typeLabel += "_IntoLightQuark";
           }
           if (childPdgId == -24){
              truthLabels += "_IntoWminus"; 
              typeLabel += "_IntoWminus"; 
           }
           if (childPdgId == 24){
              truthLabels += "_IntoWplus"; 
              typeLabel += "_IntoWplus"; 
           }
           if (childPdgId == 25){
              truthLabels += "_IntoHiggs0"; 
              typeLabel += "_IntoHiggs0"; 
           }
           if (childPdgId == 23){
              truthLabels += "_IntoZ0"; 
              typeLabel += "_IntoZ0"; 
           }
        } 			   
     }
     for ( unsigned int iMother = 0; iMother < (*mcpartItr)->nParents(); ++iMother ) { 
        bool motherHasPdgId = (*mcpartItr)->mother(iMother)->hasPdgId(); 
        if ( motherHasPdgId ){ 
	   int motherPdgId = (*mcpartItr)->mother(iMother)->pdgId();       
           truthLabels += "_MNo=" + DataType(iMother+1).toString() + "_MPdgId=" + 
	                   DataType( motherPdgId ).toString(); 
	   if ( (*mcpartItr)->mother(iMother)->et()/CLHEP::GeV <= m_truthPtCut ){ continue; }
           if (motherPdgId == -24){
              truthLabels += "_FromWminus"; 
              typeLabel += "_FromWminus"; 
           }
           if (motherPdgId == 24){
              truthLabels += "_FromWplus"; 
              typeLabel += "_FromWplus"; 
           }
           if (motherPdgId == 25){
              truthLabels += "_FromHiggs0"; 
              typeLabel += "_FromHiggs0"; 
           }
           if (motherPdgId == 23){
              truthLabels += "_FromZ0"; 
              typeLabel += "_FromZ0"; 
           }
           if (motherPdgId == 6){
              truthLabels += "_FromTop"; 
              typeLabel += "_FromTop"; 
           }
           if (motherPdgId == -6){
              truthLabels += "_FromAntiTop"; 
              typeLabel += "_FromAntiTop"; 
           }
           if (motherPdgId == 5){
              truthLabels += "_FromBottom"; 
              typeLabel += "_FromBottom"; 
           }
           if (motherPdgId == -5){
              truthLabels += "_FromAntiBottom"; 
              typeLabel += "_FromAntiBottom"; 
           }
           if (motherPdgId == 36){
              truthLabels += "_FromA0"; 
              typeLabel += "_FromA0"; 
           }
           if (motherPdgId == 37){
              truthLabels += "_FromHiggsPlus"; 
              typeLabel += "_FromHiggsPlus"; 
           }
           if (motherPdgId == -37){
              truthLabels += "_FromHiggsMinus"; 
              typeLabel += "_FromHiggsMinus"; 
	   }
	} // end of hasMotherPdgId 
     } // end of MotherParticle loop
     countTruth++;
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TruthParticles label: " <<  DataType( truthLabels ).toString() << endmsg;
     label.push_back( DataType( truthLabels ).toString() );
     typeLabelStr.push_back( DataType( typeLabel ).toString() );
     
    //    log << MSG::DEBUG << "TruthParticles status: " << statusList << endmsg;
  } // end of TruthParticle Loop
    DataMap myDataMap;
      myDataMap["pt"] = pt;
      myDataMap["phi"] = phi;
      myDataMap["eta"] = eta;
      myDataMap["typeEV"] = typeEV;
//      myDataMap["label"] = label;
      myDataMap["label"] = typeLabelStr;
      myDataMap["pdgId"] = pdgId;
      myDataMap["dataType"] = dataType;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
