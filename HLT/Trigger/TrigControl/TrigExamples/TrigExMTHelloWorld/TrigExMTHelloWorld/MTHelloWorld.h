/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

/////////////////////////////////////////////////////////////////////////////

class MTHelloWorld : public Algorithm {
public:
  MTHelloWorld (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  virtual StatusCode beginRun();
  virtual StatusCode endRun();
  virtual StatusCode start();
  virtual StatusCode stop();
  
private:
  MsgStream m_log;
  IntegerProperty      m_myInt;
  BooleanProperty      m_myBool;
  DoubleProperty       m_myDouble;
  StringArrayProperty  m_myStringVec;
};
