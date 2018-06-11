/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ActsAlignment/ShiftCondObj.h"
#include "ActsGeometry/IExCellWriterSvc.h"
#include "ActsGeometry/IExtrapolationTool.h"

#include "GeoModelUtilities/GeoAlignmentStore.h"

#include <string>

namespace Acts {
  class TrackingGeometry;
  class ITrackingGeometrySvc;
  
  //class IMaterialTrackWriterSvc;
  
  template<typename>
  class ExtrapolationCell;

}

class ParticleGun;

class ActsAlignedExtrapAlg  :  public AthAlgorithm {
  
public:
    
  ActsAlignedExtrapAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ActsAlignedExtrapAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  
private:
  
  SG::ReadCondHandleKey<GeoAlignmentStore> m_rch {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};

  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;
  ServiceHandle<Acts::IExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;
  
  std::unique_ptr<ParticleGun> m_particleGun;

  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  ToolHandle<Acts::IExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "Acts__ExtrapolationTool"};

};

