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

  /// Standard execute when you have both cluster and eg object
  virtual StatusCode execute(xAOD::CaloCluster* cluster,
			     const xAOD::Egamma* eg) const = 0;

  /// An execute that just has cluster and egType. A converted photon
  /// is treated like an unconverted photon since no access to vertex.
  virtual StatusCode execute(xAOD::CaloCluster* cluster,
			     xAOD::EgammaParameters::EgammaType egType) const = 0;

  /// The old hlt interface. Consider migrating to the one above
  /// since it's more efficient to use enums instead of strings.
  virtual StatusCode hltexecute(xAOD::CaloCluster* cluster,
				const std::string& egType) const =0;

};

#endif
