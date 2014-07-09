/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetHistoBase.h"
#include <algorithm>

#include "TH1F.h"
#include "TString.h"
JetHistoBase::JetHistoBase(const std::string& n) : HistoGroupBase(n)
{
  declareInterface<JetHistoBase>(this);

}

StatusCode JetHistoBase::initialize() {

  //if(m_histoDir=="") {m_histoDir = "Jets/"; m_histoDir+=m_inputContainer+"/";}
  return StatusCode::SUCCESS;
}



int JetHistoBase::fillHistosFromContainer(const xAOD::JetContainer & cont){
  int count=0;
  for( const xAOD::Jet* jet : cont ){ count+=fillHistosFromJet( *jet );}
  return count;    
}

int JetHistoBase::fillHistosFromJet(const xAOD::Jet &) { return 0;}

int JetHistoBase::finalizeHistos() {return 0;}

// void JetHistoBase::setContainerAndDir(const std::string& cont, const std::string& dir){
//   m_histoDir = dir;
//   m_inputContainer = cont;
//   if(m_histoDir=="") {m_histoDir = "Jets/"; m_histoDir+=m_inputContainer+"/";}
// }


