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

#include "TMinuit.h"

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

      /// Struct for passing data to/from TMinuit fit function
      struct MdtSegmentT0FcnData {
        struct HitCoords {
          double z;
          double t;
          double y;
          double w;
          double r;
          const MuonCalib::IRtRelation *rt;
        };

        bool use_hardcoded;
        bool use_shift_constraint;
        double constrainT0Error;  
        std::vector<HitCoords> data;
        double t_lo, t_hi;
        int used;
        int t0Error;
      };

    private:
      ToolHandle<MdtCalibrationDbTool> m_calibrationDbTool{this,"CalibrationDbTool","MdtCalibrationDbTool"};

      Gaudi::Property<bool> m_trace{this,"TraceOperation",false,"debug - traces operation"};
      Gaudi::Property<bool> m_dumpToFile{this,"DumpToFile",false,"debug - dumps some performance info"};
      Gaudi::Property<bool> m_dumpNoFit{this,"DumpNoFit",false,"debug - print hit info where fit does not run"};
      Gaudi::Property<bool> m_useInternalRT{this,"UseInternalRT",false,"whether to use an internal RT function or the one from Calibration Service"};
      Gaudi::Property<bool> m_constrainShifts{this,"ConstrainShifts",false,"whether to constrain t0 shifts to a 50 ns window"};
      Gaudi::Property<bool> m_rejectWeakTopologies{this,"RejectWeakTopologies",true,"reject topolgies that do not have at least one +- combination in one multilayer"};
      Gaudi::Property<bool> m_scaleErrors{this,"RescaleErrors",true,"rescale errors in fit"};
      Gaudi::Property<bool> m_propagateErrors{this,"PropagateErrors",true,"propagate errors"};
      Gaudi::Property<int> m_minHits{this,"MinimumHits",4,"minimum number of selected hits for t0 fit. Otherwise use default"};
      Gaudi::Property<double> m_constrainT0Error{this,"ConstrainT0Error",10,"t0 error that is used in the constraint"};
      Gaudi::Property<float> m_dRTol{this,"dRTolerance",0.1};

      std::unique_ptr<TMinuit> m_minuit;

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
