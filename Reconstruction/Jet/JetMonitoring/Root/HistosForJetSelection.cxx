/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/HistosForJetSelection.h"
#include "AthContainers/ConstDataVector.h"
#include "TH1.h"

HistosForJetSelection::HistosForJetSelection(const std::string &t) : JetHistoBase(t) 
                                                                   , m_histoTools(this)
                                                                   , m_selType(AllJets)
                                                                   , m_titleSuffix("")
                                                                   , m_nameSuffix("")
{
  declareProperty("SelectionType", m_selType=AllJets);
  declareProperty("HistoTools", m_histoTools);
  declareProperty("HistoTitleSuffix", m_titleSuffix);
  declareProperty("HistoNameSuffix", m_nameSuffix);
  declareProperty("JetSelectorTool", m_selTool);
  declareProperty("InverseToolSelection", m_inverseToolSelection);
}
  
HistosForJetSelection::~HistosForJetSelection(){
}
  
StatusCode HistosForJetSelection::initialize() {
  CHECK( m_histoTools.retrieve() );
  CHECK( JetHistoBase::initialize() );

  // build the title and name modifiers

  switch((SelectionType)m_selType){
  case LeadingJet:
    m_titleSuffix = " (Leading Jet)";
    m_nameSuffix = "leadingjet";
    break;
  case SubLeadingJet:
    m_titleSuffix = " (SubLeading Jet)";
    m_nameSuffix = "subleadingjet";
    break;
  case FromTool:
    {
      CHECK(m_selTool.retrieve());
      if( (m_nameSuffix == "" ) || (m_titleSuffix=="") ) {
        ATH_MSG_ERROR("When using a selection tool, please set BOTH of HistoTitleSuffix (="<<m_titleSuffix<<") and HistoNameSuffix (="<<m_nameSuffix<<") or set them explicitely to 'none'. This is to avoid histo names clashes.");
        return StatusCode::FAILURE;      
      }
      if(m_nameSuffix == "none")  m_nameSuffix="";
      if(m_titleSuffix == "none") m_titleSuffix="";
      // either ask from tool or let m_xxSuffix be properties
    }
    break;
  default:
    break;
  }
  return StatusCode::SUCCESS;
}


int HistosForJetSelection::buildHistos(){
  // build histos of each subtools
  int count=0;
  for( auto jtool : m_histoTools){
    count+=jtool->buildHistos();
    // keep a pointer to histos :
    const auto & hdata = jtool->bookedHistograms();
    ATH_MSG_DEBUG(" Histo tool "<< jtool->name() << "  count= "<< count<< " nh="<<hdata.size());
    for( const auto & hd : hdata ){ 
      TH1 * h = const_cast<TH1*>(hd.hist);
      ATH_MSG_DEBUG(" Histo tool  h "<< h->GetName() );
      m_vBookedHistograms.push_back(hd);
      modifyNameAndTitle(h);
    }
  }
  return count;
}

  
  
int HistosForJetSelection::fillHistosFromContainer(const xAOD::JetContainer & cont){

  ConstDataVector< xAOD::JetContainer > tmpCont(SG::VIEW_ELEMENTS);
  const xAOD::JetContainer * contPtr = NULL;

  /// Set the contPtr to the relevant container according to m_selType
  switch((SelectionType)m_selType){
  case LeadingJet:
    {
      if(!cont.empty()){
        tmpCont.push_back(cont[0]);
        contPtr = tmpCont.asDataVector();
      } else return 0;
    }
    break;

  case SubLeadingJet:
    {
      if(cont.size()>1){
        tmpCont.push_back(cont[1]);
        contPtr = tmpCont.asDataVector();
      }else return 0;
    }
    break;

  case AllJets:
    contPtr = &cont;
    break;

  case FromTool:
    {
      // filter :
      for(const xAOD::Jet* jet : cont ) { 
	if( m_inverseToolSelection ) 
	  {
	    if(! m_selTool->keep(*jet) ) 
	      {
		tmpCont.push_back(jet) ;
	      }
	  }
	else if( m_selTool->keep(*jet) ) 
	  {
	    tmpCont.push_back(jet) ;
	  }
      }
      contPtr = tmpCont.asDataVector();
    }
    break;
  default:
    break;
  }

  // then fill histos
  for( auto jtool : m_histoTools){
    ATH_MSG_DEBUG ("    Filling  " << jtool->name() << "..." );
    jtool->fillHistosFromContainer(*contPtr);
  }
  return 0;
}

void HistosForJetSelection::modifyNameAndTitle(TH1* h){
  h->SetName( (h->GetName()+m_nameSuffix).c_str());
  h->SetTitle( (h->GetTitle()+m_titleSuffix).c_str() );
}

int HistosForJetSelection::finalizeHistos(){
  int count = 0;
  for( auto jtool : m_histoTools){
    count+=jtool->finalizeHistos();
  }
  return count;

}

void HistosForJetSelection::prefixHistoDir(const std::string & preDir){
  for( auto jtool : m_histoTools){
    jtool->prefixHistoDir(preDir+m_histoDir);
  }
}

void HistosForJetSelection::setInterval(Interval_t ityp, bool force ){
  // propagate interval to sub-tools
  for( auto jtool : m_histoTools){
    jtool->setInterval(ityp,force);
  }
}
