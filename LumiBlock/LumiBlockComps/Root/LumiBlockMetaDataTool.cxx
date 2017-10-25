/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Modified from @file ReadMeta.cxx
 *  @brief The LumiBlockMetaDataTool reads luminosity metadata from input files and transfers it to output files
 *  @author Marjorie Shapiro <mdshapiro@lbl.gov> based on work from Peter van Gemmeren <gemmeren@anl.gov> 
 *  $Id: LumiBlockMetaDataTool.cxx 783129 2016-11-09 03:47:26Z ssnyder $
 **/

#include "LumiBlockComps/LumiBlockMetaDataTool.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
//#include "LumiCalc/LumiBlockRangeContainerConverter.h"
#include "LumiBlockComps/ILumiCalcSvc.h"
#include "xAODLuminosity/SortLumiBlockRangeByStart.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"
#include "DBDataModel/CollectionMetadata.h"

#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TUniqueGRLString.h"

//#include "urldecode.h"
#include "TROOT.h"

#include <algorithm>

//___________________________________________________________________________
LumiBlockMetaDataTool::LumiBlockMetaDataTool(const std::string& type, const std::string& name, const IInterface* parent) : 
   AthAlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/InputMetaDataStore", name),
   m_tagDataStore   ("StoreGateSvc/TagMetaDataStore", name),
   m_nfiles(0),
   m_fileCurrentlyOpened(false),
   m_CurrentFileName("none"),
   /*   m_converter(new LumiBlockRangeContainerConverter()),*/
   m_grlcollection(new Root::TGRLCollection()),
   m_GoodRunsListSelectorTool("GoodRunsListSelectorTool")
{
   declareInterface<IMetaDataTool>(this);
   declareInterface<ILumiBlockMetaDataTool>(this); 

   declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
   declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
   declareProperty("suspectLBCollName", m_suspectLBColl_name="SuspectLumiBlocks");
   declareProperty("VersionString",m_version = "10");
   declareProperty("calcLumi", m_calcLumi = false);
   declareProperty("storeXMLFiles", m_storexmlfiles = false);
   declareProperty("applyDQCuts", m_applydqcuts = false);
 


   // Here is where we create the LumiBlockRange objects.  When we open a 
   // file, we fill the m_cacheInputRangeContainer from the input metadata store.  
   // When the file closes, we move the LumiBlockRange objects to the
   // m_cacheOutputRangeContainer.  Objects in the m_cacheOutputRangeContainer
   // will be sorted and merged at the end of the job
   // Note: the Suspect Range objects are kept in separate containers because
   // once an object is suspect, it remains suspect forever
   // If a file is only partially read, the objects in the m_cacheInputRangeContainer
   // will be moved to the m_suspectOutputRangeContainer 

   m_cacheInputRangeContainer = new xAOD::LumiBlockRangeContainer();
   m_cacheInputRangeAuxContainer = new xAOD::LumiBlockRangeAuxContainer();
   m_cacheInputRangeContainer->setStore( m_cacheInputRangeAuxContainer );

   m_cacheSuspectInputRangeContainer = new xAOD::LumiBlockRangeContainer();
   m_cacheSuspectInputRangeAuxContainer = new xAOD::LumiBlockRangeAuxContainer();
   m_cacheSuspectInputRangeContainer->setStore( m_cacheSuspectInputRangeAuxContainer );

   m_cacheOutputRangeContainer = new xAOD::LumiBlockRangeContainer();
   m_cacheOutputRangeAuxContainer = new xAOD::LumiBlockRangeAuxContainer();
   m_cacheOutputRangeContainer->setStore( m_cacheOutputRangeAuxContainer );

   m_cacheSuspectOutputRangeContainer = new xAOD::LumiBlockRangeContainer();
   m_cacheSuspectOutputRangeAuxContainer = new xAOD::LumiBlockRangeAuxContainer();
   m_cacheSuspectOutputRangeContainer->setStore( m_cacheSuspectOutputRangeAuxContainer );

   m_DQLBColl_name = TString(m_LBColl_name)+"_GoodDQ";
   m_unfinishedDQLBColl_name = TString(m_unfinishedLBColl_name)+"_GoodDQ";
}
  
//___________________________________________________________________________
LumiBlockMetaDataTool::~LumiBlockMetaDataTool() {

  delete m_cacheInputRangeContainer; m_cacheInputRangeContainer = 0;
  delete m_cacheInputRangeAuxContainer; m_cacheInputRangeAuxContainer = 0;
  delete m_cacheSuspectInputRangeContainer; m_cacheSuspectInputRangeContainer = 0;
  delete m_cacheSuspectInputRangeAuxContainer; m_cacheSuspectInputRangeAuxContainer = 0;

  delete m_cacheOutputRangeContainer; m_cacheOutputRangeContainer = 0;
  delete m_cacheOutputRangeAuxContainer; m_cacheOutputRangeAuxContainer = 0;
  delete m_cacheSuspectOutputRangeContainer; m_cacheSuspectOutputRangeContainer = 0;
  delete m_cacheSuspectOutputRangeAuxContainer; m_cacheSuspectOutputRangeAuxContainer = 0;

  if (m_grlcollection!=0) { delete m_grlcollection; m_grlcollection=0; }
}

//___________________________________________________________________________
StatusCode LumiBlockMetaDataTool::initialize() {
   ATH_MSG_INFO( "in initialize()" );

   ATH_CHECK( m_pMetaDataStore.retrieve() );
   ATH_CHECK( m_pInputStore.retrieve() );
   ATH_CHECK( m_tagDataStore.retrieve() );

   // Set to be listener for end of event
   /*
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   ATH_CHECK( incSvc.retrieve() );
   incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc and AthenaOutputStream.
   incSvc->addListener(this, "LastInputFile", 50); // pri has to be > 20 to be before MetaDataSvc and AthenaOutputStream.
   incSvc->addListener(this, "MetaDataStop", 70); 
   */


   // Don't try to retrieve this during initialize().
   // Otherwise, we induce a component initialization dependency
   // loop that prevents address remappings from getting
   // set up correctly.  The retrieval will be deferred
   // until the handle is actually used.
   //ATH_CHECK( m_GoodRunsListSelectorTool.retrieve() );


   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode LumiBlockMetaDataTool::finalize() {
   return(StatusCode::SUCCESS);
}

StatusCode LumiBlockMetaDataTool::beginInputFile()
{
   std::string fileName = "Undefined ";
   bool alreadyRecorded=false;
     if(m_CurrentFileName==fileName) {
       alreadyRecorded=true;
       }
     if(m_fileCurrentlyOpened) {
       alreadyRecorded=true;
     }
     m_CurrentFileName = fileName;
     if(alreadyRecorded) return StatusCode::SUCCESS;
      m_nfiles++;
      m_fileCurrentlyOpened=true;
      //
      // Look for LB information on input store and transfer it to temporay cache
      // ===========================================================================

      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_LBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_LBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_LBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find unfinished xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return StatusCode::SUCCESS;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheInputRangeContainer->push_back(iovr);
	 }
      }
      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_unfinishedLBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_unfinishedLBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_unfinishedLBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find unfinished xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return StatusCode::SUCCESS;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheInputRangeContainer->push_back(iovr);
	}
      }
      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_suspectLBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_suspectLBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_suspectLBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find suspect xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return StatusCode::SUCCESS;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheSuspectInputRangeContainer->push_back(iovr);
	 }
      }
   return(StatusCode::SUCCESS);
}
StatusCode LumiBlockMetaDataTool::endInputFile()
{
   std::string fileName = "Undefined ";
   bool alreadyRecorded=false;
      m_fileCurrentlyOpened=false;
      xAOD::LumiBlockRangeContainer::const_iterator it;
      for(it=m_cacheInputRangeContainer->begin(); it!=m_cacheInputRangeContainer->end(); it++) {
	xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
        m_cacheOutputRangeContainer->push_back(iovr);
      }
      m_cacheInputRangeContainer->clear();

      for(it=m_cacheSuspectInputRangeContainer->begin(); it!=m_cacheSuspectInputRangeContainer->end(); it++) {
	xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
        m_cacheSuspectOutputRangeContainer->push_back(iovr);
      }
      m_cacheSuspectInputRangeContainer->clear();
   return(StatusCode::SUCCESS);
}
StatusCode LumiBlockMetaDataTool::metaDataStop()
{
   std::string fileName = "Undefined ";
   bool alreadyRecorded=false;
     if(m_fileCurrentlyOpened) {
         ATH_MSG_INFO( "MetaDataStop called when input file open: LumiBlock is suspect" );
         xAOD::LumiBlockRangeContainer::const_iterator it;
         for(it=m_cacheInputRangeContainer->begin(); it!=m_cacheInputRangeContainer->end(); it++) {
	   xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
          m_cacheSuspectOutputRangeContainer->push_back(iovr);
	 }
      m_cacheInputRangeContainer->clear();
     }

      StatusCode sc = finishUp();
      if (!sc.isSuccess()) {
         ATH_MSG_INFO( "finishup failed" );
         return StatusCode::FAILURE;
    } 
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
/*
void LumiBlockMetaDataTool::handle(const Incident& inc) {
  // This handle is fired when files are opened or closed.
  // The "MetaDataStop" incident is fired when the final write
  // of metadata to the output store should occur

   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   std::string fileName;
   if (fileInc == 0) { fileName = "Undefined "; }
   else { fileName = fileInc->fileName(); }
   ATH_MSG_INFO( "handle() " << inc.type());
   bool alreadyRecorded=false;
   if (inc.type() == "BeginInputFile") {

     if(m_CurrentFileName==fileName) {
       alreadyRecorded=true;
       }
     if(m_fileCurrentlyOpened) {
       alreadyRecorded=true;
     }
     m_CurrentFileName = fileName;
     if(alreadyRecorded) return;
      m_nfiles++;
      m_fileCurrentlyOpened=true;
      //
      // Look for LB information on input store and transfer it to temporay cache
      // ===========================================================================

      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_LBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_LBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_LBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find unfinished xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheInputRangeContainer->push_back(iovr);
	 }
      }
      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_unfinishedLBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_unfinishedLBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_unfinishedLBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find unfinished xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheInputRangeContainer->push_back(iovr);
	}
      }
      if (m_pInputStore->contains<xAOD::LumiBlockRangeContainer>(m_suspectLBColl_name)) {
	msg(MSG::INFO) << " Contains xAOD::LumiBlockRangeContainer " << m_suspectLBColl_name << endmsg;
	const xAOD::LumiBlockRangeContainer* lbrange =0;
	StatusCode sc = m_pInputStore->retrieve(lbrange,m_suspectLBColl_name);
         if (!sc.isSuccess()) {
            ATH_MSG_INFO( "Could not find suspect xAOD::LumiBlockRangeContainer in input metatdata store" );
	    return;
	 }
	 ATH_MSG_INFO( "xAOD::LumiBlockRangeContainer size" << lbrange->size() );
         for ( const auto* lb : *lbrange ) {
	    xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*lb);
            m_cacheSuspectInputRangeContainer->push_back(iovr);
	 }
      }
   }
   else if (inc.type() == "EndInputFile") {
      m_fileCurrentlyOpened=false;
      xAOD::LumiBlockRangeContainer::const_iterator it;
      for(it=m_cacheInputRangeContainer->begin(); it!=m_cacheInputRangeContainer->end(); it++) {
	xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
        m_cacheOutputRangeContainer->push_back(iovr);
      }
      m_cacheInputRangeContainer->clear();

      for(it=m_cacheSuspectInputRangeContainer->begin(); it!=m_cacheSuspectInputRangeContainer->end(); it++) {
	xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
        m_cacheSuspectOutputRangeContainer->push_back(iovr);
      }
      m_cacheSuspectInputRangeContainer->clear();
   }
   else if (inc.type() == "MetaDataStop") {
     if(m_fileCurrentlyOpened) {
         ATH_MSG_INFO( "MetaDataStop called when input file open: LumiBlock is suspect" );
         xAOD::LumiBlockRangeContainer::const_iterator it;
         for(it=m_cacheInputRangeContainer->begin(); it!=m_cacheInputRangeContainer->end(); it++) {
	   xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
          m_cacheSuspectOutputRangeContainer->push_back(iovr);
	 }
      m_cacheInputRangeContainer->clear();
     }

      StatusCode sc = finishUp();
      if (!sc.isSuccess()) {
         ATH_MSG_INFO( "finishup failed" );
         return;
    } 
   }
}
*/

//__________________________________________________________________________
StatusCode   LumiBlockMetaDataTool::finishUp() {
// 
// stop() is called whenever the event loop is finished.
// ======================================================
   ATH_MSG_VERBOSE(  " finishUp: write lumiblocks to meta data store " );

   xAOD::LumiBlockRangeContainer* piovComplete = new xAOD::LumiBlockRangeContainer();
   xAOD::LumiBlockRangeAuxContainer* piovCompleteAux = new xAOD::LumiBlockRangeAuxContainer();
   piovComplete->setStore( piovCompleteAux );

   xAOD::LumiBlockRangeContainer* piovUnfinished = new xAOD::LumiBlockRangeContainer();
   xAOD::LumiBlockRangeAuxContainer* piovUnfinishedAux = new xAOD::LumiBlockRangeAuxContainer();
   piovUnfinished->setStore( piovUnfinishedAux );

   xAOD::LumiBlockRangeContainer* piovSuspect = new xAOD::LumiBlockRangeContainer();
   xAOD::LumiBlockRangeAuxContainer* piovSuspectAux = new xAOD::LumiBlockRangeAuxContainer();
   piovSuspect->setStore( piovSuspectAux );

   xAOD::LumiBlockRangeContainer::const_iterator it;

   if(m_cacheSuspectOutputRangeContainer->size()>0) {
     ATH_MSG_VERBOSE("Suspect OutputRangeCollection with size " << m_cacheSuspectOutputRangeContainer->size());
     for(it=m_cacheSuspectOutputRangeContainer->begin(); it!=m_cacheSuspectOutputRangeContainer->end(); it++) {
       xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
       piovSuspect->push_back(iovr);
     }
   }

   if(m_cacheOutputRangeContainer->size()>0) {
     ATH_MSG_VERBOSE("OutputRangeCollection with size " << m_cacheOutputRangeContainer->size());
     m_cacheOutputRangeContainer->sort(xAOD::SortLumiBlockRangeByStart());

     //  Use tmp collection to do the merging
     xAOD::LumiBlockRangeContainer* p_tempLBColl = new xAOD::LumiBlockRangeContainer();
     xAOD::LumiBlockRangeAuxContainer* p_tempAuxLBColl = new xAOD::LumiBlockRangeAuxContainer();
     p_tempLBColl->setStore( p_tempAuxLBColl );

     // Sort and Merge LumiBlockRange objects if necessary
     // Merge LumiBlockRange objects for same run and lumiblock
     xAOD::LumiBlockRangeContainer::const_iterator i = m_cacheOutputRangeContainer->begin();
     xAOD::LumiBlockRangeContainer::const_iterator ie = m_cacheOutputRangeContainer->end();
     xAOD::LumiBlockRangeContainer::const_iterator ilast = m_cacheOutputRangeContainer->begin();
     xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*i));
     p_tempLBColl->push_back(iovr);
     ATH_MSG_VERBOSE(  "Push_back tmpLBColl with run  " 
		<< (*i)->startRunNumber() << " LB " << (*i)->startLumiBlockNumber() << " events seen "     
		    << (*ilast)->eventsSeen() << " expected " << (*i)->eventsExpected());
     i++;
     while (i != ie) {
       if( ((*i)->startRunNumber()==(*ilast)->startRunNumber()) &&
           ((*i)->stopRunNumber()==(*ilast)->stopRunNumber()) &&
           ((*i)->startLumiBlockNumber()==(*ilast)->startLumiBlockNumber()) &&
	   ((*i)->stopLumiBlockNumber()==(*ilast)->stopLumiBlockNumber()) ) {

	 if((*ilast)->eventsExpected()!=(*i)->eventsExpected()) {
	   ATH_MSG_WARNING(  "Error: tmpLBColl with run  " << (*i)->startRunNumber() << " LB " << (*i)->startLumiBlockNumber() << " events expected "     
	 	 		  << (*ilast)->eventsExpected() << " and " << (*i)->eventsExpected() );
	 }
         else {
	   ATH_MSG_VERBOSE(  "Merge Run " << (*i)->startRunNumber() << " LB " << (*i)->startLumiBlockNumber() 
			               << " events seen "  << iovr->eventsSeen() << "+" 
                                       << (*i)->eventsSeen() << " and events expected "
				       << iovr->eventsExpected() );

	     iovr->setEventsSeen((*i)->eventsSeen()+iovr->eventsSeen());
	 }
       }
       else {
         iovr = new xAOD::LumiBlockRange(*(*i));

         ATH_MSG_VERBOSE(  "Push_back tmpLBColl with run  " 
		<< iovr->startRunNumber() << " LB " << iovr->startLumiBlockNumber() << " events seen "     
		    << iovr->eventsSeen() << " expected " << iovr->eventsExpected());

     //	 ATH_MSG_INFO(  "Push_back tmpLBColl with run  " << (*i)->startRunNumber() << " LB " << (*i)->startLumiBlockNumber() << " events seen "     
     //	 	 		  << (*ilast)->eventsExpected() << "+" << (*i)->eventsExpected() );
         p_tempLBColl->push_back(iovr);
         ilast = i;
       }
       i++;
     }

     for(it=p_tempLBColl->begin(); it!=p_tempLBColl->end(); it++) {
       xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange(*(*it));
       if((*it)->eventsSeen() == (*it)->eventsExpected() ) {piovComplete->push_back(iovr);}
       else if((*it)->eventsSeen() > (*it)->eventsExpected() ) {piovSuspect->push_back(iovr);}
       else {piovUnfinished->push_back(iovr);}
     }
     delete p_tempLBColl;
   }


  if(piovComplete->size()>0) {
    ATH_MSG_DEBUG( "Number of Complete LumiBlocks:" << piovComplete->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovComplete->begin(); it!=piovComplete->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }

  if(piovUnfinished->size()>0) {
    ATH_MSG_DEBUG( "Number of Unfinished LumiBlocks:" << piovUnfinished->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovUnfinished->begin(); it!=piovUnfinished->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }
  if(piovSuspect->size()>0) {
    ATH_MSG_DEBUG( "Number of Suspect LumiBlocks:"  << piovSuspect->size() );
    xAOD::LumiBlockRangeContainer::const_iterator it;
    for(it=piovSuspect->begin(); it!=piovSuspect->end(); it++) {
      msg(MSG::INFO) << "\t [ ("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << "):("
	  << (*it)->startRunNumber()  << "," << (*it)->startLumiBlockNumber()
	  << ") eventsSeen = " << (*it)->eventsSeen()
	  << ", eventsExpected = " << (*it)->eventsExpected()
	  << " ]"
	  << endmsg;
    }
  }

  // Store the LumiBlockRangeContainer in the metadata store
  // =======================================================
  if(piovComplete->size()>0) {
    ATH_MSG_INFO(  "Write Complete LumiBlocks with size  " <<  piovComplete->size());
    ATH_CHECK( m_pMetaDataStore->record( piovComplete, m_LBColl_name ) );
    ATH_CHECK( m_pMetaDataStore->record( piovCompleteAux, m_LBColl_name + "Aux." ) );
  }
  
  if(piovUnfinished->size()>0) {
    ATH_MSG_INFO(  "Write Unfinished LumiBlocks with size  " <<  piovUnfinished->size());
    ATH_CHECK( m_pMetaDataStore->record( piovUnfinished, m_unfinishedLBColl_name ) );
    ATH_CHECK( m_pMetaDataStore->record( piovUnfinishedAux, m_unfinishedLBColl_name + "Aux." ) );
  }

  if(piovSuspect->size()>0) {
    ATH_MSG_INFO(  "Write Suspect LumiBlocks with size  " <<  piovSuspect->size());
    ATH_CHECK( m_pMetaDataStore->record( piovSuspect, m_suspectLBColl_name ) );
    ATH_CHECK( m_pMetaDataStore->record( piovSuspectAux, m_suspectLBColl_name + "Aux." ) );
  }

  /*
  if (m_storexmlfiles) {
    if(piovComplete->size()>0) {
       TString this_version("30"); // [0-10): ATLRunQuery, [10-20): ntuple production, [20-30): xml merging, [30-40): LumiCalc 
       std::map<TString,TString> metadata;
       metadata["complete"]=TString("complete");
       TString complete   = m_converter->GetSuggestedName(*piovComplete);
       TString subname = "complete"+complete;
       TString filename = "LumiBlockMetaData_" + subname + ".xml";
       Root::TGoodRunsList* pgrl = m_converter->GetGRLObject(*piovComplete,metadata,this_version);
       m_converter->CreateXMLFile(*pgrl,filename.Data());
    }
    if(piovUnfinished->size()>0) {
       TString this_version("30"); // [0-10): ATLRunQuery, [10-20): ntuple production, [20-30): xml merging, [30-40): LumiCalc 
       std::map<TString,TString> metadata;
       metadata["complete"]=TString("unfinished");
       TString unfinished   = m_converter->GetSuggestedName(*piovUnfinished);
       TString subname = "unfinished"+unfinished;
       TString filename = "LumiBlockMetaData_" + subname + ".xml";
       Root::TGoodRunsList* pgrl = m_converter->GetGRLObject(*piovUnfinished,metadata,this_version);
       m_converter->CreateXMLFile(*pgrl,filename.Data());
    }
    if(piovSuspect->size()>0) {
       TString this_version("30"); // [0-10): ATLRunQuery, [10-20): ntuple production, [20-30): xml merging, [30-40): LumiCalc 
       std::map<TString,TString> metadata;
       metadata["suspect"]=TString("suspect");
       TString suspect   = m_converter->GetSuggestedName(*piovSuspect);
       TString subname = "suspect"+suspect;
       TString filename = "LumiBlockMetaData_" + subname + ".xml";
       Root::TGoodRunsList* pgrl = m_converter->GetGRLObject(*piovSuspect,metadata,this_version);
       m_converter->CreateXMLFile(*pgrl,filename.Data());
    }

  }
  */


  return(StatusCode::SUCCESS);
}

const TString
LumiBlockMetaDataTool::getGRLString( const TString& /*grlname*/ ) const
{
  /*
  std::vector< Root::TGoodRunsList >::const_iterator itr = m_grlcollection->find( grlname );
  if (itr!=m_grlcollection->end())
    return m_converter->GetXMLString(*itr);

  ATH_MSG_WARNING("getGRLString() : GoodRunsList with name <" << grlname << "> not found. Return empty string." );
  */
  return "";
}

