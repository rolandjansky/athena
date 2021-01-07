/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Fitting for in situ calibration of segments
#ifndef MUON_MDTSEGMENTT0FITTER_H
#define MUON_MDTSEGMENTT0FITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrkDriftCircleMath/DCSLFitter.h"
#include "MuonSegmentMakerInterfaces/IDCSLFitProvider.h"

#include "MdtCalibSvc/MdtCalibrationDbTool.h"

#include <atomic>
#include <vector>
#include <memory>

namespace TrkDriftCircleMath {

  class MdtSegmentT0Fitter : public AthAlgTool, public DCSLFitter, virtual public Muon::IDCSLFitProvider {
    public:

      MdtSegmentT0Fitter(const std::string&,const std::string&,const IInterface*);
      virtual ~MdtSegmentT0Fitter()=default;
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      
      virtual bool fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, double t0Seed ) const override;
      virtual bool fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, const HitSelection& selection, double t0Seed ) const override;
 
      virtual const DCSLFitter* getFitter() const override { return this; }

    private:
      ToolHandle<MdtCalibrationDbTool> m_calibrationDbTool{this,"CalibrationDbTool","MdtCalibrationDbTool"};

      Gaudi::Property<bool> m_trace{this,"TraceOperation",false,"debug - traces operation"};
      Gaudi::Property<bool> m_rejectWeakTopologies{this,"RejectWeakTopologies",true,"reject topolgies that do not have at least one +- combination in one multilayer"};
      Gaudi::Property<bool> m_scaleErrors{this,"RescaleErrors",true,"rescale errors in fit"};
      Gaudi::Property<bool> m_propagateErrors{this,"PropagateErrors",true,"propagate errors"};
      Gaudi::Property<int> m_minHits{this,"MinimumHits",4,"minimum number of selected hits for t0 fit. Otherwise use default"};
      Gaudi::Property<float> m_dRTol{this,"dRTolerance",0.1};

      // counters
      mutable std::atomic_uint m_ntotalCalls;
      mutable std::atomic_uint m_npassedNHits;
      mutable std::atomic_uint m_npassedSelectionConsistency;
      mutable std::atomic_uint m_npassedNSelectedHits;
      mutable std::atomic_uint m_npassedMinHits;
      mutable std::atomic_uint m_npassedMinuitFit;

    };
    
  inline bool MdtSegmentT0Fitter::fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, double t0Seed ) const { 
    HitSelection selection(dcs.size(),0);
    return fit( result, line, dcs, selection, t0Seed ); 
  }
}

#endif
