/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetTrackParticleThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_VERTEXPARTICLETHINNING_H
#define DERIVATIONFRAMEWORK_VERTEXPARTICLETHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing 
{
  class ExpressionParser;
}
namespace InDet
{
  class IInDetTrackSelectionTool;
}

class IThinningSvc;

namespace DerivationFramework {

  class VertexParticleThinning : public AthAlgTool, public IThinningTool {
    public: 
      VertexParticleThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~VertexParticleThinning();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass; // TODO: fix mutable r22; issue is that doThinning() is a const function
      mutable unsigned int m_nVtxTot,m_nVtxPass; // TODO: fix mutable r22; issue is that doThinning() is a const function
      std::string m_vertexSGKey, m_inDetSGKey, m_selectionString;
      ToolHandle< InDet::IInDetTrackSelectionTool > m_trkSelTool;
      unsigned int m_minGoodTracks; // Minimum number of tracks in a vertex passing TrackSelectionTool
      bool m_and;
      ExpressionParsing::ExpressionParser *m_parser;


  }; 
}

#endif // DERIVATIONFRAMEWORK_JETTRACKPARTICLETHINNING_H

