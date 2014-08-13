/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class SiSpacePointsSeedMaker_ATLxk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsSeedMaker_CTBxk.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsComparison.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_CTBxk::SiSpacePointsSeedMaker_CTBxk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n) 
{
  m_usefield = false                             ;
  m_state    = 0                                 ;
  m_pixel    = true                              ;
  m_sct      = true                              ;
  m_nspoint  = 3                                 ;
  m_mode     = 0                                 ;
  m_ptmin    = 500.                              ;
  m_r1min    = 0.                                ;
  m_r1max    = 600.                              ;
  m_r2min    = 0.                                ;
  m_r2max    = 600.                              ;
  m_r3min    = 0.                                ;
  m_r3max    = 600.                              ;
  m_drmin    = 10.                               ;
  m_drmax    = 250.                              ;
  m_dzdrmin  =-5.0                               ;
  m_dzdrmax  = 5.0                               ;
  m_daymax   = .02                               ;
  m_dazmax   = .02                               ;
  m_dzver    = 5.                                ;
  m_dzdrver  = .02                               ;
  i_seed     = l_seeds.begin()                   ;
  i_seede    = l_seeds.end  ()                   ;
  m_spacepointsPixelname   = "PixelSpacePoints"  ;
  m_spacepointsOverlapname = "OverlapSpacePoints"; 
  m_spacepointsSCTname     = "SCT_SpacePoints"   ;
  m_spacepointsSCT         = 0                   ;
  m_spacepointsPixel       = 0                   ;
  m_spacepointsOverlap     = 0                   ;

  declareInterface<ISiSpacePointsSeedMaker>(this);

  declareProperty("usePixel"              ,m_pixel                 );
  declareProperty("useSCT"                ,m_sct                   );
  declareProperty("useField"              ,m_usefield              );
  declareProperty("pTmin"                 ,m_ptmin                 );
  declareProperty("minRad1"               ,m_r1min                 );
  declareProperty("minRad2"               ,m_r2min                 );
  declareProperty("minRad3"               ,m_r3min                 );
  declareProperty("maxRad1"               ,m_r1max                 );
  declareProperty("maxRad2"               ,m_r2max                 );
  declareProperty("maxRad3"               ,m_r3max                 );
  declareProperty("dRmin"                 ,m_drmin                 );
  declareProperty("dRmax"                 ,m_drmax                 );
  declareProperty("dZdRmin"               ,m_dzdrmin               );
  declareProperty("dZdRmax"               ,m_dzdrmax               );
  declareProperty("dZdRdifmax"            ,m_dazmax                );
  declareProperty("dYdRdifmax"            ,m_daymax                );
  declareProperty("dZvertex"              ,m_dzver                 );
  declareProperty("dZdRvertex"            ,m_dzdrver               );
  declareProperty("SpacePointsSCTName"    ,m_spacepointsSCTname    );
  declareProperty("SpacePointsPixelName"  ,m_spacepointsPixelname  );
  declareProperty("SpacePointsOverlapName",m_spacepointsOverlapname);
  declareProperty("MagFieldSvc"           , m_fieldServiceHandle   );
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiSpacePointsSeedMaker_CTBxk::~SiSpacePointsSeedMaker_CTBxk()
{
  for(i_seed=l_seeds.begin(); i_seed!=l_seeds.end (); ++i_seed) {
    delete *i_seed;
  }
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_CTBxk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endreq; return sc;
  }
  m_fieldService = 0;
  if(m_usefield) {

    // Get magnetic field service
    //
    if( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }    
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
    m_fieldService = &*m_fieldServiceHandle;
  }
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) msg(MSG::DEBUG)<<(*this)<<endreq;

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiSpacePointsSeedMaker_CTBxk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::newEvent (int)
{
  if(!m_pixel && !m_sct) return;
  m_state = 0;
  x_sorted.erase(x_sorted.begin(),x_sorted.end());

  // Get pixels space points containers from store gate 
  //
  if(m_pixel) {

    m_spacepointsPixel = 0;
    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);

    if(m_spacepointsPixel) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsPixel->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
	for(; sp != spe; ++sp) x_sorted.push_back(*sp);
      }
    }
  }

  if(m_sct) {

    m_spacepointsSCT = 0;
    StatusCode sc = evtStore()->retrieve(m_spacepointsSCT,m_spacepointsSCTname);

    if(m_spacepointsSCT) {

      SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      for(; spc != spce; ++spc) {

	SpacePointCollection::const_iterator sp  = (*spc)->begin();
	SpacePointCollection::const_iterator spe = (*spc)->end  ();
    	for(; sp != spe; ++sp) x_sorted.push_back(*sp);
      }
    }

    // Get sct overlap space points containers from store gate 
    //
    m_spacepointsOverlap = 0;
    sc = evtStore()->retrieve(m_spacepointsOverlap,m_spacepointsOverlapname);

    if(m_spacepointsOverlap) {

      SpacePointOverlapCollection::const_iterator sp  = m_spacepointsOverlap->begin();
      SpacePointOverlapCollection::const_iterator spe = m_spacepointsOverlap->end  ();

      for(; sp!=spe; ++sp) x_sorted.push_back(*sp);
    }
  }
  sortRadius();
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  if(!m_pixel && !m_sct) return;
  m_state = 1;
  x_sorted.erase(x_sorted.begin(),x_sorted.end());

  // Get pixels space points containers from store gate 
  //
  if(m_pixel && vPixel.size()) {

    m_spacepointsPixel   = 0;
    StatusCode sc = evtStore()->retrieve(m_spacepointsPixel,m_spacepointsPixelname);
    
    if(m_spacepointsPixel) {

      SpacePointContainer::const_iterator spce =  m_spacepointsPixel->end  ();

      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	SpacePointContainer::const_iterator  w =  m_spacepointsPixel->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	if(sp!=spe) for(; sp != spe; ++sp) x_sorted.push_back(*sp);
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if(m_sct && vSCT.size()) {

    m_spacepointsSCT = 0;
    StatusCode sc = evtStore()->retrieve(m_spacepointsSCT,m_spacepointsSCTname);
    
    if(m_spacepointsSCT) {

      SpacePointContainer::const_iterator spce =  m_spacepointsSCT->end  ();

      std::vector<IdentifierHash>::const_iterator l = vSCT.begin(), le = vSCT.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	SpacePointContainer::const_iterator  w =  m_spacepointsSCT->indexFind((*l));
	if(w==spce) continue;
	SpacePointCollection::const_iterator sp = (*w)->begin(), spe = (*w)->end();

	if(sp==spe) for(; sp != spe; ++sp) x_sorted.push_back(*sp);
      }
    }
  }
  std::sort(x_sorted.begin(),x_sorted.end(),InDet::SiSpacePointsComparison_R());
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,const IRoiDescriptor&) 
{
  newRegion(vPixel,vSCT);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::find2Sp (const std::list<Trk::Vertex>& lv) 
{
  int mode; lv.begin()!=lv.end() ?  mode = 1 : mode=0; findSp(mode,lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with three space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::find3Sp(const std::list<Trk::Vertex>& lv) 
{
  int mode; lv.begin()!=lv.end() ?  mode = 3 : mode = 2; findSp(mode,lv);
}
void InDet::SiSpacePointsSeedMaker_CTBxk::find3Sp(const std::list<Trk::Vertex>& lv,const double*) 
{
  find3Sp(lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with variable number space points with or without vertex constraint
// Variable means (2,3,4,....) any number space points
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::findVSp (const std::list<Trk::Vertex>& lv)
{
  find3Sp(lv);
}

///////////////////////////////////////////////////////////////////
// Methods to initilize different strategies of seeds production
// with any number space points with or without vertex constrain
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::findSp
(int mode, const std::list<Trk::Vertex>& lv) 
{
  bool newv = newVertices(lv);
  if(newv || !m_state || m_mode!=mode) {
    
    for(i_seed=l_seeds.begin(); i_seed!=l_seeds.end(); ++i_seed) {
      delete (*i_seed);
    } 
    l_seeds.erase(l_seeds.begin(),l_seeds.end());

    m_state   = 1   ;
    m_mode    = mode;
    if     (mode==0 || mode==1) {m_nspoint = 2; production2 ();}
    else if(mode==2 || mode==3) {m_nspoint = 3; production3 ();}
  }
  i_seed  = l_seeds.begin();
  i_seede = l_seeds.end  ();
}

///////////////////////////////////////////////////////////////////
// Production 2 space points seeds with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::production2()
{ 
  int ns = x_sorted.size(); if(ns<2) return;
  int vs = l_vertex.size();
  std::list<Trk::Vertex>::iterator v,ve=l_vertex.end();  

  // Loop through all trigger space points
  //
  for(int n0  = 1; n0!=ns; ++n0) {

    float X  = x_sorted[n0]->globalPosition().x(); 
    if(X<m_r2min) continue; if(X>m_r2max) break;
    float Y  = x_sorted[n0]->globalPosition().y();
    float Z  = x_sorted[n0]->globalPosition().z();

    // Bottom link production
    //
    for(int nb=n0-1; nb>=0; --nb) {
      
      float Xb =   x_sorted[nb]->globalPosition().x(); 
      if(Xb>m_r1max) continue; if(X<m_r1min) break;
      float dx = X-Xb; if(dx<m_drmin) continue; if(dx>m_drmax) break; dx=1./dx;
      float Az = (Z-x_sorted[nb]->globalPosition().z())*dx; 
      if(Az < m_dzdrmin || Az > m_dzdrmax ) continue;
      float Ay = (Y-x_sorted[nb]->globalPosition().y())*dx; 
      if(Ay < m_dzdrmin || Ay > m_dzdrmax ) continue;
      float Zo = Z-Az*X;	          

      if(vs) {
	float dZmin = 10000.;
	for(v=l_vertex.begin(); v!=ve; ++v) {
	  float dZ = fabs((*v).position().z()-Zo); if(dZ<dZmin) dZmin = dZ;
	}
	if(dZmin>((m_dzver+m_dzdrver*Xb))) continue;
      }
      
    InDet::SiSpacePointsSeed* 
	se      = new InDet::SiSpacePointsSeed(x_sorted[nb],x_sorted[n0],Zo);
      l_seeds.push_back(se);	
    }
  }
}

///////////////////////////////////////////////////////////////////
// Production 3 space points seeds with or without vertex constraint
///////////////////////////////////////////////////////////////////

void InDet::SiSpacePointsSeedMaker_CTBxk::production3()
{ 

  int ns = x_sorted.size(); if(ns<3) return;
  int vs = l_vertex.size();
  std::list<Trk::Vertex>::iterator v,ve=l_vertex.end();  
 
  bool  mf  = false     ;
  float K   = 0.        ;
  float ipt = 100000000.; if(m_ptmin!=0.) ipt= 1./fabs(m_ptmin);

  if(m_fieldService) {
    double f[3], gP[3] ={10.,10.,0.}; m_fieldService->getFieldZR(gP,f);
    if(fabs(f[2])>1.e-8) {K = 1./(300.*f[2]); mf=true;}	
  }
  const Trk::SpacePoint* Sp[500];
  float                 Ay[500];
  float                 Az[500];
  float                 Zo[500];
  float                 U [500];
  float                 V [500];

  // Loop through all trigger space points
  //
  for(int n0  = 1; n0!=ns; ++n0) {

    float X  = x_sorted[n0]->globalPosition().x(); 
    float Y  = x_sorted[n0]->globalPosition().y();
    float Z  = x_sorted[n0]->globalPosition().z();
    float R  = sqrt(X*X+Y*Y);
    
    if(R<m_r2min) continue; if(R>m_r2max) break;
   
    // Bottom link production
    //
    int Nb = 0;
    for(int nb=n0-1; nb>=0; --nb) {
      
      float Xb = x_sorted[nb]->globalPosition().x();
      float Yb = x_sorted[nb]->globalPosition().y();
      float Zb = x_sorted[nb]->globalPosition().z();
      float Rb = sqrt(Xb*Xb+Yb*Yb);
 
      if(Rb>m_r1max) continue; if(Rb<m_r1min) break;

      float Ab = X*Xb+Y*Yb;
      if(Ab < 0.) continue;

      float dx = X-Xb;
      float dy = Y-Yb;
      float dz = Z-Zb;
      float dr = sqrt(dx*dx+dy*dy);
      if(dr < m_drmin) continue; if(dr>m_drmax) break; dr = 1./dr;
    
      Az[Nb] = dz*dr;
      if(Az[Nb] < m_dzdrmin || Az[Nb] > m_dzdrmax ) continue;
      Ay[Nb] = dy*dr;
      if(Ay[Nb] < m_dzdrmin || Ay[Nb] > m_dzdrmax ) continue;
      Sp[Nb] = x_sorted[nb];
      Zo[Nb] = Z-Az[Nb]*R;

      if(vs) {
	float dZmin = 10000.;
	for(v=l_vertex.begin(); v!=ve; ++v) {
	  float dZ = fabs((*v).position().z()-Zo[Nb]); if(dZ<dZmin) dZmin = dZ;
	}
	if(dZmin>((m_dzver+m_dzdrver*Xb))) continue;
      }

      if(++Nb==500) break;
    }
    if(Nb==0 || Nb==500) continue;

    // Top link production
    //
    int Nt = Nb;
    for(int nt=n0+1; nt!=ns; ++nt) {


      float Xt = x_sorted[nt]->globalPosition().x();
      float Yt = x_sorted[nt]->globalPosition().y();
      float Zt = x_sorted[nt]->globalPosition().z();
      float Rt = sqrt(Xt*Xt+Yt*Yt);
 
      if(Rt<m_r3min) continue; if(Rt>m_r3max) break;
      float At = X*Xt+Y*Yt;
      if(At < 0.) continue;

      float dx = Xt-X;
      float dy = Yt-Y;
      float dz = Zt-Z;
      float dr = sqrt(dx*dx+dy*dy);
      if(dr < m_drmin) continue; if(dr>m_drmax) break; dr = 1./dr;
      
      Az[Nt] = dz*dr;
      if(Az[Nt] < m_dzdrmin || Az[Nt] > m_dzdrmax ) continue;
      Ay[Nt] = dy*dr;
      if(Ay[Nt] < m_dzdrmin || Ay[Nt] > m_dzdrmax ) continue;
      Sp[Nt] = x_sorted[nt];
      if(++Nt==500) break;
    }
    if(Nt==Nb) continue;

    // In case magnetic field
    //
    if(mf) {

      float ax = X/R, ay = Y/R;
      for(int i=0; i!=Nt; ++i) {
	float dx = x_sorted[i]->globalPosition().x()-X, dy = x_sorted[i]->globalPosition().y()-Y;
	float x  = dx*ax+dy*ay, y  =-dx*ay+dy*ax, r2 = 1./(x*x+y*y);
	U[i]      = x*r2;      V[i] = y*r2;
      }
    }

    // Three space points comparison
    //
    for(int b=0; b!=Nb; ++b) {
      for(int t=Nb; t!=Nt; ++t) {

	if(fabs(Az[b]-Az[t])>m_dazmax) continue;
	if(mf) {
	  float A = (V[t]-V[b])/(U[t]-U[b]), B = V[t]-A*U[t], C = 2.*B/sqrt(1.+A*A);
	  if(fabs(C*K)>ipt) continue;
	}
	else   {
	  if(fabs(Ay[b]-Ay[t])>m_daymax) continue;
	}
	InDet::SiSpacePointsSeed* 
	  se = new InDet::SiSpacePointsSeed(Sp[b],x_sorted[n0],Sp[t],Zo[b]);
	l_seeds.push_back(se);	
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiSpacePointsSeedMaker_CTBxk::dump( MsgStream& out ) const
{
  out<<std::endl;
  int n = 62-m_spacepointsPixelname.size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 62-m_spacepointsSCTname.size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 62-m_spacepointsOverlapname.size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Pixel    space points   | "<<m_spacepointsPixelname <<s2
     <<std::endl;
  out<<"| SCT      space points   | "<<m_spacepointsSCTname    <<s3
     <<std::endl;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlapname<<s4
     <<std::endl;
  out<<"| usePixel                | "
     <<std::setw(12)<<m_pixel 
     <<"                                                  |"<<std::endl;
  out<<"| useSCT                  | "
     <<std::setw(12)<<m_sct 
     <<"                                                  |"<<std::endl;
  out<<"| use Magnetic field      | "
     <<std::setw(12)<<m_usefield 
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
 
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiSpacePointsSeedMaker_CTBxk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiSpacePointsSeedMaker_CTBxk& se)
{ 
  return se.dump(sl); 
}
///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiSpacePointsSeedMaker_CTBxk& se)
{ 
  return se.dump(sl); 
}   


///////////////////////////////////////////////////////////////////
// New and old list vertices comparison
///////////////////////////////////////////////////////////////////

bool InDet::SiSpacePointsSeedMaker_CTBxk::newVertices(const std::list<Trk::Vertex>& lV)
{
  int s1 = l_vertex.size(); 
  int s2 = lV      .size(); 
  if(s1==0 && s2==0) return false;

  std::list<Trk::Vertex>::const_iterator v,vn=lV.begin();
  if(s1==s2) {

    for(v=l_vertex.begin(); v!=l_vertex.end(); ++v) {

      if(fabs((*v).position().x()-(*vn).position().x())>.01) break;
      if(fabs((*v).position().y()-(*vn).position().y())>.01) break;
      if(fabs((*v).position().z()-(*vn).position().z())>.01) break; ++vn;
    }
    if(v==l_vertex.end()) return false;
  }

  l_vertex.erase(l_vertex.begin(),l_vertex.end());
  
  for(v=lV.begin(); v!=lV.end(); ++v) {
    l_vertex.push_back((*v));
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// Sort space points in radius order
///////////////////////////////////////////////////////////////////
//void InDet::TRT_Trajectory_xk::sort(float* a,int* b,int n) {

void InDet::SiSpacePointsSeedMaker_CTBxk::sortRadius() {

  int n = x_sorted.size();
  if(n<=1) return; 
  
  int mt[50],lt[50], j=n-1, i=0, m=0;
  while (1) {
    if(j-i>1) {
      int ip=(j+i)>>1, q=j; 
      const Trk::SpacePoint* ta=x_sorted[ip]; x_sorted[ip]=x_sorted[i]; 
  
    for(int k =i+1; k<=q; ++k) {

	if(x_sorted[k]->globalPosition().perp() <= ta->globalPosition().perp()) continue; 

	int l=q;
	while(1) {
	  if(x_sorted[l]->globalPosition().perp() < ta->globalPosition().perp()) {

	    const Trk::SpacePoint* x=x_sorted[k]; x_sorted[k]=x_sorted[l]; x_sorted[l]=x;
	    q=l-1; break;
	  }
	  if(--l< k) {q=k-1; break;}
	}
      }
      x_sorted[i]=x_sorted[q]; x_sorted[q]=ta; 
      if((q<<1)>i+j) {lt[m]=i;   mt[m++]=q-1; i=q+1;}
      else           {lt[m]=q+1; mt[m++]=j;   j=q-1;} 
    }
    else      {
      if(i<j && x_sorted[i]->globalPosition().perp() > x_sorted[j]->globalPosition().perp()) {
	const Trk::SpacePoint*	x=x_sorted[i]; x_sorted[i]=x_sorted[j]; x_sorted[j]=x;
      }
      if(--m<0) return; i=lt[m]; j=mt[m];
    }
  }
}

