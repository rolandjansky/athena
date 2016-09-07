/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprPedestalPlotManager.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"

L1CaloPprPedestalPlotManager::L1CaloPprPedestalPlotManager(ITHistSvc* histoSvc, 
							   ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(histoSvc, 
			   offlineTT_tool, 
			   0, 
			   lumimax, 
			   pathInRootFile, 
			   "Pedestal", 
			   "pedestal", 
			   "Pedestal", 
			   "",
			   true),
      m_l1CondSvc("L1CaloCondSvc", histoSvc->name()),
      m_conditionsContainer(0),
      m_conditionsContainerRun2(0),
      m_pedestalMaxWidth(0.),
      m_distanceFromHeadOfTrain(0),
      m_firstCall(true),
      m_isRun2(false)
{
}

// --------------------------------------------------------------------------

L1CaloPprPedestalPlotManager::L1CaloPprPedestalPlotManager(ManagedMonitorToolBase* aMonObj,
							   ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(aMonObj,
			   onlineTT_tool,
			   0,
			   lumimax,
			   pathInRootFile,
			   "Pedestal",
			   "pedestal",
			   "Pedestal",
			   ""),
      m_l1CondSvc("L1CaloCondSvc", aMonObj->name()),
      m_conditionsContainer(0),
      m_conditionsContainerRun2(0),
      m_pedestalMaxWidth(0.),
      m_distanceFromHeadOfTrain(0),
      m_firstCall(true),
      m_isRun2(false)

{
}

// --------------------------------------------------------------------------

double L1CaloPprPedestalPlotManager::getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum /*theLayer*/)
{
  
    // only analyze non-signal towers
    // otherwise return default value -1000.
    if ( trigTower->cpET() ) { return -1000 ; } // && trigTower->jepET()

    unsigned int coolID = trigTower->coolId();

    const std::vector<short unsigned int>& EtLut = trigTower->adc();
    
    const int nSlices = EtLut.size();
    double pedMean = 0;

    if (m_currentRunNo >= 253377) {
      m_isRun2 = true;
    }

    if (m_firstCall) {
        this->loadConditionsContainer();
        m_firstCall = false;
    }
    
    if (m_conditionsContainer || m_conditionsContainerRun2)
    {   
        if (m_isRun2 == false) {
	  
	  const L1CaloPprConditions* ttConditions = m_conditionsContainer->pprConditions(coolID);
	  if (ttConditions)
	  {
	      pedMean = ttConditions->pedMean();
	  }
	  else
	  {
	      *m_log<<MSG::WARNING<< "No L1CaloPprConditions available" << endmsg;
	  }
	}
	if (m_isRun2 == true) {
	  const L1CaloPprConditionsRun2* ttConditions = m_conditionsContainerRun2->pprConditions(coolID);
	  if (ttConditions)
	  {
	      pedMean = ttConditions->pedMean();
	  }
	  else
	  {
	      *m_log<<MSG::WARNING<< "No L1CaloPprConditions available" << endmsg;
	  }
	}
    }
    else
    {
        *m_log<<MSG::WARNING<< "No Conditions Container available" << endmsg;
    }

    double pedestal = 0;  // to be returned
    double pedestalAtSlice;
    
    // now compute monitored value:
    // average the difference between the ADC count at every slice
    // and the mean pedestal applied to the tower over the slices
    for( int i = 0; i < nSlices; i++ )
    {
        pedestalAtSlice = EtLut[i]-pedMean;
	// only analyze towers with activity close 
	// to the pedestal mean value
	if ( fabs(pedestalAtSlice) > m_pedestalMaxWidth ) { return -1000.; }
	pedestal += pedestalAtSlice;
    }
    pedestal/=static_cast<double>(nSlices);
    return pedestal;
    
}


// --------------------------------------------------------------------------

void L1CaloPprPedestalPlotManager::fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value)
{
    /* Create global partition integrated plots */
    /********************************************************************************************************/
  
    double eta  = trigTower->eta();
    double phi  = trigTower->phi();

    if(!m_p_online_em_valueVsLumi){

        std::string mergeMethod("");
	int nbins = m_lumiMax;
	double xmin = 0.;
	double xmax = m_lumiMax;
	if (AthenaMonManager::environment() != AthenaMonManager::online) {
	    // Tier0 - assumes each job processes only one lumiblock
	    mergeMethod = "mergeRebinned";
	    nbins = 1;
	    xmin  = m_lumiNo;
	    xmax  = m_lumiNo + 1.;
        }

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_GlobalMrg(m_monObj,m_pathInRootFile, ManagedMonitorToolBase::run, attr, "", mergeMethod);
        m_histTool->setMonGroup(&ADC_GlobalMrg);

        m_p_online_em_valueVsLumi= 
	  m_histTool->bookProfile(Form("ppm_em_1d_profile_adc_%sVsLumi",m_monitoringName.data()),
			 	  Form("Profile plot of %s Vs Lumi Block: Em TriggerTower; Lumi Block; %s %s",
				     m_monitoringTitle.data(),
				     m_monitoringTitle.data(),
				     m_monitoringDimension.data()),
				     nbins, xmin, xmax);
        m_p_online_had_valueVsLumi 
	  = m_histTool->bookProfile(Form("ppm_had_1d_profile_adc_%sVsLumi",m_monitoringName.data()),
				    Form("Profile plot of %s Vs Lumi Block: Had TriggerTower; Lumi Block; %s %s",
				       m_monitoringTitle.data(),
				       m_monitoringTitle.data(),
				       m_monitoringDimension.data()),
				       nbins, xmin, xmax);
	  
	m_h_ppm_em_2d_value_BCN_Lumi =
          m_histTool->bookProfile2D(Form("ppm_em_pedestal_BCN_lumi"),
                                  Form("Profile plot of Pedestal Vs BCN Vs Lumi: Em TriggerTower; Lumi Block; Relative BCN"),
                                  nbins, xmin, xmax, 77, 0, 154);
	  
        m_h_ppm_had_2d_value_BCN_Lumi =
          m_histTool->bookProfile2D(Form("ppm_had_pedestal_BCN_lumi"),
                                  Form("Profile plot of Pedestal Vs BCN Vs Lumi: Had TriggerTower; Lumi Block; Relative BCN"),
                                  nbins, xmin, xmax, 77, 0, 154);
    
        ManagedMonitorToolBase::MonGroup ADC_Global(m_monObj,m_pathInRootFile, ManagedMonitorToolBase::run, attr);
        m_histTool->setMonGroup(&ADC_Global);

        m_p_online_em_etaPhiValue
	  = m_histTool->bookProfilePPMEmEtaVsPhi(Form("ppm_em_2d_profile_etaPhi_adc_%s",m_monitoringName.data()),
					         Form("#eta-#phi profile map of %s: Em Trigger Tower",m_monitoringTitle.data()));

        m_p_online_had_etaPhiValue
	  = m_histTool->bookProfilePPMHadEtaVsPhi(Form("ppm_had_2d_profile_etaPhi_adc_%s",m_monitoringName.data()),
						  Form("#eta-#phi profile map of %s: Had Trigger Tower",m_monitoringTitle.data()));
    
        m_h_online_em_etaPhiValueRMS
	  = m_histTool->bookPPMEmEtaVsPhi(Form("ppm_em_2d_etaPhi_adc_%sRMS",m_monitoringName.data()),
					  Form("#eta-#phi map of %s RMS: Em Trigger Tower",m_monitoringTitle.data()));
    
        m_h_online_had_etaPhiValueRMS
	  = m_histTool->bookPPMHadEtaVsPhi(Form("ppm_had_2d_etaPhi_adc_%sRMS",m_monitoringName.data()),
					   Form("#eta-#phi map of %s RMS: Had Trigger Tower",m_monitoringTitle.data()));

        m_p_online_em_valueVsBCN= 
	  m_histTool->bookProfile(Form("ppm_em_1d_profile_adc_%sVsBCN",m_monitoringName.data()),
			 	  Form("Profile plot of %s Vs BCN: Em TriggerTower; BCN; %s %s",
				     m_monitoringTitle.data(),
				     m_monitoringTitle.data(),
				     m_monitoringDimension.data()),
				     0xdec, 0, 0xdec);
        m_p_online_had_valueVsBCN 
	  = m_histTool->bookProfile(Form("ppm_had_1d_profile_adc_%sVsBCN",m_monitoringName.data()),
				    Form("Profile plot of %s Vs BCN: Had TriggerTower; BCN; %s %s",
				       m_monitoringTitle.data(),
				       m_monitoringTitle.data(),
				       m_monitoringDimension.data()),
				       0xdec, 0, 0xdec);
        m_histTool->unsetMonGroup();
    }
    //Fill Channel and detector partition integrated plots.
    if( layer == 0 ) {
      m_p_online_em_valueVsLumi->Fill(m_lumiNo,value);
      m_p_online_em_valueVsBCN->Fill(m_bunchCrossing,value);
      if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 1) { //After long gap
            m_h_ppm_em_2d_value_BCN_Lumi->Fill(m_lumiNo, m_distanceFromHeadOfTrain[m_bunchCrossing].second, value);
          }
      if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 0) { //After short gap
            m_h_ppm_em_2d_value_BCN_Lumi->Fill(m_lumiNo, (m_distanceFromHeadOfTrain[m_bunchCrossing].second + 80), value);
          }
    }
    else {
      m_p_online_had_valueVsLumi->Fill(m_lumiNo,value);
      m_p_online_had_valueVsBCN->Fill(m_bunchCrossing,value);
      if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 1) { //After long gap
            m_h_ppm_had_2d_value_BCN_Lumi->Fill(m_lumiNo, m_distanceFromHeadOfTrain[m_bunchCrossing].second, value);
      }
      if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 0) { //After short gap
            m_h_ppm_had_2d_value_BCN_Lumi->Fill(m_lumiNo, (m_distanceFromHeadOfTrain[m_bunchCrossing].second + 80), value);
      }
    }    
    
    //Fill Etaphi Plots
    int binX(0),binY(0);
    double entries = 0;
    double val = 0;
    double rms = 0;
    
    if( layer == 0 )
    {
        m_histTool->fillPPMEmEtaVsPhi(m_p_online_em_etaPhiValue,eta,phi,value,1);
	if (AthenaMonManager::environment() == AthenaMonManager::online) { // Needs post-processing algorithm on Tier0
	    m_histTool->findBinPPMEmEtaVsPhi(m_p_online_em_etaPhiValue,eta,phi,binX,binY);
	    m_p_online_em_etaPhiValue->GetBinInfo(binX, binY, entries, val, rms);
	    m_histTool->setBinPPMEmEtaVsPhi(m_h_online_em_etaPhiValueRMS, 
					    binX, binY, rms, rms/sqrt(2*entries));
        }
    }
    else
    {
        m_histTool->fillPPMHadEtaVsPhi(m_p_online_had_etaPhiValue,eta,phi,value,1);
	if (AthenaMonManager::environment() == AthenaMonManager::online) {
	    m_histTool->findBinPPMHadEtaVsPhi(m_p_online_had_etaPhiValue,eta,phi,binX,binY);
	    m_p_online_had_etaPhiValue->GetBinInfo(binX, binY, entries, val, rms);
	    m_histTool->setBinPPMHadEtaVsPhi(m_h_online_had_etaPhiValueRMS,
					     binX, binY, rms, rms/sqrt(2*entries));
        }
    }
    
}

// --------------------------------------------------------------------------

void L1CaloPprPedestalPlotManager::fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value)
{   
    //Create Channel Integrated, Partition differentiated plots
    /****************************************************************************************************/
    
    double eta  = trigTower->eta();
    CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta,theLayer);
    
    std::map<CaloDivisionEnum, TProfile_LW*>::iterator part_itr = m_map_online_partitionProfile_ValueVsLumi.find(detectorRegion);
    std::map<CaloDivisionEnum, TProfile_LW*>::iterator part_itr2 = m_map_online_partitionProfile_ValueVsBCN.find(detectorRegion);
    std::map<CaloDivisionEnum, TProfile2D_LW*>::iterator part_itr3 = m_map_online_partitionProfile_ValueVsLumiVsBCN.find(detectorRegion);

    if(part_itr == m_map_online_partitionProfile_ValueVsLumi.end())
    {
        std::string plotType = this->GetDetectorLayerString(theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);
        std::string mergeMethod("");
	int nbins = m_lumiMax;
	double xmin = 0.;
	double xmax = m_lumiMax;
	if (AthenaMonManager::environment() != AthenaMonManager::online) {
	    mergeMethod = "mergeRebinned";
	    nbins = 1;
	    xmin  = m_lumiNo;
	    xmax  = m_lumiNo + 1.;
        }

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Partitions(m_monObj, m_pathInRootFile , ManagedMonitorToolBase::run, attr, "", mergeMethod);
        m_histTool->setMonGroup(&ADC_Partitions);
	
        TProfile_LW* anLWProfileHist = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%s_%sVsLumi",
								    plotType.data(),
								    detectorRegionString.data(),
								    m_monitoringName.data()),
							       Form("Run:%d, %s Vs Lumi Profile for partition %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    detectorRegionString.data()),
								    nbins, xmin, xmax);
	
	TProfile_LW* anLWProfileHist2 = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%s_%sVsBCN",
								    plotType.data(),
								    detectorRegionString.data(),
								    m_monitoringName.data()),
							       Form("Run:%d, %s Vs BCN for partition %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    detectorRegionString.data()),
								    0xdec, 0, 0xdec);
	
	TProfile2D_LW* anLWProfileHist3 = m_histTool->bookProfile2D(Form("ppm_%s_2d_profile_adc_%s_%sVsLumiVsBCN",
								    plotType.data(),
								    detectorRegionString.data(),
								    m_monitoringName.data()),
							       Form("Run:%d, %s Vs BCN Vs LumiBlock for partition %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    detectorRegionString.data()),
								    nbins, xmin, xmax, 77, 0, 154);
	
	anLWProfileHist->Fill(m_lumiNo,value);
	anLWProfileHist2->Fill(m_bunchCrossing,value);
	if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 1) { //After long gap
	  anLWProfileHist3->Fill(m_lumiNo,m_distanceFromHeadOfTrain[m_bunchCrossing].second,value);
	}
	if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 0) { //After short gap
	  anLWProfileHist3->Fill(m_lumiNo,(m_distanceFromHeadOfTrain[m_bunchCrossing].second+80),value);
	}
	
	m_map_online_partitionProfile_ValueVsLumi.insert( std::pair<CaloDivisionEnum, TProfile_LW*> (detectorRegion,anLWProfileHist));
	m_map_online_partitionProfile_ValueVsBCN.insert( std::pair<CaloDivisionEnum, TProfile_LW*> (detectorRegion,anLWProfileHist2));
	m_map_online_partitionProfile_ValueVsLumiVsBCN.insert( std::pair<CaloDivisionEnum, TProfile2D_LW*> (detectorRegion,anLWProfileHist3));
	
        m_histTool->unsetMonGroup();
    }
    else
    {
        part_itr->second->Fill(m_lumiNo,value);
	part_itr2->second->Fill(m_bunchCrossing,value);
	if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 1) { //After long gap
	  part_itr3->second->Fill(m_lumiNo,m_distanceFromHeadOfTrain[m_bunchCrossing].second,value);
	}
	if (!m_distanceFromHeadOfTrain.empty() && m_distanceFromHeadOfTrain[m_bunchCrossing].first == 0) { //After short gap
	  part_itr3->second->Fill(m_lumiNo,(m_distanceFromHeadOfTrain[m_bunchCrossing].second+80),value);
	}
    } 
}

// --------------------------------------------------------------------------

/*virtual*/
void L1CaloPprPedestalPlotManager::fillDifferentialOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int &coolId, CalLayerEnum theLayer, double &value)
{
    //Create fully differential plots
    /****************************************************************************************************/
    
    std::map<unsigned int, TProfile_LW*>::iterator p_itr;
    p_itr = m_map_online_coolIDProfile_ValueVsLumi.find(coolId);
    
    if(p_itr == m_map_online_coolIDProfile_ValueVsLumi.end())
    {
        double eta  = trigTower->eta();
        std::string plotType = this->GetDetectorLayerString(theLayer);
        CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta,theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Channels(m_monObj, 
						      m_pathInRootFile + Form("/%s",detectorRegionString.data()), 
						      ManagedMonitorToolBase::run,
                                                      attr);
        m_histTool->setMonGroup(&ADC_Channels);

        std::string titles;
	if ( m_ppmAdcMinValue > 0 ) {
	    titles=Form("Run:%d, %s Vs Lumi Profile for %sFADC >%d, Channel:%08x ;Lumi Block; %s %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    plotType.data(),
								    m_ppmAdcMinValue,
								    coolId,
								    m_monitoringTitle.data(),
								    m_monitoringDimension.data());
	}
	else {
	    titles=Form("Run:%d, %s Vs Lumi Profile, Channel:%08x ;Lumi Block; %s %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    coolId,
								    m_monitoringTitle.data(),
								    m_monitoringDimension.data());
	}
        TProfile_LW* anLWProfileHist = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%08x_%sVsLumi",
								    plotType.data(),
								    coolId,
								    m_monitoringName.data()),
							       titles,									
							       m_lumiMax,
							       0,
							       m_lumiMax);
	anLWProfileHist->Fill(m_lumiNo,value);
	m_map_online_coolIDProfile_ValueVsLumi.insert ( std::pair<unsigned int,TProfile_LW*>(coolId,anLWProfileHist) );

        m_histTool->unsetMonGroup();
	
    }
    else
    {
        p_itr->second->Fill(m_lumiNo,value);
    }

    //Now for bunch crossing number:
    std::map<unsigned int, TProfile_LW*>::iterator p_itr2;
    p_itr2 = m_map_online_coolIDProfile_ValueVsBCN.find(coolId);
    if(p_itr2 == m_map_online_coolIDProfile_ValueVsBCN.end())
    {
        double eta  = trigTower->eta();
        std::string plotType = this->GetDetectorLayerString(theLayer);
        CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta,theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);

        ManagedMonitorToolBase::MgmtAttr_t attr = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Channels(m_monObj, 
						      m_pathInRootFile + Form("/%s",detectorRegionString.data()), 
						      ManagedMonitorToolBase::run,
                                                      attr);
        m_histTool->setMonGroup(&ADC_Channels);

        std::string titles;
	if ( m_ppmAdcMinValue > 0 ) {
	    titles=Form("Run:%d, %s Vs BCN Profile for %sFADC >%d, Channel:%08x ;BCN; %s %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    plotType.data(),
								    m_ppmAdcMinValue,
								    coolId,
								    m_monitoringTitle.data(),
								    m_monitoringDimension.data());
	}
	else {
	    titles=Form("Run:%d, %s Vs BCN Profile, Channel:%08x ;BCN; %s %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    coolId,
								    m_monitoringTitle.data(),
								    m_monitoringDimension.data());
	}
        TProfile_LW* anLWProfileHist2 = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%08x_%sVsBCN",
								    plotType.data(),
								    coolId,
								    m_monitoringName.data()),
							       titles,									
							       0xdec, 0, 0xdec);
	anLWProfileHist2->Fill(m_bunchCrossing,value);
	m_map_online_coolIDProfile_ValueVsBCN.insert ( std::pair<unsigned int,TProfile_LW*>(coolId,anLWProfileHist2) );

        m_histTool->unsetMonGroup();
	
    }
    else
    {
        p_itr2->second->Fill(m_bunchCrossing,value);
    }

}

// --------------------------------------------------------------------------

bool L1CaloPprPedestalPlotManager::doMonitoring(double &value)
{
    // check if default value (-1000.) has been determined
    return ( value!=-1000. );
}

// --------------------------------------------------------------------------

void L1CaloPprPedestalPlotManager::loadConditionsContainer()
{
    if (m_l1CondSvc)
    {
        *m_log<<MSG::DEBUG<< "Retrieving Conditions Container" << endmsg;
	StatusCode sc;
	if (m_isRun2 == true) {
	  sc = m_l1CondSvc->retrieve(m_conditionsContainerRun2);
	}
	else {
	  sc = m_l1CondSvc->retrieve(m_conditionsContainer);
	}
	if (sc.isFailure()) 
	{
	    *m_log<<MSG::WARNING<< "Could not retrieve Conditions Container" << endmsg;
	}
	else 
	{
	    *m_log<<MSG::DEBUG<< "Retrieved Conditions Container" << endmsg;
	}
    }
    else 
    {
        *m_log<<MSG::WARNING<< "Could not retrieve Conditions Containers" << endmsg;
    }
}
