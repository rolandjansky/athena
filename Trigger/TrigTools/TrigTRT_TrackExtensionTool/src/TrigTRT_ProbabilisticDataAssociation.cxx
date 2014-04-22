/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<vector>
#include<cmath>
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_ProbabilisticDataAssociation.h"

//#define PDAF_DBG 1

TrigTRT_BasePDAF::TrigTRT_BasePDAF(double eff, 
				   double prob, 
				   double res, 
				   double cut,
				   double aCut,
				   bool wH) : m_hitEfficiency(eff), 
					      m_noiseProbability(prob),
					      m_hitResolution(res),
					      m_validationCut(cut),
					      m_associationCut(aCut),
					      m_makeWireHits(wH)
{
  m_P0=0.0;m_strawRadius=2.0;  
  m_VS.clear();m_vpSW.clear();
}

TrigTRT_BasePDAF::~TrigTRT_BasePDAF()
{

}

/*
double TrigTRT_BasePDAF::m_errorFunction(double x)
{
  return erff(x);
}
*/

double TrigTRT_BasePDAF::m_errorFunction(double x)
{
  if(x>5.0) return 1.0;

  double x2=x*x;
  double res = 1.0-exp(-x2*(1+0.27323956/(1+0.14*x2)));
  res = sqrt(res);

  // printf("Argument x=%f appr = %f erff = %f\n",x,res,erff(x));

  return res;

}

/*
double TrigTRT_BasePDAF::m_errorFunction(double x)
{
  const double sqrtpi=1.7724538;
  const double tol=1.0e-10;


  double x2, sum, sum1, term,res ;
  long int i = 0;

  if(x<20.0)
    {
      if(x>tol)
	{
	  x2 = x * x;sum = x;term = x;
	  do 
	    {
	      i++;sum1 = sum;
	      term = 2.0 * term * x2/(1.0 + 2.0 * i);
	      sum = term + sum1;
	    } while (term >= tol * sum);
	  res=2.0 * sum * exp(-x2)/sqrtpi;
	}
      else res=0.0;
      if(fabs(res)<1e-8) res=0.0;
      return res;
    }
  else return 1.0;
}
*/

double TrigTRT_BasePDAF::m_calculatePDetect(double y1,double y2)
{
  if(y1>0.0)
    {
      return 0.5*(m_errorFunction(y2)-m_errorFunction(y1));
    }
  else
    {
      if(y2>0.0)
	{
	  return 0.5*(m_errorFunction(y2)+m_errorFunction(fabs(y1)));
	}
      else
	{
	  return 0.5*(m_errorFunction(fabs(y1))-m_errorFunction(fabs(y2)));
	}
    }
}

void TrigTRT_BasePDAF::m_runKalmanFilter(Trk::TrkTrackState* pTS)
{
  pTS->m_report();

}

void TrigTRT_BasePDAF::m_clear()
{
  for(std::vector<TrigTRT_Straw*>::iterator psIt=m_VS.begin();psIt!=m_VS.end();++psIt)
    {
      delete (*psIt);
    }
  for(std::vector<WeightedStraw*>::iterator pwIt=m_vpSW.begin();pwIt!=m_vpSW.end();++pwIt)
    {
      delete (*pwIt);
    }
  m_VS.clear();m_vpSW.clear();
}

TrigTRT_BarrelPDAF::TrigTRT_BarrelPDAF(double eff, 
				       double prob, 
				       double res, 
				       double cut,
				       double aCut,
				       bool wH) : TrigTRT_BasePDAF(eff,prob,res,
								       cut,aCut,wH)
{

}

TrigTRT_BarrelPDAF::~TrigTRT_BarrelPDAF()
{
  m_clear();
}

void TrigTRT_BarrelPDAF::m_update(Trk::TrkTrackState* pTS, TrigTRT_Info* pTI, Trk::TrkTrackState* pUS)
{
  double PE=0.0,covRes,covWire,sQ,sQwire,norm,PD,
    weightedResW,weightedResD,squaredResW,squaredResD,
    BetaW,BetaD;
  const InDet::TRT_DriftCircle* pBestHit=NULL;
  double bestProb=0.0;
  
  WeightedStraw* pSW;
  std::vector<WeightedStraw*>::iterator pswIt;
  int i,j;
  
#ifdef PDAF_DBG
  printf("Barrel PDAF Pbkg=%f\n",m_noiseProbability);
#endif
  
  covRes=m_hitResolution*m_hitResolution+pTS->m_getTrackCovariance(0,0);
  covWire=4.0*m_strawRadius*m_strawRadius/12.0+pTS->m_getTrackCovariance(0,0);

#ifdef PDAF_DBG
  printf("Track pars: %f %f %f %f %f\n",
	 pTS->m_getTrackState(0),pTS->m_getTrackState(1),
	 pTS->m_getTrackState(2),pTS->m_getTrackState(3),pTS->m_getTrackState(4));
  printf("sigmaRes=%f sigmaWire=%f\n",sqrt(covRes),sqrt(covWire));
#endif

  sQ=1.0/sqrt(2.0*3.1415926*covRes);
  sQwire=1.0/sqrt(2.0*3.1415926*covWire);
  
  for(std::vector<TrigTRT_Straw*>::iterator psIt=m_VS.begin();
      psIt!=m_VS.end();++psIt)
    {
      TrigTRT_Straw* pS=(*psIt);
      double x1,x2;
#ifdef PDAF_DBG
      printf("Ys=%f\n",pS->m_localCoordinate());     
#endif

      x1=pS->m_localCoordinate()-m_strawRadius;
      x2=pS->m_localCoordinate()+m_strawRadius;      
      double y1,y2;
      y1=sqrt(2.0/pTS->m_getTrackCovariance(0,0))*(x1-pTS->m_getTrackState(0));
      y2=sqrt(2.0/pTS->m_getTrackCovariance(0,0))*(x2-pTS->m_getTrackState(0));
      PD=m_calculatePDetect(y1,y2);
      if(fabs(PD)<1e-8) PD=0.0;

      if(pTI!=NULL) pTI->m_addCrossedStraw(PD);

#ifdef PDAF_DBG
      printf("x1=%f tp=%f x2=%f y1=%f y2=%f\n",
	     x1,pTS->m_getTrackState(0),x2,y1,y2);     
#endif
      //      PD=1.0/pVS->size();

      const InDet::TRT_DriftCircle* pH=pS->m_getDC();
      if(pH!=NULL) 
	{
	  pSW=new WeightedStraw;
	  pSW->m_PD=PD;
	  pSW->m_pS=pS;
	  if((pH->localPosition()[0]<0.001)||(m_makeWireHits && (fabs(pH->localPosition()[0]-2.0)<0.001)))
	    {
	      pSW->m_nComponents=1;
	      pSW->m_Resid[0]=pS->m_localCoordinate()-pTS->m_getTrackState(0);
	      pSW->m_Weight[0]=m_hitEfficiency*(1.0-m_noiseProbability)*pSW->m_PD*sQwire*
		exp(-0.5*pSW->m_Resid[0]*pSW->m_Resid[0]/covWire);

	      if(pUS!=NULL)
		{
		  pSW->m_Resid[0]=pS->m_localCoordinate()-pUS->m_getTrackState(0);
		}
	    }
	  else
	    {
	      pSW->m_nComponents=2;
	      pSW->m_Resid[0]=pS->m_localCoordinate()-pH->localPosition()[0]-pTS->m_getTrackState(0);
	      pSW->m_Resid[1]=pS->m_localCoordinate()+pH->localPosition()[0]-pTS->m_getTrackState(0);
	      for(i=0;i<2;i++)
		{
		  pSW->m_Weight[i]=m_hitEfficiency*(1.0-m_noiseProbability)*pSW->m_PD*sQ*
		    exp(-0.5*pSW->m_Resid[i]*pSW->m_Resid[i]/covRes);
		}
	      if(pUS!=NULL)
		{
		  pSW->m_Resid[0]=pS->m_localCoordinate()-pH->localPosition()[0]-pUS->m_getTrackState(0);
		  pSW->m_Resid[1]=pS->m_localCoordinate()+pH->localPosition()[0]-pUS->m_getTrackState(0);
		}
	    }
#ifdef PDAF_DBG
	  printf("   q=%f PD=%f\n",pH->localPosition()[0],pSW->m_PD);
	  for(int m=0;m<pSW->m_nComponents;m++)
	    {
	      printf("   res=%f beta=%f\n",pSW->m_Resid[m],pSW->m_Weight[m]);
	    }
#endif
	  m_vpSW.push_back(pSW);
	}
      else
	{
	  PE+=PD;
	}
    }
  norm=m_noiseProbability*(1.0-m_hitEfficiency*PE)/m_strawRadius;
  m_P0=norm;
#ifdef PDAF_DBG
  printf("PE=%f unnorm P0=%f\n",PE,m_P0);
#endif
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      for(i=0;i<(*pswIt)->m_nComponents;i++) norm+=(*pswIt)->m_Weight[i];
    }
  norm=1.0/norm;
  m_P0*=norm;
  
  BetaD=0.0;BetaW=0.0;
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      (*pswIt)->m_P=0.0;
      for(i=0;i<(*pswIt)->m_nComponents;i++) 
	{
	  (*pswIt)->m_Weight[i]*=norm;
	  (*pswIt)->m_P+=(*pswIt)->m_Weight[i];
	}
      if((*pswIt)->m_nComponents==1) BetaW+=(*pswIt)->m_P;
      else BetaD+=(*pswIt)->m_P;
    }
#ifdef PDAF_DBG
  printf("P0=%f BetaD=%f BetaW=%f\n",m_P0,BetaD,BetaW);
#endif

  weightedResW=0.0;weightedResD=0.0;
  squaredResW=0.0;squaredResD=0.0;
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      if((*pswIt)->m_nComponents==1)
	{
	  weightedResW+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0];
	  squaredResW+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]*(*pswIt)->m_Resid[0];
	}
      else
	{
	  weightedResD+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]+
	    (*pswIt)->m_Weight[1]*(*pswIt)->m_Resid[1];
	  squaredResD+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]*(*pswIt)->m_Resid[0]+
	    (*pswIt)->m_Weight[1]*(*pswIt)->m_Resid[1]*(*pswIt)->m_Resid[1];
	}
    }

  double updPar[5],updCov[15],Kw[5],Kd[5],Bk[5],dGw[5][5],dGd[5][5],Ga[5][5];
  
  Trk::TrkTrackState* pAS=(pUS!=NULL)?pUS:pTS;

  covRes=m_hitResolution*m_hitResolution+pAS->m_getTrackCovariance(0,0);
  covWire=4.0*m_strawRadius*m_strawRadius/12.0+pAS->m_getTrackCovariance(0,0);
  
  for(i=0;i<5;i++) 
    {
      Bk[i]=pAS->m_getTrackCovariance(0,i);
      Kd[i]=Bk[i]/covRes;
      Kw[i]=Bk[i]/covWire; 
    }

  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	dGd[i][j]=-Kd[i]*Bk[j];
	dGw[i][j]=-Kw[i]*Bk[j];
      }

  for(i=0;i<5;i++) 
    {
      updPar[i]=Kd[i]*weightedResD+Kw[i]*weightedResW;
    }

  pAS->m_updateTrackState(updPar);

  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	Ga[i][j]=Kw[i]*Kw[j]*(squaredResW-weightedResW*weightedResW)+
	  Kd[i]*Kd[j]*(squaredResD-weightedResD*weightedResD);
      }
 
#ifdef PDAF_DBG
  printf("weighted resD=%f resW=%f new pars %f %f %f %f %f\n",
	 weightedResD,weightedResW,pTS->m_getTrackState(0),
	 pTS->m_getTrackState(1),pTS->m_getTrackState(2),pTS->m_getTrackState(3),pTS->m_getTrackState(4));
#endif
  int idx=0;
  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	updCov[idx]=BetaW*dGw[i][j]+BetaD*dGd[i][j]+Ga[i][j];
	idx++;
      }
  
  pAS->m_updateTrackCovariance(updCov);
  
  if(pTI!=NULL)
    {
      pTI->m_addCrossedLayer();
      pTI->m_addDetectionWeight(1.0-m_P0);
      for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
	{
	  if((*pswIt)->m_pS->m_getDC()!=NULL)
	    {
	      bool isHigh=(*pswIt)->m_pS->m_getDC()->highLevel();
	      if(isHigh) 
		pTI->m_addTR_HitWeight((*pswIt)->m_P,((*pswIt)->m_nComponents==2));
	      pTI->m_addTRT_HitWeight((*pswIt)->m_P,((*pswIt)->m_nComponents==2));
	      if((*pswIt)->m_P>bestProb)
		{
		  bestProb=(*pswIt)->m_P;
		  pBestHit=(*pswIt)->m_pS->m_getDC();
		}
	    }
	}
      if((bestProb>m_associationCut)&&(pBestHit!=NULL))
	{
	  pTI->m_addTRT_Hit(pBestHit);
	}
    }

  m_clear();
}

bool TrigTRT_BarrelPDAF::m_validateTRT_Hit(Trk::TrkTrackState* pTS, 
					   TrigTRT_Straw* pS)
{
  double res=pS->m_localCoordinate()-pTS->m_getTrackState(0); 

  if(fabs(res)<m_validationCut)
    {
      m_VS.push_back(new TrigTRT_Straw(pS));
      if(pS->m_getDC()!=NULL) return true;
      else return false;
    }
  return false;
}

TrigTRT_EndcapPDAF::TrigTRT_EndcapPDAF(double eff, 
				       double prob, 
				       double res, 
				       double cut,
				       double aCut,
				       bool wH) : TrigTRT_BasePDAF(eff,prob,res,
								       cut,aCut,wH)
{

}


TrigTRT_EndcapPDAF::~TrigTRT_EndcapPDAF()
{
  m_clear();
}

void TrigTRT_EndcapPDAF::m_update(Trk::TrkTrackState* pTS, TrigTRT_Info* pTI, Trk::TrkTrackState* pUS)
{
  double PE=0.0,covRes,covWire,sQ,sQwire,norm,PD,
    weightedResW,weightedResD,squaredResW,squaredResD,
    BetaW,BetaD;

  const InDet::TRT_DriftCircle* pBestHit=NULL;
  double bestProb=0.0;

  WeightedStraw* pSW;
  std::vector<WeightedStraw*>::iterator pswIt;
  int i,j;
  
#ifdef PDAF_DBG
  printf("Endcap PDAF Pbkg=%f\n",m_noiseProbability);
#endif

  covRes=m_hitResolution*m_hitResolution+pTS->m_getTrackCovariance(1,1);
  covWire=4.0*m_strawRadius*m_strawRadius/12.0+pTS->m_getTrackCovariance(1,1);

#ifdef PDAF_DBG
  printf("Track pars: %f %f %f %f %f\n",
	 pTS->m_getTrackState(0),pTS->m_getTrackState(1),
	 pTS->m_getTrackState(2),pTS->m_getTrackState(3),pTS->m_getTrackState(4));
  printf("sigmaRes=%f sigmaWire=%f\n",sqrt(covRes),sqrt(covWire));
#endif

  sQ=1.0/sqrt(2.0*3.1415926*covRes);
  sQwire=1.0/sqrt(2.0*3.1415926*covWire);

  for(std::vector<TrigTRT_Straw*>::iterator psIt=m_VS.begin();
      psIt!=m_VS.end();++psIt)
    {
      TrigTRT_Straw* pS=(*psIt);
      double x1,x2;
#ifdef PDAF_DBG
      printf("Xs=%f\n",pS->m_localCoordinate());     
#endif
      double q=m_strawRadius;
      x1=pS->m_localCoordinate()-q;
      x2=pS->m_localCoordinate()+q;      
      double y1,y2,cov;
      cov=sqrt(2.0/(pTS->m_getTrackCovariance(1,1)));
      y1=cov*(x1-pTS->m_getTrackState(1));
      y2=cov*(x2-pTS->m_getTrackState(1));
      PD=m_calculatePDetect(y1,y2);
      if(fabs(PD)<1e-8) PD=0.0;
      if(pTI!=NULL) pTI->m_addCrossedStraw(PD);

#ifdef PDAF_DBG
      printf("x1=%f tp=%f x2=%f y1=%f y2=%f\n",
	     x1,pTS->m_getTrackState(1),x2,y1,y2);     
#endif

      const InDet::TRT_DriftCircle* pH=pS->m_getDC();
      if(pH!=NULL) 
	{
	  pSW=new WeightedStraw;
	  pSW->m_PD=PD;
	  pSW->m_pS=pS;
	  //	  if(pH->localPosition()[0]<0.001)
	  if((pH->localPosition()[0]<0.001)||(m_makeWireHits && (fabs(pH->localPosition()[0]-2.0)<0.001)))
	    {
	      pSW->m_nComponents=1;
	      pSW->m_Resid[0]=pS->m_localCoordinate()-pTS->m_getTrackState(1);
	      pSW->m_Weight[0]=m_hitEfficiency*(1.0-m_noiseProbability)*pSW->m_PD*sQwire*
		exp(-0.5*pSW->m_Resid[0]*pSW->m_Resid[0]/covWire);

	      if(pUS!=NULL)
		{
		  pSW->m_Resid[0]=pS->m_localCoordinate()-pUS->m_getTrackState(1);
		}
	    }
	  else
	    {
	      pSW->m_nComponents=2;
	      q=pH->localPosition()[0];
	      pSW->m_Resid[0]=pS->m_localCoordinate()-q-pTS->m_getTrackState(1);
	      pSW->m_Resid[1]=pS->m_localCoordinate()+q-pTS->m_getTrackState(1);
	      for(i=0;i<2;i++)
		{
		  pSW->m_Weight[i]=m_hitEfficiency*(1.0-m_noiseProbability)*pSW->m_PD*sQ*
		    exp(-0.5*pSW->m_Resid[i]*pSW->m_Resid[i]/covRes);
		}
	      
	      if(pUS!=NULL)
		{
		  pSW->m_Resid[0]=pS->m_localCoordinate()-q-pUS->m_getTrackState(1);
		  pSW->m_Resid[1]=pS->m_localCoordinate()+q-pUS->m_getTrackState(1);
		}
	    }
#ifdef PDAF_DBG
	  printf("   q=%f PD=%f\n",pH->localPosition()[0],pSW->m_PD);
	  for(int m=0;m<pSW->m_nComponents;m++)
	    {
	      printf("   res=%f beta=%f\n",pSW->m_Resid[m],pSW->m_Weight[m]);
	    }
#endif
	  m_vpSW.push_back(pSW);
	}
      else
	{
	  PE+=PD;
	}
    }
  norm=m_noiseProbability*(1.0-m_hitEfficiency*PE)/m_strawRadius;
  m_P0=norm;
#ifdef PDAF_DBG
  printf("PE=%f unnorm P0=%f\n",PE,m_P0);
#endif
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      for(i=0;i<(*pswIt)->m_nComponents;i++) norm+=(*pswIt)->m_Weight[i];
    }
  norm=1.0/norm;
  m_P0*=norm;

  BetaD=0.0;BetaW=0.0;
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      (*pswIt)->m_P=0.0;
      for(i=0;i<(*pswIt)->m_nComponents;i++) 
	{
	  (*pswIt)->m_Weight[i]*=norm;
	  (*pswIt)->m_P+=(*pswIt)->m_Weight[i];
	}
      if((*pswIt)->m_nComponents==1) BetaW+=(*pswIt)->m_P;
      else BetaD+=(*pswIt)->m_P;
    }
#ifdef PDAF_DBG
  printf("P0=%f BetaD=%f BetaW=%f\n",m_P0,BetaD,BetaW);
#endif

  weightedResW=0.0;weightedResD=0.0;
  squaredResW=0.0;squaredResD=0.0;
  for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
    {
      if((*pswIt)->m_nComponents==1)
	{
	  weightedResW+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0];
	  squaredResW+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]*(*pswIt)->m_Resid[0];
	}
      else
	{
	  weightedResD+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]+
	    (*pswIt)->m_Weight[1]*(*pswIt)->m_Resid[1];
	  squaredResD+=(*pswIt)->m_Weight[0]*(*pswIt)->m_Resid[0]*(*pswIt)->m_Resid[0]+
	    (*pswIt)->m_Weight[1]*(*pswIt)->m_Resid[1]*(*pswIt)->m_Resid[1];
	}
    }

  Trk::TrkTrackState* pAS=(pUS!=NULL)?pUS:pTS;

  double Kw[5],Kd[5],Bk[5],dGw[5][5],dGd[5][5],Ga[5][5],updPar[5],updCov[15];

  covRes=m_hitResolution*m_hitResolution+pAS->m_getTrackCovariance(1,1);
  covWire=4.0*m_strawRadius*m_strawRadius/12.0+pAS->m_getTrackCovariance(1,1);

  Bk[0]=pAS->m_getTrackCovariance(0,1);
  Bk[1]=pAS->m_getTrackCovariance(1,1);
  Bk[2]=pAS->m_getTrackCovariance(1,2);
  Bk[3]=pAS->m_getTrackCovariance(1,3);
  Bk[4]=pAS->m_getTrackCovariance(1,4);

  for(i=0;i<5;i++) 
    {
      Kd[i]=Bk[i]/covRes;
      Kw[i]=Bk[i]/covWire; 
    }

  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	dGd[i][j]=-Kd[i]*Bk[j];
	dGw[i][j]=-Kw[i]*Bk[j];
      }
  for(i=0;i<5;i++) 
    {
      updPar[i]=Kd[i]*weightedResD+Kw[i]*weightedResW;
    }

  pAS->m_updateTrackState(updPar);

  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	Ga[i][j]=Kw[i]*Kw[j]*(squaredResW-weightedResW*weightedResW)+
	  Kd[i]*Kd[j]*(squaredResD-weightedResD*weightedResD);
      }
 
#ifdef PDAF_DBG
  printf("weighted resD=%f resW=%f new pars %f %f %f %f %f\n",
	 weightedResD,weightedResW,pTS->m_getTrackState(0),
	 pTS->m_getTrackState(1),pTS->m_getTrackState(2),pTS->m_getTrackState(3),pTS->m_getTrackState(4));
#endif
  int idx=0;
  for(i=0;i<5;i++)
    for(j=i;j<5;j++)
      {
	updCov[idx]=BetaW*dGw[i][j]+BetaD*dGd[i][j]+Ga[i][j];
	idx++;
      }
  
  pAS->m_updateTrackCovariance(updCov);

  if(pTI!=NULL)
    {
      pTI->m_addCrossedLayer();
      pTI->m_addDetectionWeight(1.0-m_P0);
      for(pswIt=m_vpSW.begin();pswIt!=m_vpSW.end();++pswIt)
	{
	  if((*pswIt)->m_pS->m_getDC()!=NULL)
	    {
	      bool isHigh=(*pswIt)->m_pS->m_getDC()->highLevel();
	      if(isHigh) pTI->m_addTR_HitWeight((*pswIt)->m_P,((*pswIt)->m_nComponents==2));
	      pTI->m_addTRT_HitWeight((*pswIt)->m_P,((*pswIt)->m_nComponents==2));
	      if((*pswIt)->m_P>bestProb)
		{
		  bestProb=(*pswIt)->m_P;
		  pBestHit=(*pswIt)->m_pS->m_getDC();
		}
	    }
	}
      if((bestProb>m_associationCut)&&(pBestHit!=NULL))
	{
	  pTI->m_addTRT_Hit(pBestHit);
	}
    }
  m_clear();
}

bool TrigTRT_EndcapPDAF::m_validateTRT_Hit(Trk::TrkTrackState* pTS, 
					   TrigTRT_Straw* pS)
{
  double res;
  res=pS->m_localCoordinate()-pTS->m_getTrackState(1);
  if(fabs(res)<m_validationCut)
    {
      m_VS.push_back(new TrigTRT_Straw(pS));
      if(pS->m_getDC()!=NULL) return true;
      else return false;
    }
  return false;
}
