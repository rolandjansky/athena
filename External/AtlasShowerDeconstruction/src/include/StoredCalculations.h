#ifndef STOREDCALCULATIONS_H
#define STOREDCALCULATIONS_H

#include "StoredJet.h"
#include <map>
#include <vector>
#include "Helper.h"

namespace Deconstruction {

  #define StoredKey std::vector<long long>

  class StoredCalculations {
    public:
      StoredCalculations();
      virtual ~StoredCalculations();

      bool check(StoredKey, double &w);
      void store(StoredKey, double w);
      void clear();

    private:
      std::map<StoredKey, double> m_table;
  };

}

#endif

