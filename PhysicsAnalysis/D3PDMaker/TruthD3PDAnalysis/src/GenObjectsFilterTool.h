// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenObjectsFilterTool.h 465784 2011-11-01 17:16:56Z aad $
/**
 * @file TruthD3PDAnalysis/src/GenObjectsFilterTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief filter for McEventCollection
*/
#ifndef TRUTHD3PDANALYSIS_GENOBJECTSFILTERTOOL_H
#define TRUTHD3PDANALYSIS_GENOBJECTSFILTERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "TruthD3PDAnalysis/IGenObjectsFilterTool.h"

//
#include <vector>

namespace D3PD {


/**
 *  @short Default implementation for the truth object selector tool
 *
 * @author Georges Aad <aad@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 465784 $
 * $Date: 2011-11-01 18:16:56 +0100 (Tue, 01 Nov 2011) $
 */
class GenObjectsFilterTool : virtual public IGenObjectsFilterTool, 
                             virtual public AthAlgTool  {

public:
   /// Default AlgTool constructor
   GenObjectsFilterTool( const std::string& type, const std::string& name,
                         const IInterface* parent );

   /// Initialization function for the tool
   virtual StatusCode initialize();

   /// Function selecting GenEvent objects
   virtual bool pass( const HepMC::GenEvent* evt,
                      const McEventCollection* coll = 0 ) const;
   /// Function selecting GenParticle objects
   virtual bool pass( HepMC::ConstGenParticlePtr part,
                      const McEventCollection* coll = 0 ) const;
   /// Function selecting GenVertex objects
   virtual bool pass( HepMC::ConstGenVertexPtr vtx,
                      const McEventCollection* coll = 0 ) const;

private:

  bool isBCHadron(HepMC::ConstGenParticlePtr part) const;
  bool isBC(int pdg) const;
  bool isKeep(int pdg) const;
  bool isRequested( HepMC::ConstGenParticlePtr part) const;
  bool passParticleCuts( HepMC::ConstGenParticlePtr part) const;
  bool isLeptonicWZ(HepMC::ConstGenParticlePtr part) const;



  /// Handle for the particle property service
  ServiceHandle< IPartPropSvc > m_partPropSvc;

  double m_ptMinCut; ///< Minimum track pT to be selected (in MeV)
  double m_etaCut; ///< Maximum track eta. Set to negative not to apply the cut

  bool m_removeEmptyEvents;
  bool m_removeDummyEvents;
  bool m_removeInTimePileUp;
  bool m_remove2BCPileUp;
  bool m_remove800nsPileUp;
  bool m_removeCavernBkg;
  bool m_selectTruthTracks; ///< Only select stable charged particles
  bool m_addOnlyFirstVertex;  ///< Only first vertex per gen event
  bool m_removeUnrequestedParticles; /// remove all particles that are not explicitly requested
  bool m_keepbcQuarks; /// keep b/c quarks
  bool m_keepbcQuarkAncestors; /// keep parents of b/c quarks (needed to reproduce HFor for example)
  bool m_keepbcHadrons; //// keep b/c hadrons
  bool m_keepbcHadronDecayChain; //// all particles from b/c hadrons including G4
  double m_bcHadronsDescendantsPtCut;
  int m_bcHadronsDescendantsBarcodeCut;
  double m_bcHadronsDescendantsEtaCut;
  std::vector<int> m_keepParticleWithPdgId;
  int m_keepFirstParticles; /// number of particles to keep starting from the begining
  bool m_keepLeptonicWZBosons; /// W/Z bosons decaying leptonically
  bool m_keepLeptonicWZBosonDecayChains; /// Descendants of W/Z bosons decaying leptonically; no G4
  bool m_keepLeptonicWZBosonsParents;  /// direct parents fo W/Z bosons
  bool m_keepStatusOneLeptonsFromWZ; /// status 1 leptons from WZ

}; // class GenObjectsFilterTool


} // namespace D3PD


#endif // TRUTHD3PDANALYSIS_GENOBJECTSFILTERTOOL_H
