/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 helper class to assign correct cluster properties, hopefully
 this is a temporary requirement (only needed during the EDM upheavals)
 ----------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKFOLLOWER_SICLUSTERPROPERTIES_H
# define IPATTRACKFOLLOWER_SICLUSTERPROPERTIES_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ISiClusterProperties.h"
#include "iPatTrack/HitStatus.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PixelID;
class SCT_ID;
namespace InDet
{
    class SiClusterOnTrack;
}
namespace InDetDD
{
    class SiDetectorElement;
}
namespace Trk
{
    class IRIO_OnTrackCreator;
}

class SiClusterProperties: public AthAlgTool,
			   virtual public ISiClusterProperties
{
    
public:
    SiClusterProperties 	(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);
    virtual ~SiClusterProperties	(void);
 
    virtual StatusCode			initialize() override;
    virtual StatusCode			finalize() override;


    // conservative cluster errors for use during pattern recognition
    std::pair<double,double>            broadErrors (const InDet::SiCluster*	cluster,
						     const SiliconDetector*	detector,
						     const Amg::Vector3D&	trackIntersect) const override;

    // precise cluster position and errors including track-dependent corrections
    HitOnTrack*				hitOnTrack (const InDet::SiCluster*	cluster,
						    const SiliconDetector*	detector,
						    const Trk::TrackParameters*	parameters) const override;
private:
    struct State;

    void				barrelSCT_Errors (State& state) const;
    void				barrelSCT_Properties (State& state) const;
    void				barrelPixelErrors (State& state) const;
    void				barrelPixelProperties (State& state) const;
    void				endcapSCT_Errors (State& state) const;
    void				endcapSCT_Properties (State& state) const;
    void				endcapPixelErrors (State& state) const;
    void				endcapPixelProperties (State& state) const;
    void				setProperties (State& state,
                                                       const SiliconDetector*	detector,
						       const Amg::Vector3D&	trackIntersect) const;
    void				setProperties (State& state,
                                                       const SiliconDetector*	detector,
						       const Amg::Vector3D&	trackIntersect,
						       const Amg::Vector3D&	trackDirection) const;
    
    // helpers, managers, tools
    const PixelID*				m_pixelIdHelper;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_pixelBroadROT_Maker;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_pixelPreciseROT_Maker;
    const SCT_ID*				m_sctIdHelper;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_sctBroadROT_Maker;
    ToolHandle<Trk::IRIO_OnTrackCreator>	m_sctPreciseROT_Maker;

    // state
    double					m_sqrt12;

    struct State
    {
      const InDet::SiCluster*			m_cluster;
      double					m_broadEtaError;
      double					m_broadPhiError;
      Amg::Vector3D	       			m_globalPosition;
      const Trk::TrackParameters*		m_parameters;
      const InDet::SiClusterOnTrack*		m_rotBroad;
      const InDet::SiClusterOnTrack*		m_rotPrecise;
      HitStatus					m_status;
      double					m_trackCotTheta;
      double					m_trackRadius;
    };

};

#endif // IPATTRACKFOLLOWER_SICLUSTERPROPERTIES_H
