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
      ATH_MSG_DEBUG( "Initialising... " );
      // AsgMetadataTool needs to call sysInitize to be registered in the incident svc
      // "hack" to make sure sysInitlize is called (only once, or we get caut in infinite loop)
      bool static first=true;
      if (first) {
	first=false; 
	return sysInitialize();
      }
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
      if (m_metaDataContainer==nullptr) std::runtime_error("Cannot access metadata: "+m_metaName);
      return StatusCode::SUCCESS;
   }

   StatusCode TruthWeightTool::beginEvent() {
      m_evtInfo = nullptr;
      ATH_CHECK( evtStore()->retrieve( m_evtInfo, "EventInfo" ) );
      uint32_t mcChannelNumber = m_evtInfo->mcChannelNumber();

      //ATH_CHECK( evtStore()->retrieve( m_truthEvents, "TruthEvents" ) );

      if( m_uninitialized || ( mcChannelNumber != m_mcChanNo ) ) {

	for (auto metaData:*m_metaDataContainer) {

      	  if ( metaData->mcChannelNumber() == mcChannelNumber) {
     	     m_uninitialized = false;
	     m_mcChanNo = mcChannelNumber;
	     m_metaData = metaData;
             this->onNewMetaData();
	     return StatusCode::SUCCESS;
          }
        }
        ATH_MSG_ERROR( "No matching TruthMetaData found" );
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
   }

   std::vector<std::string> const & TruthWeightTool::getWeightNames() const {
      if (m_uninitialized) {
        ATH_MSG_ERROR( "Weight name not found in event, is the meta data already loaded?" );
        throw std::runtime_error("Weight name not found in event, is the meta data already loaded?");
      }
      return m_metaData->weightNames();
   }

  bool TruthWeightTool::hasWeight(std::string weightName) {
    const std::vector<std::string> &wns = getWeightNames();
    return std::find(wns.begin(),wns.end(),weightName) != wns.end();
  }

  const std::vector<float> &TruthWeightTool::getWeights() const {
    if (m_uninitialized||m_evtInfo==nullptr) 
      std::runtime_error("Cannot access MC weights. Tool is not properly initialized.");
    // TODO: first check truth event. Else take from EventINfo. Check number of weights
    return m_evtInfo->mcEventWeights();
  }

} // namespace xAOD
