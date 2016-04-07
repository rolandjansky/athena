/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExpressionEvaluation/xAODVariableProxyLoaders.h"
#include "AthContainers/normalizedTypeinfoName.h"

#include "TClass.h"

#define TRY_TYPE(DATANAME, VT_PREFIX, TYPE, VT_TYPE) do { \
  if (*ti == typeid(TYPE)) { \
    BaseAccessorWrapper *accWrap = new AccessorWrapper<TYPE>(varname); \
    if (accWrap && accWrap->isValid(DATANAME)) { \
      m_accessorCache[varname] = accWrap;  \
      return VT_PREFIX ## VT_TYPE; \
    } else if (accWrap) { \
      accessorNotAvailableForThisContainer = true; \
      delete accWrap; \
    } \
  } \
} while(0)

#define TRY_ALL_KNOWN_TYPES(DATANAME, VT_PREFIX) do { \
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance(); \
  SG::auxid_t auxid = r.findAuxID(varname); \
  if (auxid != SG::null_auxid) { \
    bool accessorNotAvailableForThisContainer = false; \
    const std::type_info *ti = r.getType(auxid); \
    TRY_TYPE(DATANAME, VT_PREFIX, int, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, bool, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, unsigned int, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, char, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, uint8_t, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, unsigned short, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, short, INT); \
    TRY_TYPE(DATANAME, VT_PREFIX, float, DOUBLE); \
    TRY_TYPE(DATANAME, VT_PREFIX, double, DOUBLE); \
    if (!accessorNotAvailableForThisContainer) { \
      throw std::runtime_error("Unsupported aux element type '"+r.getTypeName(auxid)+"' for '"+varname+"'"); \
    } \
  } \
} while(0)

#define TRY_TMETHOD_WRAPPER() do { \
  const std::type_info &containerTypeinfo = typeid(*(m_auxElement->container())); \
  TClass *containerClass = TClass::GetClass(containerTypeinfo); \
  if (!containerClass) { \
    containerClass = TClass::GetClass(SG::normalizedTypeinfoName(containerTypeinfo).c_str()); \
  } \
  if (containerClass) { \
    TMethodWrapper* accWrap(0);						\
    if( !strcmp(containerClass->GetName(),"SG::AuxElementStandaloneData") ) { /* special case where the element type is the aux element */ \
      accWrap = new TMethodWrapper( typeid(*m_auxElement) , varname ); \
    } else {							\
      TVirtualCollectionProxy* collProxy = containerClass->GetCollectionProxy(); \
      if(collProxy) { \
	const std::type_info &elementTypeinfo = *(collProxy->GetValueClass()->GetTypeInfo()); \
	std::cout << " element type = " << System::typeinfoName( elementTypeinfo ) << std::endl; \
	accWrap = new TMethodWrapper(elementTypeinfo, varname); \
      }									\
    }								\
    if (accWrap && accWrap->isValid(m_auxElement)) { \
      m_accessorCache[varname] = accWrap;  \
      return accWrap->variableType(); \
    } \
    else if(accWrap) delete accWrap;		\
  } \
} while(0)

#define TRY_TMETHOD_COLLECTION_WRAPPER() do { \
  TMethodCollectionWrapper *accWrap = new TMethodCollectionWrapper(typeid(*m_auxVectorData), varname); \
  if (accWrap && accWrap->isValid(m_auxVectorData)) { \
    m_accessorCache[varname] = accWrap;  \
    return accWrap->variableType(); \
  } \
  else delete accWrap; \
} while(0)


namespace ExpressionParsing {

  TMethodWrapper::TMethodWrapper(const std::type_info &elementTypeinfo, 
      const std::string &methodName)
    : m_methodCall(NULL), m_valid(false)
  {
    TClass *cls = TClass::GetClass(elementTypeinfo);
    if (!cls) {
      cls = TClass::GetClass(SG::normalizedTypeinfoName(elementTypeinfo).c_str());
      if (!cls) return;
    }

    m_methodCall = new TMethodCall(cls, methodName.c_str(), "");
    if (!m_methodCall) return;

    m_valid = m_methodCall->IsValid();
  }

  TMethodWrapper::~TMethodWrapper()
  {
    if (m_methodCall) {
      delete m_methodCall;
      m_methodCall = NULL;
    }
  }

  IProxyLoader::VariableType TMethodWrapper::variableType() const
  {
    if (!m_methodCall) return IProxyLoader::VT_UNK;
    switch (m_methodCall->ReturnType()) {
      case TMethodCall::kLong:
        return IProxyLoader::VT_INT;
      case TMethodCall::kDouble: 
        return IProxyLoader::VT_DOUBLE;
      case TMethodCall::kString:
      case TMethodCall::kOther:
      case TMethodCall::kNone:
      default:                   return IProxyLoader::VT_UNK;
    }
  }

  bool TMethodWrapper::isValid(const SG::AuxElement *) const
  {
    return m_valid;
  }

  bool TMethodWrapper::isValid(const SG::AuxVectorData *) const
  {
    return m_valid;
  }

  int TMethodWrapper::getIntValue(const SG::AuxElement *auxElement) const
  {
    long retLong;
    m_methodCall->Execute((void *)auxElement, retLong);
    return (int) retLong;
  }

  double TMethodWrapper::getDoubleValue(const SG::AuxElement *auxElement) const
  {
    double retDouble;
    m_methodCall->Execute((void *)auxElement, retDouble);
    return retDouble;
  }

  std::vector<int> TMethodWrapper::getVecIntValue(const SG::AuxVectorData *) const
  {
    throw std::runtime_error("TMethodWrapper doesn't deal with containers");
  }

  std::vector<double> TMethodWrapper::getVecDoubleValue(const SG::AuxVectorData *) const
  {
    throw std::runtime_error("TMethodWrapper doesn't deal with containers");
  }




  TMethodCollectionWrapper::TMethodCollectionWrapper(const std::type_info &containerTypeinfo, 
      const std::string &methodName)
    : m_collectionProxy(nullptr),
      m_methodCall(NULL), m_valid(false)
  {
    TClass *containerClass = TClass::GetClass(containerTypeinfo);
    if (!containerClass) {
      containerClass = TClass::GetClass(SG::normalizedTypeinfoName(containerTypeinfo).c_str());
      if (!containerClass) return;
    }

    m_collectionProxy = containerClass->GetCollectionProxy();
    if (!m_collectionProxy) return;

    TClass *elementClass = m_collectionProxy->GetValueClass();
    if (!elementClass) return;

    m_methodCall = new TMethodCall(elementClass, methodName.c_str(), "");
    if (!m_methodCall) return;

    m_valid = m_methodCall->IsValid();
  }

  TMethodCollectionWrapper::~TMethodCollectionWrapper()
  {
    if (m_methodCall) {
      delete m_methodCall;
      m_methodCall = NULL;
    }
  }

  IProxyLoader::VariableType TMethodCollectionWrapper::variableType() const
  {
    if (!m_methodCall) return IProxyLoader::VT_UNK;
    switch (m_methodCall->ReturnType()) {
      case TMethodCall::kLong:
        return IProxyLoader::VT_VECINT;
      case TMethodCall::kDouble: 
        return IProxyLoader::VT_VECDOUBLE;
      case TMethodCall::kString:
      case TMethodCall::kOther:
      case TMethodCall::kNone:
      default:                   return IProxyLoader::VT_UNK;
    }
  }

  bool TMethodCollectionWrapper::isValid(const SG::AuxElement *) const
  {
    return m_valid;
  }

  bool TMethodCollectionWrapper::isValid(const SG::AuxVectorData *) const
  {
    return m_valid;
  }

  int TMethodCollectionWrapper::getIntValue(const SG::AuxElement *) const
  {
    throw std::runtime_error("TMethodCollectionWrapper doesn't deal with scalars");
  }

  double TMethodCollectionWrapper::getDoubleValue(const SG::AuxElement *) const
  {
    throw std::runtime_error("TMethodCollectionWrapper doesn't deal with scalars");
  }

  std::vector<int> TMethodCollectionWrapper::getVecIntValue(const SG::AuxVectorData *auxVectorData) const
  {
    long retLong;
    m_collectionProxy->PushProxy((void *)auxVectorData);
    const UInt_t N = m_collectionProxy->Size();
    std::vector<int> result(N);
    for (UInt_t i = 0; i < N; ++i) {
      void *element = (*m_collectionProxy)[i];
      m_methodCall->Execute(element, retLong);
      result[i] = (int) retLong;
    }
    m_collectionProxy->PopProxy();
    return result;
  }

  std::vector<double> TMethodCollectionWrapper::getVecDoubleValue(const SG::AuxVectorData *auxVectorData) const
  {
    double retDouble;
    m_collectionProxy->PushProxy((void *)auxVectorData);
    const UInt_t N = m_collectionProxy->Size();
    std::vector<double> result(N);
    for (UInt_t i = 0; i < N; ++i) {
      void *element = (*m_collectionProxy)[i];
      m_methodCall->Execute(element, retDouble);
      result[i] = (double) retDouble;
    }
    m_collectionProxy->PopProxy();
    return result;
  }




  xAODElementProxyLoader::xAODElementProxyLoader()
    : m_auxElement(NULL)
  {
  }

  xAODElementProxyLoader::xAODElementProxyLoader(const SG::AuxElement *auxElement)
    : m_auxElement(auxElement)
  {
  }

  xAODElementProxyLoader::~xAODElementProxyLoader()
  {
    reset();
  }

  void xAODElementProxyLoader::reset()
  {
    for (auto &x : m_accessorCache) {
      if (x.second != NULL) {
        delete x.second;
      }
      x.second = NULL;
    }
    m_accessorCache.clear();
  }

  void xAODElementProxyLoader::setData(const SG::AuxElement *auxElement)
  {
    m_auxElement = auxElement;
  }

  IProxyLoader::VariableType xAODElementProxyLoader::variableTypeFromString(const std::string &varname)
  {
    TRY_TMETHOD_WRAPPER();
    TRY_ALL_KNOWN_TYPES(m_auxElement, VT_);
    return VT_UNK;
  }

  int xAODElementProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    return m_accessorCache[varname]->getIntValue(m_auxElement);
  }

  double xAODElementProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    return m_accessorCache[varname]->getDoubleValue(m_auxElement);
  }

  std::vector<int> xAODElementProxyLoader::loadVecIntVariableFromString(const std::string &)
  {
    throw std::runtime_error("xAODElementProxyLoader can't load vector types");
  }

  std::vector<double> xAODElementProxyLoader::loadVecDoubleVariableFromString(const std::string &)
  {
    throw std::runtime_error("xAODElementProxyLoader can't load vector types");
  }




  xAODVectorProxyLoader::xAODVectorProxyLoader()
    : m_auxVectorData(NULL)
  {
  }

  xAODVectorProxyLoader::xAODVectorProxyLoader(const SG::AuxVectorData *auxVectorData)
    : m_auxVectorData(auxVectorData)
  {
  }

  xAODVectorProxyLoader::~xAODVectorProxyLoader()
  {
    reset();
  }

  void xAODVectorProxyLoader::reset()
  {
    for (auto &x : m_accessorCache) {
      if (x.second != NULL) {
        delete x.second;
      }
      x.second = NULL;
    }
    m_accessorCache.clear();
  }

  void xAODVectorProxyLoader::setData(const SG::AuxVectorData *auxVectorData)
  {
    m_auxVectorData = auxVectorData;
  }

  IProxyLoader::VariableType xAODVectorProxyLoader::variableTypeFromString(const std::string &varname)
  {
    TRY_TMETHOD_COLLECTION_WRAPPER();
    TRY_ALL_KNOWN_TYPES(m_auxVectorData, VT_VEC);

    // Before giving up completely, check the size of the vector. If it's
    // 0, it may be that it's empty on *all* events of the current input
    // file. Meaning that dynamic variables will be missing from each event.
    if( m_auxVectorData->size_v() == 0 ) {
       // Let's claim a vector<double> type, that seems to be the safest bet.
       // Even if the variable should actually be vector<int>, this is a
       // simple conversion at least.
       return VT_VECDOUBLE;
    }

    return VT_UNK;
  }

  int xAODVectorProxyLoader::loadIntVariableFromString(const std::string &)
  {
    throw std::runtime_error("xAODVectorProxyLoader can't load scalar types");
  }

  double xAODVectorProxyLoader::loadDoubleVariableFromString(const std::string &)
  {
    throw std::runtime_error("xAODVectorProxyLoader can't load scalar types");
  }

  std::vector<int> xAODVectorProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    return m_accessorCache[varname]->getVecIntValue(m_auxVectorData);
  }

  std::vector<double> xAODVectorProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
     // Check whether we have an accessor already:
     std::map< std::string, BaseAccessorWrapper* >::iterator itr;
     if( ( itr = m_accessorCache.find( varname ) ) == m_accessorCache.end() ) {
        // For an empty container let's not bother too much:
        if( m_auxVectorData->size_v() == 0 ) {
           return std::vector< double >();
        }
        // If it's not empty, then let the variableTypeFromString function
        // figure out the variable type, and create the accessor:
        if( variableTypeFromString( varname ) == VT_UNK ) {
           throw std::runtime_error( "Couldn't find variable type for " +
                                     varname );
        }
        // Update the iterator:
        itr = m_accessorCache.find( varname );
     }
     // Now do the "regular thing". Note that even if the type turns out
     // to be an integer type, the accessor wrapper does the conversion
     // reasonably anyway, behind the scenes.
     return itr->second->getVecDoubleValue(m_auxVectorData);
  }

}
