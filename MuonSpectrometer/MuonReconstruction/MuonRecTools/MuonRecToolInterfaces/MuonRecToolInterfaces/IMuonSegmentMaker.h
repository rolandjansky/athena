/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTMAKER_H
#define MUON_IMUONSEGMENTMAKER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkSegment/SegmentCollection.h"

static const InterfaceID IID_IMuonSegmentMaker
    ("Muon::IMuonSegmentMaker",1,0);

namespace Trk {
  class PrepRawData;
  class RIO_OnTrack;
  class TrackRoad;
}

namespace Muon {

  class MuonSegment;
  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;

  /** @brief The IMuonSegmentFinder is a pure virtual interface for tools to find segments in the muon spectrometer.
      It provides several routines for the segment search. These routines have in common that they 
      take a set of RIO_OnTrack objects as input and return a pointer to a vector of MuonSegment objects.
      The client calling the IMuonSegmentFinder is responsible for the memory management of the segments and 
      should ensure that they get deleted before the end of the event.

      Several interfaces are available, they can be divided into two categories:
      - unseeded search
      @code
       virtual std::vector<const MuonSegment*>* find( const std::vector<const MdtDriftCircleOnTrack*>& mdts,
                                                      const std::vector<const MuonClusterOnTrack*>&  clusters) =0;
       virtual std::vector<const MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios ) =0;
 
       virtual std::vector<const MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
                                                      const std::vector<const Trk::RIO_OnTrack*>& rios2 ) =0;
      @endcode					      
      - seeded search
      @code
       virtual std::vector<const MuonSegment*>* find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
                                                      const std::vector<const MdtDriftCircleOnTrack*>& mdts,
						      const std::vector<const MuonClusterOnTrack*>&  clusters, 
						      bool updatePhi=false, double momentum ) =0;

       virtual std::vector<const MuonSegment*>* find( const Trk::TrackRoad& road,
                                                      const std::vector< std::vector< const MdtDriftCircleOnTrack* > >& mdts,
						      const std::vector< std::vector< const MuonClusterOnTrack* > >&  clusters, 
						      bool updatePhi=false, double momentum ) =0;      
      @endcode
      The boolean updatePhi is used to determine whether the predicted phi measurement should be used to update the 
      @f$ \phi @f$ coordinate of the segment or whether the center of the tube should be used.

      It is recommended but not required that users implement all interfaces.
      Currently the following implementations of the interface exist:
      - Muon::DCMathSegmentMaker, tool to find segments using in a station layer using both MDT hits and trigger hits.
      - Muon::MooMdtSegmentMaker, tool to find segments in a MDT chamber. 
      - Muon::MDT_DHoughSegmentMakerTool, tool to find segments in a station layer using MDT hits.
      - Muon::MooCscSegmentMaker, tool to find segments in a CSC chamber.
      - Muon::CSC_DHoughSegmentMakerTool, tool to find segments in a CSC chamber.
      - Muon::RPC_LinearSegmentMakerTool, tool to find segments in a RPC station layer.
      - Muon::TGC_LinearSegmentMakerTool, tool to find segments in a TGC station layer.
  */  
  class IMuonSegmentMaker : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

     /** @brief find segments starting from a list of Trk::RIO_OnTrack objects
       @param rios a vector of Trk::RIO_OnTrack objects
       @param segColl a Trk::SegmentCollection to store the segments
       The ownership of the segments is passed to the SegmentCollection
     */
    virtual void find( const std::vector<const Trk::RIO_OnTrack*>& rios, Trk::SegmentCollection* segColl=nullptr ) const =0;
 
    /** @brief find segments starting from two lists of Trk::RIO_OnTrack objects in two neighbouring chambers 
      @param rios1 a vector of Trk::RIO_OnTrack objects in the first chamber
      @param rios2 a vector of Trk::RIO_OnTrack objects in the second chamber
      Note: this interface is deprecated!
    */
    virtual void find( const std::vector<const Trk::RIO_OnTrack*>& rios1,
		       const std::vector<const Trk::RIO_OnTrack*>& rios2 ) const =0;

    /** @brief find segments starting from a list of MdtDriftCircleOnTrack objects and a list of MuonClusterOnTrack objects 
	@param mdts a vector of Muon::MdtDriftCircleOnTrack obejcts
	@param clusters a vector of Muon::MuonClusterOnTrack obejcts
	@param segColl a Trk::SegmentCollection to store the segments
       The ownership of the segments is passed to the SegmentCollection  
    */
    virtual void find( const std::vector<const MdtDriftCircleOnTrack*>& mdts,
		       const std::vector<const MuonClusterOnTrack*>&  clusters,
		       Trk::SegmentCollection* segColl=nullptr) const =0;


    /** @brief seeded segment search starting from a list of MdtDriftCircleOnTrack objects and a list of MuonClusterOnTrack objects 
	@param gpos an estimate of the global position of the muon in the chamber
	@param gdir an estimate of the global direction of the muon in the chamber
	@param mdts a vector of Muon::MdtDriftCircleOnTrack obejcts
	@param clusters a vector of Muon::MuonClusterOnTrack obejcts
	@param updatePhi a boolean to indicate whether the external prediction should be used to set the 
                                 @f$ \phi @f$-direction of the segment
        @param momentum for low momentum tracks the segment finders may increase the errors on the measurements
                        to allow for multiple scattering			       
        @param segColl a Trk::SegmentCollection to store the segments
        The ownership of the segments is passed to the SegmentCollection  
	
    */
    virtual void find( const Amg::Vector3D& gpos, const Amg::Vector3D& gdir,
		       const std::vector<const MdtDriftCircleOnTrack*>& mdts,
		       const std::vector<const MuonClusterOnTrack*>&  clusters, 
		       bool updatePhi=false, Trk::SegmentCollection* segColl=nullptr, double momentum = 1e9, double sinAngleCut=0. ) const =0;

    /** @brief seeded segment search starting from a list of MdtDriftCircleOnTrack objects and a list of MuonClusterOnTrack objects
	@param road an estimate of the position and direction of the muon in the chamber
	@param mdts a vector of Muon::MdtDriftCircleOnTrack obejcts
	@param clusters a vector of Muon::MuonClusterOnTrack obejcts
	@param updatePhi a boolean to indicate whether the external prediction should be used to set the 
                                 @f$ \phi @f$-direction of the segment
	@param segColl: if a Trk::SegmentCollection is passed (should be the case if segments will go into SG), store the segments in here directly
        @param momentum for low momentum tracks the segment finders may increase the errors on the measurements
                        to allow for multiple scattering

        @param segColl a Trk::SegmentCollection to store the segments
        The ownership of the segments is passed to the SegmentCollection  
    */
    virtual void find( const Trk::TrackRoad& road,
		       const std::vector< std::vector< const MdtDriftCircleOnTrack* > >& mdts,
		       const std::vector< std::vector< const MuonClusterOnTrack* > >&  clusters, 
		       Trk::SegmentCollection* segColl,
		       bool updatePhi=false, double momentum = 1e9 ) const =0;
  };
  
  inline const InterfaceID& IMuonSegmentMaker::interfaceID()
  {
    return IID_IMuonSegmentMaker;
  }
} // end of name space

#endif // IMuonSegmentMaker_H

