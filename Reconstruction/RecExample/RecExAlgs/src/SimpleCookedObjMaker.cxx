/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleCookedObjMaker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "RecExAlgs/SimpleCookedObjMaker.h"
#include "RecExAlgs/SimpleCookedObj.h"
#include "RecExAlgs/SimpleRawObj.h"
#include "StoreGate/StoreGateSvc.h"

//================ Constructor =================================================

::SimpleCookedObjMaker::SimpleCookedObjMaker(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_cookedObjOutputName("unspecified"),
  m_cookedMessage("unspecified")

{
  //  template for property decalration

  declareProperty("RawObjNames", m_rawObjNames, " input objects key list ");
  declareProperty("CookedObjOutputName", m_cookedObjOutputName, " output object key ");
  declareProperty("CookedMessage", m_cookedMessage, " message to be printed");
}

//================ Destructor =================================================

SimpleCookedObjMaker::~SimpleCookedObjMaker()
{}


//================ Initialisation =================================================

StatusCode SimpleCookedObjMaker::initialize()
{
  // Code entered here will be executed once at program start.
  
  ATH_MSG_INFO(" initialize()" << endreq
               << " Cooked Message is: " << m_cookedMessage);

  if (m_rawObjNames.size()==0) {
    ATH_MSG_ERROR(" no input rawObj specified ");
    return StatusCode::FAILURE;
  } else {
    for (std::vector<std::string>::const_iterator 
           theRawObjName=m_rawObjNames.begin(),
           itrEnd=m_rawObjNames.end();
         theRawObjName != itrEnd;
         ++theRawObjName) {
      ATH_MSG_INFO(" will read in rawObj with key " << (*theRawObjName));
    }
  }

  ATH_MSG_INFO(" will write out CookedObj with key " << m_cookedObjOutputName);

  // retrieve the StoreGate Service (delete if not needed)
  if (!evtStore().retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not retrieve StoreGateSvc!");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("StoreGateSvc retrieved!");
  }
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode SimpleCookedObjMaker::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode SimpleCookedObjMaker::execute()
{

  ATH_MSG_INFO(" Cooked Message is: " << m_cookedMessage);

  for (std::vector<std::string>::const_iterator 
         theRawObjName=m_rawObjNames.begin(),
         itrEnd = m_rawObjNames.end();
       theRawObjName!=itrEnd;
       ++theRawObjName) {
    ATH_MSG_INFO(" reading in in rawObj with key " << (*theRawObjName));

    const SimpleRawObj * theRawObj=0;
    StatusCode sc = evtStore()->retrieve(theRawObj,*theRawObjName);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR(" could not read in SimpleRawObj " << m_cookedObjOutputName);
    }
  }


  SimpleCookedObj * theCookedObj = new SimpleCookedObj();

  ATH_MSG_INFO(" recording SimpleCookedObj " << m_cookedObjOutputName);

  StatusCode sc = evtStore()->record(theCookedObj,m_cookedObjOutputName,false);

  if (!sc.isSuccess()) {
    ATH_MSG_ERROR(" could not record SimpleCookedObj " << m_cookedObjOutputName);
  }
 



  // Code entered here will be executed once per event
  return StatusCode::SUCCESS;
}

//============================================================================================

