// $Id$

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "xAODEventInfo/EventInfo.h"

namespace xAOD {

   TruthWeightTool::TruthWeightTool( const std::string& name )
     :asg::AsgMetadataTool( name ), m_metaDataContainer(nullptr),
      m_metaData(nullptr), m_uninitialized(true)  {
      declareProperty( "MetaObjectName", m_metaName = "TruthMetaData" );
   }

   StatusCode TruthWeightTool::initialize() {
      ATH_MSG_INFO( "Initialising... " );
      return StatusCode::SUCCESS;
   }

   std::shared_ptr<IIndexRetriever> TruthWeightTool::spawnIndexRetriever(std::string weightName) { 
     auto sp = m_indexRetrievers[weightName].lock();
     if(!sp) {
       m_indexRetrievers[weightName] = sp = std::make_shared<IndexRetriever>(weightName);	   
       sp->update(m_metaData);
     }
     return sp;
   }
   
  int TruthWeightTool::getWeightIndex(std::string weightName) {
    auto sp = spawnIndexRetriever(weightName);
    if (sp) return sp->getIndex();
    return -1;
  }

   void TruthWeightTool::onNewMetaData(){
     ATH_MSG_DEBUG( "Updating all spawned index retreivers" );
     for(auto& indexRetriever: m_indexRetrievers){
	     auto sp =indexRetriever.second.lock();
	     if(sp) sp->update(m_metaData);
     }
   }
   
   StatusCode TruthWeightTool::beginInputFile() {
      ATH_MSG_DEBUG( "Retrieving truth meta data from a new file" );
      m_metaDataContainer = nullptr;
      ATH_CHECK( inputMetaStore()->retrieve( m_metaDataContainer, m_metaName ) );
      return StatusCode::SUCCESS;
   }

   StatusCode TruthWeightTool::beginEvent() {
      const xAOD::EventInfo* ei = nullptr; 
      ATH_CHECK( evtStore()->retrieve( ei, "EventInfo" ) );
      uint32_t mcChannelNumber = ei->mcChannelNumber();

      if( m_uninitialized || ( mcChannelNumber != m_mcChanNo ) ) {

	for( auto metaDataIterator = m_metaDataContainer->begin(); 
	metaDataIterator != m_metaDataContainer->end(); metaDataIterator++) {

      	  if( (*metaDataIterator)->mcChannelNumber() == mcChannelNumber)
          {
     	     m_uninitialized = false;
	     m_mcChanNo = mcChannelNumber;
	     m_metaData = *metaDataIterator;
             this->onNewMetaData();
	     return StatusCode::SUCCESS;
          }
        }
        return StatusCode::FAILURE;
        ATH_MSG_ERROR( "No matching meta data found" );
      }
      return StatusCode::SUCCESS;
   }

   std::vector<std::string> const & TruthWeightTool::getWeightNames() const {
      if(m_uninitialized) {
        ATH_MSG_ERROR( "Weight name not found in event, is the meta data already loaded?" );
        throw std::runtime_error("Weight name not found in event, is the meta data already loaded?");
      }
      return m_metaData->weightNames();
   }
} // namespace xAOD
