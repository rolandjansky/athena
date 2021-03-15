/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhotonsDirectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H
#define DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEgamma/PhotonContainer.h"
//
#include <string>
#include <vector>
namespace DerivationFramework {

class PhotonsDirectionTool
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  PhotonsDirectionTool(const std::string& t,
                       const std::string& n,
                       const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  SG::ReadHandleKey<xAOD::PhotonContainer> m_collName{ this,
                                                       "PhotonContainer",
                                                       "Photons",
                                                       "Input Photons" };

  SG::WriteHandleKey<std::vector<float>> m_sgEta{ this,
                                                  "EtaSGEntry",
                                                  "",
                                                  "output Eta vector" };

  SG::WriteHandleKey<std::vector<float>> m_sgPhi{ this,
                                                  "PhiSGEntry",
                                                  "",
                                                  "output Phi vector" };

  SG::WriteHandleKey<std::vector<float>> m_sgEt{ this,
                                                 "EtSGEntry",
                                                 "",
                                                 "output E vector" };

  SG::WriteHandleKey<std::vector<float>> m_sgE{ this,
                                                "ESGEntry",
                                                "",
                                                "output E vector" };

  bool m_doEta = false;
  bool m_doPhi = false;
  bool m_doEt = false;
  bool m_doE = false;
};
}

#endif // DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H
