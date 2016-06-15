///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecisionSvc.cxx
// Implementation file for class DecisionSvc
// Author: S.Binet<binet@cern.ch>
//         B.Radics<radbal@cern.ch>
///////////////////////////////////////////////////////////////////


#include "DecisionSvc.h"

// Gaudi include files
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include <algorithm>
#include <sstream>


DecisionSvc::DecisionSvc(const std::string& name,
		       ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_calcStats(false),
  m_frozen(false),
#ifdef SIMULATIONBASE
  m_cutflowSvc("",name),
#else
  m_cutflowSvc("CutFlowSvc/CutFlowSvc",name),
#endif
  m_eventCount(0),
  m_badEvents(0)
{
  declareProperty("CalcStats",m_calcStats);

  ATH_MSG_DEBUG ("In DecisionSvc::DecisionSvc ");

  assert( pSvcLocator );

  // Allocate  m_SacceptAlgNames vector
  m_SacceptAlgNames = new std::vector<std::string>;
  // Allocate  m_SrequireAlgNames vector
  m_SrequireAlgNames = new std::vector<std::string>;
  // Allocate  m_SvetoAlgNames vector
  m_SvetoAlgNames = new std::vector<std::string>;
  // Allocate m_streamNames
  m_streamNames = new std::vector<std::string>;
}

DecisionSvc::~DecisionSvc()
{
  delete m_SacceptAlgNames;
  delete m_SrequireAlgNames;
  delete m_SvetoAlgNames;
  delete m_streamNames;
}

StatusCode
DecisionSvc::initialize()
{
  // Decode the accept, required and veto Algorithms.
  // The logic is the following:
  //  a. The event is accepted if all lists are empty.
  //  b. The event is provisionally accepted if any Algorithm in the
  //     accept list
  //     has been executed and has indicated that its filter is passed. This
  //     provisional acceptance can be overridden by the other lists.
  //  c. The event is rejected unless all Algorithms in the required list have
  //     been executed and have indicated that their filter passed.
  //  d. The event is rejected if any Algorithm in the veto list has been
  //     executed and has indicated that its filter has passed.

  // Set to be listener for BeginRun
  // Declares top filters to ICutFlowSvc if (and only if) needed (David Cote, Sep 2010)

  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  if (!incSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Unable to get the IncidentSvc");
    return StatusCode::FAILURE;
  }
  incSvc->addListener(this, "EndEvent", 100);

  return StatusCode::SUCCESS;
}


StatusCode
DecisionSvc::finalize()
{
  ATH_MSG_INFO("-- OUTPUT STREAM EVENT OVERLAP SUMMARY --");

  msg(MSG::INFO) << "List of registered " << m_streamNames->size()
		 << " Streams: ";
  for(std::vector<std::string>::const_iterator it = m_streamNames->begin();
                                               it != m_streamNames->end(); ++it){
    msg(MSG::INFO)<< (*it) << " ";
  }
  msg(MSG::INFO)<<endreq;
  if (m_calcStats) {
    // Print unstreamed event count
    ATH_MSG_INFO(" Unstreamed events " << m_badEvents);
    // Print overlap levels
    ATH_MSG_INFO(" Overlap multiplicities seen:");
    for (std::map<unsigned int,unsigned int>::const_iterator olit = m_overlapLevelCounts.begin();
                                                             olit != m_overlapLevelCounts.end(); olit++) {
      ATH_MSG_INFO("   ** Events went into " << olit->first << " streams " << olit->second << " times");
    }
    // Print overlap summary
    ATH_MSG_INFO (" ** Printing Stream Overlap summary");
    unsigned int sid=0;
    for (std::map<std::string, std::vector<unsigned int> >::const_iterator xit = m_stream2Counts.begin();
                             xit != m_stream2Counts.end(); ++xit) {
      ATH_MSG_INFO(" Stream " << xit->first << " " << xit->second.at(sid) << " events");
      if (xit->second.at(sid)>0) {
        for (unsigned int sid2 = 0; sid2 < xit->second.size(); ++sid2) {
          if (sid != sid2 && xit->second.at(sid2)>0) {
            msg() << MSG::INFO << " --> overlap fraction " << m_streamNames->at(sid2)
                  << " " << std::fixed << std::setprecision(2) << float(xit->second.at(sid2))/float(xit->second.at(sid)) << endreq;
          }
        }
      }
      sid++;
    }
  }

  ATH_MSG_INFO ("Finalized successfully.");

  return StatusCode::SUCCESS;

}

StatusCode
DecisionSvc::interpretAlgMap()
{
  StatusCode status = StatusCode::SUCCESS;

  for(std::map<std::string, std::vector<std::string> >::iterator ait =  m_stream_accept.begin();
                                                                 ait != m_stream_accept.end(); ++ait){

    status = decodeAlgorithms(ait->first, ait->second, &m_stream_acceptAlgs);
    if( !status.isSuccess() )
      {
 	ATH_MSG_FATAL ("Unable to decodeAcceptAlgs.");
 	return status;
      }
  }

  for(std::map<std::string, std::vector<std::string> >::iterator ait =  m_stream_require.begin();
                                                                 ait != m_stream_require.end(); ++ait){

    status = decodeAlgorithms(ait->first, ait->second, &m_stream_requireAlgs);
    if( !status.isSuccess() )
      {
	ATH_MSG_FATAL ("Unable to decodeRequireAlgs.");
	return status;
      }
  }

  for(std::map<std::string, std::vector<std::string> >::iterator ait =  m_stream_veto.begin();
                                                                 ait != m_stream_veto.end(); ++ait){

    status = decodeAlgorithms(ait->first, ait->second, &m_stream_vetoAlgs );
    if( !status.isSuccess() )
      {
	ATH_MSG_FATAL ("Unable to decodeVetoAlgs.");
	return status;
      }
  }

  // Fill up m_streamNames vector
  std::set<std::string> uniStreams;
  m_streamNames->clear();
  // first take list of streams from accept streams
  for(std::map<std::string, std::vector<std::string> >::iterator iter = m_stream_accept.begin();
                                                                 iter != m_stream_accept.end(); ++iter){
    uniStreams.insert(iter->first);
  }
  // then require
  for(std::map<std::string, std::vector<std::string> >::iterator iter = m_stream_require.begin();
                                                                 iter != m_stream_require.end(); ++iter){
    uniStreams.insert(iter->first);
  }
  // then veto
  for(std::map<std::string, std::vector<std::string> >::iterator iter = m_stream_veto.begin();
                                                                 iter != m_stream_veto.end(); ++iter){
    uniStreams.insert(iter->first);
  }
  std::copy(uniStreams.begin(),uniStreams.end(),std::back_inserter(*m_streamNames));

  return status;
}


///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////


StatusCode
DecisionSvc::addStream(const std::string& stream)
{
  StatusCode status = StatusCode::SUCCESS;
  if(m_frozen != true){
    // check if this stream already exist
    std::map<std::string, std::vector<std::string> >::iterator it = m_stream_accept.find(stream);
    if(it != m_stream_accept.end()){
      // ok, it exists, then do nothing
      msg(MSG::WARNING) << "Stream name : " << stream << " already been registered!" << endreq;
      status = StatusCode::FAILURE;
    }else{
      //if the stream doesn't exist yet, then insert it to the accept list with an empty vector of Algs
      std::vector<std::string> tmpvec;
      tmpvec.clear();
      msg(MSG::INFO) << "Inserting stream: "<< stream  << " with no Algs" << endreq;
      m_stream_accept.insert(std::make_pair(stream, tmpvec));
      status = StatusCode::SUCCESS;
    }
  }
  return status;
}

StatusCode
DecisionSvc::fillMap(std::map<std::string, std::vector<std::string> >& streamsModeMap,
                     const std::string& name, const std::string& stream)
{
  // Check if Alg exists.
  IAlgManager* theAlgMgr;
  StatusCode result = serviceLocator( )->getService("ApplicationMgr",
                                                    IAlgManager::interfaceID(),
                                                    *pp_cast<IInterface>(&theAlgMgr));

  if ( result.isSuccess( ) ) {

    IAlgorithm* theIAlg;
    result = theAlgMgr->getAlgorithm( name, theIAlg );
    if ( result.isSuccess( ) ) {

      if (m_frozen != true) {

	// check if this stream already exist
	std::map<std::string, std::vector<std::string> >::iterator it = streamsModeMap.find(stream);
	if(it != streamsModeMap.end()){
	  // ok, it exists, then check if the algname was already been inserted

	  // save the list
	  std::vector<std::string> tmpvec = it->second;

	  bool algexist = false;
          // Check if alg already registered for this stream
	  for(std::vector<std::string>::iterator vit = (it->second).begin();
                                                 vit != (it->second).end(); ++vit) {
	    if((*vit) == name){
	      algexist = true;
	      // it seems the alg was already inserted, warn the user
	      msg(MSG::ERROR) << "Alg name : " << name
                              << " of stream " << stream
                              << " has already been registered!" << endreq;
	      return StatusCode::FAILURE;
	    }
	  }


	  // So, if the stream exist but the alg has not been registered
	  // update its content std::vector with a alg
	  if(algexist == false){
	    tmpvec.push_back(name);
	    streamsModeMap.erase(stream);
	    streamsModeMap.insert(std::make_pair(stream, tmpvec));
	  }

	  //if the stream doesn't exist yet, then insert it
	} else {
	  std::vector<std::string> tmpvec;
	  tmpvec.push_back(name);
	  streamsModeMap.insert(std::make_pair(stream, tmpvec));
	}
      }else{

	msg(MSG::WARNING) << "State of DecisionSvc is " << m_frozen << " ( " << false << " - open, " << true << " - frozen )" << endreq;
	msg(MSG::WARNING) << "Adding Algs not allowed anymore!"<< endreq;
      }
    } else{
      msg(MSG::WARNING) << "Algorithm " << name << " doesn't exist - ignored" << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
DecisionSvc::addAcceptAlg(const std::string& name, const std::string& stream)
{
  return fillMap(m_stream_accept,name,stream);
}

StatusCode
DecisionSvc::addRequireAlg(const std::string& name,
                           const std::string& stream)
{
  return fillMap(m_stream_require,name,stream);
}

StatusCode
DecisionSvc::addVetoAlg(const std::string& name,
                        const std::string& stream)
{
  return fillMap(m_stream_veto,name,stream);
}


// Given a list of names, it checks for the Algorithm objects in the memory from AlgMgr,
// and copy the pointers of Algorithm objects to the streamname -- Algorithm map
StatusCode
DecisionSvc::decodeAlgorithms(const std::string& stream,
			      const std::vector<std::string>& theNames,
			      std::map<std::string, std::vector<Algorithm*> > * theAlgMap )
{
  msg(MSG::DEBUG) << "In DecisionSvc::decodeAlgorithms " << endreq ;
  // Reset the list of Algorithms
  //  theAlgs->clear( );
  std::vector<Algorithm*> tempAlgs;

  IAlgManager* theAlgMgr;
  StatusCode result = serviceLocator( )->getService("ApplicationMgr",
                                                     IAlgManager::interfaceID(),
                                                     *pp_cast<IInterface>(&theAlgMgr));

  if ( result.isSuccess( ) ) {

    // Build the list of Algorithms from the names list
    const std::vector<std::string>& nameList = theNames;
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
	theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
	if (0==theAlgorithm) {
	  result = StatusCode::FAILURE;
	}
      }
      if ( result.isSuccess( ) ) {

        // Check that the specified algorithm doesn't already exist in the list
        std::vector<Algorithm*>::iterator ita;
        std::vector<Algorithm*>::iterator itaend = tempAlgs.end( );
        for (ita = tempAlgs.begin(); ita != itaend; ita++) {
          Algorithm* existAlgorithm = (*ita);
          if ( theAlgorithm == existAlgorithm ) {
            result = StatusCode::FAILURE;
            break;
          }
        }
        if ( result.isSuccess( ) ) {
	  //          theAlgs->push_back( theAlgorithm );
          tempAlgs.push_back( theAlgorithm );
        }
      } else {
        ATH_MSG_FATAL (theName << " doesn't exist -- trying to get pointer to a non-existing Algorithm! Stop the job!");
	result = StatusCode::FAILURE;
	return result;
      }
    }

    theAlgMap->insert(std::make_pair(stream, tempAlgs));

    result = StatusCode::SUCCESS;
  } else {
    ATH_MSG_FATAL ("Can't locate ApplicationMgr!!!");
  }

  return result;
}

const std::vector<std::string>* DecisionSvc::getStreams() const
{
  return m_streamNames;
}

const std::vector<std::string> *
DecisionSvc::getAcceptAlgs(const std::string& stream) const
{
  m_SacceptAlgNames->clear();

  if(m_frozen == true){
    //Loop over all streams of accept type and find the one of interest
    std::map<std::string, std::vector<Algorithm*> >::const_iterator itAlgs = m_stream_acceptAlgs.find(stream);
    if(itAlgs != m_stream_acceptAlgs.end()){
      // get a handle of the streams' *existing* algos vector
      std::vector<Algorithm*>  vecAlgs = itAlgs->second;
      if ( ! vecAlgs.empty( ) ) {
	std::vector<Algorithm*>::iterator it;
	std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
	for (it = vecAlgs.begin(); it != itend; it++) {
	  m_SacceptAlgNames->push_back((*it)->name());
	}
      }else{
	ATH_MSG_WARNING ("No proper AcceptAlgs got registered to DecisionSvc");
      }
    }
  }else{
    ATH_MSG_WARNING ("DecisionSvc accessors only available after start()");
  }

  return m_SacceptAlgNames;

}

const std::vector<std::string> *
DecisionSvc::getRequireAlgs(const std::string& stream) const
{
  m_SrequireAlgNames->clear();


  if(m_frozen == true){
    //Loop over all streams of accept type and find the one of interest
    std::map<std::string, std::vector<Algorithm*> >::const_iterator itAlgs = m_stream_requireAlgs.find(stream);
    if(itAlgs != m_stream_requireAlgs.end()){
      // get a handle of the streams' *existing* algos vector
      std::vector<Algorithm*>  vecAlgs = itAlgs->second;
      if ( ! vecAlgs.empty( ) ) {
	std::vector<Algorithm*>::iterator it;
	std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
	for (it = vecAlgs.begin(); it != itend; it++) {
	  m_SrequireAlgNames->push_back((*it)->name());
	}
      }else{
	ATH_MSG_WARNING ("No proper RequireAlgs got registered to DecisionSvc");
      }
    }
  }else{
    ATH_MSG_WARNING ("DecisionSvc accessors only available after start()");
  }

  return m_SrequireAlgNames;
}

const std::vector<std::string> *
DecisionSvc::getVetoAlgs(const std::string& stream) const
{
  m_SvetoAlgNames->clear();

  if(m_frozen == true){
    //Loop over all streams of accept type and find the one of interest
    std::map<std::string, std::vector<Algorithm*> >::const_iterator itAlgs = m_stream_vetoAlgs.find(stream);
    if(itAlgs != m_stream_vetoAlgs.end()){
      // get a handle of the streams' *existing* algos vector
      std::vector<Algorithm*>  vecAlgs = itAlgs->second;
      if ( ! vecAlgs.empty( ) ) {
	std::vector<Algorithm*>::iterator it;
	std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
	for (it = vecAlgs.begin(); it != itend; it++) {
	  m_SvetoAlgNames->push_back((*it)->name());
	}
      }else{
	ATH_MSG_WARNING ("No proper VetoAlgs got registered to DecisionSvc");
      }
    }
  }else{
    ATH_MSG_WARNING ("DecisionSvc accessors only available after start()");
  }

  return m_SvetoAlgNames;
}



///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////


bool
DecisionSvc::isEventAccepted( const std::string& stream ) const
{

  ATH_MSG_DEBUG("In DecisionSvc::isEventAccepted( " << stream << " )");

  // By construction a stream is accepted
  bool result = true;

  bool found_accept = false;
  bool found_require = false;
  bool found_veto = false;

   //Loop over all streams of accept type and find the one of interest
  std::map<std::string, std::vector<Algorithm*> >::const_iterator itAlgs = m_stream_acceptAlgs.find(stream);
  if(itAlgs != m_stream_acceptAlgs.end()){
    found_accept = true;
    // get a handle of the streams' algos vector
    std::vector<Algorithm*> vecAlgs = itAlgs->second;
    // Loop over all Algorithms in the list to see
    // whether any have been executed and have their filter
    // passed flag set. Any match causes the event to be
    // provisionally accepted.
    if ( ! vecAlgs.empty( ) ) {
      result = false;
      std::vector<Algorithm*>::iterator it;
      std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
      for (it = vecAlgs.begin(); it != itend; it++) {
	Algorithm* theAlgorithm = (*it);
	// little cross check__________________
	msg(MSG::DEBUG) << "Algorithm " << theAlgorithm->name()
			<< "got Executed? ";
	if(theAlgorithm->isExecuted() == true) {
	  msg(MSG::DEBUG) << " Yes!" << endreq;
	} else {
	  msg(MSG::DEBUG) << " No!" << endreq;
	}
	// ______________________
	if ( theAlgorithm->isExecuted( ) && theAlgorithm->filterPassed( ) ) {
	  result = true;
	  break;
	}
      }
    }
  }

   //Loop over all streams of require type and find the one of interest
  itAlgs = m_stream_requireAlgs.find(stream);
  if(itAlgs != m_stream_requireAlgs.end()){
    found_require = true;
    // get a handle of the streams' algos vector
    std::vector<Algorithm*> vecAlgs = itAlgs->second;
   // Loop over all Algorithms in the required list to see
   // whether all have been executed and have their filter
   // passed flag set. Any mismatch causes the event to be
   // rejected.
    if ( result && ! vecAlgs.empty( ) ) {
     std::vector<Algorithm*>::iterator it;
     std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
     for (it = vecAlgs.begin(); it != itend; it++) {
       Algorithm* theAlgorithm = (*it);
	// little cross check__________________
       msg(MSG::DEBUG) << "Algorithm " << theAlgorithm->name()
		       << "got Executed? ";
       if(theAlgorithm->isExecuted() == true) {
	 msg(MSG::DEBUG) << " Yes!" << endreq;
       } else {
	 msg(MSG::DEBUG) << " No!" << endreq;
       }
	// ______________________
       if ( ! theAlgorithm->isExecuted( ) || ! theAlgorithm->filterPassed( ) ) {
         result = false;
         break;
       }
     }
    }
  }

   //Loop over all streams of veto type and find the one of interest
  itAlgs = m_stream_vetoAlgs.find(stream);
  if(itAlgs != m_stream_vetoAlgs.end()){
    found_veto = true;
    // get a handle of the streams' algos vector
    std::vector<Algorithm*>  vecAlgs = itAlgs->second;
    // Loop over all Algorithms in the veto list to see
    // whether any have been executed and have their filter
    // passed flag set. Any match causes the event to be
    // rejected.
    if ( result && ! vecAlgs.empty( ) ) {
      std::vector<Algorithm*>::iterator it;
      std::vector<Algorithm*>::iterator itend = vecAlgs.end( );
      for (it = vecAlgs.begin(); it != itend; it++) {
	Algorithm* theAlgorithm = (*it);
	// little cross check__________________
	msg(MSG::DEBUG) << "Algorithm " << theAlgorithm->name()
			<< "got Executed? ";
	if(theAlgorithm->isExecuted() == true) {
	  msg(MSG::DEBUG) << " Yes!" << endreq;
	} else {
	  msg(MSG::DEBUG) << " No!" << endreq;
	}
	// ______________________
	if ( theAlgorithm->isExecuted( ) && theAlgorithm->filterPassed( ) ) {
         result = false;
         break;
	}
      }
    }
  }

  if(found_accept == false && found_require == false && found_veto == false){
    ATH_MSG_DEBUG("Stream: " << stream << " not found registered in DecisionSvc -- accepting event by default ");
  }


  return result;
}

//__________________________________________________________________________
void
DecisionSvc::handle(const Incident& inc)
{
  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  std::string fileName;
  if (fileInc == 0) {
    fileName = "Undefined ";
  } else {
    fileName = fileInc->fileName();
  }

  ATH_MSG_DEBUG("handle() " << inc.type() << " for file: " << fileName);

  // Statistics
  if (inc.type()=="EndEvent" && m_calcStats) {
    m_eventCount++;
    unsigned int streamCount = 0;
    unsigned int acceptCount = 0;
    for (std::vector<std::string>::const_iterator it  = m_streamNames->begin();
                                                  it != m_streamNames->end(); ++it) {
      if (this->isEventAccepted(*it)) {
        // set streamed flag
        acceptCount++;
        // increment stream overlap counters
        std::map<std::string, std::vector<unsigned int> >::iterator m2it = m_stream2Counts.find(*it);
        if (m2it == m_stream2Counts.end()) {
          ATH_MSG_ERROR("Stream counting map not initialized properly");
        }
        else {
          for (std::vector<std::string>::const_iterator it2  = m_streamNames->begin();
                                                  it2 != m_streamNames->end(); ++it2) {
            if (this->isEventAccepted(*it2)) {
              (m_stream2Counts[*it2])[streamCount]++;
            }
          }
        }
      }
      streamCount++;
    }
    // Count events which went to no stream
    if (acceptCount==0) m_badEvents++;
    // Check overlap level
    std::map<unsigned int,unsigned int>::iterator olit = m_overlapLevelCounts.find(acceptCount);
    if (olit != m_overlapLevelCounts.end()) (olit->second)++;
    else m_overlapLevelCounts.insert(std::make_pair(acceptCount,1));
  }

}


StatusCode DecisionSvc::start()
{
  ATH_MSG_DEBUG("in start");
  CHECK( this->interpretAlgMap() );
  m_frozen = true;

  // initialize statistics vector
  if (m_calcStats) {
    for(std::vector<std::string>::const_iterator ait  = m_streamNames->begin();
                                                 ait != m_streamNames->end(); ++ait) {
      std::vector<unsigned int> temp;
      std::fill_n(back_inserter(temp), m_streamNames->size(), (unsigned int)0);
      m_stream2Counts.insert(std::make_pair(*ait,temp));
    }
  }

  //Retrieve CutFlowSvc if (and only if) needed
  if( m_SacceptAlgNames->size()>0 or m_SrequireAlgNames->size()>0 or m_SvetoAlgNames->size()>0 ){
    if (!m_cutflowSvc.empty())
      {
        if (m_cutflowSvc.retrieve().isFailure())
          {
            ATH_MSG_ERROR("Cannot get ICutFlowSvc interface.");
            return StatusCode::RECOVERABLE;
          }
      }
  }

  //Now that everything is said and done, match filters with stream and logic in CutFlowSvc
  DeclareToCutFlowSvc();

  return StatusCode::SUCCESS;
}

void DecisionSvc::DeclareToCutFlowSvc()
{
  for(std::vector<std::string>::const_iterator ait  = m_streamNames->begin();
                                            ait != m_streamNames->end(); ++ait) {
    std::string streamName=*ait;
    std::vector<std::string> accFilt;
    std::vector<std::string> reqFilt;
    std::vector<std::string> vetFilt;
    std::vector< std::vector<std::string>* > totFilt;
    if (m_stream_accept.find(streamName) != m_stream_accept.end())  {
      accFilt = m_stream_accept[streamName];
      totFilt.push_back(&accFilt);
    }
    if (m_stream_require.find(streamName)!= m_stream_require.end()) {
      reqFilt = m_stream_require[streamName];
      totFilt.push_back(&reqFilt);
    }
    if (m_stream_veto.find(streamName)   != m_stream_veto.end())    {
      vetFilt = m_stream_veto[streamName];
      totFilt.push_back(&vetFilt);
    }

    std::string accstring(""), reqstring(""), vetstring("");
    for (std::vector<std::string>::const_iterator it = accFilt.begin(); it != accFilt.end(); ++it) {
      if(accstring.size()>0) accstring += "||";
      else accstring += "(";
      accstring+=*it;
    }
    for (std::vector<std::string>::const_iterator it = reqFilt.begin(); it != reqFilt.end(); ++it) {
      if(reqstring.size()>0) reqstring += "&&";
      else reqstring += "(";
      reqstring+=*it;
    }
    for (std::vector<std::string>::const_iterator it = vetFilt.begin(); it != vetFilt.end(); ++it) {
      if(vetstring.size()>0) vetstring += "||";
      else vetstring += "(";
      vetstring+=*it;
    }
    std::string logicalKey("");
    if(accstring.size()>0) {
      accstring += ")";
      logicalKey += accstring;
    }
    if(reqstring.size()>0) {
      reqstring += ")";
      if (logicalKey.size()>0) logicalKey += "&&";
      logicalKey += reqstring;
    }
    if(vetstring.size()>0) {
      vetstring += ")";
      if (logicalKey.size()>0) logicalKey += "&&";
      logicalKey = logicalKey + "!" + vetstring;
    }
    if (logicalKey.size()==0) logicalKey="PassThru";
    ATH_MSG_DEBUG("stream " << streamName << " uses logic " << logicalKey);
    for(std::vector< std::vector<std::string>* >::const_iterator vec  = totFilt.begin();
                                                                 vec != totFilt.end(); ++vec) {
      ATH_MSG_DEBUG("Declaring logic " << logicalKey << " for " << streamName);
      for (std::vector<std::string>::const_iterator filter  = (*vec)->begin();
                                                    filter != (*vec)->end(); ++filter) {
        if(!m_cutflowSvc.empty()) {m_cutflowSvc->registerTopFilter( (*filter), logicalKey, 2, streamName ); }
      }
    }
  }

  return;
}


StatusCode
DecisionSvc::queryInterface( const InterfaceID& riid, void** ppvi )
{
  // valid placeholder?
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }  // RETURN
  if ( IDecisionSvc::interfaceID() == riid )
    {
      *ppvi = static_cast<IDecisionSvc*>(this);
      addRef(); // NB! : inrement the reference count!
      return StatusCode::SUCCESS;                     // RETURN
    }
  // Interface is not directly availible: try out a base class
  return Service::queryInterface( riid, ppvi );
}
