/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteerMonitor/TrigTEMoni.h"

#include "TrigConfL1Data/ThresholdConfig.h"

#include "TrigMonitorBase/TrigLBNHist.h"

#include "TrigInterfaces/AlgoConfig.h"

#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/Lvl1ItemsAndRoIs.h"
#include "TrigSteering/Sequence.h"
#include "TrigSteering/Signature.h"
#include "TrigNavigation/Navigation.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"

#include <EventInfo/EventID.h>
#include <vector>
#include <algorithm>
#include <TH1I.h>
#include <TH2I.h>

TrigTEMoni::TrigTEMoni(const std::string & type, const std::string & name,
		       const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),
     m_lvl1ConfigSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
     m_parentAlg(0),
     m_numberOfTEsHist(0),
     m_numberOfallTEsHist(0),

     m_numberOflvl1TEsHist(0),
     m_numberOfalllvl1TEsHist(0),

     m_numberOfTEsHist2d(0),
     m_numberOfallTEsHist2d(0),

     m_numberOflvl1TEsHist2d(0),
     m_numberOfalllvl1TEsHist2d(0),

     //     m_reserveLumiHistos(0),
     //     m_bookedLumiHistos(0),
     //     m_histoPathexpert(""),
     m_trigLvl("")

  
{
  declareInterface<IMonitorToolBase>(this);
  //  declareProperty("HistoPath", m_histoPathexpert = "/EXPERT/TrigSteering");
  //  declareProperty("ReserveLumiHistos", m_reserveLumiHistos = 1);
  //  declareProperty("useLBHistos",m_useLBHistos=true);
}

StatusCode TrigTEMoni::initialize()
{
  ATH_CHECK(TrigMonitorToolBase::initialize());

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  
  if ( !m_parentAlg ) {
    ATH_MSG_ERROR(" Unable to cast the parent algorithm to HLT::TrigSteer !");
    return StatusCode::FAILURE;
  }

  m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;
  
  // Get LVL1 Config Svc handle:
  
  if (m_lvl1ConfigSvc.empty()) {
    ATH_MSG_FATAL("no LVL1ConfigSvc set in the jobOptions-> abort");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_lvl1ConfigSvc.retrieve());

  //need LVL1 Config Svc for LVL1 thresholds
  /*sc = service( "TrigConf::LVL1ConfigSvc", m_lvl1ConfigSvc);
    if(sc.isFailure()) {
    if(m_logLvl <= MSG::FATAL) msg() << MSG::FATAL << " Unable to get pointer to Lvl1ConfigService " << endreq;
    return sc;
    }
  */
  
  return StatusCode::SUCCESS;
}


StatusCode TrigTEMoni::bookHists()
{
  ATH_CHECK(bookHistograms( false, false, true ));
  return StatusCode::SUCCESS;
}

StatusCode TrigTEMoni::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{
  std::string tmpstring;
  TString htit;
  TString hname;  
  TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );

  //reset private members 

  m_numberOfTEsHist=0;
  m_numberOfallTEsHist=0;  
  m_numberOflvl1TEsHist=0;
  m_numberOfalllvl1TEsHist=0;
  m_numberOfTEsHist2d=0;
  m_numberOfallTEsHist2d=0;
  m_numberOflvl1TEsHist2d=0;
  m_numberOfalllvl1TEsHist2d=0;
  m_configuredTETypes.clear();
  m_configuredlvl1TETypes.clear();
  m_labels.clear();
  m_lvl1labels.clear();
  
  /* 
     test new stuff here: get Lvl1 thresholds 
  */
  const TrigConf::ThresholdConfig* thresholdConfig = m_lvl1ConfigSvc->thresholdConfig();
  //std::vector<TrigConf::TriggerThreshold*>* thresholdVector=new std::vector<TrigConf::TriggerThreshold *>;

  const std::vector<TrigConf::TriggerThreshold*>& thresholdVector = thresholdConfig->getThresholdVector();

//   std::vector<TrigConf::TriggerThreshold*> thresholdVector;
//   const std::vector<TrigConf::TriggerThreshold*>* clusterThresholdConfig = thresholdConfig->getClusterThresholdPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* jetThresholdConfig = thresholdConfig->getJetThresholdPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* fjetThresholdConfig = thresholdConfig->getFJetThresholdPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* totEtConfig = thresholdConfig->getTotEtPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* jetEtConfig = thresholdConfig->getJetEtPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* missEtConfig = thresholdConfig->getMissEtPointer();
//   const std::vector<TrigConf::TriggerThreshold*>* muonThresholdConfig = thresholdConfig->getMuonThresholdPointer();
  
//   thresholdVector.insert( thresholdVector.end(),clusterThresholdConfig->begin(),clusterThresholdConfig->end());
//   thresholdVector.insert( thresholdVector.end(),jetThresholdConfig->begin(),jetThresholdConfig->end());
//   thresholdVector.insert( thresholdVector.end(),fjetThresholdConfig->begin(),fjetThresholdConfig->end());
//   thresholdVector.insert( thresholdVector.end(),totEtConfig->begin(),totEtConfig->end());
//   thresholdVector.insert( thresholdVector.end(),jetEtConfig->begin(),jetEtConfig->end());
//   thresholdVector.insert( thresholdVector.end(),missEtConfig->begin(),missEtConfig->end());
//   thresholdVector.insert( thresholdVector.end(),muonThresholdConfig->begin(),muonThresholdConfig->end());
  
  //std::vector< unsigned int > m_configuredlvl1TETypes;

  // MM, 16.8.11: first stick the TE type IDs into a map keyed by TE name (for sorting) 
  // then into m_configuredlvl1TETypes, m=lvl1labels

  std::map<std::string, unsigned int> foundLvl1TETypes;

  for (std::vector<TrigConf::TriggerThreshold*>::const_iterator lvl1TT = thresholdVector.begin();
       lvl1TT != thresholdVector.end(); ++lvl1TT) {

    unsigned int teId = 0;
    bool activeHLT = TrigConf::HLTTriggerElement::getId( (*lvl1TT)->name().c_str(), teId);
    if (activeHLT) {

      foundLvl1TETypes[(*lvl1TT)->name()] = teId;
      
    } else {
      //if(m_logLvl <= MSG::DEBUG) msg() << MSG::DEBUG << " => NO" << endreq;
    }
    int tn = (*lvl1TT)->thresholdNumber()+1;
    if (tn < 0) {
      msg() << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
	       << ", go on w/o this threshold!" <<endreq;
    }
  }
      //now build m_configuredlvl1TETypes, m_lvl1labels

  unsigned int bin_number_lvl1=0;
  for(std::map<std::string, unsigned int>::const_iterator fit = foundLvl1TETypes.begin();
      fit != foundLvl1TETypes.end(); fit++) {
    bin_number_lvl1++;
    m_configuredlvl1TETypes.push_back(fit->second);
    m_lvl1labels[fit->second]=bin_number_lvl1;
  }
  //ende test new stuff  - it works

  /*
    get input and output HLT Triggerelement types -> store types in m_configuredTETypes and
    corresponding bin number m_labels
  */
  std::vector<const HLT::SteeringChain*> configuredChains = m_parentAlg->getConfiguredChains();
  unsigned int bin_number = 0;
  // MM, 31.7.11: first stick the TE type IDs into a map keyed by TE name (for sorting) 
  // then into m_configuredTETypes, m=labels

  std::map<std::string, unsigned int> foundTETypes;

  //loop over configured chains
  for ( const HLT::SteeringChain* chain : configuredChains ) {

     // loop over all signatures of this chain
     for ( const HLT::Signature* sign : chain->getSignatures() ) {

        if ( sign == 0 ) continue;

        // loop over all sequences of this signature
        for ( const HLT::Sequence* seq : sign->getSequences() ) {

           //add output TEs to map
           unsigned int outputTeType = seq->outputTEType();
           std::string tmpstring;
           TrigConf::HLTTriggerElement::getLabel( outputTeType, tmpstring );
           foundTETypes[tmpstring] = outputTeType;

           //loop over input teTypes of this sequence
           for ( unsigned int tetype : seq->inputTETypes() ) {
              TrigConf::HLTTriggerElement::getLabel( tetype, tmpstring );
              foundTETypes[tmpstring] = tetype;
           }

        } // seq loop
     } // signature loop
  } // chain loop
  
  bin_number = 0;
  for(std::map<std::string, unsigned int>::const_iterator fit = foundTETypes.begin();
      fit != foundTETypes.end(); fit++) {

    m_configuredTETypes.push_back( fit->second );

    if(fit->first != "") {
      bin_number++;
      m_labels[ fit->second ] = bin_number;
    } else {
      m_labels[ fit->second ] = 0; // into the underflow bin
      ATH_MSG_WARNING("TE with blank label found. TEType: " << fit->second);
    }
  }
  
  //cross check bin_number = number of TETypes ? 
  // This can happen now (17.8.11) if blank labels are found (as in the rerundb test)
  if(bin_number !=  m_configuredTETypes.size() ){
    ATH_MSG_WARNING(" size of triggername, m_configuredTETypes, bin_number "
                    <<m_configuredTETypes.size()<<" "<<bin_number<<" doesnt match ");
  }
  //lvl1 stuff
  if(m_trigLvl!="EF"){
    if(bin_number_lvl1 !=  m_configuredlvl1TETypes.size() ){
      ATH_MSG_WARNING(" size of triggername, m_configuredlvl1TETypes, bin_number_lvl1 "
                      <<m_configuredlvl1TETypes.size()<<" "<<bin_number_lvl1<<" doesnt match ");
    }
  }

  
  
  /*
    now book histos
  */

  
  /*
    run summary
  */
  
  /* 
     histo for number of TE 
  */
  /*
  std::string title = "TE abundance in "+ m_trigLvl;// + " Run Summary";
  std::string run_title = title + " Run Summary";
  std::string name  = "TEnumber_" + m_trigLvl;// + "_runsummary";
  std::string runname = name + "_runsummary";
  
  m_numberOfTEsHist_runsummary = new TH1I(run_name.c_str(),run_title.c_str(), bin_number,0.5, bin_number+0.5 );
  if (expertHistograms.regHist(m_numberOfTEsHist_runsummary).isFailure()){
    msg() << MSG::WARNING << "Can't book "
	     <<  m_numberOfTEsHist_runsummary->GetName() << endreq;
  }
  if ( m_useLBHistos) {
    m_numberOfTEsHist = new TrigLBNHist<TH1I>(*m_numberOfTEsHist_runsummary);
    m_numberOfTEsHist->SetNameTitle(name.c_str(), title.c_str());
  }
  */

  std::string name  = "NumberOfActiveTEs";
  std::string title = "TE abundance in "+ m_trigLvl;
  m_numberOfTEsHist = new  TH1I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5) ;
  if ( expertHistograms.regHist(m_numberOfTEsHist).isFailure() ) 
    msg() << MSG::WARNING << "Can't book histogram " << name << endreq;

  /*
    histo for number of all TE
  */

  /*
  title    = "all TE abundance in "+ m_trigLvl;
  run_title = totle + " Run Summary";
  name  = "allTEnumber_" + m_trigLvl;
  run_name = name + "_runsummary";
  m_numberOfallTEsHist_runsummary =new TH1I(run_name.c_str(),run_title.c_str(), bin_number,0.5, bin_number+0.5 );
  if (expertHistograms.regHist(m_numberOfallTEsHist_runsummary).isFailure()){
    msg() << MSG::WARNING << "Can't book "
	     << m_numberOfallTEsHist_runsummary ->GetName() << endreq;
  }
  if ( m_useLBHistos ) {
    m_numberOfallTEsHist = new TrigLBNHist<TH1I>(*m_numberOfallTEsHist_runsummary);
    m_numberOfallTEsHist->SetNameTitle(name.c_str(), title.c_str());
  }
  */
  name  = "NumberOfAllTEs";
  title    = "All TE abundance in "+ m_trigLvl;
  m_numberOfallTEsHist =new TH1I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 ) ;  
  if ( expertHistograms.regHist( m_numberOfallTEsHist ).isFailure()) 
    msg() << MSG::WARNING << "Can't book histogram " << name << endreq;


  /*
    2dhisto for number of input TEs 
  */
  /*
    title  = "number of TE per event in "+ m_trigLvl;
    run_title = title + " Run Summary";
    name  = "TEnumberevent_" + m_trigLvl;
    run_name = name + "_runsummary";
    m_numberOfTEsHist2d_runsummary =new TH2I(run_name.c_str(),run_title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 );
    
    if (expertHistograms.regHist(m_numberOfTEsHist2d_runsummary).isFailure()){
    msg() << MSG::WARNING << "Can't book "
    << m_numberOfTEsHist2d_runsummary ->GetName() << endreq;
    }
    if ( m_useLBHistos ) {
    m_numberOfTEsHist2d = new TrigLBNHist<TH2I>(*m_numberOfTEsHist2d_runsummary);
    m_numberOfTEsHist2d->SetNameTitle(name.c_str(), title.c_str());
    
    
    }
  */

  name  = "NumberOfActiveTEsPerEvent";
  title  = "Number of TE per event in "+ m_trigLvl;
  m_numberOfTEsHist2d =new TH2I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 ) ;
  if ( expertHistograms.regHist( m_numberOfTEsHist2d ).isFailure()) 
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endreq;





  /*
    2dhisto for number of all input TEs 
  */
  /*
    title  = "number of all TE per event in "+ m_trigLvl;
    run_title = title + " Run Summary";
    name  = "allTEnumberevent_" + m_trigLvl;
    run_name = name + "_runsummary";
    m_numberOfallTEsHist2d_runsummary =new TH2I(run_name.c_str(),run_title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 );
    
    if (expertHistograms.regHist(m_numberOfallTEsHist2d_runsummary).isFailure()){
    msg() << MSG::WARNING << "Can't book "
    << m_numberOfallTEsHist2d_runsummary ->GetName() << endreq;
    }
    if ( m_useLBHistos ) {
    m_numberOfallTEsHist2d = new TrigLBNHist<TH2I>(m_numberOfallTEsHist2d_runsummary);
    m_numberOfallTEsHist2d->SetNameTitle(name.c_str(), title.c_str());
    }
  */

  name  = "NumberOfAllTEsPerEvent";
  title  = "Number of all TE per event in "+ m_trigLvl;
  m_numberOfallTEsHist2d = new TH2I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 );

  if ( expertHistograms.regHist( m_numberOfallTEsHist2d ).isFailure()) 
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endreq;




  
  if(m_trigLvl != "EF"){
    /*
      histo for number of input TEs from LVL1
    */
    name  = "NumberOfLvl1TEs";

    title  = "LVL1-TEs abundance in (L1 threshods counts)"+ m_trigLvl;

    if(bin_number_lvl1>0){
      m_numberOflvl1TEsHist =new TH1I(name.c_str(),title.c_str(), bin_number_lvl1,0.5, bin_number_lvl1+0.5 );
    }else{
      m_numberOflvl1TEsHist =new TH1I(name.c_str(),title.c_str(), 1,-0.5,0.5 );
    }

  if ( expertHistograms.regHist( m_numberOflvl1TEsHist ).isFailure()) 
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endreq;





    /*
      histo for number of all input TEs from LVL1
    */
  /* L1 TEs can not be deactivated 
    title  = "all LVL1-TE abundance in "+ m_trigLvl;

    name  = "alllvl1TEnumber_" + m_trigLvl;

    if(bin_number_lvl1>0){
      m_numberOfalllvl1TEsHist =new TrigLBNHist<TH1I>(TH1I(name.c_str(),title.c_str(), bin_number_lvl1,0.5, bin_number_lvl1+0.5 ));
    }else{
      m_numberOfalllvl1TEsHist =new TrigLBNHist<TH1I>(TH1I(name.c_str(),title.c_str(), 1,-0.5,0.5));
    }

    if ( expertHistograms.regHist( (ITrigLBNHist*)m_numberOfalllvl1TEsHist ).isFailure()) 
      msg() << MSG::WARNING << "Can't book TrigLBN histogram " << name << endreq;
  */




    /*
      2dhisto for number of input TEs from LVL1
    */
  name  = "NumberOfLvl1TEsPerEvent";
    title  = "LVL1-TEs abundance per event in (threshold multiplicities)"+ m_trigLvl;

    if(bin_number_lvl1>0){
      m_numberOflvl1TEsHist2d = new TH2I(name.c_str(), title.c_str(), bin_number_lvl1,0.5, bin_number_lvl1+0.5, 20,-0.5,19.5  );
    }else{
      m_numberOflvl1TEsHist2d = new TH2I(name.c_str(), title.c_str(), 1,-0.5, 0.5, 20,-0.5,19.5  );
    }
    if ( expertHistograms.regHist( m_numberOflvl1TEsHist2d ).isFailure()) 
      msg() << MSG::WARNING << "Can't book Trig histogram " << name << endreq;




    /*
      2dhisto for number of all input TEs from LVL1
    */
    /*
    title  = "number of all LVL1-TE per event in "+ m_trigLvl;
    name  = "alllvl1TEnumberevent_" + m_trigLvl;
    if(bin_number_lvl1>0){
      m_numberOfalllvl1TEsHist2d =new TrigLBNHist<TH2I>(TH2I(name.c_str(), title.c_str(), bin_number_lvl1,0.5, bin_number_lvl1+0.5, 20,-0.5,19.5  ));
    }else{
      m_numberOfalllvl1TEsHist2d =new TrigLBNHist<TH2I>(TH2I(name.c_str(), title.c_str(), 1,-0.5, 0.5, 20,-0.5,19.5  ));
    }

    if ( expertHistograms.regHist( (ITrigLBNHist*)m_numberOfalllvl1TEsHist2d ).isFailure()) 
      msg() << MSG::WARNING << "Can't book TrigLBN histogram " << name << endreq; 
    */
  }//"EF"

  
  //naming the histogram bins according to TE label
  int tmpbin=0;
  for ( std::vector< unsigned int >::iterator type = m_configuredTETypes.begin();
	type != m_configuredTETypes.end(); ++type) {
    TrigConf::HLTTriggerElement::getLabel ((*type),tmpstring);
    tmpbin++;
    
    m_numberOfTEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    m_numberOfallTEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    
    m_numberOfTEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    m_numberOfallTEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str()); 
    
  }

  if(m_trigLvl!="EF"){// LVL1 TES only for L2 and HLT
    tmpbin=0;
    for ( std::vector< unsigned int >::iterator type = m_configuredlvl1TETypes.begin();
	  type != m_configuredlvl1TETypes.end(); ++type) {
      TrigConf::HLTTriggerElement::getLabel ((*type),tmpstring);
      tmpbin++;
      
      m_numberOflvl1TEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
      //      m_numberOfalllvl1TEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
      
      m_numberOflvl1TEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
      //      m_numberOfalllvl1TEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
      
    }//for ...m_configuredlvl1TETypes.begin
  }
    
  return StatusCode::SUCCESS;
}


StatusCode TrigTEMoni::fillHists()
{
  
  
  if( ( !m_numberOfTEsHist || !m_numberOfallTEsHist || !m_numberOfTEsHist2d ||
	!m_numberOfallTEsHist2d )
      ||  ( (m_trigLvl != "EF") && ( !m_numberOflvl1TEsHist2d || //!( m_numberOfalllvl1TEsHist2d>0)|| 
				     !m_numberOflvl1TEsHist )
				     //|| !( m_numberOfalllvl1TEsHist>0)  
	    )  ){
    msg()<<MSG::WARNING<<" pointers to runsummary histograms not ok, dont Fill ! "<<endreq;
    return StatusCode::FAILURE;  
  }
  
  
  for ( std::vector< unsigned int >::iterator type = m_configuredTETypes.begin();
	type != m_configuredTETypes.end(); ++type) {
    
    m_numberOfTEsHist->Fill( m_labels[ (*type) ], 
			     m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),true));
    
    m_numberOfallTEsHist->Fill( m_labels[ (*type) ], 
				m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),false));
    //2dhistos
    m_numberOfTEsHist2d->Fill( m_labels[ (*type) ], 
			       m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),true));
    
    m_numberOfallTEsHist2d->Fill( m_labels[ (*type) ], 
				  m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),false));
  }
  
  if(m_trigLvl != "EF"){
    for ( std::vector< unsigned int >::iterator type = m_configuredlvl1TETypes.begin();
	  type != m_configuredlvl1TETypes.end(); ++type) {
      
      //for ( std::vector< unsigned int >::iterator type = m_configuredlvl1TETypes_from_lvl1config.begin();
      //	  type != m_configuredlvl1TETypes_from_lvl1config.end(); ++type) {
      

	
      
      m_numberOflvl1TEsHist->Fill( m_lvl1labels[ (*type) ], 
				   m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),true));
      
      //      m_numberOfalllvl1TEsHist->Fill( m_lvl1labels[ (*type) ], 
      // m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),false));
    
      //2d histos
      m_numberOflvl1TEsHist2d->Fill( m_lvl1labels[ (*type) ], 
				     m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),true));
      
      //      m_numberOfalllvl1TEsHist2d->Fill( m_lvl1labels[ (*type) ], 
      //				m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType((*type),false));

    }
  }
  return StatusCode::SUCCESS;
}

