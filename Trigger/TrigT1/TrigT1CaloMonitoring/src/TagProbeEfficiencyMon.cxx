/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
//
// NAME:    TagProbeEfficiencyMon.cxx
// PACKAGE: TrigT1CaloMonitoring
//
// AUTHOR:  Jason Barkeloo & Kate Whalen
//
// ************************************************************************

#include <numeric>
#include <utility>
#include <cmath>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "TVector2.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"

// ============================================
// L1 objects
// ============================================
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// ============================================
// xAOD
// ============================================
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
#include "xAODEgamma/ElectronContainer.h"

#include "TagProbeEfficiencyMon.h"

//=============================================
namespace LVL1 {
//=============================================

TagProbeEfficiencyMon::TagProbeEfficiencyMon( const std::string & type, const std::string& name, 
                                              const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent ),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
	m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_histBooked(false),
    m_h_probe_eta_tot(0),
    m_h_probe_phi_tot(0),
    m_h_probe_Et_tot(0),
    m_h_z_inv_mass(0)
{
  declareProperty("ErrorTool", m_errorTool);       //example property declaration
  declareProperty("HistogramTool", m_histTool);
  declareProperty("CPMTobRoILocation", 
                   m_cpmTobRoiLocation = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("RootDirectory", m_rootDir = "L1Calo/TagProbeEfficiency");
  declareProperty("MaxEnergyRange", m_maxEnergyRange = 256,
                  "Maximum energy plotted");
  declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

/*-------------------------------------------------------------------*/
TagProbeEfficiencyMon::~TagProbeEfficiencyMon() {}
/*-------------------------------------------------------------------*/

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*------------------------------------------------------------------*/
StatusCode TagProbeEfficiencyMon::initialize()
/*------------------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;
  
  /// Here we initialize our tools 

  StatusCode sc;
  CHECK(m_configSvc.retrieve());//
  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;
  
  sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }
  //Setting some values necessary for later use and keeping graphs consistent
  lowcutTag = 20000.;
  lowcutProbe = 2500.;
  inv_m_range = 20000.;
  m_z = 91188.;
  probe_et_bins = 20;
  probe_eta_bins = 25;
  probe_phi_bins = 32;
  probe_et_min = 0;
  probe_et_max = 100;
  probe_eta_min = -2.50;
  probe_eta_max = 2.5;
  probe_phi_min = -3.2;
  probe_phi_max = 3.2;

  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------*/
StatusCode TagProbeEfficiencyMon::finalize() 
/*----------------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------*/
StatusCode TagProbeEfficiencyMon::bookHistogramsRecurrent()
/*----------------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In TagProbeEfficiencyMon::bookHistogramsRecurrent" << endmsg;

  if (m_environment == AthenaMonManager::online) {
    // Book histograms that are only made in the online environment
  }

  if (m_dataType == AthenaMonManager::cosmics) { 
    // Book histograms that are only relevant for cosmics data
  }

  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if (newRun) {
 
    // Create the directories & MonGroups for the histograms
    // Each directory has its own MonGroup

    std::string zeeDir(m_rootDir + "/ZeeEfficiencies");
	std::string AllProbeDir(zeeDir+"/All_Probes");
	std::string PassProbeDir(zeeDir+"/Passing_Probes");
	std::string EtEffDir(zeeDir+"/Et_Efficiencies");
	std::string EtaEffDir(zeeDir+"/Eta_Efficiencies");
	std::string PhiEffDir(zeeDir+"/Phi_Efficiencies");
    MonGroup monZee(this, zeeDir, run, attr);
	MonGroup monAllProbe(this,AllProbeDir,run,attr);
	MonGroup monPassProbe(this,PassProbeDir,run,attr);
	MonGroup monEtEff(this,EtEffDir,run,attr);
	MonGroup monEtaEff(this,EtaEffDir,run,attr);
	MonGroup monPhiEff(this,PhiEffDir,run,attr);
        
    // Book the histograms 
    // Book the Zee efficiency histograms
    
    m_histTool->setMonGroup(&monZee);
    
	m_h_z_inv_mass = m_histTool->book1F("z_inv_mass","Z Invariant Mass; Z_{ee} Invariant Mass [GeV]; Entries/GeV",40,71,111);

    // etc...
	m_histTool->setMonGroup(&monAllProbe);
    m_h_probe_eta_tot = m_histTool->book1F("probe_eta_tot", "All Probes Eta;Probe Electron #eta; Entries",probe_eta_bins,probe_eta_min,probe_eta_max);
    m_h_probe_phi_tot = m_histTool->book1F("probe_phi_tot", "All Probes Phi;Probe Electron #phi; Entries", probe_phi_bins,probe_phi_min,probe_phi_max);
    m_h_probe_Et_tot = m_histTool->book1F("probe_Et_tot","All Probes Et; Probe Electron E_{T} [GeV]; Entries/5GeV",probe_et_bins,probe_et_min,probe_et_max);
    


    TrigConf::L1DataDef def;
    if (m_configSvc->ctpConfig()) {
      const std::vector<TrigConf::TriggerThreshold*>& thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());
      std::vector<TrigConf::TriggerThreshold*>::const_iterator it;  
      
      for (it = thresholds.begin(); it!= thresholds.end(); ++it) {
	if ( (*it)->type() == def.emType() ) {
	  std::string threshName   = (*it)->name();
	  int threshNumber = (*it)->thresholdNumber();
	  m_histTool->setMonGroup(&monPassProbe);
	  m_h_probe_eta_pass[threshNumber]=m_histTool->book1F("eta_"+threshName,"Passing Probes Eta: "+threshName+";Probe Electron #eta ;Entries", probe_eta_bins,probe_eta_min,probe_eta_max);
	  m_h_probe_phi_pass[threshNumber]=m_histTool->book1F("phi_"+threshName,"Passing Probes Phi: "+ threshName+";Probe Electron #phi ;Entries", probe_phi_bins,probe_phi_min,probe_phi_max);
	  m_h_probe_Et_pass[threshNumber]=m_histTool->book1F("Et_"+threshName,"Passing Probes Et: "+threshName+";Probe Electron E_{T} [GeV];Entries/5GeV", probe_et_bins,probe_et_min,probe_et_max);
	  m_histTool->setMonGroup(&monEtaEff);
	  m_h_eta_eff[threshNumber]=m_histTool->book1F("eff_eta_"+threshName,"Eta Eff: "+threshName+";Probe Electron #eta ;Efficiency",probe_eta_bins,probe_eta_min,probe_eta_max);
	  m_histTool->setMonGroup(&monPhiEff);
	  m_h_phi_eff[threshNumber]=m_histTool->book1F("eff_phi_"+threshName,"Phi Eff: "+threshName+";Probe Electron #phi ;Efficiency",probe_phi_bins,probe_phi_min,probe_phi_max);
	  m_histTool->setMonGroup(&monEtEff);
	  m_h_Et_eff[threshNumber]=m_histTool->book1F("eff_et_"+threshName,"Et Eff: "+threshName+";Probe Electron E_{T} [GeV];Efficiency",probe_et_bins,probe_et_min,probe_et_max);
	}       
      } 
    }

    // Unset the MonGroup when all histograms are booked

    m_histTool->unsetMonGroup();
    m_histBooked = true;

  } // End if (newRun)

  msg(MSG::DEBUG) << "Leaving bookHistogramsRecurrent" << endmsg;

  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------*/
StatusCode TagProbeEfficiencyMon::fillHistograms()
/*----------------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histograms not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt
  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }


//// .............
/// ELECTRONS
//// .............

  const xAOD::ElectronContainer* eles = 0;
  StatusCode sc;
  sc = evtStore()->retrieve(eles, "Electrons");
  if (sc.isFailure() || !eles) {
    if (debug) msg(MSG::DEBUG) << "No xAOD Electron container found" << endmsg;
  }

  TLorentzVector probeLV1; 
  TLorentzVector tagLV;
  
  if (eles) {
    xAOD::ElectronContainer::const_iterator ele_itr = eles->begin();
    xAOD::ElectronContainer::const_iterator ele_end = eles->end();
//// ...........
/// TAG
//// ...........
    int tag_itr = -1;
    int tag_index = -1;
    int tag_pt_max = 0;
    ele_itr = eles->begin();   
    for( ; ele_itr != ele_end; ++ele_itr) {
      tag_index +=1;
      if ((*ele_itr)->passSelection("LHMedium")==0) continue; 
      if (abs((*ele_itr)->eta()) > 2.47) continue;
      if (abs((*ele_itr)->eta())>1.37 and (abs((*ele_itr)->eta())<1.52)) continue;
      if ((*ele_itr)->pt() > lowcutTag and (*ele_itr)->pt()>tag_pt_max){ //set tag
        tag_pt_max=((*ele_itr)->pt());
        tagLV.SetPtEtaPhiM((*ele_itr)->pt(),(*ele_itr)->eta(),(*ele_itr)->phi(),(*ele_itr)->m());
        tag_itr = tag_index;
        tag_charge = (*ele_itr)->charge();
      }//Otherwise tag_itr = -1 so we can skip the event!
    } 

//// ...........
/// PROBE
//// ...........
    probelist.clear();
    int probe_itr = -1;
    ele_itr = eles->begin();
    for( ; ele_itr != ele_end; ++ele_itr) {
      probe_itr +=1;
      if (tag_itr == -1) break; //no tag, no reason to check for probes
      if (probe_itr == tag_itr) continue; // if same electron, probe cannot be same as tag
      if ((*ele_itr)->charge()==tag_charge) continue; //cant have same charge
      if ((*ele_itr)->pt() < lowcutProbe) continue;
      if (abs((*ele_itr)->eta()) > 2.47) continue;  //Eta forward
      if (abs((*ele_itr)->eta())>1.37 and (abs((*ele_itr)->eta())<1.52)) continue; //Eta Crack
      probeLV.SetPtEtaPhiM((*ele_itr)->pt(),(*ele_itr)->eta(),(*ele_itr)->phi(),(*ele_itr)->m());
      combLV = tagLV+probeLV;
      if (abs(m_z - combLV.M())>inv_m_range) continue;  //Outside of invariant mass range around the Z, passes all checks
      probelist.push_back (probe_itr);
    }

//// ...........
/// Analysis using Tags and Probes
//// ...........
    probe_check = -1;
    float tmpM =10000000.;
    ele_itr = eles->begin();
    for(; ele_itr != ele_end; ++ele_itr) { //Probe selection (for >1probe)
      probe_check +=1; 
      for(uint i=0; i<probelist.size();i++){ 
        if (probelist[i]==probe_check){
          tmpProbe.SetPtEtaPhiM((*ele_itr)->pt(),(*ele_itr)->eta(),(*ele_itr)->phi(),(*ele_itr)->m());
          combLV1 = tagLV+tmpProbe;
          if ((abs(m_z - combLV1.M())) <tmpM){
            tmpM =(abs(m_z - combLV1.M()));// Want smallest (closest to Zmass) 
            probeLV1.SetPtEtaPhiM((*ele_itr)->pt(),(*ele_itr)->eta(),(*ele_itr)->phi(),(*ele_itr)->m());
          }
        }   
      }
    }
    
  }//end if{eles}, have found Tag and Probe Electrons, probeLV1 will be used now

  if (probeLV1.Pt() != 0){
    // This will only happen if we have a legitimate probe
    // Retrieve CPM TOB RoIs from StoreGate
    const xAOD::CPMTobRoIContainer* cpmTobRoiTES = 0;
    sc = evtStore()->retrieve(cpmTobRoiTES, m_cpmTobRoiLocation);
    if (sc.isFailure() || !cpmTobRoiTES) {
      if (debug) msg(MSG::DEBUG) << "No CPM TOB RoI container found in TES: "
				 << m_cpmTobRoiLocation << endmsg;
    }
 
  // Loop over the CPM TOB RoIs and fill histograms
    std::vector<double> CPMProbeValues (2);//Will contain [Energy,Isolation]  Possibly bad style, easiest way to include it for later

    if (cpmTobRoiTES) {
      LVL1::CPRoIDecoder decoder;
      xAOD::CPMTobRoIContainer::const_iterator crIterator    = cpmTobRoiTES->begin();
      xAOD::CPMTobRoIContainer::const_iterator crIteratorEnd = cpmTobRoiTES->end();
      
            
      for(; crIterator != crIteratorEnd; ++crIterator) {
	const int type   = (*crIterator)->type();           // 0=EM, 1=TAU
	const int energy = (*crIterator)->energy();
	const int isolation = (*crIterator)->isolation();
	const LVL1::CoordinateRange coord(decoder.coordinate((*crIterator)->roiWord()));
	const double eta = coord.eta();
	const double phi = coord.phi();
	double delRmintmp = 1000; 
	// Go through EM CPMRoIs to calculate which is closest to the probe
	if (energy) {
	  if (type == 0) {   // EM
	    float delRtmp = calcDeltaR(eta,phi,probeLV1.Eta(), probeLV1.Phi()) ;
	    if (delRtmp < delRmintmp){
	      delRmintmp = delRtmp;
	      if (delRtmp < 0.15) {
		CPMProbeValues[0] = energy; //Sets value if our probe is within DeltaR 0.15 of a CPMTobRoI, only will continue if we have a match
		CPMProbeValues[1] = isolation;
	      }
	    }
	  }
	}
    } // End of loop over CPM TOB RoI container 

    if ( CPMProbeValues[0] > 0) {  
      combLV1 = tagLV + probeLV1;
      m_h_probe_eta_tot->Fill(probeLV1.Eta());
      m_h_probe_phi_tot->Fill(probeLV1.Phi());
      m_h_probe_Et_tot->Fill(probeLV1.Et()*0.001);
      m_h_z_inv_mass->Fill(combLV1.M()*0.001);
      //This Fills the all probe catagories, now we will fill each passing threshold hist:
      TrigConf::L1DataDef def;
      if (m_configSvc->ctpConfig()) {
        const std::vector<TrigConf::TriggerThreshold*>& thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());
        std::string threshName;
        std::vector<TrigConf::TriggerThreshold*>::const_iterator it; 
		/// Now lets check against which thresholds are passed by grabbing the information from the Trigger Menu
        for(it = thresholds.begin(); it!=thresholds.end();++it){
			if ( (*it)->type() != def.emType() ) continue;
			int Eta = (probeLV1.Eta() >=0.)?(int)(probeLV1.Eta()*10.+0.5):(int)(probeLV1.Eta()*10.-0.5);
			double tmpPhi = TVector2::Phi_0_2pi(probeLV1.Phi());
			int Phi = (tmpPhi >=0.)?(int)(tmpPhi*10.+0.5):(int)(tmpPhi*10.-0.5);
			TrigConf::TriggerThresholdValue* ttv = (*it)->triggerThresholdValue(Eta,Phi);//Can move this to use actual values
			TrigConf::ClusterThresholdValue* ctv = dynamic_cast< TrigConf::ClusterThresholdValue* >( ttv );
			if (ctv){
				float cpScale = m_configSvc->thresholdConfig()->caloInfo().globalEmScale();
				int etCut = ctv->ptcut()*cpScale;
				unsigned int isolMask = ctv-> isolationMask();
				bool isolationPassed = true;
				for (unsigned int bit = 0; bit < LVL1::TrigT1CaloDefs::numOfIsolationBits; ++bit)
					if ( (isolMask & (1<<bit)) && !((int)CPMProbeValues[1] & (1<<bit)) ) isolationPassed = false;
				if ( CPMProbeValues[0] > etCut && isolationPassed ) {
					int num = ( *it )->thresholdNumber();
					if (num < 16){
						m_h_probe_eta_pass[num]->Fill(probeLV1.Eta());
						m_h_probe_phi_pass[num]->Fill(probeLV1.Phi());
						m_h_probe_Et_pass[num]->Fill(probeLV1.Et()*0.001);
					}
				}
			}
		}
      }
    } //end if CPMProbeValues[0]>0, has nonzero energy 
  }// End if (cpmTobRoiTES)
    
}//end if (probeLV1.Pt() != 0)
  msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;
  

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TagProbeEfficiencyMon::procHistograms()
/*---------------------------------------------------------*/
{
  // Stuff that happens at the end of a run or LB goes here
  // In practice, this isn't used much

  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  if (endOfLumiBlock) {
  }
  if (endOfRun){
    TrigConf::L1DataDef def;
    if (m_configSvc->ctpConfig()) {
      const std::vector<TrigConf::TriggerThreshold*>& end_thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());  
      std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
      int threshNumber;
      for (it = end_thresholds.begin(); it!= end_thresholds.end(); ++it) {
	if ( (*it)->type() == def.emType() ) {
	  threshNumber = (*it)->thresholdNumber();
	  m_histTool->efficienciesForMerge(m_h_probe_eta_tot,m_h_probe_eta_pass[threshNumber],m_h_eta_eff[threshNumber]);
	  m_histTool->efficienciesForMerge(m_h_probe_phi_tot,m_h_probe_phi_pass[threshNumber],m_h_phi_eff[threshNumber]);
	  m_histTool->efficienciesForMerge(m_h_probe_Et_tot,m_h_probe_Et_pass[threshNumber],m_h_Et_eff[threshNumber]);
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
// Add private functions here 
/*---------------------------------------------------------*/

//------------------------------------------------------------------
//Calculate delta R quickly between two objects:adopted from EmEfficienciesMonTool
//------------------------------------------------------------------
  
double TagProbeEfficiencyMon::calcDeltaR(double eta1, double phi1, double eta2, double phi2) {
  double dEta = eta1 - eta2;
  double dPhi = correctDeltaPhi(phi1 - phi2);
  double dR = sqrt((dEta * dEta) + (dPhi * dPhi));
  return dR;
}
//------------------------------------------------------------------
//Correct the value of deltaPhi so that it falls in +-M_PI range
//------------------------------------------------------------------
double TagProbeEfficiencyMon::correctDeltaPhi(double dPhi) {
  if (abs(dPhi)> M_PI) {
  dPhi = (dPhi > 0)? dPhi-2 * M_PI : dPhi + 2 * M_PI;
  }
  return dPhi;
}


// ============================================================================
} // end namespace
// ============================================================================

