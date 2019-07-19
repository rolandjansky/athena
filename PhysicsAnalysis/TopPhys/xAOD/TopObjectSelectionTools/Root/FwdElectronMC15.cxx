/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/FwdElectronMC15.h"
#include "TopEvent/EventTools.h"

namespace top {

FwdElectronMC15::FwdElectronMC15(double ptcut, double minEtacut, double maxEtacut, const std::shared_ptr<top::TopConfig> config) :
    m_ptcut(ptcut),
    m_minEtacut(minEtacut),
    m_maxEtacut(maxEtacut),
    m_fwdElectronSelectionTool("CP::FwdElectronSelector"),
    m_fwdElectronLooseSelectionTool("CP::FwdElectronSelectorLoose"),
    m_webBunchCrossingTool("CP::WebBunchCrossingTool"),
    m_config(config)
{
	m_isMC=m_config->isMC();
	top::check( m_fwdElectronSelectionTool.retrieve() , "Failed to retrieve CP::FwdElectronSelector" );
	top::check( m_fwdElectronLooseSelectionTool.retrieve() , "Failed to retrieve CP::FwdElectronSelectorLoose" );
	if(!m_isMC)
		top::check( m_webBunchCrossingTool.retrieve(), "Failed to retrieve CP::WebBunchCrossingTool");
	m_fwdElectronBCIDCleaningMinRun=config->fwdElectronBCIDCleaningMinRun();
    m_fwdElectronBCIDCleaningMaxRun=config->fwdElectronBCIDCleaningMaxRun();
}

bool FwdElectronMC15::passSelection(const xAOD::Electron& el, int bcid, int runNumber) const {

    if (!passSelectionCommon(el, bcid, runNumber))
      return false;
      
    if (!m_fwdElectronSelectionTool->accept(&el))
      return false;
	
 
  
    return true;
}

bool FwdElectronMC15::passSelectionLoose(const xAOD::Electron& el, int bcid, int runNumber) const {
    
    if (!passSelectionCommon(el,bcid, runNumber))
      return false;
      
    if (!m_fwdElectronLooseSelectionTool->accept(&el))
		return false;
	
	return true;

}

bool FwdElectronMC15::passSelectionCommon(const xAOD::Electron& el, int bcid, int runNumber) const {

    if (el.pt() < m_ptcut)
        return false;
    
    if(fabs(el.eta()) < m_minEtacut)
		return false;
    
    if(fabs(el.eta()) > m_maxEtacut)
		return false;
    
    if(!m_isMC){
		//fwd electrons in data in first 6 bunch crossings shouldn't be used in 2015+16 data, see ANALYSISTO-760
		
		if(fabs(el.eta())>2.5 && fabs(el.eta())<3.2 && runNumber>=m_fwdElectronBCIDCleaningMinRun && runNumber <=m_fwdElectronBCIDCleaningMaxRun)
		{ 
			double distance = m_webBunchCrossingTool->gapBeforeTrain(bcid)>500 ? m_webBunchCrossingTool->distanceFromFront(bcid) : (m_webBunchCrossingTool->distanceFromFront(bcid)+m_webBunchCrossingTool->gapBeforeTrain(bcid) + ( m_webBunchCrossingTool->distanceFromFront(bcid)+m_webBunchCrossingTool->distanceFromTail(bcid) ));
			 if ( distance< 5.5*25) return false;
		}
	}

    return true;
}

void FwdElectronMC15::print(std::ostream& os) const {
    os << "FwdElectronMC15\n";
    os << "    * pT > " << m_ptcut << "\n";

}

}
