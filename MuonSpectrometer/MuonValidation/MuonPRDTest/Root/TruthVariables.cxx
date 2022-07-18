/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/TruthVariables.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
namespace MuonPRDTest {
    TruthVariables::TruthVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "TruthTestModule", false, msglvl), m_key{container_name} {}
    bool TruthVariables::declare_keys() { return declare_dependency(m_key); }

    bool TruthVariables::fill(const EventContext& ctx) {
        SG::ReadHandle<McEventCollection> truthContainer{m_key, ctx};
        if (!truthContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve container " << m_key.fullKey());
            return false;
        }
        unsigned int truth_vertices{0}, truth_parts{0};
        for (auto it : *truthContainer) {
            const HepMC::GenEvent* subEvent = it;
#ifdef HEPMC3
            for (auto vertex : subEvent->vertices()) {
#else
            // Vertex
            HepMC::ConstGenEventVertexRange vertex_range = subEvent->vertex_range();
            for (auto vit : vertex_range) {
                const HepMC::GenVertex* vertex = vit;
#endif
                m_Truth_vertex.push_back(vertex->position());
                m_Truth_vertexId.push_back(vertex->id());
                ++truth_vertices;
            }

#ifdef HEPMC3
            for (auto particle : subEvent->particles()) {
#else
            // Particle
            HepMC::ConstGenEventParticleRange particle_range = subEvent->particle_range();
            for (auto pit : particle_range) {
                const HepMC::GenParticle* particle = pit;
#endif
                m_truthParticleP4.push_back(particle->momentum());
                m_Truth_particlePdg_id.push_back(particle->pdg_id());
                m_Truth_particleStatus.push_back(particle->status());
                m_Truth_particleBarcode.push_back(HepMC::barcode(particle));
                auto production_vertex = particle->production_vertex();
                m_Truth_particleProduction_vertex_id.push_back(production_vertex ? production_vertex->id() : -1);
                auto end_vertex = particle->end_vertex();
                m_Truth_particleEnd_vertex_id.push_back(end_vertex ? end_vertex->id() : -1);
                ++truth_parts;
            }
        }
        m_Truth_nVertices = truth_vertices;
        m_Truth_nParticles = truth_parts;
        return true;
    }
}  // namespace MuonPRDTest