/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBALGOSEQUENCER
#define TBREC_TBALGOSEQUENCER
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthLegacySequence.h"


#include <string>
#include <vector>
#include <map>

class TBAlgoSequencer : public AthLegacySequence
{

  typedef std::string             KeyType;
  typedef std::vector<Gaudi::Algorithm*> AlgoStore;
  typedef AlgoStore::iterator     AlgoIterator;

 public:

  TBAlgoSequencer(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBAlgoSequencer();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:

  ////////////////
  // Properties //
  ////////////////

  std::vector<std::string> m_subAlgoNames;
  AlgoStore  m_subAlgos;
  std::vector<std::string> m_algoNameStore;
  bool m_timingOn;

  ////////////////
  // Statistics //
  ////////////////

  unsigned int m_eventPrintFreq;
  unsigned int m_eventCounter;
  unsigned int m_rejectCounter;
  unsigned int m_rejectNoEvent;
  std::map<std::string,unsigned int> m_rejectPattern;
  std::map<std::string,unsigned int> m_acceptPattern;
};
#endif
