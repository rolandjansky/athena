// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_JetGoodiesFiller_H
#define MissingETGoodness_JetGoodiesFiller_H
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"

#include "MissingETGoodness/JetVarTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MissingETGoodness/Goodies.h"

#include "TString.h"

/////////////////////////////////////////////////////////////////////////////
static const InterfaceID IID_JetGoodiesFiller("JetGoodiesFiller", 1, 0);
struct JetSignalState;

class JetGoodiesFiller : public AthAlgorithm 
{
public:
   JetGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator );
   virtual ~JetGoodiesFiller();
  
   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();
  
private:
  void initGoodiesMap();
  
  static MET::Goodies& s_goodies;
  
  ToolHandle<JetVarTool> m_JetVariables;
  
  //storegate key names
  std::string m_curdir, m_name;
  
  std::string m_PrimaryJetAlg;
  std::string m_SecondaryJetAlg;
  std::string m_LArNoisyROSummary;
  int m_NumberOfJets;

protected:
  friend struct JetSignalState;

};

#endif // MissingETGoodness_JetGoodiesFiller_H

