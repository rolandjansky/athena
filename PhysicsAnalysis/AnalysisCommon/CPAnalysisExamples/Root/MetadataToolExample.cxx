/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CPAnalysisExamples includes
#include "CPAnalysisExamples/MetadataToolExample.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODMetaData/FileMetaData.h"

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

#include <boost/algorithm/string.hpp>

namespace CP {


MetadataToolExample::MetadataToolExample( const std::string& name ) : asg::AsgMetadataTool( name ){

   //cannot determine fast/fullsim in EventLoop yet, so declare it as a property
   //the user will need to set this
   //In athena, this will be determined from the input file automatically
   declareProperty("FastSim", m_isFastSim = false, "This will be set automatically for you if using athena");


}


StatusCode MetadataToolExample::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  //Don't do anything in the initialize step
  //just flag that we still need configuring
  m_isConfigured = false;

  return StatusCode::SUCCESS;
}

StatusCode MetadataToolExample::beginInputFile() {
   //when this method fires, you cannot yet use the information from evtStore 
   //instead you must use information from inputMetaStore()

   //currently only athena (POOL) can provide useful metadata
   //xAOD-format metdata will be coming soon
#ifndef ROOTCORE
   //Determine MC/Data
   std::string dataType("");
   CHECK( AthAnalysisHelper::retrieveMetadata( "/TagInfo", "project_name" , dataType, inputMetaStore() ) );
   m_isMC = (dataType == "IS_SIMULATION");

   //Determine Fast/FullSim
   if(m_isMC) {
      std::string simType("");
      CHECK( AthAnalysisHelper::retrieveMetadata( "/Simulation/Parameters", "SimulationFlavour" , simType, inputMetaStore() ) );
      boost::to_upper(simType);
      m_isFastSim = (simType.find("ATLFASTII")!=std::string::npos); 
   }

   m_isConfigured=true;
#endif

   //here's how things will work dual use, when file metadata is available in files
   if(!m_isConfigured && inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
      const xAOD::FileMetaData* fmd = 0;
      ATH_CHECK( inputMetaStore()->retrieve( fmd, "FileMetaData" ) );

      std::string simType("");
      bool result = fmd->value( xAOD::FileMetaData::simFlavour , simType );
      if(!result) { //no simFlavour metadata, so must be Data
         m_isMC = false;
      } else {
         m_isMC = true;
         boost::to_upper(simType);
         m_isFastSim = (simType.find("ATLFASTII")!=std::string::npos); 
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode MetadataToolExample::endInputFile() {
   m_isConfigured = false;
   return StatusCode::SUCCESS;
}

StatusCode MetadataToolExample::beginEvent() {
   if(m_isConfigured) return StatusCode::SUCCESS;

   //determine MC/Data from evtInfo ... this will work for both athena and eventloop
   const xAOD::EventInfo* evtInfo = 0;
   ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
   m_isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

   m_isConfigured=true;

   return StatusCode::SUCCESS;
}


void MetadataToolExample::print() const {

   if(!m_isConfigured) { //check if we still need to configure
      const_cast<MetadataToolExample*>(this)->beginInputFile().ignore(); 
      const_cast<MetadataToolExample*>(this)->beginEvent().ignore(); 
   }

   ATH_MSG_INFO("IsMC = " << m_isMC << " isFastSim = " << m_isFastSim);

}


}
