/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "fastjet/ClusterSequenceArea.hh"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"
#include "EventShapeTools/EventEtDensityClusterTool.h"

#include "xAODEventShape/EventShape.h"
#include "xAODEventShape/EventShapeAuxInfo.h"

EventEtDensityClusterTool::EventEtDensityClusterTool(const std::string& type,
						     const std::string& name,
						     const IInterface* pParent)
  : EventEtDensityProviderBase(type,name,pParent)
  , m_clusterEtMin(0.)
  , m_useAreaFourMom(false)
{
  declareProperty("ClusterPtMin",m_clusterEtMin);
  declareProperty("UseAreaFourMomentum",m_useAreaFourMom);
  declareProperty("EventShapeKey",m_evtShapeKey="");
}

EventEtDensityClusterTool::~EventEtDensityClusterTool()
{ }

StatusCode EventEtDensityClusterTool::executeAction() 
{
  this->valueServer().reset();
  // retrieve input collection
  const xAOD::CaloClusterContainer* pClusColl = 0;
  if ( evtStore()->retrieve(pClusColl,inputCollection()).isFailure() )
    {
      ATH_MSG_WARNING("Cannot allocate CaloClusterContainer with key <"
		      << inputCollection() << ">");
      return StatusCode::SUCCESS;
    }

  // convert to PseudoJets
  jetcontainer_t inJets; inJets.reserve(pClusColl->size());
  int i=0;

  for( auto it=pClusColl->begin(); it != pClusColl->end(); it++){
    const xAOD::CaloCluster * clus = *it;
    // change the cluster state
    CaloClusterChangeSignalState changeState(clus,(xAOD::CaloCluster::State)signalState()); //The destructor of this one will reset the state
    // build the PseudoJet
    if(clus->e()<0) continue;
    const xAOD::CaloCluster::FourMom_t & p4 = clus->p4();
    inJets.push_back( jet_t( p4.Px(), p4.Py(), p4.Pz(), p4.E() ) );
    inJets.back().set_user_index(i);
    i++;
  }


  // apply FastJetTool
  jetcontainer_t outJets;
  if ( m_fastJetTool->execute(inJets,outJets).isFailure() )
    {
      ATH_MSG_WARNING("Failed to execute " 
		      << m_fastJetTool->type()  << "/"
		      << m_fastJetTool->name() << " -> event Et density = 0");
      return StatusCode::SUCCESS;
    }
  
  // get cluster sequence
  const fastjet::ClusterSequenceArea* pSeq = 
    dynamic_cast<const fastjet::ClusterSequenceArea*>(m_fastJetTool->
						      clusterSequence());
  if ( pSeq == 0 )
    {
      ATH_MSG_ERROR("Configuration mismatch: need valid ClusterSequenceArea"
		    << " from FastJet tool " 
		      << m_fastJetTool->type()  << "/"
		    << m_fastJetTool->name()    << " - UNRECOVERABLE");
      return StatusCode::FAILURE;
    }
  
  // get rho etc.
  //this->etaRange().get_rap_limits(this->valueServer().etaRangeLow,
  // this->etaRange().get_rapidity_extent(this->valueServer().etaRangeLow,
  //                                      this->valueServer().etaRangeHigh);
  this->valueServer().etaRangeLow = etaMin();
  this->valueServer().etaRangeHigh = etaMax();
  ATH_MSG_DEBUG("calculating rho ... InputCluster " << inJets.size() << "  pt= "<< inJets.front().pt() << "  "<< inJets.back().pt());
  fastjet::Selector sel = this->etaRange();
  pSeq->get_median_rho_and_sigma(sel,
                                 m_useAreaFourMom,
                                 this->valueServer().rhoValue,
                                 this->valueServer().rhoSigma,
                                 this->valueServer().areaValue);
  ATH_MSG_DEBUG("calculated rho  : "<< this->valueServer().rhoValue);
  
  if(m_evtShapeKey != ""){ 
    ATH_MSG_DEBUG(" Filling EventShape object ");
    // Fill a EventShape object 
    xAOD::EventShape * evtshape;
    if( evtStore()->contains<xAOD::EventShape>(m_evtShapeKey) ){
      // need to const_cast as long as we don't know how to *add* info
      evtshape = const_cast<xAOD::EventShape*>(evtStore()->retrieve<xAOD::EventShape>(m_evtShapeKey) );
    }else {
      // build and record
      evtshape = new xAOD::EventShape();
      xAOD::EventShapeAuxInfo* aux = new xAOD::EventShapeAuxInfo();
      evtshape->setStore( aux );
      CHECK( evtStore()->record( aux, m_evtShapeKey + "Aux." ) );
      CHECK( evtStore()->record( evtshape, m_evtShapeKey ) );
      ATH_MSG_DEBUG(" Built a new EventShape object "<< m_evtShapeKey);
    }

    xAOD::EventShape::EventDensityID id = xAOD::EventShape::UnknownDensity;
    if(m_fastJetTool->getJetDefinition()->R() == 0.4 ) id = xAOD::EventShape::DensityForJetsR4;
    else if (m_fastJetTool->getJetDefinition()->R() == 0.6 ) id = xAOD::EventShape::DensityForJetsR6;
    else { ATH_MSG_WARNING("Unknown density : R="<< m_fastJetTool->getJetDefinition()->R() << "  for requested EventShape key="<< m_evtShapeKey); }
    
    if( id != xAOD::EventShape::UnknownDensity) {
      // Fill the EventShape object
      typedef xAOD::EventShape::EventDensityID DensityID;
      evtshape->setDensity( id, this->valueServer().rhoValue );
      evtshape->setDensity( (DensityID) (id+1), this->valueServer().rhoSigma );
      evtshape->setDensity( (DensityID) (id+2), this->valueServer().areaValue);
      ATH_MSG_DEBUG(" Filled EventShape object ");
    } 
    
  }
    
  
    

  // 
  return StatusCode::SUCCESS;
}

StatusCode EventEtDensityClusterTool::executeAction(double eta)
{
  this->valueServer().reset();
  return ( (etaMin()<eta) && (eta< etaMax()) ) 
    ? this->executeAction() : StatusCode::SUCCESS;
}

StatusCode EventEtDensityClusterTool::executeAction(double etaMin,double etaMax)
{
  this->valueServer().reset();
  return ( (this->etaMin()<etaMin) && (etaMin< this->etaMax()) && 
           (this->etaMin()<etaMax) && (etaMax< this->etaMax()) )
    ? this->executeAction() : StatusCode::SUCCESS;
}
