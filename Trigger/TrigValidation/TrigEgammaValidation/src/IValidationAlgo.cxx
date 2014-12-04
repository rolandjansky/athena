/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "TrigEgammaValidation/IValidationAlgo.h"
#include "TrigEgammaValidation/ValidationException.h"

using namespace std;

IValidationAlgo::IValidationAlgo(const std::string &myname, ISvcLocator *svcloc, const std::string &myfile)
  : Algorithm(myname, svcloc), m_log(0) {
  m_log = new MsgStream(msgSvc(), name());
  m_file = myfile;
}

IValidationAlgo::~IValidationAlgo() {
  if (m_log)
    delete m_log;
}

void IValidationAlgo::cd(const std::string &dir) {
  // Check if it exists!
  m_currentDir = dir;
}

void IValidationAlgo::addDirectory(const std::string &dir) {
  m_dir.push_back(dir);
  cd(dir);
}

void IValidationAlgo::addHistogram(TH1 *h, const std::string &dir) {
  if (!h)
    throw ValidationException("IValidationAlgo::addHistogram(TH1 *h == 0, ...)!");
  
  std::vector<std::string>::iterator dirItr;
  std::string theDir;
  if (dir == "") {
    theDir = m_currentDir;
  } else {
    theDir = dir;
  }
  
  dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
  if (dirItr == m_dir.end())
    throw ValidationException(std::string(std::string("Adding histogram ") + h->GetName()) + std::string(" failed: no directory named ") + theDir);

  std::stringstream ss;
  ss << "/" << m_file << "/" << theDir << "/" << h->GetName();
  
  StatusCode sc;
  sc = m_histsvc->regHist(ss.str(), h);
  if (sc.isFailure()) {
    throw ValidationException(std::string("Failure registering histogram ") + ss.str());
  }

  m_hist1.insert(std::pair<std::string, TH1 *>(ss.str(), h));
}

void IValidationAlgo::addHistogram(TH2 *h, const std::string &dir) {
  if (!h)
    throw ValidationException("IValidationAlgo::addHistogram(TH2 *h == 0, ...)!");
  
  std::vector<std::string>::iterator dirItr;
  std::string theDir;
  if (dir == "") {
    theDir = m_currentDir;
  } else {
    theDir = dir;
  }
  
  dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
  if (dirItr == m_dir.end())
    throw ValidationException(std::string(std::string("Adding histogram ") + h->GetName()) + std::string(" failed: no directory named ") + theDir);

  std::stringstream ss;
  ss << "/" << m_file << "/" << theDir << "/" << h->GetName();
  
  StatusCode sc;
  sc = m_histsvc->regHist(ss.str(), h);
  if (sc.isFailure()) {
    throw ValidationException(std::string("Failure registering histogram ") + ss.str());
  }

  m_hist2.insert(std::pair<std::string, TH2 *>(ss.str(), h));
}

void IValidationAlgo::addTree(TTree *t, const std::string &dir) {
  if (!t)
    throw ValidationException("IValidationAlgo::addTree(TTree *t == 0, ...)!");
  
  std::vector<std::string>::iterator dirItr;
  std::string theDir;
  if (dir == "") {
    theDir = m_currentDir;
  } else {
    theDir = dir;
  }
  
  dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
  if (dirItr == m_dir.end())
    throw ValidationException(std::string(std::string("Adding tree ") + t->GetName()) + std::string(" failed: no directory named ") + theDir);

  std::stringstream ss;
  ss << "/" << m_file << "/" << theDir << "/" << t->GetName();
  
  StatusCode sc;
  sc = m_histsvc->regTree(ss.str(), t);
  if (sc.isFailure()) {
    throw ValidationException(std::string("Failure registering tree ") + ss.str());
  }

  m_tree.insert(std::pair<std::string, TTree *>(ss.str(), t));
}

void IValidationAlgo::getHistsFromPath(const std::vector<std::string> &pattern, const std::vector<std::string> &notpattern, std::map<std::string, TH1 *> &ret) {
  for (std::map<std::string, TH1 *>::const_iterator i = m_hist1.begin(); i != m_hist1.end(); i++) {
    bool goodToGo = true;
    for (std::vector<std::string>::const_iterator pat_it = pattern.begin(); pat_it != pattern.end(); pat_it++) {
      if (i->first.find(*pat_it) == std::string::npos) {
        goodToGo = false;
	break;
      }
    }
    
    if (!goodToGo)
      continue;
      
    for (std::vector<std::string>::const_iterator pat_it = notpattern.begin(); pat_it != notpattern.end(); pat_it++) {
      if (i->first.find(*pat_it) != std::string::npos) {
        goodToGo = false;
	break;
      }
    }
    if (goodToGo)
      ret.insert(std::pair<std::string, TH1 *>(i->first, i->second));
  }
}

std::string IValidationAlgo::getPath(const std::string &histName, const std::string &dir) {
  std::string theDir;
  
  std::vector<std::string>::iterator dirItr;
  if (dir == "") {
    theDir = m_currentDir;
  } else {
    theDir = dir;
  }
  
  dirItr = std::find(m_dir.begin(), m_dir.end(), theDir);
  if (dirItr == m_dir.end())
    throw ValidationException(std::string(std::string("Getting object ") + histName) + std::string(" failed: no directory named ") + theDir);

  std::stringstream ss;
  ss << "/" << m_file << "/" << theDir << "/" << histName;
  return ss.str();
}

TH1 *IValidationAlgo::hist1(const std::string &histName, const std::string &dir) {
  std::string fullPath = getPath(histName, dir);
  std::map<std::string, TH1 *>::iterator histItr = m_hist1.find(fullPath);
  if (histItr == m_hist1.end())
    throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
  if (!histItr->second)
    throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

  return histItr->second;
}

TH2 *IValidationAlgo::hist2(const std::string &histName, const std::string &dir) {
  std::string fullPath = getPath(histName, dir);
  std::map<std::string, TH2 *>::iterator histItr = m_hist2.find(fullPath);
  if (histItr == m_hist2.end())
    throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! No such object!"));
  if (!histItr->second)
    throw ValidationException(std::string(std::string("Getting histogram with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

  return histItr->second;
}
TTree *IValidationAlgo::tree(const std::string &treeName, const std::string &dir) {
  std::string fullPath = getPath(treeName, dir);
  std::map<std::string, TTree *>::iterator treeItr = m_tree.find(fullPath);
  if (treeItr == m_tree.end())
    throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! No such object!"));
  if (!treeItr->second)
    throw ValidationException(std::string(std::string("Getting tree with path ") + fullPath) + std::string(" failed! It is a NULL pointer!"));

  return treeItr->second;
}

StatusCode IValidationAlgo::initialize() {
  StatusCode sc;

  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
    *m_log << MSG::FATAL << "Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  }
 
  sc = service("THistSvc", m_histsvc);
  if( sc.isFailure() ) {
    *m_log << MSG::FATAL << "Unable to locate Service THistSvc" << endreq;
    return sc;
  }

  if ((sc = service("ToolSvc", m_toolSvc)).isFailure()) {
    (*m_log) << MSG::ERROR << "Unable to get ToolSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  // Clear map of histograms first ... don't delete anything, as ROOT takes care of this!
  if (m_hist1.size() != 0)
    m_hist1.clear();
  if (m_hist2.size() != 0)
    m_hist2.clear();
  if (m_tree.size() != 0)
    m_tree.clear();

  try {
    sc = childInitialize();
  } catch(const ValidationException &e) {
    *m_log << MSG::ERROR << "Exception thrown: " << e.msg() << endreq;
    return StatusCode::FAILURE;
  } catch(...) {
    *m_log << MSG::ERROR << "Unknown exception caught, while initializing" << endreq;
    return StatusCode::FAILURE;
  }
  return sc;
}

StatusCode IValidationAlgo::execute() {
  StatusCode sc = StatusCode::FAILURE;
  try {
    *m_log << MSG::INFO << "Running execute() for " << name() << endreq;
    sc = childExecute();
  } catch(const ValidationException &e) {
    *m_log << MSG::ERROR << "Exception thrown: " << e.msg() << endreq;
    return StatusCode::FAILURE;
  } catch(...) {
    sc.ignore();
    *m_log << MSG::ERROR << "Unknown exception caught, while filling histograms" << endreq;
    return StatusCode::FAILURE;
  }
  return sc;
}

StatusCode IValidationAlgo::finalize() {
  StatusCode sc;
  try {
    sc = childFinalize();
  } catch(const ValidationException &e) {
    *m_log << MSG::ERROR << "Exception thrown: " << e.msg() << endreq;
    return StatusCode::FAILURE;
  } catch(...) {
    *m_log << MSG::ERROR << "Unknown exception caught, while processing histograms" << endreq;
    return StatusCode::FAILURE;
  }
  return sc;
}

