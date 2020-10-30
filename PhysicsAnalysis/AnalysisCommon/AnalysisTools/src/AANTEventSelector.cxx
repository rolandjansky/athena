/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	AANTEventSelector.cxx
//====================================================================
// 
// Include files.

#include "AnalysisTools/AANTEventSelector.h"
#include "AnalysisTools/AANTEventContext.h"
#include "RootUtils/PyAthenaGILStateEnsure.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h" 

#include "TChain.h"

TChain *AANTTreeGate::m_tree = 0;

// Constructor.
AANTEventSelector::AANTEventSelector(const std::string& name, ISvcLocator* svcloc)
  : AthService(name, svcloc),
    m_storeGate(0),
    m_skipEvents(0), m_numEvents(0), m_totalNEvents(0),
    m_tree(0),
    m_runNumber(0),
    m_eventNumber(0),
    m_convFunc(0), m_selectionFunc(0)
{
  declareProperty("InputCollections",          m_inputCollectionsProp);
  declareProperty("SkipEvents",                m_skipEvents = 0);
  declareProperty("Converter",                 m_strConverter = "");
  declareProperty("Selection",                 m_strSelection = "");
}


// Destructor.
AANTEventSelector::~AANTEventSelector() 
{
  if (m_tree) 
    delete m_tree;
}


// AANTEventSelector::initialize().
StatusCode AANTEventSelector::initialize()
{
  // Initialize the Service base class.
  StatusCode sc = Service::initialize();
  
  // Create a message stream.
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "initialize()" << endmsg;

  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to initialize Service base class" << endmsg;
      return sc;
    }

  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc" << endmsg;
      return sc;
    }
  

  // parse jobO
  if (m_inputCollectionsProp.value().size() == 0)
    {
      log << MSG::ERROR << "Use the property:"
	  << " EventSelector.InputCollections = [ \"<collectionName>\" ] (list of collections)"
	  << endmsg;
      return StatusCode::FAILURE;
    }
  // create TChain
  m_tree = new TChain("CollectionTree");
  std::vector<std::string> inputColl = m_inputCollectionsProp.value();
  std::vector<std::string>::iterator it  = inputColl.begin();
  std::vector<std::string>::iterator itE = inputColl.end();
  for (; it!=itE; ++it)
    {
      log << MSG::DEBUG << "Add : " << *it << endmsg;      
      m_tree->Add(it->c_str());
    }

  // get total number of events
  m_totalNEvents = m_tree->GetEntries();
  
  log << MSG::DEBUG << "Total Events : " << m_totalNEvents << endmsg;      
  
  // RunNumber and EventNumber
  m_tree->SetBranchAddress("EventNumber",&m_eventNumber);
  m_tree->SetBranchAddress("RunNumber",  &m_runNumber);

  // set TTree to AANTTreeGate
  AANTTreeGate::setTree(m_tree);

  // selection criteria
  log << MSG::DEBUG << "Load Sel: " << m_strSelection << " from __main__" << endmsg;      
  char smain[] = "__main__";
#if PY_MAJOR_VERSION < 3
  m_selectionFunc = PyObject_GetAttr(PyImport_AddModule(smain),PyString_FromString(m_strSelection.c_str()));
#else
  m_selectionFunc = PyObject_GetAttr(PyImport_AddModule(smain),PyUnicode_FromString(m_strSelection.c_str()));
#endif
  if (m_selectionFunc == NULL)
    {
      log << MSG::ERROR << "Could not load sel : " << m_strSelection << endmsg;
      return StatusCode::FAILURE;
    }

  log << MSG::DEBUG << "Load Cnv: " << m_strConverter << " from __main__" << endmsg;      
#if PY_MAJOR_VERSION < 3
  m_convFunc = PyObject_GetAttr(PyImport_AddModule(smain),PyString_FromString(m_strConverter.c_str()));
#else
  m_convFunc = PyObject_GetAttr(PyImport_AddModule(smain),PyUnicode_FromString(m_strConverter.c_str()));
#endif
  if (m_convFunc == NULL)
    {
      log << MSG::ERROR << "Could not load conv : " << m_strConverter << endmsg;
      return StatusCode::FAILURE;
    }

  return  StatusCode::SUCCESS;
}


// createContext
StatusCode AANTEventSelector::createContext(IEvtSelector::Context*& it) const
{
  it = new AANTEventContext(this);
  return StatusCode::SUCCESS;
}


// Implementation of IEvtSelector::next().
StatusCode AANTEventSelector::next(IEvtSelector::Context& it)const 
{
  return this->next(it,0);
}


// jump
StatusCode AANTEventSelector::next(IEvtSelector::Context& it, int jump) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "next(" << jump << ") : iEvt " << m_numEvents << endmsg;

  // get EventContext
  AANTEventContext* ct = dynamic_cast<AANTEventContext*>(&it);
  if (ct == 0)
    {
      log << MSG::ERROR << "Could not dcast to AANTEventContext" << endmsg;
      return StatusCode::FAILURE;
    }
  // jump
  if (((m_numEvents + jump) >= 0) and ((m_numEvents + jump) < m_totalNEvents))
    {
      // move pointer
      m_numEvents += jump;
      // get entry
      m_tree->GetEntry(m_numEvents);
      // increment pointer
      ++m_numEvents;
      // convert C++ obj to Python obj
      RootUtils::PyGILStateEnsure gil;
      PyObject *pyObj = PyObject_CallObject(m_convFunc,NULL); 
      // execute Python code fragment
      PyObject *tup = Py_BuildValue((char*)"(O)",pyObj);
      PyObject *ret = PyObject_CallObject(m_selectionFunc,tup); 
      // decrement reference counters
      Py_DECREF(pyObj); 
      Py_DECREF(tup); 
      if (ret != NULL and PyObject_IsTrue(ret))
	{
	  // decrement reference counters
	  Py_DECREF(ret); 
	  // EventInfo
	  EventType *peT = new EventType();
	  EventInfo *evtInfo = new EventInfo( new EventID(m_runNumber,m_eventNumber,0),peT);
	  StatusCode sc = m_storeGate->record(evtInfo, "AANTEventInfo");
	  if (sc.isFailure())
	    {
	      log << MSG::ERROR << "Could not record AANTEventInfo" << endmsg;
	      return sc;
	    }
	  // return
	  return StatusCode::SUCCESS;
	}
      else
	{
	  // decrement reference counters
	  Py_XDECREF(ret); 
	  // next
	  return this->next(it,0);
	}
    }

  // EOF
  //ct = new AANTEventContext(0);
  return StatusCode::FAILURE; 
}


// previous
StatusCode AANTEventSelector::previous(IEvtSelector::Context& it) const
{
  return this->next(it,-1);
}


// previous jump
StatusCode AANTEventSelector::previous(IEvtSelector::Context& it, int jump) const
{
  return this->next(it,-jump);
}


// last
StatusCode AANTEventSelector::last(IEvtSelector::Context& /*it*/) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "AANTEventSelector::last() not implemented" << endmsg;
  return StatusCode::FAILURE;
}


// resetCriteria
StatusCode AANTEventSelector::resetCriteria(const std::string& /*criteria*/, IEvtSelector::Context& /*ctxt*/) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "AANTEventSelector::resetCriteria() not implemented" << endmsg;
  return StatusCode::FAILURE;
}


// rewind
StatusCode AANTEventSelector::rewind(IEvtSelector::Context& /*it*/) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "AANTEventSelector::rewind() not implemented" << endmsg;
  return StatusCode::FAILURE;
}


// createAddress
StatusCode AANTEventSelector::createAddress(const IEvtSelector::Context& /*it*/,
					    IOpaqueAddress*& /*iop*/) const
{
  return StatusCode::SUCCESS;
}


// releaseContext
StatusCode AANTEventSelector::releaseContext(IEvtSelector::Context*& /*it*/) const
{
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "AANTEventSelector::releaseContext() not implemented" << endmsg;
  return StatusCode::FAILURE;
}


// Implementation of IInterface::queryInterface.
StatusCode AANTEventSelector::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (riid == IEvtSelector::interfaceID())  
     {
       *ppvInterface = (IEvtSelector*)this;
     }
  else if (riid == IProperty::interfaceID())
     {
       *ppvInterface = (IProperty*)this;
     }
   else   
     {
       return Service::queryInterface(riid, ppvInterface);
     }
   
   addRef();
   
   return StatusCode::SUCCESS; 
}

