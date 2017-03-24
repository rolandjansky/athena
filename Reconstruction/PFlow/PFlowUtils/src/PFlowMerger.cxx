// PFlowUtils includes
#include "PFlowMerger.h"

PFlowMerger::PFlowMerger( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){
  
  declareProperty( "InputContainerNames", m_inputContainerNames = std::vector<std::string>({"JetETMissChargedParticleFlowObjects","JetETMissNeutralParticleFlowObjects"}));
  declareProperty( "OutputContainerName", m_outputContainerName = "JetETMissParticleFlowObjects");
}
    
PFlowMerger::~PFlowMerger() {}

StatusCode PFlowMerger::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode PFlowMerger::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode PFlowMerger::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  	const xAOD::PFOContainer* input;

  	xAOD::PFOContainer* temp = new xAOD::PFOContainer();
  	xAOD::PFOAuxContainer* tempAux = new xAOD::PFOAuxContainer();
  	temp->setStore(tempAux);
  	
    for (auto inputName: m_inputContainerNames){
  		ATH_CHECK(evtStore()->retrieve(input, inputName));
  		//std::pair< xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > copy = xAOD::shallowCopyContainer(*input);

  		for (const xAOD::PFO* pfo: *input){
  			xAOD::PFO* p=new xAOD::PFO();
  			p->makePrivateStore(*pfo);
  			temp->push_back(const_cast<xAOD::PFO *>(p));
  		}
  	}

    /*
  	if(evtStore()->record( temp, m_outputContainerName ).isFailure() || evtStore()->record( tempAux, m_outputContainerName+"Aux." ).isFailure() ){
      ATH_MSG_ERROR("Unable to record " << m_outputContainerName );
      return NULL;
    }
    */

    std::pair< xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > output = xAOD::shallowCopyContainer(*temp );
    output.second->setShallowIO(false);
    if(evtStore()->record( output.first, m_outputContainerName ).isFailure() || evtStore()->record( output.second, m_outputContainerName+"Aux." ).isFailure() ){
        ATH_MSG_ERROR("Unable to record " << m_outputContainerName );
        return NULL;
    }

  return StatusCode::SUCCESS;
}


