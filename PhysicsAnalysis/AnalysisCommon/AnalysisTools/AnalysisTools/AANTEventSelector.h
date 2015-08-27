/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_AANTEVENTSELECTOR_H
#define ANALYSISTOOLS_AANTEVENTSELECTOR_H

//  AANTEventSelector

// Include files.
#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
# undef _XOPEN_SOURCE
#endif
#include "Python.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "Rtypes.h"

// Forward declarations.
class ISvcLocator;
class StoreGateSvc;
class TChain;

// Class AANTEventSelector.
class AANTEventSelector : virtual public AthService, 
			  virtual public IEvtSelector, 
			  virtual public IProperty 
{
public:

  // Standard Constructor.
  AANTEventSelector(const std::string& name, ISvcLocator* svcloc);
  
  // Standard Destructor.
  ~AANTEventSelector();

  // Implementation of Service base class methods.
  virtual StatusCode initialize();
  
  // Implementation of the IEvtSelector interface methods.
  virtual StatusCode createContext(Context*& it) const;
  virtual StatusCode next(Context& it) const;
  virtual StatusCode next(Context& it, int jump) const;
  virtual StatusCode previous(Context& it) const;
  virtual StatusCode previous(Context& it, int jump) const;
  
  virtual StatusCode last(Context& it) const;
  virtual StatusCode rewind(Context& it) const;

  virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const;
  virtual StatusCode releaseContext(Context*& it) const;
  virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const;

  // Implementation of IInterface methods.
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

private:
  // StoreGateSvc
  StoreGateSvc *m_storeGate;

  // property 
  StringArrayProperty m_inputCollectionsProp;
  
  // Number of events to skip at the beginning 
  int m_skipEvents;

  // Number of Events read so far.
  mutable long m_numEvents;

  // Total number of events
  long m_totalNEvents;

  // name of converter for TTree
  std::string m_strConverter;

  // name of selection criteria
  std::string m_strSelection;

  // TChain
  TChain *m_tree;

  Long64_t m_runNumber;
  Long64_t m_eventNumber;

  // Py I/F
  PyObject *m_convFunc;
  PyObject *m_selectionFunc;
};


struct AANTTreeGate
{
public:
  static void setTree (TChain *chain) { m_tree = chain;}
  static TChain * getTree () { return m_tree; }
private:
  static TChain *m_tree;
};

#endif  
