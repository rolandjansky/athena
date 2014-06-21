/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TauD3PDMaker_ElectronIsoUD_h
#define TauD3PDMaker_ElectronIsoUD_h


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace xAOD {
  class ITrackIsolationTool;
}

namespace D3PD
{
    class ICollectionGetterTool;
}

class ElectronIsoUD : public AthAlgorithm
{
    public:
        ElectronIsoUD (const std::string& name, ISvcLocator* svcloc);

        StatusCode initialize();
        StatusCode execute();
    
    private:
        ToolHandle<D3PD::ICollectionGetterTool> m_getter;
        ToolHandle<xAOD::ITrackIsolationTool> m_track_iso_tool;
};


#endif // TauD3PDMaker_ElectronIsoUD_h
