// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAINTERFACES_IEGAMMAMVASVC_H
#define EGAMMAINTERFACES_IEGAMMAMVASVC_H

#include "GaudiKernel/IService.h"

// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterFwd.h"


class IegammaMVASvc : virtual public IService {
public:
  DeclareInterfaceID(IegammaMVASvc, 1, 0);

  virtual ~IegammaMVASvc() override {};

  virtual StatusCode execute(xAOD::CaloCluster* cluster,
			     const xAOD::Egamma* eg) const = 0;
  virtual StatusCode execute(xAOD::CaloCluster* cluster,
			     const xAOD::EgammaParameters::EgammaType egType) const = 0;
  virtual StatusCode hltexecute(xAOD::CaloCluster* cluster,
				const std::string& egType) const =0;

};

#endif
