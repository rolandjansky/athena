/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to obtain barcode (or kine) for all charged tracks produced or
 reconstructable inside the Inner Detector acceptance.

 @todo AB: Only in ID acceptance? Why?
***************************************************************************/

#include <cmath>
#include <iomanip>
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "TruthUtils/HepMCHelpers.h"
#include "TruthTools/TruthSelector.h"


TruthSelector::TruthSelector (const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool          (type, name, parent),
    m_maxEta            (2.7),
    m_maxRIndet         (300.0*Gaudi::Units::mm),
    m_maxRStartPrimary  (10.0*Gaudi::Units::mm),
    m_maxRStartSecondary(400.0*Gaudi::Units::mm),
    m_maxZIndet         (300.0*Gaudi::Units::mm),
    m_maxZStartPrimary  (300.0*Gaudi::Units::mm),
    m_maxZStartSecondary(1500.0*Gaudi::Units::mm),
    m_mcEventsName      (""),
    m_minPt             (0.2*Gaudi::Units::GeV),
    m_minREndPrimary    (400.0*Gaudi::Units::mm),
    m_minREndSecondary  (1000.0*Gaudi::Units::mm),
    m_minZEndPrimary    (2300.0*Gaudi::Units::mm),
    m_minZEndSecondary  (3200.0*Gaudi::Units::mm)
{
    declareInterface<ITruthSelector>(this);
    declareProperty("maxEta",             m_maxEta);
    declareProperty("maxRIndet",          m_maxRIndet);
    declareProperty("maxRStartPrimary",   m_maxRStartPrimary);
    declareProperty("maxRStartSecondary", m_maxRStartSecondary);
    declareProperty("maxZIndet",          m_maxZIndet);
    declareProperty("maxZStartPrimary",   m_maxZStartPrimary);
    declareProperty("maxZStartSecondary", m_maxZStartSecondary);
    declareProperty("minPt",              m_minPt);
    declareProperty("minREndPrimary",     m_minREndPrimary);
    declareProperty("minREndSecondary",   m_minREndSecondary);
    declareProperty("minZEndPrimary",     m_minZEndPrimary);
    declareProperty("minZEndSecondary",   m_minZEndSecondary);
}


/// @todo Just leave as default (or at least inline)
TruthSelector::~TruthSelector() {  }


bool TruthSelector::selectParticle (HepMC::ConstGenParticlePtr particle, double minPt) {
    // skip null barcode
    int barCode = HepMC::barcode(particle);
    if (barCode == 0) return false;

    // G4 uses helper to select particle in final state.
    /// @note Temporary cut at 0.1mm for non-final state particles while truth helpers get sorted
    /// @todo Why this cut? Won't this exclude B decay products?
    if (particle->production_vertex() && particle->production_vertex()->position().perp() < 0.1*Gaudi::Units::mm && ! MC::isGenStable(particle)) return false;
    if (barCode >= 10100) return false; //< @todo Why?

    // known particle
    int pdgCode = particle->pdg_id();
    if (pdgCode == 0) return false;

    // charged particle
    const HepPDT::ParticleData*    particleData    = m_particleDataTable->particle(std::abs(pdgCode));
    if (! particleData || particleData->charge() == 0.) return false;

    // momentum and eta cut-offs
    if (particle->momentum().perp() < minPt || std::abs(particle->momentum().pseudoRapidity()) > m_maxEta) return false;

    return true;
}


StatusCode TruthSelector::initialize() {
    // print where you are
    ATH_MSG_INFO( "TruthSelector::initialize()" << "-package version " << PACKAGE_VERSION );

    // get the Particle Properties Service
    IPartPropSvc* partPropSvc;
    if (StatusCode::SUCCESS != service("PartPropSvc", partPropSvc, true)) {
      ATH_MSG_FATAL( " Could not initialize Particle Properties Service" );
      return StatusCode::FAILURE;
    }
    m_particleDataTable = partPropSvc->PDT();

    ATH_MSG_DEBUG( " maxEta " << m_maxEta << "  max vertex(R,Z) Primary "
           << m_maxRStartPrimary << ", " << m_maxZStartPrimary
           << "  Secondary "
           << m_maxRStartSecondary << ", " << m_maxZStartSecondary );

    return StatusCode::SUCCESS;
}


/// @todo Not needed -- this is a duplicate of inherited AlgTool::finalize()
StatusCode TruthSelector::finalize() { return StatusCode::SUCCESS; }


const std::map<int,int>& TruthSelector::indetKineMap() {
    m_indetKineMap.erase(m_indetKineMap.begin(),m_indetKineMap.end());

    // retrieve McEventCollection
    const McEventCollection*    mcCollection = mcEventCollection();
    if (! mcCollection) return m_indetKineMap;

    const HepMC::GenEvent*    genEvent = *mcCollection->begin();
    for (auto particle: *genEvent)    {
    if (! selectParticle(particle,m_minPt)) continue;

    // check start vertex in indet
    if (! particle->production_vertex()) continue;
    HepMC::FourVector     startVertex     = particle->production_vertex()->position();
    if (startVertex.perp()          > m_minREndPrimary
        || std::abs(startVertex.z())    > m_minZEndPrimary) continue;

    // kine/barcode map for G3 compatibility
    int kine = 0;

        // temporary for G4: kine == barcode
        kine = HepMC::barcode(particle);

    m_indetKineMap[kine] = HepMC::barcode(particle);
    }
    return m_indetKineMap;
}

const std::vector<int>&
TruthSelector::indetMuons (double minPt)
{
    // clear state
    m_barcodes.clear();

    // retrieve McEventCollection
    const McEventCollection*    mcCollection = mcEventCollection();
    if (! mcCollection) return m_barcodes;

    const HepMC::GenEvent*    genEvent = *mcCollection->begin();
    for (auto particle: *genEvent)    {
      // select charged particle in allowed pt/eta range
      if (! selectParticle(particle,minPt))        continue;

      // select muon pdgCode
      if (std::abs(particle->pdg_id()) != 13)        continue;

      // check production vertex inside indet envelope
      if (! particle->production_vertex())        continue;
      HepMC::FourVector     startVertex     = particle->production_vertex()->position();
      if (startVertex.perp()        > m_maxRIndet
      || std::abs(startVertex.z())    > m_maxZIndet)    continue;

    m_barcodes.push_back(HepMC::barcode(particle));
    ATH_MSG_DEBUG(" select barcode #" << m_barcodes.size()
              << " at production vertex R,Z " << startVertex.perp()
              << ", " << startVertex.z() );
    }
    return m_barcodes;
}

const McEventCollection*
TruthSelector::mcEventCollection (void)
{
    // retrieve McEventCollection
    const McEventCollection* mcCollection = 0;
    std::string key = "TruthEvent";
    if (StatusCode::FAILURE == evtStore()->retrieve(mcCollection,key))
    {
    key = "G4Truth";
    if (StatusCode::FAILURE == evtStore()->retrieve(mcCollection,key))
    {
        ATH_MSG_ERROR( "Could not find the McEventCollection" );
    }
    }

    return mcCollection;
}

const std::vector<int>&
TruthSelector::reconstructablePrimaries (double minPt)
{
    // clear state
    m_barcodes.clear();

    // retrieve McEventCollection
    const McEventCollection*    mcCollection = mcEventCollection();
    if (! mcCollection) return m_barcodes;

    ATH_MSG_DEBUG( "reconstructablePrimaries:" );
    const HepMC::GenEvent*    genEvent = *mcCollection->begin();
    for (auto particle: *genEvent)    {
    // select charged particle in allowed pt/eta range
    if (! selectParticle(particle,minPt))            continue;

    // reconstructable - depends on start/end vertex coords
    if (! particle->production_vertex()) continue;
    HepMC::FourVector     startVertex     = particle->production_vertex()->position();
    if (startVertex.perp()            > m_maxRStartPrimary
        || std::abs(startVertex.z())    > m_maxZStartPrimary)    continue;

    if (particle->end_vertex())
    {
        HepMC::FourVector     endVertex     = particle->end_vertex()->position();
        if (endVertex.perp()               < m_minREndPrimary
        && std::abs(endVertex.z())    < m_minZEndPrimary)    continue;
    }

    // print stuff
    if (msgLvl(MSG::DEBUG))
    {
        int    pdgCode        = particle->pdg_id();
        double pt        = particle->momentum().perp();
        int charge        = (*m_particleDataTable)[std::abs(pdgCode)]->charge();
        if (pdgCode < 0.) charge = -1;
        double mass        = (*m_particleDataTable)[std::abs(pdgCode)]->mass();
        double cotTheta    = particle->momentum().z()/particle->momentum().perp();
        double rEnd        = 0.;
        double zEnd        = 0.;
        if (particle->end_vertex())
        {
        rEnd    = particle->end_vertex()->position().perp();
        zEnd    = particle->end_vertex()->position().z();
        }
        ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
               << std::setw(6) << m_barcodes.size()
               << "  barcode" << std::setw(7)    << HepMC::barcode(particle)
               << "  start/end vertex R,Z"
               << std::setw(5) << std::setprecision(1)  << startVertex.perp() << ", "
               << std::setw(6) << std::setprecision(1)  << startVertex.z()    << " / "
               << std::setw(6) << std::setprecision(1)  << rEnd << ", "
               << std::setw(7) << std::setprecision(1)  << zEnd
               << "   pdg"    << std::setw(5) << pdgCode
               << "   status" << std::setw(3) << particle->status()
               << "   pt" << std::setw(7) << std::setprecision(1) << pt/Gaudi::Units::GeV
               << "  cotTheta" << std::setw(8) << std::setprecision(4) << cotTheta
               << "  eta" << std::setw(6) << std::setprecision(2)
               << particle->momentum().pseudoRapidity()
               << "  q" << std::setw(3) << charge
               << "  mass" << std::setw(7) << std::setprecision(4)
               << mass/Gaudi::Units::GeV );
    }

    m_barcodes.push_back(HepMC::barcode(particle));
    }
    return m_barcodes;
}

const std::vector<int>&
TruthSelector::reconstructableSecondaries (double minPt)
{
    // clear state
    m_barcodes.clear();

    // retrieve McEventCollection
    const McEventCollection*    mcCollection = mcEventCollection();
    if (! mcCollection) return m_barcodes;

    ATH_MSG_DEBUG( "reconstructableSecondaries:" );
    const HepMC::GenEvent*    genEvent = *mcCollection->begin();
    for (auto particle: *genEvent)    {
    // select charged final-state particle in allowed pt/eta range
    if (! selectParticle(particle,minPt))            continue;

    // reconstructable - depends on start/end vertex coords
    if (! particle->production_vertex()) continue;
    HepMC::FourVector    startVertex    = particle->production_vertex()->position();
    if ((startVertex.perp()            <= m_maxRStartPrimary
         && std::abs(startVertex.z())    <= m_maxZStartPrimary)
        || startVertex.perp()              >  m_maxRStartSecondary
        || std::abs(startVertex.z())    >  m_maxZStartSecondary)    continue;

    if (particle->end_vertex() != 0)
    {
        HepMC::FourVector      endVertex  = particle->end_vertex()->position();
        if (endVertex.perp()               < m_minREndSecondary
        && std::abs(endVertex.z())    < m_minZEndSecondary)        continue;
    }

    // print stuff
    if (msgLvl(MSG::DEBUG))
    {
        int    pdgCode        = particle->pdg_id();
        double pt        = particle->momentum().perp();
        int charge        = (*m_particleDataTable)[std::abs(pdgCode)]->charge();
        if (pdgCode < 0.) charge = -1;
        double mass        = (*m_particleDataTable)[std::abs(pdgCode)]->mass();
        double cotTheta    = particle->momentum().z()/particle->momentum().perp();
        double rEnd        = 0.;
        double zEnd        = 0.;
        if (particle->end_vertex() )
        {
        rEnd    = particle->end_vertex()->position().perp();
        zEnd    = particle->end_vertex()->position().z();
        }
        ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
               << std::setw(6) << m_barcodes.size()
               << "  barcode" << std::setw(7)    << HepMC::barcode(particle)
               << "  start/end vertex R,Z"
               << std::setw(5) << std::setprecision(1)  << startVertex.perp() << ", "
               << std::setw(6) << std::setprecision(1)  << startVertex.z()    << " / "
               << std::setw(6) << std::setprecision(1)  << rEnd << ", "
               << std::setw(7) << std::setprecision(1)  << zEnd
               << "   pdg"    << std::setw(5) << pdgCode
               << "   status" << std::setw(3) << particle->status()
               << "   pt" << std::setw(7) << std::setprecision(1) << pt/Gaudi::Units::GeV
               << "  cotTheta" << std::setw(8) << std::setprecision(4) << cotTheta
               << "  eta" << std::setw(6) << std::setprecision(2)
               << particle->momentum().pseudoRapidity()
               << "  q" << std::setw(3) << charge
               << "  mass" << std::setw(7) << std::setprecision(4)
               << mass/Gaudi::Units::GeV );
    }

    m_barcodes.push_back(HepMC::barcode(particle));
    }
    return m_barcodes;
}

