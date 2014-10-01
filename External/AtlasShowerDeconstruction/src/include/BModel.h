#ifndef BMODEL_H
#define BMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"

#include "StoredJet.h"

namespace Deconstruction {

  class BModel : public Model {
    public:
      BModel(Parameters &param, Flavour::id flavourb, Flavour::id flavourt);
      virtual ~BModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sum);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sum);
      //void setTopset(const std::vector<fastjet::PseudoJet> &topset);
      void setTopset(const StoredJet &topset);
      double hamiltonian(double aScale = 0);

    private:
      Flavour::id m_flavourb;
      Flavour::id m_flavourt;
      //std::vector<fastjet::PseudoJet> m_topset;
      std::vector<int> m_topset;
  };

}

#endif

