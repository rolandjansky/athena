/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftCircleOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include <cassert>
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <limits>


// Constructor with parameters:
InDet::TRT_DriftCircleOnTrack::TRT_DriftCircleOnTrack( 
    const InDet::TRT_DriftCircle* RIO, 
    const Trk::LocalParameters& driftRadius,
    const Amg::MatrixX& errDriftRadius,
    const IdentifierHash& idDE,
    double predictedLocZ,
    const Amg::Vector3D& predictedTrackDirection,
    const Trk::DriftCircleStatus status)
	:
  Trk::RIO_OnTrack(driftRadius, errDriftRadius, RIO->identify()), //call base class constructor
  m_globalPosition(0), 
  m_positionAlongWire(predictedLocZ),
  m_idDE(idDE),
  m_status(status),
  m_highLevel(RIO->highLevel()),  
  m_timeOverThreshold(RIO->timeOverThreshold()),
  m_detEl( RIO->detectorElement() )
{
  m_rio.setElement(RIO);
  const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>(&(m_detEl->surface(RIO->identify())));
  if(slsf) m_globalPosition = slsf->localToGlobal(driftRadius, predictedTrackDirection, predictedLocZ);
  Amg::Vector3D  loc_gDirection = predictedTrackDirection; 
  const double dr = driftRadius[Trk::driftRadius];
  //scaling the direction with drift radius   
  if(dr !=0.){ 
   m_localAngle = atan2(loc_gDirection.y(),loc_gDirection.x());
  } else m_localAngle = 0.;
 
}

// Destructor:
InDet::TRT_DriftCircleOnTrack::~TRT_DriftCircleOnTrack()
{ delete m_globalPosition; }

// default constructor:
InDet::TRT_DriftCircleOnTrack::TRT_DriftCircleOnTrack()
	:
  Trk::RIO_OnTrack(),
  m_globalPosition(0),
  m_localAngle(std::numeric_limits<float>::quiet_NaN()),
  m_positionAlongWire(std::numeric_limits<float>::quiet_NaN()),
  m_rio(),
  m_idDE(),
  m_status(Trk::UNDECIDED),
  m_highLevel(false),
  m_timeOverThreshold(0.),
  m_detEl(0)
{}

InDet::TRT_DriftCircleOnTrack::TRT_DriftCircleOnTrack
   ( const ElementLinkToIDCTRT_DriftCircleContainer& RIO,
     const Trk::LocalParameters& driftRadius,
     const Amg::MatrixX& errDriftRadius, 
     IdentifierHash idDE,
     const Identifier& id,
     double predictedLocZ,
     float localAngle,
     const Trk::DriftCircleStatus status,
     bool highLevel,
     double timeOverThreshold)
     : Trk::RIO_OnTrack (driftRadius, errDriftRadius, id),
       m_globalPosition(nullptr),
       m_localAngle(localAngle),
       m_positionAlongWire(predictedLocZ),
       m_rio(RIO),
       m_idDE(idDE),
       m_status(status),
       m_highLevel(highLevel),
       m_timeOverThreshold(timeOverThreshold),
       m_detEl( nullptr)
{
}

//copy constructor:
InDet::TRT_DriftCircleOnTrack::TRT_DriftCircleOnTrack( const InDet::TRT_DriftCircleOnTrack& rot):
	Trk::RIO_OnTrack(rot),
  m_globalPosition(rot.m_globalPosition ? new Amg::Vector3D(*rot.m_globalPosition.load()) : 0 ),
  m_localAngle(rot.m_localAngle.load()),
  m_positionAlongWire(rot.m_positionAlongWire.load()),
  m_rio(rot.m_rio),
  m_idDE(rot.m_idDE),
  m_status(rot.m_status),
  m_highLevel(rot.m_highLevel),
  m_timeOverThreshold(rot.m_timeOverThreshold),
  m_detEl(rot.m_detEl)
{}

//assignment operator:
InDet::TRT_DriftCircleOnTrack& InDet::TRT_DriftCircleOnTrack::operator=( const InDet::TRT_DriftCircleOnTrack& rot)
{ 
  if ( &rot != this) {
    Trk::RIO_OnTrack::operator= (rot);
    delete m_globalPosition;
    m_globalPosition        = (rot.m_globalPosition ? new Amg::Vector3D(*rot.m_globalPosition) : 0);
    m_rio                   = rot.m_rio;
    m_localAngle            = rot.m_localAngle.load();
    m_positionAlongWire     = rot.m_positionAlongWire.load();
    m_idDE                  = rot.m_idDE;
    m_status                = rot.m_status;
    m_highLevel             = rot.m_highLevel;
    m_timeOverThreshold     = rot.m_timeOverThreshold;
    m_detEl                 = rot.m_detEl;
   }
  return *this;
}

//move assignment operator:
InDet::TRT_DriftCircleOnTrack& InDet::TRT_DriftCircleOnTrack::operator=( InDet::TRT_DriftCircleOnTrack&& rot)
{ 
  if ( &rot != this) {
    Trk::RIO_OnTrack::operator= (rot);
    delete m_globalPosition;
    m_globalPosition        = rot.m_globalPosition.load();
    m_rio                   = rot.m_rio;
    m_localAngle            = rot.m_localAngle.load();
    m_positionAlongWire     = rot.m_positionAlongWire.load();
    m_idDE                  = rot.m_idDE;
    m_status                = rot.m_status;
    m_highLevel             = rot.m_highLevel;
    m_timeOverThreshold     = rot.m_timeOverThreshold;
    m_detEl                 = rot.m_detEl;
   }
  return *this;
}

Trk::DriftCircleSide InDet::TRT_DriftCircleOnTrack::side() const{ 
  if (m_status == Trk::UNDECIDED) return Trk::NONE;
  if (localParameters()[Trk::driftRadius] < 0. ) return Trk::LEFT;
  return Trk::RIGHT; 
}


const Trk::Surface& InDet::TRT_DriftCircleOnTrack::associatedSurface() const
{ 
    assert(0!=m_detEl);
    return (m_detEl->surface(identify())); 
}
  
void InDet::TRT_DriftCircleOnTrack::setGlobalPosition(Amg::Vector3D& loc3Dframe) const{
   const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>( &(associatedSurface()) );
   if(slsf) {
     delete m_globalPosition;
     m_globalPosition = nullptr;
     const Amg::Vector3D* expected{nullptr};
     const Amg::Vector3D* desired{new Amg::Vector3D(slsf->transform() * loc3Dframe)};
     if (not m_globalPosition.compare_exchange_strong(expected, desired)) {
       // This happens if more than one threads try to set m_globalPosition.
       delete desired;
       desired = nullptr;
     }
   }else{
    throw GaudiException("Dynamic_cast to StraightLineSurface failed!",             
                    	 "TRT_DriftCircleOnTrack::setGlobalPosition()", 
                    	 StatusCode::FAILURE);
  }
}

void InDet::TRT_DriftCircleOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData*)
{
    m_detEl = dynamic_cast<const InDetDD::TRT_BaseElement* >(detEl);
}

const Amg::Vector3D& InDet::TRT_DriftCircleOnTrack::globalPosition() const { 
  
   if (!m_globalPosition)
   { 

    if (side()==Trk::NONE) 
    {
     //local position   
     Amg::Vector3D loc3Dframe(0., 0., m_positionAlongWire);

     //transfrom to global
     setGlobalPosition(loc3Dframe);
    }else{
  
      // get global position where track and drift radius intersect.
      double Sf,Cf; sincos(m_localAngle,&Sf,&Cf);
      double x = localParameters()[Trk::driftRadius]*Sf;
      double y = localParameters()[Trk::driftRadius]*Cf;
      /*
      double x = localParameters()[Trk::driftRadius]*std::sin(m_localAngle);
      double y = localParameters()[Trk::driftRadius]*std::cos(m_localAngle);
      */
      //get local position
      Amg::Vector3D loc3Dframe(x, y, m_positionAlongWire);

     //transform to global
      setGlobalPosition(loc3Dframe);
    }//end of checking the side information availability
       
   }//end of checking whether the global position is there

   //returning the result (now stored in private datamember)
   return (*m_globalPosition); 
 }



MsgStream& InDet::TRT_DriftCircleOnTrack::dump( MsgStream& sl ) const
{
    Trk::RIO_OnTrack::dump(sl); 
	std::string name("TRT_DriftCircleOnTrack: ");
	sl <<name<< "\t  identifier  = "<< identify()<<endmsg;
        sl <<name<< "\t  time-over-threshold = " << timeOverThreshold()
           << (highLevel() ? " with TR flag ON":" with TR flag OFF") << endmsg;
	sl <<name<< "\t  driftradius = (" 
		 << (localParameters())[Trk::loc1] << ") " <<endmsg;
	sl <<name<< "\t  has Error Matrix: "<<endmsg;
	sl<<localCovariance()<<endmsg; 
	return sl;
}

std::ostream& InDet::TRT_DriftCircleOnTrack::dump( std::ostream& sl ) const
{
    sl << "TRT_DriftCircleOnTrack {"<<std::endl;

    Trk::RIO_OnTrack::dump(sl); 

    sl << "Global position (x,y,z) = (";
    this->globalPosition();
    if ( m_globalPosition !=0 )
    {
        sl  <<this->globalPosition().x()<<", "
            <<this->globalPosition().y()<<", "
            <<this->globalPosition().z()<<")"<<std::endl;
    } else {
        sl<<"NULL!), "<<std::endl;
    }

    sl << "\t  time-over-threshold = " << timeOverThreshold()
        << (highLevel() ? " with TR flag ON":" with TR flag OFF")<<std::endl;
    sl<<"}"<<std::endl;
    
	return sl;
}




