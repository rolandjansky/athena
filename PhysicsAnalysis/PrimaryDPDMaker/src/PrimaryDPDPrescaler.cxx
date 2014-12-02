/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 *  @brief This file contains the class definition for the PrimaryDPDPrescaler class.
 *  @author Marcello Barisonzi <marcello.barisonzi@desy.de>
 *  $Id: PrimaryDPDPrescaler.cxx,v 1.6 2009-02-04 03:31:02 cote Exp $
 **/

#include "PrimaryDPDPrescaler.h"

// the user data-class defintions

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "xAODEventInfo/EventInfo.h"

#include "TRandom3.h"
#include "SGTools/crc64.h"
#include "GaudiKernel/FileIncident.h"
#include <sstream>




//___________________________________________________________________________
PrimaryDPDPrescaler::PrimaryDPDPrescaler(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthFilterAlgorithm(name, pSvcLocator), 
  m_tRand(0),
  m_evtcount(0),
  m_accepted(0),
  m_vetoed(0),
  m_processed(0),
  m_filtered(0),
  m_rollover(0),
  m_nameHash(0)
{
  declareProperty("Prescale",       m_prescale= 1.0);
  declareProperty("AcceptAlgs",     m_theAcceptAlgNames );
  declareProperty("RequireAlgs",    m_theRequireAlgNames );
}


  
//___________________________________________________________________________
PrimaryDPDPrescaler::~PrimaryDPDPrescaler()
{
}



//___________________________________________________________________________
StatusCode PrimaryDPDPrescaler::initialize()
{
  ATH_MSG_DEBUG( "in initialize()" );

  m_tRand = new TRandom3();
  // Create a 32-bit integer from the instance name of this class
  const uint64_t hash64 = SG::crc64( this->name() );
  uint32_t tmpInt = (uint32_t)(hash64 & 0xFFFFFFFF);
  m_nameHash = (unsigned int)tmpInt;


  // Decode the accept and require algorithms
  ATH_CHECK( decodeAlgorithms( &m_theAcceptAlgNames, &m_theAcceptAlgs) );
  ATH_CHECK( decodeAlgorithms( &m_theRequireAlgNames, &m_theRequireAlgs) );
  

  //Print summary info and declare all child filters to CutFlwSvc
  ATH_MSG_DEBUG( "Accept Algs, Size: "  <<  m_theAcceptAlgs.size()  <<  " Empty: " << m_theAcceptAlgs.empty() );
  for(std::vector<Algorithm*>::iterator i=m_theAcceptAlgs.begin(),end=m_theAcceptAlgs.end(); i != end; ++i) {
    ATH_MSG_DEBUG( (*i)->name() );
    cutFlowSvc()->registerCut((*i)->name(), "", cutID());
  }

  ATH_MSG_DEBUG( "Require Algs, Size: " <<  m_theRequireAlgs.size()  <<  " Empty: " << m_theRequireAlgs.empty() );
  for(std::vector<Algorithm*>::iterator i=m_theRequireAlgs.begin(),end=m_theRequireAlgs.end(); i != end; ++i) {
    ATH_MSG_DEBUG(  (*i)->name() );
    cutFlowSvc()->registerCut((*i)->name(), "", cutID());
  }


  //Final sanity checks
  if( m_theAcceptAlgs.empty() &&  m_theRequireAlgs.empty() )
    {
      ATH_MSG_WARNING ( "No filter algorithms at input. Counting all events." );
    }
   

  if( m_prescale <= 0.0 )
    {
      ATH_MSG_WARNING ( "Prescale factor <= 0. Resetting to default value (10)" );
      m_prescale = 1.0;
    }


  // Set to be listener for end of event
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.
  //incSvc->addListener(this, "LastInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.

  ResetSelfDescription(); 

  return StatusCode::SUCCESS;
}




//___________________________________________________________________________

void PrimaryDPDPrescaler::ResetSelfDescription(){
  //Default filter description (not applied if explicitely specified via python job properties)
  std::string descr="Prescale 1/";
  std::stringstream s_prescale;
  s_prescale << m_prescale;
  descr+=s_prescale.str();
  std::string transition = "";
  unsigned int nReqAlgs=m_theRequireAlgNames.value().size();
  if (nReqAlgs>0){ 
    transition = " and ";
    descr+=" if Required Algs: '";
    for(unsigned int i=0; i<nReqAlgs; i++){
      descr+=m_theRequireAlgNames.value().at(i);
      if(i<(nReqAlgs-1)){ descr+=" and "; }
      else{ descr+=" "; }
    }
    descr+="'";
  }
  unsigned int nAccAlgs=m_theAcceptAlgNames.value().size();
  if (nAccAlgs>0){ 
    descr+=(transition+" if Accept Algs: '");
    for(unsigned int i=0; i<nAccAlgs; i++){
      descr+=m_theAcceptAlgNames.value().at(i);
      if(i<(nAccAlgs-1)){ descr+=" or "; }
      else{ descr+=" "; }
    }
    descr+="'";
  }
  setFilterDescription(descr);
  return;
}



//___________________________________________________________________________
StatusCode PrimaryDPDPrescaler::execute()
{
  bool isVetoed     = false;

  // check if events passed filter
  bool isFiltered = isEventAccepted();


  if ( isFiltered ) 
    {
      m_filtered++;
      
      // Veto events; get random number between 0 and 1
      // but only if the prescale is different from 1.0
      if ( m_prescale > 1.00001 )
        {
          // Get the EventInfo object
          const xAOD::EventInfo* eventInfo(NULL);
          ATH_CHECK( evtStore()->retrieve(eventInfo) );
          unsigned int runNum = eventInfo->runNumber();
          unsigned int evtNum = eventInfo->eventNumber();

          // re-seed the random number generator for this event
          unsigned int seed = runNum + evtNum + m_nameHash;
          m_tRand->SetSeed(seed);

          double randomNumber = m_tRand->Rndm();
          if ( m_prescale * randomNumber < 1.0 )
            {
              isVetoed = false;
            }
          else
            {	  
              isVetoed = true;
            }
        }
      
      m_evtcount++;
      
      ATH_MSG_DEBUG ( "Event count: " << m_evtcount );
    }
  
  
  if( isVetoed )
    {
      ATH_MSG_DEBUG( "Event vetoed" );
      m_vetoed++;
    }
  else if ( isFiltered )
    {
      ATH_MSG_DEBUG ( "Event accepted" );
      m_accepted++;
    }
  else
    {
      ATH_MSG_DEBUG ( "Mother alg failed, skipping. Count: " << m_evtcount );
    }

  setFilterPassed( isFiltered && !isVetoed );	
  
  m_processed++;
      
  return StatusCode::SUCCESS;
}



//___________________________________________________________________________
StatusCode PrimaryDPDPrescaler::finalize()
{
  ATH_MSG_DEBUG( "***************************************************************");
  ATH_MSG_DEBUG( "Total processed events    : " << m_processed );
  ATH_MSG_DEBUG( "Passed the mother filters : " << m_filtered );
  ATH_MSG_DEBUG( "Accepted events           : " << m_accepted );
  ATH_MSG_DEBUG( "Vetoed events             : " << m_vetoed );
  ATH_MSG_DEBUG( "Nominal prescale/ratio    : " << m_prescale << " / " << 1./m_prescale );
  if( (m_accepted+m_vetoed)>0 ){
    ATH_MSG_DEBUG( "Accept ratio              : " << 1.*m_accepted/(m_accepted+m_vetoed) );
  }
  else{ ATH_MSG_DEBUG( "Accept ratio              : " << 0 ); }
  ATH_MSG_DEBUG( "File rollover             : " << m_rollover );
  ATH_MSG_DEBUG( "***************************************************************" ); 

  return StatusCode::SUCCESS;
}



//__________________________________________________________________________
void PrimaryDPDPrescaler::handle(const Incident& inc) {
  
  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if (fileInc == 0) {
    ATH_MSG_ERROR( " Unable to get FileName from BeginFile/EndFile incident" );
    return;
  }
  const std::string fileName = fileInc->fileName();
  ATH_MSG_DEBUG( "handle() " << inc.type() << " for " << fileName );

  if (inc.type() == "BeginInputFile") {
    m_evtcount = 0;
    m_rollover++;
     
    ATH_MSG_DEBUG ( "Event count reset to: " << m_evtcount );
    return;
  }
  else if (inc.type() == "EndInputFile") {

    ATH_MSG_DEBUG ( "End of file " << fileName << " event count: " << m_evtcount );
    return;
  }

  return;
}



//__________________________________________________________________________
StatusCode PrimaryDPDPrescaler::decodeAlgorithms(StringArrayProperty *theAlgNames, std::vector<Algorithm*> *theAlgs)
{
  // Reset the list of Algorithms
  theAlgs->clear( );
  
  
  
  ServiceHandle<IAlgManager> theAlgMgr("ApplicationMgr", this->name());
  StatusCode result = theAlgMgr.retrieve();

  if ( result.isSuccess( ) ) {
    // Build the list of Algorithms from the names list
    const std::vector<std::string>& nameList = theAlgNames->value( );
    std::vector<std::string>::const_iterator it;
    std::vector<std::string>::const_iterator itend = nameList.end( );
    
    for (it = nameList.begin(); it != itend; it++) {
      // Check whether the supplied name corresponds to an existing
      // Algorithm object.
      const std::string& theName = (*it);
      IAlgorithm* theIAlg;
      Algorithm*  theAlgorithm;
      result = theAlgMgr->getAlgorithm( theName, theIAlg );
      if ( result.isSuccess( ) ) {
	try{
	  theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
	} catch(...){
          result = StatusCode::FAILURE;
	}
      }
      if ( result.isSuccess( ) ) {
	// Check that the specified algorithm doesn't already exist in the list
	std::vector<Algorithm*>::iterator ita;
	std::vector<Algorithm*>::iterator itaend = theAlgs->end( );
	for (ita = theAlgs->begin(); ita != itaend; ita++) {
	  Algorithm* existAlgorithm = (*ita);
	  if ( theAlgorithm == existAlgorithm ) {
	    result = StatusCode::FAILURE;
	    break;
	  }
	}
   if ( result.isSuccess( ) ) {
	  theAlgs->push_back( theAlgorithm );
	}
      }
      else {
        ATH_MSG_DEBUG ( theName << " doesn't exist - ignored" );
      }
    }
    result = StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_FATAL ( "Can't locate ApplicationMgr!!!" );
  }
  return result;
}



//__________________________________________________________________________
bool PrimaryDPDPrescaler::isEventAccepted()
{
  typedef std::vector<Algorithm*>::iterator AlgIter;
  bool result = true;
 
  // Loop over all Algorithms in the accept list to see
  // whether any have been executed and have their filter
  // passed flag set. Any match causes the event to be
  // provisionally accepted. 
  // If no algs are present, count everything
  if ( m_theAcceptAlgs.empty() && m_theRequireAlgs.empty() ) 
    {
      result = true;
    }
  else
    {
      result = true;

      bool passAcceptAlgs(true);
      bool passRequireAlgs(true);


      // Accept Algs (OR)
      bool isPassed(false);
      if ( m_theAcceptAlgs.empty() )
        {
          isPassed = true;
        }
      for(AlgIter i=m_theAcceptAlgs.begin(),end=m_theAcceptAlgs.end(); i != end; ++i)
        {
          if ( (*i)->isExecuted() && (*i)->filterPassed() )
            {
              isPassed = true;
              break;
            }
        }
      passAcceptAlgs = isPassed;


      // Require Algs (AND)
      bool isFailed(false);
      for(AlgIter i=m_theRequireAlgs.begin(),end=m_theRequireAlgs.end(); i != end; ++i)
        {
          if ( (*i)->isExecuted() && !(*i)->filterPassed() )
            {
              isFailed = true;
              break;
            }    
        }
      passRequireAlgs = !isFailed;
    

      result = passAcceptAlgs && passRequireAlgs;
    }

  return result;
}
//__________________________________________________________________________

