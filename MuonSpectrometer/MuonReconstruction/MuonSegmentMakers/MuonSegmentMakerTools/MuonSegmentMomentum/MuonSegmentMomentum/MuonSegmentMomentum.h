/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  /** destructor */
  virtual ~MuonSegmentMomentum();

  /** to initiate private members */
  virtual StatusCode initialize(); 
  /** to delete private members */
  virtual StatusCode finalize(); 

  /** fits a momentum to 2 segments */
  virtual void fitMomentum2Segments( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const;

  /** fits a momentum to a vector of segments */
  virtual void fitMomentumVectorSegments( const std::vector <const Muon::MuonSegment*>, double & signedMomentum ) const;
 private:

  /** flag to print out debugging information */
  bool m_debug;
  /** flag to print out a summary of what comes in and what comes out */
  bool m_summary; 
  /** flag for use of cosmics, straight line model will be used, no interaction point constraint */
  bool m_cosmics; 

};

#endif // MuonSegmentMomentum_H
