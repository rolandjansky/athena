#ifndef STOREDJET_H
#define STOREDJET_H

#include <vector>
#include "Storage.h"
#include <fastjet/PseudoJet.hh>

namespace Deconstruction {

  class StoredJet {
    public:
      StoredJet();
      StoredJet(const StoredJet &s);
      StoredJet(const Storage &store);
      StoredJet(const Storage *store);
      StoredJet(const Storage &store, std::vector<int> list);
      virtual ~StoredJet();

      int getIdx(int i) const;
      std::vector<int> getList() const;
      const fastjet::PseudoJet &operator[](int i) const;
      const fastjet::PseudoJet &at(int i) const;
      int size() const;
      fastjet::PseudoJet sum() const;

      void push_back(int i);

      operator std::vector<fastjet::PseudoJet>() const;

      const Storage &store() const;

    private:
      const Storage *m_store;
      std::vector<int> m_list;
  };

}

#endif
