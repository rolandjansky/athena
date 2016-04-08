/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackCollection.h"
#include "InDetTrackPRD_Association/InDetTrackPRD_Association.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::InDetTrackPRD_Association::InDetTrackPRD_Association
(const std::string& name,ISvcLocator* pSvcLocator) : 

  AthAlgorithm(name, pSvcLocator)                           ,
  m_assoTool  ("InDet::InDetPRD_AssociationToolGangedPixels")
{
  m_nprint      = 0;
  m_outputlevel = 0;
 
  // InDetTrackPRD_Association steering parameters
  //
  declareProperty("AssociationTool", m_assoTool  );
  declareProperty("TracksName"     , m_tracksName);
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::InDetTrackPRD_Association::initialize() 
{
  
  // Get association tool
  //
  if( m_assoTool.retrieve().isFailure()) {

    msg(MSG::FATAL)<< "Failed to retrieve tool " << m_assoTool << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endreq;
  }

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  
  if(m_outputlevel<=0) {
    m_nprint=0; msg(MSG::DEBUG)<<(*this)<<endreq;
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////

StatusCode InDet::InDetTrackPRD_Association::execute() 
{ 

  // Assosiate tracks with PRDs
  //
  m_assoTool->reset();

  std::vector<std::string>::const_iterator col=m_tracksName.begin(),cole=m_tracksName.end();

  int n = 0;
  for(; col!=cole; ++col) {

    m_istracks  [n]               = false;
    m_tracksPRD [n]               = 0    ;
    m_tracksPRDn[n]               = 0    ;
    const TrackCollection* tracks = 0    ;

    StatusCode sc = evtStore()->retrieve(tracks,(*col));
    if (sc.isFailure() || !tracks) continue;

    m_istracks  [n] = true;
    
    TrackCollection::const_iterator t  = tracks->begin();
    TrackCollection::const_iterator te = tracks->end  ();

    for ( ; t!=te; ++t) {
      
      if((m_assoTool->addPRDs(**t)).isFailure()) ++m_tracksPRDn[n]; else ++m_tracksPRD[n];
    }

    if(n<9) ++n;
  } 

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

StatusCode InDet::InDetTrackPRD_Association::finalize() {return StatusCode::SUCCESS;}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::InDetTrackPRD_Association& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::InDetTrackPRD_Association& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrackPRD_Association::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::InDetTrackPRD_Association::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrackPRD_Association::dumptools( MsgStream& out ) const
{
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;
  int n = 65-m_assoTool.type().size();

  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  out<<"| Tool for track-prd association                  | "<<m_assoTool.type()<<s1
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"----------------------------------------------------|"
     <<std::endl;

  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::InDetTrackPRD_Association::dumpevent( MsgStream& out ) const
{
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;

  std::vector<std::string>::const_iterator col=m_tracksName.begin(),cole=m_tracksName.end();

  int N = 0;

  for(; col!=cole; ++col) {

    int n  = 30-(*col).size();
    std::string s; for(int i=0; i<n; ++i) s.append(" "); s.append("|");
    out<<"| "
       <<(*col)<<s
       <<" is collection "
       <<std::setw(2)<<m_istracks  [N]
       <<"  tracks with PRD "
       <<std::setw(5)<<m_tracksPRD [N]
       <<"  without PRD "
       <<std::setw(5)<<m_tracksPRDn[N]
       <<"     |"
       <<std::endl;
    if(N<9) ++N;
  }

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;

  return out;
}





