/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrackExtensionToolCosmics
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


#include <iostream>
#include <iomanip>
#include <utility>

#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionToolCosmics.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionToolCosmics::TRT_TrackExtensionToolCosmics
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_riontrackD("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackToolUniversal"                      ),
    m_riontrackN("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool")
{
  m_trtname         = "TRT_DriftCircles";
  m_trtmanager      = "TRT"             ;
  m_minNumberDCs    = 9                 ;
  m_roadwidth       = 10.               ;
  m_roadwidth_locz  = 10.               ;

  declareInterface<ITRT_TrackExtensionTool>(this);

  declareProperty("RIOonTrackToolYesDr"  ,m_riontrackD     );
  declareProperty("RIOonTrackToolNoDr"   ,m_riontrackN     );
  declareProperty("Extrapolator"         ,m_extrapolator   );
  declareProperty("Propagator"           ,m_propagator     );

  declareProperty("TrtManagerLocation"   ,m_trtmanager     );
  declareProperty("RoadWidth"            ,m_roadwidth      );
  declareProperty("BoundaryLocZTolerance",m_roadwidth_locz );
  declareProperty("SearchNeighbour"      ,m_searchNeighbour=false);
  declareProperty("MinNumberDriftCircles",m_minNumberDCs   );
  declareProperty("TRT_ClustersContainer",m_trtname        );
  declareProperty("BoundaryCheck"        ,m_boundarycheck=false);

  m_trtcylinder=0;
  m_trtdiscA=0;
  m_trtdiscC=0;
 }

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionToolCosmics::~TRT_TrackExtensionToolCosmics()
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionToolCosmics::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // Get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endreq;
    return StatusCode::FAILURE;
  }


  // Get RIO_OnTrack creator with drift time information
  //
  if(m_riontrackD.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackD <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackD << endreq;
  }

  // Get RIO_OnTrack creator without drift time information
  //
  if(m_riontrackN.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackN <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackN << endreq;
  }

  if ((detStore()->retrieve(m_trtid)).isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endreq;
    return StatusCode::FAILURE;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
  return sc;


  

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionToolCosmics::finalize()
{
   StatusCode sc = AlgTool::finalize(); 

   if(m_trtcylinder) delete m_trtcylinder;
   if(m_trtdiscA) delete m_trtdiscA;
   if(m_trtdiscC) delete m_trtdiscC;

   return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionToolCosmics::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)
    return dumpEvent(out);
  else
    return dumpConditions(out);
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionToolCosmics::dumpConditions( MsgStream& out ) const
{

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionToolCosmics::dumpEvent( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackExtensionToolCosmics::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrackExtensionToolCosmics& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrackExtensionToolCosmics& se)
{
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Track extension initiation
///////////////////////////////////////////////////////////////////

void InDet::TRT_TrackExtensionToolCosmics::newEvent()
{
  //create the boundary surfaces
  //
  Amg::RotationMatrix3D r; r.setIdentity(); 
  Amg::Transform3D* t = 0;

  if(!m_trtcylinder) {
    t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D::Zero()));
    m_trtcylinder= new Trk::CylinderSurface(t,1150.,3000.);
  }
  if(!m_trtdiscA   ) {
    t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D(0.,0.,3000)));
    m_trtdiscA   = new Trk::DiscSurface    (t,1.,1200.);
  }
  if(!m_trtdiscC) {
    t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D(0.,0.,-3000)));
    m_trtdiscC   = new Trk::DiscSurface    (t,1.,1200.);
  }
  m_trtcontainer = 0;
  StatusCode sc = evtStore()->retrieve(m_trtcontainer,m_trtname); 
  if(sc.isFailure() && m_outputlevel<=0) {
    msg(MSG::DEBUG)<<"Could not get TRT_DriftCircleContainer"<<endreq;
  }
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>& 
InDet::TRT_TrackExtensionToolCosmics::extendTrack(const Trk::Track& Tr)
{ 
  m_measurement.erase(m_measurement.begin(), m_measurement.end());

  if(!m_trtcontainer) return m_measurement;

  const DataVector<const Trk::TrackStateOnSurface>* 
    tsos = Tr.trackStateOnSurfaces();

  const Trk::TrackParameters* 
    par = (*(tsos->rbegin()))->trackParameters(); if(!par ) return m_measurement;
  const Trk::TrackParameters* 
    parb = (*(tsos->begin()))->trackParameters();

  
  if(parb && par!=parb) {

    const Amg::Vector3D& g1 = par ->position();
    const Amg::Vector3D& g2 = parb->position();
    if((g2.x()*g2.x()+g2.y()*g2.y()) > (g1.x()*g1.x()+g1.y()*g1.y())) par=parb;
  }

  if(Tr.perigeeParameters()) {
    return extendTrack(*Tr.perigeeParameters());
  }
  m_measurement.erase(m_measurement.begin(), m_measurement.end());
  return m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////
void InDet::TRT_TrackExtensionToolCosmics::analyze_tpars(const std::vector<const Trk::TrackParameters* >* tpars)
{
  msg(MSG::DEBUG)<<"Number of tpars: "<<tpars->size()<<endreq;
  
  double lastz=-99999;
  std::vector< const Trk::TrackParameters* >::const_iterator parameterIter = tpars->begin();
  for ( ; parameterIter != tpars->end(); ++parameterIter) {
    msg(MSG::DEBUG)<< "par pos: " << (**parameterIter).position() <<endreq;
      
    if ( (*parameterIter)->associatedSurface().associatedDetectorElementIdentifier()==0 ) {
      msg(MSG::DEBUG)<<"No DE identifier!!!"<<endreq;
      continue; 
    }

    const Identifier& DCId = (*parameterIter)->associatedSurface().associatedDetectorElementIdentifier();
    if ( m_trtid->is_trt( DCId ) ) {
      
      IdentifierHash detElements[3];
      detElements[1]=(*parameterIter)->associatedSurface().associatedDetectorElement()->identifyHash();
      if(m_searchNeighbour){
	//fill entry 0 and 2 with the neighbours (different in phi but identical in layer)
	int bec=m_trtid->barrel_ec(DCId);
	int phi_mod=m_trtid->phi_module(DCId);
	int layer_or_wheel=m_trtid->layer_or_wheel(DCId);
	int slay=m_trtid->straw_layer(DCId);
	
	phi_mod-=1;
	if(phi_mod<0) phi_mod=31;
	
	Identifier temp=m_trtid->straw_id(bec,phi_mod,layer_or_wheel,slay,0);
	detElements[0]=m_trtid->straw_layer_hash(temp);
	
	phi_mod+=2;
	if(phi_mod>31) phi_mod=0;
	temp=m_trtid->straw_id(bec,phi_mod,layer_or_wheel,slay,0);
	detElements[2]=m_trtid->straw_layer_hash(temp);
      }
      double maxdist=m_roadwidth;
      const InDet::TRT_DriftCircle *circ=NULL;
      
      for(int i=-1;i<2;i++) {
	if(m_searchNeighbour || i==0){	    
	  
	  //check if this PRD exists
	  // get the driftCircleCollection belonging to this id
	  InDet::TRT_DriftCircleContainer::const_iterator containerIterator = m_trtcontainer->indexFind(detElements[i+1]);
	  
	  if(containerIterator==m_trtcontainer->end()) {
	    msg(MSG::DEBUG)<<"for the current detectorElement no DriftCircleContainer seems to exist: "<<m_trtid->show_to_string(m_trtid->layer_id(detElements[i+1]))<<endreq;
	    continue;
	  }
	  
	  msg(MSG::DEBUG)<< "There are "  << (*containerIterator)->size() << " entries in the TRT_DriftCircleCollection "<<m_trtid->show_to_string(m_trtid->layer_id(detElements[i+1])) <<endreq;
	  
	  //take the closest one in case it satisfies some default cuts
	  InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = (*containerIterator)->begin();
	  for (; driftCircleIterator != (*containerIterator)->end(); driftCircleIterator++) {

	    //get the associated surface of the driftcircle
	    const Trk::Surface &dc_surface=(*driftCircleIterator)->detectorElement()->surface((*driftCircleIterator)->identify());

	    //get the local position of the track prediction in the frame of the driftcircle
	    const Amg::Vector2D *lpos=dc_surface.globalToLocal((*parameterIter)->position());

	    double distance=m_roadwidth+1;
	    if(lpos){
	      distance = fabs(lpos->x());
	      msg(MSG::DEBUG)<<"Hit "<<m_trtid->show_to_string((*driftCircleIterator)->identify())<<" has a distance of "<<distance<<endreq;

	      double dist_locz=fabs(lpos->y());
	      if(distance<m_roadwidth+1){
		if(!dc_surface.insideBounds(*lpos,m_roadwidth,m_roadwidth_locz)){
		  msg(MSG::DEBUG)<<"Hit not inside surface bounds! "<<distance<<" , "<<dist_locz<<endreq;
		  msg(MSG::DEBUG)<<"\trejecting hit"<<endreq;
		  distance=m_roadwidth+1;
		}
	      }

	      delete lpos;
	    }

	    if(distance<maxdist){
	      maxdist=distance;
	      circ=(*driftCircleIterator);
	      //}else{
	      //  msg(MSG::DEBUG)<<"Driftcircle "<<m_trtid->show_to_string((*driftCircleIterator)->identify())<<" has a distance of "<<distance<<endreq;
	    }
	  }
	}
      }
      msg(MSG::DEBUG)<<"Maximal distance: "<<maxdist<<endreq;
      if(circ){
	msg(MSG::DEBUG)<<"Found Driftcircle! Adding it to list ..."<<m_trtid->show_to_string(circ->identify())<<endreq;
        if (lastz<-9999) lastz=(**parameterIter).position().z();
        if (fabs(lastz-(**parameterIter).position().z())>500.) return;
        lastz=(**parameterIter).position().z();
        const Trk::StraightLineSurface *slsurf=dynamic_cast<const Trk::StraightLineSurface *>(&circ->detectorElement()->surface(circ->identify())); if(!slsurf) continue;
        Trk::AtaStraightLine atasl((**parameterIter).position(),(**parameterIter).parameters()[Trk::phi],(**parameterIter).parameters()[Trk::theta],(**parameterIter).parameters()[Trk::qOverP],*slsurf);
        const Trk::MeasurementBase *newmeas=m_riontrackN->correct(*circ,atasl); 
	m_measurement.push_back(newmeas);
        //std::cout << "param pos: " << (**parameterIter).position() << " meas pos: " << newmeas->globalPosition() << std::endl;

      }	  
    }
  }
}

namespace InDet{
class tp_sort_cosmics{
          public:
          tp_sort_cosmics(double theta){m_theta=theta;}
          bool operator()(const Trk::TrackParameters *par1,const Trk::TrackParameters *par2){
            if (m_theta>M_PI/2) return (par1->position().z()>par2->position().z());
            else return (par1->position().z()<par2->position().z());
          }
          private:
          double m_theta;
	};
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>& 
InDet::TRT_TrackExtensionToolCosmics::extendTrack(const Trk::TrackParameters& par)
{ 
 
  m_measurement.erase(m_measurement.begin(), m_measurement.end());

  std::vector<Identifier> vecID;
  std::vector<const Trk::TrackParameters*> vecTP;



  std::vector<const Trk::TrackParameters* >* tpars_down=0;
  std::vector<const Trk::TrackParameters* >* tpars_up=0;
const Trk::Perigee *per=dynamic_cast<const Trk::Perigee *>(&par);
InDet::TRT_DriftCircleContainer::const_iterator
   w = m_trtcontainer->begin(),we = m_trtcontainer->end();
   for(; w!=we; ++w) {
     if ((**w).empty()) continue; 
     const Trk::Surface &surf=(**(**w).begin()).detectorElement()->surface();
     Amg::Vector3D pos=intersect(&surf,per);
     //std::cout << "pos: " << pos << " surf: " << surf << std::endl;
     Amg::Vector3D locintersec = (surf.transform().inverse())*pos;
     Amg::Vector2D locpos(locintersec.x(), locintersec.y());
     if (pos.perp()<500. || !surf.insideBounds(locpos,50.,50.)) continue;

     Amg::Vector3D pos2=surf.transform()*Amg::Vector3D(locintersec.x(),locintersec.y(),0);

     const Trk::PlaneSurface *plsurf=dynamic_cast<const Trk::PlaneSurface *>(&surf);
     const Trk::DiscSurface *discsurf=dynamic_cast<const Trk::DiscSurface *>(&surf);
     Trk::TrackParameters *newpar=0;
     if (plsurf) newpar=new Trk::AtaPlane(pos2,per->parameters()[Trk::phi],per->parameters()[Trk::theta],per->parameters()[Trk::qOverP],*plsurf);
     else newpar=new Trk::AtaDisc(pos2,per->parameters()[Trk::phi],per->parameters()[Trk::theta],per->parameters()[Trk::qOverP],*discsurf); 
     //std::cout << "pos: " << pos << " pos2: " << pos2 << " param pos: " << newpar->position() << std::endl; 
     vecTP.push_back(newpar);
   }
  tpars_down=new std::vector<const Trk::TrackParameters* >;
  tpars_up=new std::vector<const Trk::TrackParameters* >;

  if(!tpars_down ||  !tpars_up) return m_measurement;

  tp_sort_cosmics sorter(per->parameters()[Trk::theta]);
  std::sort(vecTP.begin(),vecTP.end(),sorter);  
  for (int i=0;i<(int)vecTP.size();i++){ 
    const Trk::TrackParameters *tmppar=vecTP[i];
    if ((per->parameters()[Trk::theta]>M_PI/2 && per->position().z()<tmppar->position().z()) || (per->parameters()[Trk::theta]<M_PI/2 && per->position().z()>tmppar->position().z())) tpars_up->push_back(tmppar); 
    else tpars_down->push_back(tmppar);

  }
  if (!tpars_up->empty()) std::reverse(tpars_up->begin(),tpars_up->end());
  
/*  Trk::Surface *boundary_surface=findBoundarySurface(par,Trk::alongMomentum);
  if(boundary_surface)
    tpars_down=m_extrapolator->extrapolateStepwise(*m_propagator, par, *boundary_surface, Trk::alongMomentum, m_boundarycheck, Trk::muon);  

  boundary_surface=findBoundarySurface(par,Trk::oppositeMomentum);
  if(boundary_surface)
    tpars_up  =m_extrapolator->extrapolateStepwise(*m_propagator, par, *boundary_surface, Trk::oppositeMomentum, m_boundarycheck, Trk::muon);
*/
  if(tpars_down){
    //std::cout << "analyze_tpars(tpars_down)" << std::endl;
    analyze_tpars(tpars_down);
  }
  if(tpars_up){
    //std::cout << "analyze_tpars(tpars_up)" << std::endl;
    analyze_tpars(tpars_up);
  }

  //clean up
  if(tpars_up){
    std::vector< const Trk::TrackParameters* >::const_iterator parameterIter= tpars_up->begin();
    for ( ; parameterIter != tpars_up->end(); ++parameterIter) {
      delete *parameterIter;
    }
    delete tpars_up;
  }
  
  if(tpars_down){
    std::vector< const Trk::TrackParameters* >::const_iterator parameterIter = tpars_down->begin();
    for ( ; parameterIter != tpars_down->end(); ++parameterIter) {
      delete *parameterIter;
    }
    delete tpars_down;
  }

  msg(MSG::DEBUG)<<"Found "<<m_measurement.size()<<" driftcircles"<<endreq;

  return m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for segment findinf in TRT
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* 
InDet::TRT_TrackExtensionToolCosmics::findSegment(const Trk::TrackParameters&)
{
  return NULL;
}


Trk::Surface* InDet::TRT_TrackExtensionToolCosmics::findBoundarySurface(const Trk::TrackParameters& par, Trk::PropDirection dir)
{

  const Trk::TrackParameters* test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*m_trtcylinder,dir,true,Trk::muon);
  if(test){
    delete test;
    return m_trtcylinder;
  }

  test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*m_trtdiscA,dir,true,Trk::muon);
  if(test){
    delete test;
    return m_trtdiscA;
  }

  test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*m_trtdiscC,dir,true,Trk::muon);
  if(test){
    delete test;
    return m_trtdiscC;
  }

  return 0;
}

Amg::Vector3D InDet::TRT_TrackExtensionToolCosmics::intersect(const Trk::Surface *surf,const Trk::Perigee *per) {
  // Calculate intersection of helix with silicon module. Assume barrel modules parallel to z-axis, endcap modules perpendicular to z-axis

  double sinTheta = sin(per->parameters()[3]);
  double r= (std::abs(per->parameters()[Trk::qOverP]) > 1e-10) ? -sinTheta/(per->parameters()[Trk::qOverP]*0.6) : 1e6;
  double xc=per->position().x()-r*sin(per->parameters()[2]);
  double yc=per->position().y()+r*cos(per->parameters()[2]);
  //double signdeltaphi= (r<0) ? -1.: 1.;
  double phi0=atan2(per->position().y()-yc,per->position().x()-xc);
  double theta=per->parameters()[Trk::theta];
  double z0=per->position().z();

  if (fabs(surf->normal().z())>0.5){ // endcap module
    double delta_s=(surf->center().z()-z0)/cos(theta);
    double delta_phi=delta_s*sin(theta)/r;
    double x=xc+fabs(r)*cos(phi0+delta_phi);
    double y=yc+fabs(r)*sin(phi0+delta_phi);
    //std::cout << "deltaphi: " << delta_phi << " delta_s: " << delta_s << " r: " << r << "intersect: " << x << " " << y << " " << surf->center().z() << std::endl;
    return Amg::Vector3D(x,y,surf->center().z());



  }
  else { // barrel module
    double x1=surf->center().x();
    double y1=surf->center().y();
    double x2=x1+surf->transform()(0,0);
    double y2=y1+surf->transform()(1,0);
    double a=(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
    double b=2*( (x2 - x1)*(x1 - xc) + (y2 - y1)*(y1 - yc));
    double c=xc*xc + yc*yc + x1*x1 + y1*y1 - 2*(xc*x1 + yc*y1) - r*r;
    double discr=b*b-4*a*c;
    //std::cout << "discr: " << discr << std::endl;
    if (discr<0) return Amg::Vector3D(0,0,0);
    double u1=(-b-sqrt(discr))/(2*a);
    double u2=(-b+sqrt(discr))/(2*a);
    double u=(fabs(u1)<fabs(u2)) ? u1 : u2;
    double x=x1+u*(x2-x1);
    double y=y1+u*(y2-y1);
    double phi=atan2(y-yc,x-xc);
   // std::cout << "phi1: " << phi1 << " phi: " << phi << std::endl;
    double delta_phi=phi-phi0;
    if (fabs(fabs(delta_phi)-2*M_PI)<fabs(delta_phi)){
      if (delta_phi<0) delta_phi+=2*M_PI;
      else delta_phi-=2*M_PI;

    }
    double delta_z=r*delta_phi/tan(theta);
    //if (theta>M_PI/2) delta_z=-delta_z;
    //if (fabs(surf.center().x()-107)<60 && fabs(surf.center().y()+290)<60 && fabs(surf.center().z()-700)<60) std::cout << "signdeltaphi: " << signdeltaphi << " delta_phi: " << delta_phi << " phi0: " << phi0 << " phi: " << phi << " z0: " << z0 << " deltaz: " << delta_z << std::endl;

    double z=z0+delta_z;
    //std::cout << "phi0: " << phi0 << " phi: " << phi << " delta phi: " << delta_phi << " z0: " << z0 << " delta z: " << delta_z << " theta: " << theta << " intersect barrel: " << x << " " << y << " " << z << std::endl;
    return Amg::Vector3D(x,y,z);
  }
}

///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks 
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track* 
InDet::TRT_TrackExtensionToolCosmics::newTrack(const Trk::Track&)
{ 
  return 0;
}

