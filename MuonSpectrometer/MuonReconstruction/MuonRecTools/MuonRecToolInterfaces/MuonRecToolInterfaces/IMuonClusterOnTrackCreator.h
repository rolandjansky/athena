/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCLUSTERONTRACKCREATOR_H
#define MUON_IMUONCLUSTERONTRACKCREATOR_H

#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "GeoPrimitives/GeoPrimitives.h"

static const InterfaceID IID_IMuonClusterOnTrackCreator
    ("Muon::IMuonClusterOnTrackCreator",1,0);

namespace Trk {
    class PrepRawData;
}

namespace Muon {


  /** @brief Interface for tools calibrating MuonCluster, turning them into Muon::MuonClusterOnTrack object. 
      The interface inherits from Trk::IRIO_OnTrackCreator. 
  */
  class IMuonClusterOnTrackCreator : virtual public Trk::IRIO_OnTrackCreator 
  {      
  public:
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////
    
    static const InterfaceID& interfaceID();
    
    /** @brief Calibrate a RPC, TGC or CSC PrepRawData object. The result is stored in a new Muon::MuonClusterOnTrack object
        @param DC Trk::PrepRawData object
        @param GP Predicted global position (including second coordinate along the strip).
        @return Fully calibrated Muon::MuonClusterOnTrack.
	        The memory management of the new Muon::MuonClusterOnTrack is passed to the person calling the function.
    */
    virtual const MuonClusterOnTrack* createRIO_OnTrack(const Trk::PrepRawData& DC,
                                                        const Amg::Vector3D& GP) const = 0;

    /** @brief Calibrate a RPC, TGC or CSC PrepRawData object. The result is stored in a new Muon::MuonClusterOnTrack object
        @param DC Trk::PrepRawData object
        @param GP Predicted global position (including second coordinate along the strip).
        @param GD Predicted global direction.
        @return Fully calibrated Muon::MuonClusterOnTrack.
	        The memory management of the new Muon::MuonClusterOnTrack is passed to the person calling the function.
    */
    virtual const MuonClusterOnTrack* createRIO_OnTrack(const Trk::PrepRawData& DC,
                                                        const Amg::Vector3D& GP,
                                                        const Amg::Vector3D& GD) const = 0;

    /** @brief Calibrate a NSW PrepRawData object. The result is stored in a new Muon::MuonClusterOnTrack object
        @param DC Trk::PrepRawData object
        @param GP Predicted global position (including second coordinate along the strip).
        @return Fully calibrated Muon::MuonClusterOnTrack.
	        The memory management of the new Muon::MuonClusterOnTrack is passed to the person calling the function.
    */
    virtual const MuonClusterOnTrack* calibratedCluster(const Trk::PrepRawData& DC,
                                                        const Amg::Vector3D& GP) const;

    
  };
  
  inline const InterfaceID& IMuonClusterOnTrackCreator::interfaceID()
  {
    return IID_IMuonClusterOnTrackCreator;
  }

  inline const MuonClusterOnTrack* IMuonClusterOnTrackCreator::calibratedCluster(const Trk::PrepRawData& DC,
										 const Amg::Vector3D& GP) const
  {
    return createRIO_OnTrack(DC,GP);
  }


} // end of name space

#endif // MUON_IMUONCLUSTERONTRACKCREATOR_H

