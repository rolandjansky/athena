// Solution by Wim Lavrijsen to handle matrices in ROOT
// see https://hypernews.cern.ch/HyperNews/Atlas/get/Prelimbugs/731/2.html
//
#include <vector>

#ifdef __MAKECINT__
#pragma link C++ class std::vector< std::vector< float > >;
#endif

template class vector< vector< float > >;

