/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZPrimeMuonSelector.h 651267 2015-03-03 16:40:48Z ssnyder $
/**
 * @file ExoticsD3PDMaker/src/ZPrimeMuonSelector.h
 * @author Hulin Wang<Hulin.Wang@cern.ch>
 * @date Jan, 2011
 * @brief select high pt muons, and filter with muons.
 */

#ifndef EXOTICSD3PDMAKER_ZPRIMEMUONSELECTOR_H
#define EXOTICSD3PDMAKER_ZPRIMEMUONSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "muonEvent/MuonContainer.h"


namespace D3PD{

class ZPrimeMuonSelector : public AthFilterAlgorithm
{
public:
  ZPrimeMuonSelector (const std::string& name,
            ISvcLocator* svcloc);


  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

private:

  ///container keys
  std::string  m_inCollKey_staco;
  std::string  m_inCollKey_muid;
  std::string  m_outCollKey_staco;
  std::string  m_outCollKey_muid;  

  ///variables for event selection
  unsigned int m_minNumberPassed;
  unsigned int m_maxNumberPassed;

  ///variables for electron selection
  double m_ptMin;

  /// Flag to dump the content of StoreGate after each event
  //bool m_dumpStoreGate;

  /// Internal event counter
  unsigned long m_nEventsProcessed;
  unsigned long m_nEventsSelected;

  /// Average of number of electrons
  unsigned long m_nStacoBefore;
  unsigned long m_nMuidBefore;
  unsigned long m_nStacoAfter;
  unsigned long m_nMuidAfter;
};

}//D3PD

#endif //
