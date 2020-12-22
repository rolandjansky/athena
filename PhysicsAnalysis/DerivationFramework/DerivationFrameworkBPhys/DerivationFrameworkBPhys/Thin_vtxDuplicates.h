/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_Thin_vtxDuplicates_H
#define DERIVATIONFRAMEWORK_Thin_vtxDuplicates_H

#include "xAODTracking/VertexContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadDecorHandleKeyArray.h"
class IThinningSvc;

namespace DerivationFramework {

  class Thin_vtxDuplicates : public AthAlgTool, public IThinningTool {
    public: 
      Thin_vtxDuplicates(const std::string& t, const std::string& n, const IInterface* p);
      ~Thin_vtxDuplicates();
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      bool m_noFlags;
      StringProperty m_streamName{ this, "StreamName", "", "Name of the stream being thinned" };
      mutable std::atomic<unsigned int> m_nVtxTot, m_nVtxPass;
      
      SG::ThinningHandleKey< xAOD::VertexContainer >  m_vertexContainerNames;
      SG::ReadDecorHandleKeyArray<xAOD::VertexContainer> m_passFlags;
      bool m_and;
  }; 
}

#endif 
