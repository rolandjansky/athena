/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_FAST_EXTRAPOLATION_TOOL_H__
#define __TRIG_L2_FAST_EXTRAPOLATION_TOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetTrackFitter/ITrigL2FastExtrapolationTool.h"

namespace Trk {	
  class TrkTrackState;      
  class TrkPlanarSurface;
}
namespace MagField {	
	class IMagFieldSvc;
}

class TrigL2FastExtrapolationTool: public AthAlgTool, virtual public ITrigL2FastExtrapolationTool
{
 public:
  TrigL2FastExtrapolationTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigL2FastExtrapolationTool(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  Trk::TrkTrackState* extrapolate(Trk::TrkTrackState*, 
				  Trk::TrkPlanarSurface*,
				  Trk::TrkPlanarSurface*, bool smooth=false);
private:
  void m_matrixInversion5x5(double a[5][5]);
  void m_getMagneticField(double[3],double*);
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;
};

#endif
