/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkSurfaces/Surface.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

#include "TRTAlignCondAlg.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

TRTAlignCondAlg::TRTAlignCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator)
  , m_condSvc("CondSvc",name)
  , m_detManager(nullptr)
{
}

TRTAlignCondAlg::~TRTAlignCondAlg()
{
}

StatusCode TRTAlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read Condition Handles initialize
  ATH_CHECK( m_readKeyRegular.initialize(!m_useDynamicFolders.value()) );
  ATH_CHECK( m_readKeyDynamicGlobal.initialize(m_useDynamicFolders.value()) );
  ATH_CHECK( m_readKeyDynamicRegular.initialize(m_useDynamicFolders.value()) );

  // Write condition handles initialize
  ATH_CHECK( m_writeKeyAlignStore.initialize() );
  ATH_CHECK( m_writeKeyDetElCont.initialize() );

  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKeyAlignStore).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKeyAlignStore.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKeyDetElCont).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKeyDetElCont.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->retrieve(m_detManager,"TRT"));

  return StatusCode::SUCCESS;
}

StatusCode TRTAlignCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handles and check their validity ____________
  SG::WriteCondHandle<GeoAlignmentStore> writeHandle{m_writeKeyAlignStore};
  SG::WriteCondHandle<InDetDD::TRT_DetElementContainer> writeHandleDetElCont{m_writeKeyDetElCont};

  if (writeHandleDetElCont.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleDetElCont.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    //writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // ____________ Construct new Write Cond Object and its range ____________
  GeoAlignmentStore* writeCdo = new GeoAlignmentStore();
  EventIDRange rangeW;

  // ____________ Get Read Cond Objects ____________
  // Container for passing read CDO-s over to the Detector Manager
  InDetDD::RawAlignmentObjects readCdoContainer;

  const InDetDD::TRT_DetElementCollection* unAlignedColl{m_detManager->getDetectorElementCollection()};
  if (unAlignedColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  if(m_useDynamicFolders) {
    // 1. Dynamic folders

    // ** Global
    SG::ReadCondHandle<CondAttrListCollection> readHandleDynamicGlobal{m_readKeyDynamicGlobal};
    // Get CDO and store it into container
    const CondAttrListCollection* readCdoDynamicGlobal{*readHandleDynamicGlobal}; 
    if(readCdoDynamicGlobal==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Dynamic Global");
      delete writeCdo;
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyDynamicGlobal.key(),readCdoDynamicGlobal);
    // Get range
    EventIDRange rangeDynamicGlobal;
    if(!readHandleDynamicGlobal.range(rangeDynamicGlobal)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDynamicGlobal.key());
      delete writeCdo;
      return StatusCode::FAILURE;
    }

    // ** Regular
    SG::ReadCondHandle<AlignableTransformContainer> readHandleDynamicRegular{m_readKeyDynamicRegular};
    // Get CDO and store it into container
    const AlignableTransformContainer* readCdoDynamicRegular{*readHandleDynamicRegular}; 
    if(readCdoDynamicRegular==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Dynamic Regular");
      delete writeCdo;
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyDynamicRegular.key(),readCdoDynamicRegular);
    // Get range
    EventIDRange rangeDynamicRegular;
    if(!readHandleDynamicRegular.range(rangeDynamicRegular)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleDynamicRegular.key());
      delete writeCdo;
      return StatusCode::FAILURE;
    }

    // Create an intersection of input ranges
    rangeW = EventIDRange::intersect(rangeDynamicGlobal,rangeDynamicRegular);
  }
  else {
    // 2. Regular folder
    SG::ReadCondHandle<AlignableTransformContainer> readHandleRegular{m_readKeyRegular};
    // Get CDO and store it into container
    const AlignableTransformContainer* readCdoRegular{*readHandleRegular}; 
    if(readCdoRegular==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object: Regular");
      delete writeCdo;
      return StatusCode::FAILURE;
    }
    readCdoContainer.emplace(m_readKeyRegular.key(),readCdoRegular);
    // Get range
    if(!readHandleRegular.range(rangeW)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleRegular.key());
      delete writeCdo;
      return StatusCode::FAILURE;
    }
  }


  // ____________ Apply alignments to TRT GeoModel ____________
  if(m_detManager->align(readCdoContainer,writeCdo).isFailure()) {
    ATH_MSG_ERROR("Failed to apply alignments to TRT");
    //delete writeCdo;
    return StatusCode::FAILURE;
  }


  const InDetDD::TRT_DetElementCollection* alignedColl{m_detManager->getDetectorElementCollection()};
  if (alignedColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________                                                                                
  std::unique_ptr<InDetDD::TRT_DetElementContainer> writeCdoDetElCont{std::make_unique<InDetDD::TRT_DetElementContainer>()};
  
  // ____________ Update writeCdo using readCdo ____________                                                                                   
  std::map<const InDetDD::TRT_BaseElement*, const InDetDD::TRT_BaseElement*> oldToNewMap;
  std::map<const InDetDD::TRT_BaseElement*, const InDetDD::TRT_EndcapElement*> oldToOldECMap;
  std::map<const InDetDD::TRT_BaseElement*, const InDetDD::TRT_BarrelElement*> oldToOldBAMap;


  oldToNewMap[nullptr] = nullptr;
  oldToOldECMap[nullptr] = nullptr;
  oldToOldBAMap[nullptr] = nullptr;

  InDetDD::TRT_DetElementCollection* newDetElColl=new InDetDD::TRT_DetElementCollection();

  newDetElColl->resize(alignedColl->size(), nullptr);

  //Will create a new aligned detector element collection
  InDetDD::TRT_DetElementCollection::iterator newEl{newDetElColl->begin()};
  for (const InDetDD::TRT_BaseElement* oldEl: *alignedColl) {

    InDetDD::TRT_BaseElement::Type type = oldEl->type();

    if(type == InDetDD::TRT_BaseElement::ENDCAP)
      {
	const InDetDD::TRT_EndcapElement* oldEl_Endcap = dynamic_cast<const InDetDD::TRT_EndcapElement*>(oldEl);
	//New encap element with new alignment created based on old element
        *newEl = new InDetDD::TRT_EndcapElement(*oldEl_Endcap,writeCdo);
       	oldToNewMap[oldEl]= *newEl;
	oldToOldECMap[oldEl]= oldEl_Endcap;

      }else if(type == InDetDD::TRT_BaseElement::BARREL){
        const InDetDD::TRT_BarrelElement* oldEl_Barrel = dynamic_cast<const InDetDD::TRT_BarrelElement*>(oldEl);
        //New barrel element with new alignment created based on old element
        *newEl = new InDetDD::TRT_BarrelElement(*oldEl_Barrel,writeCdo);
        oldToNewMap[oldEl]= *newEl;
        oldToOldBAMap[oldEl]= oldEl_Barrel;

    }else{
      ATH_MSG_FATAL("Unknown TRT detector element found");
      return StatusCode::FAILURE;
    }
    newEl++;
  }

  //Set detector elements links
  InDetDD::TRT_DetElementCollection::const_iterator oldIt{alignedColl->begin()};
  for (InDetDD::TRT_BaseElement* newEl: *newDetElColl) {
    if (oldToNewMap[(*oldIt)]!=newEl) {
      ATH_MSG_ERROR("Old and new elements are not synchronized!");
    }
    InDetDD::TRT_BaseElement::Type type = newEl->type();
    if(type == InDetDD::TRT_BaseElement::ENDCAP){
      InDetDD::TRT_EndcapElement* newEl_Endcap = dynamic_cast<InDetDD::TRT_EndcapElement*>(newEl);
      newEl_Endcap->setNextInZ(oldToOldECMap[(*oldIt)]->nextInZ());
      newEl_Endcap->setPreviousInZ(oldToOldECMap[(*oldIt)]->previousInZ());
      newEl = newEl_Endcap;
    }else if(type == InDetDD::TRT_BaseElement::BARREL){
      InDetDD::TRT_BarrelElement* newEl_Barrel = dynamic_cast<InDetDD::TRT_BarrelElement*>(newEl);
      newEl_Barrel->setNextInR(oldToOldBAMap[(*oldIt)]->nextInR());
      newEl_Barrel->setPreviousInR(oldToOldBAMap[(*oldIt)]->previousInR());
      newEl_Barrel->setNextInPhi(oldToOldBAMap[(*oldIt)]->nextInPhi());
      newEl_Barrel->setPreviousInPhi(oldToOldBAMap[(*oldIt)]->previousInPhi());
      newEl = newEl_Barrel;
    }
    oldIt++;
  }

  // Update all detector elements caches and add aligned det elements to the corresponding array based on their type
  for (InDetDD::TRT_BaseElement* newEl: *newDetElColl) {
    newEl->updateAllCaches();
    InDetDD::TRT_BaseElement::Type type = newEl->type();
    if(type == InDetDD::TRT_BaseElement::ENDCAP){
      InDetDD::TRT_EndcapElement* newEl_Endcap = dynamic_cast<InDetDD::TRT_EndcapElement*>(newEl);
      writeCdoDetElCont->addEndcapElement(newEl_Endcap);
    }else if(type == InDetDD::TRT_BaseElement::BARREL){
      InDetDD::TRT_BarrelElement* newEl_Barrel = dynamic_cast<InDetDD::TRT_BarrelElement*>(newEl);
      writeCdoDetElCont->addBarrelElement(newEl_Barrel);
    }    
  }

  writeCdoDetElCont->setDetElementCollection(newDetElColl);
  writeCdoDetElCont->setNumerology(m_detManager->getNumerology());

  // Record the resulting CDO
  if(writeHandle.record(rangeW,writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record GeoAlignmentStore " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
		delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  // Record WriteCondHandle (size is meaning less here?)
  const std::size_t size{newDetElColl->size()};

  if (writeHandleDetElCont.record(rangeW, std::move(writeCdoDetElCont)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandleDetElCont.key()
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandleDetElCont.key() << " with range " << rangeW << " with size of " << size << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode TRTAlignCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
