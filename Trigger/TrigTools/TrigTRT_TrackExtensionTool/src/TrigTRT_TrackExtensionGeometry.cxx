/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkSurfaces/Surface.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoad.h"
#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_ProbabilisticDataAssociation.h"

TrigTRT_TrackExtensionLayer::TrigTRT_TrackExtensionLayer(int type,
							 double par[10],
							 int nStraws,
							 ToolHandle<TrigMagneticFieldTool> pMagTool,
							 const TRT_ID* pI, TrigTRT_BasePDAF* pU) :
  m_type(type), m_nStraws(nStraws), m_pMagTool(pMagTool), m_trtHelper(pI), m_pdafUpdator(pU)
{
  m_minBound=par[0];
  m_maxBound=par[1];
  m_refCoord=par[2];
  m_phi0=par[7];
  m_phiMax=par[8];
  m_dphi=par[9]; 
  for(int i=0;i<nStraws;i++)
    {
      m_vpStraws.push_back(new TrigTRT_Straw());
    }
  if(type==0)
    {
      m_xFirst=par[3];
      m_xLast=par[4];
      m_yFirst=par[5];
      m_yLast=par[6]; 
      m_phiPitch = 0.0;
    }
  else
    {
      m_phiPitch=par[3];
    }
  m_vpElements.clear();
}

TrigTRT_TrackExtensionLayer::~TrigTRT_TrackExtensionLayer()
{
  for(std::vector<TrigTRT_DetElement*>::iterator peIt=m_vpElements.begin();
      peIt!=m_vpElements.end();++peIt)
    {
      delete (*peIt);
    }
  for(std::vector<TrigTRT_Straw*>::iterator psIt=m_vpStraws.begin();
      psIt!=m_vpStraws.end();++psIt)
    {
      delete (*psIt);
    }
}

const TRT_ID* TrigTRT_TrackExtensionLayer::m_getTRT_Helper()
{
  return m_trtHelper;
}

TrigTRT_BasePDAF* TrigTRT_TrackExtensionLayer::m_getUpdator()
{
  return m_pdafUpdator;
}

void TrigTRT_TrackExtensionLayer::m_dump(FILE* pF)
{
  if(m_type==0)
    {
      fprintf(pF,"-- barrel layer ---\n");
      fprintf(pF,"zMin=%f zMax=%f R=%f\n",
	      m_minBound,m_maxBound,m_refCoord);
      fprintf(pF,"PHI: first straw %f sector width %f step %f\n",
	      m_phi0,m_phiMax,m_dphi);
      fprintf(pF,"XF=%f YF=%f XL=%f YL=%f\n",
	      m_xFirst,m_yFirst,m_xLast,m_yLast);
      fprintf(pF," Total %d del elements per layer\n",
	      (int) m_vpElements.size());
      for(std::vector<TrigTRT_DetElement*>::iterator peIt=m_vpElements.begin();
      peIt!=m_vpElements.end();++peIt)
	{
	  (*peIt)->m_dump(pF);
	}
    }
  else
    {
      fprintf(pF,"-- endcap layer ---\n");
      fprintf(pF,"rMin=%f rMax=%f Z=%f\n",
	      m_minBound,m_maxBound,m_refCoord);
      fprintf(pF,"PHI: first straw %f sector width %f step %f\n",
	      m_phi0,m_phiMax,m_dphi);
      fprintf(pF,"Straw pitch %f\n",m_phiPitch);
      
      fprintf(pF," Total %d del elements per layer\n",
	      (int) m_vpElements.size());
      for(std::vector<TrigTRT_DetElement*>::iterator peIt=m_vpElements.begin();
      peIt!=m_vpElements.end();++peIt)
	{
	  (*peIt)->m_dump(pF);
	}
    }
}

void TrigTRT_TrackExtensionLayer::m_addElement(TrigTRT_DetElement* pE)
{
  m_vpElements.push_back(pE); 
}

TrigTRT_DetElementPoint* TrigTRT_TrackExtensionLayer::m_findRoadPoint(double* pTS)
{
  const double C=0.0299997;
  double dPhi,Phi,sT,rt,Delta,DeltaPhi,sinf,cosf,Bz,PhiT,
    P[3],B[3],s,cosB,sinB,Beta;
  int nSec1,N;
  TrigTRT_DetElementPoint* pTP=NULL;
  
//  printf("In findRoadPoint, refCoord=%f %s\n",m_refCoord,(m_type==0)?"BARREL":"ENDCAP");
//  printf("x=%f y=%f z=%f phi=%f theta=%f P=%f\n",pTS[0],pTS[1],pTS[2],pTS[3],pTS[4],1.0/pTS[5]);
  
  rt=sqrt(pTS[0]*pTS[0]+pTS[1]*pTS[1]);
 // printf("track r=%f, refCoord=%f\n",rt,m_refCoord);

  if((m_type==0)&&(rt>m_refCoord)) {
    return NULL;
  }

 
  PhiT=atan2(pTS[1],pTS[0]);
  Beta=pTS[3]-PhiT;
  cosB=cos(Beta);sinB=sin(Beta);

  N=m_vpElements.size();

  for(int i=0;i<3;i++) P[i]=pTS[i];
 // printf("Getting field at... %f %f %f\n",P[0],P[1],P[2]);
  m_pMagTool->m_getMagneticField(P,&B[0]);
  Bz=B[2];
 // printf("Field is %f\n",Bz);

  sinf=sin(pTS[3]);cosf=cos(pTS[3]);
  if(m_type==0)
    {
      sT=(m_refCoord/rt-1.0)*(pTS[0]*cosf-pTS[1]*sinf);
      s=m_refCoord/rt;
      //  double sqA=s*s-sinB*sinB;
   //   printf("sqA=%f\n",sqA);
      sT=rt*(sqrt(s*s-sinB*sinB)-cosB);
   //   printf("sT=%f\n",sT);
      DeltaPhi=-pTS[5]*sT*Bz*C*cos(pTS[4])/sin(pTS[4]);
      Delta=0.5*DeltaPhi*sT*cos(pTS[4]);
      s=sqrt(sT*sT+Delta*Delta);
      pTS[2]+=s*cos(pTS[4])/sin(pTS[4]);
      
      pTS[0]+=sT*cosf-Delta*sinf;
      pTS[1]+=sT*sinf+Delta*cosf;
      pTS[3]+=DeltaPhi;
      
    //  printf("BR: new coords x=%f y=%f z=%f phi=%f Bz=%f\n",pTS[0],pTS[1],pTS[2],pTS[3],Bz);
    //  printf("Rad=%f Bounds %f %f\n",m_refCoord,m_minBound,m_maxBound);
      
      if((pTS[2]>m_minBound)&&(pTS[2]<m_maxBound))
	{
	  Phi=atan2(pTS[1],pTS[0]);
	  if(Phi<0.0) Phi+=2.0*M_PI;
	  dPhi=Phi-m_phi0;
	  if(dPhi>2.0*M_PI) dPhi-=2.0*M_PI;
	  nSec1=(int)(dPhi/m_dphi);
//	    printf("BR: Phi=%f dPhi=%f N=%d nSec1=%d\n",Phi,dPhi,N,nSec1);
	  if(nSec1<N)
	    {
	      for(int i=0;i<3;i++) P[i]=pTS[i];
	      pTP=new TrigTRT_DetElementPoint(P,m_type,this);
	      pTP->m_addDetectorElement(m_vpElements[nSec1]);
	    }
	}
    }
  else
    {
      sT=(m_refCoord-pTS[2])*sin(pTS[4])/cos(pTS[4]);
      pTS[2]=m_refCoord;
      DeltaPhi=-pTS[5]*sT*Bz*C/sin(pTS[4]);
      Delta=0.5*DeltaPhi*sT;
      pTS[0]+=sT*cosf-Delta*sinf;
      pTS[1]+=sT*sinf+Delta*cosf;
      pTS[3]+=DeltaPhi;

//      printf("EC: new coords %f %f %f %f\n",pTS[0],pTS[1],pTS[2],pTS[3]);

      rt=sqrt(pTS[0]*pTS[0]+pTS[1]*pTS[1]);

      if((rt>m_minBound)&&(rt<m_maxBound))
	{
	  Phi=atan2(pTS[1],pTS[0]);
	  if(Phi<0.0) Phi+=2.0*M_PI;
	  dPhi=Phi-m_phi0;
	  nSec1=(int)(dPhi/m_dphi);
//	    printf("EC: Phi=%f dPhi=%f N=%d nSec1=%d\n",Phi,dPhi,N,nSec1);
	  if(nSec1<N)
	    {
	      for(int i=0;i<3;i++) P[i]=pTS[i];
	      pTP=new TrigTRT_DetElementPoint(P,m_type,this);
	      pTP->m_addDetectorElement(m_vpElements[nSec1]);
	    }
	}
    }
  return pTP;
}

Trk::TrkPlanarSurface* TrigTRT_TrackExtensionLayer::m_createSurface(int secId)
{
  const double radLength=0.006;

  Trk::TrkPlanarSurface* pS=NULL;
  double N[3],C[3],M[3][3];
  Amg::Vector3D mx,my,mz;

  if(m_type==0)
    {
      const InDetDD::TRT_BarrelElement* pBE=dynamic_cast<const InDetDD::TRT_BarrelElement*>
	(m_vpElements[secId]->m_getDetectorElement());

      if(pBE==NULL) return NULL;

      const Trk::Surface& rSurf=pBE->surface();
      N[0]=rSurf.normal().x();
      N[1]=rSurf.normal().y();
      N[2]=rSurf.normal().z();
      C[0]=rSurf.center().x();
      C[1]=rSurf.center().y();
      C[2]=rSurf.center().z();
			//x, y and z rows of transformation matrix
      mx=Amg::Vector3D(rSurf.transform().rotation()(0,0), rSurf.transform().rotation()(0,1), rSurf.transform().rotation()(0,2));
      my=Amg::Vector3D(rSurf.transform().rotation()(1,0), rSurf.transform().rotation()(1,1), rSurf.transform().rotation()(1,2));
      mz=Amg::Vector3D(rSurf.transform().rotation()(2,0), rSurf.transform().rotation()(2,1), rSurf.transform().rotation()(2,2));
      for(int i=0;i<3;i++) 
	{
	  M[0][i]=mx[i];M[1][i]=my[i];M[2][i]=mz[i];
	}
    }
  else
    {
      const InDetDD::TRT_EndcapElement* pEE=dynamic_cast<const InDetDD::TRT_EndcapElement*>
	(m_vpElements[secId]->m_getDetectorElement());

      if(pEE==NULL) return NULL;

      int firstStraw = 0;
      int lastStraw = pEE->nStraws() - 1;

      const Amg::Vector3D & centerFirstStraw = pEE->strawCenter(firstStraw);
      const Amg::Vector3D & centerLastStraw = pEE->strawCenter(lastStraw);
      const Trk::Surface& rSurf=pEE->surface();
      C[0]=rSurf.center().x();
      C[1]=rSurf.center().y();
      C[2]=rSurf.center().z();

      Amg::Vector3D Ay = centerLastStraw - centerFirstStraw;
      Ay = Ay.unit();      

      Amg::Vector3D Ax(-C[0]+0.5*(centerFirstStraw.x()+centerLastStraw.x()),
		     -C[1]+0.5*(centerFirstStraw.y()+centerLastStraw.y()),
		     -C[2]+0.5*(centerFirstStraw.z()+centerLastStraw.z()));

      Ax = Ax.unit();
      Amg::Vector3D Az = Ax.cross(Ay);
      Az = Az.unit();
      
      N[0]=Az.x();
      N[1]=Az.y();
      N[2]=Az.z();
      M[0][0]=Ax.x();M[0][1]=Ay.x();M[0][2]=Az.x();
      M[1][0]=Ax.y();M[1][1]=Ay.y();M[1][2]=Az.y();
      M[2][0]=Ax.z();M[2][1]=Ay.z();M[2][2]=Az.z();
    }
  pS=new Trk::TrkPlanarSurface(C,N,M,radLength);
  return pS;
}


std::vector<TrigTRT_Straw*>* TrigTRT_TrackExtensionLayer::m_getVectorOfStraws(int secId,
									      const double C[3],
									      Trk::TrkTrackState* pTS)
{
  unsigned int i;
  double x[2],y[2],dx,dy,L0,dL,L,locPos;

  if(m_type==0)
    {
      const InDetDD::TRT_BarrelElement* pBE=dynamic_cast<const InDetDD::TRT_BarrelElement*>
	(m_vpElements[secId]->m_getDetectorElement());

      if(pBE==NULL) return NULL;

      x[0]=pBE->strawCenter(0).x();y[0]=pBE->strawCenter(0).y();
      x[1]=pBE->strawCenter(pBE->nStraws()-1).x();y[1]=pBE->strawCenter(pBE->nStraws()-1).y();
      
      dx=(x[1]-x[0]);dy=(y[1]-y[0]);
      L=sqrt(dx*dx+dy*dy);
      dL=L/(m_nStraws-1);
      L0=((C[0]-x[0])*dx+(C[1]-y[0])*dy)/L;
      for(i=0;i<pBE->nStraws();i++)
	{
	  m_vpStraws[i]->m_setLocalCoordinate(-L0+dL*i);
	  m_vpStraws[i]->m_setDC(NULL);
	}
    }
  else
    {
      const InDetDD::TRT_EndcapElement* pEE=dynamic_cast<const InDetDD::TRT_EndcapElement*>
	(m_vpElements[secId]->m_getDetectorElement());

      if(pEE==NULL) return NULL;

      double phiPitch=pEE->getDescriptor()->strawPitch();
      for(i=0;i<pEE->nStraws();i++)
	{
	  double deltaPhi=phiPitch*i-0.5*phiPitch*(pEE->nStraws()-1);
	  //double deltaPhi=phiPitch*i-0.5*phiPitch*(pEE->nStraws());
	  locPos=pTS->m_getTrackState(0)*sin(deltaPhi)/cos(deltaPhi);
	  m_vpStraws[i]->m_setLocalCoordinate(locPos);
	  m_vpStraws[i]->m_setDC(NULL);
	  /*
	  printf("Straw %d x=%f y=%f z=%f\n",i,pEE->strawCenter(i).x(),
		 pEE->strawCenter(i).y(),pEE->strawCenter(i).z());
	  */
	}
    }
  return &m_vpStraws;
}

bool TrigTRT_TrackExtensionLayer::m_isInAcceptance(Trk::TrkTrackState* pTS)
{
  double width;
  if(m_type==0)
    {
      width=0.5*(m_maxBound-m_minBound);
      return (fabs(pTS->m_getTrackState(1))<width);
    }
  else
    {
      return ((pTS->m_getTrackState(0)>m_minBound)&&(pTS->m_getTrackState(0)<m_maxBound));
    }
}


TrigTRT_TrackExtensionRegion::TrigTRT_TrackExtensionRegion(int t):
  m_type(t), m_minZ(0.0), m_maxZ(0.0), m_minR(0.0), m_maxR(0.0)
{
  m_vpLayers.clear();
}

TrigTRT_TrackExtensionRegion::~TrigTRT_TrackExtensionRegion()
{
  for(std::vector<TrigTRT_TrackExtensionLayer*>::iterator plIt=m_vpLayers.begin();
      plIt!=m_vpLayers.end();++plIt)
    {
      delete(*plIt);
    }
  m_vpLayers.clear();
}

void TrigTRT_TrackExtensionRegion::m_addLayer(TrigTRT_TrackExtensionLayer* pL)
{
  m_vpLayers.push_back(pL);
}

void TrigTRT_TrackExtensionRegion::m_setType(int t)
{
  m_type=t;
}

void TrigTRT_TrackExtensionRegion::m_dump(FILE* pF)
{
  fprintf(pF,"%s region\n",(m_type==0)?"BARREL":"ENDCAP");
  fprintf(pF,"minZ=%f maxZ=%f minR=%f maxR=%f\n",m_minZ,m_maxZ,m_minR,m_maxR);
  for(std::vector<TrigTRT_TrackExtensionLayer*>::iterator plIt=m_vpLayers.begin();
      plIt!=m_vpLayers.end();++plIt)
    {
      (*plIt)->m_dump(pF);
    }
}

void TrigTRT_TrackExtensionRegion::m_report()
{
  printf("%s region\n",(m_type==0)?"BARREL":"ENDCAP");
  printf("minZ=%f maxZ=%f minR=%f maxR=%f\n",m_minZ,m_maxZ,m_minR,m_maxR);
}


void TrigTRT_TrackExtensionRegion::m_setBoundaries(double minZ, double maxZ, 
						   double minR, double maxR)
{
  m_minZ=minZ;m_maxZ=maxZ;m_minR=minR;m_maxR=maxR;
}

bool TrigTRT_TrackExtensionRegion::m_isInAcceptance(double P[6])
{
  double rt,zt,zc[2],rc[2],sint,cost;
  int sig=0;
  int i,j;

  rt=sqrt(P[0]*P[0]+P[1]*P[1]);
  zt=P[2];
  sint=sin(P[4]);
  cost=cos(P[4]);
  zc[0]=m_minZ-zt;
  zc[1]=m_maxZ-zt;
  rc[0]=m_minR-rt;
  rc[1]=m_maxR-rt;

  for(i=0;i<2;i++)
    for(j=0;j<2;j++)
      {
	sig+=(rc[i]*cost>zc[j]*sint)?1:-1;
      }
  return (abs(sig)!=4);
}

void TrigTRT_TrackExtensionRegion::m_collectDetElements(TrigTRT_DetElementRoad* pR,double P[6])
{
  for(std::vector<TrigTRT_TrackExtensionLayer*>::iterator plIt=m_vpLayers.begin();
      plIt!=m_vpLayers.end();++plIt)
    {
      TrigTRT_DetElementPoint* pRP=(*plIt)->m_findRoadPoint(&P[0]);
      if(pRP!=NULL) pR->m_addPoint(pRP);
    }
}


TrigTRT_TrackExtensionGeometry::TrigTRT_TrackExtensionGeometry(const InDetDD::TRT_DetectorManager* pMgr,  
							       ToolHandle<TrigMagneticFieldTool> pMagTool, 
							       TrigTRT_BasePDAF* pBarrelPDAF, 
							       TrigTRT_BasePDAF* pEndcapPDAF, const TRT_ID* trtHelper) : 
  m_barrelUpdator(pBarrelPDAF), m_endcapUpdator(pEndcapPDAF)
{
  int iSide,iLayer,nBarrLays,RegId,iWheel,nEcLays,iPhiModule,iStraw;
  TrigTRT_TrackExtensionRegion* pReg;

  for(RegId=0;RegId<4;RegId++)
    {
      m_pRegions[RegId]=new TrigTRT_TrackExtensionRegion(0);
      pReg=m_pRegions[RegId];
      if(RegId<2)
	{
	  pReg->m_setType(0);
	  iSide=RegId;
	  double minR=100000.0;
	  double maxR=-100000.0;
	  double minZ=1000000.0;
	  double maxZ=-1000000.0;
	  for(iLayer=0;iLayer<3;iLayer++) 
	    {
	      nBarrLays=pMgr->getNumerology()->getNBarrelLayers(iLayer);
	      for(iStraw=0;iStraw<nBarrLays;iStraw++) 
		{
		  const InDetDD::TRT_BarrelElement* pBE0=pMgr->getBarrelElement(iSide,iLayer,0,iStraw);
		  const InDetDD::TRT_BarrelElement* pBE1=pMgr->getBarrelElement(iSide,iLayer,1,iStraw);

		  double xf1,xf2,yf1,yf2,z,param[10],xl1,yl1,xm,ym;
		  int nStraws=pBE0->nStraws();
		  double strawLen=pBE0->strawLength();
		  Amg::Vector3D center=pBE0->transform()*Amg::Vector3D(0.0,0.0,0.0);
		  z=center.z();
		  param[0]=z-0.5*strawLen;
		  param[1]=z+0.5*strawLen;

		  if(minZ>param[0]) minZ=param[0];
		  if(maxZ<param[1]) maxZ=param[1];

		  xf1=pBE0->strawCenter(0).x();yf1=pBE0->strawCenter(0).y();
		  xf2=pBE1->strawCenter(0).x();yf2=pBE1->strawCenter(0).y();
		  xl1=pBE0->strawCenter(nStraws-1).x();
		  yl1=pBE0->strawCenter(nStraws-1).y();
		  
		  xm=0.5*(xf1+xl1);ym=0.5*(yf1+yl1);
		  param[2]=sqrt(xm*xm+ym*ym);

		  if(param[2]>maxR) maxR=param[2];
		  if(param[2]<minR) minR=param[2];

		  param[3]=xf1;param[4]=xl1;param[5]=yf1;param[6]=yl1;
		  
		  param[7]=atan2(yf1,xf1);
		  param[9]=atan2(yf2,xf2)-param[7];
		  param[8]=atan2(yl1,xl1)-param[7];
		  
		  TrigTRT_TrackExtensionLayer* pL=new TrigTRT_TrackExtensionLayer(0,param,nStraws,
										  pMagTool,trtHelper,
										  pBarrelPDAF);
		  int nPhiMod=pMgr->getNumerology()->getNBarrelPhi();
		  for(iPhiModule=0;iPhiModule<nPhiMod;iPhiModule++) 
		    {
		      const InDetDD::TRT_BarrelElement*
			pBE=pMgr->getBarrelElement(iSide,iLayer,iPhiModule,iStraw);
		      IdentifierHash hashId=pBE->identifyHash();

		      TrigTRT_DetElement* pTDE=new TrigTRT_DetElement(hashId,iPhiModule,pBE);
		      pL->m_addElement(pTDE);
		    }
		  pReg->m_addLayer(pL);
		}
	      pReg->m_setBoundaries(minZ,maxZ,minR,maxR);
	    }
	}
      else
	{
	  pReg->m_setType(1);
	  iSide=(RegId==2)?0:1;
	  int nWheels=pMgr->getNumerology()->getNEndcapWheels();
	  double minR=100000.0;
	  double maxR=-100000.0;
	  double minZ=100000.0;
	  double maxZ=-100000.0;
	  for(iWheel=0;iWheel<nWheels;iWheel++) 
	    {
	      nEcLays=pMgr->getNumerology()->getNEndcapLayers(iWheel);
	      for(iStraw=0;iStraw<nEcLays;iStraw++)
		{ 
		  const InDetDD::TRT_EndcapElement* pEE0=pMgr->getEndcapElement(iSide,iWheel,iStraw,0);
		  const InDetDD::TRT_EndcapElement* pEE1=pMgr->getEndcapElement(iSide,iWheel,iStraw,1);

		  double param[10];
		  int nStraws=pEE0->nStraws();
		  double strawLen=pEE0->getDescriptor()->strawLength();
		  Amg::Vector3D center=pEE0->transform()*Amg::Vector3D(0.0,0.0,0.0);
		  param[2]=center.z();

		  if(minZ>param[2]) minZ=param[2];
		  if(maxZ<param[2]) maxZ=param[2];

		  param[0]=pEE0->getDescriptor()->innerRadius();
		  param[1]=param[0]+strawLen;

		  if(param[0]<minR) minR=param[0];
		  if(param[1]>maxR) maxR=param[1];

		  param[3]=pEE0->getDescriptor()->strawPitch();
		  param[7]=pEE0->getDescriptor()->startPhi();
		  param[8]=nStraws*param[3];
		  param[9]=pEE1->getDescriptor()->startPhi()-param[7];
		  
		  TrigTRT_TrackExtensionLayer* pL=new TrigTRT_TrackExtensionLayer(1,param,nStraws,
										  pMagTool,trtHelper,
										  pEndcapPDAF);
		  int nPhiMod=pMgr->getNumerology()->getNEndcapPhi();
		  for(iPhiModule=0;iPhiModule<nPhiMod;iPhiModule++) 
		    {
		      const InDetDD::TRT_EndcapElement*
			pEE=pMgr->getEndcapElement(iSide,iWheel,iStraw,iPhiModule);
		      IdentifierHash hashId=pEE->identifyHash();

		      TrigTRT_DetElement* pTDE=new TrigTRT_DetElement(hashId,iPhiModule,pEE);
		      pL->m_addElement(pTDE);
		    }
		  pReg->m_addLayer(pL);
		}
	      pReg->m_setBoundaries(minZ,maxZ,minR,maxR);
	    }
	}
    }
}

TrigTRT_TrackExtensionGeometry::~TrigTRT_TrackExtensionGeometry()
{
  int i;
  for(i=0;i<4;i++) delete m_pRegions[i];
  delete m_barrelUpdator;
  delete m_endcapUpdator;
}

TrigTRT_DetElementRoad* TrigTRT_TrackExtensionGeometry::m_buildTRT_Road(double P[6])
{
  TrigTRT_DetElementRoad* pR=NULL;
  int i;
  /*  
  printf("Road builder: IniParams X=%f Y=%f Z=%f phi=%f theta=%f pT=%f\n",
	 P[0],P[1],P[2],P[3],P[4],sin(P[4])/P[5]);
  */
  pR=new TrigTRT_DetElementRoad();

  for(i=0;i<4;i++)
    {
      if(m_pRegions[i]->m_isInAcceptance(P))
	{
	  //  m_pRegions[i]->m_report();
	  //printf("Region %d - OK\n",i);
	  m_pRegions[i]->m_collectDetElements(pR,P);
	}
      //else printf("Region %d - not in acceptance\n",i);
    }
  if(pR->m_roadPoints()->size()==0) 
    {
      delete pR;
      pR=NULL;
    }
  return pR;
}


TrigTRT_DetElement::TrigTRT_DetElement(IdentifierHash id, int sec, const InDetDD::TRT_BaseElement* pE):
  m_hashId(id),m_sectorId(sec), m_pDE(pE)
{
  m_pDC=NULL;
}

const InDetDD::TRT_BaseElement* TrigTRT_DetElement::m_getDetectorElement()
{
  return m_pDE;
}

void TrigTRT_DetElement::m_addDC_Collection(const InDet::TRT_DriftCircleCollection* pDC)
{
  m_pDC=pDC;
}

const InDet::TRT_DriftCircleCollection* TrigTRT_DetElement::m_getDC_Collection()
{
  return m_pDC;
}

IdentifierHash TrigTRT_DetElement::m_getHashId()
{
  return m_hashId;
}

int TrigTRT_DetElement::m_getSectorId()
{
  return m_sectorId;
}

void TrigTRT_DetElement::m_dump(FILE* pF)
{
  fprintf(pF,"DE %d S%d\n",(int)(m_getHashId()),m_sectorId);
}

void TrigTRT_DetElement::m_report()
{
  printf("DE %d PhiSector=%d\n",(int)(m_getHashId()),m_sectorId);
  if(m_getDC_Collection()!=NULL)
    {
      printf("  with %d TRT hits\n",(int) m_getDC_Collection()->size());
    }
}


TrigTRT_DetElementPoint::TrigTRT_DetElementPoint(double C[3],int t, TrigTRT_TrackExtensionLayer* pL): 
  m_type(t), m_pTRT_Layer(pL)
{
  m_distance=0.0;
  for(int i=0;i<3;i++) 
    {
      m_Coordinates[i]=C[i];
      m_distance+=C[i]*C[i];
    }
  m_distance=sqrt(m_distance);
  m_pElement=NULL;
}

int TrigTRT_DetElementPoint::m_getType()
{
  return m_type;
}

void TrigTRT_DetElementPoint::m_addDetectorElement(TrigTRT_DetElement* pDE)
{
  m_pElement=pDE;
  pDE->m_addDC_Collection(NULL);
}

TrigTRT_DetElement* TrigTRT_DetElementPoint::m_detectorElement()
{
  return m_pElement;
}

void TrigTRT_DetElementPoint::m_report()
{
  printf("at X=%f Y=%f Z=%f\n",m_Coordinates[0],m_Coordinates[1],m_Coordinates[2]);
  m_detectorElement()->m_report();
}

bool TrigTRT_DetElementPoint::m_hasHits()
{
  return (m_detectorElement()->m_getDC_Collection()!=NULL);
}

Trk::TrkPlanarSurface*  TrigTRT_DetElementPoint::m_createSurface()
{
  int secId=m_detectorElement()->m_getSectorId();
  
  return (m_pTRT_Layer->m_createSurface(secId));
}

void TrigTRT_DetElementPoint::m_updateTrackState(Trk::TrkTrackState* pTS, TrigTRT_Info* pTI, Trk::TrkTrackState* pUS)
{
  std::vector<TrigTRT_Straw*> VS;
  int nHits=0;
  //  pTS->m_report();

  if(!m_pTRT_Layer->m_isInAcceptance(pTS)) return;

  const InDet::TRT_DriftCircleCollection* pColl=m_detectorElement()->m_getDC_Collection();
      
  // printf("pColl=%x sector=%d\n",pColl,(*deIt)->m_getSectorId());
      
  std::vector<TrigTRT_Straw*>* pVS=m_pTRT_Layer->m_getVectorOfStraws(m_detectorElement()->m_getSectorId(),
								     pTS->m_getSurface()->m_getCenter(),pTS);
  if(pColl!=NULL)
    {
      for(DataVector<InDet::TRT_DriftCircle>::const_iterator dcIt=pColl->begin();
	  dcIt!=pColl->end();dcIt++) 
	{
	  const InDet::TRT_DriftCircle* pDC=(*dcIt);
	  Identifier dcID=pDC->identify();
	  int nStraw=m_pTRT_Layer->m_getTRT_Helper()->straw(dcID);
	  (*pVS)[nStraw]->m_setDC(pDC);
	}
      for(std::vector<TrigTRT_Straw*>::iterator psIt=pVS->begin();psIt!=pVS->end();++psIt)
	{
	  if(m_pTRT_Layer->m_getUpdator()->m_validateTRT_Hit(pTS,(*psIt))) nHits++;
	}
    }
  if(nHits!=0) m_pTRT_Layer->m_getUpdator()->m_update(pTS,pTI,pUS);
  else m_pTRT_Layer->m_getUpdator()->m_clear();
}



TrigTRT_DetElementRoad::TrigTRT_DetElementRoad()
{
  m_vpPoints.clear();
}

TrigTRT_DetElementRoad::~TrigTRT_DetElementRoad()
{
  for(std::vector<TrigTRT_DetElementPoint*>::iterator ppIt=m_vpPoints.begin();
      ppIt!=m_vpPoints.end();++ppIt)
    {
      delete (*ppIt);
    }
  m_vpPoints.clear();
}

void TrigTRT_DetElementRoad::m_addPoint(TrigTRT_DetElementPoint* pDP)
{
  m_vpPoints.push_back(pDP);
}

std::vector<TrigTRT_DetElementPoint*>* TrigTRT_DetElementRoad::m_roadPoints()
{
  return (&m_vpPoints);
}

void TrigTRT_DetElementRoad::m_collectDetectorElements(std::list<TrigTRT_DetElement*>* pL)
{
  for(std::vector<TrigTRT_DetElementPoint*>::iterator ppIt=m_vpPoints.begin();
      ppIt!=m_vpPoints.end();++ppIt)
    {
      pL->push_back((*ppIt)->m_detectorElement());
    }
}

void TrigTRT_DetElementRoad::m_report()
{
  printf("--- TRT track road contains %d points ----\n",
	 (int) m_vpPoints.size());
  for(std::vector<TrigTRT_DetElementPoint*>::iterator ppIt=m_vpPoints.begin();
      ppIt!=m_vpPoints.end();++ppIt)
    {
      (*ppIt)->m_report();
    }
}

void TrigTRT_TrackExtensionGeometry::m_dump(char fileName[])
{
  FILE* pF;
  int RegId;

  pF=fopen(fileName,"w");

  for(RegId=0;RegId<4;RegId++)
    {
      fprintf(pF,"------ Region %d -------\n",RegId);
      m_pRegions[RegId]->m_dump(pF);
    }
  fclose(pF);
}

TrigTRT_Straw::TrigTRT_Straw()
{
  m_loc=0.0;m_pDC=NULL;
}

TrigTRT_Straw::TrigTRT_Straw(TrigTRT_Straw* pS)
{
  m_setLocalCoordinate(pS->m_localCoordinate());
  m_setDC(pS->m_getDC());
}

void TrigTRT_Straw::m_setLocalCoordinate(double l)
{
  m_loc=l;
}

void TrigTRT_Straw::m_setDC(const InDet::TRT_DriftCircle* pDC)
{
  m_pDC=pDC;
}

double TrigTRT_Straw::m_localCoordinate()
{
  return m_loc;
}

const InDet::TRT_DriftCircle* TrigTRT_Straw::m_getDC()
{
  return m_pDC;
}

TrigTRT_Info::TrigTRT_Info()
{
  m_trHitsDT=0.0;
  m_trtHitsDT=0.0;
  m_trHitsNoDT=0.0;
  m_trtHitsNoDT=0.0;
  m_nCrossedLayers=0;
  m_crossedStraws=0.0;
  m_detectionSum=0.0;
  m_driftCircles.clear();
}

void TrigTRT_Info::m_addTR_HitWeight(double w, bool dt)
{
  if(dt) m_trHitsDT+=w;
  else m_trHitsNoDT+=w;
}

void TrigTRT_Info::m_addTRT_HitWeight(double w, bool dt)
{
  if(dt) m_trtHitsDT+=w;
  else m_trtHitsNoDT+=w;
}

double TrigTRT_Info::m_getTR_DriftTime_Sum()
{
  return m_trHitsDT;
}

double TrigTRT_Info::m_getTRT_DriftTime_Sum()
{
  return m_trtHitsDT;
}

double TrigTRT_Info::m_getTR_NoDriftTime_Sum()
{
  return m_trHitsNoDT;
}

double TrigTRT_Info::m_getTRT_NoDriftTime_Sum()
{
  return m_trtHitsNoDT;
}

int TrigTRT_Info::m_getCrossedLayers()
{
  return m_nCrossedLayers;
}

double TrigTRT_Info::m_getDetectionWeight()
{
  return m_detectionSum;
}

double TrigTRT_Info::m_getCrossedStraws()
{
  return m_crossedStraws;
}

void TrigTRT_Info::m_addCrossedStraw(double w)
{
  m_crossedStraws+=w;
}

void TrigTRT_Info::m_addCrossedLayer()
{
  m_nCrossedLayers++;
}

void TrigTRT_Info::m_addDetectionWeight(double w)
{
  m_detectionSum+=w;
}
void TrigTRT_Info::m_addTRT_Hit(const InDet::TRT_DriftCircle* pH)
{
  m_driftCircles.push_back(pH);
}

std::vector<const InDet::TRT_DriftCircle*>& TrigTRT_Info::m_getTRT_Hits()
{
  return m_driftCircles;
}


