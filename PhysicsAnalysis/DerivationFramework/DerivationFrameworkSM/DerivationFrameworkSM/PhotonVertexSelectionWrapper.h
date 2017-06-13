/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PhotonVertexSelectionWrapper.h
///////////////////////////////////////////////////////////////////
// Author: Matthias Saimpert (matthias.saimpert@cern.ch)
//
 
#ifndef DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
#define DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
 
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace CP
{
  class IPhotonPointingTool;
}
 
namespace DerivationFramework {
 
  class PhotonVertexSelectionWrapper : public AthAlgTool, public IAugmentationTool {
    public:
      PhotonVertexSelectionWrapper(const std::string& t, const std::string& n, const IInterface* p);
 
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
 
    private:
      ToolHandle< CP::IPhotonPointingTool > m_tool;
      std::string m_sgName;
      std::string m_photonContainerName;
      std::string m_vertexContainerName;
  };
}
 
#endif // DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
