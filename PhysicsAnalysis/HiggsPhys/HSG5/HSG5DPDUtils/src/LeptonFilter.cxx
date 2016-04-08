/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"

#include "HSG5DPDUtils/LeptonFilter.h"

namespace HSG5 {

//________________________________________________________________________
LeptonFilter::LeptonFilter( const std::string& name,
			    ISvcLocator* pSvcLocator ) 
  : AthFilterAlgorithm(name,pSvcLocator),
    m_electronFilter(0)
{

  declareProperty("ElectronFilterNameAndType", m_electronFilterNameAndType="D2PDElectronSelector/ElectronSelector");

  //declareProperty("MuonFilterNameAndType", m_muonFilterNameAndType="D2PDMuonSelector/MuonSelector");

  std::vector<std::string> defaultFilters;
  defaultFilters.push_back("D2PDMuonSelector/MuonSelector");
  declareProperty("MuonFilterNameAndTypeVec", m_muonFilterNameAndTypeVec=defaultFilters);

}

//________________________________________________________________________
LeptonFilter::~LeptonFilter()
{

}

//________________________________________________________________________
StatusCode LeptonFilter::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  // get electron sub-algorithm type and instance names
  std::string elType = findType(m_electronFilterNameAndType);
  std::string elName = findName(m_electronFilterNameAndType);
  ATH_MSG_INFO("Electron filter sub-algorithm type="<<elType
	       <<" and instance name="<<elName);
  sc = createSubAlgorithm(elType,elName,m_electronFilter);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failed to create electron filter algorithm");
    return sc;
  }

  // get muon sub-algorithms
  int nfilters=m_muonFilterNameAndTypeVec.size();
  m_muonFilter.resize(nfilters);

  for (int ifilter=0;ifilter<nfilters;ifilter++) {
    std::string muType = findType(m_muonFilterNameAndTypeVec[ifilter]);
    std::string muName = findName(m_muonFilterNameAndTypeVec[ifilter]);
    ATH_MSG_INFO("Muon filter sub-algorithm type="<<muType<<" and instance name="
		 <<muName);
    sc = createSubAlgorithm(muType,muName,m_muonFilter[ifilter]);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Failed to create muon filter algorithm");
    }
  }

  return sc;
}

//________________________________________________________________________
StatusCode LeptonFilter::finalize()
{
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode LeptonFilter::execute()
{
  ATH_MSG_DEBUG("Start executing lepton filter");

  StatusCode sc = StatusCode::SUCCESS;
  
  bool muonPassed=false;
  for (int ifilter=0;ifilter<(int)m_muonFilter.size();ifilter++) {
    sc = m_muonFilter[ifilter] -> execute();
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Failure in muonFilter execute");
      return sc;
    }
    if (m_muonFilter[ifilter]->filterPassed()) {
      muonPassed=true;
      break;
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    if (muonPassed) ATH_MSG_DEBUG("passed muon selection");
    else            ATH_MSG_DEBUG("failed muon selection");
  }
    
  sc = m_electronFilter -> execute();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failure in electronFilter execute");
    return sc;
  }

  bool electronPassed = m_electronFilter->filterPassed();
  if (msgLvl(MSG::DEBUG)) {
    if (electronPassed) ATH_MSG_DEBUG("passed electron selection");
    else                ATH_MSG_DEBUG("failed electron selection");
  }

  if (electronPassed || muonPassed) { 
    ATH_MSG_DEBUG("passed lepton selection");
    setFilterPassed(true);
  }
  else {
    ATH_MSG_DEBUG("failed lepton selection");
    setFilterPassed(false);
  }
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
std::string LeptonFilter::findType(std::string l_property)
{
  int slash_pos = l_property.find_first_of("/");
  std::string type = l_property.substr( 0, slash_pos );
  return type ;
}

//________________________________________________________________________
std::string LeptonFilter::findName(std::string l_property){
  int slash_pos = l_property.find_first_of("/");
  std::string type = l_property.substr( 0, slash_pos );
  std::string name = (slash_pos > 0) ? l_property.substr( slash_pos + 1) : type ;
  return name ;
}

} // end namespace
