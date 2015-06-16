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
  virtual ~TrigL2ResidualCalculator(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  StatusCode getResiduals(const TrigInDetTrack*, std::vector<TrigL2HitResidual>&);
  StatusCode getUnbiassedResiduals(const TrigInDetTrack*, std::vector<TrigL2HitResidual>&);
  StatusCode getUnbiassedResiduals(const Trk::Track&, std::vector<TrigL2HitResidual>&);

private:

  Trk::TrkTrackState* m_extrapolate(Trk::TrkTrackState*, 
				    Trk::TrkPlanarSurface*,
				    Trk::TrkPlanarSurface*);
  void m_matrixInversion5x5(double a[5][5]);
  void m_getMagneticField(double[3],double*);

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
