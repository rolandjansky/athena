/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGONLINEMONITOR_TRIGL1TOPOWRITEVALDATA_H
#define TRIGONLINEMONITOR_TRIGL1TOPOWRITEVALDATA_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

/**
   @brief Save the L1Topo simulated bits to the HLTResult

   The simulated bits are produced by L1TopoSimulation; they are
   needed to validate the L1Topo triggers.

   This class retrieves the bits (as a LVL1::FrontPanelCTP object from
   TStore) and attaches them to the HLT::HLTResult as anonymous extra
   data.

   Original authors: s.george@rhul.ac.uk , Werner.Wiedenmann@cern.ch
   Update (2017): davide.gerbaudo@gmail.com
 */
class TrigL1TopoWriteValData:public AthAlgorithm {
 public:
  TrigL1TopoWriteValData(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  StatusCode doWriteValData();
  StringProperty m_simTopoCTPLocation;
  StringProperty m_HltResultName;
};

#endif // TRIGONLINEMONITOR_TRIGL1TOPOWRITEVALDATA_H
