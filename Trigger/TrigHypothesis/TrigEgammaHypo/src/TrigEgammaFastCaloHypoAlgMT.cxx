/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "TrigEgammaFastCaloHypoAlgMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaFastCaloHypoAlgMT::TrigEgammaFastCaloHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ),
   m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
  {
    declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
  }


StatusCode TrigEgammaFastCaloHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );

  m_ringerTool_vLoose.setConstantsCalibPath( m_constantsCalibPath_vLoose ); 
  m_ringerTool_vLoose.setThresholdsCalibPath( m_thresholdsCalibPath_vLoose );
  m_ringerTool_Loose.setConstantsCalibPath( m_constantsCalibPath_Loose );
  m_ringerTool_Loose.setThresholdsCalibPath( m_thresholdsCalibPath_Loose );
  m_ringerTool_Medium.setConstantsCalibPath( m_constantsCalibPath_Medium );
  m_ringerTool_Medium.setThresholdsCalibPath( m_thresholdsCalibPath_Medium );
  m_ringerTool_Tight.setConstantsCalibPath( m_constantsCalibPath_Tight );
  m_ringerTool_Tight.setThresholdsCalibPath( m_thresholdsCalibPath_Tight );
       
  if(m_ringerTool_vLoose.initialize().isFailure()||m_ringerTool_Loose.initialize().isFailure()||m_ringerTool_Medium.initialize().isFailure()||m_ringerTool_Tight.initialize().isFailure())
    return StatusCode::FAILURE;
  else
    ATH_MSG_DEBUG( "Initialization of RingerTool completed successfully"   ); 

  ATH_CHECK( m_clustersKey.initialize() );
  ATH_CHECK( m_ringsKey.initialize(SG::AllowEmpty));

  renounce( m_clustersKey );// clusters are made in views, so they are not in the EvtStore: hide them
  renounce( m_ringsKey );

  ATH_MSG_DEBUG( "Initialization of FastCaloHypoAlg completed successfully");
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaFastCaloHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");
  
  // new decisions

  // new output decisions
  SG::WriteHandle<TCU::DecisionContainer> outputHandle = TCU::createAndStore(decisionOutput(), context );
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo> toolInput;

  // loop over previous decisions
  size_t counter=0;
  for ( const auto previousDecision: *previousDecisionsHandle ) {
    //get RoI  
    auto roiELInfo = TCU::findLink<TrigRoiDescriptorCollection>( previousDecision, TCU::initialRoIString() );
    
    ATH_CHECK( roiELInfo.isValid() );
    const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get View
    const auto viewEL = previousDecision->objectLink<ViewContainer>( TCU::viewString() );
    ATH_CHECK( viewEL.isValid() );

    // get cluster
    auto clusterHandle = ViewHelper::makeHandle( *viewEL, m_clustersKey, context);
    ATH_CHECK( clusterHandle.isValid() );
    ATH_MSG_DEBUG ( "Cluster handle size: " << clusterHandle->size() << "..." );

    auto d = TCU::newDecisionIn( decisions, TCU::hypoAlgNodeName() );
    
     
    // get Rings
    bool accept_vLoose, accept_Loose, accept_Medium, accept_Tight;
    accept_vLoose = false;
    accept_Loose = false;
    accept_Medium = false;
    accept_Tight = false;

    const xAOD::TrigRingerRingsContainer* ringHandle = nullptr;    
    if ( not m_ringsKey.empty() ) {      
      auto ringerShapeHandle = ViewHelper::makeHandle( *viewEL, m_ringsKey, context);      
      ATH_CHECK( ringerShapeHandle.isValid());
      ringHandle = ringerShapeHandle.cptr();	
      ATH_MSG_DEBUG ( "Ringer handle size: " << ringerShapeHandle->size() << "..." );

      // link the rings      
      auto el = ViewHelper::makeLink( *viewEL, ringerShapeHandle, 0 );
      ATH_CHECK( el.isValid() );
      d->setObjectLink( "ringer",  el );
      auto ringerShape = ringHandle->at(0);
      const xAOD::TrigEMCluster *emCluster = 0;      
      
      if(ringerShape){
        emCluster = ringerShape->emCluster();
        if(emCluster){
         float avgmu   = m_lumiBlockMuTool->averageInteractionsPerCrossing();
         float output_vLoose, output_Loose, output_Medium, output_Tight;
         const std::vector<float> rings = ringerShape->rings();
         std::vector<float> refRings(rings.size());
         refRings.assign(rings.begin(), rings.end());
         output_vLoose = m_ringerTool_vLoose.calculate( refRings, emCluster->et(), emCluster->eta(), avgmu );
         output_Loose = m_ringerTool_Loose.calculate( refRings, emCluster->et(), emCluster->eta(), avgmu );
         output_Medium = m_ringerTool_Medium.calculate( refRings, emCluster->et(), emCluster->eta(), avgmu );
         output_Tight = m_ringerTool_Tight.calculate( refRings, emCluster->et(), emCluster->eta(), avgmu );
        
         ATH_MSG_DEBUG(name()<< " generate as NN output_vLoose " << output_vLoose );
         ATH_MSG_DEBUG(name()<< " generate as NN output_Loose " << output_Loose );
         ATH_MSG_DEBUG(name()<< " generate as NN output_Medium " << output_Medium );
         ATH_MSG_DEBUG(name()<< " generate as NN output_Tight " << output_Tight );
 
         accept_vLoose = m_ringerTool_vLoose.accept(output_vLoose, emCluster->et(),emCluster->eta(),avgmu) ;
         accept_Loose = m_ringerTool_Loose.accept(output_Loose, emCluster->et(),emCluster->eta(),avgmu) ;
         accept_Medium = m_ringerTool_Medium.accept(output_Medium, emCluster->et(),emCluster->eta(),avgmu) ;
         accept_Tight = m_ringerTool_Tight.accept(output_Tight, emCluster->et(),emCluster->eta(),avgmu) ;
       }
      }
     }
     bool const accept_vLoose_const = accept_vLoose;
     bool const accept_Loose_const = accept_Loose;
     bool const accept_Medium_const = accept_Medium;
     bool const accept_Tight_const = accept_Tight;

     ATH_MSG_DEBUG("accept_vLoose_const: "<<accept_vLoose_const);
     ATH_MSG_DEBUG("accept_Loose_const: "<<accept_Loose_const);
     ATH_MSG_DEBUG("accept_Medium_const: "<<accept_Medium_const);
     ATH_MSG_DEBUG("accept_Tight_const: "<<accept_Tight_const);

      // create new decision
     toolInput.emplace_back( d, roi, clusterHandle.cptr()->at(0), ringHandle->at(0), accept_vLoose_const, accept_Loose_const, accept_Medium_const, accept_Tight_const, previousDecision );
    

     // link the cluster
     { 
       auto clus = ViewHelper::makeLink( *viewEL, clusterHandle, 0 );
       ATH_CHECK( clus.isValid() );
       d->setObjectLink( TCU::featureString(),  clus );
     }
    
     d->setObjectLink( TCU::roiString(), roiELInfo.link );
    
     TCU::linkToPrevious( d, previousDecision, context );
     ATH_MSG_DEBUG( "Added view, roi, cluster, previous decision to new decision " << counter << " for view " << (*viewEL)->name()  );
     counter++;

  }

  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");

   
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
