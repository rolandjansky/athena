/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTEMoni.h"

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
     m_lvl1ConfigSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
{
  declareInterface<IMonitorToolBase>(this);
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
  
  const TrigConf::ThresholdConfig* thresholdConfig = m_lvl1ConfigSvc->thresholdConfig();
  const std::vector<TrigConf::TriggerThreshold*>& thresholdVector = thresholdConfig->getThresholdVector();

  std::map<std::string, unsigned int> foundLvl1TETypes;

  for (const TrigConf::TriggerThreshold* lvl1TT : thresholdVector) {

    unsigned int teId = 0;
    bool activeHLT = TrigConf::HLTTriggerElement::getId(lvl1TT->name().c_str(), teId);
    if (activeHLT) {

      foundLvl1TETypes[lvl1TT->name()] = teId;
      
    }
    int tn = lvl1TT->thresholdNumber()+1;
    if (tn < 0) {
      msg() << MSG::WARNING << "this LVL1 threshold bit position (" << tn << ") is smaller 0"
            << ", go on w/o this threshold!" << endmsg;
    }
  }
  //now build m_configuredlvl1TETypes, m_lvl1labels

  unsigned int bin_number_lvl1=0;
  for (const auto& l1te : foundLvl1TETypes) { // std::map<std::string, unsigned int>
    bin_number_lvl1++;
    m_configuredlvl1TETypes.push_back(l1te.second);
    m_lvl1labels[l1te.second]=bin_number_lvl1;
  }

  /*
    get input and output HLT Triggerelement types -> store types in m_configuredTETypes and
    corresponding bin number m_labels
  */
  std::vector<const HLT::SteeringChain*> configuredChains = m_parentAlg->getConfiguredChains();
  unsigned int bin_number = 0;
  // first stick the TE type IDs into a map keyed by TE name (for sorting) 
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
  for (const auto& te : foundTETypes) { // std::map<std::string, unsigned int>

    m_configuredTETypes.push_back( te.second );

    if(!te.first.empty()) {
      bin_number++;
      m_labels[ te.second ] = bin_number;
    } else {
      m_labels[ te.second ] = 0; // into the underflow bin
      ATH_MSG_WARNING("TE with blank label found. TEType: " << te.second);
    }
  }
  
  // cross check bin_number = number of TETypes ? 
  // This can happen now if blank labels are found (as in the rerundb test)
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
    run summary
  */   
  std::string name  = "NumberOfActiveTEs";
  std::string title = "TE abundance in "+ m_trigLvl;
  m_numberOfTEsHist = new  TH1I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5) ;
  if ( expertHistograms.regHist(m_numberOfTEsHist).isFailure() ) 
    msg() << MSG::WARNING << "Can't book histogram " << name << endmsg;

  /*
    histo for number of all TE
  */
  name  = "NumberOfAllTEs";
  title    = "All TE abundance in "+ m_trigLvl;
  m_numberOfallTEsHist =new TH1I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 ) ;  
  if ( expertHistograms.regHist( m_numberOfallTEsHist ).isFailure()) 
    msg() << MSG::WARNING << "Can't book histogram " << name << endmsg;

  /*
    2dhisto for number of input TEs 
  */
  name  = "NumberOfActiveTEsPerEvent";
  title  = "Number of TE per event in "+ m_trigLvl;
  m_numberOfTEsHist2d =new TH2I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 ) ;
  if ( expertHistograms.regHist( m_numberOfTEsHist2d ).isFailure()) 
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endmsg;

  /*
    2dhisto for number of all input TEs 
  */  
  name  = "NumberOfAllTEsPerEvent";
  title  = "Number of all TE per event in "+ m_trigLvl;
  m_numberOfallTEsHist2d = new TH2I(name.c_str(),title.c_str(), bin_number,0.5, bin_number+0.5 , 20,-0.5,19.5 );

  if ( expertHistograms.regHist( m_numberOfallTEsHist2d ).isFailure()) 
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endmsg;
 
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
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endmsg;


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
    msg() << MSG::WARNING << "Can't book Trig histogram " << name << endmsg;
    
  
  //naming the histogram bins according to TE label
  int tmpbin=0;
  for ( unsigned int type : m_configuredTETypes ) {
    TrigConf::HLTTriggerElement::getLabel (type,tmpstring);
    tmpbin++;
    
    m_numberOfTEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    m_numberOfallTEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    
    m_numberOfTEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());
    m_numberOfallTEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str()); 
    
  }

  tmpbin=0;
  for ( unsigned int type : m_configuredlvl1TETypes ) {
    TrigConf::HLTTriggerElement::getLabel (type,tmpstring);
    tmpbin++;
      
    m_numberOflvl1TEsHist->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());      
    m_numberOflvl1TEsHist2d->GetXaxis()->SetBinLabel(tmpbin,tmpstring.c_str());      
  } 
    
  return StatusCode::SUCCESS;
}


StatusCode TrigTEMoni::fillHists()
{  
  for ( unsigned int type : m_configuredTETypes ) {
    int c = m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType(type,true);
    int cAll = m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType(type,false);

    m_numberOfTEsHist->Fill( m_labels[type], c);
    m_numberOfallTEsHist->Fill( m_labels[type], cAll);

    //2dhistos
    m_numberOfTEsHist2d->Fill( m_labels[type], c);
    m_numberOfallTEsHist2d->Fill( m_labels[type], cAll);
  }
  
  for ( unsigned int type : m_configuredlvl1TETypes ) {
    int c = m_parentAlg->getAlgoConfig()->getNavigation()->countAllOfType(type,true);
    m_numberOflvl1TEsHist->Fill( m_lvl1labels[type], c);
    
    //2d histos
    m_numberOflvl1TEsHist2d->Fill( m_lvl1labels[type], c);
  }
  return StatusCode::SUCCESS;
}

