/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Interface for MuonClusterOnTrack production
// (for CSC and RPC technologies)
///////////////////////////////////////////////////////////////////

#ifndef MuonClusterOnTrackCreator_H
#define MuonClusterOnTrackCreator_H

#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "TrkParameters/TrackParameters.h"

/** @class MuonClusterOnTrackCreator
    This tool creates MuonClusterOnTrack objects using a given
    global postion prediction.
*/
 
namespace Muon {

  class MuonClusterOnTrackCreator : public AthAlgTool, virtual public IMuonClusterOnTrackCreator
  {
    // /////////////////////////////////////////////////////////////////
    // Public methods:
    // /////////////////////////////////////////////////////////////////

  public:

    MuonClusterOnTrackCreator(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonClusterOnTrackCreator()=default;
    virtual StatusCode initialize();
 
    /** @brief Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and a predicted Trk::TrackParameter. 
        @param RIO Trk::PrepRawData object to be calibrated
        @param GP  Predicted intersect position of the muon with the measurement plane 
        @return a pointer to a new Muon::MuonClusterOnTrack object, zero if calibration failed.
        The ownership of the new Muon::MuonClusterOnTrack is passed to the client calling the tool
    */
    virtual const MuonClusterOnTrack* createRIO_OnTrack(const Trk::PrepRawData& RIO,
                                                        const Amg::Vector3D& GP) const;
 
    /** @brief Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and a prediction of the global position and direction.
	It is only implemented for the CSCs, for RPC and TGC Trk::PrepRawData the result is the same as for the routine without the direction. 
	@param RIO Trk::PrepRawData object to be calibrated
	@param GP  Predicted intersect position of the muon with the measurement plane 
	@param GD  Predicted direction at the intersect position of the muon with the measurement plane 
	@return a pointer to a new Muon::MuonClusterOnTrack object, zero if calibration failed.
	The ownership of the new Muon::MuonClusterOnTrack is passed to the client calling the tool
    */
    virtual const MuonClusterOnTrack* createRIO_OnTrack(const Trk::PrepRawData& RIO,
                                                        const Amg::Vector3D& GP,
                                                        const Amg::Vector3D& GD) const;
 
    /** @brief Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and the predicted Trk::TrackParameter at the measurement surface. 
        @param RIO Trk::PrepRawData object to be calibrated
        @param TP  Predicted Trk::TrackParameter at the measurement surface
        @return a pointer to a new Muon::MuonClusterOnTrack object, zero if calibration failed.
        The ownership of the new Muon::MuonClusterOnTrack is passed to the client calling the tool
    */
    virtual const MuonClusterOnTrack* correct(const Trk::PrepRawData& RIO,const Trk::TrackParameters& TP) const; 
  
  private:

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    bool                                 m_doFixedErrorTgcEta;
    bool                                 m_doFixedErrorRpcEta;
    bool                                 m_doFixedErrorCscEta;
    bool                                 m_doFixedErrorTgcPhi;
    bool                                 m_doFixedErrorRpcPhi;
    bool                                 m_doFixedErrorCscPhi;
    double                               m_fixedErrorTgcEta;
    double                               m_fixedErrorRpcEta;
    double                               m_fixedErrorCscEta;
    double                               m_fixedErrorTgcPhi;
    double                               m_fixedErrorRpcPhi;
    double                               m_fixedErrorCscPhi;
  };
}
#endif // MuonClusterOnTrackCreator_H
