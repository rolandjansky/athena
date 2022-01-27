/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentMomentum_MuonSegmentMomentum_H
#define MuonSegmentMomentum_MuonSegmentMomentum_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMomentumEstimator.h"


namespace Muon {
  class MuonSegment;
}


class MuonSegmentMomentum : public AthAlgTool, virtual public Muon::IMuonSegmentMomentumEstimator
{
 public: 
  /** constructor */
  MuonSegmentMomentum(const std::string&,const std::string&,const IInterface*);
  MuonSegmentMomentum& operator= (const MuonSegmentMomentum& ) = delete;

  /** fits a momentum to 2 segments */
  virtual void fitMomentum2Segments( const EventContext&, const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const override;

  /** fits a momentum to a vector of segments */
  virtual void fitMomentumVectorSegments( const EventContext&, const std::vector <const Muon::MuonSegment*>, double & signedMomentum ) const override;

 private:  
  /** flag for use of cosmics, straight line model will be used, no interaction point constraint */
  Gaudi::Property<bool> m_cosmics {this, "DoCosmics", false, "Straight line model will be used, no interaction point constraint"};

};

#endif // MuonSegmentMomentum_H
