/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/HistoDefinitionTool.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

HistoDefinitionTool::HistoDefinitionTool(const std::string& n) : asg::AsgTool(n)
                                                               , m_attname("")
                                                               , m_htitle("")
{
  declareInterface<HistoDefinitionTool>(this);

  // Kepp lower case property names so they match the ROOT variable names
  // in TH1 constructors.
  declareProperty("hname", m_attname); // can't use 'name' because of incompatilbility in python
  declareProperty("title", m_htitle);

  declareProperty("nbinsx", m_nbinsx);
  declareProperty("xlow", m_xlow);
  declareProperty("xup", m_xup);

  declareProperty("nbinsy", m_nbinsy);
  declareProperty("ylow", m_ylow);
  declareProperty("yup", m_yup);

}

StatusCode HistoDefinitionTool::initialize() {

  // Use the name of the tool by default. Extract "Something.Var" -> "Var"
  if(m_attname=="") {
    auto found = name().rfind('.');    
    if(found != std::string::npos) m_attname = name().substr(++found);
    else m_attname= name();
    ATH_MSG_INFO("Built  "<< name() << "  -> "<< m_attname ) ;

  }

  return StatusCode::SUCCESS;
}

TH1F* HistoDefinitionTool::buildTH1F(){
  return new TH1F(m_attname.c_str(), m_htitle.c_str(), m_nbinsx, m_xlow, m_xup);
}

TH2F* HistoDefinitionTool::buildTH2F(){
  return new TH2F(m_attname.c_str(), m_htitle.c_str(), m_nbinsx, m_xlow, m_xup, m_nbinsy, m_ylow, m_yup);
}

TProfile* HistoDefinitionTool::buildTProfile(bool useYLimits){

  if(useYLimits) return new TProfile(m_attname.c_str(), m_htitle.c_str(), m_nbinsx, m_xlow, m_xup,  m_ylow, m_yup);
  else return new TProfile(m_attname.c_str(), m_htitle.c_str(), m_nbinsx, m_xlow, m_xup);
}


template<> TH1F* HistoDefinitionTool::build<TH1F>(){return buildTH1F();}
template<> TH2F* HistoDefinitionTool::build<TH2F>(){return buildTH2F();}
template<> TProfile* HistoDefinitionTool::build<TProfile>(){return buildTProfile();}
