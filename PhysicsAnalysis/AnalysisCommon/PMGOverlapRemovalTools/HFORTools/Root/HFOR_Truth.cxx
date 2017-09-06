/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <math.h>

#include "boost/current_function.hpp"
#include <boost/any.hpp>

#include "boost/foreach.hpp"
#include "boost/property_tree/info_parser.hpp"
#include "boost/property_tree/xml_parser.hpp"

// Framework include(s):

#include "xAODBase/IParticle.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"


#include <HFORTools/HFOR_Truth.h>

using namespace std;
//==============================================================================
//HFOR_Truth Instantiation
//There is no default strategy, it's mandatory to chose either angularBased or
//jetBased overlap removal.
//Several bookkeeper maps are  provided to track the action of the tool
//==============================================================================
HFOR_Truth::HFOR_Truth() {
  m_debug = false;
  //You will need to configure the tool to the HFOR strategy you want
  angularBased_HFOR = false;
  jetBased_HFOR = false;

  m_matchCone = 0.4 ;
  m_jetBasedHFOR_pT_min = 5000.; //5 GeV is the lower limit of AntiKt4Jets reclustering
  m_jetBasedHFOR_eta_max = 4.5; 
  m_sampleType = HFORType::noType;

  //Helper to understand what each number means
  m_sampleNameMap[HFORType::isBB]   = "bb";
  m_sampleNameMap[HFORType::isCC]   = "cc" ;
  m_sampleNameMap[HFORType::isC]    = "c" ;
  m_sampleNameMap[HFORType::isLight]  = "light" ;
  m_sampleNameMap[HFORType::noType] = "unknown" ;

  m_sampleName =  m_sampleNameMap[m_sampleType] ;

  m_runConfigMap[HFORType::isLight].clear() ;
  m_runConfigMap[HFORType::isBB].clear() ;
  m_runConfigMap[HFORType::isCC].clear() ;
  m_runConfigMap[HFORType::isC].clear() ;

  m_runConfigFile = "" ;

  //Initialize the maps book-keepping information

  //Number of quarks identifyed/found
  m_Nquarks["FS"][4] = 0 ;
  m_Nquarks["FS"][5] = 0 ;

  m_Nquarks["ME"][4] = 0 ;
  m_Nquarks["ME"][5] = 0 ;

  m_Nquarks["GS"][4] = 0 ;
  m_Nquarks["GS"][5] = 0 ;

  m_Nquarks["MPI"][4] = 0 ;
  m_Nquarks["MPI"][5] = 0 ;

  //Number of classifications
  m_Nclass[HFORType::isBB]     = 0 ;
  m_Nclass[HFORType::isCC]     = 0 ;
  m_Nclass[HFORType::isC]      = 0 ;
  m_Nclass[HFORType::isLight]  = 0 ;
  m_Nclass[HFORType::noType]   = 0 ;

  //Vector of deltaR for each type

  m_dR["ME"][4].clear() ;
  m_dR["ME"][5].clear() ;

  m_dR["GS"][4].clear() ;
  m_dR["GS"][5].clear() ;

}
//==============================================================================

//==============================================================================
//HFOR_Truth Instantiation
//This method will navigate through the MC container, finds final state quarks
//and finds if overlaps are present. It will then call the configured strategy
//(jet or angular based removal) and return an "action" (keep the event which
//is now classified accordingly to HFORType or kill it)
//==============================================================================
HFORType HFOR_Truth::findOverlap(const xAOD::TruthEventContainer& truthEvent,
		                         const xAOD::JetContainer* jets) {

  unsigned int nPart = 0 ;
  unsigned int tEvent = 0 ;

  unsigned int quarks_4 = 0 ;
  unsigned int quarks_5 = 0 ;

  HFORType action = HFORType::noType;

  //If this sample is not Alpgen+Pythia, do nothing
  if ( m_sampleType == HFORType::noType ) {
    //This message should be debug ...
    //Warning(BOOST_CURRENT_FUNCTION, "This is not a valid AlpgenPythia sample, do nothing") ;
    return action ;
  }

  //Navigates through the MC truth container and try to find if overlaps are present

  std::map <int, std::vector<const xAOD::TruthParticle* > > fsQuarksMap ;

  //Scan the container and find the final state quarks
  for (const auto& truth : truthEvent ) {
    //std::cout << "---> " <<  truth->crossSection() << "  " << truth->crossSectionError() << std::endl ;
    if (truth != nullptr) {
      //nVert = truth->nTruthVertices();
      nPart = truth->nTruthParticles();
      for (unsigned int i = 0; i < nPart; i++) {
        const xAOD::TruthParticle* truthParticle   = truth->truthParticle(i);
        if (truthParticle != nullptr) {
          int pdgId = truthParticle->pdgId() ;
          if (fabs(pdgId) == 4) {
            quarks_4 ++ ;
            if (is_FinalState(truthParticle) ) {
              fsQuarksMap[4].push_back(truthParticle) ;
            }
          }
          if (fabs(pdgId) == 5) {
            quarks_5 ++ ;
            if ( is_FinalState(truthParticle) ) {
              fsQuarksMap[5].push_back(truthParticle) ;
            }
          }
        }
      }
    }
    tEvent++ ;
  }

  //Book keeping
  m_Nquarks["FS"][4] += fsQuarksMap[4].size() ;
  m_Nquarks["FS"][5] += fsQuarksMap[5].size() ;

  //std::cout << "==============> " << quarks_4 << quarks_5 << std::endl ;
  //Now scan the final state particle container to identify the type of action to take
  //It return an string that could decorate the original container for further processing

  findHFQuarks(fsQuarksMap) ;

  if(angularBased_HFOR){ //Perform an angular based removal action
    action = angularBasedRemoval() ;
  } else if (jetBased_HFOR) {

    if(jets) action = jetBasedRemoval(jets);
    else Warning("HFOR_Truth::findOverlap()","No JetContainer passed to jet-based HFOR!");
    
  } else{
    Error("HFOR_Truth::findOverlap()","No HFOR type has been specified!");
  }

  if(m_debug)	{
    for (auto& cc: m_qq ) {
      std::string cc_name = cc.first ;
      for (auto& qmap: cc.second) {
        int id = qmap.first ;
        std::cout << " ***** ----> " << cc_name << "  " << id << "  " <<  qmap.second.size() << std::endl ;
      }
    }
  }

  //std::cout << "!!!!! " << fsQuarksMap[4].size() << "  " << fsQuarksMap[5].size() << std::endl ;
  return (action) ;
}
//==============================================================================

//==============================================================================
// This method will determine the type of action (keep or kill the event) using
// b and c truth jets.
//==============================================================================
HFORType HFOR_Truth::jetBasedRemoval( const xAOD::JetContainer* jets) {

  //If this sample is not Alpgen+Pythia, do nothing
  if ( m_sampleType == HFORType::noType ) {		//This message should be debug ...
    Warning(BOOST_CURRENT_FUNCTION, "This is not a valid AlpgenPythia sample, do nothing") ;
    return (HFORType::noType) ;
  }

  if(m_debug) std::cout<<std::endl<<"jetBased HFOR"<<std::endl;
  bool killEvent = false;
  if(!jets) Error("HFOR_Truth::jetBasedRemoval()","Couldn't jet container! It is needed for JetBased HFOR!");

  //Load the quark vectors (these are always the same in the event)
  std::vector <const xAOD::TruthParticle*> B_ME_v = m_qq["ME"][5];
  std::vector <const xAOD::TruthParticle*> C_ME_v = m_qq["ME"][4];
  std::vector <const xAOD::TruthParticle*> B_GS_v = m_qq["GS"][5];
  std::vector <const xAOD::TruthParticle*> C_GS_v = m_qq["GS"][4];       
  
  for (const xAOD::Jet * jet : *jets) {
    const std::string labelGhostB  = "GhostBQuarksFinal", labelGhostC  = "GhostCQuarksFinal";
    std::vector<const xAOD::TruthParticle *> ghostB, ghostC;
    if( !jet->getAssociatedObjects<xAOD::TruthParticle>( labelGhostB, ghostB   ) ||
        !jet->getAssociatedObjects<xAOD::TruthParticle>( labelGhostC, ghostC   ) ){
      Error("HFOR_Truth::jetBasedRemoval()","Couldn't access truth particleas associated to HF quarks!");
      return HFORType::noType;
    }
    
    int nGhostMatchedBQuarks = ghostB.size();
    int nGhostMatchedCQuarks = ghostC.size();   
    int nB_ME = 0, nC_ME = 0, nB_GS = 0, nC_GS = 0; 

    if(nGhostMatchedCQuarks == 0 && nGhostMatchedBQuarks == 0) continue;//do nothing for jets without HF hadrons!
    if( jet->pt() < m_jetBasedHFOR_pT_min || fabs(jet->eta()) > m_jetBasedHFOR_eta_max ) continue;

    //Look only in region where to use only GS only for 2 quarks in 1 jet and ME for 2 quarks in 2 jets 
      
    for (const xAOD::TruthParticle * part : ghostB ) {
      if(!part) Error("HFOR_Truth::jetBasedRemoval()","No BQuark particles but ghost-matching exists!! It should not happen!");
      for (const xAOD::TruthParticle * part_B_ME : B_ME_v ) 
        if( part->barcode() == part_B_ME->barcode()) nB_ME++;
      for (const xAOD::TruthParticle * part_B_GS : B_GS_v ) 
        if( part->barcode() == part_B_GS->barcode()) nB_GS++;
    }          
    
    for (const xAOD::TruthParticle * part : ghostC ) {
      if(!part) Error("HFOR_Truth::jetBasedRemoval()","No CQuark particles but ghost-matching exists!! It should not happen!");
      for (const xAOD::TruthParticle * part_C_ME : C_ME_v ) 
        if( part->barcode() == part_C_ME->barcode()) nC_ME++;
      for (const xAOD::TruthParticle * part_C_GS : C_GS_v ) 
        if( part->barcode() == part_C_GS->barcode()) nC_GS++;
    }      
    
    //Decide if you want to keep or not the event on the basis of this jet

    if(m_sampleType == HFORType::isLight){
      if( nB_GS == 1 || nC_GS == 1) killEvent |= true;
      else killEvent |= false;
    }
    
    if(m_sampleType == HFORType::isBB ){
      if( nB_ME == 2 ) killEvent |= true;         
      else killEvent |= false;
    }
    
    if(m_sampleType == HFORType::isC || m_sampleType == HFORType::isCC){
      if( nC_ME == 2 ) killEvent |= true; 
      else killEvent |= false;
    }
      
    if(m_debug) {//All debug informaiton I may need...
      std::cout<<"Looking j: pt= "<<jet->pt()/1000.<<" eta= "<<jet->eta()<<" phi= "<<jet->phi()
               <<" FinalCount B "<<jet->auxdata<int>("GhostBQuarksFinalCount")
               <<" FinalCount C  "<<jet->auxdata<int>("GhostCQuarksFinalCount")<<std::endl;
      
      std::cout<<"nB_ME = "<<nB_ME<<" nC_ME = "<<nC_ME<<" nB_GS = "<<nB_GS<<" nC_GS = "<<nC_GS<<std::endl;
      for (const xAOD::TruthParticle * part : ghostB ) {
        std::cout<<"ghost b status: "<<part->status()<<" pdgId: "<<part->pdgId()<<" m:"<<part->m()<<" barcode "<<part->barcode()
                 <<" Vtx barcode: "<<part->prodVtx()->barcode()<<" n parent: "<<part->nParents()<<" n children: "<<part->nChildren()<<std::endl;          
        if(part->nParents())
          std::cout<<" mother status: "<<part->parent()->status()<<" mother barcode: "<<part->parent()->barcode()
                   <<" mother pdgID: "<<part->parent()->pdgId()<<" mother mothers: "<<part->parent()->nParents()<<std::endl;         
      }
      
      for (const xAOD::TruthParticle * part : ghostC ) {
        if(part){
          std::cout<<"ghost c status: "<<part->status()<<" pdgId: "<<part->pdgId()<<" m:"<<part->m()<<" barcode "<<part->barcode()
                   <<" n parent: "<<part->nParents()<<" n children: "<<part->nChildren()<<std::endl;
          if(part->nParents())
            std::cout<<"mother status: "<<part->parent()->status()<<" mother barcode: "<<part->parent()->barcode()
                     <<" Vtx barcode: "<<part->prodVtx()->barcode()<<" mother pdgID: "<<part->parent()->pdgId()<<" mother mothers: "<<part->parent()->nParents()<<std::endl;           
        } else {
          std::cout<<"WARNING: MISSING LINK!!!"<<std::endl;
        }
      }
    }
  }
  if(m_debug) std::cout<<"KILL event? "<<killEvent<<std::endl;
  if(killEvent) return HFORType::kill ;

  return (HFORType::noType) ;
}
//==============================================================================

//==============================================================================
// Here we take all b and c quarks in the MC event tree and find if it is a
// final state quark
//==============================================================================
bool HFOR_Truth::is_FinalState(const xAOD::TruthParticle* bcQuark) {
  //This checks if the quark passed is final state or not
  //TODO: check that this particle is really a b/c quark, check that it is not null

  const xAOD::TruthParticle * father = nullptr ;
  const xAOD::TruthParticle * son    = nullptr ;

  unsigned int nChildren = bcQuark->nChildren() ;
  unsigned int nParents  = bcQuark->nParents() ;

  bool isFromHadron = false ;
  bool iscFromb = false ;
  //+++++++++NB: THIS CHECK ALSO REMOVES B/C ORIGINATING FROM THE INIT. STATE PROTONS 
  // (MAYBE MPI PARTICLES) IS THIS FINE? I THINK IT WORKS BECAUSE THEY SHOULD NOT BE HFOR-ED
  // BUT I AM NOT SURE....
  for (unsigned int i=0; i<nParents; i++) {
    father = bcQuark->parent(i) ;
    if(m_debug)	std::cout << "  Father PDGID " << father->pdgId() << " (quark is " << bcQuark->pdgId() << ")" << std::endl ;
    isFromHadron = (isFromHadron | father->isHadron() ) ;
    if ( is_c(bcQuark) ) {
      iscFromb = (iscFromb | is_b(father) ) ;
    }
  }
  if (isFromHadron || iscFromb) {
    return (false) ;
  }

  for (unsigned int i=0; i<nChildren; i++) {
    son = bcQuark->child(i) ;
    if (son != nullptr) {
      if ( is_c(son) | is_b(son) ) {
        return (false) ;
      }
    } else {
      return (false) ;
    }
  }

  return (true) ;
}
//==============================================================================

//==============================================================================
// Here we will take the final state quarks found in this event and classify
// them as from Matrix Element, Gluon Spliting PDF or MPI. Containers are filled
// with quarks classified as ME or GS for further check on the fase space they
// occupy
//==============================================================================
bool HFOR_Truth::findHFQuarks(const std::map <int,
                              std::vector<const xAOD::TruthParticle* > > fsQuarksMap)  {

  //What I return is a map containing a map of vectors  GS quarks and ME quarks
  m_qq.clear() ;	//VERY IMPORTANT

  int fsq_pdgId = -999 ;
  int fsq_status = 0 ;

  int pdgId = 0 ;
  int vtxBarcode = 0 ;
  int level = 0 ;
  int nid = 0 ;

  unsigned int nParents = 0 ;

  bool hasAncestor = false ;
  bool hasProton   = false ;
  bool hasW        = false ;
  bool hasTop      = false ;
  bool hasHF       = false ;

  bool isME        = false ;


  bool bc34 = false ;


  std::vector <const xAOD::TruthParticle*> ME ;
  std::vector <const xAOD::TruthParticle*> PDF ;

  std::map< int, std::vector <const xAOD::TruthParticle*> > xME  ;
  std::map< int, std::vector <const xAOD::TruthParticle*> > xPDF ;

  std::map< int, std::vector <const xAOD::TruthParticle*> > MPI ;
  std::map< int, std::vector <const xAOD::TruthParticle*> > GS  ;
  std::map< int, std::vector <const xAOD::TruthParticle*> > ME_PDF ;
  std::map< int, std::vector <const xAOD::TruthParticle*> > UFO ;

  const xAOD::TruthParticle* father = nullptr ;
  //const xAOD::TruthParticle* son    = nullptr ;
  const xAOD::TruthParticle* ancestor    = nullptr ;

  const xAOD::TruthVertex * prodVtx = nullptr;
  const xAOD::TruthVertex * pvtx34   = nullptr;

  std::map<std::string, bool> tipo ;


  // 1) Loop and find fs quarks with status 3

  for (auto& fsQuarks: fsQuarksMap ) {
    fsq_pdgId = fsQuarks.first ;
    for (auto& bcQuark: fsQuarks.second) {
      fsq_status = bcQuark->status() ;
      if (fsq_status == 3) {
        if ( bcQuark->nChildren() == 0 ) {
          //This is a Matrix Element quark
          ME.push_back(bcQuark) ;

          for (unsigned int i=0; i<bcQuark->nParents(); i++) {
            //If one of the parents has status = 3 and is the same flavor of the particle
            //--> the PARENT will be a PDF parton
            father = bcQuark->parent(i) ;
            if ( (father->absPdgId() == fsq_pdgId) && (father->status() == 3) ) {
              PDF.push_back(father) ;
            }
          }
        }
        else {
          //This is a PDF Quark
          PDF.push_back(bcQuark) ;
        }
      }
    }
  }

  // 2) Loop again and find fs quarks with status != 3

  for (auto& fsQuarks: fsQuarksMap ) {
    fsq_pdgId = fsQuarks.first ;
    for (auto& bcQuark: fsQuarks.second) {
      fsq_status = bcQuark->status() ;
      if (fsq_status != 3) {
        //recursive search through ancestors, as xAOD cannot give a list of
        //particle ancestors ...
        level = 0 ;
        ancestor = bcQuark ;

        hasAncestor = true ;
        hasProton   = false ;
        hasW        = false ;
        hasTop      = false ;
        hasHF       = false ;

        while (hasAncestor) {
          // Check if the ancestor :
          // 1) The ancestor is NOT SAME FLAVOR of bcQuark :
          // 	- check if Ancestor is a proton
          //     - check if Ancestor is a top
          //     - check if Ancestor is a W
          //     - if bcQuark is c, check if Ancestor is a b or a b-Hadron -> STOP NAVIGATION
          //     - if bcQuark is b, check if Ancestor is a b-Hadron -> STOP NAVIGATION
                  
          // 2) The ancestor IS SAME FLAVOR of bcQuark :
          //     - Do nothing, as this is from b/c or ME/PDF parent
                                    
          nParents = ancestor->nParents() ;
          if ( nParents != 0 ) {
            //FIXME: Should loop over all parents
            ancestor = ancestor->parent(0) ;
            level++ ;
          }
          else hasAncestor = false ;

          if (level != 0) {
            tipo = checkAncestor(ancestor,bcQuark) ;
            if ( tipo["bastard"] or tipo["cFromb"] or tipo["bFromb"] ) {
              hasAncestor = false ;
            }
            else {
              hasProton |= tipo["proton"] ;
              hasW      |= tipo["W"] ;
              hasTop    |= tipo["top"] ;
              hasHF     |= (tipo["cFromb"] | tipo ["bFromb"]) ;
            }
          }
        }

        if ( (level == 1) & hasProton ) {
          //This is a MPI
          MPI[fsq_pdgId].push_back(bcQuark) ;
        }
        if ( (level == 2) & hasProton ) {
          if ( (ME.size() + PDF.size()) == 0 ) {
            //This comes from GS
            GS[fsq_pdgId].push_back(bcQuark) ;
          }
          else {
            //ME or PDF parton
            ME_PDF[fsq_pdgId].push_back(bcQuark) ;

            //Check the production vertex
            prodVtx = bcQuark->prodVtx() ;
            pdgId   = bcQuark->pdgId() ;
            vtxBarcode = prodVtx->barcode() ;
            bc34 = ( (vtxBarcode == -3) | (vtxBarcode == -4) ) ;
            pvtx34 = prodVtx ;
            if (! bc34) {
              //Navigate the ancestor, stop if find a vtxBarcode == -3 or -4
              hasAncestor = true ;
              ancestor = bcQuark ;
              while (hasAncestor) {
                nParents = ancestor->nParents() ;
                if (nParents != 0) {
                  for (unsigned int npp=0; npp<nParents; npp++) {
                    ancestor = ancestor->parent(npp) ;
                    prodVtx = ancestor->prodVtx() ;
                    vtxBarcode = prodVtx->barcode() ;
                    bc34 = ( (vtxBarcode == -3) | (vtxBarcode == -4) ) & (ancestor->pdgId() == pdgId) ;
                    if (bc34) pvtx34 = prodVtx ;
                  }
                }
                else hasAncestor = false ;
              }
            }
            if (bc34) {
              //Loop over the PDF partons container and see if from this vertex we get
              //an oposite charge, same flavour quark
              nid = 0 ;
              for (auto& part: PDF) {
                if ( ( part->prodVtx() == pvtx34 ) & (part->pdgId() == -pdgId) ) {
                  xPDF[fsq_pdgId].push_back(bcQuark) ;
                  nid++ ;
                }
              }

              //Check production vertex of parents, stop at the first
              isME = false ;
              for (auto& part: ME) {
                if (!isME) {
                  nParents = part->nParents() ;
                  for (unsigned int i=0; i<nParents; i++) {
                    father = part->parent(i) ;
                    if ( ( father->prodVtx() == pvtx34 ) & (part->pdgId() == pdgId) ) {
                      xME[fsq_pdgId].push_back(bcQuark) ;
                      nid++ ;
                      isME = true ;
                    }
                  }
                }
              }
              if (nid == 0) {
                GS[fsq_pdgId].push_back(bcQuark) ;
              }
            }
            else {
              //Don't know what is this ...."
              UFO[fsq_pdgId].push_back(bcQuark) ;
            }
          }
        }
      }
    }
    if (PDF.size() != xPDF[fsq_pdgId].size()) {
      //Warning(BOOST_CURRENT_FUNCTION, "Mismatch number of PDF identified partons of flavor %i", fsq_pdgId);
    }
  }

  //Store the quarks as classified in ME or GS
  m_qq["ME"] = xME ;
  m_qq["GS"] = GS  ;

  //Book keeping
  m_Nquarks["ME"][4] += xME[4].size() ;
  m_Nquarks["ME"][5] += xME[5].size() ;

  m_Nquarks["GS"][4] += GS[4].size() ;
  m_Nquarks["GS"][5] += GS[5].size() ;

  m_Nquarks["MPI"][4] += MPI[4].size() ;
  m_Nquarks["MPI"][5] += MPI[5].size() ;

  if(m_debug){
    std::cout<<"m_qq.size = "<<m_qq.size()<<endl;
    std::cout<<"PDF n = "<<PDF.size()<<std::endl;
    for(uint i=0; i<PDF.size(); ++i)  std::cout<<PDF[i]->barcode()<<std::endl;
    std::cout<<"ME n = "<<ME.size()<<std::endl;
    for(uint i=0; i<ME.size(); ++i)  std::cout<<ME[i]->barcode()<<std::endl;
    std::cout<<"GS n = "<<GS.size()<<std::endl;
    for (auto gs_item : GS){
      std::cout<<"GS item i = "<<gs_item.first<<std::endl;
      for(uint i=0; i<gs_item.second.size(); ++i)  std::cout<<gs_item.second.at(i)->barcode()<<std::endl;
    }
      
    std::cout<<"xPDF n = "<<xPDF.size()<<std::endl;
    for (auto xPDF_item : xPDF){
      std::cout<<"xPDF item i = "<<xPDF_item.first<<std::endl;
      for(uint i=0; i<xPDF_item.second.size(); ++i)  std::cout<<xPDF_item.second.at(i)->barcode()<<std::endl;
    }
      
    std::cout<<"xME n = "<<xME.size()<<std::endl;
    for (auto xME_item : xME){
      std::cout<<"xME item i = "<<xME_item.first<<std::endl;
      for(uint i=0; i<xME_item.second.size(); ++i)  std::cout<<xME_item.second.at(i)->barcode()<<std::endl;
    }
      
    std::cout<<"MPI n = "<<MPI.size()<<std::endl;
    for (auto mpi_item : MPI){
      std::cout<<"MPI item i = "<<mpi_item.first<<std::endl;
      for(uint i=0; i<mpi_item.second.size(); ++i)  std::cout<<mpi_item.second.at(i)->barcode()<<std::endl;
    }
    std::cout<<"ME_PDF n = "<<ME_PDF.size()<<std::endl;
    for (auto ME_PDF_item : ME_PDF){
      std::cout<<"ME_PDF item i = "<<ME_PDF_item.first<<std::endl;
      for(uint i=0; i<ME_PDF_item.second.size(); ++i)  std::cout<<ME_PDF_item.second.at(i)->barcode()<<std::endl;
    }
      
    std::cout<<"UFO n = "<<UFO.size()<<std::endl;
    for (auto UFO_item : UFO){
      std::cout<<"UFO item i = "<<UFO_item.first<<std::endl;
      for(uint i=0; i<UFO_item.second.size(); ++i)  std::cout<<UFO_item.second.at(i)->barcode()<<std::endl;
    }
  }

  return (true) ;
}
//==============================================================================

//==============================================================================
// Helper to check the parent type of the b or c quark
//==============================================================================
std::map<std::string, bool> HFOR_Truth::checkAncestor(const xAOD::TruthParticle* ancestor,
                                                      const xAOD::TruthParticle* bcQuark) {

  std::map<std::string, bool> tipo ;

  tipo ["proton"]  = false ;
  tipo ["top"]     = false ;
  tipo ["W"]       = false ;
  tipo ["cFromb"]  = false ;
  tipo ["bFromb"]  = false ;
  tipo ["bastard"] = false ;
  if (!is_sameFlavor(ancestor, bcQuark) ) {
    tipo ["proton"]  = is_proton(ancestor);
    tipo ["top"]     = ancestor->isTop() ;
    tipo ["W"]       = ancestor->isW() ;
    tipo ["cFromb"]  = (is_c(bcQuark) & (is_b(ancestor) | is_bHadron(ancestor))) ;
    tipo ["bFromb"]  = (is_b(bcQuark) & is_bHadron(ancestor)) ;
  }
  else {
    tipo["bastard"] = true ;
  }

  return (tipo) ;

}

//==============================================================================

//==============================================================================
// Helper to check if particle is a b
//==============================================================================
bool HFOR_Truth::is_b(const xAOD::TruthParticle* particle) {
  return (particle->absPdgId() == 5) ;
}
//==============================================================================

//==============================================================================
// Helper to check if particle is a c
//==============================================================================
bool HFOR_Truth::is_c(const xAOD::TruthParticle* particle) {
  return (particle->absPdgId() == 4) ;
}
//==============================================================================

//==============================================================================
// Helper to check if particle is a hadron
//==============================================================================
bool HFOR_Truth::is_bHadron(const xAOD::TruthParticle* x) {
  int apdg = x->absPdgId() ;
  bool isBH = false ;

  isBH = ((apdg%10000)/1000 == 5) | ((apdg%1000)/100 == 5) ;
  return (isBH);
}
//==============================================================================

//==============================================================================
// Helper to check if particle is a proton
//==============================================================================
bool HFOR_Truth::is_proton(const xAOD::TruthParticle* x) {
  return (x->absPdgId() == 2212) ;
}
//==============================================================================

//==============================================================================

//==============================================================================
// Helper to check if 2 particles have the same flavor
//==============================================================================
bool HFOR_Truth::is_sameFlavor(const xAOD::TruthParticle* x,
                               const xAOD::TruthParticle* y) {
  return ( x->absPdgId() == y->absPdgId() ) ;
}
//==============================================================================

//==============================================================================
// Here we need to identify which flavor this MC sample is. We rely on a
// configuration file which relates the MC ChannelNumber (or run number) to
// his flavor.
//==============================================================================
int HFOR_Truth::readRunConfig(std::string runConfigFile) {

  m_runConfigFile = runConfigFile ;

  //std::cout <<  BOOST_CURRENT_FUNCTION << " Reading Configuration file: " << m_runConfigFile << std::endl;
  boost::property_tree::ptree runConfig ;
  boost::property_tree::read_info (m_runConfigFile, runConfig) ;

  //std::cout <<  BOOST_CURRENT_FUNCTION << " Configuration Name:    " <<  runConfig.get<std::string>("configuration.name")    << std::endl ;
  //std::cout <<  BOOST_CURRENT_FUNCTION << " Configuration Version: " <<  runConfig.get<std::string>("configuration.version") << std::endl ;
  //std::cout <<  BOOST_CURRENT_FUNCTION << " Configuration Date:    " <<  runConfig.get<std::string>("configuration.date")    << std::endl ;


  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, runConfig.get_child("flavor.isLight")) {
    //std::cout << BOOST_CURRENT_FUNCTION << " In flavor isLight: " << v.first.data() << " --> " << v.second.data() << std::endl;
    m_runConfigMap[HFORType::isLight].push_back( atoi(v.first.data()) ) ;
  }
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, runConfig.get_child("flavor.isBB")) {
    //std::cout << BOOST_CURRENT_FUNCTION << " In flavor isBB: " << v.first.data() << " --> " << v.second.data() << std::endl;;
    m_runConfigMap[HFORType::isBB].push_back( atoi(v.first.data()) ) ;
  }
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, runConfig.get_child("flavor.isCC")) {
    //std::cout << BOOST_CURRENT_FUNCTION << " In flavor isCC: " << v.first.data() << " --> " << v.second.data() << std::endl;;
    m_runConfigMap[HFORType::isCC].push_back( atoi(v.first.data()) ) ;
  }
  BOOST_FOREACH(boost::property_tree::ptree::value_type &v, runConfig.get_child("flavor.isC")) {
    //std::cout << BOOST_CURRENT_FUNCTION << " In flavor isC: " << v.first.data() << " --> " << v.second.data() << std::endl;;
    m_runConfigMap[HFORType::isC].push_back( atoi(v.first.data()) ) ;
  }

  return 1;
}

//==============================================================================

//==============================================================================
// Here we keep the record of the MC sample type. The decision to keep or
// kill the event depends on this information
//==============================================================================
void HFOR_Truth::setSampleType(unsigned int sampleRunNumber) {

  std::vector<unsigned int>::iterator it;
  HFORType key ;
  bool found = false ;

  for (auto& kk: m_runConfigMap ) {
    if (!found) {
      key = kk.first ;
      it = find (kk.second.begin(), kk.second.end(), sampleRunNumber);
      if (it != m_runConfigMap[key].end() ) found = true ;
    }
  }

  if(!found) key = HFORType::noType ;

  m_sampleType = key ;
  m_sampleName = m_sampleNameMap[m_sampleType] ;

}
//==============================================================================

//==============================================================================
// Getter to access the MC sample type
//==============================================================================
HFORType HFOR_Truth::getSampleType() {
  return m_sampleType ;
}
//==============================================================================

//==============================================================================
// Getter for a human readable format of the MC sample
//==============================================================================
std::string HFOR_Truth::getSampleName() {
  return m_sampleName ;
}
//==============================================================================

//==============================================================================
// Setter for the size of the cone to be used in the overlap removal
//==============================================================================
void HFOR_Truth::setMatchConeSize(double deltaR) {

  //Sanity check
  if (deltaR > 0.0) {
    m_matchCone =  deltaR ;
  }
  else {
    //ATH_MSG_ERROR( BOOST_CURRENT_FUNCTION << "Invalid deltaR. Must be >= 0 " ) ;
  }
}
//==============================================================================

//==============================================================================
// Getter to access the value of cone size used in the overlap removal
//==============================================================================
double HFOR_Truth::getMatchConeSize() {
  return m_matchCone ;
}
//==============================================================================


//==============================================================================
//Here we will perform the angular based removal based on the sample type.
//Based on the angle between the 2 b or c quarks and it's classification as from
//ME or GS the event will be flagged to be killed (if it's a ME quark living
//in a GS phase space and vice-versa)
//==============================================================================
HFORType HFOR_Truth::angularBasedRemoval( void ) {

  HFORType tipo ;
  std::string proc ;
  int flavor ;

  std::map< std::string, std::map<int, int> > hasMatch ;
  double dR = 0;

  for (auto& bcProcMap : m_qq ) {
    proc = bcProcMap.first ;
    for (auto& bcMap : bcProcMap.second) {
      flavor = bcMap.first ;
      hasMatch[proc][flavor] = 0 ;
      m_dR[proc][flavor].clear() ;
      if ( bcMap.second.size() > 1)  {
        //Combination of 2 different quarks
        for (unsigned int i=0;i<bcMap.second.size();i++) {
          for (unsigned int j=i+1;j<bcMap.second.size();j++) {
            const xAOD::TruthParticle* x =  bcMap.second[i] ;
            const xAOD::TruthParticle* y =  bcMap.second[j] ;
            //Make sure they are opposite charge
            //TODO: make sure they are from same vtx
            if ( x->pdgId() == -y->pdgId() ) {

              dR = sqrt( pow(x->phi() - y->phi(),2) + pow(x->eta() - y->eta(),2) ) ;
              m_dR[proc][flavor].push_back(dR) ;
              if ( dR < m_matchCone ) {
                hasMatch[proc][flavor]++ ;
                if(m_debug)  { // Same vertex ?
                  if (x->prodVtx() == y->prodVtx()) {
                    std::cout << "Match from same Vertex flavor->" << flavor << std::endl ;
                  }
                  else {
                    std::cout << "Match NOT from same Vertex flavor->" << flavor << std::endl ;
                  }
                  std::cout << "///////////////// " << proc << "  " << flavor << " dr " << dR << std::endl ;
                }								 
              }
              else {
                if(m_debug) std::cout << "No Match (dr>0.4)" << std::endl ;
              }
            }
          }
        }
      }
    }
  }

  tipo = m_sampleType ;
  if(tipo == HFORType::noType ) return tipo;

  //light sample
  if (m_sampleType == HFORType::isLight) {
    //Remove ME b,c quarks
    if ( (m_qq["ME"][4].size() > 0) | (m_qq["ME"][5].size()>0) ) {
      tipo = HFORType::kill ;
      if(m_debug) std::cout << "Killed by ME size 4,5: " <<  m_qq["ME"][4].size() << " , " << m_qq["ME"][4].size() << std::endl ;
    }
    //Remove un-matched GS b,c quarks
    else if (m_qq["GS"][5].size() > 0) {
      if (hasMatch["GS"][5] > 0) {
        tipo = HFORType::isBB;
      }
      else {
        tipo = HFORType::kill;
      }

    }
    else if (m_qq["GS"][4].size() > 0) {
      if (hasMatch["GS"][4] > 0) {
        tipo = HFORType::isCC;
      }
      else {
        tipo = HFORType::kill;
      }
    }
  }

  //cc sample
  if (m_sampleType == HFORType::isCC) {
    //Remove matched ME c quarks
    if ( hasMatch["ME"][4] > 0 ) {
      tipo = HFORType::kill ;
    }
    else if (hasMatch["GS"][5] > 0) {
      tipo = HFORType::isBB ;
    }
    else if (m_qq["GS"][5].size()>0) {
      tipo = HFORType::kill ;
    }
  }

  //c sample
  if (m_sampleType == HFORType::isC) {
    //Remove
    if ( hasMatch["GS"][5] > 0 ) {
      tipo = HFORType::isBB ;
    }
    else if (m_qq["GS"][5].size() > 0 ) {
      tipo = HFORType::kill ;
    }
  }

  //bb sample
  if (m_sampleType == HFORType::isBB) {
    //Remove matched ME b quarks
    if (hasMatch["ME"][5] > 0 )
      tipo = HFORType::kill ;
  }

  m_Nclass[tipo] ++ ;
  return tipo ;
}
//==============================================================================

//==============================================================================
//Access the bookkeeper map
//==============================================================================
unsigned long int HFOR_Truth::getNquarks(std::string process, int flavor) {
  //Access the number of quarks found in this sample according to the
  //process and flavor
  //TODO: check the keys ..
  return m_Nquarks[process][flavor] ;
}
//==============================================================================

//==============================================================================
//Access the number of decisions taken in this sample
//==============================================================================
unsigned long int HFOR_Truth::getNclass(HFORType htipo) {
  //TODO check the keys
  return m_Nclass[htipo] ;
}
//==============================================================================

//=============================================================================
//Access a vector containing the deltaR of a given process and flavor
//BEFORE selection
//==============================================================================
std::map<std::string, std::map<int, std::vector<float> > > HFOR_Truth::getdR() {

  return m_dR ;
}
//==============================================================================


