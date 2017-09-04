/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_Thin_vtxDuplicates_H
#define DERIVATIONFRAMEWORK_Thin_vtxDuplicates_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"

class IThinningSvc;

namespace DerivationFramework {

  class Thin_vtxDuplicates : public AthAlgTool, public IThinningTool {
    public: 
      Thin_vtxDuplicates(const std::string& t, const std::string& n, const IInterface* p);
      ~Thin_vtxDuplicates();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      bool m_noFlags;
      mutable unsigned int m_nVtxTot, m_nVtxPass;
      
      std::string m_trackParticleContainerName; 
      std::vector<std::string> m_vertexContainerNames;
      std::vector<std::string> m_passFlags;
      //double m_acceptanceR;
      bool m_and;
      //bool m_trackAnd;
      //bool m_thinTracks;
  }; 
}

#endif 
