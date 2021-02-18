/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HVJetMETFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/HVJetMETFilterTool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::HVJetMETFilterTool::HVJetMETFilterTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_metTerm("Final"),
  m_metCut(50000.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("METTerm", m_metTerm);
    declareProperty("METCut", m_metCut);
  }
  
// Destructor
DerivationFramework::HVJetMETFilterTool::~HVJetMETFilterTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::HVJetMETFilterTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_metSGKey.initialize());
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::HVJetMETFilterTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::HVJetMETFilterTool::eventPassesFilter() const
{
     ++m_ntot;

     const xAOD::MissingET* met(0);
     SG::ReadHandle<xAOD::MissingETContainer> metContainer(m_metSGKey);
     if( !metContainer.isValid() ) {
       msg(MSG::WARNING) << "No MET container found, will skip this event" << endmsg;
       return false;
     } 
     msg(MSG::DEBUG)<<"size of  MET container is "<<metContainer->size()<<endmsg;

     if (metContainer->size() !=0) {

       met = (*metContainer)[m_metTerm];
       if (!met) {
	 msg(MSG::WARNING) << "Cannot retrieve MissingET term " << m_metTerm << " in " << m_metSGKey << endmsg;
	 return false;
       }
 
       // double MET = metContainer->at(0)->met();

       // msg(MSG::DEBUG) << "MET and phi are " << met->met() << " " << met->phi() <<endmsg;

       bool passesEvent= (met->met() > m_metCut) ;
       
       if (passesEvent) {
	 ++m_npass;
       }
       return passesEvent;
       }
     return false;

}  
  
