#include "JetRec/setAssocObjectsSummary.h"

void
setAssocObjectsSummary(const std::vector<const xAOD::IParticle*>& constituents,
                       const std::string& label,
                       xAOD::Jet& jet) {
  
  double ptSum = std::accumulate(constituents.begin(),
                                 constituents.end(),
                                 0.0,
                                 [](double sumPt, const xAOD::IParticle* p){
                                   return sumPt + p->pt();});
  
  jet.setAttribute<float>(label + "Pt", ptSum);
  jet.setAttribute<int>(label+"Count", constituents.size());
}
