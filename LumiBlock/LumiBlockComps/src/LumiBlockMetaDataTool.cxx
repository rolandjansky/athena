/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** Modified from @file ReadMeta.cxx
 *  @brief The LumiBlockMetaDataTool reads luminosity metadata from input files and transfers it to output files
 *  @author Marjorie Shapiro <mdshapiro@lbl.gov> based on work from Peter van Gemmeren <gemmeren@anl.gov> 
 *  $Id: LumiBlockMetaDataTool.cxx,v 1.7 2009-05-19 07:51:30 radbal Exp $
 **/

#include "LumiBlockComps/LumiBlockMetaDataTool.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/ITriggerRegistryTool.h"
#include "LumiBlockComps/LumiBlockCollectionConverter.h"
#include "LumiBlockComps/ILumiCalcSvc.h"

// the user data-class defintions
#include "LumiBlockData/LumiBlockCollection.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"
#include "DBDataModel/CollectionMetadata.h"

#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TUniqueGRLString.h"

#include "urldecode.h"
#include "TROOT.h"

#include <algorithm>

//___________________________________________________________________________
LumiBlockMetaDataTool::LumiBlockMetaDataTool(const std::string& type, const std::string& name, const IInterface* parent) : 
   AlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/InputMetaDataStore", name),
   m_tagDataStore   ("StoreGateSvc/TagMetaDataStore", name),
   m_nfiles(0),
   m_fileCurrentlyOpened(false),
   m_lbc(0),
   m_lbr(0),
   m_converter(new LumiBlockCollectionConverter()),
   m_grlcollection(new Root::TGRLCollection()),
   m_lcSvc("LumiCalcSvc/LumiCalcSvc",name),
   m_GoodRunsListSelectorTool("GoodRunsListSelectorTool"),
   m_TriggerRegistryTool("TriggerRegistryTool")
{
   declareInterface<IMetaDataTool>(this);
   declareInterface<ILumiBlockMetaDataTool>(this); 

   declareProperty("LBCollName",m_LBColl_name = "LumiBlocks");
   declareProperty("unfinishedLBCollName",m_unfinishedLBColl_name = "IncompleteLumiBlocks");
   declareProperty("VersionString",m_version = "10");
   declareProperty("createAANT",m_createAANT = false);
   declareProperty("calcLumi", m_calcLumi = false);
   declareProperty("storeXMLFiles", m_storexmlfiles = true);
   declareProperty("applyDQCuts", m_applydqcuts = true);
   declareProperty("lumicalcSvc", m_lcSvc);

   m_cacheLBColl = new LumiBlockCollection();         // Complete LB where EOF has been seen
   m_tempLBColl = new LumiBlockCollection();          // Temp storage while reading the file
   m_unfinishedLBColl = new LumiBlockCollection();    // Incomplete LB's from input file

   m_DQLBColl_name = TString(m_LBColl_name)+"_GoodDQ";
   m_unfinishedDQLBColl_name = TString(m_unfinishedLBColl_name)+"_GoodDQ";
}
  
//___________________________________________________________________________
LumiBlockMetaDataTool::~LumiBlockMetaDataTool() {
  delete m_cacheLBColl; m_cacheLBColl = 0;
  delete m_tempLBColl; m_tempLBColl = 0;
  delete m_unfinishedLBColl; m_unfinishedLBColl = 0;
  if (m_converter!=0)     { delete m_converter; m_converter=0; }
  if (m_grlcollection!=0) { delete m_grlcollection; m_grlcollection=0; }
}

//___________________________________________________________________________
StatusCode LumiBlockMetaDataTool::initialize() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the DetectorStore and initialize our local ptr
   sc = m_pMetaDataStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pMetaDataStore) {
      log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
      return(StatusCode::FAILURE);
   }
   sc = m_pInputStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pInputStore) {
      log << MSG::ERROR << "Could not find InputMetaDataStore" << endreq;
      return(StatusCode::FAILURE);
   }

   sc = m_tagDataStore.retrieve();
   if(!sc.isSuccess()) {
      log << MSG::WARNING << "Could not find TagMetaDataStore" << endreq;
   }

   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   sc = incSvc.retrieve();
   if (!sc.isSuccess()) {
     log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
     return(sc);
   }
   incSvc->addListener(this, "BeginTagFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   incSvc->addListener(this, "EndTagFile", 50); // pri has to be > 10 to be before MetaDataSvc.
   // incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.
   incSvc->addListener(this, "LastInputFile", 50); // pri has to be > 20 to be before MetaDataSvc and AthenaOutputStream.

   sc = m_lcSvc.retrieve();
   if (sc.isFailure()) {
     log << MSG::ERROR << "Cannot get ILumiCalcSvc interface." << endreq;
     return(sc);
   } else {
     log << MSG::DEBUG << "Found ILumiCalcSvc." << endreq;
   }

   // Retrieve the GoodRunsListSelector tool using the ToolHandles
   sc = m_GoodRunsListSelectorTool.retrieve();
   if ( sc.isFailure() ) {
     log << MSG::ERROR << m_GoodRunsListSelectorTool.propertyName() 
         << ": Failed to retrieve tool " << m_GoodRunsListSelectorTool.type() << endreq;
     return StatusCode::FAILURE;
   } else {
     log << MSG::DEBUG << m_GoodRunsListSelectorTool.propertyName() 
         << ": Retrieved tool " << m_GoodRunsListSelectorTool.type() << endreq;
   }

   // Retrieve the TriggerRegistry tool using the ToolHandles
   sc = m_TriggerRegistryTool.retrieve();
   if ( sc.isFailure() ) {
     log << MSG::ERROR << m_TriggerRegistryTool.propertyName()
         << ": Failed to retrieve tool " << m_TriggerRegistryTool.type() << endreq;
     return StatusCode::FAILURE;
   } else {
     log << MSG::DEBUG << m_TriggerRegistryTool.propertyName()
         << ": Retrieved tool " << m_TriggerRegistryTool.type() << endreq;
   }

   // Creating a TTree in the output AANT file, using the TTree::Bronch method
   // see LumiBlockComps/CreateAANTFromLumiBlockCollection example Algorithm class
   if(m_createAANT == true){
     // Initialize histogram service
     sc = serviceLocator()->service("THistSvc", tHistSvc);
     if (sc.isFailure()) {
       log << MSG::ERROR << "Unable to retrieve pointer to THistSvc!" << endreq;
       return sc;
     }
     
     // Create TTree
     AANtree = new TTree("MetaData","MetaData");
     sc=tHistSvc->regTree("/AANT/MetaData",AANtree);
     if(sc.isFailure()){
       log << MSG::ERROR << "Cannot register TTree" << endreq;
     }
     AANtree->Bronch("LumiBlockCollection_p1_LumiBlocks", "LumiBlockCollection_p1", &m_lbc);
   }


   m_cacheLBColl->clear();
   m_tempLBColl->clear();
   m_unfinishedLBColl->clear();

   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode LumiBlockMetaDataTool::finalize() {
   MsgStream log(msgSvc(), name());

   if(m_cacheLBColl->size() >0) {
      log << MSG::INFO << " Luminosity Blocks Recorded in Metadata store: " << endreq;
      m_cacheLBColl->dump();
   }
   else { log << MSG::INFO << "No Complete Luminosity Blocks read in this job" << endreq; }

   if(m_tempLBColl->size()>0 ) {
     log << MSG::INFO << "Incomplete File Reading led to suspect LumiBlocks: " << endreq;
     m_tempLBColl->dump();
   }

   if(m_unfinishedLBColl->size()>0 ) {
     log << MSG::INFO << "Transferring Incomplete LB metadata from input file for suspect LumiBlocks: " << endreq;
     m_unfinishedLBColl->dump();
   }

   m_grlcollection->Summary();

   return(StatusCode::SUCCESS);

}
//__________________________________________________________________________
void LumiBlockMetaDataTool::handle(const Incident& inc) {
   MsgStream log(msgSvc(), name());
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   std::string fileName;
   if (fileInc == 0) { fileName = "Undefined "; }
   else { fileName = fileInc->fileName(); }
   log << MSG::INFO << "handle() " << inc.type() << " for file: " << fileName << endreq;

   // This incident is called whether or not a Tag file is being read.  If there is no Tag file,
   // this incident serves the same purpose as BeginInputFile
   if (inc.type() == "BeginTagFile") {
      //
      // Retrieve LB info from TAG metadata and cache it locally
      // ========================================================
      CollectionMetadata* metadataMap = 0;
      if(!m_tagDataStore->contains<CollectionMetadata>("CollectionMetadata")) return;
      StatusCode status = m_tagDataStore->retrieve(metadataMap, "CollectionMetadata");
      if (!status.isSuccess()) {
         log << MSG::WARNING << "Cannot retrieve Metadata map from TagDataStore." << endreq;
         return;
      }
      std::string myValue = "NotFound";
      CollectionMetadata::iterator iter = metadataMap->find("OutputLumirange");
      if (iter != metadataMap->end()) {
         myValue = iter->second;
      }
      log << MSG::DEBUG << "OutputLumirange: " << myValue << endreq;
      if (myValue != "NotFound") {

        StatusCode status = fillFromXML(m_tempLBColl, myValue);
        if (!status.isSuccess()) {
          log << MSG::WARNING << "fillFromXML failed!" << endreq;
        }

                  log << MSG::DEBUG << "This should now be parsed to get the GoodRunsList" << endreq;
         log << MSG::DEBUG << " Size of CollectionMetadata is: " << metadataMap->size() << endreq;
         for (iter = metadataMap->begin(); iter != metadataMap->end(); iter++) {
            log << MSG::INFO << " first: " << iter->first << " second: "  << iter->second << endreq;
         }
      } else {
         for (CollectionMetadata::iterator cmit = metadataMap->begin(); cmit != metadataMap->end(); cmit++) {
            log << MSG::DEBUG << "CollectionMetadata has " << cmit->first << ", " << cmit->second << endreq;
         }
      }
   } else if (inc.type() == "BeginInputFile") {
      m_nfiles++;
      m_fileCurrentlyOpened=true;
      //
      // Look for LB information on input store and transfer it to temporay cache
      // ===========================================================================
      if (m_pInputStore->contains<LumiBlockCollection>(m_LBColl_name)) {
	 std::list<SG::ObjectWithVersion<LumiBlockCollection> > allVersions;
	 StatusCode sc = m_pInputStore->retrieveAllVersions(allVersions, m_LBColl_name);
         if (!sc.isSuccess()) {
            log << MSG::DEBUG << "Could not find LumiBlockColl in input metatdata store" << endreq;
	    return;
	 }
         for(std::list<SG::ObjectWithVersion<LumiBlockCollection> >::const_iterator iter = allVersions.begin(); iter != allVersions.end(); iter++) {
           const DataHandle<LumiBlockCollection> lbc = iter->dataObject;
           LumiBlockCollection::const_iterator i(lbc->begin()), ie(lbc->end());
           LumiBlockCollection::const_iterator ilast(lbc->begin());
           // Now cache it locally
           m_tempLBColl->reserve(m_tempLBColl->size()+lbc->size());
           if(i!=ie) m_tempLBColl->push_back(new IOVRange(*(*i++)));
           while (i != ie) {
	     if(**i!=**ilast ) { m_tempLBColl->push_back(new IOVRange(*(*i))); }
             else {
	       log << MSG::DEBUG << "Remove duplicate with range " << **i << endreq;
	     }
             ilast = i;
             i++;
	   }
	 }
	 m_tempLBColl->sort(LumiBlockCollection::SortIOVRangeByStart());
         m_tempLBColl->erase( std::unique( m_tempLBColl->begin(), m_tempLBColl->end() ), m_tempLBColl->end() );
      }
      //
      // Look for information on incomplete LB in input store and transfer it to unfinished cache
      // ==========================================================================================
      if (m_pInputStore->contains<LumiBlockCollection>(m_unfinishedLBColl_name)) {
        std::list<SG::ObjectWithVersion<LumiBlockCollection> > allVersions;
	StatusCode sc = m_pInputStore->retrieveAllVersions(allVersions, m_unfinishedLBColl_name);
        if (!sc.isSuccess()) {
          log << MSG::DEBUG << "Could not find Incomplete LumiBlockColl in input metatdata store" << endreq;
	  return;
        }
        for(std::list<SG::ObjectWithVersion<LumiBlockCollection> >::const_iterator iter = allVersions.begin(); iter != allVersions.end(); iter++) {
          const DataHandle<LumiBlockCollection> lbc = iter->dataObject;
          LumiBlockCollection::const_iterator i(lbc->begin()), ie(lbc->end());
          LumiBlockCollection::const_iterator ilast(lbc->begin());
          // Now cache it locally
          m_tempLBColl->reserve(m_unfinishedLBColl->size()+lbc->size());
          if(i!=ie) m_unfinishedLBColl->push_back(new IOVRange(*(*i++)));
          while (i != ie) {
	    if(**i!=**ilast ) { m_unfinishedLBColl->push_back(new IOVRange(*(*i))); }
            else {
	      log << MSG::DEBUG << "Remove duplicate in unfinished collection with range " << **i << endreq;
	    }
            ilast = i;
            i++;
	  }
	}
      }
   }
   else if (inc.type() == "EndTagFile") {
     //
     // The file ended properly.  Move the LB ranges into the cache of completed LB
     // ============================================================================
     if(m_tempLBColl->size() >0 ) {
      m_cacheLBColl->reserve(m_cacheLBColl->size()+m_tempLBColl->size());
      LumiBlockCollection::const_iterator i(m_tempLBColl->begin()), ie(m_tempLBColl->end());
      while (i != ie) {m_cacheLBColl->push_back(new IOVRange(*(*i++)));}
      m_tempLBColl->clear();
     }
      m_fileCurrentlyOpened=false;

   }
   else if (inc.type() == "LastInputFile") {
     finishUp();
   }
   
}

//__________________________________________________________________________
void  LumiBlockMetaDataTool::finishUp() {
// 
// stop() is called whenever the event loop is finished.
// ======================================================
   MsgStream log(msgSvc(), name());
   log << MSG::INFO <<  " finishUp: write lumiblocks to meta data store " << endreq;

   //   if(m_nfiles >1 && m_cacheLBColl->size()>0) 
   if(m_cacheLBColl->size()>0) 
       m_cacheLBColl->sort(LumiBlockCollection::SortIOVRangeByStart());
   //   if(m_nfiles >1 && m_unfinishedLBColl->size()>0)
   if(m_unfinishedLBColl->size()>0)
       m_unfinishedLBColl->sort(LumiBlockCollection::SortIOVRangeByStart());

// 
// Fill the metadata store here for all cases where the full file was read
// =========================================================================
   if(m_cacheLBColl->size()>0) {

//    *** We still don't have checking for duplicates or merging
//    use DV sort instead of std::sort as the latter leaks memory with DV
      LumiBlockCollection* tmpColl = new LumiBlockCollection();

//    We have to do this because the copy constructor of DataVector only
//    copies the pointers...
      LumiBlockCollection::const_iterator i(m_cacheLBColl->begin()), ie(m_cacheLBColl->end());
      tmpColl->push_back(new IOVRange(*(*i++)));
      while (i != ie) { 
        tmpColl->push_back(new IOVRange(*(*i))); 
        i++;
      }

      if(m_createAANT == true){
	// try to persistify it
	m_lbc = m_lbc_conv.createPersistent( tmpColl, log);
      }
      if(tmpColl->size()>0) {
         if (m_pMetaDataStore->contains<LumiBlockCollection>(m_LBColl_name)) {
          DataHandle<LumiBlockCollection> tmp;
          StatusCode ss = m_pMetaDataStore->retrieve(tmp,m_LBColl_name);
          if(ss.isSuccess())  {m_pMetaDataStore->remove(tmp.ptr());}
          else { log << MSG::ERROR <<"Error removing exisiting LB collection from  MetaDataStore " << endreq; }
         }
         StatusCode sc = m_pMetaDataStore->record(tmpColl, m_LBColl_name);
         if (!sc.isSuccess()) {
           delete tmpColl; tmpColl = 0;
           log << MSG::ERROR << "Could not record LumiBlockColl : " << m_LBColl_name << endreq;
	   //return sc;
	 }
         if (m_applydqcuts) {
           sc = this->AddDQCollections(*m_cacheLBColl,m_DQLBColl_name,"luminosity_");
           if (!sc.isSuccess()) { 
             log << MSG::ERROR << "Could not apply DQ selection to LumiBlockColl : " << m_LBColl_name << endreq;
             //return sc; 
           }
         }
	 if(m_createAANT == true){
	   AANtree->Fill();
	 }
      } else {
         delete tmpColl; tmpColl = 0;
      }
   }

//
// Handle the case where a file is partially read in this job or previous jobs
// ============================================================================
   if (m_tempLBColl->size()>0 || m_unfinishedLBColl->size()>0) {

      // collect incomplete LB collections
      LumiBlockCollection* tmp2Coll = new LumiBlockCollection();
      if (m_tempLBColl->size()>0 || m_unfinishedLBColl->size()>0) {
        LumiBlockCollection::const_iterator i(m_tempLBColl->begin()), ie(m_tempLBColl->end());
        while (i != ie) { tmp2Coll->push_back(new IOVRange(*(*i++)));}
        LumiBlockCollection::const_iterator j(m_unfinishedLBColl->begin()), je(m_unfinishedLBColl->end());
        while (j != je) { tmp2Coll->push_back(new IOVRange(*(*j++)));}
        tmp2Coll->sort(LumiBlockCollection::SortIOVRangeByStart());
      }

      StatusCode sc = m_pMetaDataStore->record(tmp2Coll, m_unfinishedLBColl_name);
      if(m_createAANT == true){
	// try to persistify it
	m_lbc = m_lbc_conv.createPersistent( tmp2Coll, log);
      }
      if (!sc.isSuccess()) {
        log << MSG::ERROR << "Could not record Unfinished LumiBlockColl : " << m_unfinishedLBColl_name << endreq;
        //return sc;
      }
      if (m_applydqcuts) {
        sc = this->AddDQCollections(*tmp2Coll,m_unfinishedDQLBColl_name,"incomplete_luminosity_");
        if (!sc.isSuccess()) { 
          log << MSG::ERROR << "Could not apply DQ selection to LumiBlockColl : " << m_unfinishedLBColl_name << endreq;
          //return sc; 
        }
      }
      if(m_createAANT == true){
	AANtree->Fill();
      }
   }

   // process xml string/file
   m_grlxmlstring = m_converter->GetXMLString(*m_grlcollection);
   Root::TUniqueGRLString& uniquestr = Root::TUniqueGRLString::instance();
   uniquestr.SetTo(m_grlxmlstring);
   if (m_storexmlfiles) {
     TString subname;
     TString complete   = m_converter->GetSuggestedName(*m_cacheLBColl);
     TString incomplete = m_converter->GetSuggestedName(*m_unfinishedLBColl);
     if (complete=="grl_empty" && incomplete!="grl_empty") { subname = incomplete; }
     else { subname = complete; }
     TString filename = "luminosity_" + subname + ".xml";
     m_converter->CreateXMLFile(*m_grlcollection,filename.Data());
   }

   //
   // Call LumiCalcSvc to calculate integrated luminosity
   // ============================================================================
   if(m_calcLumi == true){
     StatusCode sc = m_lcSvc->calcLumi();
     if (!sc.isSuccess()) {
       log << MSG::WARNING << "Couldn't calculate lumi!" << endreq;
       //return sc;
     }
   }

   //return(StatusCode::SUCCESS);
}


LumiBlockCollection* 
LumiBlockMetaDataTool::GetCopyOfCollection( const LumiBlockCollection& lbc )
{
  LumiBlockCollection* copyColl = new LumiBlockCollection();
  LumiBlockCollection::const_iterator itr(lbc.begin()), end(lbc.end());
  for (; itr!=end; ++itr) { copyColl->push_back(new IOVRange(*(*itr))); }
  copyColl->sort(LumiBlockCollection::SortIOVRangeByStart());

  return copyColl;
}


LumiBlockCollection* 
LumiBlockMetaDataTool::FilterOnDQFlags( const LumiBlockCollection& lbc,
                                        const std::vector<std::string>& grlnameVec,
                                        const std::vector<std::string>& brlnameVec )
{
  LumiBlockCollection* iovc = new LumiBlockCollection();

  bool somethingtostore(false);
  int lastoklbnr(-1), cur_runnbr(-1), cur_lbstart(-1), cur_lbstop(-1);

  for( LumiBlockCollection::const_iterator it=lbc.begin(); it != lbc.end(); ++it ) {
    const IOVRange* iovr = (*it);
    cur_runnbr = iovr->start().run();
    cur_lbstart = iovr->start().event();
    cur_lbstop = iovr->stop().event();

    for (int lbnr=cur_lbstart; lbnr<=cur_lbstop; ++lbnr) {
      bool lbOk = m_GoodRunsListSelectorTool->passRunLB(cur_runnbr,lbnr,grlnameVec,brlnameVec);
      
      if (lbOk) {
        lastoklbnr=lbnr; // store last known good lb's number 
        if (!somethingtostore) { // fresh lb-range?
	  somethingtostore=true;
	  cur_lbstart = lbnr;
        }
      } else if (somethingtostore) {
        iovc->push_back( new IOVRange(IOVTime(cur_runnbr, cur_lbstart),IOVTime(cur_runnbr, lastoklbnr)) );
        somethingtostore=false;
      }
    }
    // store remaining good iovrange
    if (somethingtostore) {
      iovc->push_back( new IOVRange(IOVTime(cur_runnbr, cur_lbstart),IOVTime(cur_runnbr, lastoklbnr)) );
      somethingtostore=false;
    }
  } 
  iovc->sort(LumiBlockCollection::SortIOVRangeByStart());

  return iovc;
}

StatusCode LumiBlockMetaDataTool::fillFromXML(LumiBlockCollection* lbc_target,
                                   const std::string& xmlString) {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "Entering LumiBlockMetaDataTool::fillFromXML with xmlString " << xmlString << endreq;
   log << MSG::INFO << "Try decoding it" << endreq;
   std::string decoded_string;
   urldecode::decoder_state state;
   state.state = urldecode::ST_SYM;
   std::pair<std::string,int> decoded = urldecode::urldecode(&state, xmlString, xmlString.size());
   decoded_string = decoded.first;
   if (decoded.second < 0) {
      log << MSG::INFO << "URL Decoding failed : " << decoded_string << " . Trying undecoded string instead." << endreq;
      decoded_string = xmlString;
      //return StatusCode::FAILURE;
   }

   // Use parser from GoodRunsLists package (Root based)
   Root::TGoodRunsListReader reader;

   log << MSG::DEBUG << "Now parsing xml string " << decoded_string << endreq;
   reader.SetXMLString(decoded_string);

   if (!reader.Interpret()) {
      log << MSG::WARNING << "GRL interpretation failed : " << decoded_string << " . Return Failure." << endreq;
      return StatusCode::FAILURE;
   } else {
      log << MSG::DEBUG << "GRL interpretation successful." << endreq;
   }

   // Since parse worked, get the good run list
   Root::TGoodRunsList grl = reader.GetMergedGoodRunsList();
   // A GoodRunsList is simply a map of run to list of lumiblock ranges

   // Iterate over runs
   
   for (std::map<Int_t,Root::TGoodRun>::const_iterator it = grl.begin(); it != grl.end(); it++) {
      int run = it->first;
      // Iterate over lumiblock ranges for that run and fill LumiBlockCollection argument
      log << MSG::INFO << "About to fill LBCollection with " << it->second.size() << "items" << endreq;
      for (std::vector<Root::TLumiBlockRange>::const_iterator lbrit = it->second.begin(); lbrit != it->second.end(); lbrit++) {
         lbc_target->push_back(new IOVRange(IOVTime(run, lbrit->Begin()), IOVTime(run, lbrit->End())));
      }
      // Ready for next LumiBlockCollection
   } // grl loop
   return StatusCode::SUCCESS;
}


StatusCode
LumiBlockMetaDataTool::RecordInMetaDataStore( LumiBlockCollection* plbc, const TString& lbcname )
{
  MsgStream log(msgSvc(), name());

  StatusCode sc = StatusCode::SUCCESS;
  if (plbc->empty()) { 
    log << MSG::DEBUG <<"RecordInMetaDataStore() : Input lbc is empty. Not storing in MetaDataStore " << endreq;
    return sc;
  }

  if (m_pMetaDataStore->contains<LumiBlockCollection>(lbcname.Data())) {
   DataHandle<LumiBlockCollection> tmp;
   StatusCode ss = m_pMetaDataStore->retrieve(tmp,lbcname.Data());
   if(ss.isSuccess())  { m_pMetaDataStore->remove(tmp.ptr()); }
   else { log << MSG::ERROR <<"Error removing exisiting LB collection <" << lbcname << "> from  MetaDataStore " << endreq; }
  }

  sc = m_pMetaDataStore->record(plbc, lbcname.Data());
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not record LumiBlockColl : " << lbcname << endreq;
  } else {
    log << MSG::DEBUG << "RecordInMetaDataStore() : Stored lbc <" << lbcname << "> in MetaDataStore " << endreq;
  }

  return sc;
}


StatusCode
LumiBlockMetaDataTool::AddDQCollections( const LumiBlockCollection& lbc, const TString& lbcprefix, const TString& grlprefix )
{
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  if (lbc.empty()) return sc;

  /// construct metadata from triggers passed to lumicalcsvc
  /// may be overwritten below
  //std::map<TString,TString> metadata;
  //for (unsigned int j=0; j<m_lcSvc->getTriggerList().size(); ++j)
  //  metadata[Form("TriggerChain%d",j)] = m_lcSvc->getTriggerList()[j];

  const std::map< std::string, vvPair >& grlselectors = m_GoodRunsListSelectorTool->getGRLSelectorRegistry();
  const std::map< TString, tvtPair >& trigselectors = m_TriggerRegistryTool->getTriggerSelectorRegistry();
  std::map< TString, tvtPair >::const_iterator trItr;
  LumiBlockCollection* iovc(0);
  Root::TGoodRunsList* grl(0);
  TString lbcname, grldescription, grlquery, trigselector, trigselection;
  std::list<TString> trignames;

  // multiple final states, specific grl selectors have been defined.
  if (!grlselectors.empty()) {

    std::map< std::string, vvPair >::const_iterator gsItr = grlselectors.begin(); 
    for (int i=0; gsItr!=grlselectors.end(); ++gsItr, ++i) {

      // get grl selection info
      const std::vector<std::string>& grlnameVec = gsItr->second.first;
      const std::vector<std::string>& brlnameVec = gsItr->second.second;

      // get trigger selection info
      trigselector=gsItr->first; 
      trItr = trigselectors.find(trigselector);
      trigselection = (trItr!=trigselectors.end() ? trItr->second.first : "");
      trignames = (trItr!=trigselectors.end() ? trItr->second.second : std::list<TString>());

      // get lbc with DQ criteria applied and store in metadata
      iovc = this->FilterOnDQFlags(lbc,grlnameVec,brlnameVec) ;
      lbcname = lbcprefix + Form("%d",i);
      sc = this->RecordInMetaDataStore(iovc,lbcname); // iovc taken over by metadatastore
      if (!sc.isSuccess()) { 
        log << MSG::ERROR << "Could not store in metadatastore LumiBlockColl: " << lbcname << endreq;
        return sc; 
      }

      // make corresponding grl, store below
      grlquery = "";
      grl = m_converter->GetGRLObject(*iovc,std::map<TString,TString>(),m_version.c_str());
      grl->SetName(lbcname.Data());
      grldescription = grlprefix + gsItr->first;

      // only one input grl: copy metadata from there
      if (grlnameVec.size()==1) 
        if ( m_GoodRunsListSelectorTool->getGRLCollection()->HasGoodRunsList(grlnameVec[0]) ) 
          grl->SetMetaData( m_GoodRunsListSelectorTool->getGRLCollection()->find(grlnameVec[0])->GetMetaData() );

      // add descr name to metadata
      grl->AddMetaData("Description",grldescription);

      // add additional grl metadata
      std::vector<std::string>::const_iterator gitr;
      int gj;
      for (gj=0, gitr=grlnameVec.begin(); gitr!=grlnameVec.end(); ++gj, ++gitr) { 
        grlquery += "_OR_"+(*gitr); 
        grl->AddMetaData(Form("GoodRunsList%d",gj),(*gitr));
      }
      for (gj=0, gitr=brlnameVec.begin(); gitr!=brlnameVec.end(); ++gj, ++gitr) { 
        grlquery += "_AND_NOT_"+(*gitr); 
        grl->AddMetaData(Form("BadRunsList%d",gj),(*gitr));
      }
      if (!grlquery.IsNull()) {
        grl->AddMetaData("GRLQuery",grlquery);
        grldescription += " __ GRLQuery = "+grlquery;
      }

      // add additional trigger metadata
      std::list<TString>::iterator titr=trignames.begin(); 
      for (int tj=0; titr!=trignames.end(); ++titr, ++tj)
        grl->AddMetaData(Form("TriggerName%d",tj),*titr);
      if (!trigselection.IsNull()) {
        grl->AddMetaData("SelectionTrigger",trigselection);
        grldescription += " __ TriggerSelection = "+trigselection;
      }

      // and store grl
      if (!grl->IsEmpty()) {
        grl->Compress(); // MB: delete duplicate LBs, seen on first data
        m_grlcollection->push_back(*grl);
      }
      delete grl;

      // register corresponding lb collection in lumicalcsvc
      m_lcSvc->registerLBCollection(lbcname,grldescription,trignames);
    }

  // grlselector tool run as preeventselector tool, one grl selection only
  } else {

    // get lbc with DQ criteria applied and store in metadata
    iovc = this->FilterOnDQFlags(lbc) ;
    lbcname = lbcprefix + "0";
    sc = this->RecordInMetaDataStore(iovc,lbcname); // iovc taken over by metadatastore
    if (!sc.isSuccess()) { 
      log << MSG::ERROR << "Could not store in metadatastore LumiBlockColl: " << lbcname << endreq;
      return sc; 
    }

    // get trigger selection info
    trItr = trigselectors.begin(); // one instance of GRL tool, so assume one trigger selection only
    trigselection = (trItr!=trigselectors.end() ? trItr->second.first : "");
    trignames = (trItr!=trigselectors.end() ? trItr->second.second : std::list<TString>());

    // construct corresponding grl
    grlquery = "";
    grl = m_converter->GetGRLObject(*iovc,std::map<TString,TString>(),m_version.c_str());
    grl->SetName(lbcname.Data());
    grldescription = grlprefix + "uniquegrlselector";

    // copy existing metadata
    if ( m_GoodRunsListSelectorTool->getGRLCollection()->size()==1 )
      grl->SetMetaData( m_GoodRunsListSelectorTool->getGRLCollection()->begin()->GetMetaData() );

    // add descr name to metadata
    grl->AddMetaData("Description",grldescription);

    // add additional grl metadata
    std::vector<Root::TGoodRunsList>::const_iterator gitr;
    int gj;
    for (gj=0, gitr=m_GoodRunsListSelectorTool->getGRLCollection()->begin(); gitr!=m_GoodRunsListSelectorTool->getGRLCollection()->end(); ++gj, ++gitr) {
      grlquery += TString("_OR_")+gitr->GetName();
      grl->AddMetaData(Form("GoodRunsList%d",gj),gitr->GetName());
    }
    for (gj=0, gitr=m_GoodRunsListSelectorTool->getBRLCollection()->begin(); gitr!=m_GoodRunsListSelectorTool->getBRLCollection()->end(); ++gj, ++gitr) {
      grlquery += TString("_AND_NOT_")+gitr->GetName();
      grl->AddMetaData(Form("BadRunsList%d",gj),gitr->GetName());
    }
    if (!grlquery.IsNull()) {
      grl->AddMetaData("GRLQuery",grlquery);
      grldescription += " __ GRLQuery = "+grlquery;
    }

    // add additional trigger metadata
    std::list<TString>::iterator titr=trignames.begin();
    for (int tj=0; titr!=trignames.end(); ++titr, ++tj)
      grl->AddMetaData(Form("TriggerName%d",tj),*titr);
    if (!trigselection.IsNull()) {
      grl->AddMetaData("SelectionTrigger",trigselection);
      grldescription += " __ TriggerSelection = "+trigselection;
    }

    // store grl
    if (!grl->IsEmpty()) {
      grl->Compress(); //// MB: delete duplicate LBs. Seen when merging dpds
      m_grlcollection->push_back(*grl);
    }
    delete grl;

    // register corresponding lb collection in lumicalcsvc
    m_lcSvc->registerLBCollection(lbcname,grldescription,trignames);
  }

  return sc;
}


const TString
LumiBlockMetaDataTool::getGRLString( const TString& grlname ) const
{
  std::vector< Root::TGoodRunsList >::const_iterator itr = m_grlcollection->find( grlname );
  if (itr!=m_grlcollection->end())
    return m_converter->GetXMLString(*itr);

  MsgStream log(msgSvc(), name());
  log << MSG::WARNING <<"getGRLString() : GoodRunsList with name <" << grlname << "> not found. Return empty string." << endreq;

  return "";
}

