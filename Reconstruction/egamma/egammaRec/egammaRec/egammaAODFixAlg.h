// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef EGAMMAREC_EGAMMAAODFIXALG_H
#define EGAMMAREC_EGAMMAAODFIXALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// xAOD include
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODEgamma/PhotonContainerFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "StoreGate/ReadHandleKey.h"

/**
 *  @short Algorithm to fix AOD electrons and photons,
 *   based on mounAODFixAlg, by Edward Moyse
 *
 * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
 *
 */

class CaloCellContainer;
class IegammaStripsShape;


class egammaAODFixAlg : public AthAlgorithm {
  
public:
  /// Regular algorithm constructor
  egammaAODFixAlg( const std::string& name, ISvcLocator* svcLoc );
  
  /// Function initialising the algorithm
  virtual StatusCode initialize() override;
  /// Function executing the algorithm
  virtual StatusCode execute() override;
  
private:

  StatusCode fixEgamma(xAOD::Egamma* egamma,
		       const CaloCellContainer *cell_container);

  // not really sure how to do this part with handles
  std::string m_electronContainerName;
  std::string m_photonContainerName;

  SG::ReadHandleKey<CaloCellContainer> m_cellsKey; 
  ToolHandle<IegammaStripsShape> m_stripsShapeTool;

}; // class egammaAODFixAlg


#endif 
