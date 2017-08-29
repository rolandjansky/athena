/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * dual-use tool for converting xAOD truth events back to HepMC
 * Principal Authors (responsible for the core conversion algorithm): Josh McFayden and James Catmore
 * Tool Author: Jim Lacey (Carleton University) 
 * ... updated tool interface to be dual-use
 * ... added missing PDF information and requirements to allow running on full sim (remove Geant4 secondaries)
 * <james.lacey@cern.ch,jlacey@physics.carleton.ca>
 * <dag.gillberg@cern.ch>
 */

#ifndef TRUTHCONVERTERS_XAODTOHEPMCTOOL_H
#define TRUTHCONVERTERS_XAODTOHEPMCTOOL_H 1

#include "AsgTools/AsgTool.h"
#include "GenInterfaces/IxAODtoHepMCTool.h"


class xAODtoHepMCTool: public asg::AsgTool, public virtual IxAODtoHepMCTool { 
 public: 
   ASG_TOOL_CLASS( xAODtoHepMCTool , IxAODtoHepMCTool )
   xAODtoHepMCTool( const std::string& name );
   virtual ~xAODtoHepMCTool () { };

   virtual StatusCode  initialize() override;
   StatusCode finalize () override;

 public:
   std::vector<HepMC::GenEvent> getHepMCEvents(const xAOD::TruthEventContainer* xTruthEventContainer, const xAOD::EventInfo* eventInfo) override;

 private:
   const HepMC::GenEvent createHepMCEvent(const xAOD::TruthEvent* xEvt, const xAOD::EventInfo* eventInfo);
   HepMC::GenVertex* vertexHelper(const xAOD::TruthVertex*,std::map<const xAOD::TruthVertex*,HepMC::GenVertex*>&,bool&);
   HepMC::GenParticle* createHepMCParticle(const xAOD::TruthParticle*);
   HepMC::GenVertex* createHepMCVertex(const xAOD::TruthVertex*);          
   void printxAODEvent(const xAOD::TruthEvent* event, const xAOD::EventInfo* eventInfo) const;
   
 private:
   /// Input container key (job property)
   float m_momFac,m_lenFac;
   bool m_signalOnly;
   int m_maxCount;
   int m_evtCount;
   /// Counters
   int m_badSuggest;
   int m_noProdVtx;
   int m_badBeams;

}; 

#endif //> !XAODTOHEPMC_XAODTOHEPMCTOOL_H
