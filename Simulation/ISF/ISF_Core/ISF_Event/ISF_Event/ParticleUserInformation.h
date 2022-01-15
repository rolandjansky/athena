/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleUserInformation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_PARTICLEUSERINFORMATION_H
#define ISF_EVENT_PARTICLEUSERINFORMATION_H

#include <cmath>

namespace ISF {

  /**
     @class ParticleUserInformation

     Each ISFParticle carries a pointer to this class. For MC production,
     the pointer will normally be 0. For validation jobs a ParticleUserInformation
     instance may be attached to the ISFParticles.
     2015-05-12 (S.Todorova) extended to carry presampled interaction/traversed material info for Fatras

     @author Elmar.Ritsch@cern.ch
  */

  struct MaterialPathInfo
  {
    float dMax;       // path limit after which sampled process occurs
    float dCollected; // material thickness traversed so far
    int   process;    // type of pre-sampled material process

    MaterialPathInfo( int proc, float pathLimit, float pathCurrent ) :
      dMax(pathLimit), dCollected(pathCurrent), process(proc)
    {}
    /**
     * collected material update
     */
    void updatePath( float d) { dCollected += d; }

    /** comparison */
    bool operator==(const MaterialPathInfo& rhs) const {
      float epsilon = 1e-6;
      bool pass = true;
      pass &= std::fabs(dMax-rhs.dMax) < epsilon;
      pass &= std::fabs(dCollected-rhs.dCollected) < epsilon;
      pass &= process == rhs.process;
      return pass;
    }
  };

  class ParticleUserInformation {

  public:
    /** empty constructor */
    ParticleUserInformation();

    /** virtual destructor */
    virtual ~ParticleUserInformation() { delete m_matInfo; };

    int getExtraBC() const;
    int process() const;
    int generation() const;
    MaterialPathInfo* materialLimit() const;

    void setExtraBC(int extrabc);
    void setProcess(int proc);
    void setGeneration(int gen);
    void setMaterialLimit(int process, float x0lim, float x0coll);

    /** Comparison */
    bool operator==(const ParticleUserInformation& rhs) const;

  private:

    int  m_process;            // generating process
    int  m_generation;         // generation number (i.e. number of vertices separating vertex of origin
    //                    from the primary vertex (GenEvent input)
    mutable MaterialPathInfo*   m_matInfo;     // presampled process and material collection
  };
} // end of namespace


inline int ISF::ParticleUserInformation::process()                const { return m_process; }

inline int ISF::ParticleUserInformation::generation()             const { return m_generation; }

inline ISF::MaterialPathInfo* ISF::ParticleUserInformation::materialLimit()   const { return m_matInfo; }

inline void ISF::ParticleUserInformation::setProcess(int proc)    { m_process = proc; }

inline void ISF::ParticleUserInformation::setGeneration(int gen)  { m_generation = gen; }

inline void ISF::ParticleUserInformation::setMaterialLimit(int proc, float dMax, float d)
{  delete m_matInfo; m_matInfo = new ISF::MaterialPathInfo(proc,dMax,d); }

#endif // ISF_EVENT_PARTICLEUSERINFORMATION_H
