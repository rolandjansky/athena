/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Top algorithm to decode the BCM BS
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDER_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class IROBDataProviderSvc;
class BCM_RawDataProviderTool;

class BCM_RawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  BCM_RawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize()
    { return StatusCode::SUCCESS; }

  //! Destructor
  ~BCM_RawDataProvider();

private:

  ServiceHandle<IROBDataProviderSvc>   m_robDataProvider;
  ToolHandle<BCM_RawDataProviderTool>  m_rawDataTool;
  std::string                          m_RDO_Key;
};

#endif

