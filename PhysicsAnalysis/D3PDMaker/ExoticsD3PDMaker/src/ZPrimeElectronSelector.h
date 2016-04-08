/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ZPrimeElectronSelector.h 651267 2015-03-03 16:40:48Z ssnyder $
/**
 * @file ExoticsD3PDMaker/src/ZPrimeElectronSelector.h
 * @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
 * @date Jan, 2011
 * @brief select high pt electrons, and filter with electrons.
 */

#ifndef EXOTICSD3PDMAKER_ZPRIMEELECTRONSELECTOR_H
#define EXOTICSD3PDMAKER_ZPRIMEELECTRONSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"

namespace D3PD{

class ZPrimeElectronSelector : public AthFilterAlgorithm
{
public:
  ZPrimeElectronSelector (const std::string& name,
            ISvcLocator* svcloc);


  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

private:

  ///container keys
  std::string  m_inCollKey;
  std::string  m_outCollKey;
  
  ///variables for event selection
  unsigned int m_minNumberPassed;
  unsigned int m_maxNumberPassed;

  ///variables for electron selection
  double m_ptMin;
  double m_etMin;
  double m_clPtMin;
  double m_reclPtMin;
  double m_siHitsMin;

  /// Flag to dump the content of StoreGate after each event
  //bool m_dumpStoreGate;

  /// Internal event counter
  unsigned long m_nEventsProcessed;
  unsigned long m_nEventsSelected;

  /// Average of number of electrons
  unsigned long m_nElectronBefore;
  unsigned long m_nElectronAfter;
};

}//D3PD

#endif //
