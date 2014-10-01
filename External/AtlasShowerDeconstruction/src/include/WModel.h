#ifndef WMODEL_H
#define WMODEL_H

#include "Model.h"
#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Parameters.h"
#include "Helper.h"

#include "StoredJet.h"

namespace Deconstruction {

  class WModel : public Model {
    public:
      WModel(Parameters &param, Flavour::id flavour = Flavour::Wp, bool includeAngleFactor = true);
      virtual ~WModel();

      //double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sumJets);
      double weight(const StoredJet &jets, fastjet::PseudoJet &sumJets);
      //void setTopset(const std::vector<fastjet::PseudoJet> &topset);
      void setTopset(const StoredJet &topset); // this is for the mother particle of the W, to be used in the angle factor
      double hamiltonian(double aScale = 0);

    private:
      Flavour::id m_flavour;
      //std::vector<fastjet::PseudoJet> m_topset;
      std::vector<int> m_topset;

      bool m_includeAngleFactor;
  };

}

#endif

