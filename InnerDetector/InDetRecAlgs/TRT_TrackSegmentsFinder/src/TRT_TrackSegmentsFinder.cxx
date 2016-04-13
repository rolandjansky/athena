/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_TrackSegmentsFinder/TRT_TrackSegmentsFinder.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"

#include "InDetReadoutGeometry/TRT_BaseElement.h"

#include "CxxUtils/make_unique.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsFinder::TRT_TrackSegmentsFinder
(const std::string& name,ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator)                          ,
  m_foundSegments("TRTSegments"),
  m_calo("InDetCaloClusterROIs"),
  m_segmentsMakerTool("InDet::TRT_TrackSegmentsMaker_ATLxk"),
  m_roadtool         ("InDet::TRT_DetElementsRoadMaker_xk")
{
  // TRT_TrackSegmentsFinder steering parameters
  //

  m_outputlevel       = 0                                ;
  m_nprint            = 0                                ;
  m_nsegments         = 0                                ;
  m_nsegmentsTotal    = 0                                ;
  m_useCaloSeeds      = false                            ;
  m_minNumberDCs     = 9                                 ; 
  declareProperty("SegmentsMakerTool",m_segmentsMakerTool);
  declareProperty("SegmentsLocation" ,m_foundSegments    );
  declareProperty("useCaloSeeds"     ,m_useCaloSeeds     );
  declareProperty("RoadTool"         ,m_roadtool         );
  declareProperty("InputClusterContainerName",m_calo);
  declareProperty("MinNumberDriftCircles"    ,m_minNumberDCs);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsFinder::initialize() 
{
  
  StatusCode sc; 

  // get tTools servise
  //
  IToolSvc* toolSvc;
  if ((sc=service("ToolSvc", toolSvc)).isFailure())  {
    msg(MSG::FATAL)<<"Toll service not found !"<<endreq; return sc;
  }

  // Get tool for drift circles seeds maker
  //
  if(m_segmentsMakerTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_segmentsMakerTool<< endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_segmentsMakerTool  << endreq;
  }
  
  if( m_useCaloSeeds) {
    // Get detector elements road maker tool
    //
    if(m_roadtool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_roadtool <<endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_roadtool << endreq;
    }
  } 


  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
  m_nsegmentsTotal = 0;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsFinder::execute() 
{
  StatusCode s = m_foundSegments.record( CxxUtils::make_unique<Trk::SegmentCollection>());
  
  if (s.isFailure() || !m_foundSegments.isValid() ) {
    msg(MSG::ERROR)<<"Could not save TRT segments" <<endreq;
    return s;
  }  
  
  Trk::Segment* segment = 0;
  m_nsegments           = 0;
  
  if(!m_useCaloSeeds) {
 
    m_segmentsMakerTool->newEvent();
    m_segmentsMakerTool->find    (); 

    // Loop through all segments and reconsrtucted segments collection preparation
    //
    while((segment = m_segmentsMakerTool->next())) {
      ++m_nsegments; m_foundSegments->push_back(segment);
    }
  }
  else   {

    Amg::Vector3D PSV(0.,0.,0.); Trk::PerigeeSurface PS(PSV);
    const Trk::TrackParameters*      par  = 0;
    
    std::vector<IdentifierHash>      vTR;
   
      
    if(m_calo.isValid()) {

      CaloClusterROI_Collection::const_iterator c = m_calo->begin(), ce = m_calo->end();

      for(; c!=ce; ++c) {
	
	double x = (*c)->globalPosition().x();
	double y = (*c)->globalPosition().y();
	double z = (*c)->globalPosition().z();
	
	par = PS.createTrackParameters(0.,0.,atan2(y,x), atan2(1.,z/sqrt(x*x+y*y)),0.,0);

	// TRT detector elements road builder
	//
	std::list<const InDetDD::TRT_BaseElement*> DE;
	m_roadtool->detElementsRoad(*par,Trk::alongMomentum,DE); delete par;
	if(int(DE.size()) < m_minNumberDCs) continue;


	vTR.clear();
	std::list<const InDetDD::TRT_BaseElement*>::iterator d,de=DE.end();
	for(d=DE.begin(); d!=de; ++d) {vTR.push_back((*d)->identifyHash());}

	
	m_segmentsMakerTool->newRegion(vTR);
	m_segmentsMakerTool->find(); 
	
	// Loop through all segments and reconsrtucted segments collection preparation
	//
	while((segment = m_segmentsMakerTool->next())) {
	  ++m_nsegments; m_foundSegments->push_back(segment);
	}
      }
    }else{
        msg(MSG::WARNING)<<"Could not find TRT segments in container " << m_calo.name() <<endreq;
        
    }
  }
  m_segmentsMakerTool->endEvent();
  m_nsegmentsTotal+=m_nsegments; 



  
  // Print common event information
  //
  if(m_outputlevel<=0) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsFinder::finalize() 
{
  m_nprint=2; msg(MSG::INFO)<<(*this)<<endreq;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_TrackSegmentsFinder& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_TrackSegmentsFinder& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_segmentsMakerTool.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_foundSegments.name().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");

  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  out<<"| Tool for TRT track segments finding             | "<<m_segmentsMakerTool.type()<<s1
     <<std::endl;
  out<<"| Location of output segments                     | "<<m_foundSegments.name()<<s2
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_TrackSegmentsFinder::dumpevent( MsgStream& out ) const
{
  int n = m_nsegments; if(m_nprint > 1) n = m_nsegmentsTotal;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  if(m_useCaloSeeds) { 
    out<<"| TRT track segments found with calo seeds        |"
       <<std::setw(7)<<n
       <<"                                       |"
       <<std::endl;
  }
  else   {
    out<<"| TRT track segments found without calo seeds     |"
       <<std::setw(7)<<n
       <<"                                       |"
       <<std::endl;
  }
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_TrackSegmentsFinder::dump( std::ostream& out ) const
{
  return out;
}





