#include "TopParticleLevel/ParticleLevelEvent.h"

std::ostream& operator<<(std::ostream& os, const top::ParticleLevelEvent& plEvent) {

  
// os << *plEvent.m_info << "\n";
    os << "Electrons: " << plEvent.m_electrons->size() << "\n";
    for (const auto* const elPtr : *plEvent.m_electrons)
        os << " " << *elPtr << "\n";
    os << "Muons: " << plEvent.m_muons->size() << "\n";
    for (const auto* const muPtr : *plEvent.m_muons)
        os << " " << *muPtr << "\n";
    os << "Jets: " << plEvent.m_jets->size() << "\n";
    for (const auto* const jetPtr : *plEvent.m_jets)
        os << " " << *jetPtr << "\n";
    os << "Large jets: " << plEvent.m_largeRJets->size() << "\n";
    for (const auto* const jetPtr : *plEvent.m_largeRJets)
        os << " " << *jetPtr << "\n";
    if(plEvent.m_photons!=NULL){
    os << "Photons: " << plEvent.m_photons->size() << "\n";
        for (const auto* const photonPtr : *plEvent.m_photons)
            os << " " << *photonPtr << "\n";
    }
    os << *plEvent.m_met;
    return os;
}
