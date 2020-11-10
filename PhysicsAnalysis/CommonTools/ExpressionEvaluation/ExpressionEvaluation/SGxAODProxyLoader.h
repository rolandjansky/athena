/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGxAODProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef SG_XAOD_PROXY_LOADER_H
#define SG_XAOD_PROXY_LOADER_H

#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/xAODVariableProxyLoaders.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/BuiltinsClids.h"

#include <map>

namespace ExpressionParsing {

  class SGxAODProxyLoader : public IProxyLoader {
    public:
      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

    private:
      class BaseVariableLoader {
        public:
          BaseVariableLoader(StoreGateSvc_t &evtStore)
            : m_evtStore(evtStore) { }
          virtual ~BaseVariableLoader() { }
          virtual int getIntValue() = 0;
          virtual double getDoubleValue() = 0;
          virtual std::vector<int> getVectorIntValue() = 0;
          virtual std::vector<double> getVectorDoubleValue() = 0;

          virtual IProxyLoader::VariableType variableType() = 0;

        protected:
          StoreGateSvc_t m_evtStore;
      };



      class BaseScalarVariableLoader : public BaseVariableLoader {
        public:
          BaseScalarVariableLoader(StoreGateSvc_t &evtStore, 
              const std::string &containerName, 
              const std::string &propertyName)
            : BaseVariableLoader(evtStore), 
            m_containerName(containerName), 
            m_propertyName(propertyName),
            m_variableType(),
            m_elementProxyLoader(NULL),
            m_initialised(false)
          { 
          }

          virtual ~BaseScalarVariableLoader()
          {
            if (m_elementProxyLoader) {
              delete m_elementProxyLoader;
              m_elementProxyLoader = NULL;
            }
          }

          virtual IProxyLoader::VariableType variableType()
          {
            if (!m_initialised) {
              m_initialised = true;
              updateProxyLoader();
              m_variableType = m_elementProxyLoader->variableTypeFromString(m_propertyName);
            }
            return m_variableType;
          }

          virtual int getIntValue()
          {
            updateProxyLoader();
            return m_elementProxyLoader->loadIntVariableFromString(m_propertyName);
          }

          virtual double getDoubleValue()
          {
            updateProxyLoader();
            return m_elementProxyLoader->loadDoubleVariableFromString(m_propertyName);
          }

          virtual std::vector<int> getVectorIntValue()
          {
            throw std::runtime_error("Can't get vector value in ScalarVariableLoader");
          }

          virtual std::vector<double> getVectorDoubleValue()
          {
            throw std::runtime_error("Can't get vector value in ScalarVariableLoader");
          }

        private:
          virtual void updateProxyLoader() = 0;

        protected:
          std::string m_containerName;
          std::string m_propertyName;
          IProxyLoader::VariableType m_variableType;
          xAODElementProxyLoader *m_elementProxyLoader;
          bool m_initialised;
      };


      template <typename CONTAINER> class ScalarVariableLoader : public BaseScalarVariableLoader {
        public:
          ScalarVariableLoader(StoreGateSvc_t &evtStore, 
              const std::string &containerName, 
              const std::string &propertyName)
            : BaseScalarVariableLoader(evtStore, containerName, propertyName)
          { 
          }

          static BaseVariableLoader *tryCreation(StoreGateSvc_t &evtStore, const std::string &containerName, const std::string &propertyName)
          {
            if (!(evtStore->contains<CONTAINER>(containerName))) return NULL;
            return new ScalarVariableLoader<CONTAINER>(evtStore, containerName, propertyName);
          }

        private:
          virtual void updateProxyLoader()
          {
            if (!m_elementProxyLoader) {
              m_elementProxyLoader = new xAODElementProxyLoader();
            }

            const CONTAINER *container = m_evtStore->retrieve<const CONTAINER>(m_containerName);
            m_elementProxyLoader->setData(container);
          }
      };



      template <typename CONTAINER> class VectorVariableLoader : public BaseVariableLoader {
        public:
          VectorVariableLoader(StoreGateSvc_t &evtStore, 
              const std::string &containerName, 
              const std::string &propertyName)
            : BaseVariableLoader(evtStore), 
            m_containerName(containerName), 
            m_propertyName(propertyName),
            m_variableType(),
            m_vectorProxyLoader(NULL),
            m_initialised(false)
          { 
          }

          virtual ~VectorVariableLoader()
          {
            if (m_vectorProxyLoader) {
              delete m_vectorProxyLoader;
              m_vectorProxyLoader = NULL;
            }
          }

          virtual IProxyLoader::VariableType variableType()
          {
            if (!m_initialised) {
              m_initialised = true;
              updateProxyLoader();
              m_variableType = m_vectorProxyLoader->variableTypeFromString(m_propertyName);
            }
            return m_variableType;
          }

          static BaseVariableLoader *tryCreation(StoreGateSvc_t &evtStore, const std::string &containerName, const std::string &propertyName)
          {
            if (!(evtStore->contains<CONTAINER>(containerName))) return NULL;
            return new VectorVariableLoader<CONTAINER>(evtStore, containerName, propertyName);
          }

          virtual int getIntValue()
          {
            throw std::runtime_error("Can't get scalar value in VectorVariableLoader");
          }

          virtual double getDoubleValue()
          {
            throw std::runtime_error("Can't get scalar value in VectorVariableLoader");
          }

          virtual std::vector<int> getVectorIntValue()
          {
            updateProxyLoader();
            return m_vectorProxyLoader->loadVecIntVariableFromString(m_propertyName);
          }

          virtual std::vector<double> getVectorDoubleValue()
          {
            updateProxyLoader();
            return m_vectorProxyLoader->loadVecDoubleVariableFromString(m_propertyName);
          }

        private:
          void updateProxyLoader()
          {
            if (!m_vectorProxyLoader) {
              m_vectorProxyLoader = new xAODVectorProxyLoader();
            }

            const CONTAINER *container = m_evtStore->retrieve<const CONTAINER>(m_containerName);
            m_vectorProxyLoader->setData(container);
          }

        protected:
          std::string m_containerName;
          std::string m_propertyName;
          IProxyLoader::VariableType m_variableType;
          xAODVectorProxyLoader *m_vectorProxyLoader;
          bool m_initialised;
      };



      class MissingETContainerLoader : public BaseScalarVariableLoader {
        public:
          MissingETContainerLoader(StoreGateSvc_t &evtStore, 
              const std::string &containerName, 
              const std::string &mapElement,
              const std::string &propertyName);
          
          static BaseVariableLoader *tryCreation(StoreGateSvc_t &evtStore, 
              const std::string &containerName, 
              const std::string &methodName);

        private:
          virtual void updateProxyLoader();

        private:
          std::string m_mapElement;
      };



    public:
      SGxAODProxyLoader(StoreGateSvc_t &evtStore) : m_evtStore(evtStore) { }
      virtual ~SGxAODProxyLoader();

      virtual void reset();

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);

      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      void splitVarnameIntoContainerAndMethod(const std::string &varname, std::string &containerName, std::string &methodName); 

      BaseVariableLoader *variableLoaderForVarname(const std::string &varname);

      void computeClassForVarname(const std::string &varname);

    private:
      StoreGateSvc_t m_evtStore;
      std::map<std::string, BaseVariableLoader *> m_varnameVariableLoader;
  };

}

#endif // SG_XAOD_PROXY_LOADER_H
