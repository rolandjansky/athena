/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/expect.h"
#include "TrigCompositeUtils/ChainNameParser.h"

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v) {
    for ( auto el : v )
        o << el << " ";
    return o;
}

int main() {
    VALUE( ChainNameParser::multiplicities("HLT_mu20_ivaloose") ) EXPECTED( {1} );
    VALUE( ChainNameParser::multiplicities("HLT_2mu20_ivaloose") ) EXPECTED( {2} );
    VALUE( ChainNameParser::multiplicities("HLT_2mu20_ivaloose_g20") ) EXPECTED( {2, 1} );
    VALUE( ChainNameParser::multiplicities("HLT_2mu20_ivaloose_5g166_lhloose") ) EXPECTED( {2, 5} );
    
    VALUE( ChainNameParser::signatures("HLT_mu20_ivaloose") ) EXPECTED( {"mu"} );
    VALUE( ChainNameParser::signatures("HLT_2mu20_ivaloose") ) EXPECTED( {"mu"} );
    VALUE( ChainNameParser::signatures("HLT_2mu20_ivaloose_g20") ) EXPECTED( {"mu", "g"} );
    VALUE( ChainNameParser::signatures("HLT_2mu20_ivaloose_5g166_lhloose") ) EXPECTED( {"mu", "g"} );


}