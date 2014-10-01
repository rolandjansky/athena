#ifndef TOPGLUONMODEL_H
#define TOPGLUONMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class TopGluonModel : public Model {
    public:
      TopGluonModel(Parameters &param);
      virtual ~TopGluonModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      double hamiltonian(double aScale = 0);

  };

}

#endif

