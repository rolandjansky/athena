/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGxAODProxyLoader.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/SGxAODProxyLoader.h"

#define BEGIN_TRY \
  std::string containerName, methodName; \
  splitVarnameIntoContainerAndMethod(varname, containerName, methodName); \
  BaseVariableLoader *variableLoader(0) \

#define STORE_VARIABLE_LOADER do { \
  if (variableLoader) { \
    m_varnameVariableLoader[varname] = variableLoader; \
    return; \
  } \
} while(0)

#define TRY_MET_CONTAINER do { \
  variableLoader = MissingETContainerLoader::tryCreation(m_evtStore, containerName, methodName); \
  STORE_VARIABLE_LOADER; \
} while (0)

#define TRY_VECTOR(CONTAINER) do { \
  variableLoader = VectorVariableLoader<CONTAINER>::tryCreation(m_evtStore, containerName, methodName); \
  STORE_VARIABLE_LOADER; \
} while (0)

#define TRY_SCALAR(CONTAINER) do { \
  variableLoader = ScalarVariableLoader<CONTAINER>::tryCreation(m_evtStore, containerName, methodName); \
  STORE_VARIABLE_LOADER; \
} while (0)

#define END_TRY do { \
  throw std::runtime_error("Couldn't make variable loader for: " + varname); \
} while (0)


#include "xAODBase/IParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "xAODEventShape/EventShape.h"

namespace ExpressionParsing {

  // Begin helper classes

  SGxAODProxyLoader::MissingETContainerLoader::MissingETContainerLoader(StoreGateSvc_t &evtStore, 
      const std::string &containerName, 
      const std::string &mapElement,
      const std::string &propertyName)
    : BaseScalarVariableLoader(evtStore, containerName, propertyName), 
    m_mapElement(mapElement) 
  { 
  }
    
  SGxAODProxyLoader::BaseVariableLoader *SGxAODProxyLoader::MissingETContainerLoader::tryCreation(StoreGateSvc_t &evtStore, const std::string &containerName, const std::string &methodName)
  {
    size_t openSquareBracketPosition = containerName.find('[');
    std::string actualContainerName = containerName.substr(0, openSquareBracketPosition);
    std::string mapElement = containerName.substr(openSquareBracketPosition + 1);
    boost::algorithm::trim_if(mapElement, boost::is_any_of("[\"']"));

    if (!(evtStore->contains<xAOD::MissingETContainer>(actualContainerName))) return NULL;

    return new MissingETContainerLoader(evtStore, actualContainerName, mapElement, methodName);
  }

  void SGxAODProxyLoader::MissingETContainerLoader::updateProxyLoader()
  {
    if (!m_elementProxyLoader) {
      m_elementProxyLoader = new xAODElementProxyLoader();
    }

    const xAOD::MissingETContainer *metContainer = m_evtStore->retrieve<const xAOD::MissingETContainer>(m_containerName);
    const xAOD::MissingET *metElement = (*metContainer)[m_mapElement];

    m_elementProxyLoader->setData(metElement);
  }

  // End helper classes



  SGxAODProxyLoader::~SGxAODProxyLoader()
  {
    reset();
  }

  void SGxAODProxyLoader::reset()
  {
    std::map<std::string, BaseVariableLoader *>::iterator iterVL = m_varnameVariableLoader.begin();
    for (; iterVL != m_varnameVariableLoader.end(); ++iterVL) {
      BaseVariableLoader *toDelete = iterVL->second;
      delete toDelete;
    }
    m_varnameVariableLoader.clear();
  }

  IProxyLoader::VariableType SGxAODProxyLoader::variableTypeFromString(const std::string &varname)
  {
    return variableLoaderForVarname(varname)->variableType();
  }
  
  void SGxAODProxyLoader::splitVarnameIntoContainerAndMethod(const std::string &varname, std::string &containerName, std::string &methodName)
  {
    size_t dotPosition = varname.find('.');
    containerName = varname.substr(0, dotPosition);
    methodName = varname.substr(dotPosition + 1);
  }

  SGxAODProxyLoader::BaseVariableLoader *SGxAODProxyLoader::variableLoaderForVarname(const std::string &varname)
  {
    if (m_varnameVariableLoader.count(varname) == 0) {
      computeClassForVarname(varname);
    }
    return m_varnameVariableLoader[varname];
  }

  void SGxAODProxyLoader::computeClassForVarname(const std::string &varname)
  {
    BEGIN_TRY;

    // Must try specific workarounds first

    // e.g. "MET_RefFinal[Final].met" (quotation marks around Final are optional, method name could be anything)
    TRY_MET_CONTAINER;

    // End specific workarounds

    TRY_VECTOR(xAOD::TruthParticleContainer);
    TRY_VECTOR(xAOD::ElectronContainer);
    TRY_VECTOR(xAOD::PhotonContainer);
    TRY_VECTOR(xAOD::MuonContainer);
    TRY_VECTOR(xAOD::JetContainer);
    TRY_VECTOR(xAOD::TrackParticleContainer);
    TRY_VECTOR(xAOD::VertexContainer);
    TRY_VECTOR(xAOD::CaloClusterContainer);
    TRY_VECTOR(xAOD::TruthEventContainer);
    TRY_VECTOR(xAOD::TruthVertexContainer);
    TRY_VECTOR(xAOD::TauJetContainer);
    TRY_VECTOR(xAOD::DiTauJetContainer);
    TRY_VECTOR(xAOD::BTaggingContainer);
    TRY_VECTOR(xAOD::PFOContainer);
    TRY_VECTOR(xAOD::ParticleContainer);
    TRY_VECTOR(xAOD::CompositeParticleContainer);
    TRY_VECTOR(xAOD::TrackMeasurementValidationContainer);

    TRY_SCALAR(xAOD::EventInfo); 
    TRY_SCALAR(xAOD::EventShape);

    END_TRY;
  }

  int SGxAODProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    return variableLoaderForVarname(varname)->getIntValue();
  }

  double SGxAODProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    return variableLoaderForVarname(varname)->getDoubleValue();
  }

  std::vector<int> SGxAODProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    return variableLoaderForVarname(varname)->getVectorIntValue();
  }

  std::vector<double> SGxAODProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
    return variableLoaderForVarname(varname)->getVectorDoubleValue();
  }
}
