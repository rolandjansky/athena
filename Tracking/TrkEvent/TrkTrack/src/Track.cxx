/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TrkTrack/Track.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "GaudiKernel/MsgStream.h"

#include <cassert>
#include <iostream> 
#include <string> 


#ifndef NDEBUG
std::atomic<unsigned int> Trk::Track::s_numberOfInstantiations{};
#endif

namespace{
const  bool debug=false;
}

Trk::Track::Track ():
  m_cachedParameterVector{},
  m_cachedMeasurementVector{},
  m_cachedOutlierVector{}, 
  m_trackStateVector(nullptr),
  m_perigeeParameters{},//default c-tor value to invalid
  m_fitQuality(nullptr),
  m_trackSummary(nullptr)
{
  if (debug) std::cout<<"Trk::Track Ctor 1 :"<<this<<std::endl;

#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track::Track( const TrackInfo& info,
                   DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces, 
                   const FitQuality* fitQuality):
  m_cachedParameterVector {},
  m_cachedMeasurementVector {},
  m_cachedOutlierVector{}, 
  m_trackStateVector(trackStateOnSurfaces),
  m_perigeeParameters{},//default c-tor value to invalid
  m_fitQuality(fitQuality),
  m_trackSummary(nullptr),
  m_trackInfo( info )
{
  if (debug) std::cout<<"Trk::Track Ctor 2 :"<<this<<std::endl;
  //find the Perigee params they will become valid given the outcome
  findPerigeeImpl();
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}


Trk::Track::Track (const Track& rhs):
  m_cachedParameterVector {},
  m_cachedMeasurementVector {},
  m_cachedOutlierVector{}, 
  m_trackStateVector(nullptr),
  m_perigeeParameters{}, //default c-tor value to invalid
  m_fitQuality(nullptr),
  m_trackSummary(nullptr)
{
  if (debug) std::cout<<"Trk::Track Ctor 3 :"<<this<<std::endl;
  using namespace Trk;

  // check that not copying itself
  if (this!=&rhs)
  {
    // might as well use assignment operator
    // rather than duplicate it all
    (*this)=rhs;
  }

#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}


Trk::Track& Trk::Track::operator= (const Track& rhs)
{
  using namespace std;
  using namespace Trk;
  if (this!=&rhs)
  {
    // ++++ clear this object
    delete m_fitQuality;
    m_fitQuality=nullptr;
    delete m_trackSummary;
    m_trackSummary=nullptr;
    //Invalidate the caches
    m_cachedParameterVector.reset();
    m_cachedMeasurementVector.reset();
    m_cachedOutlierVector.reset();
    //The following is a DataVectors and so delete 
    //the contained objects automatically.
    delete m_trackStateVector;
    m_trackStateVector=nullptr;

    //set the author to be that of the Track being copied.
    m_trackInfo = rhs.m_trackInfo;

    // create & copy other variables
    if (rhs.fitQuality()!=nullptr){
      m_fitQuality = new FitQuality( *(rhs.m_fitQuality) );
    }
    // create & copy other variables
    if (rhs.trackSummary()!=nullptr){
      m_trackSummary = new TrackSummary( *(rhs.m_trackSummary) );
    }
    //perigee parameters set to invalid
    m_perigeeParameters.reset();
    
    //Create the TrackStateVector and the perigeeParamters
    if( rhs.m_trackStateVector!=0 )
    {
      m_trackStateVector = new DataVector<const TrackStateOnSurface>;
      m_trackStateVector->reserve(rhs.m_trackStateVector->size());
      TSoS_iterator itTSoSEnd = rhs.m_trackStateVector->end();
      for( TSoS_iterator itTSoS = rhs.m_trackStateVector->begin();
           itTSoS!=itTSoSEnd; ++itTSoS){
        assert(*itTSoS!=0); // check that is defined.
        TrackStateOnSurface* tsos = (**itTSoS).clone();
        m_trackStateVector->push_back( tsos );
        if(tsos!=nullptr){
          const Trk::Perigee*  perigee = dynamic_cast<const Trk::Perigee*>(tsos->trackParameters() ) ;
          if (perigee!=0 && tsos->type(TrackStateOnSurface::Perigee)){ 
            m_perigeeParameters.store(perigee);//Now they will be valid
          }
        }
      }
    }
  }
  return *this;
}

Trk::Track::~Track()
{
  if (debug) std::cout<<"Trk::Track dtor :"<<this<<std::endl;
  delete m_fitQuality;
  delete m_trackSummary;
  //the following is DataVectors and so delete the contained objects automatically.
  delete m_trackStateVector;

#ifndef NDEBUG
  s_numberOfInstantiations--; // delete Track, so decrement total count
#endif  
}

const DataVector<const Trk::TrackParameters>* Trk::Track::trackParameters() const
{	
  // Do work only if it is not valid.
  if ( !m_cachedParameterVector.isValid()){
    // create cached parameter vector (which DOES NOT OWN ELEMENTS ...
    DataVector<const Trk::TrackParameters> tmp_ParameterVector (SG::VIEW_ELEMENTS);
    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for (TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
      const TrackParameters* trackParameters = (*itTSoS)->trackParameters();
      // check to make sure that the TrackParameters exists first
      if (trackParameters!=0) tmp_ParameterVector.push_back( trackParameters );
    }
    m_cachedParameterVector.set(std::move(tmp_ParameterVector)); 
  }
  return m_cachedParameterVector.ptr();
}

void Trk::Track::findPerigee() const {
  if (!m_perigeeParameters.isValid()){
    findPerigeeImpl();
  }
} 

void Trk::Track::findPerigeeImpl() const
{
  // loop through all passed parameters and, if there is a Perigee in there,
  // assign it to Perigee parameters. Obviously there should never be more
  // than one perigee type. I could check for it, but it will make the
  // code slower which (in my opinion) makes it not worth doing. EJWM
  // there can be other objects, like VertexOnTrack measurements, with
  // params at a Perigee surface, thus an additional TSoS type check. AS/WL

 const Trk::Perigee* tmpPerigeeParameters=nullptr; 
  if (m_trackStateVector!=nullptr){
    DataVector<const TrackStateOnSurface>::const_iterator it = 
      m_trackStateVector->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itEnd = 
      m_trackStateVector->end();
    for( ; it!=itEnd; ++it )
    {
      tmpPerigeeParameters = dynamic_cast<const Trk::Perigee*>( (*it)->trackParameters() ) ;
      if (tmpPerigeeParameters!=0 && (*it)->type(TrackStateOnSurface::Perigee)){
        break; // found perigee so stop loop.
      }
    }
  }
  //set to value and valid
  if (tmpPerigeeParameters) {
    m_perigeeParameters.set(tmpPerigeeParameters);
  }
  return;
}

const DataVector<const Trk::MeasurementBase>* Trk::Track::measurementsOnTrack() const
{

  // We only need to do work if not valid.
  if ( !m_cachedMeasurementVector.isValid()){
    // create new DataVector which DOES NOT OWN ELEMENTS .
    DataVector< const Trk::MeasurementBase> tmpMeasurementVector(SG::VIEW_ELEMENTS);
    // for measurements on track it is very likely that #(meas) ~ #(TSOS)-> reserve(#(TSOS))
    tmpMeasurementVector.reserve(m_trackStateVector->size());

    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for ( TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
      //    if ((*itTSoS)->type(TrackStateOnSurface::Measurement) ) 
      if ( ! (*itTSoS)->type(TrackStateOnSurface::Outlier) ) 
      {
        const Trk::MeasurementBase* rot = (*itTSoS)->measurementOnTrack();
        // does it have a measurement ?
        if (rot!=0) tmpMeasurementVector.push_back( rot );
      }
    }
    m_cachedMeasurementVector.set(std::move(tmpMeasurementVector));
  }
  return m_cachedMeasurementVector.ptr();
}

const DataVector<const Trk::MeasurementBase>* Trk::Track::outliersOnTrack() const
{
  //We only need to do work if not valid
  if ( !m_cachedOutlierVector.isValid()){
    // create new DataVector which DOES NOT OWN ELEMENTS .
    DataVector< const Trk::MeasurementBase> tmpOutlierVector(SG::VIEW_ELEMENTS);
    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for ( TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
      if ((*itTSoS)->type(TrackStateOnSurface::Outlier) ) 
      {
        const Trk::MeasurementBase* rot = (*itTSoS)->measurementOnTrack();
        assert(rot!=0);
        tmpOutlierVector.push_back( rot );
      }
    }
    m_cachedOutlierVector.set(std::move(tmpOutlierVector));
  }
  return m_cachedOutlierVector.ptr();
}

void Trk::Track::reset(){
  m_cachedParameterVector.reset();
  m_cachedMeasurementVector.reset();
  m_cachedOutlierVector.reset();
  m_perigeeParameters.reset();
}

unsigned int Trk::Track::numberOfInstantiations() 
{

#ifndef NDEBUG 
  return s_numberOfInstantiations;
#else
  return 0;
#endif
}


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::Track& track)
{ 
  std::string name("Track ");
  sl <<name<<"Author = "<<track.info().dumpInfo()<<endmsg;
  if (track.fitQuality()!=0) sl << *(track.fitQuality() )<<endmsg;
  if (track.trackSummary()!=0) sl << *(track.trackSummary())<<endmsg;
  else sl << "No TrackSummary available in this track."<<endmsg;
  if (track.trackStateOnSurfaces() !=0)
  { 
    sl << name <<"has " << (track.trackStateOnSurfaces()->size()) << " trackStateOnSurface(s)" << endmsg;

    //level()sh shows the output level, currentLevel() 
    //shows what the stream is set to	
    if (sl.level()<MSG::INFO) 
    {
      // loop over TrackStateOnSurfaces if verbose turned on
      DataVector<const TrackStateOnSurface>::const_iterator it=track.trackStateOnSurfaces()->begin();
      int num=0;
      for (;it!=track.trackStateOnSurfaces()->end();++it)
      {
        sl<< " --------- Start of TrackStateOnSurface \t"<<num<<"\t-------"<<endmsg;
        sl<<(**it);
        sl<< " ---------   End of TrackStateOnSurface \t"<<num++<<"\t-------"<<endmsg;
      }
    }
  }
  return sl; 
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::Track& track)
{
  std::string name("Track ");
  sl <<name<<"Author = "<<track.info().dumpInfo()<<std::endl;
  if (track.fitQuality()!=0) sl << *(track.fitQuality() )<<std::endl;
  if (track.trackSummary()!=0) sl << *(track.trackSummary())<<std::endl;
  else sl << "No TrackSummary available in this track."<<std::endl;

  if (track.trackStateOnSurfaces() !=0)
  { 
    sl << name <<"has " << (track.trackStateOnSurfaces()->size()) << " trackStateOnSurface(s)" << std::endl;
    DataVector<const TrackStateOnSurface>::const_iterator it=track.trackStateOnSurfaces()->begin();
    int num=0;
    for (;it!=track.trackStateOnSurfaces()->end();++it)
    {
      sl<< " --------- Start of TrackStateOnSurface \t"<<num<<"\t-------"<<std::endl;
      sl<<(**it);
      sl<< " ---------   End of TrackStateOnSurface \t"<<num++<<"\t-------"<<std::endl;
    }
  }
  return sl; 
}
