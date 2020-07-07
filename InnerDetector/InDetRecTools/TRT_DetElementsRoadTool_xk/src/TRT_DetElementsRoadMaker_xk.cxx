/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "GeoModelInterfaces/IGeoModelTool.h"

#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "TRT_DetElementsRoadUtils_xk.h"

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsRoadMaker_xk.h"
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsComparison.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DetElementsRoadMaker_xk::TRT_DetElementsRoadMaker_xk
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p)                                          ,
    m_proptool   ("Trk::RungeKuttaPropagator/InDetPropagator" )
{
  m_width       = 10.                        ;
  m_step        = 20.                        ;
  m_fieldmode   = "MapSolenoid"              ;

  declareInterface<ITRT_DetElementsRoadMaker>(this);
  declareProperty("RoadWidth"            ,m_width      );
  declareProperty("MaxStep"              ,m_step       );
  declareProperty("PropagatorTool"       ,m_proptool   );
  declareProperty("MagneticFieldMode"    ,m_fieldmode  );
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

  if(m_fieldmode == "NoField") m_fieldModeEnum = Trk::NoField;
  else if(m_fieldmode == "MapSolenoid") m_fieldModeEnum = Trk::FastField;
  else m_fieldModeEnum = Trk::FullField;

  // Get propagator tool
  if (m_proptool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_proptool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool " << m_proptool);
  }

  ATH_CHECK(m_roadDataKey.initialize());
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

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
  SG::ReadCondHandle<AtlasFieldCacheCondObj> fieldHandle(m_fieldCacheCondObjInputKey);
  const AtlasFieldCacheCondObj* fieldCondObj{*fieldHandle};
  if (fieldCondObj) {
    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);
    if(!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  }
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);
  int mode = fieldprop.magneticFieldMode();
  if(mode<0 || mode>8 ) mode = 8;

  n     = 62-fieldmode[mode].size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");

  const TRT_DetElementsLayerVectors_xk &layer = *getLayers();

  int maps = 0;
  if(layer[0].size()) ++maps;
  if(layer[1].size()) ++maps;
  if(layer[2].size()) ++maps;

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

  if(!maps || !msgLvl(MSG::VERBOSE)) return out;

  if(layer[1].size()) {
    int nl = layer[1].size();
    int nc = 0;
    for(unsigned int i=0; i!=layer[1].size(); ++i) nc+=layer[1][i].nElements();
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
    for(unsigned int i=0; i!=layer[1].size(); ++i) {
      float zmin = layer[1][i].z()-layer[1][i].dz();
      float zmax = layer[1][i].z()+layer[1][i].dz();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[1][i].r ()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<< layer[1][i].dfe()<<" | "
	 <<std::setw(4)<<layer[1][i].nElements()<<" | "
	 <<std::endl;
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;

  }
  if(layer[0].size()) {

    int nl = layer[0].size();
    int nc = 0;
    for(unsigned int i=0; i!=layer[0].size(); ++i) nc+=layer[0][i].nElements();
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
    for(unsigned int i=0; i!=layer[0].size(); ++i) {
      float rmin = layer[0][i].r()-layer[0][i].dr();
      float rmax = layer[0][i].r()+layer[0][i].dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[0][i].z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<layer[0][i].dfe()<<" | "
	 <<std::setw(4)<<layer[0][i].nElements()<<" | "
	 <<std::endl;
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<std::endl;
  }
  if(layer[2].size()) {
    int nl = layer[2].size();
    int nc = 0;
    for(unsigned int i=0; i!=layer[2].size(); ++i) nc+=layer[2][i].nElements();
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
    for(unsigned int i=0; i!=layer[2].size(); ++i) {
      float rmin = layer[2][i].r()-layer[0][i].dr();
      float rmax = layer[2][i].r()+layer[0][i].dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[2][i].z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<layer[2][i].dfe()<<" | "
	 <<std::setw(4)<<layer[2][i].nElements()<<" | "
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

MsgStream& InDet::TRT_DetElementsRoadMaker_xk::dumpEvent( MsgStream& out, int size_road) const
{
  out<<"|--------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Road size               | "<<std::setw(12)<<size_road
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
// Main methods for road builder
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoad
(const EventContext& ctx,
 MagField::AtlasFieldCache& fieldCache,
 const Trk::TrackParameters& Tp,Trk::PropDirection D,
 std::vector<const InDetDD::TRT_BaseElement*>& R) const
{

  double qp   = fabs(500.*Tp.parameters()[4]) ; if( qp < 1.e-10  ) qp = 1.e-10;
  double S    = m_step/qp                     ; if( S  > 200.    ) S  = 200.  ; if(D<0) S=-S;

  Trk::CylinderBounds CB = getBound(fieldCache, Tp);

  double rminTRT = getTRTMinR();

  if( CB.r() > rminTRT) {
    Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
    if(!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
    Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

    std::list<Amg::Vector3D> G;
    m_proptool->globalPositions(ctx, G,Tp,fieldprop,CB,S,Trk::pion);

    if(G.size() > 1 ) {
      detElementsRoadATL(G,R);
    }
  }

  if (msgLvl(MSG::VERBOSE)) {
    dumpEvent(msg(MSG::VERBOSE),R.size());
    dumpConditions(msg(MSG::VERBOSE));
    msg(MSG::VERBOSE) << endmsg;
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoad
(const EventContext& ctx,
 MagField::AtlasFieldCache& fieldCache,
 const Trk::TrackParameters& Tp,
 Trk::PropDirection D,
 std::vector<std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> >& R) const
{
 std::vector<const InDetDD::TRT_BaseElement*> RE;
 detElementsRoad(ctx, fieldCache, Tp,D,RE);

 if (msgLvl(MSG::VERBOSE)) {
    dumpEvent(msg(MSG::VERBOSE), R.size());
    dumpConditions(msg(MSG::VERBOSE));
    msg(MSG::VERBOSE) << endmsg;
 }

 std::vector<const InDetDD::TRT_BaseElement*>::const_iterator r=RE.begin(),re=RE.end();
 if(r==re) return;

 Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
 if(!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
 Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

 const Trk::TrackParameters* tp0 =
   m_proptool->propagate(ctx, Tp,(*r)->surface(),D,false,fieldprop,Trk::pion);
 if(!tp0) return;

 std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> EP0((*r),tp0);
 R.push_back(EP0);

 for(++r; r!=re; ++r) {

   const Trk::TrackParameters* tp =
     m_proptool->propagate(ctx, (*tp0),(*r)->surface(),D,false,fieldprop,Trk::pion);
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
 std::vector<const InDetDD::TRT_BaseElement*>& Road) const
{
  int n0     = 0;
  int n1     = 0;
  int n2     = 0;
  std::list<Amg::Vector3D>::iterator g=GP.begin(),ge=GP.end();

  const TRT_DetElementsLayerVectors_xk &layer = *getLayers();

  float Po[6] = {float((*g).x()),float((*g).y()),float((*g).z()),
		 float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y())),m_width,0.};

  for(; n0!=(int)layer[0].size(); ++n0) {if(Po[2] > layer[0][n0].z()) break;}
  for(; n1!=(int)layer[1].size(); ++n1) {if(Po[3] < layer[1][n1].r()) break;}
  for(; n2!=(int)layer[2].size(); ++n2) {if(Po[2] < layer[2][n2].z()) break;}

  std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> > lDE;
  std::array<std::vector<std::vector<InDet::TRT_DetElementLink_xk::Used_t> >,3> used;
  for ( unsigned int module_i=0; module_i<3; ++module_i) {
     used[module_i].resize( layer[module_i].size() );
     for (unsigned int layer_i=0; layer_i < layer[module_i].size(); ++layer_i) {
        used[module_i][layer_i].resize( layer[module_i][layer_i].nElements() );
     }
  }

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
      for(; n1<(int)layer[1].size(); ++n1) {

	if(Pn[3] < layer[1][n1].r()) break;
        assert( used.at(1).size() > static_cast<unsigned int>(n1) );
	layer[1][n1].getBarrelDetElementsATL(Po,A,lDE,used[1][n1]);

      }
    }
    else     {
      for(--n1; n1>=0; --n1) {
	if(Pn[3] > layer[1][n1].r()) break;
        assert( used.at(1).size() > static_cast<unsigned int>(n1) );
	layer[1][n1].getBarrelDetElementsATL(Po,A,lDE,used[1][n1]);
     }
      ++n1;
    }

    // Positive endcap
    //
    if(Pn[2]>Po[2]) {

      for(; n2<(int)layer[2].size(); ++n2) {
	if(Pn[2] < layer[2][n2].z()) break;
        assert( used.at(2).size()> static_cast<unsigned int>(n2));
	layer[2][n2].getEndcapDetElements(Po,A,lDE,used[2][n2]);
      }
    }
    else     {
      for(--n2; n2>=0; --n2) {
	if(Pn[2] > layer[2][n2].z()) break;
        assert( used.at(2).size() > static_cast<unsigned int>(n2));
	layer[2][n2].getEndcapDetElements(Po,A,lDE,used[2][n2]);
      }
      ++n2;
    }

    // Negative endcap
    //
    if(Pn[2]<Po[2]) {

      for(; n0<(int)layer[0].size(); ++n0) {
	if(Pn[2] > layer[0][n0].z()) break;
        assert( used.at(0).size() > static_cast<unsigned int>(n0));
	layer[0][n0].getEndcapDetElements(Po,A,lDE,used[0][n0]);
      }
    }
     else   {
      for(--n0; n0>=0; --n0) {
	if(Pn[2] < layer[0][n0].z()) break;
        assert( used.at(0).size() > static_cast<unsigned int>(n0));
	layer[0][n0].getEndcapDetElements(Po,A,lDE,used[0][n0]);
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
  std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> >::iterator l=lDE.begin(),le=lDE.end(),n,m;
  if(l==le) return;

  bool nc =true;
  while(nc) {

    nc =false; m=l; n=l;
    for(++n; n!=le; ++n) {

      if( (*m).second > (*n).second ) {
         std::pair<const InDet::TRT_DetElementLink_xk*,float>  d=(*m); (*m)=(*n); (*n)=d; nc=true;
      }
      ++m;
    }
  }

  // Fill list pointers to detector elements
  //
  for(l=lDE.begin(); l!=le; ++l) {
    Road.push_back((*l).first->detElement());
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using input list global positions
// for CTB geometry
///////////////////////////////////////////////////////////////////

void InDet::TRT_DetElementsRoadMaker_xk::detElementsRoadCTB
(std::list<Amg::Vector3D>& GP,
 std::vector<const InDetDD::TRT_BaseElement*>& Road) const
{
  int n1     = 0;
  std::list<Amg::Vector3D>::iterator g=GP.begin(),ge=GP.end();

  const TRT_DetElementsLayerVectors_xk &layer = *getLayers();

  float Po[6] = {float((*g).x()),float((*g).y()),float((*g).z()),
		 float(sqrt((*g).x()*(*g).x()+(*g).y()*(*g).y())),m_width,0.};

  for(; n1!=(int)layer[1].size(); ++n1) {if(Po[3] < layer[1][n1].r()) break;}

  std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> > lDE;
  std::array<std::vector<std::vector<InDet::TRT_DetElementLink_xk::Used_t> >,3> used;
  for ( unsigned int module_i=0; module_i<3; ++module_i) {
     used[module_i].resize( layer[module_i].size() );
     for (unsigned int layer_i=0; layer_i < layer[module_i].size(); ++layer_i) {
        used[module_i][layer_i].resize( layer[module_i][layer_i].nElements() );
     }
  }

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
      for(; n1<(int)layer[1].size(); ++n1) {
	if(Pn[3] < layer[1][n1].r()) break;
        assert( used.at(1).size() > static_cast<unsigned int>(n1) );
	layer[1][n1].getBarrelDetElementsCTB(Po,A,lDE,used[1][n1]);
      }
    }
    else     {
      for(--n1; n1>=0; --n1) {
	if(Pn[3] > layer[1][n1].r()) break;
	layer[1][n1].getBarrelDetElementsCTB(Po,A,lDE,used[1][n1]);
      }
      ++n1;
    }
  }

  // Sort list in propogation order
  //
  std::vector<std::pair<const InDet::TRT_DetElementLink_xk*, float> >::iterator l=lDE.begin(),le=lDE.end(),n;
  if(l==le) return;

  bool nc =true;
  while(nc) {

    nc =false; n=l;
    for(++n; n!=le; ++n) {

      if( (*l).second > (*n).second ) {
         std::pair<const InDet::TRT_DetElementLink_xk*,float> d = (*l); (*l) = (*n); (*n) = d;
         nc = true;
      }
      ++l;
    }
  }

  // Fill list pointers to detector elements
  //
  for(l=lDE.begin(); l!=le; ++l) {
    Road.push_back((*l).first->detElement());
  }
}

///////////////////////////////////////////////////////////////////
// Distance to detector element according stright line model
///////////////////////////////////////////////////////////////////

double InDet::TRT_DetElementsRoadMaker_xk::stepToDetElement
(const InDetDD::TRT_BaseElement*& de,Amg::Vector3D& r,Amg::Vector3D& a) const
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
(MagField::AtlasFieldCache& fieldCache, const Trk::TrackParameters& Tp) const
{
  const double cor = 0.8;

  double zfield = 0.;
  if(m_fieldModeEnum!=Trk::NoField && fieldCache.solenoidOn()) {
    const Amg::Vector3D& pos = Tp.position();
    double f[3], p[3] ={pos[Amg::x],pos[Amg::y],pos[Amg::z]};
    fieldCache.getFieldZR  (p, f);
    zfield =  299.7925*f[2];
  }

  const Trk::CylinderBounds bounds = get_bounds();

  if( fabs(zfield) < .0000001    ) return bounds;

  const AmgVector(5)&  Vp = Tp.parameters();

  double cur  = zfield*Vp[4]/sin(Vp[3]);

  if( fabs(cur)*bounds.r() < cor ) return bounds;

  double rad  = 1./cur;
  if(cor*fabs(rad) > bounds.r()  ) return bounds;

  const  Amg::Vector3D& Gp = Tp.position()  ;
  double sn,cs; sincos(Vp[2],&sn,&cs);
  double xc   = Gp.x()+sn*rad            ;
  double yc   = Gp.y()-cs*rad            ;
  double rm   = (sqrt(xc*xc+yc*yc)+fabs(rad))*cor;
  if( rm          > bounds.r()   ) return bounds;
  Trk::CylinderBounds CB(rm,bounds.halflengthZ());
  return CB;
}
