/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNERRORTOOL_MUONALIGNERROREXAMPLEALG_H
#define MUONALIGNERRORTOOL_MUONALIGNERROREXAMPLEALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "MuonAlignErrorToolInterfaces/IAlignmentErrorTool.h"
#include "TrkToolInterfaces/ITrkAlignmentDeviationTool.h"

namespace Trk {
  class Track;
}

namespace MuonAlign {
  class MuonAlignErrorExampleAlg : public AthAlgorithm {
    public:
      MuonAlignErrorExampleAlg(const std::string& name, ISvcLocator* pSvcLocator);
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();
    private:
      void muonTrack(const Trk::Track* track) const;

      ToolHandle<Trk::ITrkAlignmentDeviationTool> m_alignErrorTool;
      //ToolHandle<MuonAlign::IAlignmentErrorTool> m_alignErrorTool;
  };
}

#endif

