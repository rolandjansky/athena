
#ifndef ELEMENTLINKDICT_${namespace}
#define ELEMENTLINKDICT_${namespace}

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthContainers/tools/AuxTypeVectorFactory.h"

namespace DictTools {

struct ElementLinkDict${namespace} {

  ElementLink< ${cont} > m_link;
  ElementLinkVector< ${cont} > m_linkVector;
  SG::ELVRef< ${cont} > m_sgref;
  SG::AuxTypeVectorFactory<ElementLink< ${cont} > > m_factory;
};

}// end DictTools namespace

#endif //ELEMENTLINKDICT_${namespace}
