/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Thin_vtxTrk.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Thin_vtxTrk_H
#define DERIVATIONFRAMEWORK_Thin_vtxTrk_H

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/HandleKeyArray.h"
#include "StoreGate/ReadDecorHandleKeyArray.h"
#include <string>

namespace SG{
  template <class T>
  using ThinningHandleKeyArray = HandleKeyArray<ReadHandle<T>, ThinningHandleKey<T>, Gaudi::DataHandle::Reader >;
}

namespace DerivationFramework {

  class Thin_vtxTrk : public AthAlgTool, public IThinningTool {
    public: 
      Thin_vtxTrk(const std::string& t, const std::string& n, const IInterface* p);
      ~Thin_vtxTrk();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      StringProperty m_streamName{ this, "StreamName", "", "Name of the stream being thinned" };
      mutable std::atomic<unsigned int> m_ntot, m_npass;
      double m_acceptanceR;
      mutable std::atomic<unsigned int> m_nVtxTot, m_nVtxPass;
      
      SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerName;
      SG::ThinningHandleKeyArray<xAOD::VertexContainer>        m_vertexContainerName;
      std::vector<std::string> m_passFlags;
      SG::ReadDecorHandleKeyArray<xAOD::VertexContainer> m_passArray{this, "INTERNALARRAY", {}};
      bool m_and;
      bool m_trackAnd;
      bool m_thinTracks;
      bool m_noFlags;  //To take all entries, regardless of flags
  }; 
}

#endif 
