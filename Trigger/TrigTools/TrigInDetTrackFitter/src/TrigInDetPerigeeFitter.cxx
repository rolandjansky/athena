/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h" 
#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetTrackFitter/TrigInDetPerigeeFitter.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrigInDetTrackFitter/PerigeeFilteringNodes.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

TrigInDetPerigeeFitter::TrigInDetPerigeeFitter(const std::string& t, 
					       const std::string& n,
					       const IInterface*  p ): AthAlgTool(t,n,p),
								       m_MagFieldSvc("AtlasFieldSvc",this->name())
{
  declareInterface< ITrigInDetTrackFitter >( this );
  declareProperty( "StraightLineMode",     m_straightLineMode = false );
  declareProperty( "AthenaFieldService", m_MagFieldSvc, "AtlasFieldSvc");
  declareProperty( "doMultScattering", m_doMultScatt = true);
  declareProperty( "doBremmCorrection", m_doBremmCorr=false);
  
  declareProperty( "Chi2Cut", m_DChi2 = 1000.0);
}

StatusCode TrigInDetPerigeeFitter::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if ( sc.isFailure() ) { 
    ATH_MSG_FATAL("DetStore service not found"); 
    return StatusCode::FAILURE; 
  };

  if(!m_straightLineMode)
    {
	  sc = m_MagFieldSvc.retrieve();
	  if(sc.isFailure()) 
	    {
	      ATH_MSG_ERROR("Unable to retrieve Athena MagFieldService");
	      return StatusCode::FAILURE;
	    }
	  double pos[3];
	  pos[0]=0.0;pos[1]=0.0;pos[2]=0.0;
	  double field[3];
	  m_MagFieldSvc->getField(pos,field);
	  m_consb=0.029997*field[2];
	  if(fabs(m_consb)<1e-6) 
	    {
	      m_straightLineMode=true;
	      m_consb=0.6;
	    }
    }
  else 
    {
      m_consb=0.6;
    }
  if(m_straightLineMode)
    {
      ATH_MSG_INFO("Straight line fitting mode ON ");
    }
  else
    {
      ATH_MSG_INFO("Field constant is set to "<<m_consb);
    }
  /*
  ITrigTimerSvc* timerSvc;
  sc = service( "TrigTimerSvc", timerSvc);
  if( sc.isFailure() ) {
    ATH_MSG_INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("CreateNodes");
    m_timer[1] = timerSvc->addItem("ForwardFilter");
    m_timer[2] = timerSvc->addItem("Smoother");
    m_timer[3] = timerSvc->addItem("TrigTrackPars");
    m_timer[4] = timerSvc->addItem("CleanUp");
    m_timer[5] = timerSvc->addItem("Validate");
    m_timer[6] = timerSvc->addItem("Update");
  }
  */
  ATH_MSG_INFO("TrigInDetPerigeeFitter constructed ");
  return sc;
}

StatusCode TrigInDetPerigeeFitter::finalize()
{
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigInDetPerigeeFitter::~TrigInDetPerigeeFitter()
{

}


void TrigInDetPerigeeFitter::fit(TrigInDetTrackCollection* recoTracks)
{
  int i,nType,nHits;
  double chi2tot;
  int ndoftot;
  double Rk[5];
  double Gk[15];
  BarrelPerigeeFilteringNode* pBS;
  DiscPerigeeFilteringNode* pDS;
  std::vector<class BasePerigeeFilteringNode*> vpHits;

  double Pt, Phi0, D0, Z0, Eta, errD0, errZ0, errEta, errPhi0, errPt;
  double CV[5][5],a,b;
  std::vector<double>* pCov;

  vpHits.clear();
  TrigInDetTrackCollection::iterator trIt = recoTracks->begin();
  TrigInDetTrackCollection::iterator lastIt = recoTracks->end();
  for(; trIt !=lastIt; trIt++) 
    {
      if((*trIt)->siSpacePoints()->size()==0) 
	{
		ATH_MSG_ERROR("Perigee Fit Failed -- TrigInDetTrack has no hits");
	  continue;
	}
      std::vector<const TrigSiSpacePoint*>::iterator pSPIt=(*trIt)->siSpacePoints()->begin();
      std::vector<const TrigSiSpacePoint*>::iterator lastSPIt=(*trIt)->siSpacePoints()->end();
      for(; pSPIt != lastSPIt; pSPIt++) 
	{
	  const TrigSiSpacePoint* pSP=(*pSPIt);
	  nType=(pSP->dr()>pSP->dz())?1:0;
	  switch(nType)
	    {
	    case 0:
	      {
		pBS=new BarrelPerigeeFilteringNode(pSP);
		vpHits.push_back(pBS);
		break;
	      }
	    case 1: 
	      {
		pDS=new DiscPerigeeFilteringNode(pSP);
		vpHits.push_back(pDS);
		break;
	      }
	    }
	}
      nHits=vpHits.size();
      TrigInDetTrackFitPar* param=const_cast<TrigInDetTrackFitPar*>((*trIt)->param());
      if(param==NULL)
	{
		ATH_MSG_ERROR("Perigee Fit Failed -- TrigInDetTrack has no parameters"); 
	  for(std::vector<class BasePerigeeFilteringNode*>::iterator phIt=vpHits.begin();phIt!=vpHits.end();++phIt)
	     {
	       delete (*phIt);
	     }
	   vpHits.clear();
	   continue;
	}
      else
	{
	  double tgHalfTheta=exp(-param->eta());
	  Rk[4]=0.5*(1-tgHalfTheta*tgHalfTheta)/tgHalfTheta;
	  if(fabs(Rk[4])<1e-6) Rk[4]=1e-6;
	  Rk[2]=param->phi0();
	  Rk[0]=0.5*m_consb/(param->pT()*1000.0);
	  Rk[1]=param->a0();
	  Rk[3]=param->z0();
	  for(i=0;i<15;i++) Gk[i]=0.0;
	  Gk[0]=0.01;Gk[2]=1.0;Gk[5]=10.0;Gk[9]=10.0;Gk[14]=0.2;
	  if(m_straightLineMode)
	    {
	      Rk[0]=1e-10;Gk[0]=1e-8;
	    }
	  chi2tot=0.0;ndoftot=0;
	  for(std::vector<class BasePerigeeFilteringNode*>::reverse_iterator pbhIt=vpHits.rbegin();
	      pbhIt!=vpHits.rend();++pbhIt)
	    {
	      double dchi2=(*pbhIt)->m_getChi2(Rk,Gk);
	      if(dchi2<m_DChi2)
		{
		  (*pbhIt)->m_acceptIt();
		  chi2tot+=dchi2;
		  ndoftot+=(*pbhIt)->m_getNdof();
		  (*pbhIt)->m_runFilter(Rk,Gk);
		}
	      else (*pbhIt)->m_rejectIt();
	      if(m_doMultScatt) (*pbhIt)->m_applyMultScatt(Rk,Gk);
	      if(m_doBremmCorr) (*pbhIt)->m_applyEnergyLoss(Rk,Gk,-1); 
	    }
	  chi2tot/=ndoftot-5;
	  Pt = -m_consb*0.5/Rk[0];   // put the right sign on Pt
	  Phi0 = Rk[2];
	  if(Phi0>M_PI) Phi0-=2*M_PI;
	  if(Phi0<-M_PI) Phi0+=2*M_PI;
	  Eta = log(Rk[4]+sqrt(1+Rk[4]*Rk[4]));
	  Z0 = Rk[3];
	  D0 = Rk[1];

	  errD0 = sqrt(Gk[2]);
	  errZ0 = sqrt(Gk[9]);
	  errEta = sqrt(Gk[14]/(1+Rk[4]*Rk[4]));
	  errPhi0 = sqrt(Gk[5]);
	  errPt = 0.5*m_consb*sqrt(Gk[0])/(Rk[0]*Rk[0]);

	  pCov=new std::vector<double>;
	  a=1/sqrt(1+Rk[4]*Rk[4]);
	  b=m_consb*0.5/(Rk[0]*Rk[0]);
	  CV[0][0]=Gk[2];
	  CV[0][1]=Gk[4];
	  CV[0][2]=Gk[7];
	  CV[0][3]=a*Gk[11];
	  CV[0][4]=b*Gk[1];
	  CV[1][1]=Gk[5];
	  CV[1][2]=Gk[8];
	  CV[1][3]=a*Gk[12];
	  CV[1][4]=b*Gk[3];
	  CV[2][2]=Gk[9];
	  CV[2][3]=a*Gk[13];
	  CV[2][4]=b*Gk[6];
	  CV[3][3]=a*a*Gk[14];
	  CV[3][4]=a*b*Gk[10];
	  CV[4][4]=b*b*Gk[0];
	  for(int i=0;i<5;i++)
	    for(int j=i;j<5;j++) pCov->push_back(CV[i][j]);
	  const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta, Pt,
									errD0,errPhi0,errZ0,
									errEta,errPt,pCov);
		ATH_MSG_DEBUG("Perigee parameters: d0="<<D0<<" phi0="<<Phi0<<" z0="<<Z0
			<<" eta0="<<Eta<<" pt="<<Pt);
	  delete param;
	  (*trIt)->param(tidtfp);
	  (*trIt)->chi2(chi2tot);
	  (*trIt)->siSpacePoints()->clear();
	  int nRejected=0;
	  for(std::vector<class BasePerigeeFilteringNode*>::iterator phIt=vpHits.begin();
	      phIt!=vpHits.end();++phIt)
	    {
	      if((*phIt)->m_isAccepted())
		{
		  (*trIt)->siSpacePoints()->push_back((*phIt)->m_getTrigSp());
		}
	      else nRejected++;
	      delete (*phIt);
	    }
		ATH_MSG_DEBUG(nHits<<" hits, "<<nRejected<<" rejected ");
	  vpHits.clear();
	}
    }
}

