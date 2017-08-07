/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Thin_vtxTrk.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Thin_vtxTrk_H
#define DERIVATIONFRAMEWORK_Thin_vtxTrk_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

class IThinningSvc;

namespace DerivationFramework {

  class Thin_vtxTrk : public AthAlgTool, public IThinningTool {
    public: 
      Thin_vtxTrk(const std::string& t, const std::string& n, const IInterface* p);
      ~Thin_vtxTrk();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      mutable unsigned int m_ntot, m_npass;
      double m_acceptanceR;
      bool m_noFlags;  //To take all entries, regardless of flags
      mutable unsigned int m_nVtxTot, m_nVtxPass;
      
      std::string m_trackParticleContainerName; 
      std::vector<std::string> m_vertexContainerNames;
      std::vector<std::string> m_passFlags;
      bool m_and;
      bool m_trackAnd;
      bool m_thinTracks;
  }; 
}

#endif 
