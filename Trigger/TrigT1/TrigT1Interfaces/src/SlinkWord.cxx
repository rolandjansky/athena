
#include "TrigT1Interfaces/SlinkWord.h"

namespace LVL1CTP {

  SlinkWord::SlinkWord( unsigned int word )
    : m_slinkWord( word ) {

  }

  SlinkWord::~SlinkWord() {

  }

  /** returns Slink word */
  unsigned int SlinkWord::word() const {
    return m_slinkWord;
  }

} // namespace LVL1CTP
