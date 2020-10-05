/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigDkfTrackMakerTool tool
// -------------------------------
// ATLAS Collaboration
//
// 17.03.2009 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include "TrigDkfTrackMakerTool.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "StoreGate/ReadCondHandle.h"

#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"

#include <cmath>
#include <iostream>

TrigDkfTrackMakerTool::TrigDkfTrackMakerTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigDkfTrackMakerTool >( this );
  m_idHelper=NULL;
}

StatusCode TrigDkfTrackMakerTool::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
  return StatusCode::SUCCESS;
}

bool TrigDkfTrackMakerTool::createDkfTrack(const Trk::Track& track, 
		std::vector<Trk::TrkBaseNode*>& vpTrkNodes,
		double DChi2) const {

	if(track.measurementsOnTrack()->size()==0) 
	{
		ATH_MSG_ERROR("Cannot create a DKF track -- Trk::Track has no hits");
		return false;
	}
	vpTrkNodes.clear();
  vpTrkNodes.reserve(track.measurementsOnTrack()->size());
	for (auto tMOT = track.measurementsOnTrack()->begin(); tMOT != track.measurementsOnTrack()->end(); ++tMOT) {
		const Trk::Surface& rSurf=(*tMOT)->associatedSurface();
		constexpr double radLength=0.022;
		double C[3],N[3],M[3][3];
		Amg::Vector3D mx,my,mz;
		N[0]=rSurf.normal().x();
		N[1]=rSurf.normal().y();
		N[2]=rSurf.normal().z();
		C[0]=rSurf.center().x();
		C[1]=rSurf.center().y();
		C[2]=rSurf.center().z();
		mx=rSurf.transform().rotation().block(0,0,3,1);
		my=rSurf.transform().rotation().block(0,1,3,1);
		mz=rSurf.transform().rotation().block(0,2,3,1);
		for(int i=0;i<3;i++) {
		  M[i][0]=mx[i];M[i][1]=my[i];M[i][2]=mz[i];
		}
		Trk::TrkPlanarSurface* pS = new Trk::TrkPlanarSurface(C,N,M,radLength, &(rSurf));



		const InDet::SiClusterOnTrack* siCLOT = dynamic_cast<const InDet::SiClusterOnTrack*>(*tMOT);
		if (siCLOT==nullptr) {
			ATH_MSG_VERBOSE("siCLOT is null");
			continue;
		}
		const InDet::SiCluster* siCL = dynamic_cast<const InDet::SiCluster*>(siCLOT->prepRawData());
		if (siCL==nullptr) {
			ATH_MSG_VERBOSE("siCL is null");
			continue;
		}
		Identifier id = (*tMOT)->associatedSurface().associatedDetectorElement()->identify();
		ATH_MSG_DEBUG("Identifier: " << m_idHelper->print_to_string(id));
		if(m_idHelper->is_sct(id)) {
			const InDetDD::SiDetectorElement* sctElement = dynamic_cast<const InDetDD::SiDetectorElement*> 
				((*tMOT)->associatedSurface().associatedDetectorElement());
      if (sctElement) {
        if(sctElement->design().shape()==InDetDD::Trapezoid) //SCT Endcap
        {
          ATH_MSG_DEBUG("SCT endcap node");
          const Trk::SurfaceBounds& rBounds=rSurf.bounds();
          const Trk::TrapezoidBounds& ecBounds=	dynamic_cast<const Trk::TrapezoidBounds&>(rBounds);
          double R=(ecBounds.maxHalflengthX()+ecBounds.minHalflengthX())*
            ecBounds.halflengthY()/(ecBounds.maxHalflengthX()-ecBounds.minHalflengthX());
          vpTrkNodes.push_back(new Trk::TrkEndCapClusterNode(pS,DChi2,siCL,R));
        }
        else {//SCT Barrel 
          ATH_MSG_DEBUG("SCT barrel node");
          vpTrkNodes.push_back(new Trk::TrkClusterNode(pS,DChi2,siCL));
        }
      }
      else {
        ATH_MSG_WARNING("Identifier is SCT but does not match endcap or barrel shape");
      }
		}
		else if (m_idHelper->is_pixel(id)) {//Pixel 
			ATH_MSG_DEBUG("Pixel node");
			vpTrkNodes.push_back(new Trk::TrkPixelNode(pS,DChi2,siCL));
		}
    else {
      ATH_MSG_WARNING("Identifier is neither SCT nor pixel");
    }
	}
	ATH_MSG_DEBUG(vpTrkNodes.size()<<" filtering nodes created");

	return true;
}

