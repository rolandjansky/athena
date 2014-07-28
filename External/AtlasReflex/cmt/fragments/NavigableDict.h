

#ifndef NAVIGABLEDICT_${namespace}
#define NAVIGABLEDICT_${namespace}
#include "Navigation/Navigable.h"

namespace DictTools {

struct NavigableDict${namespace} {
  
    std::pair< ElementLink< ${cont} > , double > m_pair; 
    Navigable< ${cont} > m_navDouble; 
    Navigable< ${cont},double> m_nav; 

};

}// end DictTools namespace
#endif //NAVIGABLEDICT_${namespace}
