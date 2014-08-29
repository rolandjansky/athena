/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeneratorCenter_H
#define GeneratorCenter_H

class G4Event;
class G4PrimaryParticle;

#include "HepMC/GenEvent.h"
//  #include "MCTGenParticle.hh"

#include <string>
#include <map>
//#include "FadsKinematics/GeneratorCenterMessenger.h"

namespace FADS {

class HepMCEventConverter;

class FadsGenerator;
class VertexManipulator;
class ParticleManipulator;
class G4EventAnalyzer;

typedef std::map<std::string, FadsGenerator *,std::less<std::string> > genMap;

typedef std::map<std::string,VertexManipulator *,std::less<std::string> >
vtxModifiers;
typedef std::map<std::string,ParticleManipulator *,std::less<std::string> >
particleModifiers;
typedef std::map<std::string,G4EventAnalyzer *,std::less<std::string> >
eventAnalyzers;

class GeneratorCenter {
protected:
	GeneratorCenter();
private:
	GeneratorCenter(const GeneratorCenter&); //declaring, but not defining as an object of this class should never be copied.   
        GeneratorCenter& operator= (const GeneratorCenter&); //declaring, but not defining as an object of this class should never be copied.

	vtxModifiers vertexMod;
	particleModifiers particleMod;
	eventAnalyzers evtAnalyses;
	static GeneratorCenter* thePointer;
	genMap generatorCatalog;
	FadsGenerator *currentGenerator;
        // for MCTruth
        HepMC::GenEvent* hepmcEvent;
	bool eventIsDeleted;
        std::string hepmcCollection;
        bool m_IncludeParentsInG4Event;

	const HepMCEventConverter *HepMCconverter;
public:
	void SetHepMCEventConverter(const HepMCEventConverter*);
	
	void DeleteHepMCEvent(bool f) {eventIsDeleted=f;}
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
	void AddEventAnalyzer(G4EventAnalyzer *v);
	vtxModifiers::const_iterator BeginVertexManipulator()
	{
		return vertexMod.begin();
	}
	vtxModifiers::const_iterator EndVertexManipulator()
	{
		return vertexMod.end();
	}
	particleModifiers::const_iterator BeginParticleManipulator()
	{
		return particleMod.begin();
	}
	particleModifiers::const_iterator EndParticleManipulator()
	{
		return particleMod.end();
	}
	eventAnalyzers::const_iterator BeginEventAnalyzer()
	{
		return evtAnalyses.begin();
	}
	eventAnalyzers::const_iterator EndEventAnalyzer()
	{
		return evtAnalyses.end();
	}

	void HepMC2G4(const HepMC::GenEvent* ,G4Event *);

        HepMC::GenEvent* GetGenEvent() const { return hepmcEvent; }

        std::string GetHepMcCollection() const { return hepmcCollection; }
        void SetHepMcCollection(std::string a) { hepmcCollection = a; }

        // iterators
//         typedef PrimaryMapG4MCT::const_iterator primary_const_iterator;
//         primary_const_iterator primaries_begin() const
//         { return g4primary2genMap.begin(); }
//         primary_const_iterator primaries_end() const
//         { return g4primary2genMap.end(); }

	void RandomNumberService(const std::string name) { m_randomnumberservice=name; }
	std::string RandomNumberService() const { return m_randomnumberservice; }
	void VertexOverrideFile(const std::string path) { m_vertexoverridefile=path; }
	std::string VertexOverrideFile() const { return m_vertexoverridefile; }
	void VertexOverrideEventFile(const std::string path) { m_vertexoverrideeventfile=path; }
	std::string VertexOverrideEventFile() const { return m_vertexoverrideeventfile; }
  std::string m_vertexoverridefile,m_vertexoverrideeventfile,m_randomnumberservice;

};

}	// end namespace

#endif
