/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H
#define MuonSegmentMomentumFromField_MuonSegmentMomentumFromField_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMomentumEstimator.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

class RpcIdHelper;
class CscIdHelper;
class TgcIdHelper;

namespace Muon {
  class MuonSegment;
}

namespace Trk {
  class IPropagator;
  class INavigator;  
}

class MuonSegmentMomentumFromField : public AthAlgTool, virtual public Muon::IMuonSegmentMomentumEstimator
{
 public: 
  /** constructor */
  MuonSegmentMomentumFromField(const std::string&,const std::string&,const IInterface*);
  /** destructor */
  virtual ~MuonSegmentMomentumFromField();

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

  /** flag to print out debugging information */
  bool m_debug;
  /** flag to print out a summary of what comes in and what comes out */
  bool m_summary; 
  /** flag for use of cosmics, straight line model will be used, no interaction point constraint */
  bool m_cosmics; 
  ServiceHandle<MagField::IMagFieldSvc>            m_magFieldSvc; 
  ToolHandle<Trk::IPropagator> m_propagator;
  ToolHandle<Trk::INavigator> m_navigator;

  const RpcIdHelper*                  m_rpcid;
  const TgcIdHelper*                  m_tgcid;
  const CscIdHelper*                  m_cscid;
  bool m_doOld;
};

#endif // MuonSegmentMomentumFromField_H
