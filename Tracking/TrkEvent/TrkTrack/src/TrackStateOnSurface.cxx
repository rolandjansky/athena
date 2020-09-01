/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <stdexcept>
#include "TrkTrack/AlignmentEffectsOnTrack.h"

namespace Trk {
TrackStateOnSurface::TrackStateOnSurface()
    :
    m_fitQualityOnSurface(nullptr),
    m_trackParameters(nullptr),
    m_measurementOnTrack(nullptr),
    m_materialEffectsOnTrack(nullptr),
    m_alignmentEffectsOnTrack(nullptr)
{
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase          *meas,
    const TrackParameters          *trackParameter,
    const FitQualityOnSurface      *fitQoS,
    const MaterialEffectsBase      *materialEffects,
    const AlignmentEffectsOnTrack       *alignmentEffectsOnTrack
    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects ),
    m_alignmentEffectsOnTrack(alignmentEffectsOnTrack)
{
    assert(isSane());
    setFlags();
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase* /**meas*/,
    const TrackParameters* /**trackParameter*/,
    const FitQualityOnSurface* /**fitQoS*/,
    const MaterialEffectsBase* /**materialEffects*/,
    const TrackStateOnSurface::TrackStateOnSurfaceType /**type*/
    )
{
    throw "Using deprecated ctor";
}

TrackStateOnSurface::TrackStateOnSurface(
    const MeasurementBase          *meas,
    const TrackParameters          *trackParameter,
    const FitQualityOnSurface      *fitQoS,
    const MaterialEffectsBase      *materialEffects,
    const std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>& typePattern,
    const AlignmentEffectsOnTrack       *alignmentEffectsOnTrack
    ):
    m_fitQualityOnSurface(fitQoS),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack( materialEffects ),
    m_alignmentEffectsOnTrack( alignmentEffectsOnTrack ),
    m_typeFlags(typePattern)
{
  assert(isSane());
  //setFlags();
}
 
  
TrackStateOnSurface::TrackStateOnSurface
    (
    const MeasurementBase* meas,
    const TrackParameters* trackParameter
    ):
    m_fitQualityOnSurface(nullptr),
    m_trackParameters(trackParameter),
    m_measurementOnTrack(meas),
    m_materialEffectsOnTrack(nullptr),
    m_alignmentEffectsOnTrack(nullptr)
{
    assert(isSane());
    setFlags();
}

TrackStateOnSurface::TrackStateOnSurface(
    const TrackStateOnSurface& rhs
    ):
    m_fitQualityOnSurface(rhs.m_fitQualityOnSurface 
        ? new FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : nullptr ),
    m_trackParameters(rhs.m_trackParameters 
        ? rhs.m_trackParameters->clone() : nullptr),
    m_measurementOnTrack(rhs.m_measurementOnTrack 
        ? rhs.m_measurementOnTrack->clone() : nullptr),
    m_materialEffectsOnTrack(rhs.m_materialEffectsOnTrack
        ? rhs.m_materialEffectsOnTrack->clone() : nullptr),
    m_alignmentEffectsOnTrack(rhs.m_alignmentEffectsOnTrack
        ? new AlignmentEffectsOnTrack(*rhs.m_alignmentEffectsOnTrack) : nullptr),
    m_typeFlags(rhs.m_typeFlags)
{
}

TrackStateOnSurface::~TrackStateOnSurface(){
    delete m_fitQualityOnSurface;
    delete m_trackParameters;
    delete m_measurementOnTrack;
    delete m_materialEffectsOnTrack;
    delete m_alignmentEffectsOnTrack;
}

TrackStateOnSurface& TrackStateOnSurface::operator=(const TrackStateOnSurface& rhs)
{
    if (this!=&rhs){
        delete m_fitQualityOnSurface;
        delete m_trackParameters;
        delete m_measurementOnTrack;
        delete m_materialEffectsOnTrack;
        delete m_alignmentEffectsOnTrack;
        m_fitQualityOnSurface = rhs.m_fitQualityOnSurface 
            ? new FitQualityOnSurface(*rhs.m_fitQualityOnSurface) : nullptr ;
        m_trackParameters = rhs.m_trackParameters 
            ? rhs.m_trackParameters->clone() : nullptr;
        m_measurementOnTrack = rhs.m_measurementOnTrack 
            ? rhs.m_measurementOnTrack->clone() : nullptr ; 
        m_materialEffectsOnTrack = rhs.m_materialEffectsOnTrack
            ? rhs.m_materialEffectsOnTrack->clone() : nullptr;
        m_alignmentEffectsOnTrack = rhs.m_alignmentEffectsOnTrack
            ? new AlignmentEffectsOnTrack(*rhs.m_alignmentEffectsOnTrack) : nullptr;
        m_typeFlags = rhs.m_typeFlags;
        assert(isSane());
    }
    return *this;
}

Trk::TrackStateOnSurface&
Trk::TrackStateOnSurface::operator=(Trk::TrackStateOnSurface&& rhs)
{
    if (this!=&rhs){
      delete m_fitQualityOnSurface;
      m_fitQualityOnSurface = rhs.m_fitQualityOnSurface;
      rhs.m_fitQualityOnSurface = nullptr;

      delete m_trackParameters;
      m_trackParameters = rhs.m_trackParameters;
      rhs.m_trackParameters = nullptr;

      delete m_measurementOnTrack;
      m_measurementOnTrack = rhs.m_measurementOnTrack;
      rhs.m_measurementOnTrack = nullptr;

      delete m_alignmentEffectsOnTrack;
      m_alignmentEffectsOnTrack = rhs.m_alignmentEffectsOnTrack;
      rhs.m_alignmentEffectsOnTrack = nullptr;

      delete m_materialEffectsOnTrack;
      m_materialEffectsOnTrack = rhs.m_materialEffectsOnTrack;
      rhs.m_materialEffectsOnTrack = nullptr;

      m_typeFlags = std::move (rhs.m_typeFlags);
      assert(isSane());
    }
    return *this;
}

std::string TrackStateOnSurface::dumpType() const{
    std::string type;
    if (m_typeFlags.test(TrackStateOnSurface::Measurement)) {
      type+="Measurement ";}
    if (m_typeFlags.test(TrackStateOnSurface::InertMaterial)) {
      type+="InertMaterial ";}
    if (m_typeFlags.test(TrackStateOnSurface::BremPoint)) {
      type+="BremPoint ";}
    if (m_typeFlags.test(TrackStateOnSurface::Scatterer)) {
      type+="Scatterer ";}
    if (m_typeFlags.test(TrackStateOnSurface::Perigee)) {
      type+="Perigee ";}
    if (m_typeFlags.test(TrackStateOnSurface::Outlier)) {
      type+="Outlier ";    }
    if (m_typeFlags.test(TrackStateOnSurface::Hole)) {
      type+="Hole ";}
    if (m_typeFlags.test(TrackStateOnSurface::CaloDeposit)) {
      type+="CaloDeposit ";}
    if (m_typeFlags.test(TrackStateOnSurface::Parameter)) {
      type+="Parameter ";}
    if (m_typeFlags.test(TrackStateOnSurface::FitQuality)) {
      type+="FitQuality ";}
    if (m_typeFlags.test(TrackStateOnSurface::Alignment)) {
      type+="Alignment ";}
    return type;
}

const Surface& 
TrackStateOnSurface::surface() const {
  if (m_trackParameters) { return m_trackParameters->associatedSurface();}
  if (m_measurementOnTrack) { return m_measurementOnTrack->associatedSurface();}
  if (m_materialEffectsOnTrack) { return m_materialEffectsOnTrack->associatedSurface();}
  if (m_alignmentEffectsOnTrack) { return m_alignmentEffectsOnTrack->associatedSurface();}
  throw std::runtime_error("TrackStateOnSurface without Surface!");
  // const Surface* dummy=0;
  // return *dummy;
}

bool 
TrackStateOnSurface::isSane() const {
  std::vector<const Surface* > surfaces;

  if (m_trackParameters) { surfaces.push_back(&(m_trackParameters->associatedSurface()));}
  if (m_measurementOnTrack) { surfaces.push_back(&(m_measurementOnTrack->associatedSurface()));}
  if (m_materialEffectsOnTrack) { surfaces.push_back(&(m_materialEffectsOnTrack->associatedSurface()));}
  if (m_alignmentEffectsOnTrack) { surfaces.push_back(&(m_alignmentEffectsOnTrack->associatedSurface()));
}

  auto surfaceIt = surfaces.begin();
  bool surfacesDiffer=false;
  while (surfaceIt!=surfaces.end()){
    if (**surfaceIt!=*surfaces[0]) {
      surfacesDiffer=true;
      break;
    }
    surfaceIt++;
  }
  
  if (surfacesDiffer){
    std::cerr<<"TrackStateOnSurface::isSane. Surfaces differ! "<<std::endl;
    if (m_trackParameters) {
      std::cerr << "ParamSurf: [" << &(m_trackParameters->associatedSurface()) << "] "
                << m_trackParameters->associatedSurface() << std::endl;
    }
    if (m_measurementOnTrack) {
      std::cerr << "measSurf: [" << &(m_measurementOnTrack->associatedSurface()) << "] "
                << m_measurementOnTrack->associatedSurface() << std::endl;
    }
    if (m_materialEffectsOnTrack) {
      std::cerr << "matSurf: [" << &(m_materialEffectsOnTrack->associatedSurface()) << "] "
                << m_materialEffectsOnTrack->associatedSurface() << std::endl;
    }
    if (m_alignmentEffectsOnTrack) {
      std::cerr << "alignSurf: [" << &(m_alignmentEffectsOnTrack->associatedSurface()) << "] "
                << m_alignmentEffectsOnTrack->associatedSurface() << std::endl;
    }
    return false;
  }
 
  return true;
}


  void TrackStateOnSurface::setHint  ( const TrackStateOnSurfaceType& type ) {
    if (type>=PartialPersistification && type<=PersistifySlimCaloDeposit) {
      // allowed to modify persistification flags although this is const
      m_typeFlags.set(type);
    }
    else {
      throw std::logic_error("Can only use TrackStateOnSurface::mark to set persistification flags");
    }
  }

  void TrackStateOnSurface::resetHint ( const TrackStateOnSurfaceType& type ) {
    if (type>=PartialPersistification && type<=PersistifySlimCaloDeposit) {
      // allowed to modify persistification flags although this is const
      m_typeFlags.reset(type);
    }
    else {
      throw std::logic_error("Can only use TrackStateOnSurface::mark to set persistification flags");
    }
  }


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const TrackStateOnSurface& tsos)
{ 
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<endmsg;
    //write out "type" of object
    
    if (sl.level()<MSG::INFO) 
    {
        sl<<name<<"Detailed dump of contained objects follows:"<<endmsg;
        if ( tsos.fitQualityOnSurface()!=nullptr) { 
            sl << *(tsos.fitQualityOnSurface() )<<"\n (end of FitQualityOnSurface dump)"<<endmsg;}
    
        if ( tsos.trackParameters() !=nullptr) { 
            sl << *(tsos.trackParameters() )<<"\n (end of TrackParameters dump)"<<endmsg;}
    
        if ( tsos.measurementOnTrack()!=nullptr) { 
            sl << *(tsos.measurementOnTrack() )<<"\n (end of MeasurementBase dump"<<endmsg;}
    
        if (tsos.materialEffectsOnTrack()!=nullptr) { 
            sl << *(tsos.materialEffectsOnTrack() )<<"\n (end of MaterialEffectsBase dump)"<<endmsg;}
        
        if (tsos.alignmentEffectsOnTrack()!=nullptr) { 
            sl << *(tsos.alignmentEffectsOnTrack() )<<"\n (end of AlignmentEffectsOnTrack dump)"<<endmsg;}
    }
    return sl; 
}

std::ostream& operator << ( std::ostream& sl, const TrackStateOnSurface& tsos)
{
    std::string name("TrackStateOnSurface: ");
    sl <<name<<"\t of type : "<<tsos.dumpType()<<std::endl;
    
    if ( tsos.fitQualityOnSurface()!=nullptr) 
    {
        sl <<"\t HAS FitQualityOnSurface(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.fitQualityOnSurface() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO FitQualityOnSurfaces."<<std::endl;
    }

    if ( tsos.trackParameters()!=nullptr) 
    {
        sl <<"\t HAS TrackParameter(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.trackParameters() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO TrackParameters."<<std::endl;
    }

    if (tsos.measurementOnTrack()!=nullptr) 
    {
        sl <<"\t HAS MeasurementBase(s)."<<std::endl;
        sl <<"\t \t"<< *(tsos.measurementOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MeasurementBase."<<std::endl;
    }

    if (tsos.materialEffectsOnTrack()!=nullptr) 
    {
        sl <<"\t HAS MaterialEffectsBase."<<std::endl;
        sl <<"\t \t"<< *(tsos.materialEffectsOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO MaterialEffects."<<std::endl;
    }   /**return sl; don't return here, the next code becomes dead**/
    
    if (tsos.alignmentEffectsOnTrack()!=nullptr) 
    {
        sl <<"\t HAS AlignmentEffectsOnTrack."<<std::endl;
        sl <<"\t \t"<< *(tsos.alignmentEffectsOnTrack() )<<std::endl;
    } 
    else 
    {
        sl <<"\t NO AlignmentEffectsOnTrack."<<std::endl;
    }   return sl;
}
}//namespace Trk
