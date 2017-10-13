/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoolHistSvc.cxx
// implementation for CoolHistSvc
// Richard Hawkings, begun 22/1/07

#include <sstream>
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
  AthService(name,svc),
  m_par_maxfiles(3),
  p_detstore(0),
  m_poolsvc(0)
{
  // declare properties
  declareProperty("MaxFiles",m_par_maxfiles);
}

CoolHistSvc::~CoolHistSvc() {}

const InterfaceID& CoolHistSvc::type() const {
  return ICoolHistSvc::interfaceID();
}

StatusCode CoolHistSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ICoolHistSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(ICoolHistSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolHistSvc::initialize() {
  // if (StatusCode::SUCCESS!=Service::initialize()) return StatusCode::FAILURE;

  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    ATH_MSG_FATAL("Detector store not found");
    return StatusCode::FAILURE;
  }
  // get PoolSvc
  if (StatusCode::SUCCESS!=service("PoolSvc",m_poolsvc)) {
    ATH_MSG_FATAL("Cannot get PoolSvc");
    return StatusCode::FAILURE;
  }
  // set up ROOT if needed
  if (0==gROOT) {
    static TROOT root("Root","ROOT I/O");
    ATH_MSG_INFO("Initialised ROOT");
  } else {
    ATH_MSG_DEBUG("ROOT already initialised");
  }
  // setup to listen for EndEvent incidents
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  if (StatusCode::SUCCESS!=incSvc.retrieve()) {
    ATH_MSG_FATAL("Unable to get IncidentSvc");
    return StatusCode::FAILURE;
  }
  long int pri=100;
  incSvc->addListener(this,"EndEvent",pri);
  return StatusCode::SUCCESS;
}

StatusCode CoolHistSvc::finalize() {
  ATH_MSG_DEBUG("Closing files");
  closeFiles();
  ATH_MSG_DEBUG("Deleting histograms");
  // delete all histograms under management
  for (HistKeyMap::const_iterator hitr=m_histkeymap.begin();
	 hitr!=m_histkeymap.end();++hitr) {
    ATH_MSG_DEBUG("Delete histogram " << hitr->first << " at " <<
		  (hitr->second).second);
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
    msg(mlevel) << "Cannot retrieve CondAttrListCollection for " <<
      folder << endmsg;
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
      msg(mlevel) << "Channel named " << channelName << 
	" does not exist in folder " << folder << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // calculate key of histogram
  const std::string histkey=getHistKey(folder,channel,histname);
  ATH_MSG_DEBUG("Key of histogram is " << histkey); 

  CondAttrListCollection::const_iterator citr=
    atrcol->chanAttrListPair(channel);
  if (citr==atrcol->end()) {
    msg(mlevel) << "Cannot find valid reference for " << folder << 
      " channel " << channel << endmsg;
    return StatusCode::FAILURE;
  }
  const std::string coolguid=citr->second["fileGUID"].data<std::string>();
  ATH_MSG_DEBUG("Histogram key "+histkey+" has current file GUID "+coolguid);
  
  // now check if we have this key/GUID combination before
  HistKeyMap::iterator hitr=m_histkeymap.find(histkey);
  if (hitr!=m_histkeymap.end()) {
    // seen this histogram before - is GUID still valid?
    ATH_MSG_DEBUG("Already seen histogram with GUID " << 
		  (hitr->second).first);
    if (coolguid==(hitr->second).first) {
      ATH_MSG_DEBUG("Existing histogram still valid");
      hist=(hitr->second).second;
      return StatusCode::SUCCESS;
    } else {
      // remove existing histogram and reference, as out of scope
      ATH_MSG_DEBUG("Removing out-of-scope histogram at " <<
		    (hitr->second).second);
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
  ATH_MSG_DEBUG("Getting object "+histname+" from file");
  pfile->GetObject(histname.c_str(),hist);
  if (hist==0) {
    msg(mlevel) << "Could not load TObject " << histname << endmsg;
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
      ATH_MSG_DEBUG("Disconnecting TDirectory "+histname+" from parent");
      mdir->GetList()->Remove(idir);
      idir->SetMother(0);
    } else {
      ATH_MSG_WARNING("Could not get MotherDir for TDirectory "+histname);
    }
  }
  return StatusCode::SUCCESS;
}

TFile* CoolHistSvc::getFile(const std::string& sguid) {
  ATH_MSG_DEBUG("getFile - lookup PFN for GUID "+sguid);
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
    if (m_filemap.size()>=m_par_maxfiles) closeFiles();
    const std::string pfname=mypfn.Next().pfname();
    ATH_MSG_INFO("Opening "+pfname+" for read");
    // now try to open file
    TFile* tfile;
    try {
      tfile=TFile::Open(pfname.c_str(),"READ");
      if (tfile==0 || !tfile->IsOpen()) {
	delete tfile;
	ATH_MSG_ERROR("Problems opening input file "+pfname);
	return 0;
      }
    }
    catch (std::exception& e) {
      ATH_MSG_ERROR("Exception thrown when trying to open input file "+pfname);
      return 0;
    }
    // check file has correct internal GUID
    TObjString* fguid;
    tfile->GetObject("fileGUID",fguid);
    if (fguid==0) {
      ATH_MSG_ERROR("Input file "+pfname+" has no fileGUID object");
      return 0;
    }
    std::string fguidstr=(fguid->GetString().Data());
    delete fguid;
    ATH_MSG_DEBUG("GUID retrieved from file is "+fguidstr+
		  " expecting "+sguid);
    if (fguidstr!=sguid) {
      // GUIDs do not match
      ATH_MSG_ERROR("GUID retrieved from file "+fguidstr+
		    " does not match that in catalogue "+sguid);
      return 0;
    }
    // file is successfully opened - add to list of open files and return ptr
    m_filemap[sguid]=tfile;
    return tfile;
  } else {
    ATH_MSG_ERROR("No PFN found in catalogue for GUID "+sguid);
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
  ATH_MSG_DEBUG("closeFiles sees " << m_filemap.size() << 
		" files to be closed");
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
	ATH_MSG_DEBUG("Removing referece to TTree " << hitr->first
		      << " as associated file is being closed");
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
      ATH_MSG_ERROR("Exception closing histogram file GUID "+
		    fitr->first);
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
