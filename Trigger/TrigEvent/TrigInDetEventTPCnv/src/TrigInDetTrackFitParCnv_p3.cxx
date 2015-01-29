/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#undef private

#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p3.h"
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

bool TrigInDetTrackFitParCnv_p3 :: restoreCovariance(const TrigInDetTrackFitPar_p3* pP, TrigInDetTrackFitPar* p)
{

  int i,j,k,idx=0;
  float L[5][5], LT[5][5];
  if(pP->m_cov[0]<0.0)
    {
      return false;
    }

  memset(&L[0][0],0,sizeof(L));memset(&LT[0][0],0,sizeof(LT));

  for(i=0;i<5;i++) for(j=0;j<=i;j++) 
    {
      L[i][j]=LT[j][i]=pP->m_cov[idx++];
    }

  if(p->cov()!=0) delete p->cov();

  std::vector<double>* pV=new std::vector<double>;

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

  p->cov(pV);

  return true;

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
  
  transObj->m_a0                = persObj->m_allDoubles[0];
  transObj->m_phi0              = persObj->m_allDoubles[1];
  transObj->m_z0                = persObj->m_allDoubles[2];
  transObj->m_eta               = persObj->m_allDoubles[3];
  transObj->m_pT                = persObj->m_allDoubles[4];
  transObj->m_surfaceCoordinate = persObj->m_allDoubles[5];

  //restore param errors from the cov matrix
  if(restoreCovariance(persObj,transObj))
    {
      transObj->m_ea0               = sqrt( (*(transObj->cov()))[0] ); 
      transObj->m_ephi0             = sqrt( (*(transObj->cov()))[5] ); 
      transObj->m_ez0               = sqrt( (*(transObj->cov()))[9] ); 
      transObj->m_eeta              = sqrt( (*(transObj->cov()))[12] );
      transObj->m_epT               = sqrt( (*(transObj->cov()))[14] );
    }
  else
    {
      transObj->cov(0);
      transObj->m_ea0               = -999; 
      transObj->m_ephi0             = -999; 
      transObj->m_ez0               = -999; 
      transObj->m_eeta              = -999;
      transObj->m_epT               = -999;
    }

  transObj->m_surfaceType       = persObj->m_surfaceType;
}



void TrigInDetTrackFitParCnv_p3 :: transToPers( const TrigInDetTrackFitPar    *transObj, TrigInDetTrackFitPar_p3 *persObj, MsgStream& //log 
) {

  // log << MSG::DEBUG << "TrigInDetTrackFitParCnv_p3::transToPers" << endreq;


  persObj->m_allDoubles[0] = transObj->m_a0    ;
  persObj->m_allDoubles[1] = transObj->m_phi0  ;
  persObj->m_allDoubles[2] = transObj->m_z0    ;
  persObj->m_allDoubles[3] = transObj->m_eta   ;
  persObj->m_allDoubles[4] = transObj->m_pT    ;
  persObj->m_allDoubles[5] = transObj->m_surfaceCoordinate;

  calculateSquareRoot(transObj,persObj);
  persObj->m_surfaceType       = transObj->m_surfaceType;
}
