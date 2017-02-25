// $Id$

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "TString.h"

namespace xAOD {

   TruthWeightTool::TruthWeightTool( const std::string& name )
     :asg::AsgMetadataTool( name ), m_metaDataContainer(nullptr),
      m_metaData(nullptr), m_uninitialized(true), m_evtInfo(nullptr)  {
      declareProperty( "MetaObjectName", m_metaName = "TruthMetaData" );
   }

   StatusCode TruthWeightTool::initialize() {
      ATH_MSG_WARNING( "Initialising... " );
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
   
  size_t TruthWeightTool::getWeightIndex(std::string weightName) {
    auto sp = spawnIndexRetriever(weightName);
    if (!sp) { ATH_MSG_ERROR("Requested weight \'"+weightName+"\' doesn't exist"); return 0; }
    return sp->getIndex();
  }

   void TruthWeightTool::onNewMetaData(){
     ATH_MSG_DEBUG( "Updating all spawned index retreivers" );
     for(auto& indexRetriever: m_indexRetrievers) {
       auto sp =indexRetriever.second.lock();
       if (sp) sp->update(m_metaData);
     }
   }
   
   StatusCode TruthWeightTool::beginInputFile() {
      ATH_MSG_DEBUG( "Retrieving truth meta data from a new file" );
      m_metaDataContainer = nullptr;
      ATH_CHECK( inputMetaStore()->retrieve( m_metaDataContainer, m_metaName ) );
      return StatusCode::SUCCESS;
   }

   StatusCode TruthWeightTool::beginEvent() {
     //const xAOD::EventInfo* ei = nullptr; 
      m_evtInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve( m_evtInfo, "EventInfo" ) );
      uint32_t mcChannelNumber = m_evtInfo->mcChannelNumber();

      //ATH_CHECK( evtStore()->retrieve( m_truthEvents, "TruthEvents" ) );

      if( m_uninitialized || ( mcChannelNumber != m_mcChanNo ) ) {

	for( auto metaDataIterator = m_metaDataContainer->begin(); 
	metaDataIterator != m_metaDataContainer->end(); metaDataIterator++) {

      	  if( (*metaDataIterator)->mcChannelNumber() == mcChannelNumber)
          {
     	     m_uninitialized = false;
	     m_mcChanNo = mcChannelNumber;
	     m_metaData = *metaDataIterator;
             this->onNewMetaData();
	     m_weightIndices.clear();
	     for (auto weightName:m_metaData->weightNames())
	       m_weightIndices.push_back(getWeightIndex(weightName));
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

  bool TruthWeightTool::hasWeight(std::string weightName) {
    const std::vector<std::string> &wns = getWeightNames();
    return std::find(wns.begin(),wns.end(),weightName) != wns.end();
  }

  std::vector<float> TruthWeightTool::getMCweights() {
    std::vector<float> ws;
    if (m_uninitialized||m_evtInfo==nullptr) {
      ATH_MSG_ERROR("Cannot call TruthWeightTool::getMCweights prior to first event beling loaded");
      return ws;
    }
    const std::vector<float> &mcWeights = m_evtInfo->mcEventWeights();
    if (mcWeights.size()!=m_weightIndices.size()) {
      // TODO: if we get here, check the truthEvent 
      ATH_MSG_ERROR(Form("Current event has %lu weights, expect %lu from the metadata! Will return no weights",
			 mcWeights.size(),m_weightIndices.size()));
    }
    for (auto i:m_weightIndices) ws.push_back(mcWeights.at(i));
    return ws;
  }


} // namespace xAOD
