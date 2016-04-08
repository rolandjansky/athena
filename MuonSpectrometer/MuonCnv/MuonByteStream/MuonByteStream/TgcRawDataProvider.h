/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_TGCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_TGCRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class IROBDataProviderSvc;

namespace Muon
{
class IMuonRawDataProviderTool;

class TgcRawDataProvider : public AthAlgorithm
{
public:

    //! Constructor.
    TgcRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

    //! Initialize
    virtual StatusCode initialize();

    //! Execute
    virtual StatusCode execute();

    //! Finalize
    virtual StatusCode finalize();

    //! Destructur
    ~TgcRawDataProvider();

private:
    ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;
    ToolHandle<Muon::IMuonRawDataProviderTool>  m_rawDataTool;
    std::vector<uint32_t>                       m_robIds;
};
} // ns end

#endif



