/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGL2RESIDUALCALCULATOR_H__
#define __TRIGL2RESIDUALCALCULATOR_H__ 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TrigL2HitResidual;

namespace Trk {	
  class TrkBaseNode;             
  class TrkTrackState;      
  class TrkPlanarSurface;
  class Track;
}

namespace MagField {	
	class IMagFieldSvc;
}

class TrigL2ResidualCalculator: public AthAlgTool, virtual public ITrigL2ResidualCalculator
{
 public:

  TrigL2ResidualCalculator( const std::string&, const std::string&, const IInterface* );
  virtual StatusCode initialize();

  StatusCode getUnbiasedResiduals(const Trk::Track&, std::vector<TrigL2HitResidual>&) const;

private:

  Trk::TrkTrackState* extrapolate(Trk::TrkTrackState*, 
                                  Trk::TrkPlanarSurface*,
                                  Trk::TrkPlanarSurface*) const;
  void getMagneticField(double[3],double*) const;

  double m_DChi2;
  bool m_doMultScatt;
  bool m_doBremm;
  bool m_offlineClusters;
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;
  ToolHandle<ITrigDkfTrackMakerTool> m_trackMaker;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;
};

#endif
