/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

Trk::Track::Track()
  : m_trackStateVector(nullptr)
  , m_cachedParameterVector{}
  , m_cachedMeasurementVector{}
  , m_cachedOutlierVector{}
  , m_perigeeParameters{}
  , m_fitQuality(nullptr)
  , m_trackSummary(nullptr)
{
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track::Track(
  const TrackInfo& info,
  DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces,
  const FitQuality* fitQuality)
  : m_trackStateVector(trackStateOnSurfaces)
  , m_cachedParameterVector{}
  , m_cachedMeasurementVector{}
  , m_cachedOutlierVector{}
  , m_perigeeParameters{}
  , m_fitQuality(fitQuality)
  , m_trackSummary(nullptr)
  , m_trackInfo(info)
{
  // find the Perigee params they will become valid given the outcome
  findPerigeeImpl();
#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track::Track(const Trk::Track& rhs)
  : m_trackStateVector(nullptr)
  , m_cachedParameterVector{}
  , m_cachedMeasurementVector{}
  , m_cachedOutlierVector{}
  , m_perigeeParameters{}
  , m_fitQuality(nullptr)
  , m_trackSummary(nullptr)
{
  //Do the actual payload copy
  copyHelper(rhs);

#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track&
Trk::Track::operator=(const Trk::Track& rhs)
{
  if (this != &rhs) {
    // First clear this object
    delete m_fitQuality;
    m_fitQuality = nullptr;
    delete m_trackSummary;
    m_trackSummary = nullptr;
    // Invalidate the caches
    m_cachedParameterVector.reset();
    m_cachedMeasurementVector.reset();
    m_cachedOutlierVector.reset();
    // perigee parameters set to invalid
    m_perigeeParameters.reset();
    // The following is a DataVectors and so will delete
    // the contained objects automatically.
    delete m_trackStateVector;
    m_trackStateVector = nullptr;

    //copy payload of rhs to this
    copyHelper(rhs);
  }
  return *this;
}

void
Trk::Track::copyHelper(const Trk::Track& rhs)
{
  // set the author to be that of the Track being copied.
  m_trackInfo = rhs.m_trackInfo;

  // create & copy other variables if available
  if (rhs.fitQuality() != nullptr) {
    m_fitQuality = new Trk::FitQuality(*(rhs.m_fitQuality));
  }
  // create & copy other variables
  if (rhs.trackSummary() != nullptr) {
    m_trackSummary = new Trk::TrackSummary(*(rhs.m_trackSummary));
  }
  // Create the TrackStateVector and the perigeeParameters
  if (rhs.m_trackStateVector != nullptr) {
    m_trackStateVector = new DataVector<const Trk::TrackStateOnSurface>;
    m_trackStateVector->reserve(rhs.m_trackStateVector->size());

    TSoS_iterator itTSoSEnd = rhs.m_trackStateVector->end();
    for (TSoS_iterator itTSoS = rhs.m_trackStateVector->begin();
         itTSoS != itTSoSEnd;
         ++itTSoS) {
      assert(*itTSoS != nullptr); // check that is defined.
      // clone and store
      TrackStateOnSurface* tsos = (**itTSoS).clone();
      m_trackStateVector->push_back(tsos);
      // Check if this a perigee so we can already cache it
      if (tsos != nullptr && tsos->type(TrackStateOnSurface::Perigee)) {
        const Trk::Perigee* perigee = nullptr;
        const Trk::TrackParameters* tp = tsos->trackParameters();
        if (tp && tp->type() == Trk::AtaSurface &&
            tp->surfaceType() == Trk::Surface::Perigee) {
          perigee = static_cast<const Trk::Perigee*>(tp);
        }
        if (perigee != nullptr) {
          m_perigeeParameters.store(perigee); // Now they will be valid
        }
      }
    }
  }
}

Trk::Track::Track(Trk::Track&& rhs) noexcept
  : m_trackStateVector(std::move(rhs.m_trackStateVector))
  , m_cachedParameterVector(std::move(rhs.m_cachedParameterVector))
  , m_cachedMeasurementVector(std::move(rhs.m_cachedMeasurementVector))
  , m_cachedOutlierVector(std::move(rhs.m_cachedOutlierVector))
  , m_perigeeParameters(std::move(rhs.m_perigeeParameters))
  , m_fitQuality(std::move(rhs.m_fitQuality))
  , m_trackSummary(std::move(rhs.m_trackSummary))
{
  // Leave the rhs in valid
  // but undefined state make the ptr null.
  rhs.m_trackStateVector = nullptr;
  rhs.m_fitQuality = nullptr;
  rhs.m_trackSummary = nullptr;

#ifndef NDEBUG
  s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track&
Trk::Track::operator=(Trk::Track&& rhs) noexcept
{
  if (this != &rhs) {
    // First clear this object
    // same as dtor of the object
    delete m_fitQuality;
    delete m_trackSummary;
    delete m_trackStateVector;
    // move from rhs to this
    m_trackStateVector = std::move(rhs.m_trackStateVector);
    m_cachedParameterVector = std::move(rhs.m_cachedParameterVector);
    m_cachedMeasurementVector = std::move(rhs.m_cachedMeasurementVector);
    m_cachedOutlierVector = std::move(rhs.m_cachedOutlierVector);
    m_perigeeParameters = std::move(rhs.m_perigeeParameters);
    m_fitQuality = std::move(rhs.m_fitQuality);
    m_trackSummary = std::move(rhs.m_trackSummary);
    // Leave the rhs in valid
    // but undefined state make the ptr null.
    rhs.m_trackStateVector = nullptr;
    rhs.m_fitQuality = nullptr;
    rhs.m_trackSummary = nullptr;
  }
  return *this;
}

Trk::Track::~Track()
{
  delete m_fitQuality;
  delete m_trackSummary;
  // the following is DataVectors
  // and so delete the contained objects automatically.
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
      if (trackParameters!=nullptr) {
        tmp_ParameterVector.push_back( trackParameters );
      }
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
  // loop through all passed parameters and, if there is a at Perigee in there,
  // assign it to Perigee parameters. There should never be more
  // than one perigee type.
  // Note that there can be other objects, like VertexOnTrack measurements, with
  // params at a Perigee surface, thus the  TSoS check.

  const Trk::Perigee* tmpPerigeeParameters = nullptr;
  if (m_trackStateVector!=nullptr){
    DataVector<const TrackStateOnSurface>::const_iterator it =
      m_trackStateVector->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itEnd =
      m_trackStateVector->end();
    for (; it != itEnd; ++it) {
      if ((*it)->type(TrackStateOnSurface::Perigee)) {
        const Trk::TrackParameters* tp = (*it)->trackParameters();
        if (tp && tp->type() == Trk::AtaSurface &&
            tp->surfaceType() == Trk::Surface::Perigee) {
          tmpPerigeeParameters = static_cast<const Trk::Perigee*>(tp);
        }

        if (tmpPerigeeParameters != nullptr) {
          break; // found perigee so stop loop.
        }
      }
    }
  }
  //set to value and valid
  if (tmpPerigeeParameters) {
    m_perigeeParameters.set(tmpPerigeeParameters);
  }
}

const Trk::Perigee*
Trk::Track::perigeeParameters() const
{

  if (!m_perigeeParameters.isValid()) {
    // findPerigee performs the setting of the parameters
    // i.e does the CachedValue set
    findPerigeeImpl();
  }

  //Return payload if valid
  if(m_perigeeParameters.isValid()){
    return  *(m_perigeeParameters.ptr());
  }

  return nullptr;
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
        if (rot!=nullptr) { tmpMeasurementVector.push_back( rot );
        }
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

void Trk::Track::setTrackStateOnSurfaces(DataVector<const Trk::TrackStateOnSurface>*  input)
{
  delete m_trackStateVector;  // delete existing
  m_trackStateVector = input; // add new
  reset();                    // reset caches
}

void Trk::Track::setInfo(const TrackInfo& input)
{
  m_trackInfo = input;
}

void Trk::Track::setTrackSummary(Trk::TrackSummary* input)
{
  delete m_trackSummary;  // delete existing
  m_trackSummary = input; // add new
}

void Trk::Track::reset()
{
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
  if (track.fitQuality()!=nullptr) { sl << *(track.fitQuality() )<<endmsg;
}
  if (track.trackSummary()!=nullptr) { sl << *(track.trackSummary())<<endmsg;
  } else { sl << "No TrackSummary available in this track."<<endmsg;
}
  if (track.trackStateOnSurfaces() !=nullptr)
  {
    sl << name <<"has " << (track.trackStateOnSurfaces()->size()) << " trackStateOnSurface(s)" << endmsg;

    //level() shows the output level, currentLevel()
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
  if (track.fitQuality()!=nullptr) { sl << *(track.fitQuality() )<<std::endl;
  }
  if (track.trackSummary()!=nullptr) { sl << *(track.trackSummary())<<std::endl;
  } else { sl << "No TrackSummary available in this track."<<std::endl;
  }

  if (track.trackStateOnSurfaces() !=nullptr)
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
