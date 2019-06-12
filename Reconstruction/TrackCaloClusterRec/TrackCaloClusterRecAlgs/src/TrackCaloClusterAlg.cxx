/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrackCaloClusterAlgs includes
#include "TrackCaloClusterRecAlgs/TrackCaloClusterAlg.h"

#include "TrackCaloClusterRecTools/TrackCaloClusterInfo.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODPFlow/TrackCaloClusterAuxContainer.h"



TrackCaloClusterAlg::TrackCaloClusterAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
  , m_tccTools(this)
{
      
  declareProperty("TCCInfo"      ,    m_tccInfoName = "TCCInfo");
  declareProperty("OutputTCCName",    m_outputTCCName   = "TrackCaloClusters", "Name of TrackCaloClusterContainer to be created"     );
  declareProperty("CopiedTCCName",    m_copiedTCCName   = ""  , "Name of TrackCaloClusterContainer to copy at the start of the output container (none by default)"   );
  declareProperty( "TCCTools", m_tccTools);

}
  
TrackCaloClusterAlg::~TrackCaloClusterAlg() {}
  
StatusCode TrackCaloClusterAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");    
  ATH_CHECK(m_tccTools.retrieve());
  return StatusCode::SUCCESS;
}
  
StatusCode TrackCaloClusterAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  return StatusCode::SUCCESS;
}

StatusCode TrackCaloClusterAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const TrackCaloClusterInfo * tccInfo;
  ATH_CHECK(evtStore()->retrieve(tccInfo, m_tccInfoName));

  
  // Create containers
  xAOD::TrackCaloClusterContainer* tccContainer = new xAOD::TrackCaloClusterContainer();
  ATH_CHECK( evtStore()->record( tccContainer, m_outputTCCName  ) );

  xAOD::TrackCaloClusterAuxContainer* tccContainerAux = new xAOD::TrackCaloClusterAuxContainer();
  ATH_CHECK( evtStore()->record( tccContainerAux, m_outputTCCName  + "Aux." ) );
  tccContainer->setStore( tccContainerAux );
  ATH_MSG_DEBUG( "Recorded TrackCaloClusterContainer with key: " << m_outputTCCName  );    

  // Optionnaly recopy an existing container.
  // This option is mainly to enable functionalities of the original alg. 
  if(m_copiedTCCName != ""){
    const xAOD::TrackCaloClusterContainer* copiedTCC;
    ATH_CHECK(evtStore()->retrieve(copiedTCC, m_copiedTCCName));
    for(const xAOD::TrackCaloCluster* tobecopied: *copiedTCC){
      xAOD::TrackCaloCluster* tcc = new xAOD::TrackCaloCluster;
      tccContainer->push_back(tcc);
      *tcc = *tobecopied;
    }
  }


  // Loop over each TCC tool  ...
  for(const ToolHandle<ITrackCaloClusterTool> & tool : m_tccTools){
    // .. and have it fills  tccContainer
    StatusCode sc=tool->fillTCC(tccContainer, *tccInfo);
    if(sc.isFailure()){
      ATH_MSG_ERROR(" TCC tool "<< tool->name() << "  failed. "); return sc;
    }
  }
  
    
    
  return StatusCode::SUCCESS;
}
