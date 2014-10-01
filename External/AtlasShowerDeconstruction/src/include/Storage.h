#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <fastjet/PseudoJet.hh>

namespace Deconstruction {

  class Storage {
    public:
      Storage();
      virtual ~Storage();

      std::vector<fastjet::PseudoJet> get(const std::vector<int> &list) const;
      fastjet::PseudoJet sum(const std::vector<int> &list) const;
      int size() const;

      void set(const std::vector<fastjet::PseudoJet> &input);
      std::vector<fastjet::PseudoJet> &get();
      const std::vector<fastjet::PseudoJet> &get() const;

      fastjet::PseudoJet &operator[](int i);
      const fastjet::PseudoJet &operator[](int i) const;

    private:
      std::vector<fastjet::PseudoJet> m_input;
  };

}

#endif

