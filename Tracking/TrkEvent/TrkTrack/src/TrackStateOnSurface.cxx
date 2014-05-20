/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <stdexcept>


Trk::TrackStateOnSurface::TrackStateOnSurface()
    :
    m_fitQualityOnSurface(0),
    m_trackParameters(0),
    m_measurementOnTrack(0),
    m_materialEffectsOnTrack(0)
{
}

Trk::TrackStateOnSurface::TrackStateOnSurface(
    const Trk::MeasurementBase          *meas,
    const Trk::TrackParameters          *trackParameter,
    const Trk::FitQualityOnSurface      *fitQoS,
    const Trk::MaterialEffectsBase      *materialEffects

    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects )
{
    assert(isSane());
    setFlags();
}

Trk::TrackStateOnSurface::TrackStateOnSurface(
    const Trk::MeasurementBase* /**meas*/,
    const Trk::TrackParameters* /**trackParameter*/,
    const Trk::FitQualityOnSurface* /**fitQoS*/,
    const Trk::MaterialEffectsBase* /**materialEffects*/,
    const Trk::TrackStateOnSurface::TrackStateOnSurfaceType /**type*/
    )
{
    throw "Using deprecated ctor";
}


Trk::TrackStateOnSurface::TrackStateOnSurface(
    const Trk::MeasurementBase          *meas,
    const Trk::TrackParameters          *trackParameter,
    const Trk::FitQualityOnSurface      *fitQoS,
    const Trk::MaterialEffectsBase      *materialEffects,
    const std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern
    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects ),
    m_typeFlags(typePattern)
{
  assert(isSane());
  //setFlags();
}
 
  
Trk::TrackStateOnSurface::TrackStateOnSurface
    (
    const Trk::MeasurementBase* meas,
    const Trk::TrackParameters* trackParameter
    ):
    m_fitQualityOnSurface(0),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack(0)
{
    assert(isSane());
    setFlags();
}

Trk::TrackStateOnSurface::TrackStateOnSurface(
    const Trk::TrackStateOnSurface& rhs
    ):
    m_fitQualityOnSurface(rhs.m_fitQualityOnSurface 
        ? new Trk::FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : 0 ),
    m_trackParameters(rhs.m_trackParameters 
        ? rhs.m_trackParameters->clone() : 0),
    m_measurementOnTrack(rhs.m_measurementOnTrack 
        ? rhs.m_measurementOnTrack->clone() : 0),
    m_materialEffectsOnTrack(rhs.m_materialEffectsOnTrack
        ? rhs.m_materialEffectsOnTrack->clone() : 0),
    m_typeFlags(rhs.m_typeFlags)
{
}

Trk::TrackStateOnSurface::~TrackStateOnSurface(){
    delete m_fitQualityOnSurface;
    delete m_trackParameters;
    delete m_measurementOnTrack;
    delete m_materialEffectsOnTrack;
}

Trk::TrackStateOnSurface& Trk::TrackStateOnSurface::operator=(const Trk::TrackStateOnSurface& rhs)
{
    if (this!=&rhs){
        delete m_fitQualityOnSurface;
        delete m_trackParameters;
        delete m_measurementOnTrack;
        delete m_materialEffectsOnTrack;
        m_fitQualityOnSurface = rhs.m_fitQualityOnSurface 
            ? new Trk::FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : 0 ;
        m_trackParameters = rhs.m_trackParameters 
            ? rhs.m_trackParameters->clone() : 0;
        m_measurementOnTrack = rhs.m_measurementOnTrack 
            ? rhs.m_measurementOnTrack->clone() : 0 ; 
        m_materialEffectsOnTrack = rhs.m_materialEffectsOnTrack
            ? rhs.m_materialEffectsOnTrack->clone() : 0;
        m_typeFlags = rhs.m_typeFlags;
        assert(isSane());
    }
    return *this;
}

std::string Trk::TrackStateOnSurface::dumpType() const{
    std::string type;
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Measurement))
      type+="Measurement ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::InertMaterial))
      type+="InertMaterial ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::BremPoint))
      type+="BremPoint ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Scatterer))
      type+="Scatterer ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Perigee))
      type+="Perigee ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Outlier))
      type+="Outlier ";    
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Hole))
      type+="Hole ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::CaloDeposit))
      type+="CaloDeposit ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::Parameter))
      type+="Parameter ";
    if (m_typeFlags.test(Trk::TrackStateOnSurface::FitQuality))
      type+="FitQuality ";
    return type;
}

const Trk::Surface& 
Trk::TrackStateOnSurface::surface() const {
  if (m_trackParameters) return m_trackParameters->associatedSurface();
  if (m_measurementOnTrack) return m_measurementOnTrack->associatedSurface();
  if (m_materialEffectsOnTrack) return m_materialEffectsOnTrack->associatedSurface();
  throw std::runtime_error("Trk::TrackStateOnSurface without Surface!");
  // const Trk::Surface* dummy=0;
  // return *dummy;
}

bool 
Trk::TrackStateOnSurface::isSane() const {
  bool isSane=true;
  
  // check that all surfaces are the same.
  const Trk::Surface* paramSurf=0;
  const Trk::Surface* measSurf=0;
  const Trk::Surface* matSurf=0;
  if (m_trackParameters) paramSurf=&(m_trackParameters->associatedSurface());
  if (m_measurementOnTrack) measSurf=&(m_measurementOnTrack->associatedSurface());
  if (m_materialEffectsOnTrack) matSurf=&(m_materialEffectsOnTrack->associatedSurface());


  if (paramSurf&&measSurf&&matSurf) {
    isSane = ( (*paramSurf)==(*measSurf) && (*measSurf)==(*matSurf));
    if (!isSane){
      std::cerr<<"Trk::TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;;
      std::cerr<<"ParamSurf:"<<(*paramSurf)<<std::endl;
      std::cerr<<"measSurf:"<<(*measSurf)<<std::endl;
      std::cerr<<"matSurf:"<<(*matSurf)<<std::endl;
    }
  } else if (paramSurf&&measSurf){
    isSane = ( (*paramSurf)==(*measSurf));
    if (!isSane){
      std::cerr<<"Trk::TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;;
      std::cerr<<"ParamSurf:"<<(*paramSurf)<<std::endl;
      std::cerr<<"measSurf:"<<(*measSurf)<<std::endl;
    }
  } else if (measSurf&&matSurf){
    isSane = ( (*measSurf)==(*matSurf));
    if (!isSane){
      std::cerr<<"Trk::TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;;
      std::cerr<<"measSurf:"<<(*measSurf)<<std::endl;
      std::cerr<<"matSurf:"<<(*matSurf)<<std::endl;
    }
  } else if (paramSurf&&matSurf) {
    isSane =  ( (*paramSurf)==(*matSurf));
    if (!isSane){
      std::cerr<<"Trk::TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;;
      std::cerr<<"ParamSurf:"<<(*paramSurf)<<std::endl;
      std::cerr<<"matSurf:"<<(*matSurf)<<std::endl;
    }
  } 
  return isSane;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::TrackStateOnSurface& tsos)
{ 
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<endreq;
    //write out "type" of object
    
    // if ( tsos.fitQualityOnSurface()!=0) 
    //     sl <<name<<"\t + HAS FitQualityOnSurface(s)."<<endreq;
    // else 
    //     sl <<name<<"\t - NO FitQualityOnSurfaces."<<endreq;
    // 
    // if ( tsos.trackParameters() !=0) 
    //     sl <<name<<"\t + HAS TrackParameter(s)."<<endreq;
    // else 
    //     sl <<name<<"\t - NO TrackParameters."<<endreq;
    // 
    // if ( tsos.measurementOnTrack()!=0) 
    //     sl <<name<<"\t + HAS MeasurementBase(s)."<<endreq;
    // else 
    //     sl <<name<<"\t - NO MeasurementBases."<<endreq;
    // 
    // if (tsos.materialEffectsOnTrack()!=0) 
    //     sl <<name<<"\t + HAS MaterialEffectsBase."<<endreq;
    // else 
    //     sl <<name<<"\t - NO MaterialEffects."<<endreq;
    
    if (sl.level()<MSG::INFO) 
    {
        sl<<name<<"Detailed dump of contained objects follows:"<<endreq;
        if ( tsos.fitQualityOnSurface()!=0) 
            sl << *(tsos.fitQualityOnSurface() )<<endreq<<" (end of FitQualityOnSurface dump)"<<endreq;
    
        if ( tsos.trackParameters() !=0) 
            sl << *(tsos.trackParameters() )<<endreq<<" (end of TrackParameters dump)"<<endreq;
    
        if ( tsos.measurementOnTrack()!=0) 
            sl << *(tsos.measurementOnTrack() )<<endreq<<" (end of MeasurementBase dump"<<endreq;
    
        if (tsos.materialEffectsOnTrack()!=0) 
            sl << *(tsos.materialEffectsOnTrack() )<<endreq<<" (end of MaterialEffectsBase dump)"<<endreq;
    }
    return sl; 
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::TrackStateOnSurface& tsos)
{
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<std::endl;
    
    if ( tsos.fitQualityOnSurface()!=0) 
    {
        sl <<"\t HAS FitQualityOnSurface(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.fitQualityOnSurface() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO FitQualityOnSurfaces."<<std::endl;
    }

    if ( tsos.trackParameters()!=0) 
    {
        sl <<"\t HAS TrackParameter(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.trackParameters() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO TrackParameters."<<std::endl;
    }

    if (tsos.measurementOnTrack()!=0) 
    {
        sl <<"\t HAS MeasurementBase(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.measurementOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MeasurementBase."<<std::endl;
    }

    if (tsos.materialEffectsOnTrack()!=0) 
    {
        sl <<"\t HAS MaterialEffectsBase."<<std::endl;
        sl <<"\t \t"<< *(tsos.materialEffectsOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MaterialEffects."<<std::endl;
    }   return sl;
}
