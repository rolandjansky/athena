/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "VxVertex/VxCandidate.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "GaudiKernel/MsgStream.h"

//#define DEBUG_CTOR_DTOR

namespace Trk
{
// Constructor 0
    TrackParticleBase::TrackParticleBase() :
        m_originalTrack(),
        m_trackParticleOrigin(NoVtx),
        m_elVxCandidate(),
        m_trackParameters(0),
        m_trackSummary(nullptr),
        m_fitQuality(nullptr),
	m_trackInfo()
    {
      #ifdef DEBUG_CTOR_DTOR	
      std::cout<<"TrackParticleBase blank ctr :"<<this<<std::endl;
      #endif
    }

    /** Merged old Constructors 0.5a, 2a and 1a from old (pre 13) TrackParticle constructors & added new info*/
    TrackParticleBase::TrackParticleBase(   const Track*                           trk, 
                                            const TrackParticleOrigin                   trkPrtOrigin, 
                                            const VxCandidate*                     vxCandidate,
                                            const TrackSummary*                    trkSummary, 
                                            std::vector<const TrackParameters*>&    parameters,
                                            const TrackParameters*                  definingParameter,
                                            const FitQuality*                      fitQuality)
        :
        m_originalTrack(),
        m_trackParticleOrigin( trkPrtOrigin  ), 
        m_elVxCandidate(),
        m_trackParameters( parameters ),
        m_trackSummary( trkSummary ),       
        m_fitQuality(fitQuality)
    {
      #ifdef DEBUG_CTOR_DTOR
      std::cout<<"TrackParticleBase ctr :"<<this<<std::endl;
      #endif

    // if no vxCandidate set, then trkPrtOrigin should be NoVtx
        assert ((vxCandidate!=0)||(trkPrtOrigin==NoVtx));
        
    // Defining parameter is always last.
        m_trackParameters.push_back(definingParameter);

    // establish element links
        if (trk!=nullptr) 
	{
	  m_originalTrack.setElement(trk);
	  m_trackInfo = trk->info();
	}
        if (vxCandidate!=nullptr) { m_elVxCandidate.setElement(vxCandidate);
}
    }
    
    TrackParticleBase::TrackParticleBase( const ElementLink<TrackCollection>& trackLink,
                                          const TrackParticleOrigin                   trkPrtOrigin, 
                                          const ElementLink<VxContainer>& vxCandidate,
                                          std::unique_ptr<Trk::TrackSummary> trkSummary,
                                          std::vector<const Trk::TrackParameters*>&&  parameters,
                                          std::unique_ptr<Trk::FitQuality> fitQuality,
                                          const TrackInfo& info)
        :
        m_originalTrack(trackLink),
        m_trackParticleOrigin( trkPrtOrigin  ), 
        m_elVxCandidate(vxCandidate),
        m_trackParameters( std::move(parameters) ),
        m_trackSummary( trkSummary.release() ),
        m_fitQuality(fitQuality.release()),
        m_trackInfo (info)
    {
      // if no vxCandidate set, then trkPrtOrigin should be NoVtx
      assert (!vxCandidate.isDefault() || trkPrtOrigin==NoVtx);
    }
    
/**
    Copy Constructor
*/
    TrackParticleBase::TrackParticleBase(const TrackParticleBase& rhs) 
        :
        m_originalTrack(rhs.m_originalTrack),
        m_trackParticleOrigin(rhs.m_trackParticleOrigin),
        m_elVxCandidate(rhs.m_elVxCandidate),
        m_trackParameters(),
        m_trackSummary( (rhs.m_trackSummary) ? new TrackSummary(*(rhs.m_trackSummary)) : nullptr ),
        m_fitQuality( (rhs.m_fitQuality) ? new FitQuality(*(rhs.m_fitQuality)) : nullptr ),
	m_trackInfo(rhs.m_trackInfo)
    {

        #ifdef DEBUG_CTOR_DTOR
        std::cout<<"TrackParticleBase copy ctr :"<<this<<std::endl;
        #endif

        std::vector<const TrackParameters*>::const_iterator it    = rhs.m_trackParameters.begin();
        std::vector<const TrackParameters*>::const_iterator itEnd = rhs.m_trackParameters.end();
        for (; it!=itEnd; ++it) { m_trackParameters.push_back( (*it)->clone() );
}
    }

/**
    Assignment operator
*/
    TrackParticleBase& TrackParticleBase::operator= (const TrackParticleBase& rhs)
    {
        if (this!=&rhs)
        {
    // only delete objects where these two pointers point to
    // the other pointers point to objects in storegate 
            if (m_trackSummary!=nullptr) { delete m_trackSummary; m_trackSummary = nullptr; }
            if (m_fitQuality  !=nullptr) { delete m_fitQuality  ; m_fitQuality   = nullptr; }
            std::vector<const TrackParameters*>::const_iterator it    = m_trackParameters.begin();
            std::vector<const TrackParameters*>::const_iterator itEnd = m_trackParameters.end();
            for (; it!=itEnd; ++it) { delete (*it);
}
            
            m_originalTrack             =   rhs.m_originalTrack;
            m_trackParticleOrigin       =   rhs.m_trackParticleOrigin;
            m_elVxCandidate             =   rhs.m_elVxCandidate;
            
            //the following are owned, and so must be copied.
            m_trackSummary              =   (rhs.m_trackSummary) ? new TrackSummary(*(rhs.m_trackSummary)) : nullptr;
            m_fitQuality                =   (rhs.m_fitQuality) ? new FitQuality(*(rhs.m_fitQuality)) : nullptr;
            m_trackInfo                 =    rhs.m_trackInfo;
	    
            it    = rhs.m_trackParameters.begin();
            itEnd = rhs.m_trackParameters.end();
            for (; it!=itEnd; ++it) { m_trackParameters.push_back( (*it)->clone() );
}
        }
        return *this;
    }

    TrackParticleBase& TrackParticleBase::operator= (TrackParticleBase&& rhs)
    {
        if (this!=&rhs)
        {
          delete m_trackSummary;
          m_trackSummary = rhs.m_trackSummary;
          rhs.m_trackSummary = nullptr;

          delete m_fitQuality;
          m_fitQuality = rhs.m_fitQuality;
          rhs.m_fitQuality = nullptr;

          for (const TrackParameters* p : m_trackParameters) {
            delete p;
}

          m_trackParameters = std::move (rhs.m_trackParameters);
            
          m_originalTrack             =   rhs.m_originalTrack;
          m_trackParticleOrigin       =   rhs.m_trackParticleOrigin;
          m_elVxCandidate             =   rhs.m_elVxCandidate;
            
          m_trackInfo                 =    rhs.m_trackInfo;
        }
        return *this;
    }

/**
    Destructor
*/
    TrackParticleBase::~TrackParticleBase() {
        #ifdef DEBUG_CTOR_DTOR
        std::cout<<"TrackParticleBase dtor :"<<this<<std::endl;
        #endif
        std::vector<const TrackParameters*>::const_iterator it    = m_trackParameters.begin();
        std::vector<const TrackParameters*>::const_iterator itEnd = m_trackParameters.end();
        for (; it!=itEnd; ++it) { delete (*it);
}
        if (m_trackSummary!=nullptr) { delete m_trackSummary;
}
        if (m_fitQuality  !=nullptr) { delete m_fitQuality;
}
    }
    
    MsgStream& TrackParticleBase::dump( MsgStream& sl ) const
    {       
        // DO NOT specificy an output level like MSG::VERBOSE (should be done by the caller)
        sl << "Printing TrackParticle. OriginType: " << this->particleOriginType() << endmsg;
        if (this->trackElementLink()->isValid())
        {
          sl << "The ElementLink<TrackCollection> is valid." << endmsg;
        } else
        {
          sl << "The ElementLink<TrackCollection> is NOT valid." << endmsg;
        }
        if ( this->fitQuality() !=nullptr ) {          sl << * ( this->fitQuality() ) <<endmsg;
}
        if ( this->trackSummary() !=nullptr ) {        sl << * ( this->trackSummary() ) <<endmsg;
}
        // vertex EL should alwasy be there as it is in ESD and AOD
        if ( this->reconstructedVertex() !=nullptr ) { sl << ( this->reconstructedVertex()->recVertex() ) <<endmsg;
}
        const std::vector<const TrackParameters*> trackParameters = this->trackParameters();
        sl << "TrackParticle has " << trackParameters.size() << " track parameters. Printing them:" << endmsg;
        for (std::vector<const TrackParameters*>::const_iterator itr = trackParameters.begin() ; itr != trackParameters.end() ; ++itr)
        {
          sl << (**itr) << endmsg;
        }
	sl<<"TrackInfo inherited from original Track comes below:"<<endmsg;
	sl<<m_trackInfo.dumpInfo();          
        return sl;
    }
    
    std::ostream& TrackParticleBase::dump( std::ostream& sl ) const
    {
        sl << "Printing TrackParticle. OriginType: " << this->particleOriginType() << std::endl;
        if (this->trackElementLink()->isValid())
        {
          sl << "The ElementLink<TrackCollection> is valid." << std::endl;
        } else
        {
          sl << "The ElementLink<TrackCollection> is NOT valid." << std::endl;
        }
        if ( this->fitQuality() !=nullptr ) {          sl << * ( this->fitQuality() ) <<std::endl;
}
        if ( this->trackSummary() !=nullptr ) {        sl << * ( this->trackSummary() ) <<std::endl;
}
        // vertex EL should alwasy be there as it is in ESD and AOD
        if ( this->reconstructedVertex() !=nullptr ) { sl << ( this->reconstructedVertex()->recVertex() ) <<std::endl;
}
        const std::vector<const TrackParameters*> trackParameters = this->trackParameters();
        sl << "TrackParticle has " << trackParameters.size() << " track parameters. Printing them:" << std::endl;
        for (std::vector<const TrackParameters*>::const_iterator itr = trackParameters.begin() ; itr != trackParameters.end() ; ++itr)
        {
          sl << (**itr) << std::endl;
        }      
	sl<<"TrackInfo inherited from original Track comes below:"<<std::endl;
	sl<<m_trackInfo.dumpInfo();      
        return sl;
    }
    
    MsgStream& operator << ( MsgStream& sl, const TrackParticleBase& trackParticleBase)
    { 
      trackParticleBase.dump(sl);
      return sl;
    }
    
    std::ostream& operator << ( std::ostream& sl, const TrackParticleBase& trackParticleBase)
    {
      trackParticleBase.dump(sl);
      return sl;
    }
}
