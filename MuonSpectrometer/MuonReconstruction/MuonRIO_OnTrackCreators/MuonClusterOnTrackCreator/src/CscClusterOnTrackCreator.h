/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  CscClusterOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for CscClusterOnTrack production
///////////////////////////////////////////////////////////////////
// Version 1.0 20/07/2004 
///////////////////////////////////////////////////////////////////

#ifndef CscClusterOnTrackCreator_H
#define CscClusterOnTrackCreator_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/ICscClusterOnTrackCreator.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonRIO_OnTrack/MuonEtaPhiRIO_OnTrackErrorScaling.h"

#include "MuonPrepRawData/CscStripPrepDataContainer.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"


namespace Muon {


  /** @class CscClusterOnTrackCreator
      @brief Interface for the reconstruction to calibration and alignment corrections. It should be used by 
             reconstruction and pattern recognition to create Muon::MuonClusterOnTrack objects (s).

       It offers several interfaces:
       - Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and a predicted Trk::TrackParameter.       
         @code const MuonClusterOnTrack* correct ( const Trk::PrepRawData& RIO, const Trk::TrackParameters& tp) const @endcode
       - Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and a prediction of the global position and direction.
         It is only implemented for the CSCs, for RPC and TGC PrepRawData the result is the same as for the routine without the direction.
         @code createRIO_OnTrack(const Trk::PrepRawData& ROP, const Trk::GlobalPosition& GP, const Trk::GlobalDirection GD) const @endcode
       - Create new Muon::MuonClusterOnTrack from a Trk::PrepRawData and a prediction intersect position of the muon with the 
         measurement surface.
         @code createRIO_OnTrack(const Trk::PrepRawData& RIO, const Trk::GlobalPosition& GP) const @endcode
       
       JobOptions Flags:
       - doCSC: switch on/off CSC ROT creation (default = true)
       - doRPC: switch on/off RPC ROT creation (default = true)
       - doTGC: switch on/off TGC ROT creation (default = true)
       - [CSC,TGC,RPC]ErrorScalingKey: if not empty use error scaling conditions data to scale the corresponding muon covariances
       - CscClusterFitter: Tool to fit charge and width of CSC clusters
       - CscStripFitter: Tool to fit charge + time of a CSC strip
       - CscStripPrepDataLocation: Storegate key of the CscStripPrepData collection
   */
 class CscClusterOnTrackCreator : public AthAlgTool, virtual public ICscClusterOnTrackCreator {
  public:
    
    CscClusterOnTrackCreator(const std::string&,const std::string&,const IInterface*);
    virtual ~CscClusterOnTrackCreator ();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
 
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
  
    virtual const ToolHandle<ICscStripFitter>& GetICscStripFitter() const;
    virtual const ToolHandle<ICscClusterFitter>& GetICscClusterFitter() const;
    virtual const ToolHandle<ICscClusterUtilTool>& GetICscClusterUtilTool() const;


  private:
    const MuonGM::MuonDetectorManager*   m_muonMgr;          // Muon GeoModel
    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
      "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
    ToolHandle<ICscStripFitter>          m_stripFitter;
    ToolHandle<ICscClusterFitter>        m_clusterFitter;
    ToolHandle<ICscClusterUtilTool>      m_clusterUtilTool;
    bool m_have_csc_tools;

  SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_cscErrorScalingKey
   {this,"CSCErrorScalingKey", "" /*"/MUON/TrkErrorScalingCSC"*/, "Key for CSC error scaling conditions data. No error scaling if empty."};

  SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_tgcErrorScalingKey
   {this,"TGCErrorScalingKey", "" /*"/MUON/TrkErrorScalingTGC"*/, "Key for TGC error scaling conditions data. No error scaling if empty."};

  SG::ReadCondHandleKey<RIO_OnTrackErrorScaling> m_rpcErrorScalingKey
   {this,"RPCErrorScalingKey", "" /*"/MUON/TrkErrorScalingRPC"*/, "Key for RPC error scaling conditions data. No error scaling if empty."};

    bool                                 m_doCsc;
    bool                                 m_doRpc;
    bool                                 m_doTgc;

    double                               m_fixedError;
    double                               m_errorScaler;
    double                               m_errorScalerBeta;
    double                               m_minimumError;

    std::string m_cscStripLocation;
  };
}
#endif // CscClusterOnTrackCreator_H
