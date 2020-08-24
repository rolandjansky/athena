/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_TrackExtensionToolCosmics
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////




#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrackExtensionToolCosmics.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
//
#include "TrkEventPrimitives/PropDirection.h"
 #include "InDetIdentifier/TRT_ID.h"
#include <iostream>
#include <utility>
#include <cmath>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackExtensionToolCosmics::TRT_TrackExtensionToolCosmics
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_riontrackD("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackToolUniversal"                      ),
    m_riontrackN("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool")
{
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
  declareProperty("BoundaryCheck"        ,m_boundarycheck=false);


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
    msg(MSG::FATAL)<<"Toll service not found !"<<endmsg;
    return StatusCode::FAILURE;
  }


  // Get RIO_OnTrack creator with drift time information
  //
  if(m_riontrackD.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackD <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackD << endmsg;
  }

  // Get RIO_OnTrack creator without drift time information
  //
  if(m_riontrackN.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_riontrackN <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_riontrackN << endmsg;
  }


  ATH_CHECK( m_extrapolator.retrieve() );
  ATH_CHECK( m_propagator.retrieve() );

  if ((detStore()->retrieve(m_trtid)).isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }


  //Initialize container
  ATH_CHECK(m_trtname.initialize());

  return sc;


  

}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackExtensionToolCosmics::finalize()
{
   StatusCode sc = AlgTool::finalize(); 

   return sc;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackExtensionToolCosmics::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out);
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

std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>
InDet::TRT_TrackExtensionToolCosmics::newEvent(const EventContext& ctx) const
{
  //create the boundary surfaces
  //
  SG::ReadHandle<TRT_DriftCircleContainer> trtcontainer(m_trtname, ctx);
  if(not trtcontainer.isValid() && m_outputlevel<=0) {
    std::stringstream msg;
    msg << "Missing TRT_DriftCircleContainer " << m_trtname.key();
    throw std::runtime_error( msg.str() );
  }

  std::unique_ptr<EventData> event_data(new EventData(trtcontainer.cptr()));

  Amg::RotationMatrix3D r; r.setIdentity();
  Amg::Transform3D* t = 0;

  t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D::Zero()));
  event_data->m_trtcylinder= new Trk::CylinderSurface(t,1150.,3000.);
  t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D(0.,0.,3000)));
  event_data->m_trtdiscA   = new Trk::DiscSurface    (t,1.,1200.);
  t = new Amg::Transform3D(r * Amg::Translation3D(Amg::Vector3D(0.,0.,-3000)));
  event_data->m_trtdiscC   = new Trk::DiscSurface    (t,1.,1200.);

  return std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>(event_data.release());

}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////

std::vector<const Trk::MeasurementBase*>& 
InDet::TRT_TrackExtensionToolCosmics::extendTrack(const EventContext& ctx,
                                                  const Trk::Track& Tr,
                                                  InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{ 
  InDet::TRT_TrackExtensionToolCosmics::EventData &
     event_data=InDet::TRT_TrackExtensionToolCosmics::EventData::getPrivateEventData(virt_event_data);

  event_data.m_measurement.clear();

  if(not event_data.m_trtcontainer) return event_data.m_measurement;

  const DataVector<const Trk::TrackStateOnSurface>* 
    tsos = Tr.trackStateOnSurfaces();

  const Trk::TrackParameters* 
    par = (*(tsos->rbegin()))->trackParameters(); if(!par ) return event_data.m_measurement;
  const Trk::TrackParameters* 
    parb = (*(tsos->begin()))->trackParameters();

  
  if(parb && par!=parb) {

    const Amg::Vector3D& g1 = par ->position();
    const Amg::Vector3D& g2 = parb->position();
    if((g2.x()*g2.x()+g2.y()*g2.y()) > (g1.x()*g1.x()+g1.y()*g1.y())) par=parb;
  }

  if(Tr.perigeeParameters()) {
    return extendTrack(ctx, *Tr.perigeeParameters(),event_data);
  }
  event_data.m_measurement.clear();
  return event_data.m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for track extension to TRT
///////////////////////////////////////////////////////////////////
void InDet::TRT_TrackExtensionToolCosmics::analyze_tpars(const std::vector<const Trk::TrackParameters* >* tpars,
                                                         InDet::TRT_TrackExtensionToolCosmics::EventData &event_data) const
{
  msg(MSG::DEBUG)<<"Number of tpars: "<<tpars->size()<<endmsg;

  double lastz=-99999;
  std::vector< const Trk::TrackParameters* >::const_iterator parameterIter = tpars->begin();
  for ( ; parameterIter != tpars->end(); ++parameterIter) {
    msg(MSG::DEBUG)<< "par pos: " << (**parameterIter).position() <<endmsg;
      
    if ( (*parameterIter)->associatedSurface().associatedDetectorElementIdentifier()==0 ) {
      msg(MSG::DEBUG)<<"No DE identifier!!!"<<endmsg;
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
	  const InDet::TRT_DriftCircleCollection *container = event_data.m_trtcontainer->indexFindPtr(detElements[i+1]);
	  
	  if(container==nullptr) {
	    msg(MSG::DEBUG)<<"for the current detectorElement no DriftCircleContainer seems to exist: "<<m_trtid->show_to_string(m_trtid->layer_id(detElements[i+1]))<<endmsg;
	    continue;
	  }
	  
	  msg(MSG::DEBUG)<< "There are "  << container->size() << " entries in the TRT_DriftCircleCollection "<<m_trtid->show_to_string(m_trtid->layer_id(detElements[i+1])) <<endmsg;
	  
	  //take the closest one in case it satisfies some default cuts
	  InDet::TRT_DriftCircleCollection::const_iterator driftCircleIterator = container->begin();
	  for (; driftCircleIterator != container->end(); driftCircleIterator++) {

	    //get the associated surface of the driftcircle
	    const Trk::Surface &dc_surface=(*driftCircleIterator)->detectorElement()->surface((*driftCircleIterator)->identify());

	    //get the local position of the track prediction in the frame of the driftcircle
	    const Amg::Vector2D *lpos=dc_surface.globalToLocal((*parameterIter)->position());

	    double distance=m_roadwidth+1;
	    if(lpos){
	      distance = std::abs(lpos->x());
	      msg(MSG::DEBUG)<<"Hit "<<m_trtid->show_to_string((*driftCircleIterator)->identify())<<" has a distance of "<<distance<<endmsg;

	      double dist_locz=std::abs(lpos->y());
	      if(distance<m_roadwidth+1){
		if(!dc_surface.insideBounds(*lpos,m_roadwidth,m_roadwidth_locz)){
		  msg(MSG::DEBUG)<<"Hit not inside surface bounds! "<<distance<<" , "<<dist_locz<<endmsg;
		  msg(MSG::DEBUG)<<"\trejecting hit"<<endmsg;
		  distance=m_roadwidth+1;
		}
	      }

	      delete lpos;
	    }

	    if(distance<maxdist){
	      maxdist=distance;
	      circ=(*driftCircleIterator);
	    }
	  }
	}
      }
      msg(MSG::DEBUG)<<"Maximal distance: "<<maxdist<<endmsg;
      if(circ){
	msg(MSG::DEBUG)<<"Found Driftcircle! Adding it to list ..."<<m_trtid->show_to_string(circ->identify())<<endmsg;
        if (lastz<-9999) lastz=(**parameterIter).position().z();
        if (std::abs(lastz-(**parameterIter).position().z())>500.) return;
        lastz=(**parameterIter).position().z();
        const Trk::StraightLineSurface *slsurf=dynamic_cast<const Trk::StraightLineSurface *>(&circ->detectorElement()->surface(circ->identify())); if(!slsurf) continue;
        Trk::AtaStraightLine atasl((**parameterIter).position(),(**parameterIter).parameters()[Trk::phi],(**parameterIter).parameters()[Trk::theta],(**parameterIter).parameters()[Trk::qOverP],*slsurf);
        const Trk::MeasurementBase *newmeas=m_riontrackN->correct(*circ,atasl); 
	event_data.m_measurement.push_back(newmeas);

      }	  
    }
  }
}

namespace InDet{
class tp_sort_cosmics{
          public:
          tp_sort_cosmics(double theta){m_theta=theta;}
          bool operator()(const Trk::TrackParameters *par1,const Trk::TrackParameters *par2){
            if (m_theta>M_PI_2) return (par1->position().z()>par2->position().z());
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
InDet::TRT_TrackExtensionToolCosmics::extendTrack(const EventContext& /*ctx*/,
                                                  const Trk::TrackParameters& par,
                                                  InDet::ITRT_TrackExtensionTool::IEventData &virt_event_data) const
{
  InDet::TRT_TrackExtensionToolCosmics::EventData &
     event_data=InDet::TRT_TrackExtensionToolCosmics::EventData::getPrivateEventData(virt_event_data);

  event_data.m_measurement.clear();

  std::vector<Identifier> vecID;
  std::vector<const Trk::TrackParameters*> vecTP;



  std::vector<const Trk::TrackParameters* >* tpars_down=0;
  std::vector<const Trk::TrackParameters* >* tpars_up=0;
  const Trk::Perigee *per=dynamic_cast<const Trk::Perigee *>(&par);
  if (!per) {
    msg(MSG::FATAL)<<"Track perigee not found!"<<endmsg;
    return event_data.m_measurement;
  }

  if (!event_data.m_trtcontainer) {
    return event_data.m_measurement;
  }

  InDet::TRT_DriftCircleContainer::const_iterator
      w = event_data.m_trtcontainer->begin(),we = event_data.m_trtcontainer->end();
  for(; w!=we; ++w) {
     if ((**w).empty()) continue; 
     const Trk::Surface &surf=(**(**w).begin()).detectorElement()->surface();
     Amg::Vector3D pos=intersect(&surf,per);
     Amg::Vector3D locintersec = (surf.transform().inverse())*pos;
     Amg::Vector2D locpos(locintersec.x(), locintersec.y());
     if (pos.perp()<500. || !surf.insideBounds(locpos,50.,50.)) continue;

     Amg::Vector3D pos2=surf.transform()*Amg::Vector3D(locintersec.x(),locintersec.y(),0);

     const Trk::PlaneSurface *plsurf=dynamic_cast<const Trk::PlaneSurface *>(&surf);
     const Trk::DiscSurface *discsurf=dynamic_cast<const Trk::DiscSurface *>(&surf);
     Trk::TrackParameters *newpar=0;
     if (plsurf) newpar=new Trk::AtaPlane(pos2,per->parameters()[Trk::phi],per->parameters()[Trk::theta],per->parameters()[Trk::qOverP],*plsurf);
     else newpar=new Trk::AtaDisc(pos2,per->parameters()[Trk::phi],per->parameters()[Trk::theta],per->parameters()[Trk::qOverP],*discsurf); 
     vecTP.push_back(newpar);
   }
  tpars_down=new std::vector<const Trk::TrackParameters* >;
  tpars_up=new std::vector<const Trk::TrackParameters* >;

  if(!tpars_down ||  !tpars_up) return event_data.m_measurement;

  tp_sort_cosmics sorter(per->parameters()[Trk::theta]);
  std::sort(vecTP.begin(),vecTP.end(),sorter);  
  for (int i=0;i<(int)vecTP.size();i++){ 
    const Trk::TrackParameters *tmppar=vecTP[i];
    if ((per->parameters()[Trk::theta]>M_PI/2 && per->position().z()<tmppar->position().z()) || (per->parameters()[Trk::theta]<M_PI/2 && per->position().z()>tmppar->position().z())) tpars_up->push_back(tmppar); 
    else tpars_down->push_back(tmppar);

  }
  if (!tpars_up->empty()) std::reverse(tpars_up->begin(),tpars_up->end());
  

  if(tpars_down){
    analyze_tpars(tpars_down,event_data);
  }
  if(tpars_up){
    analyze_tpars(tpars_up, event_data);
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

  msg(MSG::DEBUG)<<"Found "<<event_data.m_measurement.size()<<" driftcircles"<<endmsg;

  return event_data.m_measurement;
}

///////////////////////////////////////////////////////////////////
// Main methods for segment findinf in TRT
///////////////////////////////////////////////////////////////////

Trk::TrackSegment* 
InDet::TRT_TrackExtensionToolCosmics::findSegment(const EventContext& /*ctx*/,
                                                  const Trk::TrackParameters&,
                                                  InDet::ITRT_TrackExtensionTool::IEventData &) const
{
  return NULL;
}


Trk::Surface*
InDet::TRT_TrackExtensionToolCosmics::findBoundarySurface(const Trk::TrackParameters&
                                                          par, Trk::PropDirection dir,
                                                          InDet::TRT_TrackExtensionToolCosmics::EventData &event_data) const
{

  const Trk::TrackParameters* test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*event_data.m_trtcylinder,dir,true,Trk::muon);
  if(test){
    delete test;
    return event_data.m_trtcylinder;
  }

  test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*event_data.m_trtdiscA,dir,true,Trk::muon);
  if(test){
    delete test;
    return event_data.m_trtdiscA;
  }

  test=m_extrapolator->extrapolateDirectly(*m_propagator,par,*event_data.m_trtdiscC,dir,true,Trk::muon);
  if(test){
    delete test;
    return event_data.m_trtdiscC;
  }

  return 0;
}

Amg::Vector3D InDet::TRT_TrackExtensionToolCosmics::intersect(const Trk::Surface *surf,const Trk::Perigee *per) {
  // Calculate intersection of helix with silicon module. Assume barrel modules parallel to z-axis, endcap modules perpendicular to z-axis

  double sinTheta = std::sin(per->parameters()[3]);
  double r= (std::abs(per->parameters()[Trk::qOverP]) > 1e-10) ? -sinTheta/(per->parameters()[Trk::qOverP]*0.6) : 1e6;
  double xc=per->position().x()-r*std::sin(per->parameters()[2]);
  double yc=per->position().y()+r*std::cos(per->parameters()[2]);
  double phi0=std::atan2(per->position().y()-yc,per->position().x()-xc);
  double theta=per->parameters()[Trk::theta];
  double z0=per->position().z();

  if (std::abs(surf->normal().z())>0.5){ // endcap module
    double delta_s=(surf->center().z()-z0)/cos(theta);
    double delta_phi=delta_s*std::sin(theta)/r;
    double x=xc+std::abs(r)*std::cos(phi0+delta_phi);
    double y=yc+std::abs(r)*std::sin(phi0+delta_phi);
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
    if (discr<0) return Amg::Vector3D(0,0,0);
    double u1=(-b-std::sqrt(discr))/(2*a);
    double u2=(-b+std::sqrt(discr))/(2*a);
    double u=(std::abs(u1)<std::abs(u2)) ? u1 : u2;
    double x=x1+u*(x2-x1);
    double y=y1+u*(y2-y1);
    double phi=std::atan2(y-yc,x-xc);
    double delta_phi=phi-phi0;
    if (std::abs(std::abs(delta_phi)-2*M_PI)<std::abs(delta_phi)){
      if (delta_phi<0) delta_phi+=2*M_PI;
      else delta_phi-=2*M_PI;

    }
    double delta_z=r*delta_phi/std::tan(theta);

    double z=z0+delta_z;
    return Amg::Vector3D(x,y,z);
  }
}

///////////////////////////////////////////////////////////////////
// Methods for track extension to TRT for pixles+sct tracks 
// and new track production
///////////////////////////////////////////////////////////////////

Trk::Track* 
InDet::TRT_TrackExtensionToolCosmics::newTrack(const EventContext& /*ctx*/,
                                               const Trk::Track&,
                                               InDet::ITRT_TrackExtensionTool::IEventData &) const
{ 
  return 0;
}

