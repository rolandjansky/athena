/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// xAODVariableProxyLoaders.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef XAOD_VARIABLE_PROXY_LOADERS_H
#define XAOD_VARIABLE_PROXY_LOADERS_H

#include "ExpressionEvaluation/IProxyLoader.h"

#include "AthContainers/AuxElement.h"
#include "TMethodCall.h"
#include "TVirtualCollectionProxy.h"

#include <map>
#include <string>
#include <typeinfo>

namespace ExpressionParsing {

  class BaseAccessorWrapper {
    public:
      virtual ~BaseAccessorWrapper() { }
      virtual bool isValid(const SG::AuxElement *auxElement) const = 0;
      virtual bool isValid(const SG::AuxVectorData *auxVectorData) const = 0;

      virtual int getIntValue(const SG::AuxElement *auxElement) const = 0;
      virtual double getDoubleValue(const SG::AuxElement *auxElement) const = 0;
      virtual std::vector<int> getVecIntValue(const SG::AuxVectorData *auxVectorData) const = 0;
      virtual std::vector<double> getVecDoubleValue(const SG::AuxVectorData *auxVectorData) const = 0;
  };


  template <typename T> class AccessorWrapper : public BaseAccessorWrapper { 
    public:
      AccessorWrapper(const std::string &elementName) :
        m_elementName(elementName), m_acc(elementName) 
      {
      }

      virtual ~AccessorWrapper() { }

      virtual bool isValid(const SG::AuxElement *auxElement) const {
        return m_acc.isAvailable(*auxElement);
      }

      virtual bool isValid(const SG::AuxVectorData *auxVectorData) const {
        // TPSG: A bit hacky - implementation copied from AuxElement.icc
        SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
        SG::auxid_t auxid = r.findAuxID(m_elementName);
        if (!auxVectorData || !auxVectorData->getConstStore()) return false;

        // At this point we just have to cross our fingers and hope...
        if (auxVectorData->size_v() == 0) return true;

        const SG::auxid_set_t& ids = auxVectorData->getConstStore()->getAuxIDs();
        if( ids.find (auxid) != ids.end() ) return true;
        return auxVectorData->getConstStore()->getData( auxid ) != 0;
      }

      virtual int getIntValue(const SG::AuxElement *auxElement) const {
        return (int) m_acc(*auxElement);
      }

      virtual double getDoubleValue(const SG::AuxElement *auxElement) const {
        return (double) m_acc(*auxElement);
      }

      virtual std::vector<int> getVecIntValue(const SG::AuxVectorData *auxVectorData) const {
        size_t N = auxVectorData->size_v();
        std::vector<int> result(N);
        for (size_t i = 0; i < N; ++i) {
          result[i] = (int) m_acc(*auxVectorData, i);
        }
        return result;
      }

      virtual std::vector<double> getVecDoubleValue(const SG::AuxVectorData *auxVectorData) const {
        size_t N = auxVectorData->size_v();
        std::vector<double> result(N);
        for (size_t i = 0; i < N; ++i) {
          result[i] = (double) m_acc(*auxVectorData, i);
        }
        return result;
      }

    private:
      std::string m_elementName;
      typename SG::AuxElement::ConstAccessor<T> m_acc;
  };


  class TMethodWrapper : public BaseAccessorWrapper {
    public:
      TMethodWrapper(const std::type_info &elementTypeinfo, const std::string &methodName);
      virtual ~TMethodWrapper();
      IProxyLoader::VariableType variableType() const;
      virtual bool isValid(const SG::AuxElement *auxElement) const;
      virtual bool isValid(const SG::AuxVectorData *auxVectorData) const;

      virtual int getIntValue(const SG::AuxElement *auxElement) const;
      virtual double getDoubleValue(const SG::AuxElement *auxElement) const;
      virtual std::vector<int> getVecIntValue(const SG::AuxVectorData *auxVectorData) const;
      virtual std::vector<double> getVecDoubleValue(const SG::AuxVectorData *auxVectorData) const;

    private:
      TMethodCall *m_methodCall;
      bool m_valid;
  };


  class TMethodCollectionWrapper : public BaseAccessorWrapper {
    public:
      TMethodCollectionWrapper(const std::type_info &containerTypeinfo, const std::string &methodName);
      virtual ~TMethodCollectionWrapper();
      IProxyLoader::VariableType variableType() const;
      virtual bool isValid(const SG::AuxElement *auxElement) const;
      virtual bool isValid(const SG::AuxVectorData *auxVectorData) const;

      virtual int getIntValue(const SG::AuxElement *auxElement) const;
      virtual double getDoubleValue(const SG::AuxElement *auxElement) const;
      virtual std::vector<int> getVecIntValue(const SG::AuxVectorData *auxVectorData) const;
      virtual std::vector<double> getVecDoubleValue(const SG::AuxVectorData *auxVectorData) const;

    private:
      TVirtualCollectionProxy *m_collectionProxy;
      TMethodCall *m_methodCall;
      bool m_valid;
  };


  class xAODElementProxyLoader : public IProxyLoader {
    public:
      xAODElementProxyLoader();
      xAODElementProxyLoader(const SG::AuxElement *auxElement);

      virtual ~xAODElementProxyLoader();
      virtual void reset();

      void setData(const SG::AuxElement *auxElement);

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);
      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      const SG::AuxElement *m_auxElement;
      std::map<std::string, BaseAccessorWrapper*> m_accessorCache;
  };


  class xAODVectorProxyLoader : public IProxyLoader {
    public:
      xAODVectorProxyLoader();
      xAODVectorProxyLoader(const SG::AuxVectorData *auxVectorData);

      virtual ~xAODVectorProxyLoader();
      virtual void reset();

      void setData(const SG::AuxVectorData *auxElement);

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);
      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      const SG::AuxVectorData *m_auxVectorData;
      std::map<std::string, BaseAccessorWrapper*> m_accessorCache;

  };

}

#endif // XAOD_VARIABLE_PROXY_LOADERS_H
