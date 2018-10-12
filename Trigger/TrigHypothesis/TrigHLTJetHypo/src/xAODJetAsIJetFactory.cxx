#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"

xAODJetAsIJetFactory::xAODJetAsIJetFactory(): m_ind(-1){}

pHypoJet xAODJetAsIJetFactory::operator() (const xAOD::Jet* j){
  return new HypoJet::xAODJetAsIJet(j, ++m_ind);
}

