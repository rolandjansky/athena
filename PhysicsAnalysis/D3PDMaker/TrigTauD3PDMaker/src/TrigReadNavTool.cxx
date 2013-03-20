/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigReadNavTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrigTauD3PDMaker/TrigReadNavTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//================ Constructor =================================================

TrigTauTool::TrigReadNavTool::TrigReadNavTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AlgTool(t,n,p),
  m_log(msgSvc(),n),
  m_incSvc ("IncidentSvc", n),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareInterface<TrigReadNavTool>(this);

  this->declareProperty ("IncidentService", m_incSvc,
                         "Gaudi incident service.");
  this->declareProperty ("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
  this->declareProperty ("ChainPattern", m_chainPattern = ".*",
                         "Pattern to use for matching chains.");
  this->declareProperty ("OnlyPassed", m_onlyPassed = true,
                         "If true, only select objects that passed trigger.");
}

//================ Destructor =================================================
//in header
//TrigTauTool::TrigReadNavTool::~TrigReadNavTool()
//{}


//================ Initialisation =================================================

StatusCode TrigTauTool::TrigReadNavTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  m_log.setLevel(outputLevel());
  if (sc.isFailure()) return sc;

  m_log << MSG::INFO << "initialize() successful in " << name() << endreq;

  CHECK( m_trigDec.retrieve() );
  CHECK( m_incSvc.retrieve() );

  m_incSvc->addListener (this, "EndEvent");

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode TrigTauTool::TrigReadNavTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//=================== Functions ==================================================

void TrigTauTool::TrigReadNavTool::handle
(const Incident& inc)
{
  if (inc.type() == "EndEvent") {
    ;
  }
}

std::vector< std::string > TrigTauTool::TrigReadNavTool::getChainNames(const std::string& chainPattern)
{
  const Trig::ChainGroup* chainGroup = m_trigDec->getChainGroup(chainPattern);
  return chainGroup->getListOfTriggers();
} 
std::map< std::string, std::vector<unsigned int> > TrigTauTool::TrigReadNavTool::mapChainsToRoIs(const std::vector<std::string>& chainNames, std::string label)
{
  std::map< std::string, std::vector<unsigned int> > chainstorois;
  std::vector<std::string> chainNamesc = chainNames;
  BOOST_FOREACH( std::string& name, chainNamesc){
    Trig::FeatureContainer f = m_trigDec->features(name);
    std::vector<Trig::Feature<TrigRoiDescriptor> > roifs = f.get<TrigRoiDescriptor>(label);
    unsigned int roiword=0;
    std::vector<unsigned int> roiwords;
    BOOST_FOREACH( Trig::Feature<TrigRoiDescriptor>& roif, roifs ){
      const TrigRoiDescriptor* roiobj = roif.cptr();
      roiword = roiobj->roiWord();
      roiwords.push_back(roiword);
    }
    chainstorois.insert( std::pair< std::string, std::vector<unsigned int> >(name, roiwords) );
  }
  return chainstorois;
}
