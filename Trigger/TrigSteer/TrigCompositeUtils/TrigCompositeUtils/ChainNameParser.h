/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCompositeUtils_ChainNameParser_h
#define TrigCompositeUtils_ChainNameParser_h

#include <string>
#include <vector>

namespace ChainNameParser {
   /*
    * returns multiplicities of the chain legs given the chain name.
    * e.g. for: HLT_2g10_loose_mu20 it would be [ 2 1 ]
    */
    std::vector<int> multiplicities(const std::string& chain);
    /*
    * returns signatures of the chain given the name
    * e.g. for: HLT_2g10_loose_mu20 it would be [ "g" "mu" ]
    * It matches the the multiplicities returned by the above method
    */
    std::vector<std::string> signatures(const std::string& chain);

}

#endif // 