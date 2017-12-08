/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeneratorCenter_H
#define GeneratorCenter_H

class G4Event;
class G4PrimaryParticle;

#include "HepMC/GenEvent.h"

#include <string>
#include <map>

namespace FADS {

  class HepMCEventConverter;

  class FadsGenerator;
  class VertexManipulator;
  class ParticleManipulator;
  typedef std::map<std::string, FadsGenerator *,std::less<std::string> > genMap;

  typedef std::map<std::string,VertexManipulator *,std::less<std::string> >
  vtxModifiers;
  typedef std::map<std::string,ParticleManipulator *,std::less<std::string> >
  particleModifiers;

  class GeneratorCenter {
  protected:
    GeneratorCenter();
  private:
    GeneratorCenter(const GeneratorCenter&); //declaring, but not defining as an object of this class should never be copied.
    GeneratorCenter& operator= (const GeneratorCenter&); //declaring, but not defining as an object of this class should never be copied.

    vtxModifiers m_vertexMod;
    particleModifiers m_particleMod;
    static GeneratorCenter *m_thePointer;
    genMap m_generatorCatalog;
    FadsGenerator *m_currentGenerator;
    // for MCTruth
    const HepMC::GenEvent *m_hepmcEvent;
    bool m_eventIsDeleted;
    //std::string hepmcCollection; // Not used
    bool m_IncludeParentsInG4Event;

    const HepMCEventConverter *m_HepMCconverter;
  public:
    void SetHepMCEventConverter(const HepMCEventConverter*);

    void DeleteHepMCEvent(bool f) {m_eventIsDeleted=f;}
    ~GeneratorCenter();
    static GeneratorCenter* GetGeneratorCenter();
    void RegisterGenerator(FadsGenerator *);
    void GenerateAnEvent(G4Event *);
    void SelectGenerator(std::string);
    void InitializeGenerator();
    void PrintGeneratorList();
    void SetIncludeParentsInG4Event(const bool b){ m_IncludeParentsInG4Event=b; }
    bool GetIncludeParentsInG4Event() const { return m_IncludeParentsInG4Event; }

    void AddVertexManipulator(VertexManipulator *v);
    void AddParticleManipulator(ParticleManipulator *v);

    vtxModifiers::const_iterator BeginVertexManipulator()
    {
      return m_vertexMod.begin();
    }
    vtxModifiers::const_iterator EndVertexManipulator()
    {
      return m_vertexMod.end();
    }
    particleModifiers::const_iterator BeginParticleManipulator()
    {
      return m_particleMod.begin();
    }
    particleModifiers::const_iterator EndParticleManipulator()
    {
      return m_particleMod.end();
    }

    void HepMC2G4(const HepMC::GenEvent* ,G4Event *);

    const HepMC::GenEvent* GetGenEvent() const { return m_hepmcEvent; }

    //std::string GetHepMcCollection() const { return hepmcCollection; } // Not used
    //void SetHepMcCollection(std::string a) { hepmcCollection = a; } // Not used
  };

} // end namespace

#endif
