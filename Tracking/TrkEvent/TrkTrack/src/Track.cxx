/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

unsigned int Trk::Track::s_numberOfInstantiations=0;
static bool debug=false;

Trk::Track::Track ():
    m_cachedParameterVector ( 0 ),
    m_cachedMeasurementVector ( 0 ),
    m_cachedOutlierVector( 0 ), 
    m_trackStateVector( 0 ),
    m_perigeeParameters( 0 ),
    m_fitQuality(0),
//    m_trackInfo(0),
    m_perigeeSearch( false ),
    m_trackSummary(0)
{
    if (debug) std::cout<<"Trk::Track Ctor 1 :"<<this<<std::endl;
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Track, so increment total count
#endif
}

Trk::Track::Track( const TrackInfo& info,
                   DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces, 
                   const FitQuality* fitQuality):
    m_cachedParameterVector ( 0 ),
    m_cachedMeasurementVector ( 0 ),
    m_cachedOutlierVector( 0 ), 
    m_trackStateVector(trackStateOnSurfaces),
    m_perigeeParameters( 0 ),
    m_fitQuality(fitQuality),
    m_trackInfo( info ),
    m_perigeeSearch( true ),
    m_trackSummary(0)
{
    if (debug) std::cout<<"Trk::Track Ctor 2 :"<<this<<std::endl;
    using namespace std;

    findPerigee();
    
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Track, so increment total count
#endif
}


Trk::Track::Track (const Track& rhs):
    m_cachedParameterVector (0),
    m_cachedMeasurementVector (0),
    m_cachedOutlierVector(0), 
    m_trackStateVector( 0 ),
    m_perigeeParameters( 0 ),
    m_fitQuality(0),
//    m_trackInfo(0),
    m_perigeeSearch( true ),
    m_trackSummary(0)
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
        m_fitQuality=0;
        delete m_trackSummary;
        m_trackSummary=0;
        //the following are all DataVectors and so delete the contained objects automatically.
        delete m_cachedParameterVector;
        m_cachedParameterVector=0;
        delete m_cachedMeasurementVector;
        m_cachedMeasurementVector=0;
        delete m_cachedOutlierVector;
        m_cachedOutlierVector=0;
        delete m_trackStateVector;
        m_trackStateVector=0;
    
        //reset perigee parameters
        m_perigeeParameters=0;

        //set the author to be that of the Track being copied.
        //       if( m_trackInfo!=0) delete m_trackInfo;
        //	m_trackInfo = 0;
        //        if (rhs.info()!=0)m_trackInfo = new TrackInfo(*(rhs.m_trackInfo));
        m_trackInfo = rhs.m_trackInfo;

        // create & copy other variables
        if (rhs.fitQuality()!=0)
            m_fitQuality = new FitQuality( *(rhs.m_fitQuality) );
        // create & copy other variables
        if (rhs.trackSummary()!=0)
            m_trackSummary = new TrackSummary( *(rhs.m_trackSummary) );

        if( rhs.m_trackStateVector!=0 )
        {
            m_trackStateVector = new DataVector<const TrackStateOnSurface>;
            m_trackStateVector->reserve(rhs.m_trackStateVector->size());
            TSoS_iterator itTSoSEnd = rhs.m_trackStateVector->end();
            for( TSoS_iterator itTSoS = rhs.m_trackStateVector->begin();
                 itTSoS!=itTSoSEnd; ++itTSoS    )
            {
                assert(*itTSoS!=0); // check that is defined.
                TrackStateOnSurface* tsos = (**itTSoS).clone();
                m_trackStateVector->push_back( tsos );
                if (m_perigeeParameters==0) 
                    m_perigeeParameters = dynamic_cast<const Trk::Perigee*>( tsos->trackParameters() ) ;
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
    //the following are all DataVectors and so delete the contained objects automatically.
    delete m_trackStateVector;
    // and m_perigeeParameters should not be deleted.
    // (the actual object is deleted when m_trackStateVector is deleted)
  
    // these two are "cached" DataVectors, and shouldn't delete their contained objects.
//    if(m_trackInfo) delete m_trackInfo;
    delete m_cachedParameterVector;
    delete m_cachedMeasurementVector;
    delete m_cachedOutlierVector;
#ifndef NDEBUG
    s_numberOfInstantiations--; // delete Track, so decrement total count
#endif  
}

const DataVector<const Trk::TrackParameters>* Trk::Track::trackParameters() const
{	
    // check if the cached vector has been created.
    if ( m_cachedParameterVector!=0) return m_cachedParameterVector;
  
    // create cached parameter vector (which DOES NOT OWN ELEMENTS ...
    // to prevent double deletion later on)
    m_cachedParameterVector = new DataVector<const Trk::TrackParameters>(SG::VIEW_ELEMENTS);
  
    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for (TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
        const TrackParameters* trackParameters = (*itTSoS)->trackParameters();
        // check to make sure that the TrackParameters exists first
        if (trackParameters!=0) m_cachedParameterVector->push_back( trackParameters );
    }
  
    return m_cachedParameterVector;
}

void Trk :: Track :: findPerigee() const
{
    // loop through all passed parameters and, if there is a Perigee in there,
    // assign it to Perigee parameters. Obviously there should never be more
    // than one perigee type. I could check for it, but it will make the
    // code slower which (in my opinion) makes it not worth doing. EJWM
    // there can be other objects, like VertexOnTrack measurements, with
    // params at a Perigee surface, thus an additional TSoS type check. AS/WL
    if (not m_trackStateVector) return; //coverity 106171 Explicit null dereferenced
    DataVector<const TrackStateOnSurface>::const_iterator it = 
            m_trackStateVector->begin();
    DataVector<const TrackStateOnSurface>::const_iterator itEnd = 
            m_trackStateVector->end();

    for( ; it!=itEnd; ++it )
    {
        m_perigeeParameters = 
            dynamic_cast<const Trk::Perigee*>( (*it)->trackParameters() ) ;
        if (m_perigeeParameters!=0 && (*it)->type(TrackStateOnSurface::Perigee))
            break; // found perigee so stop loop.
    }

    m_perigeeSearch = true;
}

const DataVector<const Trk::MeasurementBase>* Trk::Track::measurementsOnTrack() const
{

    // So now we check if the cached vector has been created.
    if ( m_cachedMeasurementVector!=0) return m_cachedMeasurementVector;
  
    // create new DataVector which DOES NOT OWN ELEMENTS .
    // .. to prevent double deletion later on)
    m_cachedMeasurementVector 
        = new DataVector< const Trk::MeasurementBase>(SG::VIEW_ELEMENTS);
    // for measurements on track it is very likely that #(meas) ~ #(TSOS)-> reserve(#(TSOS))
    m_cachedMeasurementVector->reserve(m_trackStateVector->size());

    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for ( TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
        //    if ((*itTSoS)->type(TrackStateOnSurface::Measurement) ) 
        if ( ! (*itTSoS)->type(TrackStateOnSurface::Outlier) ) 
        {
            const Trk::MeasurementBase* rot = (*itTSoS)->measurementOnTrack();
            // does it have a measurement ?
            if (rot!=0) m_cachedMeasurementVector->push_back( rot );
        }
    }
    return m_cachedMeasurementVector;
}

const DataVector<const Trk::MeasurementBase>* Trk::Track::outliersOnTrack() const
{

    // So now we check if the cached vector has been created.
    if ( m_cachedOutlierVector!=0) return m_cachedOutlierVector;
  
    // create new DataVector which DOES NOT OWN ELEMENTS .
    // .. to prevent double deletion later on)
    m_cachedOutlierVector 
        = new DataVector< const Trk::MeasurementBase>(SG::VIEW_ELEMENTS);
    TSoS_iterator itTSoSEnd = m_trackStateVector->end();
    for ( TSoS_iterator itTSoS = m_trackStateVector->begin(); itTSoS!=itTSoSEnd; ++itTSoS)
    {
        if ((*itTSoS)->type(TrackStateOnSurface::Outlier) ) 
        {
            const Trk::MeasurementBase* rot = (*itTSoS)->measurementOnTrack();
            assert(rot!=0);
            m_cachedOutlierVector->push_back( rot );
        }
    }
    return m_cachedOutlierVector;
}

unsigned int Trk::Track::numberOfInstantiations() 
{
    return s_numberOfInstantiations;
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
