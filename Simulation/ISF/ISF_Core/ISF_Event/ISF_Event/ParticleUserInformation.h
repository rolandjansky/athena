/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleUserInformation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_PARTICLEUSERINFORMATION_H
#define ISF_EVENT_PARTICLEUSERINFORMATION_H

namespace ISF {

  /**
   @class ParticleUserInformation 

   Each ISFParticle carries a pointer to this class. For MC production,
   the pointer will normally be 0. For validation jobs a ParticleUserInformation
   instance may be attached to the ISFParticles.

   @author Elmar.Ritsch@cern.ch
  */

  class ParticleUserInformation {

    public:
      /** empty constructor */
    ParticleUserInformation();
    
      /** virtual destructor */
      virtual ~ParticleUserInformation() { };

      int getExtraBC() const;
      int process() const;
      int generation() const;

      void setExtraBC(int extrabc);
      void setProcess(int proc);
      void setGeneration(int gen);
    
    private:

      int  m_extrabc;     // extra barcode
      int  m_process;            // generating process
      int  m_generation;         // generation number (i.e. number of vertices separating vertex of origin
                                 //                    from the primary vertex (GenEvent input) 
  };
} // end of namespace


inline int ISF::ParticleUserInformation::getExtraBC()             const { return m_extrabc; }

inline int ISF::ParticleUserInformation::process()                const { return m_process; }

inline int ISF::ParticleUserInformation::generation()             const { return m_generation; }

inline void ISF::ParticleUserInformation::setExtraBC(int extrabc) { m_extrabc = extrabc; }

inline void ISF::ParticleUserInformation::setProcess(int proc)    { m_process = proc; }

inline void ISF::ParticleUserInformation::setGeneration(int gen)  { m_generation = gen; }

#endif // ISF_EVENT_PARTICLEUSERINFORMATION_H
