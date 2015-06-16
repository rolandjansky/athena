/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigInDetOfflineTrackFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 08.05.2010 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrkTrack/TrackCollection.h"

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetOfflineTrackFitter.h"

TrigInDetOfflineTrackFitter::TrigInDetOfflineTrackFitter(const std::string& t, 
					   const std::string& n,
					   const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_trackFitter("Trk::GlobalChi2Fitter/InDetTrigTrackFitter")
{
  declareInterface< ITrigInDetTrackFitter >( this );

  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
  declareProperty( "OfflineClusters", m_offlineClusters = true);
  declareProperty("OfflineTrackFitter", m_trackFitter);
}

StatusCode TrigInDetOfflineTrackFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  
  sc=m_trackFitter.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve "<<m_trackFitter);
      return sc;
    }
  
  /*
  StatusCode scFit = toolSvc->retrieveTool(m_FitterName, m_FitterInstance, m_trackFitter);
  if (scFit.isFailure())
    {
      athenaLog<<MSG::ERROR<<"Could not find refit tool of type "<<m_FitterName<<". Exiting."<< endreq;
      return scFit;
    }
  else 
    {
      athenaLog << MSG::INFO << "Refit tool \""<<m_FitterName<<" "<<m_FitterInstance<<"\" booked."<< endreq;
    }
  */
  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper");
    return StatusCode::FAILURE;
  }
   
  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    ATH_MSG_INFO("Unable to locate Service TrigTimerSvc ");
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
//   add some timers:
//
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("TrigToOffline");
    m_timer[1] = timerSvc->addItem("OfflineFit");
    m_timer[2] = timerSvc->addItem("OfflineToTrig");
  }
  ATH_MSG_INFO("TrigInDetOflineTrackFitter constructed ");
  return sc;
}

StatusCode TrigInDetOfflineTrackFitter::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

void TrigInDetOfflineTrackFitter::fit(TrigInDetTrackCollection* recoTracks )
{
  double Pt, Phi0, D0, Z0, Eta, errD0, errZ0, errEta, errPhi0, errPt,a,b,c;
  double CV[5][5],Theta;
  std::vector<double>* pCov;
  TrigInDetTrackCollection::iterator trIt,lastIt;

  if(m_timers)
    {
      m_timer[0]->start();
      m_timer[0]->pause();
      m_timer[1]->start();
      m_timer[1]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
    }

  trIt = recoTracks->begin();lastIt = recoTracks->end();
  for(; trIt !=lastIt; trIt++) 
    {
      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      if(param==NULL)
	{
    ATH_MSG_WARNING("Fit Failed -- TrigInDetTrack has no parameters");
	  continue;
	}
      Pt=param->pT();
      if(fabs(Pt)<100.0)
	{
		ATH_MSG_DEBUG("Estimated Pt is too low "<<Pt<<" - skipping fit");
	  continue;
	}
      if (!(*trIt)->siSpacePoints())
	{
		ATH_MSG_DEBUG("Track has no spacepoints - skipping fit");
	  continue;
	}

      // 1. TrigInDetTrack -> TrkTrack
      if ( m_timers ) m_timer[0]->resume();
      Phi0=param->phi0();
      Theta=2.0*atan(exp(-param->eta()));	
      AmgSymMatrix(5)* pM = new AmgSymMatrix(5);
      pM->setZero();
      (*pM)(0,0)=100.0;
      (*pM)(1,1)=100.0;
      (*pM)(2,2)=0.01;
      (*pM)(3,3)=0.01;
      (*pM)(4,4)=1e-6;
      const Trk::PerigeeSurface   perSurf;
      const Trk::Perigee* pMP = new Trk::Perigee(param->a0(),param->z0(),Phi0,Theta,sin(Theta)/Pt,perSurf, pM);

      Trk::PrepRawDataSet prepRDColl;

      std::vector<const TrigSiSpacePoint*>::iterator pSP=(*trIt)->siSpacePoints()->begin();
      std::vector<const TrigSiSpacePoint*>::iterator lastSP=(*trIt)->siSpacePoints()->end();
      for(int iSP=0; pSP != lastSP; pSP++, iSP++)
	{
	  if (m_sctId->is_sct((*pSP)->identify()))
	    {
	      for (int iClusInSP=0; iClusInSP<2; iClusInSP++)
		{
		  const InDet::SCT_Cluster* pCL;
		  if (iClusInSP == 1)
		    pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().first);
		  else                     
		    pCL = dynamic_cast<const InDet::SCT_Cluster*>((*pSP)->clusters().second);
		  if(!pCL) continue;
		  /*
		  const Trk::LocalParameters*  loc = new Trk::LocalParameters(pCL->localPosition());
		  const Trk::GlobalPosition  glo(pCL->globalPosition());
		  const Trk::ErrorMatrix*    err = new Trk::ErrorMatrix(pCL->localErrorMatrix ());
		  const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(pCL->identify()));
		  InDet::SCT_ClusterOnTrack* pSC=new InDet::SCT_ClusterOnTrack(pCL,loc,err,idHash,glo);
		  */
		  prepRDColl.push_back(pCL);	                            
		}
	    }
	  else 
	    {  // Pixel spacepoint
	      const InDet::PixelCluster* pCL = dynamic_cast<const InDet::PixelCluster*>((*pSP)->clusters().first);
	      if(!pCL) continue;
	      /*
	      const Trk::LocalParameters*  loc = new Trk::LocalParameters(pCL->localPosition());
	      const Trk::GlobalPosition  glo(pCL->globalPosition());
	      const Trk::ErrorMatrix*    err = new Trk::ErrorMatrix(pCL->localErrorMatrix ());
	      const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pCL->identify()));
	      InDet::PixelClusterOnTrack* pSC=new InDet::PixelClusterOnTrack(pCL,loc,err,idHash,glo,
									     pCL->gangedPixel());
	      */
	      prepRDColl.push_back(pCL);
	    }
	}
      if ( m_timers ) m_timer[0]->pause();
      if ( m_timers ) m_timer[1]->resume();

      Trk::Track* newtrack = m_trackFitter->fit(prepRDColl,*pMP,false,Trk::pion);

      if ( m_timers ) m_timer[1]->pause();
      if ( m_timers ) m_timer[2]->resume();

      delete pMP;

      bool badFit=false;
      if(newtrack!=NULL)
	{
	  const Trk::Perigee *aMeasPer = newtrack->perigeeParameters();
	  if (aMeasPer==nullptr)
	    {
	      ATH_MSG_DEBUG("Could not get Trk::MeasuredPerigee");
	      badFit=true;
	    }
	  else
	    {
	      D0 = aMeasPer->parameters()[Trk::d0];
	      Z0 = aMeasPer->parameters()[Trk::z0];
	      Phi0 = aMeasPer->parameters()[Trk::phi0];
	      Theta = aMeasPer->parameters()[Trk::theta];
	      if(Phi0>M_PI) Phi0-=2*M_PI;
	      if(Phi0<-M_PI) Phi0+=2*M_PI;
	      Eta = -log(sin(0.5*Theta)/cos(0.5*Theta));

	      double qOverP = aMeasPer->parameters()[Trk::qOverP];
	      Pt=sin(Theta)/qOverP;
				ATH_MSG_DEBUG("Refitted offline parameters " << D0  << " " << Z0  << " " 
					<< Phi0 << " " << Eta << " " << Pt);

	      double Gk[5][5];

	      for(int i=0;i<5;i++) 
		for(int j=0;j<5;j++)
		  Gk[i][j]=(*aMeasPer->covariance())(i,j);

	      errD0 = sqrt(Gk[0][0]);
	      errZ0 = sqrt(Gk[1][1]);
	      errPhi0 = sqrt(Gk[2][2]);
	      errEta = sqrt(Gk[3][3])/fabs(sin(Theta));
	      b=cos(Theta)/qOverP;
	      c=-Pt/qOverP;
	      a=-1.0/sin(Theta);
	      errPt = sqrt(b*b*(Gk[3][3])+c*c*Gk[4][4]+2.0*b*c*Gk[3][4]);
	    
	      pCov=new std::vector<double>;
	      
	      CV[0][0]=Gk[0][0];
	      CV[0][1]=Gk[0][2];
	      CV[0][2]=Gk[0][1];
	      CV[0][3]=a*Gk[0][3];
	      CV[0][4]=b*Gk[0][3]+c*Gk[0][4];
	      CV[1][1]=Gk[2][2];
	    
	      CV[1][2]=Gk[1][2];
	      CV[1][3]=a*Gk[2][3];
	      CV[1][4]=b*Gk[2][3]+c*Gk[2][4];
	      CV[2][2]=Gk[1][1];
	      CV[2][3]=a*Gk[1][3];
	      CV[2][4]=b*Gk[1][3]+c*Gk[1][4];
	      CV[3][3]=a*a*Gk[3][3];
	      CV[3][4]=a*(b*Gk[3][3]+c*Gk[3][4]);
	      CV[4][4]=b*b*Gk[3][3]+2.0*b*c*Gk[3][4]+c*c*Gk[4][4];

	      const Trk::FitQuality* pFQ=newtrack->fitQuality();  
	      double chi2tot=pFQ->chiSquared();
	      int ndoftot=pFQ->numberDoF();
			    
	      for(int i=0;i<5;i++)
		for(int j=i;j<5;j++) pCov->push_back(CV[i][j]);
	      pCov->push_back(ndoftot+0.001);

	      const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
									    errD0,errPhi0,errZ0,
									    errEta,errPt,pCov);

		  ATH_MSG_DEBUG("Total chi2 ="<<chi2tot<<" NDOF="<<ndoftot);
		  ATH_MSG_DEBUG("Fitted parameters: d0="<<D0<<" phi0="<<Phi0<<" z0="<<Z0	
			    <<" eta0="<<Eta<<" pt="<<Pt);
	      if((ndoftot<0) || (fabs(Pt)<100.0) || (std::isnan(Pt)))
		{
	    ATH_MSG_DEBUG("Fit failed - possibly floating point problem");
		  delete tidtfp;
		  (*trIt)->chi2(1e8);
		}
	      else
		{
		  delete param;
		  if(ndoftot>1) chi2tot/=ndoftot;
		  (*trIt)->param(tidtfp);
		  (*trIt)->chi2(chi2tot);
		}
	    }
	}
      else badFit=true;
      if(badFit)
	{
		ATH_MSG_DEBUG("Offline fitter failed");
	  (*trIt)->chi2(1e8);
	}
      delete newtrack;
      if ( m_timers ) m_timer[2]->pause();
    }
  if(m_timers) 
    {
      m_timer[0]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
    }	  
}
