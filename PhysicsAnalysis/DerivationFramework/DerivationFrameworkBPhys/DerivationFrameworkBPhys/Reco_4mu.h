/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Reco_4mu.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Reco_4mu_H
#define DERIVATIONFRAMEWORK_Reco_4mu_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "DerivationFrameworkBPhys/FourMuonTool.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "xAODBPhys/BPhysHelper.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

namespace xAOD {
  class BPhysHypoHelper;
}

/** THE reconstruction tool
 */
namespace DerivationFramework {

  class Reco_4mu : public AthAlgTool, public ISkimmingTool {
    public: 
      Reco_4mu(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual bool eventPassesFilter() const;
      
    private:
      /** tools
       */
      void ProcessVertex(xAOD::BPhysHypoHelper&, xAOD::BPhysHelper::pv_type, std::vector<double> trackMasses) const;
      ToolHandle<Trk::V0Tools>                      m_v0Tools;
      ToolHandle<DerivationFramework::FourMuonTool> m_fourMuonTool;
      ToolHandle<Analysis::PrimaryVertexRefitter>   m_pvRefitter;
      
      /** job options
       */
      std::string m_pairName;
      std::string m_quadName;
      std::string m_pvContainerName;
      std::string m_refPVContainerName;
      bool        m_refitPV;
      int m_PV_max;
      int m_DoVertexType;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_4mu_H
