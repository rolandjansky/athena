/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
TrackParticle.cxx  -  Description
    -------------------
    begin   : 27-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

***************************************************************************/

#include "Particle/TrackParticle.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GaudiKernel/MsgStream.h"

#include <sstream>

namespace Rec
{

  // Constructor 0
    TrackParticle::TrackParticle()
      : Trk::TrackParticleBase()
      , P4PxPyPzE()
      , NavigableTerminalNode()
  {}

    TrackParticle::TrackParticle(
      const Trk::Track* trk,
      const Trk::TrackParticleOrigin trkPrtOrigin,
      const Trk::VxCandidate* vxCandidate,
      const Trk::TrackSummary* trkSummary,
      std::vector<const Trk::TrackParameters*>& parameters,
      const Trk::TrackParameters* definingParameter,
      const Trk::FitQuality* fitQuality)
      : Trk::TrackParticleBase(trk,
                               trkPrtOrigin,
                               vxCandidate,
                               trkSummary,
                               parameters,
                               definingParameter,
                               fitQuality)
      , P4PxPyPzE(definingParameter->momentum()[Trk::px],
                  definingParameter->momentum()[Trk::py],
                  definingParameter->momentum()[Trk::pz],
                  sqrt(pow(Trk::ParticleMasses().mass[Trk::pion], 2) +
                       pow(definingParameter->momentum()[Trk::px], 2) +
                       pow(definingParameter->momentum()[Trk::py], 2) +
                       pow(definingParameter->momentum()[Trk::pz], 2)))
      , NavigableTerminalNode()
    {}

    TrackParticle::TrackParticle(
      const ElementLink<TrackCollection>& trackLink,
      const Trk::TrackParticleOrigin trkPrtOrigin,
      const ElementLink<VxContainer>& vxCandidate,
      std::unique_ptr<Trk::TrackSummary> trkSummary,
      // passes ownership of elements.
      std::vector<const Trk::TrackParameters*>&& parameters,
      std::unique_ptr<Trk::FitQuality> fitQuality,
      const Trk::TrackInfo& info,
      const P4PxPyPzE& mom)
      : Trk::TrackParticleBase(trackLink,
                               trkPrtOrigin,
                               vxCandidate,
                               std::move(trkSummary),
                               std::move(parameters),
                               std::move(fitQuality),
                               std::move(info))
      , P4PxPyPzE(mom)
      , NavigableTerminalNode()
    {}

    /**
      Copy Constructor
    */
    TrackParticle::TrackParticle(const TrackParticle& rhs)
      : IAthenaBarCode(rhs)
      , INavigable(rhs)
      , I4Momentum(rhs)
      , INavigable4Momentum(rhs)
      , P4PxPyPzEBase(rhs)
      , Trk::TrackParticleBase(rhs)
      , P4PxPyPzE(rhs)
      , NavigableTerminalNode(rhs)
      , m_abc(rhs.m_abc)
    {}

    /**
      Assignment operator
    */
    TrackParticle&
    TrackParticle::operator=(const TrackParticle& rhs)
    {
      if (this != &rhs) {
        P4PxPyPzE::operator=(rhs);
        Trk::TrackParticleBase::operator=(rhs);
        m_abc = rhs.m_abc;
      }
      return *this;
    }

    TrackParticle& TrackParticle::operator= (TrackParticle&& rhs)
    {
        if (this!=&rhs)
        {
          P4PxPyPzE::operator= (rhs);
          m_abc = rhs.m_abc;
          Trk::TrackParticleBase::operator=(std::move(rhs));
        }
        return *this;
    }

  /**
    Destructor
  */
    TrackParticle::~TrackParticle() 
    {
    }

  /** 
      removeSummary()
  */
  void TrackParticle::removeSummary()
  {
    delete m_trackSummary;
    m_trackSummary=0;
  }

  void TrackParticle::removeFitQuality()
  {
    delete m_fitQuality;
    m_fitQuality=0;
  }

  void TrackParticle::removeErrorMatrix()
  {
    for (std::vector<const Trk::TrackParameters*>::iterator iter =
           this->m_trackParameters.begin();
         iter != this->m_trackParameters.end();
         ++iter) {
      /* If this is a measured perigee, then we clone it without error matrix
        and replace the Perigee with its clone */
      const Trk::Perigee* measPg=dynamic_cast<const Trk::Perigee*>(*iter);
      if (measPg) {
       *iter=measPg->clone();
       delete measPg;
      }
    }
  }

  MsgStream& Rec::TrackParticle::dump( MsgStream& sl ) const
  {       
      Trk::TrackParticleBase::dump( sl );
      std::ostringstream os;
      m_abc.dump(os);
      sl<<std::endl<<os.str();
      return sl;
  }
  
  std::ostream& Rec::TrackParticle::dump( std::ostream& sl ) const
  {
      Trk::TrackParticleBase::dump( sl );
      sl<<std::endl;
      m_abc.dump(sl);
      return sl;
  }

  /** set 4Momentum (will throw exception if cannot be implemented) */
  void TrackParticle::set4Mom (const I4Momentum * const )
  {
    std::cout << " FATAL ERROR : TrackParticle::set4Mom called. Changing the 4 "
                 "momentum of the TrackParticle is not allowed! Aborting!"
              << std::endl;
    std::abort();
    return;
  }

  /** set 4Momentum (will throw exception if cannot be implemented) */
  void TrackParticle::set4Mom (const I4Momentum & )
  {
    std::cout << " FATAL ERROR : TrackParticle::set4Mom called. Changing the 4 "
                 "momentum of the TrackParticle is not allowed! Aborting!"
              << std::endl;
    std::abort();
    return;
  }

  /** set 4Momentum (will throw exception if cannot be implemented) */
  void TrackParticle::set4Mom (const CLHEP::HepLorentzVector & )
  {
    std::cout << " FATAL ERROR : TrackParticle::set4Mom called. Changing the 4 "
                 "momentum of the TrackParticle is not allowed! Aborting!"
              << std::endl;
    std::abort();
    return;
  }


////////////////////////////////////////////////
// IAthenaBarCode implementation
////////////////////////////////////////////////

  AthenaBarCode_t TrackParticle::getAthenaBarCode() const {
    return m_abc.getAthenaBarCode();
  }

  void TrackParticle::setAthenaBarCode(AthenaBarCode_t id) {
    m_abc.setAthenaBarCode(id);
  }

  //Comparing & Versioning Control
  bool TrackParticle::hasSameAthenaBarCode(const IAthenaBarCode &bc) const {
    return m_abc.hasSameAthenaBarCode(bc);
  }

  bool TrackParticle::hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &bc) const {
    return m_abc.hasSameAthenaBarCodeExceptVersion(bc);
  }

  AthenaBarCodeVersion_t TrackParticle::getVersion() const {
    return m_abc.getVersion();
  }

  void TrackParticle::newVersion() {
    m_abc.newVersion();
  }

  void TrackParticle::setVersion(AthenaBarCodeVersion_t newversion) {
    m_abc.setVersion(newversion);
  }

////////////////////////////////////////////////
// Finished Passing all interface of IAthenaBarCode
////////////////////////////////////////////////




}//end of namespace definitions

//the sream operatoes are defined in the global namespace 
//header files. Making the same here.

  MsgStream& operator << ( MsgStream& sl, const Rec::TrackParticle& trackParticle)
  { 
    trackParticle.dump(sl);
    return sl;
  }
  
  std::ostream& operator << ( std::ostream& sl, const Rec::TrackParticle& trackParticle)
  {
    trackParticle.dump(sl);
    return sl;
  }

