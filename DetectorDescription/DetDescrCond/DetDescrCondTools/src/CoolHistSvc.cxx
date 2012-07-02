/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoolHistSvc.cxx
// implementation for CoolHistSvc
// Richard Hawkings, begun 22/1/07

#include <sstream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PoolSvc/IPoolSvc.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TError.h"
#include "TObjString.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoolHistSvc.h"

CoolHistSvc::CoolHistSvc(const std::string& name, ISvcLocator* svc):
  Service(name,svc),
  par_maxfiles(3),
  p_detstore(0),
  m_poolsvc(0),
  m_log(0)
{
  // declare properties
  declareProperty("MaxFiles",par_maxfiles);
}

CoolHistSvc::~CoolHistSvc() {}

const InterfaceID& CoolHistSvc::type() const {
  return ICoolHistSvc::interfaceID();
}

StatusCode CoolHistSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ICoolHistSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(ICoolHistSvc*)this;
  } else {
    return Service::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistSvc::initialize() {
  if (StatusCode::SUCCESS!=Service::initialize()) return StatusCode::FAILURE;
  m_log=new MsgStream(msgSvc(),name());

  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    *m_log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // get PoolSvc
  if (StatusCode::SUCCESS!=service("PoolSvc",m_poolsvc)) {
    *m_log << MSG::FATAL << "Cannot get PoolSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // set up ROOT if needed
  if (0==gROOT) {
    static TROOT root("Root","ROOT I/O");
    *m_log << MSG::INFO << "Initialised ROOT" << endreq;
  } else {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
      "ROOT already initialised, debug=" << gDebug << endreq;
  }
  // setup to listen for EndEvent incidents
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  if (StatusCode::SUCCESS!=incSvc.retrieve()) {
    *m_log << MSG::FATAL << "Unable to get IncidentSvc" << endreq;
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incSvc->addListener(this,"EndEvent",pri);
  return StatusCode::SUCCESS;
}

StatusCode CoolHistSvc::finalize() {
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "Closing files" << endreq;
  closeFiles();
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "Deleting histograms" << endreq;
  // delete all histograms under management
  for (HistKeyMap::const_iterator hitr=m_histkeymap.begin();
	 hitr!=m_histkeymap.end();++hitr) {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
      "Delete histogram " << hitr->first << " at " <<
      (hitr->second).second << endreq;
    delete (hitr->second).second;
  }
  return StatusCode::SUCCESS;
}

void CoolHistSvc::handle(const Incident& inc) {
  // use EndEvent incidents to close any open files
  if (inc.type()=="EndEvent") {
    if (m_filemap.size()>0) closeFiles();
  }
}

StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const unsigned int channel,
				const std::string& histname, TH1*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,channel,"",histname,phist);
  hist=dynamic_cast<TH1*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const unsigned int channel,
				const std::string& histname, TH2*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,channel,"",histname,phist);
  hist=dynamic_cast<TH2*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const unsigned int channel,
				const std::string& histname, TH3*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,channel,"",histname,phist);
  hist=dynamic_cast<TH3*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getTObject(const std::string& folder, 
				const unsigned int channel,
				const std::string& histname,TObject*& hist) {
  return getHist_i(folder,channel,"",histname,hist);
}

bool CoolHistSvc::objectExists(const std::string& folder,
			       const unsigned int channel,
			       const std::string& histname) {
  TObject* phist;
  return (getHist_i(folder,channel,"",histname,phist,false)==
	  StatusCode::SUCCESS);
}


StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const std::string& channel,
				const std::string& histname, TH1*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,0,channel,histname,phist);
  hist=dynamic_cast<TH1*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const std::string& channel,
				const std::string& histname, TH2*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,0,channel,histname,phist);
  hist=dynamic_cast<TH2*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getHist(const std::string& folder, 
				const std::string& channel,
				const std::string& histname, TH3*& hist) {
  TObject* phist;
  StatusCode sc=getHist_i(folder,0,channel,histname,phist);
  hist=dynamic_cast<TH3*>(phist);
  return sc;
}

StatusCode CoolHistSvc::getTObject(const std::string& folder, 
				const std::string& channel,
				const std::string& histname,TObject*& hist) {
  return getHist_i(folder,0,channel,histname,hist);
}

bool CoolHistSvc::objectExists(const std::string& folder,
			       const std::string& channel,
			       const std::string& histname) {
  TObject* phist;
  return (getHist_i(folder,0,channel,histname,phist,false)==
	  StatusCode::SUCCESS);
}


StatusCode CoolHistSvc::getHist_i(const std::string& folder, 
				  const unsigned int channelId, 
				  const std::string& channelName,
				  const std::string& histname,TObject*& hist,
				  const bool printerr) {
  hist=0;
  // set print level for errors - normally ERROR, but can switch to DEBUG
  MSG::Level mlevel=MSG::ERROR;
  if (!printerr) mlevel=MSG::DEBUG;

  // lookup the GUID of the histogram which is 'current' in COOL
  const CondAttrListCollection* atrcol;
  if (StatusCode::SUCCESS!=p_detstore->retrieve(atrcol,folder)) {
    *m_log << mlevel << "Cannot retrieve CondAttrListCollection for " <<
      folder << endreq;
    return StatusCode::FAILURE;
  }
  // find the channel number to look at
  unsigned int channel=0;
  if (channelName=="") {
    // explicitly specified channel number on input
    channel=channelId;
  } else {
    // channel name - have to lookup to get number
    bool found=false;
    for (CondAttrListCollection::name_const_iterator nitr=atrcol->name_begin();
	 nitr!=atrcol->name_end();++nitr) {
      if (nitr->second==channelName) {
	found=true;
	channel=nitr->first;
      }
    }
    if (!found) {
      *m_log << mlevel << "Channel named " << channelName << 
	" does not exist in folder " << folder << endreq;
      return StatusCode::FAILURE;
    }
  }
  // calculate key of histogram
  const std::string histkey=getHistKey(folder,channel,histname);
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "Key of histogram is " << histkey << endreq;

  CondAttrListCollection::const_iterator citr=
    atrcol->chanAttrListPair(channel);
  if (citr==atrcol->end()) {
    *m_log << mlevel << "Cannot find valid reference for " << folder << 
      " channel " << channel << endreq;
    return StatusCode::FAILURE;
  }
  const std::string coolguid=citr->second["fileGUID"].data<std::string>();
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "Histogram key " << histkey <<
    " has current file GUID" << coolguid << endreq;
  
  // now check if we have this key/GUID combination before
  HistKeyMap::iterator hitr=m_histkeymap.find(histkey);
  if (hitr!=m_histkeymap.end()) {
    // seen this histogram before - is GUID still valid?
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
      "Already seen histogram with GUID " <<
      (hitr->second).first << endreq;
    if (coolguid==(hitr->second).first) {
      if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
	"Existing histogram still valid" << endreq;
      hist=(hitr->second).second;
      return StatusCode::SUCCESS;
    } else {
      // remove existing histogram and reference, as out of scope
      if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
	"Removing out-of-scope histogram at " <<
	(hitr->second).second << endreq;
      delete (hitr->second).second;
      m_histkeymap.erase(hitr);
    }
  }
  // going to have to access ROOT - save current directory state
  GlobalDirectoryRestore restore;
  // check if this file is already open
  FileMap::const_iterator fitr=m_filemap.find(coolguid);
  if (fitr==m_filemap.end()) {
    // file not found - have to open it
    TFile* pfile=getFile(coolguid);
    if (pfile!=0) {
      fitr=(m_filemap.insert(FileMap::value_type(coolguid,pfile)).first);
    } else {
      // was not able to access file
      return StatusCode::FAILURE;
    }
  }
  TFile* pfile=fitr->second;
  // now read the histogram into memory
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "Getting object " << histname << " from file" << endreq;
  pfile->GetObject(histname.c_str(),hist);
  if (hist==0) {
    *m_log << mlevel << "Could not load TObject " << histname << endreq;
    return StatusCode::FAILURE;
  }
  m_histkeymap[histkey]=std::pair<std::string,TObject*>(coolguid,hist);
  // make this histogram unassociated with the TFile, so file can be closed
  // only for histogram objects, others do not get associated
  // TTrees have special treatment
  TH1* ihist=dynamic_cast<TH1*>(hist);
  if (ihist!=0) ihist->SetDirectory(0);
  // if it is a TDirectory, also need special treatment to unassociate parent
  TDirectory* idir=dynamic_cast<TDirectory*>(hist);
  if (idir!=0) {
    TDirectory* mdir=idir->GetMotherDir();
    if (mdir!=0) {
      if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
	"Disconnecting TDirectory " << histname << " from parent" << endreq;
      mdir->GetList()->Remove(idir);
      idir->SetMother(0);
    } else {
      *m_log << MSG::WARNING << "Could not get MotherDir for TDirectory " <<
	histname << endreq;
    }
  }
  return StatusCode::SUCCESS;
}

TFile* CoolHistSvc::getFile(const std::string& sguid) {
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "getFile - lookup PFN for GUID " << sguid << endreq;
  // get the POOL file catalogue service
  pool::IFileCatalog* catalog=
    const_cast<pool::IFileCatalog*>(m_poolsvc->catalog());
  pool::FClookup lookup;
  catalog->setAction(lookup);
  pool::FileCatalog::FileID guid=sguid;
  pool::PFNContainer mypfn(catalog,10);
  lookup.lookupPFN(guid,mypfn);
  if (mypfn.hasNext()) {
    // check not about to exceed max number of open files, close if needed
    if (m_filemap.size()>=par_maxfiles) closeFiles();
    const std::string pfname=mypfn.Next().pfname();
    *m_log << MSG::INFO << "Opening " << pfname << " for read" 
	  << endreq;
    // now try to open file
    TFile* tfile;
    try {
      tfile=TFile::Open(pfname.c_str(),"READ");
      if (tfile==0 || !tfile->IsOpen()) {
	delete tfile;
	*m_log << MSG::ERROR << "Problems opening input file " << pfname << 
	  endreq;
	return 0;
      }
    }
    catch (std::exception& e) {
      *m_log << MSG::ERROR << 
	"Exception thrown when trying to open input file " << pfname << endreq;
      return 0;
    }
    // check file has correct internal GUID
    TObjString* fguid;
    tfile->GetObject("fileGUID",fguid);
    if (fguid==0) {
      *m_log << MSG::ERROR << "Input file " << pfname << 
	" has no fileGUID object" << endreq;
      return 0;
    }
    std::string fguidstr=(fguid->GetString().Data());
    delete fguid;
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "GUID retrieved from file is " << fguidstr << 
      " expecting " << sguid << endreq;
    if (fguidstr!=sguid) {
      // GUIDs do not match
      *m_log << MSG::ERROR << "GUID retrieved from file " << fguidstr << 
      " does not match that in catalogue " << sguid << endreq;
      return 0;
    }
    // file is successfully opened - add to list of open files and return ptr
    m_filemap[sguid]=tfile;
    return tfile;
  } else {
    *m_log << MSG::ERROR << "No PFN found in catalogue for GUID " << sguid 
	<< endreq;
    return 0;
  }
}

std::string CoolHistSvc::getHistKey(const std::string& folder, 
	    const int channel, const std::string& histname) {
  std::ostringstream chanstr;
  chanstr << folder << ":" << channel << ":" << histname;
  return chanstr.str();
}

void CoolHistSvc::closeFiles() {
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
    "closeFiles sees " << m_filemap.size() << " files to be closed" << endreq;
  for (FileMap::iterator fitr=m_filemap.begin();fitr!=m_filemap.end();++fitr) {
    // check to see if any TObjects associated to this file are TTrees
    // if so, delete the reference as the TTree will have disappeared
    // note this must be done before the file is closed
    const std::string& sguid=fitr->first;
    HistKeyMap::iterator hitr=m_histkeymap.begin();
    while (hitr!=m_histkeymap.end()) {
      TTree* ttree=0;
      if ((hitr->second).first==sguid) {
	// entry in this file - check if TObject is a TTree
	ttree=dynamic_cast<TTree*>((hitr->second).second);
      }
      if (ttree!=0) {
	if (m_log->level()<MSG::DEBUG)
	*m_log << MSG::DEBUG << "Removing referece to TTree " << hitr->first
	      << " as associated file is being closed" << endreq;
	m_histkeymap.erase(hitr++);
      } else {
	++hitr;
      }
    }
    // close the file, catch exceptions just in case they have gone stale
    try {
      fitr->second->Close();
      delete fitr->second;
    }
    catch (std::exception& e) {
      *m_log << MSG::ERROR << "Exception closing histogram file GUID " << 
	fitr->first << endreq;
    }
  }
  m_filemap.clear();
}


// directory restore class copied from Gaudi THistSvc

CoolHistSvc::GlobalDirectoryRestore::GlobalDirectoryRestore() {
  m_gd = gDirectory;
  m_gf = gFile;
  m_ge = gErrorIgnoreLevel;
}

CoolHistSvc::GlobalDirectoryRestore::~GlobalDirectoryRestore() {
  gDirectory = m_gd;
  gFile = m_gf;
  gErrorIgnoreLevel = m_ge;
}
