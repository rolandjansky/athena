/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigInDetCombinedTrackFitter tool
// -------------------------------
// ATLAS Collaboration
//
// 15.06.2010 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetCombinedTrackFitter.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigInDetCombinedTrackFitter::TrigInDetCombinedTrackFitter(const std::string& t, 
							     const std::string& n,
							     const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_doMultScatt(true),
  m_doBremm(false),
  m_momentumThreshold(10000.0),
  m_highPtFitter("TrigL2HighPtTrackFitter"),
  m_lowPtFitter("TrigL2LowPtTrackFitter"),
  m_trackMaker("TrigDkfTrackMakerTool")
{
  declareInterface< ITrigInDetTrackFitter >( this ); 
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremm=false);  
  declareProperty( "HighPtTrackFitter", m_highPtFitter, "TrigL2HighPtTrackFitter");
  declareProperty( "LowPtTrackFitter", m_lowPtFitter, "TrigL2LowPtTrackFitter");
  declareProperty( "MomentumThreshold",m_momentumThreshold=10000.0);  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
}

StatusCode TrigInDetCombinedTrackFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  sc=m_trackMaker.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve "<<m_trackMaker);
      return sc;
    }
  sc=m_highPtFitter.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve "<<m_highPtFitter);
      return sc;
    }
  sc=m_lowPtFitter.retrieve();
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve "<<m_lowPtFitter);
      return sc;
    }
  ATH_MSG_INFO("TrigInDetCombinedTrackFitter initialized ");
  return sc;
}

StatusCode TrigInDetCombinedTrackFitter::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


void TrigInDetCombinedTrackFitter::fit(TrigInDetTrackCollection* recoTracks )
{
  double Rk[5], Phi0, D0, Z0, Eta, errD0, errZ0, errEta, errPhi0, errPt;
  double CV[5][5],a,b,c,Theta;
  std::vector<double>* pCov;
  TrigInDetTrackCollection::iterator trIt,lastIt;

  std::vector<Trk::TrkBaseNode*> vpTrkNodes;

  trIt = recoTracks->begin();lastIt = recoTracks->end();
  for(; trIt !=lastIt; trIt++) 
    {
      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      if(param==NULL)
	{
    ATH_MSG_WARNING("Fit Failed -- TrigInDetTrack has no parameters");
	  continue;
	}
      if(fabs(param->pT())<100.0)
	{
    ATH_MSG_DEBUG("Estimated Pt is too low "<<param->pT()<<" - skipping fit");
	  continue;
	}
      
      // 1. Create filtering nodes

      bool trackResult = m_trackMaker->createDkfTrack(*(*trIt)->siSpacePoints(),vpTrkNodes, m_DChi2);
      if(!trackResult) continue;

      int nHits=vpTrkNodes.size();

			ATH_MSG_DEBUG(nHits<<" filtering nodes created");

      // 2. Create initial track state:

      double chi2tot=0.0;
      int ndoftot=-5;

      Rk[0]=param->a0();
      Rk[1]=param->z0();
      Rk[2]=param->phi0();
      Theta=2.0*atan(exp(-param->eta()));
      Rk[3]=Theta;
      Rk[4]=sin(Theta)/param->pT();
      double Momentum = 1/Rk[4];

      Trk::TrkTrackState* pTS = new Trk::TrkTrackState(Rk);
      double Gk[5][5];
      memset(&Gk[0][0],0,sizeof(Gk));
      Gk[0][0]=100.0;Gk[1][1]=1000.0;Gk[2][2]=0.01;Gk[3][3]=0.01;Gk[4][4]=1e-6;
      pTS->m_setTrackCovariance(Gk);
      if(m_doMultScatt)  
	pTS->m_setScatteringMode(1);
      if(m_doBremm)
	pTS->m_setScatteringMode(2);

			ATH_MSG_DEBUG("Initial params: locT="<<Rk[0]<<" locL="<<Rk[1]<<" phi="<<Rk[2]
		  <<" theta="<<Rk[3]<<" Q="<<Rk[4]<<" pT="<<sin(Rk[3])/Rk[4]);

      // 3. Call appropriate fitter
        
      if(fabs(Momentum)<m_momentumThreshold)
	{
	  pTS = m_lowPtFitter->fit(pTS,vpTrkNodes,true);
	}
      else
	{
	  pTS = m_highPtFitter->fit(pTS,vpTrkNodes,true);
	}
      
      //pTS = m_lowPtFitter->fit(pTS,vpTrkNodes);
      std::vector<Trk::TrkBaseNode*>::iterator pnIt(vpTrkNodes.begin()),
	pnEnd(vpTrkNodes.end());
      if(pTS!=NULL)
	{
	  
	  for(;pnIt!=pnEnd;++pnIt)
	    {
				ATH_MSG_DEBUG("dChi2="<<(*pnIt)->m_getChi2());
	      if((*pnIt)->m_isValidated())
		{
		  chi2tot+=(*pnIt)->m_getChi2();
		  ndoftot+=(*pnIt)->m_getNdof();
		}
	    }
	  double Pt=sin(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
	  Phi0 = pTS->m_getTrackState(2);
	  if(Phi0>M_PI) Phi0-=2*M_PI;
	  if(Phi0<-M_PI) Phi0+=2*M_PI;
	  Eta = -log(sin(0.5*pTS->m_getTrackState(3))/cos(0.5*pTS->m_getTrackState(3)));
	  Z0 = pTS->m_getTrackState(1);
	  D0 = pTS->m_getTrackState(0);

	  errD0 = sqrt(pTS->m_getTrackCovariance(0,0));
	  errZ0 = sqrt(pTS->m_getTrackCovariance(1,1));
	  errPhi0 = sqrt(pTS->m_getTrackCovariance(2,2));
	  errEta = sqrt(pTS->m_getTrackCovariance(3,3))/fabs(sin(pTS->m_getTrackState(3)));
	  b=cos(pTS->m_getTrackState(3))/pTS->m_getTrackState(4);
	  c=-Pt/pTS->m_getTrackState(4);
	  a=-1.0/sin(pTS->m_getTrackState(3));
	  errPt = sqrt(b*b*(pTS->m_getTrackCovariance(3,3))+c*c*(pTS->m_getTrackCovariance(4,4))+
		       2.0*b*c*(pTS->m_getTrackCovariance(3,4)));
	  
	  pCov=new std::vector<double>;
      
	  CV[0][0]=pTS->m_getTrackCovariance(0,0);
	  CV[0][1]=pTS->m_getTrackCovariance(0,2);
	  CV[0][2]=pTS->m_getTrackCovariance(0,1);
	  CV[0][3]=a*(pTS->m_getTrackCovariance(0,3));
	  CV[0][4]=b*(pTS->m_getTrackCovariance(0,3))+c*(pTS->m_getTrackCovariance(0,4));
	  CV[1][1]=pTS->m_getTrackCovariance(2,2);
	  
	  CV[1][2]=pTS->m_getTrackCovariance(1,2);
	  CV[1][3]=a*(pTS->m_getTrackCovariance(2,3));
	  CV[1][4]=b*(pTS->m_getTrackCovariance(2,3))+c*(pTS->m_getTrackCovariance(2,4));
	  CV[2][2]=pTS->m_getTrackCovariance(1,1);
	  CV[2][3]=a*(pTS->m_getTrackCovariance(1,3));
	  CV[2][4]=b*(pTS->m_getTrackCovariance(1,3))+c*(pTS->m_getTrackCovariance(1,4));
	  CV[3][3]=a*a*(pTS->m_getTrackCovariance(3,3));
	  CV[3][4]=a*(b*(pTS->m_getTrackCovariance(3,3))+c*(pTS->m_getTrackCovariance(3,4)));
	  CV[4][4]=b*b*(pTS->m_getTrackCovariance(3,3))+2.0*b*c*(pTS->m_getTrackCovariance(3,4))+
	    c*c*(pTS->m_getTrackCovariance(4,4));

	  for(int i=0;i<5;i++)
	    for(int j=i;j<5;j++) pCov->push_back(CV[i][j]);

	  const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
									errD0,errPhi0,errZ0,
									errEta,errPt,pCov);
	  delete pTS;
            
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
	      (*trIt)->param(tidtfp);
	      if(ndoftot>1) chi2tot/=ndoftot;
	      (*trIt)->chi2(chi2tot);
	    }
	}
      else
	{
	  ATH_MSG_DEBUG("Track fitter failure ");
	  (*trIt)->chi2(1e8);
	}

      pnIt=vpTrkNodes.begin();pnEnd=vpTrkNodes.end();
      for(;pnIt!=pnEnd;++pnIt) 
	{
	  delete((*pnIt)->m_getSurface());
	  delete (*pnIt);
	}
      vpTrkNodes.clear();
    }
}

