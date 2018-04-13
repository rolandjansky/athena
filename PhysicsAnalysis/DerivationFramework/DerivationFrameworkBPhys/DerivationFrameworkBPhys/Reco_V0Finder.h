/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// Reco_V0Finder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_V0FINDER_H
#define DERIVATIONFRAMEWORK_V0FINDER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "InDetV0Finder/InDetV0FinderTool.h"
#include "HepPDT/ParticleDataTable.hh"

/** forward declarations
 */
namespace Trk
{
  class V0Tools;
  class ParticleDataTable;
}

namespace DerivationFramework {

  class Reco_V0Finder : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_V0Finder(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;
      StatusCode finalize() override;
      
      virtual StatusCode addBranches() const override;
      
    private:
      
      std::vector<std::string> m_CollectionsToCheck;
      ToolHandle <InDet::InDetV0FinderTool> m_v0FinderTool;
      ToolHandle <Trk::V0Tools> m_V0Tools;
      const HepPDT::ParticleDataTable *m_particleDataTable;

      int           m_masses;                   //!< = 1 if using PDG values, = 2 if user set (1)
      double        m_masspi;                   //!< pion mass (139.57 MeV)
      double        m_massp;                    //!< proton mass (938.272 MeV)
      double        m_masse;                    //!< electron mass (0.510999 MeV)
      double        m_massK0S;                  //!< Kshort mass (497.672 MeV)
      double        m_massLambda;               //!< Lambda mass (1115.68 MeV)

      std::string   m_VxPrimaryCandidateName;   //!< Name of primary vertex container

      std::string                          m_v0ContainerName;
      std::string                          m_ksContainerName;
      std::string                          m_laContainerName;
      std::string                          m_lbContainerName;

  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_dimuTrk_H

