/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : EgammaTagTool.cxx
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a collection of EgammaTag

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "ElectronPhotonTagTools/EgammaTagTool.h"
#include "TagEvent/EgammaAttributeNames.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"

//using namespace Analysis;

/** the constructor */
EgammaTagTool::EgammaTagTool (const std::string& type, const std::string& name, 
                              const IInterface* parent) : 
  AthAlgTool( type, name, parent )
{
  /** Electron/Photon AOD Container Names */
  declareProperty("ElectronContainer", m_electronContainer = "ElectronCollection");
  declareProperty("PhotonContainer",   m_photonContainer   = "PhotonCollection");
  /** selection cut of Pt */
  declareProperty("ElectronPtCut",     m_electronPtCut = 20.0*Gaudi::Units::GeV);
  declareProperty("PhotonPtCut",       m_photonPtCut   = 10.0*Gaudi::Units::GeV);
  /** mass cut for Z */
  declareProperty("MassZLow",          m_massZLow   = 45.*Gaudi::Units::GeV );
  declareProperty("MassZHigh",         m_massZHigh  = 180.*Gaudi::Units::GeV);
  /** missing Et cut */
  declareProperty("MissingEtObject",   m_missingEtObject = "MET_RefFinal");
  declareProperty("MissingEtCut",      m_missingEtCut    = 25.*Gaudi::Units::GeV );
  declareProperty("WElectronPtCut",    m_welectronPtCut  = 15.*Gaudi::Units::GeV );
  declareProperty("MissingEtTerm",     m_missingEtTerm   = "Final");

  declareInterface<EgammaTagTool>( this );
}



/** initialization - called once at the begginning */
StatusCode  EgammaTagTool::initialize() 
{
  ATH_MSG_DEBUG("in initialize()");
  ATH_CHECK( m_evt.initialize() );
  return StatusCode::SUCCESS;
}



/** build the attribute list - called in initialize */
StatusCode EgammaTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int& /*max*/)
{
  ATH_MSG_DEBUG("in attributeSpecification()");

  /** specifiy the Egamma - Electron/Photon attributes */
  attrMap[ EgammaAttributeNames[0] ] = AthenaAttributeType("unsigned int", EgammaAttributeUnitNames[0], EgammaAttributeGroupNames[0]);

  return StatusCode::SUCCESS;
}



/** execute - called on every event */
StatusCode EgammaTagTool::execute(TagFragmentCollection& egammaTagCol, const int& /*max*/)
{
  ATH_MSG_DEBUG("in execute()");
  
  const xAOD::ElectronContainer* eleColl = nullptr;
  ATH_CHECK( evtStore()->retrieve( eleColl, m_electronContainer) );

  const xAOD::MissingETContainer* metTopo = nullptr;
  ATH_CHECK( evtStore()->retrieve(metTopo, m_missingEtObject) );

  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  /** Flags for Z and W events */ 
  m_flagZeeEvent  = false;
  m_flagWenuEvent = false;
  
  //===================================================
  // Zee event reconstruction
  //===================================================
  m_flagZeeEvent = ZeeSelection (eleColl, evt);

  //===================================================
  // Wenu event reconstruction
  //===================================================
  m_flagWenuEvent = WenuSelection (eleColl, metTopo, evt);

  
  //===================================================
  // Set the bits for Z and W events
  //===================================================
  unsigned int fragment = 0x0;
  if(m_flagZeeEvent) fragment |= 1<<0;
  if(m_flagWenuEvent) fragment |= 1<<1; 

  
  egammaTagCol.insert( EgammaAttributeNames[0], fragment );

  return StatusCode::SUCCESS;
}

/** finialize - called once at the end */
StatusCode  EgammaTagTool::finalize() {

  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}


bool EgammaTagTool::ZeeSelection (const xAOD::ElectronContainer* eleColl,
                                  SG::ReadHandle<xAOD::EventInfo> eventInfo)
{

  ATH_MSG_DEBUG("in ZeeSelection()");

  /** Electron candidates */
  std::vector<const xAOD::Electron*> electrons_positive;
  std::vector<const xAOD::Electron*> electrons_negative;


  xAOD::ElectronContainer::const_iterator eleItr    = eleColl->begin();
  xAOD::ElectronContainer::const_iterator eleItrEnd = eleColl->end();
  for ( ; eleItr != eleItrEnd; ++eleItr  ) {
    /** only allow electons from "Electron" algo */
    bool select = ( (*eleItr)->author(xAOD::EgammaParameters::AuthorElectron ) && ( (*eleItr)->pt() > m_electronPtCut));
    if ( select ) {
      if ( (*eleItr)->charge() > 0 ) electrons_positive.push_back( (*eleItr) );
      else if ((*eleItr)->charge() < 0 ) electrons_negative.push_back( (*eleItr) );
    }
  }
  
  /** Check existance of electron candidates */
  if ( (electrons_positive.size()==0) || (electrons_negative.size()==0) ) {
    m_flagZeeEvent = false;
    return m_flagZeeEvent;
  }

  for (unsigned int i=0; i<electrons_positive.size(); i++) {
    for (unsigned int j=0; j<electrons_negative.size(); j++) {
      
      bool value_loose_pos=0;
      bool value_loose_neg=0;
      
      if(!(electrons_positive[i]->passSelection(value_loose_pos,"Loose"))){
	ATH_MSG_ERROR( "No loose selection exits" );
      }
      
      if(!(electrons_negative[i]->passSelection(value_loose_neg,"Loose"))){
	ATH_MSG_ERROR( "No loose selection exits" );
      }
      
      if ( value_loose_pos == 1 || value_loose_neg == 1 ) {
	double invMass = -999.;
	
	/** Calculate invariant mass of ee */
	invMass = ( electrons_positive[ i ]->p4() + electrons_negative[ i ]->p4() ).M();

	
	/** Make decision for a good Zee */
	if ( (m_massZLow < invMass) && (invMass < m_massZHigh) ) m_flagZeeEvent = true;
	
	/** Print the details of Zee Candidate if outputLevel=DEBUG */      
	if (m_flagZeeEvent && msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG("***** First electron candidate of Zee  *****" );
          dumpEventDetails(electrons_positive[i], eventInfo);
          ATH_MSG_DEBUG("***** Second electron candidate of Zee  *****" );
          dumpEventDetails(electrons_negative[j], eventInfo);
        }
       }
    }
  }

  return m_flagZeeEvent;

}


bool EgammaTagTool::WenuSelection (const xAOD::ElectronContainer* eleColl,
                                   const xAOD::MissingETContainer* metTopo,
                                   SG::ReadHandle<xAOD::EventInfo> eventInfo)
{
  ATH_MSG_DEBUG("in WenuSelection()");
  
  if  ((*metTopo)[m_missingEtTerm]->met() < m_missingEtCut ) {
    m_flagWenuEvent = false;
    return m_flagWenuEvent;
  }  
  
  xAOD::ElectronContainer::const_iterator eleItr    = eleColl->begin();
  xAOD::ElectronContainer::const_iterator eleItrEnd = eleColl->end();
  for ( ; eleItr != eleItrEnd; ++eleItr  ) {
    bool value_medium=0;
    
    if(!(*eleItr)->passSelection(value_medium,"Medium")){
      ATH_MSG_ERROR( "No medium selection exits" );
    }
    if(value_medium == 0)continue;
    bool select = ( (*eleItr)->author(xAOD::EgammaParameters::AuthorElectron ) && ( (*eleItr)->pt() > m_welectronPtCut));
 
    if (select) m_flagWenuEvent = true;

    if (m_flagWenuEvent && msgLvl(MSG::DEBUG)) {
       ATH_MSG_DEBUG("***** Electron Candidate in Wenu *****" );
       dumpEventDetails(*eleItr, eventInfo);
    } 
  }

  return m_flagWenuEvent;

}

void EgammaTagTool::dumpEventDetails(const xAOD::Electron* eleItr,
                                     SG::ReadHandle<xAOD::EventInfo> eventInfo)
{
  ATH_MSG_DEBUG("Event info : "
		<< " runNumber = " << eventInfo->runNumber()
		<< " lumiBlock = " << eventInfo->lumiBlock()
		<< " eventNumber = " << eventInfo->eventNumber()
		);

  ATH_MSG_DEBUG("Electrons : "
		<<" pt = "  << eleItr->pt()
		<<" eta = " << eleItr->eta()
		<<" phi = " << eleItr->phi()
		<<" charge = " << eleItr->charge()
		);

  ATH_MSG_DEBUG("Invariant mass  = " << m_invMass );
}
