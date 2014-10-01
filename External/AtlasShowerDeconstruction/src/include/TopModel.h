#ifndef TOPMODEL_H
#define TOPMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class TopModel : public Model {
    public:
      TopModel(Parameters &param, Flavour::id flavour);
      virtual ~TopModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      double hamiltonian(double aScale = 0);

    private:
      Flavour::id m_flavour;
  };

}

#endif

