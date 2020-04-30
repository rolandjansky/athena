/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H
#define MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMomentumEstimator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

namespace Muon {
  class MuonSegment;
}

class MuonSegmentMomentumFromField : public AthAlgTool, virtual public Muon::IMuonSegmentMomentumEstimator
{
 public: 
  /** constructor */
  MuonSegmentMomentumFromField(const std::string&,const std::string&,const IInterface*);
  MuonSegmentMomentumFromField& operator= (const MuonSegmentMomentumFromField& ) = delete;
  
  /** to initiate private members */
  virtual StatusCode initialize(); 

  /** fits a momentum to 2 segments */
  virtual void fitMomentum2Segments( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const;
  virtual void fitMomentum2Segments_old( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const;

  /** fits a momentum to a vector of segments */
  virtual void fitMomentumVectorSegments( const std::vector <const Muon::MuonSegment*>, double & signedMomentum ) const;
 private:
  double fieldIntegralEstimate(const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const;
  double fieldIntegralEstimate_old(const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const;
  
  ToolHandle<Trk::IPropagator>          m_propagator{this, "PropagatorTool", 
                                          "Trk::STEP_Propagator/MuonPropagator"};  
  ToolHandle<Trk::INavigator>           m_navigator {this, "NavigatorTool",
                                          "Trk::Navigator/MuonNavigator"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj"};

  Gaudi::Property<bool> m_doOld {this, "DoOld", false, "Use old fitMomentum2Segments"};
};

#endif // MuonSegmentMomentumFromField_H
