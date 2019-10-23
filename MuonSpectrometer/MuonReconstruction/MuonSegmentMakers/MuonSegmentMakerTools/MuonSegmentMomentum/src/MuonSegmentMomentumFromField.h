/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H
#define MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMomentumEstimator.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"

class RpcIdHelper;
class CscIdHelper;
class TgcIdHelper;
class sTgcIdHelper;

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
  /** to delete private members */
  virtual StatusCode finalize(); 

  /** fits a momentum to 2 segments */
  virtual void fitMomentum2Segments( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const;
  virtual void fitMomentum2Segments_old( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const;

  /** fits a momentum to a vector of segments */
  virtual void fitMomentumVectorSegments( const std::vector <const Muon::MuonSegment*>, double & signedMomentum ) const;
 private:
  double fieldIntegralEstimate(const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const;
  double fieldIntegralEstimate_old(const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const;
  
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc {this, "MagFieldSvc", "AtlasFieldSvc"};
  ToolHandle<Trk::IPropagator>          m_propagator{this, "PropagatorTool", 
                                          "Trk::STEP_Propagator/MuonPropagator"};  
  ToolHandle<Trk::INavigator>           m_navigator {this, "NavigatorTool",
                                          "Trk::Navigator/MuonNavigator"};

  const RpcIdHelper*                  m_rpcid {nullptr};
  const TgcIdHelper*                  m_tgcid {nullptr};
  const CscIdHelper*                  m_cscid {nullptr};
  const sTgcIdHelper*                 m_stgcid {nullptr};
  Gaudi::Property<bool> m_doOld {this, "DoOld", false, "Use old fitMomentum2Segments"};
  Gaudi::Property<bool> m_hasCSC {this, "HasCSC", true, "Is CSC available?"};
  Gaudi::Property<bool> m_hasSTgc {this, "HasSTgc", true, "is sTGC available?"};
};

#endif // MuonSegmentMomentumFromField_H
