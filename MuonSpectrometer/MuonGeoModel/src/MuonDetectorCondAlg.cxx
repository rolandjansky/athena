/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonDetectorCondAlg.h" 
#include "MuonGeoModel/MuonDetectorFactory001.h"
#include "MuonGeoModel/MuonDetectorTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <fstream>
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

MuonDetectorCondAlg::MuonDetectorCondAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_condSvc{"CondSvc", name}
{ }

StatusCode
MuonDetectorCondAlg::initialize()
{
  ATH_MSG_DEBUG("Initializing ...");

  ATH_CHECK(m_condSvc.retrieve());

  // Retrieve the MuonDetectorManager from the detector store to get 
  // the m_applyCscIntAlignment and m_applyMdtAsBuiltParams flags
  std::string managerName="Muon";
  const MuonGM::MuonDetectorManager* MuonDetMgrDS;
  if (detStore()->retrieve(MuonDetMgrDS).isFailure()) {
    ATH_MSG_INFO("Could not find the MuonGeoModel Manager: " << managerName << " from the Detector Store! " );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG(" Found the MuonGeoModel Manager from the Detector Store" );}

  m_applyCscIntAlignment = MuonDetMgrDS->applyCscIntAlignment();
  m_applyMdtAsBuiltParams = MuonDetMgrDS->applyMdtAsBuiltParams();

  std::cout << "m_applyCscIntAlignment " << m_applyCscIntAlignment << " m_applyMdtAsBuiltParams " << m_applyMdtAsBuiltParams << std::endl;

  // Read Handles
  ATH_CHECK( m_iGeoModelTool.retrieve() );

  ATH_CHECK(m_readALineKey.initialize());
  ATH_CHECK(m_readBLineKey.initialize());
  ATH_CHECK(m_readILineKey.initialize(m_applyCscIntAlignment));
  ATH_CHECK(m_readAsBuiltKey.initialize(m_applyMdtAsBuiltParams));

  // Write Handles
  // std::string ThisKey = "MuonDetectorManager";
  // std::size_t pos = name().find("MuonDetectorCondAlg");
  // m_writeDetectorManagerKey = ThisKey + name().substr (pos);
  ATH_CHECK(m_writeDetectorManagerKey.initialize());
  if(m_condSvc->regHandle(this, m_writeDetectorManagerKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeDetectorManagerKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}

StatusCode MuonDetectorCondAlg::execute()
{
  ATH_MSG_DEBUG( "execute " << name() ); 

  // =======================
  // Write ILine Cond Handle
  // =======================
  SG::WriteCondHandle<MuonGM::MuonDetectorManager> writeHandle{m_writeDetectorManagerKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
  		  << ". In theory this should not be called, but may happen"
  		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // =======================
  // Create the MuonDetectorManager by calling the MuonDetectorFactory001
  // =======================
  MuonDetectorTool* MuDetTool = dynamic_cast<MuonDetectorTool*>(m_iGeoModelTool.get());
  MuonGM::MuonDetectorFactory001 theFactory(detStore().operator->());
  if(MuDetTool->createFactory(theFactory).isFailure()){
    ATH_MSG_FATAL("unable to create MuonDetectorFactory001 ");
    return StatusCode::FAILURE;
  }

  std::unique_ptr<MuonGM::MuonDetectorManager> MuonMgrData = (std::unique_ptr<MuonGM::MuonDetectorManager>) theFactory.getDetectorManager();

  // =======================
  // Update Alignment, ALINES
  // =======================
  
  SG::ReadCondHandle<ALineMapContainer> readALinesHandle{m_readALineKey};
  const ALineMapContainer* readALinesCdo{*readALinesHandle}; 
  if(readALinesCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
    return StatusCode::FAILURE; 
  } 

  if (MuonMgrData->updateAlignment(readALinesCdo).isFailure()) ATH_MSG_ERROR("Unable to update Alignment" );
  else ATH_MSG_DEBUG("update Alignment DONE" );
  
  // =======================
  // Update Deformations, BLINES
  // =======================

  SG::ReadCondHandle<BLineMapContainer> readBLinesHandle{m_readBLineKey};
  const BLineMapContainer* readBLinesCdo{*readBLinesHandle}; 
  if(readBLinesCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read BLINES conditions object");
    return StatusCode::FAILURE; 
  } 
  
  writeHandle.addDependency( readALinesHandle, readBLinesHandle );

  if (MuonMgrData->updateDeformations(readBLinesCdo).isFailure()) ATH_MSG_ERROR("Unable to update Deformations" );
  else ATH_MSG_DEBUG("update Deformations DONE" );
  
  // =======================
  // Update CSC Internal Alignment if requested
  // =======================

  if (m_applyCscIntAlignment) {
    SG::ReadCondHandle<CscInternalAlignmentMapContainer> readILinesHandle{m_readILineKey};
    const CscInternalAlignmentMapContainer* readILinesCdo{*readILinesHandle}; 
    if(readILinesCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read CSC/ILINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    writeHandle.addDependency( readILinesHandle );
    
    if (MuonMgrData->updateCSCInternalAlignmentMap(readILinesCdo).isFailure()) ATH_MSG_ERROR("Unable to update CSC/ILINES" );
    else ATH_MSG_DEBUG("update CSC/ILINES DONE" );
  }

  // =======================
  // Update MdtAsBuiltMapContainer
  // =======================
  if (m_applyMdtAsBuiltParams) {
    SG::ReadCondHandle<MdtAsBuiltMapContainer> readAsBuiltHandle{m_readAsBuiltKey};
    const MdtAsBuiltMapContainer* readAsBuiltCdo{*readAsBuiltHandle}; 
    if(readAsBuiltCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MDT AsBuilt conditions object");
      return StatusCode::FAILURE; 
    } 
    
    writeHandle.addDependency( readAsBuiltHandle );

    if (MuonMgrData->updateAsBuiltParams(readAsBuiltCdo).isFailure()) ATH_MSG_ERROR("Unable to update MDT AsBuilt parameters" );
    else ATH_MSG_DEBUG("update MDT AsBuilt parameters DONE" );
  }

  // !!!!!!!! UPDATE ANYTHING ELSE ???????

  if (writeHandle.record(std::move(MuonMgrData)).isFailure()) {
    ATH_MSG_FATAL("Could not record MuonDetectorManager " << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}


StatusCode MuonDetectorCondAlg::finalize()
{
  ATH_MSG_DEBUG( "finalize " << name() );
  return StatusCode::SUCCESS;
 }

