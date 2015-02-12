/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_egammaClusterOverlapMarker_H
#define EGAMMATOOLS_egammaClusterOverlapMarker_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "egammaInterfaces/IegammaClusterOverlapMarker.h"
#include "egammaInterfaces/IegammaShowerShape.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "GaudiKernel/ToolHandle.h" 
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"

class StoreGateSvc;
class IBeamCondSvc;
class IEMExtrapolationTools;
class IegammaTopoClusterMap;
class egammaTopoClusterMap;
class IChronoStatSvc;
class CaloCellContainer;
class CaloCellDetPos;
class CaloClusterCellLinkContainer;

class egammaClusterOverlapMarker : public AthAlgTool,  virtual public IegammaClusterOverlapMarker {

 public:

  //Constructor/destructor.
  egammaClusterOverlapMarker(const std::string& type,
			     const std::string& name,
			     const IInterface* parent);

  egammaClusterOverlapMarker();
  ~egammaClusterOverlapMarker();
  
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();

  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute(const xAOD::CaloCluster*);

 private:

  float m_dEta, m_dPhi;

  std::string m_elecName;
  std::string m_tcName;

};

#endif
