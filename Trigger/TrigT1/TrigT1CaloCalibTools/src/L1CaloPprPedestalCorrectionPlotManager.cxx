/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprPedestalCorrectionPlotManager.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"



L1CaloPprPedestalCorrectionPlotManager::L1CaloPprPedestalCorrectionPlotManager(ITHistSvc* histoSvc, 
							   ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(histoSvc, 
			   offlineTT_tool, 
			   0, 
			   lumimax, 
			   pathInRootFile, 
			   "PedestalCorrection", 
			   "pedestalCorrection", 
			   "PedestalCorrection", 
			   "",
			   true),
      m_l1CondSvc("L1CaloCondSvc", histoSvc->name()),
      m_firstCall(true)
{
}

// --------------------------------------------------------------------------

L1CaloPprPedestalCorrectionPlotManager::L1CaloPprPedestalCorrectionPlotManager(ManagedMonitorToolBase* aMonObj,
							   ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(aMonObj,
			   onlineTT_tool,
			   0,
			   lumimax,
			   pathInRootFile,
			   "PedestalCorrection",
			   "pedestalCorrection",
			   "PedestalCorrection",
			   ""),
      m_l1CondSvc("L1CaloCondSvc", aMonObj->name()),
      m_firstCall(true)

{
}

// --------------------------------------------------------------------------

double L1CaloPprPedestalCorrectionPlotManager::getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer)
{   
    const std::vector<short int>& pedCorrVec = trigTower->correction();
    const int nSlices = pedCorrVec.size();
    double pedestalCorrection = 0;  // to be returned
    
    // now compute monitored value:
    for( int i = 0; i < nSlices; i++ )
    {
	pedestalCorrection += pedCorrVec[i];
    }
    pedestalCorrection/=static_cast<double>(nSlices);
    return pedestalCorrection;
    
}

// --------------------------------------------------------------------------

void L1CaloPprPedestalCorrectionPlotManager::fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value)
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
	  
	m_p_online_em_valueVsBCN 
	  = m_histTool->bookProfile(Form("ppm_em_1d_profile_adc_%sVsBCN",m_monitoringName.data()),
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
    }
    else { 
      m_p_online_had_valueVsLumi->Fill(m_lumiNo,value);
      m_p_online_had_valueVsBCN->Fill(m_bunchCrossing,value);
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

void L1CaloPprPedestalCorrectionPlotManager::fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value)
{   
    //Create Channel Integrated, Partition differentiated plots
    /****************************************************************************************************/
    
    double eta  = trigTower->eta();
    CaloDivisionEnum detectorRegion = this->GetDetectorRegion(eta,theLayer);
    std::map<CaloDivisionEnum, TProfile_LW*>::iterator part_itr = m_map_online_partitionProfile_ValueVsLumi.find(detectorRegion);

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
	std::cout << "m_histTool = " << m_histTool << std::endl;
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
	anLWProfileHist->Fill(m_lumiNo,value);

	m_map_online_partitionProfile_ValueVsLumi.insert( std::pair<CaloDivisionEnum, TProfile_LW*> (detectorRegion,anLWProfileHist));

        m_histTool->unsetMonGroup();
    }
    else
    {
        part_itr->second->Fill(m_lumiNo,value);
    }
    
    //Now for bunch crossing number:
    std::map<CaloDivisionEnum, TProfile_LW*>::iterator part_itr2 = m_map_online_partitionProfile_ValueVsBCN.find(detectorRegion);
    if(part_itr2 == m_map_online_partitionProfile_ValueVsBCN.end())
      {
        std::string plotType = this->GetDetectorLayerString(theLayer);
        std::string detectorRegionString = this->GetDetectorRegionString(detectorRegion);
        std::string mergeMethod("");
	if (AthenaMonManager::environment() != AthenaMonManager::online) {
	    mergeMethod = "mergeRebinned";
            std::cout << "mergeMethod = ''mergeRebinned''" << std::endl;
        }

        ManagedMonitorToolBase::MgmtAttr_t attr2 = ManagedMonitorToolBase::ATTRIB_UNMANAGED;
        ManagedMonitorToolBase::MonGroup ADC_Partitions2(m_monObj, m_pathInRootFile , ManagedMonitorToolBase::run, attr2, "", mergeMethod);
        m_histTool->setMonGroup(&ADC_Partitions2);
        std::cout << Form("ppm_%s_1d_profile_adc_%s_%sVsBCN",plotType.data(),detectorRegionString.data(),m_monitoringName.data()) << std::endl;
        TProfile_LW* anLWProfileHist2 = m_histTool->bookProfile(Form("ppm_%s_1d_profile_adc_%s_%sVsBCN",
								    plotType.data(),
								    detectorRegionString.data(),
								    m_monitoringName.data()),
							       Form("Run:%d, %s Vs BCN for partition %s",
								    m_currentRunNo,
								    m_monitoringTitle.data(),
								    detectorRegionString.data()),
								    0xdec, 0, 0xdec);
	anLWProfileHist2->Fill(m_bunchCrossing,value);

	m_map_online_partitionProfile_ValueVsBCN.insert( std::pair<CaloDivisionEnum, TProfile_LW*> (detectorRegion,anLWProfileHist2));

        m_histTool->unsetMonGroup();
      }
      else
      {
	part_itr2->second->Fill(m_bunchCrossing,value);
      } 
      
}

// --------------------------------------------------------------------------

bool L1CaloPprPedestalCorrectionPlotManager::doMonitoring(double &value)
{
    // check if default value (-1000.) has been determined
    return ( value!=-1000. );
}