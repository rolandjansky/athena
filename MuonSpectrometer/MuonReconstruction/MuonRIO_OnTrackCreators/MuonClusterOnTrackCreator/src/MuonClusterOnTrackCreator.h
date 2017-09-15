/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  MuonClusterOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for MuonClusterOnTrack production
// (for CSC and RPC technologies)
///////////////////////////////////////////////////////////////////
// Version 1.0 20/07/2004 
///////////////////////////////////////////////////////////////////

#ifndef MuonClusterOnTrackCreator_H
#define MuonClusterOnTrackCreator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkToolInterfaces/IRIO_OnTrackErrorScalingTool.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "TrkParameters/TrackParameters.h"

class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

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
    virtual ~MuonClusterOnTrackCreator ();
    virtual StatusCode initialize();
    virtual StatusCode finalize  ();
 
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

    // /////////////////////////////////////////////////////////////////
    // Private data:
    // /////////////////////////////////////////////////////////////////

    ToolHandle<Trk::IRIO_OnTrackErrorScalingTool>   m_errorScalingTool;
    ToolHandle<Muon::MuonIdHelperTool>   m_idHelper;
    bool                                 m_scaleCscCov;
    bool                                 m_scaleRpcCov;
    bool                                 m_scaleTgcCov;

    bool                                 m_doCsc;
    bool                                 m_doRpc;
    bool                                 m_doTgc;

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
