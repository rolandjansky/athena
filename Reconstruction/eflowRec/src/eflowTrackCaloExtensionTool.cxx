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
#include "eflowRec/cycle.h"
#include "eflowRec/eflowDepthCalculator.h"

#include "TrkParameters/TrackParameters.h"  // typedef
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "CaloDetDescr/CaloDepthTool.h"

#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/ToolHandle.h"

#include <map>
#include <vector>
#include <utility>

//using std::pair;

eflowTrackCaloExtensionTool::eflowTrackCaloExtensionTool(const std::string& type, const std::string& name, const IInterface* parent)  :
    AthAlgTool(type, name, parent),
    m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool"),
    m_trackParametersIdHelper(new Trk::TrackParametersIdHelper)//,
    //m_tracksProcessed(0)
{
  declareInterface<eflowTrackExtrapolatorBaseAlgTool>(this);
  declareProperty("TrackCaloExtensionTool", m_theTrackExtrapolatorTool, "TrackCaloExtension Tool Handle");
}

eflowTrackCaloExtensionTool::~eflowTrackCaloExtensionTool() {
  delete m_trackParametersIdHelper;
}

StatusCode eflowTrackCaloExtensionTool::initialize() {
  /* Tool service */
  IToolSvc* myToolSvc;
  if (service("ToolSvc", myToolSvc).isFailure()) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_theTrackExtrapolatorTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find Extrapolation tool "
    << m_theTrackExtrapolatorTool.typeAndName() << endmsg;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Successfully retrieved Extrapolation tool "
    << m_theTrackExtrapolatorTool.typeAndName() << endmsg;
  }

  return StatusCode::SUCCESS;
}

eflowTrackCaloPoints* eflowTrackCaloExtensionTool::execute(const xAOD::TrackParticle* track) const {
  //++m_tracksProcessed;
  msg(MSG::VERBOSE) << " Now running eflowTrackCaloExtensionTool" << endmsg;

  /*make the map*/
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*> parametersMap;

  /*get the CaloExtension object*/
  const Trk::CaloExtension* extension = 0;
  if (m_theTrackExtrapolatorTool->caloExtension(*track, extension)) {

    /*extract the CurvilinearParameters*/
    const std::vector<const Trk::CurvilinearParameters*>& clParametersVector = extension->caloLayerIntersections();

    /*fill the map*/
    for (auto clParameter : clParametersVector) {
      if (parametersMap[getLayer(clParameter)] == NULL) {
        parametersMap[getLayer(clParameter)] = clParameter->clone();
      } else if (m_trackParametersIdHelper->isEntryToVolume(clParameter->cIdentifier())) {
        delete parametersMap[getLayer(clParameter)];
        parametersMap[getLayer(clParameter)] = clParameter->clone();
      }
    }
    return new eflowTrackCaloPoints(parametersMap);
  }
  else{
    if (track->pt() > 3*Gaudi::Units::GeV)  ATH_MSG_WARNING("TrackExtension failed for track with pt and eta " << track->pt() << " and " << track->eta());
    parametersMap[eflowCalo::LAYER::Unknown] = 0;
    return new eflowTrackCaloPoints(parametersMap);
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
    msg(MSG::ERROR) << "invalid Track Identifier"<<endmsg;
    return eflowCalo::LAYER::Unknown;
  };

  if(m_trackParametersIdHelper->isEntryToVolume(parametersIdentifier)) {
    msg(MSG::VERBOSE) << "is Volume Entry" << endmsg;
 } else {
    msg(MSG::VERBOSE) << "is Volume Exit" << endmsg;
    }

  return eflowCalo::translateSampl(m_trackParametersIdHelper->caloSample(parametersIdentifier));
}
