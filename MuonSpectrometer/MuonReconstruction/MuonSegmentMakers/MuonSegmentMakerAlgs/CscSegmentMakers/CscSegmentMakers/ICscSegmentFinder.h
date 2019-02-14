/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ICscSegmentFinder.h

#ifndef ICscSegmentFinder_H
#define ICscSegmentFinder_H

#include "GaudiKernel/IAlgTool.h"
//#include "MuonRecToolInterfaces/IMuonSegmentMaker.h" // added..
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

//IMuonSegmentMaker.h
namespace Muon {
    class MuonSegment;
    class MdtDriftCircleOnTrack;
    class MuonClusterOnTrack;
    class CscClusterOnTrack;
}

namespace Trk {
  class RIO_OnTrack;
  class MeasurementBase;
}

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscSegmentFinder("ICscSegmentFinder", 1, 0);

class ICscSegmentFinder :  virtual public IAlgTool {
  //class ICscSegmentFinder : public Muon::IMuonSegmentMaker {
  
public:  // Static methods

  // Return the interface ID.
  static const InterfaceID& interfaceID() { return IID_ICscSegmentFinder; }
  
public:  // Interface methods

  // for 2d SegmentMaker
  //  virtual MuonSegmentCombinationCollection* find( const std::vector<Muon::CscPrepDataCollection>& pcols) =0;
  //  virtual MuonSegmentCombinationCollection* find( const Muon::CscPrepDataCollection& pcol) =0;
  //  virtual Muon::MuonSegmentCombination* find( const Muon::CscPrepDataCollection& pcol) =0;

  //  using Muon::IMuonSegmentMaker::find;
  virtual std::unique_ptr<MuonSegmentCombinationCollection> find( const std::vector<const Muon::CscPrepDataCollection*>& pcols) const =0;

  // for 4d SegmentMaker
  virtual std::unique_ptr<MuonSegmentCombinationCollection> find( const MuonSegmentCombinationCollection& segcols) const =0;
  /*

  virtual std::vector<const Muon::MuonSegment*>* find( const Trk::TrackRoad& road,
                                                       const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >& mdts,
                                                       const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >&  clusters,
                                                       bool updatePhi=false, double momentum = 1e9 ) =0;
  
  virtual std::vector<const Muon::MuonSegment*>*  find( const Amg::Vector3D&, const Trk::GlobalDirection&,
                                                        const std::vector< const Muon::MdtDriftCircleOnTrack* > &,
                                                        const std::vector< const Muon::MuonClusterOnTrack* > &  ,
                                                        bool updatePhi=false, double momentum = 1e9) =0;
  */


  struct Cluster {
    // local representation of a CSC cluster in the AMDB reference system
    // in this system the x-coordinate corresponds to the phi strips whereas the y coordinate corresponds to the eta strips
    // the z coordinate is along the strip planes
    // the methods locX returns the local strip coordinate for the given hit and the locY returns the position along the strip plane

    Cluster() : cl(NULL), measPhi(0) {}
    Cluster(const Amg::Vector3D& lp, const Muon::CscClusterOnTrack* c, bool measuresPhi ) : lpos(lp),cl(c),measPhi(measuresPhi) {}
    Amg::Vector3D lpos;
    const Muon::CscClusterOnTrack* cl;
    bool measPhi;
    double locY() const { 
      if(measPhi) return lpos.x();
      else        return lpos.y();
    }
    double locX() const  { return lpos.z();}
  };
  
public:
/*   typedef std::vector<Muon::CscClusterOnTrack*> TrkClusters; */
/*   typedef std::vector<Muon::CscClusterOnTrack*> ChamberTrkClusters[4]; */
  typedef std::vector<Cluster> TrkClusters;
  typedef std::vector<Cluster> ChamberTrkClusters[4];

  typedef std::vector<const Trk::RIO_OnTrack*> RioList;
  typedef DataVector<const Trk::MeasurementBase> MbaseList;

  typedef Muon::MuonSegmentCombination::SegmentVec SegmentVec;


  class Segment {
  public:
    double s0;
    double s1;
    double d0;
    double d1;
    double d01;
    double time;
    double dtime;
    double chsq;
    double zshift;
    int nunspoil;
    int nclus;

    TrkClusters clus;
    int outlierid;
    bool operator<(const Segment& rhs) const { return chsq < rhs.chsq; }

    double res[4];
    double dres[4];

    Segment():
      s0(-999.), s1(-999.), d0(-999.), d1(-999.), d01(-999.), time(-9999.), dtime(-9999.),
      chsq(9999.), zshift(0), nunspoil(0), nclus(0), outlierid(-9) 
    {
      for (int i=0; i<4; ++i) {
        res[i] = -9999.;
        dres[i]= -9999.;
      }
      clus.resize(4);
    }
  };

  typedef std::vector<Segment> Segments;

  struct sortByNunspoilAndChsq : public std::binary_function<class Segment, class Segment, bool>
  {
    bool operator()(const Segment& seg1, const Segment& seg2 )
    {
      // sort by numbers of unspoil cluster
      if( seg1.nunspoil > seg2.nunspoil ) return true;
      else if( seg1.nunspoil < seg2.nunspoil ) return false;
      // finally take the one with best chi2
      return seg1.chsq < seg2.chsq;
    }
  };

  
};
#endif
