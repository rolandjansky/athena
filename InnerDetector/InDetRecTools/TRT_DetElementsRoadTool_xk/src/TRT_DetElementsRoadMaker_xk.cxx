/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_DetElementsRoadMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include <utility>

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "EventInfo/TagInfo.h"

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsRoadMaker_xk.h"
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsComparison.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DetElementsRoadMaker_xk::TRT_DetElementsRoadMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                          ,
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator" )
{
  m_trt         = "TRT"                      ;
  m_width       = 10.                        ;
  m_step        = 20.                        ;
  m_fieldmode   = "MapSolenoid"              ;

  declareInterface<ITRT_DetElementsRoadMaker>(this);
  declareProperty("TRTManagerLocation"   ,m_trt        );
  declareProperty("RoadWidth"            ,m_width      );
  declareProperty("MaxStep"              ,m_step       );
  declareProperty("PropagatorTool"       ,m_proptool   );
  declareProperty("MagneticFieldMode"    ,m_fieldmode  );
  declareProperty("MagFieldSvc"          , m_fieldServiceHandle);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DetElementsRoadMaker_xk::~TRT_DetElementsRoadMaker_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetElementsRoadMaker_xk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get  TRT Detector Manager
  //
  const InDetDD::TRT_DetectorManager*  trtmgr = 0;
  sc = detStore()->retrieve(trtmgr,m_trt);
  if (sc.isFailure() || !trtmgr) {
    msg(MSG::FATAL)<<"Could not get TRT DetectorManager  !"<<endmsg; 
    return StatusCode::FAILURE;
  }
 
  // Get magnetic field service
  //
  if(m_fieldmode != "NoField" ) {
    
    if( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }    
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
    m_fieldService = &*m_fieldServiceHandle;
  }
  if(m_fieldmode == "NoField") m_fieldModeEnum = Trk::NoField;
  else if(m_fieldmode == "MapSolenoid") m_fieldModeEnum = Trk::FastField;
  else m_fieldModeEnum = Trk::FullField;
  
  // Get propagator tool
  //
  if (m_proptool.retrieve().isFailure() ) {
    msg(MSG::FATAL)<< "Failed to retrieve tool " << m_proptool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_proptool << endmsg;
  }

  // get the key -- from StoreGate (DetectorStore)
  //
  std::vector< std::string > tagInfoKeys = detStore()->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if(tagInfoKeys.size()==0)
    msg(MSG::WARNING) << " No TagInfo keys in DetectorStore "<< endmsg;
   else {
     if(tagInfoKeys.size() > 1) {
       msg(MSG::WARNING) <<"More than one TagInfo key in the DetectorStore, using the first one "<< endmsg;
     }
     tagInfoKey = tagInfoKeys[0];
   }

  m_callbackString = tagInfoKey;

  const DataHandle<TagInfo> tagInfoH;
  
  // register the Callback
  //

  sc = detStore()->regFcn(&InDet::TRT_DetElementsRoadMaker_xk::mapDetectorElementsProduction,
			  this,tagInfoH,m_callbackString);
  if (sc.isFailure()) {
    msg(MSG::FATAL)<< "Failed to register the callback " << m_proptool << endmsg;
  }
  else {
    msg(MSG::INFO) << "Register the callback" << m_proptool << endmsg;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetElementsRoadMaker_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_DetElementsRoadMaker_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out);
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_DetElementsRoadMaker_xk::dumpConditions( MsgStream& out ) const
{
  int n = 62-m_proptool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if(!m_fieldService->solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  int mode = fieldprop.magneticFieldMode();
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  int maps = 0;
  if(m_layer[0].size()) ++maps;
  if(m_layer[1].size()) ++maps;
  if(m_layer[2].size()) ++maps;

  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  out<<"| Tool for propagation    | "<<m_proptool.type()<<s1<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]<<s3<<std::endl;
  out<<"| Width of the road (mm)  | "
     <<std::setw(12)<<std::setprecision(5)<<m_width
     <<"                                                  |"<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;

  if(!maps || m_outputlevel==0) return out;

  if(m_layer[1].size()) {
    int nl = m_layer[1].size();
    int nc = 0;
    for(unsigned int i=0; i!=m_layer[1].size(); ++i) nc+=m_layer[1][i].nElements();
    out<<"|----------------------------------------------------------------|"
       <<std::endl;
    out<<"| Barrel map containt "
       <<std::setw(4)<<nl<<" layers and "
       <<std::setw(6)<<nc<<" elements            |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     R     |   Z min    |   Z max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[1].size(); ++i) {
      float zmin = m_layer[1][i].z()-m_layer[1][i].dz();
      float zmax = m_layer[1][i].z()+m_layer[1][i].dz();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  m_layer[1][i].r ()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<< m_layer[1][i].dfe()<<" | "
	 <<std::setw(4)<<m_layer[1][i].nElements()<<" | "
	 <<std::endl;
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;

  }
  if(m_layer[0].size()) {

    int nl = m_layer[0].size();
    int nc = 0;
    for(unsigned int i=0; i!=m_layer[0].size(); ++i) nc+=m_layer[0][i].nElements();
    out<<"|----------------------------------------------------------------|"
       <<std::endl;
    out<<"| L.Endcap map containt "
       <<std::setw(4)<<nl<<" layers and "
       <<std::setw(6)<<nc<<" elements          |"
       <<std::endl;

    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[0].size(); ++i) {
      float rmin = m_layer[0][i].r()-m_layer[0][i].dr();
      float rmax = m_layer[0][i].r()+m_layer[0][i].dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  m_layer[0][i].z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<m_layer[0][i].dfe()<<" | "
	 <<std::setw(4)<<m_layer[0][i].nElements()<<" | "
	 <<std::endl;
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
  }
  if(m_layer[2].size()) {
    int nl = m_layer[2].size();
    int nc = 0;
    for(unsigned int i=0; i!=m_layer[2].size(); ++i) nc+=m_layer[2][i].nElements();
    out<<"|----------------------------------------------------------------|"
       <<std::endl;
   out<<"| R.Endcap map containt "
       <<std::setw(4)<<nl<<" layers and "
       <<std::setw(6)<<nc<<" elements          |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[2].size(); ++i) {
      float rmin = m_layer[2][i].r()-m_layer[0][i].dr();
      float rmax = m_layer[2][i].r()+m_layer[0][i].dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  m_layer[2][i].z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<m_layer[2][i].dfe()<<" | "
	 <<std::setw(4)<<m_layer[2][i].nElements()<<" | "
	 <<std::endl;
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
 }
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_DetElementsRoadMaker_xk::dumpEvent( MsgStream& out ) const
{
  out<<"|--------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Road size               | "<<std::setw(12)<<m_sizeroad
     <<"                             |"<<std::endl;
  out<<"|--------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_DetElementsRoadMaker_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_DetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_DetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Main methods for road builder 
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoad
(const Trk::TrackParameters& Tp,Trk::PropDirection D, 
 std::list<const InDetDD::TRT_BaseElement*>& R) 
{

  double qp   = fabs(500.*Tp.parameters()[4]) ; if( qp < 1.e-10  ) qp = 1.e-10; 
  double S    = m_step/qp                     ; if( S  > 200.    ) S  = 200.  ; if(D<0) S=-S; 

  Trk::CylinderBounds CB = getBound(Tp);

  if( CB.r() > m_rminTRT) {
    Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
    if(!m_fieldService->solenoidOn()) fieldModeEnum = Trk::NoField;
    Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

    std::list<Amg::Vector3D> G;
    m_proptool->globalPositions(G,Tp,fieldprop,CB,S,Trk::pion);

    if(G.size() > 1 ) {
      m_map[0]>0 ? detElementsRoadATL(G,R) : detElementsRoadCTB(G,R);
    }  
  }

  if(m_outputlevel<0) {
    m_sizeroad = R.size();
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder 
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoad
(const Trk::TrackParameters& Tp,Trk::PropDirection D, 
 std::list<std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> >& R)
{
 std::list<const InDetDD::TRT_BaseElement*> RE;  detElementsRoad(Tp,D,RE);

 if(m_outputlevel<0) {
   m_sizeroad = R.size();
   m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
 }

 std::list<const InDetDD::TRT_BaseElement*>::const_iterator r=RE.begin(),re=RE.end();
 if(r==re) return;

 Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
 if(!m_fieldService->solenoidOn()) fieldModeEnum = Trk::NoField;
 Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

 const Trk::TrackParameters* tp0 = 
   m_proptool->propagate(Tp,(*r)->surface(),D,false,fieldprop,Trk::pion);
 if(!tp0) return;

 std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> EP0((*r),tp0);
 R.push_back(EP0);

 for(++r; r!=re; ++r) {
   
   const Trk::TrackParameters* tp = 
     m_proptool->propagate((*tp0),(*r)->surface(),D,false,fieldprop,Trk::pion);
   if(!tp) return;

   std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> EP((*r),tp);
   R.push_back(EP); tp0=tp;
 }
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using input list global positions
// for Atlas geometry
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoadATL
(std::list<Amg::Vector3D>& GP,
 std::list<const InDetDD::TRT_BaseElement*>& Road) 
{
  m_sizeroad = 0;
  int n0     = 0;
  int n1     = 0;
  int n2     = 0;
  std::list<Amg::Vector3D>::iterator g=GP.begin(),ge=GP.end();

  float Po[6] = {float((*g).x()),float((*g).y()),float((*g).z()),
		 float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y())),m_width,0.};

  for(; n0!=m_map[0]; ++n0) {if(Po[2] > m_layer[0][n0].z()) break;}
  for(; n1!=m_map[1]; ++n1) {if(Po[3] < m_layer[1][n1].r()) break;}
  for(; n2!=m_map[2]; ++n2) {if(Po[2] < m_layer[2][n2].z()) break;}

  std::list<InDet::TRT_DetElementLink_xk*> lDE;

  for(++g; g!=ge; ++g) {

    float Pn[4] = {float((*g).x()),float((*g).y()),float((*g).z()),
		   float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y()))};

    float dx = Pn[0]-Po[0];
    float dy = Pn[1]-Po[1];
    float dz = Pn[2]-Po[2];
    float st = sqrt(dx*dx+dy*dy+dz*dz); if(st <=0.) continue;
    float ds = 1./st;
    float A[3]= {dx*ds,dy*ds,dz*ds};
   
    // Barrel
    //
    if(Pn[3]>Po[3]) {
      for(; n1<m_map[1]; ++n1) {
	if(Pn[3] < m_layer[1][n1].r()) break; 
	m_layer[1][n1].getBarrelDetElementsATL(Po,A,lDE);
      }
    }
    else     {
      for(--n1; n1>=0; --n1) {
	if(Pn[3] > m_layer[1][n1].r()) break; 
	m_layer[1][n1].getBarrelDetElementsATL(Po,A,lDE);
     }
      ++n1;
    }

    // Positive endcap
    //
    if(Pn[2]>Po[2]) {

      for(; n2<m_map[2]; ++n2) {
	if(Pn[2] < m_layer[2][n2].z()) break; 
	m_layer[2][n2].getEndcapDetElements(Po,A,lDE);
      }
    }
    else     {
      for(--n2; n2>=0; --n2) {
	if(Pn[2] > m_layer[2][n2].z()) break; 
	m_layer[2][n2].getEndcapDetElements(Po,A,lDE);
      }
      ++n2;
    }
    
    // Negative endcap
    //
    if(Pn[2]<Po[2]) {

      for(; n0<m_map[0]; ++n0) {
	if(Pn[2] > m_layer[0][n0].z()) break; 
	m_layer[0][n0].getEndcapDetElements(Po,A,lDE);
      }
    }
     else   {
      for(--n0; n0>=0; --n0) {
	if(Pn[2] < m_layer[0][n0].z()) break; 
	m_layer[0][n0].getEndcapDetElements(Po,A,lDE);
      } 
      ++n0;
    }
    Po[0] = Pn[0];
    Po[1] = Pn[1];
    Po[2] = Pn[2];
    Po[3] = Pn[3];
    Po[5]+= st;
  }

  // Sort list in propogation order
  //
  std::list<InDet::TRT_DetElementLink_xk*>::iterator l=lDE.begin(),le=lDE.end(),n,m;
  if(l==le) return;

  bool nc =true;
  while(nc) {
 
    nc =false; m=l; n=l;
    for(++n; n!=le; ++n) {
   
      if( (*m)->way() > (*n)->way() ) {
	InDet::TRT_DetElementLink_xk* d=(*m); (*m)=(*n); (*n)=d; nc=true;
      }
      ++m;
    }
  }

  // Fill list pointers to detector elements
  //
  for(l=lDE.begin(); l!=le; ++l) {
    Road.push_back((*l)->detElement()); (*l)->clearUsed();
  }
  m_sizeroad = Road.size();
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using input list global positions
// for CTB geometry
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoadCTB
(std::list<Amg::Vector3D>& GP,
 std::list<const InDetDD::TRT_BaseElement*>& Road) 
{
  m_sizeroad = 0;
  int n1     = 0;
  std::list<Amg::Vector3D>::iterator g=GP.begin(),ge=GP.end();

  float Po[6] = {float((*g).x()),float((*g).y()),float((*g).z()),
		 float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y())),m_width,0.};

  for(; n1!=m_map[1]; ++n1) {if(Po[3] < m_layer[1][n1].r()) break;}

  std::list<InDet::TRT_DetElementLink_xk*> lDE;

  for(++g; g!=ge; ++g) {

    float Pn[4] = {float((*g).x()),float((*g).y()),float((*g).z()),
		   float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y()))};

    float dx = Pn[0]-Po[0];
    float dy = Pn[1]-Po[1];
    float dz = Pn[2]-Po[2];
    float st = sqrt(dx*dx+dy*dy+dz*dz);
    float ds = 1./st;
    float A[3]= {dx*ds,dy*ds,dz*ds};

    // Barrel
    //
    if(Pn[3]>Po[3]) {
      for(; n1<m_map[1]; ++n1) {
	if(Pn[3] < m_layer[1][n1].r()) break; 
	m_layer[1][n1].getBarrelDetElementsCTB(Po,A,lDE);
      }
    }
    else     {
      for(--n1; n1>=0; --n1) {
	if(Pn[3] > m_layer[1][n1].r()) break; 
	m_layer[1][n1].getBarrelDetElementsCTB(Po,A,lDE);
      }
      ++n1;
    }
  }

  // Sort list in propogation order
  //
  std::list<InDet::TRT_DetElementLink_xk*>::iterator l=lDE.begin(),le=lDE.end(),n;
  if(l==le) return;

  bool nc =true;
  while(nc) {

    nc =false; n=l;
    for(++n; n!=le; ++n) {
   
      if( (*l)->way() > (*n)->way() ) {
	InDet::TRT_DetElementLink_xk* d = (*l); (*l) = (*n); (*n) = d;
	nc = true;
      }
      ++l;
    }
  }

  // Fill list pointers to detector elements
  //
  for(l=lDE.begin(); l!=le; ++l) {
    Road.push_back((*l)->detElement()); (*l)->clearUsed();
  }
  m_sizeroad = Road.size();
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetElementsRoadMaker_xk::mapDetectorElementsProduction
(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  (void) I;

  const InDetDD::TRT_DetectorManager*  trtmgr = 0;
  StatusCode sc = detStore()->retrieve(trtmgr,m_trt);

  if (sc.isFailure() || !trtmgr) {
    msg(MSG::FATAL)<<"Could not get TRT DetectorManager  !"<<endmsg; 
    return StatusCode::FAILURE;
  }

  // check if the string is ESD for guaranteeing that misalignment has been introduced already
  //
  bool needsUpdate = false;

  for (std::list<std::string>::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    if ((*k) == m_callbackString) {needsUpdate = true; break;}
  } 
  if(!needsUpdate) return StatusCode::SUCCESS;

  const double pi2=2.*M_PI, pi=M_PI;
  m_map[0]=m_map[1]=m_map[2]=0;
  m_layer[0].erase(m_layer[0].begin(),m_layer[0].end());  
  m_layer[1].erase(m_layer[1].begin(),m_layer[1].end());  
  m_layer[2].erase(m_layer[2].begin(),m_layer[2].end());  

  double P[33], Wf = 0.,Wz = 0.;
  double mzmin [3] ;  // min Z coordinate 
  double mzmax [3] ;  // max Z coordinate
  double mrmin [3] ;  // min radius
  double mrmax [3] ;  // max radius

  // Barrel
  //
  int N = 1;

  int Rings  = trtmgr->getNumerology()->getNBarrelRings();
  int NPhi   = trtmgr->getNumerology()->getNBarrelPhi(); 
  
  mrmin[N] = 100000.; mrmax[N] =-100000.;
  mzmin[N] = 100000.; mzmax[N] =-100000.;
  
  for(int ring = 0; ring!=Rings; ++ring) {
    
    int NSlayers = trtmgr->getNumerology()->getNBarrelLayers(ring);
    
    for(int nsl=0; nsl!=NSlayers; ++nsl) {
      
      InDet::TRT_DetElementsLayer_xk layer;
      double rmin = 100000., rmax =-100000.;
      double zmin = 100000., zmax =-100000.;
      double dfm  = 0.;
      
      std::vector<const InDetDD::TRT_BaseElement*> pE;

      for(int f=0; f!=NPhi; ++f) {
	pE.push_back(trtmgr->getBarrelElement(0,ring,f,nsl));
	pE.push_back(trtmgr->getBarrelElement(1,ring,f,nsl));
      }
      
      std::sort(pE.begin(),pE.end(),InDet::compTRTDetElements_AZ());
      for(unsigned int j=0; j!=pE.size(); ++j) {

	if(pE[j]) {

	  detElementInformation(*(pE[j]),P);
	  Wf = sqrt(P[20]*P[20]+P[21]*P[21]);
	  Wz = sqrt(P[22]*P[22]+P[23]*P[23]); 
	  if( P[ 9] < mrmin[N] ) mrmin[N] = P[ 9]; 
	  if( P[10] > mrmax[N] ) mrmax[N] = P[10]; 
	  if( P[11] < mzmin[N] ) mzmin[N] = P[11]; 
	  if( P[12] > mzmax[N] ) mzmax[N] = P[12]; 
	  
	  if( P[ 9] < rmin ) rmin = P[ 9]; 
	  if( P[10] > rmax ) rmax = P[10]; 
	  if( P[11] < zmin ) zmin = P[11]; 
	  if( P[12] > zmax ) zmax = P[12]; 
	  
	  double df1 = fabs(P[13]-P[2]); if(df1>pi) df1 = fabs(df1-pi2); 
	  double df2 = fabs(P[14]-P[2]); if(df2>pi) df2 = fabs(df2-pi2); 
	  if(df1>dfm) dfm = df1;
	  if(df2>dfm) dfm = df2;
	  InDet::TRT_DetElementLink_xk link(pE[j],P);
	  layer.add(link);
	}
      }
      double r  =(rmax+rmin)*.5;
      double dr =(rmax-rmin)*.5; 
      double z  =(zmax+zmin)*.5;
      double dz =(zmax-zmin)*.5;
      layer.set(r,dr,z,dz,dfm,Wf,Wz);
      m_layer[N].push_back(layer);
    }
  }

  // Endcaps
  //
  int Wheels = trtmgr->getNumerology()->getNEndcapWheels();
  NPhi       = trtmgr->getNumerology()->getNEndcapPhi   (); 
  if(Wheels ) {

    for(N=0; N<3; N+=2) {

      mrmin[N] = 100000.; mrmax[N] =-100000.;
      mzmin[N] = 100000.; mzmax[N] =-100000.;
      
      int side = 0; if(N==2) side = 1;
      for(int wh = 0; wh!=Wheels; ++wh) {
	
	int ns = trtmgr->getNumerology()->getNEndcapLayers(wh);
	for(int s = 0; s!=ns; ++s) {
	  
	  InDet::TRT_DetElementsLayer_xk layer;
	  double rmin = 100000., rmax =-100000.;
	  double zmin = 100000., zmax =-100000.;
	  double dfm  = 0.;
	  std::vector<const InDetDD::TRT_BaseElement*> pE;
	  
	  for(int f=0; f!=NPhi; ++f) {
	    pE.push_back(trtmgr->getEndcapElement(side,wh,s,f));
	  }
	  std::sort(pE.begin(),pE.end(),InDet::compTRTDetElements_A());
	  
	  for(unsigned int j=0; j!=pE.size(); ++j) {
	    
	    if(pE[j]) {

	      detElementInformation(*(pE[j]),P);
	      Wf = sqrt(P[20]*P[20]+P[21]*P[21]);
	      Wz = sqrt(P[22]*P[22]+P[23]*P[23]); 
	      
	      if( P[ 9] < mrmin[N] ) mrmin[N] = P[ 9]; 
	      if( P[10] > mrmax[N] ) mrmax[N] = P[10]; 
	      if( P[11] < mzmin[N] ) mzmin[N] = P[11]; 
	      if( P[12] > mzmax[N] ) mzmax[N] = P[12]; 
	      
	      if( P[ 9] < rmin ) rmin = P[ 9]; 
	      if( P[10] > rmax ) rmax = P[10]; 
	      if( P[11] < zmin ) zmin = P[11]; 
	      if( P[12] > zmax ) zmax = P[12]; 
	      
	      double df1 = fabs(P[13]-P[2]); if(df1>pi) df1 = fabs(df1-pi2); 
	      double df2 = fabs(P[14]-P[2]); if(df2>pi) df2 = fabs(df2-pi2); 
	      if(df1>dfm) dfm = df1;
	      if(df2>dfm) dfm = df2;
	      
	      InDet::TRT_DetElementLink_xk link(pE[j],P);
	      layer.add(link);
	    }
	  }
	  double r  =(rmax+rmin)*.5;
	  double dr =(rmax-rmin)*.5; 
	  double z  =(zmax+zmin)*.5;
	  double dz =(zmax-zmin)*.5;
	  layer.set(r,dr,z,dz,dfm,Wf,Wz);
	  m_layer[N].push_back(layer);
	}
      }
    }
  }

  m_map[0] = m_layer[0].size();
  m_map[1] = m_layer[1].size();
  m_map[2] = m_layer[2].size();

  // CylinderBounds production
  //
  double zmi = +100000.;
  double zma = -100000.;
  double rma = -100000.;
  double rmi = +100000.; 
  for(int i=0; i!=3; ++i) {
    if(m_map[i]) {
      if(mzmin[i]<zmi) zmi=mzmin[i]; 
      if(mzmax[i]>zma) zma=mzmax[i]; 
      if(mrmax[i]>rma) rma=mrmax[i];
      if(mrmin[i]<rmi) rmi=mrmin[i];
    }
  }

  m_rminTRT = rmi;

  double hz = fabs(zma); if(hz<fabs(zmi)) hz = fabs(zmi);
  const Trk::CylinderBounds CB(rma+20.,hz+20.);
  m_bounds  = CB;

  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Output parameters: P[ 0]  - radius          centre of wafer
//                    P[ 1]  - z               centre of wafer
//                    P[ 2]  - azimuthal angle centra of wifer
//                    P[ 3]  - min. distance to surface
//                    P[ 4]  - azimythal angle normal to surface
//                    P[ 5]  - sin(P[4])
//                    P[ 6]  - cos(P[4])
//                    P[ 7]  - sin(Polar angle)
//                    P[ 8]  - cos(Polar abgle)
//                    P[ 9]  - min. radius   
//                    P[10]  - max. radius   
//                    P[11]  - min. z
//                    P[12]  - max  z  
//                    P[13]  - min. azimythal angle  
//                    P[14]  - max. azimuthal angle
//                    P[15]  - tilt angle
//                    P[16]  - min. dist to surface in XY-plane
//                    P[17]  - Tilt angle
//                    P[18]  - first  local coordinate of wafer
//                    P[19]  - second local coordinate of wafer 
//               P[20],P[21] - vector to -F  boundary 
//               P[22],P[23] - vector to +ZR boundary 
//               P[24],P[25] - vector to +F  boundary 
//               P[26],P[27] - vector to -ZR boundary 
//                    P[28]  - thikness of the detector element
//                    P[29]  - Ax of Phi exis
//                    P[30]  - Ay of Phi exis
//                    P[31]  - Ax of Eta exis
//                    P[32]  - Ay of Eta exis
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementInformation
(const InDetDD::TRT_BaseElement& E,double* P) 
{
  const double pi2=2.*M_PI, pi=M_PI;

  const Trk::RectangleBounds* rb = dynamic_cast<const Trk::RectangleBounds*>(&E.bounds());
  const Trk::DiscBounds*      db = dynamic_cast<const Trk::DiscBounds*>     (&E.bounds());

  Amg::Vector3D  C  = E.center();  double x0=C.x(), y0=C.y(), z0=C.z(); 
  Amg::Vector3D  AT = E.normal();  double A[3]={AT.x(),AT.y(),AT.z()};
  const Amg::Transform3D& T = E.surface().transform();
  double Ax[3]            = {T(0,0),T(1,0),T(2,0)};
  double Ay[3]            = {T(0,1),T(1,1),T(2,1)};
  double x[4],y[4],z[4];

  if(rb) {

    P[ 0]    = sqrt(x0*x0+y0*y0);
    P[ 1] = z0;   if(fabs(P[1])<.00004) P[1]=0.; 
    P[ 2] = atan2(y0,x0);
    P[ 3] = A[0]*x0+A[1]*y0+A[2]*z0;
    if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
    double B  = A[0]*A[0]+A[1]*A[1]; 
    if(B>.01) P[4]=atan2(A[1],A[0]); else P[4]=P[2]; 

    sincos(P[4],&P[5],&P[6]);

    if(fabs(P[5])<.00004) P[5]=0.; 
    if(fabs(P[6])<.00004) P[6]=0.; 
    P[ 7] = sqrt(A[0]*A[0]+A[1]*A[1]); if(fabs(P[7])<.00004) P[7]=0.; 
    P[ 8] = A[2];                      if(fabs(P[8])<.00004) P[8]=0.;
   
    double dx = rb->halflengthX();
    double dy = rb->halflengthY();

    x[ 0] = x0+dx*Ax[0]+dy*Ay[0];
    y[ 0] = y0+dx*Ax[1]+dy*Ay[1];
    z[ 0] = z0+dx*Ax[2]+dy*Ay[2];

    x[ 1] = x0+dx*Ax[0]-dy*Ay[0];
    y[ 1] = y0+dx*Ax[1]-dy*Ay[1];
    z[ 1] = z0+dx*Ax[2]-dy*Ay[2];

    x[ 2] = x0-dx*Ax[0]-dy*Ay[0];
    y[ 2] = y0-dx*Ax[1]-dy*Ay[1];
    z[ 2] = z0-dx*Ax[2]-dy*Ay[2];

    x[ 3] = x0-dx*Ax[0]+dy*Ay[0];
    y[ 3] = y0-dx*Ax[1]+dy*Ay[1];
    z[ 3] = z0-dx*Ax[2]+dy*Ay[2];
  }
  else if (db) {

    Ax[0]=T(0,1); Ax[1]=T(1,1); Ax[2]=T(2,1);
    Ay[0]=T(0,0); Ay[1]=T(1,0); Ay[2]=T(2,0);

    P[ 0] = .5*(db->rMin()+db->rMax()); 
    P[ 1] = z0; 
    P[ 2] = atan2(Ay[1],Ay[0]);
    P[ 3] = A[0]*x0+A[1]*y0+A[2]*z0;
    if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
    P[ 4] = P[2];

    sincos(P[4],&P[5],&P[6]);
    P[ 7] = sqrt(A[0]*A[0]+A[1]*A[1]); if(fabs(P[7])<.00004) P[7]=0.; 
    P[ 8] = A[2];                      if(fabs(P[8])<.00004) P[8]=0.;
    x0    = P[0]*P[6];
    y0    = P[0]*P[5];
    double df = db->halfPhiSector();
    double sn1,sn2,cs1,cs2;

    sincos(P[2]+df,&sn1,&cs1);
    sincos(P[2]-df,&sn2,&cs2);
  
    x[ 0] = db->rMax()*cs1;
    y[ 0] = db->rMax()*sn1;
    z[ 0] = z0;

    x[ 1] = db->rMin()*cs1;
    y[ 1] = db->rMin()*sn1;
    z[ 1] = z0;

    x[ 2] = db->rMin()*cs2;
    y[ 2] = db->rMin()*sn2;
    z[ 2] = z0;

    x[ 3] = db->rMax()*cs2;
    y[ 3] = db->rMax()*sn2;
    z[ 3] = z0;
  }
  else return;

  double rmin= 10000., rmax=-10000.;
  double zmin= 10000., zmax=-10000.;
  double fmin= 10000., fmax=-10000.;
  for(int i=0; i!=4; ++i) {
    double r = sqrt(x[i]*x[i]+y[i]*y[i]);
    double f = atan2(y[i],x[i])-P[2]; if(f<-pi) f+=pi2; else if(f>pi) f-=pi2;
    double zf= z[i];
    if(r <rmin) rmin= r;
    if(r >rmax) rmax= r;
    if(zf<zmin) zmin=zf;
    if(zf>zmax) zmax=zf;
    if(f <fmin) fmin= f;
    if(f >fmax) fmax= f;
  }
  P[ 9]    = rmin;
  P[10]    = rmax;
  P[11]    = zmin;
  P[12]    = zmax;
  P[13]    = P[2]+fmin;
  P[14]    = P[2]+fmax;
  P[15]    = P[4]-P[2]; if(P[15]<-pi) P[15]+=pi2; else if(P[15]>pi) P[15]-=pi2;
  P[16]    = A[0]*x0+A[1]*y0;
  P[17]    = atan2(Ay[1],Ay[0]);
  P[17]    = P[17]-P[2]; if(P[17]<-pi) P[17]+=pi2; else if(P[17]>pi) P[17]-=pi2;

  // Calculation shape parameters
  //
  P[18]    = y0*P[6]-x0*P[5];                       // F  center
  P[19]    = P[8]*(x0*P[6]+y0*P[5])-P[7]*z0;        // ZR center
  P[20]    = +10000.;                               // -F
  P[21]    = 0.     ;                               //
  P[22]    = 0.     ;                               // +ZR
  P[23]    = -10000.;                               //
  P[24]    = -10000.;                               // +F
  P[25]    = 0.     ;                               //
  P[26]    = 0.     ;                               // -ZR
  P[27]    = +10000.;                               //
  //  
  for(int i=0; i!=4; ++i) {

    int   k1    = i;
    int   k2    = i+1; if(k2>3) k2=0;
    double x1   = y[k1]*P[6]-x[k1]*P[5]                  -P[18];                        
    double y1   = P[8]*(x[k1]*P[6]+y[k1]*P[5])-P[7]*z[k1]-P[19];
    double x2   = y[k2]*P[6]-x[k2]*P[5]                  -P[18];                        
    double y2   = P[8]*(x[k2]*P[6]+y[k2]*P[5])-P[7]*z[k2]-P[19];
    double d    = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
    double ax   =-(y2-y1)*(y1*x2-x1*y2)/d;
    double ay   = (x2-x1)*(y1*x2-x1*y2)/d;
    if(ax<P[20]) {P[20]=ax; P[21]=ay;}
    if(ax>P[24]) {P[24]=ax; P[25]=ay;}
    if(ay<P[27]) {P[26]=ax; P[27]=ay;}
    if(ay>P[23]) {P[22]=ax; P[23]=ay;}
  }
  for(int i=15; i!=28; ++i) {if(fabs(P[i])<.0004 ) P[i]=0.;}

  // Directions Phi and Eta in local system coordinates 
  //
  P[29] = Ax[1]*P[6]-Ax[0]*P[5];                
  P[30] = P[8]*(Ax[0]*P[6]+Ax[1]*P[5])-P[7]*Ax[2];
  P[31] = Ay[1]*P[6]-Ay[0]*P[5];                
  P[32] = P[8]*(Ay[0]*P[6]+Ay[1]*P[5])-P[7]*Ay[2];
}

///////////////////////////////////////////////////////////////////
// Distance to detector element according stright line model
///////////////////////////////////////////////////////////////////

double InDet::TRT_DetElementsRoadMaker_xk::stepToDetElement
(const InDetDD::TRT_BaseElement*& de,Amg::Vector3D& r,Amg::Vector3D& a) 
{
  Amg::Vector3D R  = de->center();
  Amg::Vector3D A  = de->normal();
  double      D  = a.x()*A.x()+a.y()*A.y()+a.z()*A.z(); if(D==0.) return D;
  return  ((A.x()*(R.x()-r.x())+A.y()*(R.y()-r.y())+A.z()*(R.z()-r.z()))/D);
}

///////////////////////////////////////////////////////////////////
// Cylinder bounds parameters estimation
///////////////////////////////////////////////////////////////////

Trk::CylinderBounds InDet::TRT_DetElementsRoadMaker_xk::getBound
(const Trk::TrackParameters& Tp)
{
  const double cor = 0.8;

  double zfield = 0.;
  if(m_fieldModeEnum!=Trk::NoField && m_fieldService->solenoidOn()) {
    const Amg::Vector3D& pos = Tp.position();
    double f[3], p[3] ={pos[Amg::x],pos[Amg::y],pos[Amg::z]};
    m_fieldService->getFieldZR(p,f);
    zfield =  299.7925*f[2];
  }

  if( fabs(zfield) < .0000001    ) return m_bounds;

  const AmgVector(5)&  Vp = Tp.parameters();
  
  double cur  = zfield*Vp[4]/sin(Vp[3]);

  if( fabs(cur)*m_bounds.r() < cor ) return m_bounds;

  double rad  = 1./cur;
  if(cor*fabs(rad) > m_bounds.r()  ) return m_bounds;

  const  Amg::Vector3D& Gp = Tp.position()  ;
  double sn,cs; sincos(Vp[2],&sn,&cs);
  double xc   = Gp.x()+sn*rad            ;
  double yc   = Gp.y()-cs*rad            ;
  double rm   = (sqrt(xc*xc+yc*yc)+fabs(rad))*cor;
  if( rm          > m_bounds.r()   ) return m_bounds;
  Trk::CylinderBounds CB(rm,m_bounds.halflengthZ());
  return CB;
}

