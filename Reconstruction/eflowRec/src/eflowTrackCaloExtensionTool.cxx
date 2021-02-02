/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     eflowTrackCaloExtensionTool.cxx
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.Guttenberger
 CREATED:  19th September, 2014

 ********************************************************************/

#include <eflowRec/eflowTrackCaloExtensionTool.h>

#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowDepthCalculator.h"

#include "TrkParameters/TrackParameters.h"  // typedef
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "CaloDetDescr/CaloDepthTool.h"

#include "GaudiKernel/ListItem.h"

#include <map>
#include <vector>
#include <utility>

eflowTrackCaloExtensionTool::eflowTrackCaloExtensionTool(const std::string& type, const std::string& name, const IInterface* parent)  :
    AthAlgTool(type, name, parent),
    m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool",this),
    m_trackParametersIdHelper(std::make_unique<Trk::TrackParametersIdHelper>())
{
  declareInterface<eflowTrackExtrapolatorBaseAlgTool>(this);
  declareProperty("TrackCaloExtensionTool", m_theTrackExtrapolatorTool, "TrackCaloExtension Tool Handle");
}

eflowTrackCaloExtensionTool::~eflowTrackCaloExtensionTool() {
}

StatusCode eflowTrackCaloExtensionTool::initialize() {
  /* Tool service */
  IToolSvc* myToolSvc;
  if (service("ToolSvc", myToolSvc).isFailure()) {
    ATH_MSG_WARNING(" Tool Service Not Found");
    return StatusCode::SUCCESS;
  }

  if (m_theTrackExtrapolatorTool.retrieve().isFailure()) {
    ATH_MSG_WARNING("Cannot find Extrapolation tool "
		    << m_theTrackExtrapolatorTool.typeAndName());
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_VERBOSE("Successfully retrieved Extrapolation tool "
		    << m_theTrackExtrapolatorTool.typeAndName());
  }

  if (!m_ParticleCacheKey.key().empty()) ATH_CHECK(m_ParticleCacheKey.initialize());
  else m_useOldCalo = true;
  
  return StatusCode::SUCCESS;
}

std::unique_ptr<eflowTrackCaloPoints> eflowTrackCaloExtensionTool::execute(const xAOD::TrackParticle* track) const {

  ATH_MSG_VERBOSE(" Now running eflowTrackCaloExtensionTool");

  /*Create a map to index the TrackParameters at calo (owned by the extension) wrt to layers*/
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*> parametersMap;

  /*get the CaloExtension object*/
  const Trk::CaloExtension * extension = nullptr;
  std::unique_ptr<Trk::CaloExtension> uniqueExtension;
  const int index = track->index();
  ATH_MSG_VERBOSE("Getting element " << index << " from the particleCache");

  if (m_useOldCalo) {
    /* If CaloExtensionBuilder is unavailable, use the calo extension tool */
    ATH_MSG_VERBOSE("Using the CaloExtensionTool");
    uniqueExtension = m_theTrackExtrapolatorTool->caloExtension(*track);
    extension = uniqueExtension.get();
  } else {
    /*get the CaloExtension object*/
    SG::ReadHandle<CaloExtensionCollection>  particleCache {m_ParticleCacheKey};
    ATH_MSG_VERBOSE("Using the CaloExtensionBuilder Cache");
    extension = (*particleCache)[index];
    ATH_MSG_VERBOSE("Getting element " << index << " from the particleCache");
    if( not extension ){
      ATH_MSG_VERBOSE("Cache does not contain a calo extension -> Calculating with the a CaloExtensionTool" );
      uniqueExtension = m_theTrackExtrapolatorTool->caloExtension(*track);
      extension = uniqueExtension.get();
    }
  }
  
  if (extension != nullptr) {

    /*extract the CurvilinearParameters*/
    const std::vector<Trk::CurvilinearParameters>& clParametersVector = extension->caloLayerIntersections();

     /*The parameters are owned by the CaloExtension so are handled by it the eflowTrackCaloPoints does
     * not take ownership */
    for ( const Trk::CurvilinearParameters& clParameter : clParametersVector) {
      if (parametersMap[getLayer(&clParameter)] == nullptr) {
        parametersMap[getLayer(&clParameter)] = &clParameter;
      } else if (m_trackParametersIdHelper->isEntryToVolume(clParameter.cIdentifier())) {
        parametersMap[getLayer(&clParameter)] = &clParameter;
      }
    }
    /*
      parametersMap may have several entries for Tile1,2,3.
      The impact is negligible as the eta/phi of these entries are very similar
      https://its.cern.ch/jira/browse/ATLJETMET-242
    */
    
    return std::make_unique<eflowTrackCaloPoints>(parametersMap);

  }
  else{
    if (track->pt() > 3*Gaudi::Units::GeV) ATH_MSG_WARNING("TrackExtension failed for track with pt and eta " << track->pt() << " and " << track->eta());
    parametersMap[eflowCalo::LAYER::Unknown] = nullptr;
    return std::make_unique<eflowTrackCaloPoints>(parametersMap);
  }
}

StatusCode eflowTrackCaloExtensionTool::finalize() {
  return StatusCode::SUCCESS;
}

/*This function translates the information embedded within the CurvilinearParameters of the CaloExtension object into an eflowCaloLayer*/
eflowCalo::LAYER eflowTrackCaloExtensionTool::getLayer(const Trk::CurvilinearParameters* clParameter) const {
  unsigned int parametersIdentifier = clParameter->cIdentifier();

  /*Return unknown when the identifier is invalid */
  if (!m_trackParametersIdHelper->isValid(parametersIdentifier)) {
    ATH_MSG_ERROR("invalid Track Identifier");
    return eflowCalo::LAYER::Unknown;
  };

  if(m_trackParametersIdHelper->isEntryToVolume(parametersIdentifier)) {
    ATH_MSG_VERBOSE("is Volume Entry");
 } else {
    ATH_MSG_VERBOSE("is Volume Exit");
    }

  return eflowCalo::translateSampl(m_trackParametersIdHelper->caloSample(parametersIdentifier));
}
