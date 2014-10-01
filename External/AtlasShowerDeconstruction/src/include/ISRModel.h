#ifndef ISRMODEL_H
#define ISRMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class ISRModel : public Model {
    public:
      ISRModel(Parameters &param);
      virtual ~ISRModel();

      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double hamiltonian(double pTsum = 0);
  };

}

#endif

