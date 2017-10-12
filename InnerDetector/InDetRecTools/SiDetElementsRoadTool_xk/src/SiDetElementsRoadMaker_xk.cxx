/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiDetElementsRoadMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <utility>
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsRoadMaker_xk.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsComparison.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "EventInfo/TagInfo.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiDetElementsRoadMaker_xk::SiDetElementsRoadMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n), 
    m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator" )
{
  m_usePIX      = true                       ;
  m_useSCT      = true                       ;
  m_sct         = "SCT"                      ;
  m_pix         = "Pixel"                    ;
  m_width       = 20.                        ;
  m_step        = 40.                        ;
  m_fieldmode   = "MapSolenoid"              ;

  declareInterface<ISiDetElementsRoadMaker>(this);
  declareProperty("SCTManagerLocation"   ,m_sct        );
  declareProperty("PixManagerLocation"   ,m_pix        );
  declareProperty("RoadWidth"            ,m_width      );
  declareProperty("MaxStep"              ,m_step       );
  declareProperty("PropagatorTool"       ,m_proptool   );
  declareProperty("MagneticFieldMode"    ,m_fieldmode  );
  declareProperty("usePixel"             ,m_usePIX     );
  declareProperty("useSCT"               ,m_useSCT     );
  declareProperty("MagFieldSvc"        , m_fieldServiceHandle);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::SiDetElementsRoadMaker_xk::~SiDetElementsRoadMaker_xk()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::initialize()
{
  StatusCode sc = AlgTool::initialize(); 
  
  if (!m_usePIX && !m_useSCT) {
    msg(MSG::FATAL) << "Please don't call this tool if usePixel and useSCT are false" << endreq;
    return StatusCode::SUCCESS;
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

  // Get propagator tool
  //
  if ( m_proptool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_proptool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_proptool << endreq;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;

  // Map of detector elements production
  //
  mapDetectorElementsProduction(); 

  // get the key -- from StoreGate (DetectorStore)
  //
  std::vector< std::string > tagInfoKeys =  detStore()->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if(tagInfoKeys.size()==0)
    msg(MSG::WARNING) << " No TagInfo keys in DetectorStore "<< endreq;
   else {
     if(tagInfoKeys.size() > 1) {
       msg(MSG::WARNING) <<"More than one TagInfo key in the DetectorStore, using the first one "<< endreq;
     }
     tagInfoKey = tagInfoKeys[0];
   }

  m_callbackString = tagInfoKey;

  const DataHandle<TagInfo> tagInfoH;
  
  // Setup call back for geometry
  //
  sc = detStore()->regFcn(&InDet::SiDetElementsRoadMaker_xk::mapDetectorElementsUpdate,
			  this,tagInfoH,m_callbackString);

  if(sc==StatusCode::SUCCESS) {
      msg(MSG::INFO) << "Registered callback for geometry " << name() << endreq;
   } else {
      msg(MSG::ERROR) << "Could not book callback for geometry " << name () << endreq;
      return StatusCode::FAILURE;
  }


 std::string folder( "/EXT/DCS/MAGNETS/SENSORDATA" );
 const DataHandle<CondAttrListCollection> currentHandle;
 if (m_fieldmode != "NoField" && detStore()->contains<CondAttrListCollection>(folder)){
   
   sc = detStore()->regFcn(&InDet::SiDetElementsRoadMaker_xk::magneticFieldInit,
			   this,currentHandle,folder);
   
   if(sc==StatusCode::SUCCESS) {
     msg(MSG::INFO) << "Registered callback from MagneticFieldSvc for " << name() << endreq;
   } else {
     msg(MSG::ERROR) << "Could not book callback from MagneticFieldSvc for " << name () << endreq;
     return StatusCode::FAILURE;
   }
 }
 else {
   magneticFieldInit();
   ATH_MSG_INFO("Folder " << folder << " not present, magnetic field callback not set up. Not a problem if AtlasFieldSvc.useDCS=False");
 }
 
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}


///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiDetElementsRoadMaker_xk::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out); return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiDetElementsRoadMaker_xk::dumpConditions( MsgStream& out ) const
{
  int n = 62-m_proptool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  n     = 62-m_sct.size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  n     = 62-m_pix.size();
  std::string s6; for(int i=0; i<n; ++i) s6.append(" "); s6.append("|");

  int maps = 0;
  if(m_layer[0].size()) ++maps;
  if(m_layer[1].size()) ++maps;
  if(m_layer[2].size()) ++maps;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<std::endl;
  if(m_useSCT) {
    out<<"| SCT   detector manager  | "<<m_sct             <<s5<<std::endl;
  }
  if(m_usePIX) {
    out<<"| Pixel detector manager  | "<<m_pix             <<s6<<std::endl;
  }
  out<<"| Tool for propagation    | "<<m_proptool.type() <<s1<<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]   <<s3<<std::endl;
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
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements               |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     R     |   Z min    |   Z max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[1].size(); ++i) {
      double zmin = m_layer[1][i].z()-m_layer[1][i].dz();
      double zmax = m_layer[1][i].z()+m_layer[1][i].dz();
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
    out<<"| L.Endcap map containt"
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements              |"
       <<std::endl;

    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[0].size(); ++i) {
      double rmin = m_layer[0][i].r()-m_layer[0][i].dr();
      double rmax = m_layer[0][i].r()+m_layer[0][i].dr();
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
   out<<"| R.Endcap map containt"
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements              |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<std::endl;
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
    for(unsigned int i=0; i!=m_layer[2].size(); ++i) {
      double rmin = m_layer[2][i].r()-m_layer[0][i].dr();
      double rmax = m_layer[2][i].r()+m_layer[0][i].dr();
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

MsgStream& InDet::SiDetElementsRoadMaker_xk::dumpEvent( MsgStream& out ) const
{
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Road size               | "<<std::setw(12)<<m_sizeroad
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiDetElementsRoadMaker_xk::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiDetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiDetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Main methods for road builder using only space points
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::detElementsRoad
(const std::list<const Trk::SpacePoint*>& Sp,
 std::vector<const InDetDD::SiDetectorElement*>& Road)
{
  if ((!m_usePIX && !m_useSCT)) return;
  
  std::list<Amg::Vector3D> G;
  std::list<const Trk::SpacePoint*>::const_iterator s=Sp.begin(),se=Sp.end();
  for(; s!=se; ++s) {G.push_back((*s)->globalPosition());}
  detElementsRoad(G,Road);
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using input list global positions
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::detElementsRoad
(std::list<Amg::Vector3D>& GP,
 std::vector<const InDetDD::SiDetectorElement*>& Road) 
{  
  if (!m_usePIX && !m_useSCT) return;

  int n0 = 0;
  int n1 = 0;
  int n2 = 0;

  std::list<Amg::Vector3D>::iterator g=GP.begin(),ge=GP.end();

  float Po[6] = {float((*g).x()),float((*g).y()),float((*g).z()),
		  float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y())),m_width,0.};

  for(; n0!=m_map[0]; ++n0) {if(Po[2] > m_layer[0][n0].z()) break;}
  for(; n1!=m_map[1]; ++n1) {if(Po[3] < m_layer[1][n1].r()) break;}
  for(; n2!=m_map[2]; ++n2) {if(Po[2] < m_layer[2][n2].z()) break;}

  std::vector<InDet::SiDetElementLink_xk*> lDE;

  ++g; while(g!=ge) {

    float  Pn[4] = {float((*g).x()),float((*g).y()),float((*g).z()),
		    float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y()))};

    float dx = Pn[0]-Po[0];
    float dy = Pn[1]-Po[1];
    float dz = Pn[2]-Po[2];
    float st = sqrt(dx*dx+dy*dy+dz*dz); if(st <=0.) {++g; continue;}
    float ds = 1./st;
    float A[3]= {dx*ds,dy*ds,dz*ds};

    // Min. radius test between this two points
    //    
    float at = A[0]*A[0]+A[1]*A[1];
    float dr = 0.                 ;
    if(at!=0.) {

      float sm = -(A[0]*Po[0]+A[1]*Po[1])/at;
      if(sm > 1. && sm < st) {
	Pn[0]     = Po[0]+A[0]*sm;
	Pn[1]     = Po[1]+A[1]*sm;
	Pn[2]     = Po[2]+A[2]*sm;
	Pn[3]     = sqrt(Pn[0]*Pn[0]+Pn[1]*Pn[1]);
	dr        = 20.;
      }
      else ++g;
    }
    else ++g;

    // Barrel
    //
    if(Pn[3]>Po[3]) {
      for(; n1<m_map[1]; ++n1) {
	if(Pn[3] < m_layer[1][n1].r()) break;
	m_layer[1][n1].getBarrelDetElements(Po,A,lDE);
      }
    }
    else     {
      for(--n1; n1>=0; --n1) {
	if(Pn[3] > m_layer[1][n1].r()+dr) break; 
	m_layer[1][n1].getBarrelDetElements(Po,A,lDE);
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
  std::vector<InDet::SiDetElementLink_xk*>::iterator l=lDE.begin(),le=lDE.end(),n,m;
  if(l==le) return;

  bool nc =true;
  while(nc) {

    nc =false; m=l; n=l;
    for(++n; n!=le; ++n) {
   
      if( (*m)->way() > (*n)->way() ) {
	InDet::SiDetElementLink_xk* d=(*m); (*m)=(*n); (*n)=d; nc=true;
      }
      ++m;
    }
  }

  // Fill list pointers to detector elements
  //
  for(l=lDE.begin(); l!=le; ++l) {

    if(m_test) {if((*l)->way() >= 0.) Road.push_back((*l)->detElement());}
    else                              Road.push_back((*l)->detElement());

    (*l)->clearUsed();
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using track parameters and direction
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::detElementsRoad 
(const Trk::TrackParameters& Tp,Trk::PropDirection D,
 std::vector<const InDetDD::SiDetectorElement*>& R)
{
  if (!m_usePIX && !m_useSCT) return;
  
  double qp   = fabs(500.*Tp.parameters()[4]) ; if( qp < 1.e-10  ) qp = 1.e-10; 
  double S    = m_step/qp                     ; if( S  > 1000.   ) S  = 1000. ;


  m_test = true; if(D<0) {m_test = false; S=-S;}

  std::list<Amg::Vector3D> G;
  m_proptool->globalPositions(G,Tp,m_fieldprop,getBound(Tp),S,Trk::pion);
  if(G.size()<2) return;

  if(D > 0) {
    std::list<Amg::Vector3D>::iterator g=G.begin(),gn,ge=G.end();
    float r0 = (*g).x()*(*g).x()+(*g).y()*(*g).y();

    while(g!=ge) {
      gn = g; if(++gn == ge) break;
      
      float r = (*gn).x()*(*gn).x()+(*gn).y()*(*gn).y();
      
      if(r < r0) {r0 = r; G.erase(g++);} else break;
    }
  }
  detElementsRoad(G,R);  
}


///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::mapDetectorElementsProduction() 
{

  const double pi2=2.*M_PI, pi=M_PI;

  StatusCode sc;

  // Get  Pixel Detector Manager
  //
  const InDetDD::PixelDetectorManager* pixmgr = 0;
  if(m_usePIX) {
    sc = detStore()->retrieve(pixmgr,m_pix);
    if (sc.isFailure() || !pixmgr) {
      msg(MSG::INFO)<<"Could not get PixelDetectorManager  !"<<endreq; 
      return;
    }
  }
  
  // Get  SCT Detector Manager
  //
  const InDetDD::SCT_DetectorManager* sctmgr = 0;
  if(m_useSCT) {
    sc = detStore()->retrieve(sctmgr,m_sct);
    if (sc.isFailure() || !sctmgr) {
      msg(MSG::INFO)<<"Could not get SCT_DetectorManager !"<<endreq; 
      return;
    }
  }

  m_map[0]=m_map[1]=m_map[2]=0;

  const PixelID* IDp = 0; 
  const SCT_ID*  IDs = 0; 

  if (m_usePIX &&  detStore()->retrieve(IDp, "PixelID").isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
  }
  
  if(m_useSCT && detStore()->retrieve(IDs, "SCT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
  }


  if(!IDs && !IDp) return;

  InDetDD::SiDetectorElementCollection::const_iterator s,se;
  std::vector<InDetDD::SiDetectorElement*> pW[3];            

  if(IDp) {

    // Loop over each wafer of pixels
    //
    s  =  pixmgr->getDetectorElementBegin();
    se =  pixmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {

      if     ((*s)->isBarrel()       ) pW[1].push_back((*s)); // Barrel
      else if((*s)->center().z() > 0.) pW[2].push_back((*s)); // Right endcap
      else                             pW[0].push_back((*s)); // Left  endcap
    }
  }

  if(IDs) {

    // Loop over each wafer of sct
    //
    s  = sctmgr->getDetectorElementBegin();
    se = sctmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {

      if     ((*s)->isBarrel()       ) pW[1].push_back((*s)); // Barrel
      else if((*s)->center().z() > 0.) pW[2].push_back((*s)); // Right endcap
      else                             pW[0].push_back((*s)); // Left  endcap
   }
  }

  int nel = pW[0].size()+pW[1].size()+pW[2].size(); if(!nel) return;

  std::sort(pW[1].begin(),pW[1].end(),InDet::compDetElements_RAZ());
  std::sort(pW[0].begin(),pW[0].end(),InDet::compDetElements_ZRA());
  std::sort(pW[2].begin(),pW[2].end(),InDet::compDetElements_ZRA());

  double   mzmin [3];  // min Z coordinate 
  double   mzmax [3];  // max Z coordinate
  double   mrmin [3];  // min radius
  double   mrmax [3];  // max radius
 
  for(int N=0; N!=3; ++N) {

    double P[40];
    int im    = int(pW[N].size()-1); 
    int If    = 0      ;
    double z0 = 0.     ;
    double r0 = 0.     ;
    mrmin[N] = 100000.; mrmax[N] =-100000.;
    mzmin[N] = 100000.; mzmax[N] =-100000.;
    
    for(int i = 0; i<= im; ++i) {
     
      detElementInformation(*(pW[N][i]),P); 

      if( P[ 9] < mrmin[N] ) mrmin[N] = P[ 9]; 
      if( P[10] > mrmax[N] ) mrmax[N] = P[10]; 
      if( P[11] < mzmin[N] ) mzmin[N] = P[11]; 
      if( P[12] > mzmax[N] ) mzmax[N] = P[12]; 
 
      double r    = P[0];
      double z    = P[1];
      bool   newl = false;
      if(N==1) {
	if(fabs(r-r0) > 10.) {newl=true; r0=r;      }
      }
      else     {
	if(fabs(z-z0) >10.) {newl=true; r0=r; z0=z;}
      }

      if(newl || i==im) {
	
	int Il = i-1; if(i==im) ++Il;

	if(If<=Il) {

	  double rmin = 100000., rmax =-100000.;
	  double zmin = 100000., zmax =-100000.;
	  double dfm  = 0.;

	  std::vector<InDetDD::SiDetectorElement*> pE;
	  for(int j=If; j<=Il; ++j) pE.push_back(pW[N][j]);
	  std::sort(pE.begin(),pE.end(),InDet::compDetElements_A());
	  
	  InDet::SiDetElementsLayer_xk layer;

	  for(unsigned int j=0; j!=pE.size(); ++j) {

	    if(pE[j]) {

	      detElementInformation(*(pE[j]),P);

	      if( P[ 9] < rmin ) rmin = P[ 9]; 
	      if( P[10] > rmax ) rmax = P[10]; 
	      if( P[11] < zmin ) zmin = P[11]; 
	      if( P[12] > zmax ) zmax = P[12]; 
	      
	      double df1 = fabs(P[13]-P[2]); if(df1>pi) df1 = fabs(df1-pi2); 
	      double df2 = fabs(P[14]-P[2]); if(df2>pi) df2 = fabs(df2-pi2); 
	      if(df1>dfm) dfm = df1;
	      if(df2>dfm) dfm = df2;
	      
	      InDet::SiDetElementLink_xk link(pE[j],P);
	      layer.add(link);
	    }
	  }
	  double r  =(rmax+rmin)*.5;
	  double dr =(rmax-rmin)*.5; 
	  double z  =(zmax+zmin)*.5;
	  double dz =(zmax-zmin)*.5;
	  layer.set(r,dr,z,dz,dfm);
	  m_layer[N].push_back(layer);
	}
	If = i;
      }
    }
  }
  m_map[0] = m_layer[0].size();
  m_map[1] = m_layer[1].size();
  m_map[2] = m_layer[2].size();

  // CylinderBounds production
  //
  double zmi = +100000;
  double zma = -100000;
  double rma = -100000;
  for(int i=0; i!=3; ++i) {
    if(m_map[i]) {
      if(mzmin[i]<zmi) zmi=mzmin[i]; 
      if(mzmax[i]>zma) zma=mzmax[i]; 
      if(mrmax[i]>rma) rma=mrmax[i];
    }
  }

  double hz = fabs(zma); if(hz<fabs(zmi)) hz = fabs(zmi);
  const Trk::CylinderBounds CB(rma+20.,hz+20.);
  m_bounds  = CB;
}

///////////////////////////////////////////////////////////////////
// Map of detector elements update
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::mapDetectorElementsUpdate
(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  (void) I;

  // check if the string is ESD for guaranteeing that misalignment has been introduced already
  //
  bool needsUpdate = false;

  for (std::list<std::string>::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    if ((*k) == m_callbackString) {needsUpdate = true; break;}
  } 
  if(!needsUpdate) return StatusCode::SUCCESS;

  const double pi2=2.*M_PI, pi=M_PI;

  double   mzmin [3];  // min Z coordinate 
  double   mzmax [3];  // max Z coordinate
  double   mrmin [3];  // min radius
  double   mrmax [3];  // max radius

  // Loop through barrel and two edncap
  //
  for(unsigned int N=0; N!=3; ++N) {

    mrmin[N] = 100000.; mrmax[N] =-100000.;
    mzmin[N] = 100000.; mzmax[N] =-100000.;

    // Loop throug all layers
    //
    for(unsigned int l=0; l!=m_layer[N].size(); ++l) {
      
      std::vector<SiDetElementLink_xk>::iterator d, de = m_layer[N][l].elements().end(); 

      double rmin = 100000., rmax =-100000.;
      double zmin = 100000., zmax =-100000.;
      double dfm  = 0.;

      for(d=m_layer[N][l].elements().begin(); d!=de; ++d) {

	double P[40]; detElementInformation(*(*d).detElement(),P); 
	(*d).set(P);

	if( P[ 9] < mrmin[N]  ) mrmin[N] = P[ 9]; 
	if( P[10] > mrmax[N]  ) mrmax[N] = P[10]; 
	if( P[11] < mzmin[N]  ) mzmin[N] = P[11]; 
	if( P[12] > mzmax[N]  ) mzmax[N] = P[12]; 
	if( P[ 9] <   rmin    )  rmin    = P[ 9]; 
	if( P[10] >   rmax    )  rmax    = P[10]; 
	if( P[11] <   zmin    )  zmin    = P[11]; 
	if( P[12] >   zmax    )  zmax    = P[12]; 

	double df1 = fabs(P[13]-P[2]); if(df1>pi) df1 = fabs(df1-pi2); 
	double df2 = fabs(P[14]-P[2]); if(df2>pi) df2 = fabs(df2-pi2); 
	if(df1>dfm) dfm = df1;
	if(df2>dfm) dfm = df2;
      }
      m_layer[N][l].sortDetectorElements();

      double r  =(rmax+rmin)*.5;
      double dr =(rmax-rmin)*.5; 
      double z  =(zmax+zmin)*.5;
      double dz =(zmax-zmin)*.5;
      m_layer[N][l].set(r,dr,z,dz,dfm);
    }
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

void InDet::SiDetElementsRoadMaker_xk::detElementInformation
(const InDetDD::SiDetectorElement& Si,double* P) 
{
  const double pi2=2.*M_PI, pi= M_PI;
  
  double Sl    = .5*Si.design().length  ();
  double Swmax = .5*Si.design().maxWidth();
  double Swmin = .5*Si.design().minWidth();
  
  // Thickness of the waver
  //
  P[28] = Si.design().thickness();
  Amg::Vector3D C  = Si.center();  double x0=C.x(), y0=C.y(), z0=C.z(); 
  Amg::Vector3D AT = Si.normal ();
  Amg::Vector3D AF = Si.phiAxis();
  Amg::Vector3D AE = Si.etaAxis();

  double      A[3]={AT.x(),AT.y(),AT.z()};   

  P[ 0]    = sqrt(x0*x0+y0*y0);
  P[ 1]    = z0;   
  P[ 2]    = atan2(y0,x0);
  P[ 3]    = A[0]*x0+A[1]*y0+A[2]*z0;
  if(P[3]<0.) {P[3]*=-1.; A[0]*=-1.; A[1]*=-1.; A[2]*=-1.;} 
  P[ 7]    =  sqrt(A[0]*A[0]+A[1]*A[1]);

  if(P[7] >.000001) P[4]=atan2(A[1],A[0]); else P[4]=P[2]; 
  sincos(P[4],&P[5],&P[6]);
  P[ 8]    = A[2];                      

  if( fabs(P[7]) <= .000001) {

    P[3] = fabs(P[1]); 
    if(P[8] > 0.) P[8] = 1.; else P[8] = -1.; 
    P[7] = 0.;
  }

  double x[4],y[4],z[4];

  x[0]     = x0+AF.x()*Swmax+AE.x()*Sl;
  y[0]     = y0+AF.y()*Swmax+AE.y()*Sl;
  z[0]     = z0+AF.z()*Swmax+AE.z()*Sl;

  x[1]     = x0+AF.x()*Swmin-AE.x()*Sl;
  y[1]     = y0+AF.y()*Swmin-AE.y()*Sl;
  z[1]     = z0+AF.z()*Swmin-AE.z()*Sl;

  x[2]     = x0-AF.x()*Swmin-AE.x()*Sl;
  y[2]     = y0-AF.y()*Swmin-AE.y()*Sl;
  z[2]     = z0-AF.z()*Swmin-AE.z()*Sl;

  x[3]     = x0-AF.x()*Swmax+AE.x()*Sl;
  y[3]     = y0-AF.y()*Swmax+AE.y()*Sl;
  z[3]     = z0-AF.z()*Swmax+AE.z()*Sl;

  double rmin= 10000., rmax=-10000.;
  double zmin= 10000., zmax=-10000.;
  double fmin= 10000., fmax=-10000.;
  for(int i=0; i!=4; ++i) {
    double r = sqrt(x[i]*x[i]+y[i]*y[i]);
    double f = atan2(y[i],x[i])-P[2]; if(f<-pi) f+=pi2; else if(f>pi) f-=pi2;
    double zf= z[i];
    if(r <rmin) rmin= r; if(r >rmax) rmax= r;
    if(zf<zmin) zmin=zf; if(zf>zmax) zmax=zf;
    if(f <fmin) fmin= f; if(f >fmax) fmax= f;
  }
  P[ 9]    = rmin;
  P[10]    = rmax;
  P[11]    = zmin;
  P[12]    = zmax;
  P[13]    = P[2]+fmin;
  P[14]    = P[2]+fmax;
  P[15]    = P[4]-P[2]; if(P[15]<-pi) P[15]+=pi2; else if(P[15]>pi) P[15]-=pi2;
  P[16]    = A[0]*x0+A[1]*y0;
  P[17]    = atan2(AE.y(),AE.x());
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

  // Directions Phi and Eta in local system coordinates 
  //
  P[29] = AF.y()*P[6]-AF.x()*P[5];                
  P[30] = P[8]*(AF.x()*P[6]+AF.y()*P[5])-P[7]*AF.z();
  P[31] = AE.y()*P[6]-AE.x()*P[5];                
  P[32] = P[8]*(AE.x()*P[6]+AE.y()*P[5])-P[7]*AE.z();
}

///////////////////////////////////////////////////////////////////
// Distance to detector element according stright line model
///////////////////////////////////////////////////////////////////

float InDet::SiDetElementsRoadMaker_xk::stepToDetElement
(const InDetDD::SiDetectorElement*& de,Amg::Vector3D& r,Amg::Vector3D& a) 
{
  Amg::Vector3D R  = de->center();
  Amg::Vector3D A  = de->normal();
  double      D  = a.x()*A.x()+a.y()*A.y()+a.z()*A.z(); if(D==0.) return float(D);
  return  float((A.x()*(R.x()-r.x())+A.y()*(R.y()-r.y())+A.z()*(R.z()-r.z()))/D);
}

///////////////////////////////////////////////////////////////////
// Callback function - get the magnetic field /
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::magneticFieldInit(IOVSVC_CALLBACK_ARGS) 
{
  // Build MagneticFieldProperties 
  //
  if(!m_fieldService->solenoidOn()) m_fieldmode ="NoField"; magneticFieldInit();
  return StatusCode::SUCCESS;
}

void InDet::SiDetElementsRoadMaker_xk::magneticFieldInit() 
{
  // Build MagneticFieldProperties 
  //
  Trk::MagneticFieldProperties* pMF = 0;
  if     (m_fieldmode == "NoField"    ) pMF = new Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField);
  else                                  pMF = new Trk::MagneticFieldProperties(Trk::FullField);
  m_fieldprop = *pMF; delete pMF;

  // Test is filed or no
  //
  m_zfield = 0.;
  if(m_fieldprop.magneticFieldMode()!=Trk::NoField) {

    double f[3], p[3] ={10.,10.,0.}; m_fieldService->getFieldZR(p,f);
    m_zfield =  299.7925*f[2];
  }
}

///////////////////////////////////////////////////////////////////
// Cylinder bounds parameters estimation
///////////////////////////////////////////////////////////////////

Trk::CylinderBounds InDet::SiDetElementsRoadMaker_xk::getBound
(const Trk::TrackParameters& Tp)
{
  const double cor = 1.;

  if( fabs(m_zfield) < .0000001    ) return m_bounds;

  const AmgVector(5)& Vp = Tp.parameters();
  
  double cur  = m_zfield*Vp[4]/sin(Vp[3]);

  if( fabs(cur)*m_bounds.r() < cor ) return m_bounds;

  double rad  = 1./cur;
  if(cor*fabs(rad) > m_bounds.r()  ) return m_bounds;

  const  Amg::Vector3D& Gp = Tp.position()  ;

  double S,C; sincos(Vp[2],&S,&C);

  double xc   = Gp.x()+S*rad;
  double yc   = Gp.y()-C*rad;
  double rm   = (sqrt(xc*xc+yc*yc)+fabs(rad))*cor;
  if( rm          > m_bounds.r()   ) return m_bounds;
  Trk::CylinderBounds CB(rm,m_bounds.halflengthZ());
  return CB;
}

