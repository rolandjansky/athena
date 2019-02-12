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

#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigSiSpacePoint.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include "TrigInDetTrackFitter/TrigDkfTrackMakerTool.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/ReadCondHandle.h"

#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"


TrigDkfTrackMakerTool::TrigDkfTrackMakerTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigDkfTrackMakerTool >( this );
  m_idHelper=NULL;
}

StatusCode TrigDkfTrackMakerTool::initialize()
{
  ATH_MSG_INFO("In initialize..."); 


 if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_FATAL("Could not get AtlasDetectorID helper AtlasID");
    return StatusCode::FAILURE;
  }  

 // Get SCT & pixel Identifier helpers

  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) { 
     ATH_MSG_FATAL("Could not get Pixel ID helper");
     return StatusCode::FAILURE;  
  }
  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {  
     ATH_MSG_FATAL("Could not get SCT ID helper");
     return StatusCode::FAILURE;
  }
  StatusCode sc = detStore()->retrieve(m_pixelManager);  
  if( sc.isFailure() ) 
   {
      ATH_MSG_ERROR("Could not retrieve Pixel DetectorManager from detStore."); 
      return sc;
   } 

  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  ATH_MSG_INFO("TrigDkfTrackMakerTool constructed ");
  return sc;
}

StatusCode TrigDkfTrackMakerTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigDkfTrackMakerTool::~TrigDkfTrackMakerTool()
{

}

bool TrigDkfTrackMakerTool::createDkfTrack(std::vector<const TrigSiSpacePoint*>& siSpacePoints, 
					   std::vector<Trk::TrkBaseNode*>& vpTrkNodes,
					   double DChi2)
{
  const double radLength=0.022;

  std::vector<const TrigSiSpacePoint*>::iterator pSPIt,lastSPIt;

  double C[3],N[3],M[3][3];int i;
  Amg::Vector3D mx,my,mz;

  vpTrkNodes.clear();

  if(siSpacePoints.size()==0) 
  {
    ATH_MSG_WARNING("Cannot create a DKF track -- TrigInDetTrack has no hits");
    return false;
  }
  pSPIt=siSpacePoints.begin();lastSPIt=siSpacePoints.end();
  for(; pSPIt != lastSPIt; pSPIt++) 
    {
      const TrigSiSpacePoint* pSP=(*pSPIt);

      // std::cout<<"SP layer="<<pSP->layer()<<" r="<<pSP->r()<<" phi="<<pSP->phi()<<" z="<<pSP->z()<<std::endl;

      Identifier ID=pSP->identify();
      if(m_idHelper->is_sct(ID))
	{
	  const InDet::SiCluster *pCL[2];
	  pCL[0] = pSP->clusters().first;
	  pCL[1] = pSP->clusters().second;
	  if((pCL[0]==NULL)||(pCL[1]==NULL)) continue;

	  IdentifierHash idHash[2];
	  const InDetDD::SiDetectorElement* pEL[2];
	  double RadVec[2];
	  int index[2];
	  for(i=0;i<2;i++)
	    {
	      idHash[i]=m_sctId->wafer_hash(m_sctId->wafer_id(pCL[i]->identify()));
	      pEL[i]=getSCTDetectorElement(idHash[i]);
	      const Trk::Surface& rSurf=pEL[i]->surface();

	      // const Trk::Surface& rSurf=pCL[i]->detectorElement()->surface();
	      RadVec[i]=rSurf.center().mag();
	      index[i]=i;
	    }
	  if(RadVec[0]>RadVec[1])
	    {
	      index[0]=1;index[1]=0;
	    }
	  for (int iClusInSP=0; iClusInSP<2; iClusInSP++)
	    {
	      const Trk::Surface& rSurf=pEL[index[iClusInSP]]->surface();
	      //const Trk::Surface& rSurf=pCL[i]->detectorElement()->surface();
	      N[0]=rSurf.normal().x();
	      N[1]=rSurf.normal().y();
	      N[2]=rSurf.normal().z();
	      C[0]=rSurf.center().x();
	      C[1]=rSurf.center().y();
	      C[2]=rSurf.center().z();

	      mx=rSurf.transform().rotation().block(0,0,3,1);
	      my=rSurf.transform().rotation().block(0,1,3,1);
	      mz=rSurf.transform().rotation().block(0,2,3,1);
	      for(i=0;i<3;i++) 
		{
		  M[i][0]=mx[i];M[i][1]=my[i];M[i][2]=mz[i];
		}
	      Trk::TrkPlanarSurface* pS = new Trk::TrkPlanarSurface(C,N,M,radLength,
								    &(pEL[index[iClusInSP]]->surface()));
	      //std::cout<<"created SCT surface"<<std::endl;pS->m_report();
	      /*
	      double locCov;
	      try {
		const Trk::ErrorMatrix& errMatRef=pCL[index[iClusInSP]]->localErrorMatrix();
		locCov=errMatRef.covariance()[0][0];
	      }
	      catch(Trk::PrepRawDataUndefinedVariable) {
		locCov=pEL[index[iClusInSP]]->phiPitch();
		locCov=locCov*locCov/12.0;
	      }
	      // override
	      
	      locCov=pEL[index[iClusInSP]]->phiPitch();
	      locCov=locCov*locCov/12.0;
	      */

	      if(pEL[index[iClusInSP]]->design().shape()!=InDetDD::Trapezoid)
		{
		  //vpTrkNodes.push_back(new Trk::TrkClusterNode(pS,DChi2,pCL[index[iClusInSP]]->localPosition()[0],locCov));
		  vpTrkNodes.push_back(new Trk::TrkClusterNode(pS,DChi2,pCL[index[iClusInSP]]));
		}
	      else
		{	  
		  const Trk::SurfaceBounds& rBounds=rSurf.bounds();
		  const Trk::TrapezoidBounds& ecBounds=
		    dynamic_cast<const Trk::TrapezoidBounds&>(rBounds);
		  double R=(ecBounds.maxHalflengthX()+ecBounds.minHalflengthX())*
		    ecBounds.halflengthY()/
		    (ecBounds.maxHalflengthX()-ecBounds.minHalflengthX());
		  vpTrkNodes.push_back(new Trk::TrkEndCapClusterNode(pS,DChi2,pCL[index[iClusInSP]],R));

		  //  vpTrkNodes.push_back(new Trk::TrkEndCapClusterNode(pS,DChi2,R,pCL[index[iClusInSP]]->localPosition()[0],locCov));
		}
	    }
	}
      else if(m_idHelper->is_pixel(ID))
	{
	  const InDet::SiCluster* pCL=pSP->clusters().first;
	  
	  if(pCL)
	    {
	      
	      const IdentifierHash idHash=
		m_pixelId->wafer_hash(m_pixelId->wafer_id(pCL->identify()));
	      InDetDD::SiDetectorElement* pEL=m_pixelManager->getDetectorElement(idHash);
	      const Trk::Surface& rSurf=pEL->surface();
	      
	      //const Trk::Surface& rSurf=pCL->detectorElement()->surface();

	      N[0]=rSurf.normal().x();
	      N[1]=rSurf.normal().y();
	      N[2]=rSurf.normal().z();
	      C[0]=rSurf.center().x();
	      C[1]=rSurf.center().y();
	      C[2]=rSurf.center().z();
	      mx=rSurf.transform().rotation().block(0,0,3,1);
	      my=rSurf.transform().rotation().block(0,1,3,1);
	      mz=rSurf.transform().rotation().block(0,2,3,1);
	      for(i=0;i<3;i++) 
		{
		  M[i][0]=mx[i];M[i][1]=my[i];M[i][2]=mz[i];
		}
	      Trk::TrkPlanarSurface* pS = new Trk::TrkPlanarSurface(C,N,M,radLength,&(pEL->surface()));
	      /*
	      std::cout<<"created PIX surface"<<std::endl;
	      std::cout<<"local position: "<<pCL->localPosition()[0]<<" "<<pCL->localPosition()[1]<<std::endl;
	      pS->m_report();
	      */
	      //	      double locPos[2];
	      /*
	      double locCov[4];
	      try {
		const Trk::ErrorMatrix& errMatRef=pCL->localErrorMatrix();
		locCov[0]=errMatRef.covariance()[0][0];
		locCov[1]=errMatRef.covariance()[0][1];
		locCov[2]=errMatRef.covariance()[1][0];
		locCov[3]=errMatRef.covariance()[1][1];
	      }
	      catch(Trk::PrepRawDataUndefinedVariable) {
		//locCov[0]=pEL->phiPitch()*pEL->phiPitch()/12.0;
		locCov[1]=0.0;locCov[2]=0.0;
		//locCov[3]=pEL->etaPitch()*pEL->etaPitch()/12.0;
		locCov[3]=0.3*0.3;locCov[0]=0.012*0.012;
	      }
	      */
	      // override
	      /*
	      locCov[0]=pEL->phiPitch()*pEL->phiPitch()/12.0;
	      locCov[1]=0.0;locCov[2]=0.0;
	      locCov[3]=pEL->etaPitch()*pEL->etaPitch()/12.0;
	      locPos[0]=pCL->localPosition()[0];
	      locPos[1]=pCL->localPosition()[1];    
	      */
	      //vpTrkNodes.push_back(new Trk::TrkPixelNode(pS,DChi2,locPos,locCov));
	      vpTrkNodes.push_back(new Trk::TrkPixelNode(pS,DChi2,pCL));
	    }
	}
    }
  ATH_MSG_DEBUG(vpTrkNodes.size());

  return true;
}

bool TrigDkfTrackMakerTool::createDkfTrack(const Trk::Track& track, 
		std::vector<Trk::TrkBaseNode*>& vpTrkNodes,
		double DChi2) {

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

const InDetDD::SiDetectorElement* TrigDkfTrackMakerTool::getSCTDetectorElement(const IdentifierHash& waferHash) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey};
  if (not  condData.isValid()) return nullptr;
  return condData->getDetectorElement(waferHash);
}
