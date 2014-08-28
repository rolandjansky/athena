#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"

namespace L1MUINT {

  MuCTPIToRoIBSLink::MuCTPIToRoIBSLink( std::vector<unsigned int> intVec )
    : m_MuCTPIToRoIBSLinkVector( intVec ) {

  }

  MuCTPIToRoIBSLink::~MuCTPIToRoIBSLink() {

  }

  const std::vector< unsigned int >& MuCTPIToRoIBSLink::getMuCTPIToRoIBWords() const {

    return m_MuCTPIToRoIBSLinkVector;

  }

} // namespace L1MUINT
