/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrigInDetBremDetectionTool.cxx
//   Source file for TrigInDetBremDetectionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Dmitry.Emeliyanov@cern.ch
///////////////////////////////////////////////////////////////////

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkFilteringNodes.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrigInDetTrackFitter/TrigInDetBremDetectionTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <cmath>
#include <cstring>
using std::memset;

//#define IDE_DEBUG 1

TrigInDetBremDetectionTool::TrigInDetBremDetectionTool(const std::string& t,const std::string& n,const IInterface* p) :
  AthAlgTool (t,n,p),
  m_log(msgSvc(), n),
  m_outputLevel(1),
  m_minDistance(10.0),
  m_P0(0.6),
  m_SignificanceCut(4.5)
{
  declareProperty("minDistance",m_minDistance);
  declareProperty("noBremProbability",m_P0);
  declareProperty("MinSignificance",m_SignificanceCut);
  declareInterface<ITrigInDetBremDetectionTool>( this );
  m_pLS=NULL;
}

// destructor
TrigInDetBremDetectionTool::~TrigInDetBremDetectionTool()
{

}
// initialize
StatusCode TrigInDetBremDetectionTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  m_log.setLevel(outputLevel());            // individual outputlevel not known before initialise
  m_outputLevel=msgSvc()->outputLevel( name() );
  m_Threshold=2.0*log(m_P0/(1.0-m_P0));

  m_log << MSG::INFO << "initialize() successful in " << name() << endreq;
  m_log << MSG::INFO << "Input detection threshold is set to " << m_Threshold << endreq;

  return sc;
}

// finalize
StatusCode TrigInDetBremDetectionTool::finalize()
{
  if(m_pLS!=NULL) delete m_pLS;

  m_log << MSG::INFO << "finalize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

void TrigInDetBremDetectionTool::reset()
{
  m_totalPath=0.0;
  m_lsmSize=0;m_resSizes.clear();m_resSize=0;
  memset(&m_A[0][0],0,sizeof(m_A));
  memset(&m_MG[0][0],0,sizeof(m_MG));
  memset(&m_S[0][0],0,sizeof(m_S));
  if(m_pLS!=NULL) delete m_pLS;m_pLS=NULL;
}

bool TrigInDetBremDetectionTool::addNewPoint(Trk::TrkTrackState* pTS,
					     Trk::TrkBaseNode* pN, Trk::TrkPlanarSurface* pS, double F[5][5],
					     double delta)
{
  int i,j,dim,k;
  double res[5];
  double K[5][2];
  double H[2][5];
  double S[2][2];
  double mu[5][5];
  double HF[2][5];
  double M[5];
  double lP[3],gP[3];


  m_totalPath+=fabs(delta);

  dim=pN->m_getKalmanGain(K);
  dim=pN->m_getMeasurementMatrix(H);
  dim=pN->m_getInverseResidualVariance(S);
  dim=pN->m_getResiduals(res);

#ifdef IDE_DEBUG

  m_log<<MSG::INFO<<"new measurement, size="<<dim<< endreq;
  m_log<< MSG::INFO << " Accumulated path = "<< m_totalPath  << endreq;

  printf("Jacobian:\n");
  for(i=0;i<5;i++)
    {
      for(j=0;j<5;j++)
	printf("%E ",F[i][j]);
      printf("\n");
    }

#endif

  if(dim!=0)
    {
#ifdef IDE_DEBUG
      printf("Measurement matrix:\n");
      for(i=0;i<dim;i++)
	{
	  for(j=0;j<5;j++)
	    printf("%E ",H[i][j]);
	  printf("\n");
	}  
#endif
      if(m_lsmSize!=0)
	{
	  m_resSizes.push_back(dim);
	  for(i=0;i<dim;i++) 
	    {
	      m_R[m_resSize]=res[i];
	      for(j=0;j<dim;j++) m_S[m_resSize][j]=S[i][j];
	      m_resSize++;
	    }
	}
      for(i=0;i<dim;i++)
	for(j=0;j<5;j++)
	  {
	    HF[i][j]=0.0;
	    for(k=0;k<5;k++) HF[i][j]+=H[i][k]*F[k][j];
	  }
      for(i=0;i<5;i++)
	for(j=0;j<5;j++) 
	  {
	    mu[i][j]=F[i][j];
	    for(k=0;k<dim;k++) mu[i][j]-=K[i][k]*HF[k][j];
	  }
    }
  else
    {
      for(i=0;i<5;i++)
	for(j=0;j<5;j++) 
	  {
	    mu[i][j]=F[i][j];
	  }
    }  
  for(i=1;i<=m_lsmSize;i++)
    {
      for(k=0;k<5;k++)
	{
	  M[k]=0.0;
	  for(j=0;j<5;j++) M[k]+=mu[k][j]*m_MG[j][i];
	}
      for(k=0;k<5;k++) m_MG[k][i]=M[k];
    }
  for(i=0;i<dim;i++)
    {
      for(k=1;k<=m_lsmSize;k++)
	{
	  m_A[i+m_resSize-dim][k-1]=0.0;
	  for(j=0;j<5;j++) m_A[i+m_resSize-dim][k-1]+=HF[i][j]*m_MG[j][k];
	}
    }
#ifdef IDE_DEBUG
  printf("Matrix A:\n");
  for(i=0;i<m_resSize;i++)
    {
      for(j=0;j<m_lsmSize;j++) printf("%E ",m_A[i][j]);
      printf("\n");
    }
#endif
  if(m_totalPath<m_minDistance) return false;

  m_totalPath=0.0;
  
  m_lsmSize++;  

  for(i=0;i<4;i++)
  {
    m_MG[i][m_lsmSize]=0.0;
  }
  m_MG[4][m_lsmSize]=1.0;

  m_surfArray[m_lsmSize-1]=pS;

  lP[0]=pTS->m_getTrackState(0);lP[1]=pTS->m_getTrackState(1);
  lP[2]=0.0;
  pS->m_transformPointToGlobal(lP,gP);
  m_jX[m_lsmSize-1]=gP[0];m_jY[m_lsmSize-1]=gP[1];m_jZ[m_lsmSize-1]=gP[2]; 
#ifdef IDE_DEBUG 
  m_log << MSG::INFO << " Added new jump at "<<gP[0]<<" "<<gP[1]<<" "<<gP[2]<<endreq;
#endif
  return true;
}

bool TrigInDetBremDetectionTool::solve(int Sign)
{
  int i;
  bool isSolved=false;
  bool isFeasible=false;
  int size=m_lsmSize-1;
  int nIter;
  double Chi2Opt;
  double g[MAX_INP_SIZE];

  m_sign=Sign;
  m_size=m_lsmSize-1;
  m_precomputeGain();
  if(m_pLS!=NULL) delete m_pLS;m_pLS=NULL;

  m_pLS=new LSMSolution(size);
  for(i=0;i<m_size;i++)
    {
      m_pLS->m_fixVariable(i);
    }
  for(i=0;i<MAX_INP_SIZE;i++)
    {
      m_Zarray[i]=1;m_Parray[i]=0;
    }
  nIter=0;
  while(!isSolved)
    {
      m_getGradient(m_pLS,g);
#ifdef IDE_DEBUG
      printf("++++ Gradient: ++++ Sign %d\n",Sign);
      for(i=0;i<size;i++)
	{
	  printf("%E ",g[i]);
	}
      printf("\n");
#endif
      Chi2Opt=m_getCriterionValue(m_pLS);
#ifdef IDE_DEBUG
      printf("***** Criterion = %f ******\n",Chi2Opt);
#endif
      if(nIter>100) break;
      if(m_isZempty()) 
	{
	  isSolved=true;break;
	}
      if(m_goodGradient(g))
	{
#ifdef IDE_DEBUG
	  printf("Good gradient - solved\n");
#endif
	  isSolved=true;break;
	}
#ifdef IDE_DEBUG
      else printf("Not good gradient - searching ...\n");
#endif
      int bestI=m_findBestDirection(g);
#ifdef IDE_DEBUG
      printf("Selected %d grad = %f\n",bestI,g[bestI]);
#endif
      m_Zarray[bestI]=0;m_Parray[bestI]=1;
#ifdef IDE_DEBUG
      for(i=0;i<m_size;i++) printf("%d ",m_Zarray[i]);
      printf("\n");
      for(i=0;i<m_size;i++) printf("%d ",m_Parray[i]);
      printf("\n");
#endif
      isFeasible=false;
     
      while(!isFeasible)
	{
	  LSMSolution* pST=m_solveLSM();
#ifdef IDE_DEBUG
	  pST->m_report();
#endif
	  isFeasible=m_checkFeasibility(pST);
	  if(!isFeasible)
	    {
#ifdef IDE_DEBUG
	      printf("Non-feasible solution");
#endif
	      m_mixSolutions(pST,m_pLS);
	      delete pST;
	    }
	  else
	    {
#ifdef IDE_DEBUG
	      printf("Feasible solution\n");
#endif
	      delete m_pLS;
	      m_pLS=pST;
	    }
	}
      nIter++;
    }
  int nChanges=0;
  bool nonZero=false;
  
  for(i=0;i<m_size;i++)
    {
      m_Zarray[i]=1;
      if(!m_pLS->m_isOnConstraint(i))
	{
	  m_Zarray[i]=0;
	  double utmp=(*m_pLS)[i];
	  (*m_pLS)[i]=0.0;
	  double Chi2Zero=m_getCriterionValue(m_pLS);
	  if(m_outputLevel<=MSG::DEBUG)
	    {
	      m_log<<MSG::DEBUG<<i+1<<" Chi2* = "<<Chi2Opt<<" Chi20 = "<<Chi2Zero<<" Th="<<m_Threshold<<endreq;
	    }
	  (*m_pLS)[i]=utmp;
	  double deltaChi2=Chi2Zero-Chi2Opt;
	  if(deltaChi2<m_Threshold)
	    {
	      m_Zarray[i]=1;
	      nChanges++;
	    }
	  else nonZero=true;

	}
    }
  if((nChanges!=0)&&nonZero)
    {
      LSMSolution* pUS=m_solveLSM();
      if(m_outputLevel<=MSG::DEBUG)
	{
	  m_log<<MSG::DEBUG<<"LSM Solution"<<endreq;
	  pUS->m_report();
	  Chi2Opt=m_getCriterionValue(pUS);
	  m_log<<MSG::DEBUG<<"Criterion = "<<Chi2Opt<<endreq;
	}
      delete m_pLS;
      m_pLS=pUS;
    }
  if(!nonZero)
    {
      delete m_pLS;
      m_pLS=new LSMSolution(size);
      for(i=0;i<m_size;i++)
	{
	  m_pLS->m_fixVariable(i);
	}
      if(m_outputLevel<=MSG::DEBUG)
	{
	  m_log<<MSG::DEBUG<<"LSM Solution"<<endreq;
	  m_pLS->m_report();
	  Chi2Opt=m_getCriterionValue(m_pLS);
	  m_log<<MSG::DEBUG<<"Criterion = "<<Chi2Opt<<endreq;
	}
    }
  return isSolved;
}

void TrigInDetBremDetectionTool::m_precomputeGain()
{
  int i,j,k;
  memset(&m_K[0][0],0,sizeof(m_K));

  int sJ,J0;

  for(i=0;i<m_lsmSize-1;i++)
    {
      J0=0;
      for(std::vector<int>::iterator iIt=m_resSizes.begin();iIt!=m_resSizes.end();++iIt)
	{
	  for(sJ=0;sJ<(*iIt);sJ++)
	    {
	      j=J0+sJ;
	      m_K[i][j]=0.0;
	      for(k=J0;k<J0+(*iIt);k++)
		{
		  m_K[i][j]+=m_A[k][i]*m_S[k][sJ];
		}
	      j++;
	    }
	  J0+=(*iIt);
	}
    }

  for(i=0;i<m_lsmSize-1;i++)
    for(j=i;j<m_lsmSize-1;j++)
      {
	m_W[i][j]=0.0;
	for(k=0;k<m_resSize;k++) m_W[i][j]+=m_K[i][k]*m_A[k][j];
	m_W[j][i]=m_W[i][j];
      }
}


void TrigInDetBremDetectionTool::m_getGradient(LSMSolution* pLS, double g[])
{
  int i,j;

  for(i=0;i<m_lsmSize-1;i++)
    {
      g[i]=0.0;
      for(j=i;j<m_resSize;j++) g[i]+=m_K[i][j]*m_R[j];
    }
  for(i=0;i<m_lsmSize-1;i++)
    {
      for(j=0;j<m_lsmSize-1;j++) g[i]-=m_W[i][j]*(*pLS)[j];
    }
}

bool TrigInDetBremDetectionTool::m_goodGradient(double* g)
{
  int i,np=0;
  for(i=0;i<m_lsmSize-1;i++)
    {
      if(m_Zarray[i]==0) continue;
      if(g[i]*m_sign>0.0) np++;
    }
  return (np==0);
}

int TrigInDetBremDetectionTool::m_findBestDirection(double* g)
{
  int i,bestI=0;
  double maxGrad=-1.0;
  for(i=0;i<m_lsmSize-1;i++)
    {
      if(g[i]*m_sign>maxGrad) 
	{
	  maxGrad=g[i]*m_sign;
	  bestI=i;
	}
    }
  return bestI;
}

bool TrigInDetBremDetectionTool::m_isZempty()
{
  int i;
  
  for(i=0;i<m_lsmSize-1;i++)
    {
      if(m_Zarray[i]==1) return false;
    }
  return true;
}

bool TrigInDetBremDetectionTool::CholeskyDecompositionNxN(double* a,int Size)
{
  int i,j,k;
  double sum;
  double* p=new double[Size];

  for(i=0;i<Size;i++)
    {
      for(j=i;j<Size;j++)
	{
	  sum=a[i+j*Size];
	  for(k=i-1;k>=0;k--)
	    sum-=a[i+k*Size]*a[j+k*Size];
	  if(i==j)
	    {
	      if(sum<=0.0)
		{
		  printf("Cholesky decomp. failed\n");delete[] p;
		  return false;
		}
	      p[i]=sqrt(sum);
	    }
	  else a[j+i*Size]=sum/p[i];
	}
    }
  for(i=0;i<Size;i++)
    {
      a[i+Size*i]=1.0/p[i];
      for(j=i+1;j<Size;j++)
	{
	  sum=0.0;for(k=i;k<j;k++) sum-=a[j+Size*k]*a[k+Size*i];
	  a[j+Size*i]=sum/p[j];
	}
    }
  delete[] p;
  return true;
}


bool TrigInDetBremDetectionTool::invertMatrixNxN(double* a, int size)
{
  double detr;
  int i,j;
  bool rc=false;

  if(size==1)
    {
      a[0]=1.0/a[0];
      rc=true;
    }
  else if(size==2)
    {
      detr=a[0]*a[3]-a[1]*a[2];
      detr=1.0/detr;
      double b[2][2];
      b[0][0]=a[3]*detr;
      b[1][0]=b[0][1]=-a[1]*detr;
      b[1][1]=a[0]*detr;
      for(i=0;i<2;i++) for(j=0;j<2;j++) a[i+j*size]=b[i][j];
      rc=true;
    }
  else if(size==3)
    {
      double A[3][3],b[3][3],M[3];
      for(i=0;i<3;i++) for(j=0;j<3;j++) A[i][j]=a[i+j*size];

      M[0]=A[1][1]*A[2][2]-A[1][2]*A[1][2];
      M[1]=A[0][1]*A[2][2]-A[1][2]*A[0][2];
      M[2]=A[0][1]*A[1][2]-A[1][1]*A[0][2];
      detr=A[0][0]*M[0]-A[0][1]*M[1]+A[0][2]*M[2];
      detr=1.0/detr;
      b[0][0]=M[0]*detr;
      b[0][1]=b[1][0]=-M[1]*detr;
      b[0][2]=b[2][0]=M[2]*detr;
      b[1][1]=(A[0][0]*A[2][2]-A[0][2]*A[0][2])*detr;
      b[1][2]=b[2][1]=-(A[0][0]*A[1][2]-A[0][1]*A[0][2])*detr;
      b[2][2]=(A[0][0]*A[1][1]-A[0][1]*A[0][1])*detr;
      for(i=0;i<3;i++) for(j=0;j<3;j++) a[i+j*size]=b[i][j];
      rc=true;
    }
  else if(size>3)
    {      
      if(!CholeskyDecompositionNxN(a,size)) return false;

      double* L=new double[size*size];
      double* LT=new double[size*size];

      memset(L,0,sizeof(double)*size*size);
      memset(LT,0,sizeof(double)*size*size);

      for(i=0;i<size;i++)
		  for(j=0;j<=i;j++)
		  {
			  L[i+j*size]=a[i+j*size];
			  LT[j+i*size]=L[i+size*j];
		  }
      for(i=0;i<size;i++)
	  {
		  for(j=i;j<size;j++)
		  {
			  double sum=0.0;
			  for(int k=0;k<size;k++) sum+=LT[i+k*size]*L[k+j*size];
			  a[j+i*size]=a[i+j*size]=sum;
		  }
	  }
      delete[] L;
      delete[] LT;
      rc=true;
    }
  return rc;
}


LSMSolution* TrigInDetBremDetectionTool::m_solveLSM()
{
  LSMSolution* pLS=NULL;

  int i,j,idxI,idxJ,newSize=m_size;
  double W[MAX_INP_SIZE][MAX_INP_SIZE];
  double P[MAX_INP_SIZE][MAX_INP_SIZE];
  double E[MAX_INP_SIZE];
  double chi2;
  
  memset(&W[0][0],0,sizeof(W));

  for(i=0;i<m_size;i++) if(m_Zarray[i]==1) newSize--;

  //  printf("newSize = %d\n",newSize);

  idxI=-1;idxJ=-1;
  for(i=0;i<m_size;i++)
    {
      if(m_Zarray[i]==1) continue;
      idxI++;idxJ=idxI-1;
      for(j=i;j<m_size;j++)
	{
	  if(m_Zarray[j]==1) continue;
	  idxJ++;
	  W[idxI][idxJ]=0.0;
	  //  printf("Computing W: I=%d J=%d\n",idxI,idxJ);
	  W[idxJ][idxI]=W[idxI][idxJ]=m_W[i][j];
	}
    }
  /*
  printf("Reduced W:\n");
  for(i=0;i<newSize;i++)
    {
      for(j=0;j<newSize;j++) printf("%E ",W[i][j]);
      printf("\n");
    }
  */
  double* a=new double[newSize*newSize];
  for(i=0;i<newSize;i++) for(j=0;j<newSize;j++) a[i+j*newSize]=W[i][j];
  bool rc=this->invertMatrixNxN(a,newSize);
  if(!rc)
    {
      delete[] a;
      return NULL;
    }
  for(i=0;i<newSize;i++) for(j=0;j<newSize;j++) P[i][j]=a[i+j*newSize];
  /*
  printf("Inverted W:\n");
  for(i=0;i<newSize;i++)
    {
      for(j=0;j<newSize;j++)
	printf("%E ",P[i][j]);
      printf("\n");
    }
  */
  idxI=-1;

  double Q[MAX_INP_SIZE];
  memset(&Q[0],0,sizeof(Q));
  for(i=0;i<m_size;i++)
    {
      if(m_Zarray[i]==1) continue;
      idxI++;
      Q[idxI]=0.0;
      for(j=0;j<m_resSize;j++) Q[idxI]+=m_K[i][j]*m_R[j];
    }
  //printf("Solution:\n");
  for(i=0;i<newSize;i++)
    {
      E[i]=0.0;
      for(j=0;j<newSize;j++) E[i]+=P[i][j]*Q[j];
      //  printf("%E \n",E[i]);
    }
  chi2=0.0;
  for(i=0;i<newSize;i++)
    for(j=0;j<newSize;j++) chi2+=E[i]*W[i][j]*E[j];
  // printf("Significance: %f\n",chi2);
  pLS = new LSMSolution(m_size);
  pLS->m_Significance()=chi2;

  idxI=-1;
  for(i=0;i<m_size;i++)
    {
      if(m_Zarray[i]==1)
	{
	  (*pLS)[i]=0.0;pLS->m_fixVariable(i);
	  continue;
	}
      idxI++;
      (*pLS)[i]=E[idxI];idxJ=-1;
      for(j=0;j<m_size;j++)
	{
	  if(m_Zarray[j]==1) continue;
	  idxJ++;
	  pLS->m_Cov(i,j)=P[idxI][idxJ];
	}
    }
  delete[] a;
  return pLS;
}


double TrigInDetBremDetectionTool::m_getCriterionValue(LSMSolution* pLS)
{
  double R[MAX_RES_SIZE];
  int i,j,k;
  double crit=0.0;

  for(i=0;i<m_resSize;i++)
    {
      R[i]=m_R[i];
      for(j=0;j<m_size;j++) R[i]-=m_A[i][j]*(*pLS)[j];
    }
  i=0;
  for(std::vector<int>::iterator iIt=m_resSizes.begin();iIt!=m_resSizes.end();++iIt)
      {
	for(j=0;j<(*iIt);j++)
	  {
	    for(k=0;k<(*iIt);k++) crit+=R[i+j]*m_S[i+j][k]*R[i+k];
	  }
	i+=(*iIt);
      }
  return crit;
}

bool TrigInDetBremDetectionTool::m_checkFeasibility(LSMSolution * pS)
{
  int i;
  bool rc=true;
  for(i=0;i<m_size;i++)
    {
      if(m_Parray[i]!=1) continue;
      if((*pS)[i]*m_sign<0.0) 
	{
	  rc=false;break;
	}
    }
  return rc;
}

void TrigInDetBremDetectionTool::m_mixSolutions(LSMSolution* pST, LSMSolution* pLS)
{
  int i,iMin=-1;
  double aMin=1000000.0,a;

  for(i=0;i<m_size;i++)
    {
      if(m_Parray[i]!=1) continue;
      if(m_sign*(*pST)[i]>0.0) continue;
      a=(*pLS)[i]/((*pLS)[i]-(*pST)[i]);
      //      printf("i=%d alpha=%f\n",i,a);
      if(a<aMin)
	{
	  aMin=a;iMin=i;
	}
  }
  if(iMin<0) return;
  // printf("Closest component %d with alpha=%f\n",iMin,aMin);
  for(i=0;i<m_size;i++)
    {
      (*pLS)[i]=(1.0-aMin)*(*pLS)[i]+aMin*(*pST)[i];
    }
  m_Zarray[iMin]=1;m_Parray[iMin]=0;
}

void TrigInDetBremDetectionTool::modifySurfaces(int flag)
{
  int i;

  if(flag!=0)
    {
      if(m_pLS!=NULL)
	{
	  if(m_pLS->m_Significance()>m_SignificanceCut)
	    {
	      for(i=0;i<m_size;i++)
		{
		  if(!m_pLS->m_isOnConstraint(i))
		    {
		      m_surfArray[i]->m_setBreakPoint((*m_pLS)[i]);
		      if(m_outputLevel<=MSG::DEBUG)
			{
			  m_log<<MSG::DEBUG<<i<<"  Breakpoint is set, u="<<(*m_pLS)[i]<<
			    " X="<<m_jX[i]<<" Y="<<m_jY[i]<<" Z="<<m_jZ[i]<<endreq;
			}
		    }
		  else 
		    if(m_outputLevel<=MSG::DEBUG)
			{
			  m_log<<MSG::DEBUG<<i<<"  Not a Breakpoint X="<<m_jX[i]<<" Y="<<m_jY[i]<<
			    " Z="<<m_jZ[i]<<endreq;
			}
		}
	    }
	}
    }
  else
    {
      for(i=0;i<m_size;i++) 
	{
	  m_surfArray[i]->m_unsetBreakPoint();                                                                               
	}
    }
  delete m_pLS;m_pLS=NULL;
}


void TrigInDetBremDetectionTool::report(int flag)
{
  int i,j,k;

  if(flag==1) //Residuals
    {
      m_log << MSG::DEBUG << "IDE Residuals : "<< endreq;
      i=0;
      for(std::vector<int>::iterator iIt=m_resSizes.begin();iIt!=m_resSizes.end();++iIt)
	{
	  for(j=0;j<(*iIt);j++)
	    {
	      m_log << MSG::DEBUG << "   "<<m_R[i]<< endreq;
	      i++;
	    }
	}
    }
  if(flag==2) //LSM
  {
    i=0;
    m_log << MSG::DEBUG << "LSM Problem : nU="<<m_lsmSize-1<<" nR="<<m_resSize<< endreq;
    for(std::vector<int>::iterator iIt=m_resSizes.begin();iIt!=m_resSizes.end();++iIt)
      {
	for(j=0;j<(*iIt);j++)
	  {
	    m_log << MSG::DEBUG <<m_R[i]<<"    [";
	    for(k=0;k<(*iIt);k++) m_log << MSG::DEBUG <<m_S[i][k]<<"  ";
	    m_log << MSG::DEBUG <<"]  ";
	    for(k=0;k<m_lsmSize-1;k++)
	      m_log << MSG::DEBUG <<m_A[i][k]<<" ";
	    m_log << MSG::DEBUG << endreq;
	    i++;
	  } 
      }
    m_log << MSG::DEBUG << "LSM Gain Matrix "<< endreq;
    for(i=0;i<m_lsmSize-1;i++)
      {
	for(j=0;j<m_resSize;j++)
	  m_log << MSG::DEBUG <<m_K[i][j]<<"  ";
	m_log << MSG::DEBUG << endreq;
      }
    m_log << MSG::DEBUG << "LSM Weight Matrix "<< endreq;
    for(i=0;i<m_lsmSize-1;i++)
      {
	for(j=0;j<m_lsmSize-1;j++)
	  m_log << MSG::DEBUG <<m_W[i][j]<<"  ";
	m_log << MSG::DEBUG << endreq;
      }

    m_log << MSG::DEBUG << "-----------------------"<< endreq;
  }
}


LSMSolution::LSMSolution()
{
  m_size=0;
  int i,j; 
  m_Chi2=0.0;
  for(i=0;i<=m_size;i++) 
    {
      m_u[i]=0.0;
      m_fixedVariables[i]=0;
      for(j=0;j<=m_size;j++) m_C[i][j]=0.0;
    }
}

LSMSolution::~LSMSolution()
{

}

LSMSolution::LSMSolution(int size) : m_size(size)
{
  int i,j; 
  m_Chi2=0.0;
  for(i=0;i<=m_size;i++) 
    {
      m_u[i]=0.0;
      m_fixedVariables[i]=0;
      for(j=0;j<=m_size;j++) m_C[i][j]=0.0;
    }
}

double& LSMSolution::operator[] (int i)
{
  if((i<0)||(i>=m_size))
    return m_u[0];
  else return m_u[i];
}

double& LSMSolution::m_Cov(int i, int j)
{
  if((i<0)||(i>=m_size)) return m_C[0][0];
  if((j<0)||(j>=m_size)) return m_C[0][0];
  return m_C[i][j];
}

double& LSMSolution::m_Significance()
{
  return m_Chi2;
}

bool LSMSolution::m_isOnConstraint(int i)
{
  return (m_fixedVariables[i]==1);
}

void LSMSolution::m_fixVariable(int i)
{
  m_fixedVariables[i]=1;
}

void LSMSolution::m_report()
{
  int i,j;

  printf("LSM Solution size %d\n",m_size);
  for(i=0;i<m_size;i++)
    {
      printf("%E  ",m_u[i]);
      if(m_isOnConstraint(i)) printf("constr.  ");
      else printf("free    ");
      for(j=0;j<m_size;j++) printf("%E ",m_C[i][j]);
      printf("\n");
    }
  printf("Significance  %f\n",m_Chi2);
  
}
