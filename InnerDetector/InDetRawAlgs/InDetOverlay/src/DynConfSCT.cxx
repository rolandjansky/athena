/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#include "InDetOverlay/DynConfSCT.h"

#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/AlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/StoreGateSvc.h"

#include "InDetOverlay/InDetOverlay.h"

#include "InDetRawData/SCT1_RawData.h"

#include <iostream>
#include <typeinfo>

//================================================================
DynConfSCT::DynConfSCT(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
  , m_inputFormatDetermined(false)
  , m_storeGateData("", name)
  , m_digiAlgTool(0)
{
  declareProperty("InDetOverlayName", m_InDetOverlayName="InDetOverlay",
                  "The name of InDetOverlay algorithm from which we take "
                  "the SG key of the 'real data' SCT RDOs and the SG store"
                  " where to find them"
                  );

  declareProperty("SCT_DigitizationName", m_SCT_DigitizationName="SCT_DigitizationTool",
                  "We set the WriteSCT1_RawData property of this SCT_Digitization algorithm.");
}

//================================================================
StatusCode DynConfSCT::initialize()
{
  ATH_MSG_DEBUG("DynConfSCT initialize begin");

  IAlgManager* theAlgMgr(0);
  if( ! serviceLocator()
      ->getService("ApplicationMgr", IAlgManager::interfaceID(), *pp_cast<IInterface>(&theAlgMgr))
      .isSuccess() )
    {
      ATH_MSG_FATAL("Can not get an IAlgManager");
      return StatusCode::FAILURE;
    }

  IToolSvc* toolSvc;
  if(!service("ToolSvc",toolSvc).isSuccess()){
    ATH_MSG_FATAL("Can't get the ToolSvc!");
    return StatusCode::FAILURE;
  }

  //----------------
  // Figure out where to find real data SCT RDOs.
  // (Need SG key and the store itself.)
  // Copy the settings from InDetOverlay algorithm.

  IAlgorithm *pAlg(0);
  if( ! theAlgMgr->getAlgorithm(m_InDetOverlayName, pAlg).isSuccess() ) {
    ATH_MSG_FATAL("Can not get algorithm \""<<m_InDetOverlayName<<"\"");
    return StatusCode::FAILURE;
  }

  Algorithm *ovlAlg = dynamic_cast<Algorithm*>(pAlg);
  if(!ovlAlg) {
    ATH_MSG_FATAL("Pointer to Algorithm \""<<m_InDetOverlayName<<"\" is NULL");
    return StatusCode::FAILURE;
  }

  m_mainInputSCT_Name = ovlAlg->getProperty("mainInputSCT_Name").toString();
  ATH_MSG_DEBUG("Got mainInputSCT_Name = "<<m_mainInputSCT_Name);

  m_storeGateData.setTypeAndName(ovlAlg->getProperty("DataStore").toString());
  if (m_storeGateData.retrieve().isFailure()) {
    ATH_MSG_FATAL("Error retrieving SG handle: "<<m_storeGateData);
    return StatusCode::FAILURE;
  }

  // another way: m_storeGateData =
  // another way:   dynamic_cast<const ServiceHandle<StoreGateSvc>&>
  // another way:   ( dynamic_cast<const GaudiHandleProperty&>(ovlAlg->getProperty("DataStore")).value() );

  ATH_MSG_DEBUG("Got DataStore "<<m_storeGateData);

  //----------------
  // get the controlled algtool

  IAlgTool *pTool(0);
  if( ! toolSvc->retrieveTool(m_SCT_DigitizationName, pTool).isSuccess() ) {
    ATH_MSG_FATAL("Can not get AlgTool \""<<m_InDetOverlayName<<"\"");
    return StatusCode::FAILURE;
  }

  m_digiAlgTool = dynamic_cast<AlgTool*>(pTool);
  if(!m_digiAlgTool) {
    ATH_MSG_FATAL("Pointer to AlgTool \""<<m_SCT_DigitizationName<<"\" is NULL");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("DynConfSCT initialized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode DynConfSCT::finalize()
{
  ATH_MSG_DEBUG("DynConfSCT finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode DynConfSCT::execute() {
  ATH_MSG_DEBUG("DynConfSCT::execute() begin");

  const std::string propName("WriteSCT1_RawData");
  bool oldDigiSetting =
    dynamic_cast<const BooleanProperty&>(m_digiAlgTool->getProperty(propName)).value();

  //----------------------------------------------------------------
  const SCT_RDO_Container *dataRDOs(0);
  if(m_storeGateData->retrieve(dataRDOs, m_mainInputSCT_Name).isSuccess()) {
    ATH_MSG_DEBUG("Got data RDOs");

    const SCT_RDORawData* rdo = getRDO(dataRDOs);
    if(!rdo) {
      if(!m_inputFormatDetermined) {
        ATH_MSG_WARNING("Can't determine SCT RDO format of the data input:"
                        " non non-NULL entries in "<<m_mainInputSCT_Name
                        );

        ATH_MSG_WARNING("This event will use the previous setting of SCT digitization:"
                        " WriteSCT1_RawData = "<<oldDigiSetting
                        );
      }
    }
    else {
      if( ( dynamic_cast<const SCT1_RawData*>(rdo) && !oldDigiSetting) ||
          (!dynamic_cast<const SCT1_RawData*>(rdo) &&  oldDigiSetting) ) {

        // need to flip the setting
        BooleanProperty bp(propName, !oldDigiSetting);

        ATH_MSG_INFO("Changing SCT_Digitization settings to "<<bp);

        if(!m_digiAlgTool->setProperty(bp).isSuccess()) {
          ATH_MSG_FATAL("Error setting SCT_Digitization property "<<bp);
          return StatusCode::FAILURE;
        }

      }
      else {
        if(!m_inputFormatDetermined) {
          ATH_MSG_INFO("SCT_Digitization settings "<<propName<<"="<<oldDigiSetting
                       <<" already agrees with the data input type."
                       );
        }
      } // if (need to flip)

      m_inputFormatDetermined = true;
    } // if(!rdo) {} else {

  }
  else {
    ATH_MSG_WARNING("Could not get data RDOs");
    ATH_MSG_WARNING("This event will use the previous setting of SCT digitization:"
                    " WriteSCT1_RawData = "<<oldDigiSetting
                    );
  }

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("DynConfSCT::execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
const SCT_RDORawData *DynConfSCT::getRDO(const SCT_RDO_Container *container) const {
  bool hasWarned = false;

  for(SCT_RDO_Container::const_iterator p = container->begin();
      p != container->end(); ++p) {

    for(SCT_RDO_Container::IDENTIFIABLE::const_iterator c = p->cptr()->begin();
        c != p->cptr()->end(); ++c) {

      if(*c) {
        return *c;
      }
      else {
        if(!hasWarned) {
          ATH_MSG_WARNING("There is a NULL RDO* in SCT_RDO_Container.");
          hasWarned = true;
        }
      }
    }
  }

  return 0;
}

//================================================================
//EOF
