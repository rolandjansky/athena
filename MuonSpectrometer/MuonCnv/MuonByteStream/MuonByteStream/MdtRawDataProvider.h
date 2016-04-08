/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTRAWDATAPROVIDER_H
#define MUONBYTESTREAM_MDTRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon {
class IMuonRawDataProviderTool;

class MdtRawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  MdtRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~MdtRawDataProvider();


private:
  ToolHandle<Muon::IMuonRawDataProviderTool>  m_rawDataTool;
};
} // ns end

#endif



