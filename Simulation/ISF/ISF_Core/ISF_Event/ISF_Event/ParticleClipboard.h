/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleClipboard.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_PARTICLECLIPBOARD_H
#define ISF_EVENT_PARTICLECLIPBOARD_H

namespace ISF {

  // forward declarations
  class ISFParticle;

  /**
   @class ParticleClipboard

   This singleton is used to pass current particle information between
   different namespaces, ie functions.
   
   Once going to multi-threading this will cause problems quite clearly
   since multiple threads will need individual clipboards. Therefore 
   the use of this singleton needs to be replaced by properly propagating
   current particle information from one function to the next.

   @author Elmar.Ritsch -at- cern.ch
   */

  class ParticleClipboard {

    public:
      /** get the singleton instance */
      inline static ParticleClipboard& getInstance();

      /** save a particle on the clipboard */
      inline void                      setParticle(const ISF::ISFParticle &p);
      /** get the particle from the clipboard */
      inline const ISF::ISFParticle   *getParticle() const;

    private :
      /** private constructor */
      ParticleClipboard();
      /** empty copy constructor */
      ParticleClipboard(ParticleClipboard const&);
      /** empty assignemtn operator */
      void operator=(ParticleClipboard const&);

      /** the actual particle in the clipboard */
      const ISFParticle      *m_particle;
  };

  //
  // inlined methods
  //

  /** inline getInstance() method to retrieve the singleton */
  ISF::ParticleClipboard& ISF::ParticleClipboard::getInstance() {
    static ParticleClipboard   staticClipboard;
    return staticClipboard;
  }

  /** inline setParticle() method to keep a particle on the clipboard */
  void ISF::ParticleClipboard::setParticle(const ISF::ISFParticle &p) {
    m_particle = &p;
  }

  /** inline getParticle() method to get a particle from the clipboard */
  const ISF::ISFParticle *ISF::ParticleClipboard::getParticle() const {
    return m_particle;
  }

} // end of namespace

#endif // ISF_EVENT_PARTICLECLIPBOARD_H

