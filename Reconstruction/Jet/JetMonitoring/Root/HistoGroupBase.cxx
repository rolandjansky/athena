/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/HistoGroupBase.h"
#include <algorithm>

#include "TH1.h"

HistoGroupBase::HistoGroupBase(const std::string& n) : asg::AsgTool(n)
                                                     , m_histoDir("")
                                                     , m_monIntervalType(8) // corresponds to "all"
                                                     , m_prefixedHistoName(false)
                                                     , m_interval(-1)
{
  declareInterface<HistoGroupBase>(this);

  declareProperty("MonitoringIntervalType", m_monIntervalType);
  declareProperty("HistoDir", m_histoDir);
  declareProperty("PrefixedHistoName", m_prefixedHistoName);
  declareProperty("IntervalType", m_interval);
}

StatusCode HistoGroupBase::initialize() {
  return StatusCode::SUCCESS;
}


int HistoGroupBase::buildHistos() { 
  return 0;
}

int HistoGroupBase::fillHistos(){
  return 0;  
}

int HistoGroupBase::finalizeHistos() {
  return 0;
}

void HistoGroupBase::prefixHistoDir(const std::string & preDir){
  m_histoDir = preDir + m_histoDir;
}

const std::vector<HistoGroupBase::HistData> & HistoGroupBase::bookedHistograms(){
  return m_vBookedHistograms;
}


void HistoGroupBase::renameAndRegister(TH1* h, const std::string & subD, Interval_t ityp){

  if( h == NULL ) return;

  if( ityp == useToolInterval) ityp = (Interval_t) m_interval;

  std::string path = m_histoDir+subD;
  std::string prefix="";
  if( m_prefixedHistoName) { 
    prefix = path;
    std::replace( prefix.begin(), prefix.end(), '/', '_');
  }
  h->SetName( (prefix+h->GetName()).c_str() );

  m_vBookedHistograms.push_back( {h, path, ityp} );
}
