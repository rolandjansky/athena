/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p3.h"
#include "CxxUtils/make_unique.h"
#include <cstring>
using std::memset;


bool TrigInDetTrackFitParCnv_p3 :: calculateSquareRoot(const TrigInDetTrackFitPar* p, TrigInDetTrackFitPar_p3* pP)
{
  int i,j,idx=0;
  double a[5][5];
  float L[5][5];

  if((p->cov()==0)||(p->cov()->size()==0))
    {
      memset(&pP->m_cov[0],0,sizeof(pP->m_cov));
      pP->m_cov[0]=-999.0;
      pP->m_cov[2]=-999.0;
      pP->m_cov[5]=-999.0;
      pP->m_cov[9]=-999.0;
      pP->m_cov[14]=-999.0;
      return false;
    }

  for(i=0;i<5;i++)
    for(j=i;j<5;j++) a[i][j]=a[j][i]=(*p->cov())[idx++];

  if(CholeskyDecomposition(a,L))
    {
      idx=0;
      for(i=0;i<5;i++) for(j=0;j<=i;j++) pP->m_cov[idx++]=L[i][j];
    }
  else 
    {
      memset(&pP->m_cov[0],0,sizeof(pP->m_cov));
      pP->m_cov[0] =(float)sqrt( (*p->cov())[0]  > 0 ? (*p->cov())[0]  : 0 );
      pP->m_cov[2] =(float)sqrt( (*p->cov())[5]  > 0 ? (*p->cov())[5]  : 0 );
      pP->m_cov[5] =(float)sqrt( (*p->cov())[9]  > 0 ? (*p->cov())[9]  : 0 );
      pP->m_cov[9] =(float)sqrt( (*p->cov())[12] > 0 ? (*p->cov())[12] : 0 );
      pP->m_cov[14]=(float)sqrt( (*p->cov())[14] > 0 ? (*p->cov())[14] : 0 );
    }
  return true;
}

std::unique_ptr<std::vector<double> >
TrigInDetTrackFitParCnv_p3 :: restoreCovariance(const TrigInDetTrackFitPar_p3* pP)
{

  int i,j,k,idx=0;
  float L[5][5], LT[5][5];
  if(pP->m_cov[0]<0.0)
    {
      return nullptr;
    }

  memset(&L[0][0],0,sizeof(L));memset(&LT[0][0],0,sizeof(LT));

  for(i=0;i<5;i++) for(j=0;j<=i;j++) 
    {
      L[i][j]=LT[j][i]=pP->m_cov[idx++];
    }

  auto pV = CxxUtils::make_unique<std::vector<double> >();

  for(i=0;i<5;i++)
  
  for(j=i;j<5;j++)
    {
      // Note: use extended (long double) precision here.
      // That happens implicitly on x86 with optimization on; saying it
      // explicitly ensures that we get the same results with and without
      // optimization.  (If this is a performance issue for platforms
      // other than x86, one could change to double for those platforms.)
      long double C=0.0;
      for(k=0;k<5;k++)
        C+=(long double)L[i][k]*LT[k][j];
      pV->push_back(C);
    }

  return pV;
}

bool TrigInDetTrackFitParCnv_p3 :: CholeskyDecomposition(double a[5][5], float L[5][5])
{

  int i,j,k;
  double sum;
  float p[5];

  for(i=0;i<5;i++)
    {
      for(j=i;j<5;j++)
	{
	  sum=a[i][j];
	  for(k=i-1;k>=0;k--)
	    sum-=a[i][k]*a[j][k];
	  if(i==j)
	    {
	      if(sum<=0.0)
		{
		  return false;
		}
	      p[i]=sqrt(sum);L[i][i]=p[i];
	    }
	  else
	    { 
	      a[j][i]=sum/p[i];
	      L[j][i]=a[j][i];
	    }
	}
    }
  return true;
}



void TrigInDetTrackFitParCnv_p3 :: persToTrans( const TrigInDetTrackFitPar_p3 *persObj, TrigInDetTrackFitPar *transObj, MsgStream& //log 
) {

  // log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p3::persToTrans" << endreq;
  
  std::unique_ptr<std::vector<double> > cov  (restoreCovariance(persObj));

  double ea0 = -999;
  double ephi0 = -999;
  double ez0 = -999;
  double eeta = -999;
  double epT = -999;
  if (cov) {
    ea0   = sqrt((*cov)[0]);
    ephi0 = sqrt((*cov)[5]);
    ez0   = sqrt((*cov)[9]);
    eeta  = sqrt((*cov)[12]);
    epT   = sqrt((*cov)[14]);
  }
  
  *transObj = TrigInDetTrackFitPar (persObj->m_allDoubles[0], // a0
                                    persObj->m_allDoubles[1], // phi0
                                    persObj->m_allDoubles[2], // z0
                                    persObj->m_allDoubles[3], // eta
                                    persObj->m_allDoubles[4], // pT
                                    ea0, ephi0, ez0, eeta, epT,
                                    (TrigInDetTrackFitPar::TrigSurfaceType) persObj->m_surfaceType,
                                    persObj->m_allDoubles[5], // surfaceCoordinate
                                    cov.release());
}



void TrigInDetTrackFitParCnv_p3 :: transToPers( const TrigInDetTrackFitPar    *transObj, TrigInDetTrackFitPar_p3 *persObj, MsgStream& //log 
) {

  // log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p3::transToPers" << endreq;


  persObj->m_allDoubles[0] = transObj->a0()    ;
  persObj->m_allDoubles[1] = transObj->phi0()  ;
  persObj->m_allDoubles[2] = transObj->z0()    ;
  persObj->m_allDoubles[3] = transObj->eta()   ;
  persObj->m_allDoubles[4] = transObj->pT()    ;
  persObj->m_allDoubles[5] = transObj->surfaceCoordinate();

  calculateSquareRoot(transObj,persObj);
  persObj->m_surfaceType       = transObj->surfaceType();
}
