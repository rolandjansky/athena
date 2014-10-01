#ifndef HBBMODEL_H
#define HBBMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class HBBModel : public Model {
    public:
      HBBModel(Parameters &param);
      virtual ~HBBModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      double hamiltonian(double pTsum = 0);

  };

}

#endif

