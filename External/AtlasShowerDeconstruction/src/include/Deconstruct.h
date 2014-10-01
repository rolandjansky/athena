#ifndef DECONSTRUCT_H
#define DECONSTRUCT_H

#include "Parameters.h"
#include "Model.h"

#include <map>

namespace Deconstruction {

  class Deconstruct {

    public:

      Deconstruct(Parameters &param, Model &signal, Model &background, Model &isr);
      virtual ~Deconstruct();

      Model &signal();
      Model &background();
      Model &isr();
      Parameters &param();

      double deconstruct(std::vector<fastjet::PseudoJet> &input,
                         double &wSignal, double &wBackground);

      const std::multimap<double, std::vector<fastjet::PseudoJet> > &signalWeight() const;

    protected:
      Parameters &m_param;

      Model &m_signal;
      Model &m_background;
      Model &m_isr;

      std::multimap<double, std::vector<fastjet::PseudoJet> > m_signalWeight;

  };

}

#endif

