/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonCalibBranchNtuple.h"
#include "MuonCalibNtuple/FillToBranch.h"
  
#include "MdtCalibFitters/DCSLFitter.h"
#include "MuonCalibNtuple/RootFileManager.h"

#include "MuonCalibEventBase/MuonCalibEventInfo.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"

#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

// 2016-03-17 E. Diehl
// Remove calls to cout (should be replaced by proper Athena messages)
// Remove calls to create and fill MuonPatternNtupleBranch.  However, do not change handling of 
// MuonCalibPattern which are needed for processing Segments.

namespace MuonCalib {

  MuonCalibBranchNtuple::MuonCalibBranchNtuple(bool fillTruth, bool doRawTgc):
    m_file(NULL), m_directory(NULL), m_tree(NULL), m_fitter(NULL), m_event(0), m_truthBranch(NULL), m_mdtTruthBranch(NULL), 
    m_rpcTruthBranch(NULL), m_tgcTruthBranch(NULL), m_cscTruthBranch(NULL), m_printLevel(0) {

    //if( m_printLevel >= 1){
     //std::cout << "MuonCalibBranchNtuple::MuonCalibBranchNtuple" << std::endl;
    //} 
    openFile();
    bookTree(fillTruth, doRawTgc);
    //if( m_printLevel >= 3){
     //std::cout << "defaultConstructor booked tree" << std::endl;
    //}
    m_fitter = createFitter();
  }  //end MuonCalibBranchNtuple::MuonCalibBranchNtuple

  MuonCalibBranchNtuple::MuonCalibBranchNtuple(std::string RegionName, bool fillTruth, bool doRawTgc) : 
    m_file(0), m_directory(0), m_tree(0) , m_fitter(NULL), m_event(0), m_truthBranch(NULL), m_mdtTruthBranch(NULL), 
    m_rpcTruthBranch(NULL), m_tgcTruthBranch(NULL), m_cscTruthBranch(NULL), m_printLevel(0) {
    m_event = 0;
    m_printLevel = 0;
    //    if( m_printLevel >= 1){
    //std::cout << "MuonCalibBranchNtuple::MuonCalibBranchNtuple" << std::endl;
    //    }
    m_directory = RootFileManager::getInstance()->getDirectory(RegionName);
    //    if( m_printLevel >= 1){
    //std::cout << "opening directory " << m_directory->GetPath() << std::endl;
    //    }
    m_event = 0;
    m_printLevel = 0;
    bookTree(fillTruth, doRawTgc);
    // if( m_printLevel >= 1){
    //std::cout << "region Constructor booked tree " << RegionName << std::endl;
    //}
    m_fitter = createFitter();
    //    if( m_printLevel >= 1){
    //std::cout << "booking fitter" << std::endl;
    //}
  }  //end MuonCalibBranchNtuple::MuonCalibBranchNtuple

  MuonCalibBranchNtuple::~MuonCalibBranchNtuple() {
    //if( m_printLevel >= 1){
    //std::cout << "MuonCalibBranchNtuple::~MuonCalibBranchNtuple" << std::endl;
    //}
    closeFile();
    //if( m_printLevel >= 3){
    //std::cout << "closed file" << std::endl;
    //}
    delete m_fitter;
    m_fitter = 0;
    if(m_truthBranch) delete m_truthBranch;
    if(m_mdtTruthBranch) delete m_mdtTruthBranch;
    if(m_rpcTruthBranch) delete m_rpcTruthBranch;
    if(m_tgcTruthBranch) delete m_tgcTruthBranch;
    if(m_cscTruthBranch) delete m_cscTruthBranch;
  }  //end MuonCalibBranchNtuple::~MuonCalibBranchNtuple

  void MuonCalibBranchNtuple::openFile() { 
    m_file = new TFile("segments.root","RECREATE"); 
    m_directory = m_file; 
  }

  void MuonCalibBranchNtuple::writeTree() { 
    //if( m_printLevel >= 1){
    //std::cout << " MuonCalibBranchNtuple::writeTree() " << m_directory << " " << m_tree << std::endl;
    //m_directory->pwd();
    //}
    //std::cout << " MuonCalibBranchNtuple::writeTree() " << m_directory << " " << m_tree << std::endl;
    m_directory->cd();
    m_tree->Write();

    //if( m_printLevel >= 3){
    //std::cout << "  Done " << std::endl;
    //}
    //std::cout << " MuonCalibBranchNtuple::writeTree() done" << std::endl;
  }

  void MuonCalibBranchNtuple::closeFile() {
    if( m_file ){
      m_file->Write();
      m_file->Close();
      delete m_file;
      m_file = 0;
    } else {
      RootFileManager::getInstance()->closeFile();
    }
  }  //end MuonCalibBranchNtuple::closeFile
  
  // Probably this should be called resetEvent, because thats what it does
  void MuonCalibBranchNtuple::setEvent(unsigned int n) { 
    m_event = n; 
    m_eventBranch.reset();
    if(m_truthBranch)m_truthBranch->reset();
    if(m_mdtTruthBranch) m_mdtTruthBranch->reset();
    if(m_rpcTruthBranch) m_rpcTruthBranch->reset();   
    if(m_tgcTruthBranch) m_tgcTruthBranch->reset();
    if(m_tgcTruthBranch )m_cscTruthBranch->reset();
    //    m_patBranch.reset();
    m_segBranch.reset();
    m_mdtHitBranch.reset();
    m_rpcHitBranch.reset();
    m_tgcHitBranch.reset();
    m_cscHitBranch.reset();
    m_rawMdtHitBranch.reset();
    m_rawRpcHitBranch.reset();
    m_rawTgcHitBranch.reset();
    m_rawTgcCoinBranch.reset();
    m_rawRpcCoinBranch.reset();
    m_rawCscHitBranch.reset();
    m_rpcSlBranch.reset();
    m_rawRpcTriggerHitBranch.reset();
  }  //MuonCalibBranchNtuple::setEvent

  void MuonCalibBranchNtuple::finishEvent() { 
    //if( m_printLevel >= 1){
      //std::cout << " MuonCalibBranchNtuple::finishEvent " << std::endl;
      //std::cout << " patterns " << m_patBranch.getBranchEntries() 
	 //<< " segments " << m_segBranch.getBranchEntries() 
	 //<< " MDT hits " << m_mdtHitBranch.getBranchEntries()    
	 //<< " RPC hits " << m_rpcHitBranch.getBranchEntries() 
	 //<< " TGC hits " << m_tgcHitBranch.getBranchEntries()    
	 //<< " CSC hits " << m_cscHitBranch.getBranchEntries() 
	 //<< " truthevents " << m_truthBranch.getBranchEntries() 
	 //<< " eventbranch " << m_eventBranch.getBranchEntries() 
	 //<< " rawMdtbranch " << m_rawMdtHitBranch.getBranchEntries() << std::endl;
    //}
    //if( m_printLevel >= 5){
    //std::cout << "  filling tree " << m_tree << std::endl;
    //}

    m_tree->Fill(); 
    //std::cout << " event filled " << std::endl;
    //if( m_printLevel >= 5){
      //std::cout << "  clearing branches" << std::endl;
    //}
    //if( m_printLevel >= 2){
      //std::cout << " MuonCalibBranchNtuple::finishEvent DONE" << std::endl;
    //}
  }  //end MuonCalibBranchNtuple::finishEvent

  void MuonCalibBranchNtuple::handlePattern(const MuonCalibPattern &pat) {
    //if( m_printLevel >= 1) {
      //std::cout << " MuonCalibBranchNtuple::handlePattern" << std::endl;
    //}

    //    m_patBranch.fillBranch( pat, PatInfo() );
  
    //if( m_printLevel >= 3) {
      //std::cout << "   loop over segments" << std::endl;
      //std::cout << "*** handlePattern: " << std::endl;
      //pat.dump(std::cout);
    //}

    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it     = pat.muonSegBegin();
    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it_end = pat.muonSegEnd();
  
    for( ;seg_it!=seg_it_end;++seg_it ){
      handleSegment( **seg_it ); 
    }
  }  //end MuonCalibBranchNtuple::handlePattern

  void MuonCalibBranchNtuple::handleSegment(const MuonCalibSegment &seg) {
    //if( m_printLevel >= 3){
      //std::cout << "  MuonCalibBranchNtuple::handleSegment " ;
    //}

    fillSegmentToTree(seg);
  }
  
  void MuonCalibBranchNtuple::fillTruthToTree( const MuonCalibTruth &truth ) {
    //if( m_printLevel >= 3){
     //std::cout << "fillTruthToTree :" << truth << std::endl;
    //}
    if(m_truthBranch) m_truthBranch->fillBranch( truth );
  }
  
  void MuonCalibBranchNtuple::fillMdtTruthToTree( const MuonCalibMdtTruthHit &hit ) {
    //if( m_printLevel >= 3){
     //std::cout << "fillMdtTruthToTree :" << std::endl;
    //}
    if(m_mdtTruthBranch) m_mdtTruthBranch->fillBranch( hit );
  }
  
  void MuonCalibBranchNtuple::fillRpcTruthToTree( const MuonCalibRpcTruthHit& hit ) {
    //if( m_printLevel >= 3) {
     //std::cout << "fillRpcTruthToTree :" << std::endl;
    //}
    if(m_rpcTruthBranch) m_rpcTruthBranch->fillBranch( hit );
  }

  void MuonCalibBranchNtuple::fillTgcTruthToTree( const MuonCalibTgcTruthHit &hit ) {
    //if( m_printLevel >= 3){
     //std::cout << "fillTgcTruthToTree :" << std::endl;
    //}
    if(m_tgcTruthBranch) m_tgcTruthBranch->fillBranch( hit );
  }

  void MuonCalibBranchNtuple::fillCscTruthToTree( const MuonCalibCscTruthHit &hit ) {
    //if( m_printLevel >= 3){
     //std::cout << "fillCscTruthToTree :" << std::endl;
    //}
    if(m_cscTruthBranch) m_cscTruthBranch->fillBranch( hit );
  }

  void MuonCalibBranchNtuple::fillRpcSectorLogic( const RpcSectorLogicContainer& rpcSlContainer ){
    //if( m_printLevel >= 3){
     //std::cout << "fillRpcSectorLogic :" << std::endl;
    //}
    m_rpcSlBranch.fillBranch(rpcSlContainer);
  }

  void MuonCalibBranchNtuple::fillEventToTree( const MuonCalibEventInfo &eventInfo, const MuonCalibTriggerTimeInfo &trigTimeInfo ) {
    //if( m_printLevel >= 3){
      //std::cout << "fillEventToTree : " << std::endl;
      //eventInfo.dump(std::cout);
      //trigTimeInfo.dump(std::cout);
    //}

    m_eventBranch.reset();
    if(m_truthBranch)    m_truthBranch->reset();
    if(m_mdtTruthBranch) m_mdtTruthBranch->reset();
    if(m_rpcTruthBranch) m_rpcTruthBranch->reset();
    if(m_tgcTruthBranch) m_tgcTruthBranch->reset();
    if(m_cscTruthBranch) m_cscTruthBranch->reset();
    //    m_patBranch.reset();
    m_segBranch.reset();
    m_mdtHitBranch.reset();
    m_rpcHitBranch.reset();
    m_tgcHitBranch.reset();
    m_cscHitBranch.reset();
    m_rawMdtHitBranch.reset();
    m_rawRpcHitBranch.reset();
    m_rawTgcHitBranch.reset();
    m_rawTgcCoinBranch.reset();
    m_rawRpcCoinBranch.reset();
    m_rawCscHitBranch.reset();
    m_rpcSlBranch.reset();
    m_rawRpcHitBranch.reset();
    m_rawRpcTriggerHitBranch.reset();

    m_eventBranch.fillBranch( eventInfo, trigTimeInfo );
  }  //end MuonCalibBranchNtuple::fillEventToTree

  // Fill segment info and segment hit info for MDT, RPC, TGC, CSC
  void MuonCalibBranchNtuple::fillSegmentToTree(const MuonCalibSegment &seg) {
    //if( m_printLevel >= 3){
      //std::cout << "   MuonCalibBranchNtuple::fillSegmentToTree. seg num " << m_segBranch.getBranchEntries() << std::endl; 
    //}

    //    m_segBranch.fillBranch( seg, m_patBranch.getBranchEntries()-1 );
    m_segBranch.fillBranch( seg );
 
    std::for_each( seg.mdtHOTBegin(),seg.mdtHOTEnd(), 
		   FillToBranch<MdtCalibHitNtupleBranch,int,MdtCalibHitBase>( &m_mdtHitBranch, m_segBranch.getBranchEntries()-1 ) );
    	
    std::for_each( seg.rpcHOTBegin(),seg.rpcHOTEnd(), 
		   FillToBranch<RpcCalibHitNtupleBranch,int,RpcCalibHitBase>( &m_rpcHitBranch, m_segBranch.getBranchEntries()-1 ) );
	
    std::for_each( seg.tgcHOTBegin(),seg.tgcHOTEnd(), 
		   FillToBranch<TgcCalibHitNtupleBranch,int,TgcCalibHitBase>( &m_tgcHitBranch, m_segBranch.getBranchEntries()-1 ) );
	
    std::for_each( seg.cscHOTBegin(),seg.cscHOTEnd(), 
		   FillToBranch<CscCalibHitNtupleBranch,int,CscCalibHitBase>( &m_cscHitBranch, m_segBranch.getBranchEntries()-1 ) );							    
  }  //end MuonCalibBranchNtuple::fillSegmentToTree

  void MuonCalibBranchNtuple::bookTree(bool doTruth, bool doRawTgc) {
    //if( m_printLevel >= 1 )
      //std::cout << " bookTree: creating new tree in " << m_directory->GetPath() << std::endl;

    if( !m_directory ){
      //std::cout << " bookTree: ERROR no directory defined " << std::endl;
      return;
    }

    m_directory->cd();

    m_tree = new TTree("Segments","My first tree");
    m_tree->SetAutoFlush(0); 
    //if( m_printLevel >= 3 ) {
    //std::cout << " New tree " << m_tree << " creating branches " << std::endl;
    //std::cout << "creating truth branch " << std::endl;
    //}
    if(doTruth) {
      createTruthBranch(m_tree);

      //if( m_printLevel >= 3 ) std::cout << "creating mdt truth hit branch " << std::endl;
      createMdtTruthHitBranch(m_tree);

      //if( m_printLevel >= 3 ) std::cout << "creating rpc truth hit branch " << std::endl;
      createRpcTruthHitBranch(m_tree);

      //if( m_printLevel >= 3 ) std::cout << "creating tgc truth hit branch " << std::endl;
      createTgcTruthHitBranch(m_tree);

      //if( m_printLevel >= 3 ) std::cout << "creating csc truth hit branch " << std::endl;
      createCscTruthHitBranch(m_tree);
    }

    //if( m_printLevel >= 3 ) std::cout << "creating event branch " << std::endl;
    createEventBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating pattern branch " << std::endl;
    //    createPatternBranch(m_tree);   //20160301 Disable pattern branch (pat_) 
    //if( m_printLevel >= 3 ) std::cout << "creating segment branch " << std::endl;
    createSegmentBranch(m_tree); 
    //if( m_printLevel >= 3 ) std::cout << "creating mdt hit branch " << std::endl;
    createMdtCalibHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating rpc hit branch " << std::endl;
    createRpcCalibHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating tgc hit branch " << std::endl;
    createTgcCalibHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating csc hit branch " << std::endl;
    createCscCalibHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating raw mdt branch " << std::endl;
    createMdtRawHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating raw rpc branch " << std::endl;
    createRpcRawHitBranch(m_tree);
    if( doRawTgc ) { 
      //if( m_printLevel >= 3 ) std::cout << "creating raw tgc branch " << std::endl;
      createTgcRawHitBranch(m_tree);
      //if( m_printLevel >= 3 ) std::cout << "creating raw tgc coin branch " << std::endl;
      createTgcRawCoinBranch(m_tree);
    }
    //if( m_printLevel >= 3 ) std::cout << "creating raw rpc coin branch " << std::endl;
    createRpcRawCoinBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating raw csc branch " << std::endl;
    createCscRawHitBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating rpc sector logic branch " << std::endl;
    m_rpcSlBranch.createBranch(m_tree);
    //if( m_printLevel >= 3 ) std::cout << "creating raw rpc trigger branch " << std::endl;
    createRpcRawTriggerHitBranch(m_tree);
    
    //std::cout << "created all branches" << std::endl;

    if( m_printLevel >= 5 ){
      m_tree->Print("all");
      //std::cout << "  done " << std::endl;
    }
  }  //end MuonCalibBranchNtuple::bookTree
 
  void  MuonCalibBranchNtuple::createEventBranch(TTree *tree) {
    m_eventBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createPatternBranch(TTree *tree) {
    m_patBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createSegmentBranch(TTree *tree) {
    m_segBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createMdtCalibHitBranch(TTree *tree) {
    m_mdtHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createRpcCalibHitBranch(TTree *tree) {
    m_rpcHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createTgcCalibHitBranch(TTree *tree) {
    m_tgcHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createCscCalibHitBranch(TTree *tree) {
    m_cscHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createTruthBranch(TTree *tree) {
    if(!m_truthBranch)m_truthBranch = new  MuonTruthNtupleBranch();
    m_truthBranch->createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createMdtTruthHitBranch(TTree *tree) {
    if(!m_mdtTruthBranch) m_mdtTruthBranch = new MdtTruthHitNtupleBranch();
    m_mdtTruthBranch->createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createRpcTruthHitBranch(TTree *tree) {
   if(!m_rpcTruthBranch) m_rpcTruthBranch = new RpcTruthHitNtupleBranch();
    m_rpcTruthBranch->createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createTgcTruthHitBranch(TTree *tree) {
   if(!m_tgcTruthBranch) m_tgcTruthBranch = new TgcTruthHitNtupleBranch();
    m_tgcTruthBranch->createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createCscTruthHitBranch(TTree *tree) {
    if(!m_cscTruthBranch) m_cscTruthBranch = new CscTruthHitNtupleBranch();
    m_cscTruthBranch->createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createMdtRawHitBranch(TTree *tree) {
    m_rawMdtHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createRpcRawHitBranch(TTree *tree) {
    m_rawRpcHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createRpcRawTriggerHitBranch(TTree *tree) {
    m_rawRpcTriggerHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createRpcRawCoinBranch(TTree *tree) {
    m_rawRpcCoinBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createTgcRawHitBranch(TTree *tree) {
    m_rawTgcHitBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createTgcRawCoinBranch(TTree *tree) {
    m_rawTgcCoinBranch.createBranch( tree );
  }

  void  MuonCalibBranchNtuple::createCscRawHitBranch(TTree *tree) {
    m_rawCscHitBranch.createBranch( tree );
  }

  IMdtSegmentFitter* MuonCalibBranchNtuple::createFitter() {
    DCSLFitter* fitter = new DCSLFitter();
    fitter->printLevel(m_printLevel);
    return fitter;
  }

  void MuonCalibBranchNtuple::setPrintLevel(int level) { 
    m_printLevel = level; 

    DCSLFitter* fitter = dynamic_cast<DCSLFitter*>(m_fitter);
    if( fitter ) fitter->printLevel(m_printLevel);

    m_resolver.setPrintLevel(level);
  }

  void MuonCalibBranchNtuple::handleRawHits( const MuonCalibRawHitCollection &rawHits ) {
    MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit mdt_it = rawHits.rawMdtHitCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit mdt_it_end = rawHits.rawMdtHitCollectionEnd();
    bool mdtAdd = true, rpcAdd = true, rpcAddCoin = true, tgcAdd = true, cscAdd = true;

    for(;mdt_it!=mdt_it_end; ++mdt_it) {
      
      //if( m_printLevel >= 3 ) {
        //std::cout << "rawMdtHit : " << std::endl;
        //(*mdt_it)->dump(std::cout);
      //}
      if(mdtAdd) mdtAdd = m_rawMdtHitBranch.fillBranch( **mdt_it );
    }
    //if (rawHits.numberOfMuonCalibRawMdtHits() > m_rawMdtHitBranch.blockSize()) {
      //std::cout << " Number of Mdt Raw Hits exceeds maximum number that can be stored: " << m_rawMdtHitBranch.blockSize() << std::endl;
    //}

    MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit rpc_it = rawHits.rawRpcHitCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit rpc_it_end = rawHits.rawRpcHitCollectionEnd();
    for(;rpc_it!=rpc_it_end; ++rpc_it) {
      //if( m_printLevel >= 3 ) {
       //std::cout << "rawRpcHit : " << std::endl;
       //(*rpc_it)->dump(std::cout);
      //}
      const MuonCalibRawRpcCoin *coin = dynamic_cast<const MuonCalibRawRpcCoin*>(*rpc_it);
      if(!coin && rpcAdd) {
	rpcAdd =m_rawRpcHitBranch.fillBranch( **rpc_it );
	//if(!rpcAdd) {
	  //std::cout << " Number of Rpc Raw Hits exceeds maximum number that can be stored: " << m_rawRpcHitBranch.blockSize() << std::endl;
	//}
      } else if(coin && rpcAddCoin) {
	rpcAddCoin = m_rawRpcCoinBranch.fillBranch( *coin );
	//if(!rpcAddCoin) {
	  //std::cout << " Number of Rpc Raw coin Hits exceeds maximum number that can be stored: " << m_rawRpcHitBranch.blockSize() << std::endl;
	//}
      }
    }

    MuonCalibRawHitCollection::MuonCalibRawCscHitVecCit csc_it = rawHits.rawCscHitCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawCscHitVecCit csc_it_end = rawHits.rawCscHitCollectionEnd();
    //if (rawHits.numberOfMuonCalibRawCscHits() > m_rawCscHitBranch.blockSize()) {
      //std::cout << " Number of Csc Raw Hits exceeds maximum number that can be stored: " << m_rawCscHitBranch.blockSize() << std::endl;
    //}

    for(;csc_it!=csc_it_end; ++csc_it) {
      //if( m_printLevel >= 3 ) {
       //std::cout << "rawCscHit : " << std::endl;
       //(*csc_it)->dump(std::cout);
      //}
      if(cscAdd) cscAdd =m_rawCscHitBranch.fillBranch( **csc_it );
    }
    MuonCalibRawHitCollection::MuonCalibRawTgcHitVecCit tgc_it = rawHits.rawTgcHitCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawTgcHitVecCit tgc_it_end = rawHits.rawTgcHitCollectionEnd();
    //if (rawHits.numberOfMuonCalibRawTgcHits() > m_rawTgcHitBranch.blockSize()) {
      //std::cout << " Number of Tgc Raw Hits exceeds maximum number that can be stored: " << m_rawTgcHitBranch.blockSize() << std::endl;
    //}

    for(;tgc_it!=tgc_it_end; ++tgc_it) {
      //if( m_printLevel >= 3 ) {
       //std::cout << "rawTgcHit : " << std::endl;
       //(*tgc_it)->dump(std::cout);
      //}
      if(tgcAdd) tgcAdd = m_rawTgcHitBranch.fillBranch( **tgc_it );
    }
    MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecCit tgcCoin_it = rawHits.rawTgcCoinCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecCit tgcCoin_it_end = rawHits.rawTgcCoinCollectionEnd();
    //if (rawHits.numberOfMuonCalibRawTgcHits() > m_rawTgcCoinBranch.blockSize()) {
      //std::cout << " Number of Tgc Raw Hits exceeds maximum number that can be stored: " << m_rawTgcCoinBranch.blockSize() << std::endl;
    //}

    for(;tgcCoin_it!=tgcCoin_it_end; ++tgcCoin_it) {
      //if( m_printLevel >= 3 ) {
       //std::cout << "rawTgcCoin : " << std::endl;
       //(*tgcCoin_it)->dump(std::cout);
      //}
      if(tgcAdd) tgcAdd = m_rawTgcCoinBranch.fillBranch( **tgcCoin_it );
    }

  }  //end MuonCalibBranchNtuple::handleRawHits

  void MuonCalibBranchNtuple::handleRawTriggerHits( const MuonCalibRawTriggerHitCollection &rawTriggerHits ) {
    MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecCit rpc_it = rawTriggerHits.rawRpcTriggerHitCollectionBegin();
    MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecCit rpc_it_end = rawTriggerHits.rawRpcTriggerHitCollectionEnd();
    for(;rpc_it!=rpc_it_end; ++rpc_it){
      //if( m_printLevel >= 3 ) {
	//std::cout << "rawRpcTriggerHit : " << std::endl;
	//(*rpc_it)->dump(std::cout);
      //}
      m_rawRpcTriggerHitBranch.fillBranch( **rpc_it );
    }
  }  //end MuonCalibBranchNtuple::handleRawTriggerHits
  
}  //namespace MuonCalib
