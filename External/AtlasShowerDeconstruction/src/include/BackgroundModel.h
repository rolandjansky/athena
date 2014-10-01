#ifndef BACKGROUNDMODEL_H
#define BACKGROUNDMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class BackgroundModel : public Model {
    public:
      BackgroundModel(Parameters &param, Flavour::id flav = Flavour::g);
      virtual ~BackgroundModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      double hamiltonian(double pTsum = 0);

    private:
      Flavour::id m_flav;
  };

}

#endif

