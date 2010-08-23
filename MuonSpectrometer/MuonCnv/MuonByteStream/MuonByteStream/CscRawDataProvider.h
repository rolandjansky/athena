/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_CSCRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class CSCcablingSvc; 
class IROBDataProviderSvc;

namespace Muon {
class IMuonRawDataProviderTool;

class CscRawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  CscRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructor
  ~CscRawDataProvider();


private:

  ToolHandle<Muon::IMuonRawDataProviderTool>  m_rawDataTool;
};
} // ns end

#endif



