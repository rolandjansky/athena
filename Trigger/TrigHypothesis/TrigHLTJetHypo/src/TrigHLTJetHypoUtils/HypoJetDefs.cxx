/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

std::ostream& operator << (std::ostream& out, const HypoJetVector& hjv) {
  auto hjv_addr = static_cast<const void*>(&hjv);
  out << "HypoJetVector (" << hjv_addr << ")\n";
  for(const auto& j : hjv) {
    out << static_cast<const void*>(j.get())
	<< " e " << j->e()
	<< " et " << j->et()
	<< " eta " << j->eta()
	<< " phi " << j->phi() << '\n';
  }
  return out;
}
