/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//---------------------------------------------------------------------------
//  $Id: TrigTileLookForMuAlg.cxx,v 1.34 2009-04-05 04:24:07 ssnyder Exp $
//   
// Author   : G Usai
//  
//---------------------------------------------------------------------------

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "RegionSelector/IRegSelSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "CaloIdentifier/TileID.h"
#include "ByteStreamData/ROBData.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigTileMuId/TrigTileLookForMuAlg.h"

class ISvcLocator;



TrigTileLookForMuAlg::TrigTileLookForMuAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("LowerTresh0MeV", m_th0d=150.0); 
  declareProperty("LowerTresh1MeV", m_th1d=150.0);
  declareProperty("LowerTresh2MeV", m_th2d=150.0);
  declareProperty("LowerTreshScinMeV", m_thitcd=160.0);
  declareProperty("UpperTresh2MeV", m_thres2);
  declareProperty("UpperTresh1MeV", m_thres1);
  declareProperty("UpperTresh0MeV", m_thres0);
  declareProperty("From3to2", m_cell32);
  declareProperty("From2to1", m_cell21);
  declareProperty("LooseSelection", m_LooseSelection=true); 
  declareProperty("ReadRoIsFromL1", m_ReadRoIsFromL1=false);
  declareProperty("DelEta_ForRoIs", m_DelEta_ForRoIs=0.1);
  declareProperty("DelPhi_ForRoIs", m_DelPhi_ForRoIs=0.2);

  // Declare monitored variables
  declareMonitoredVariable("NTileMu", m_allNMuon);
  declareMonitoredVariable("NCellsAccess", m_NCellsAccess);
  declareMonitoredVariable("EffEvtswRoI", m_EffEvtswRoI);
  declareMonitoredVariable("ConversionErrors", m_conversionError);
  declareMonitoredVariable("AlgorithmErrors", m_algorithmError);

  // Declare monitored containers
  declareMonitoredStdContainer("EtaTileMu", m_allEta);
  declareMonitoredStdContainer("PhiTileMu", m_allPhi);
  declareMonitoredStdContainer("ETileMu", m_allEne);
  declareMonitoredStdContainer("EtaTileMuLoose", m_allEta_LS);
  declareMonitoredStdContainer("PhiTileMuLoose", m_allPhi_LS);
  declareMonitoredStdContainer("ETileMuLoose", m_allEne_LS);
  declareMonitoredStdContainer("EffEvtswRoI_LimitedEta",m_EffEvtswRoI_LimitedEta);
  declareMonitoredStdContainer("EtaTileMu_wRoI", m_EtaTileMu_wRoI);
  declareMonitoredStdContainer("EtaRoI_wTileMu", m_EtaRoI_wTileMu);
  declareMonitoredStdContainer("PhiTileMu_wRoI", m_PhiTileMu_wRoI);
  declareMonitoredStdContainer("PhiRoI_wTileMu", m_PhiRoI_wTileMu);
  declareMonitoredStdContainer("EtaRoI_NoTileMu", m_EtaRoI_NoTileMu);
  declareMonitoredStdContainer("PhiRoI_NoTileMu", m_PhiRoI_NoTileMu);
}



TrigTileLookForMuAlg::~TrigTileLookForMuAlg()
{
}



HLT::ErrorCode TrigTileLookForMuAlg::hltInitialize()
{
  // Timers
  m_timerTotal = addTimer("Total");
  m_timerLink = addTimer("Link");
  m_timerCandSearch = addTimer("CandSearch");
  m_timerLoopOnCand = addTimer("LoopOnCand");
  m_timerLoopOnCellCollections = addTimer("LoopOnCellCollections");

  if ( detStore()->retrieve(m_tileID).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve TileID helper from DetectorStore");
    return HLT::BAD_JOB_SETUP;
  }

  IToolSvc* toolSvc;
  if(service("ToolSvc",toolSvc).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve IToolSvc. Exiting!");
    return HLT::TOOL_FAILURE;
  }

  if (toolSvc->retrieveTool("TrigDataAccess", m_data).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve TrigDataAccess");
    return HLT::TOOL_FAILURE;
  }

  // Define a numbering scheme for the cells  
  double f;
  unsigned int i=0; 
  for (f = -1.2; f <1.3 ; f += 0.2) { 
    m_eta2[i] = f;
    i=i+1;
  }

  i=0;
  for (f = -1.45; f<1.5; f += 0.1) { 
    m_eta1[i] = f;
    i++;
  }

  i=0;
  for (f = -1.55; f<1.6; f += 0.1){
    if ( fabs(f) > (1.05-0.001) && fabs(f) < (1.05+0.001)){ 
      ATH_MSG_VERBOSE("SKIP for A-11 or A+11 cell! eta=" << f);
    } else {
      m_eta0[i] = f;
      ATH_MSG_VERBOSE("Fill m_eta0[i]=" << m_eta0[i] << "[" << i << "]");
      i++;
    } 
  } 

  m_deta2 = 0.1;
  m_deta1 = 0.05;

  ATH_MSG_DEBUG("TrigTileLookForMuAlg initialization completed");
  
  return HLT::OK;
}



HLT::ErrorCode TrigTileLookForMuAlg::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputVector, unsigned int type_out)
{    
  // total execution time 
  if ( timerSvc() ) m_timerTotal->start();

  ATH_MSG_DEBUG("exec HLTAlgorithm started");

  m_error=0x0;

  const double etamin = -1.609430;
  const double etamax =  1.609430;
  const double phimin =  0.0;       //-3.15;
  const double phimax =  6.283185;  // 3.15;

  const double User_Pi = 3.141592654;
  const int NMax_Muons = 30;  
  int NRoIs = 0;                        // RoIs from |eta| < 1.7
  int NRoIs_LimitedEta = 0;             // RoIs from |eta| < 1.4 
  float Eta_RoIs[NMax_Muons], Phi_RoIs[NMax_Muons];
  float Eta_Mus[NMax_Muons],  Phi_Mus[NMax_Muons];
  double Eta_MinMax[2][NMax_Muons], Phi_MinMax[2][NMax_Muons];
  int here_NCellsAccess = 0;

  double m0[30][64] = {{0}};  
  double m1[30][64] = {{0}}; 
  double m2[13][64] = {{0}}; 

  //int Flg_TrigFire[2] = {0}; // 1:From TrigTileMu  2:From Cosmic

  // monitored variables
  beforeExecMonitors().ignore();

  m_allNMuon = 0;
  m_NCellsAccess = 0;
  m_EffEvtswRoI = 0.0;

  m_allEta.clear();
  m_allPhi.clear();
  m_allEne.clear();
  m_allEta_LS.clear();
  m_allPhi_LS.clear();
  m_allEne_LS.clear();
  m_EffEvtswRoI_LimitedEta.clear();
  m_EtaTileMu_wRoI.clear();
  m_EtaRoI_wTileMu.clear();
  m_PhiTileMu_wRoI.clear();
  m_PhiRoI_wTileMu.clear();
  m_EtaRoI_NoTileMu.clear();
  m_PhiRoI_NoTileMu.clear();

  int j;
  double ph[64]={0};

  if ( timerSvc() ) m_timerLink->start();

  // ----------- Try to read RoIs from L1_RPC (by HL, 2008-12-12) -----------//
  if (m_ReadRoIsFromL1) {

    ATH_MSG_DEBUG("Found " << inputVector.size() << " TE vectors");

    if ( inputVector.size() != 1 ) {
      ATH_MSG_WARNING("inputVector.size()(=" << inputVector.size()
               << ") != 1, Please check TE from LvL1!!" );
    }

    int ii=0;
    for (std::vector<HLT::TEVec>::const_iterator it = inputVector.begin();it != inputVector.end(); ++it) {

      ATH_MSG_DEBUG("TE vector " << ii++ << "contains " << (*it).size() << " TEs");

      for (HLT::TEVec::const_iterator inner_it = (*it).begin();inner_it != (*it).end(); ++inner_it) {

        const TrigRoiDescriptor* roiDescriptor = 0;

        HLT::ErrorCode sc = getFeature((*inner_it),roiDescriptor, "");
        if (sc != HLT::OK) {
          ATH_MSG_ERROR(" Failed to find RoiDescriptor ");
          return HLT::BAD_JOB_SETUP;
        }

	ATH_MSG_DEBUG(" REGTEST: RoI id " << roiDescriptor->roiId()
		 << " located at phi = " << roiDescriptor->phi()
                 << ", eta = " << roiDescriptor->eta());

	Eta_RoIs[NRoIs] = roiDescriptor->eta();
	Phi_RoIs[NRoIs] = roiDescriptor->phi();

	double now_eta = roiDescriptor->eta();
	double now_phi = roiDescriptor->phi();
	if (now_phi < 0) now_phi = now_phi + 2*User_Pi; // phi --> 0 ~ 2pi

	double now_etamin = now_eta - m_DelEta_ForRoIs;
	double now_etamax = now_eta + m_DelEta_ForRoIs;
	double now_phimin = now_phi - m_DelPhi_ForRoIs;
	double now_phimax = now_phi + m_DelPhi_ForRoIs;

	if (now_phimin < 0.)        { now_phimin = now_phimin + 2*User_Pi; }
	if (now_phimax > 2*User_Pi) { now_phimax = now_phimax - 2*User_Pi; }

	/*	--> For Half-Scan method <--
          if ( now_eta < 0) {
	  now_etamax = std::max(0.0,    now_etamax);
	  now_etamin = std::min(etamin, now_etamin);
          } else {
	  now_etamax = std::max(etamax, now_etamax);
	  now_etamin = std::min(0.0,    now_etamin);
          } */

	if ( fabs(now_eta) > 0.6 && fabs(now_eta) < 1.2 ) {
	  if (now_eta<0) { now_etamin = etamin;  now_etamax = -0.2; }
	  if (now_eta>0) { now_etamin = 0.2;     now_etamax = etamax; }
	} 

	ATH_MSG_DEBUG("For DetROBIDListUint, EtaMin=" << now_etamin
		 << " EtaMax=" << now_etamax
		 << " PhiMin=" << now_phimin
		 << " PhiMax=" << now_phimax);

	Eta_MinMax[0][NRoIs] = now_etamin;
	Eta_MinMax[1][NRoIs] = now_etamax;
	Phi_MinMax[0][NRoIs] = now_phimin;
	Phi_MinMax[1][NRoIs] = now_phimax;

	NRoIs++;
	if ( fabs(roiDescriptor->eta()) < 0.7 ) NRoIs_LimitedEta++;

      }

    }

  } else {

    Eta_MinMax[0][NRoIs] = etamin;
    Eta_MinMax[1][NRoIs] = etamax;
    Phi_MinMax[0][NRoIs] = phimin;
    Phi_MinMax[1][NRoIs] = phimax;
    NRoIs++;
    
  } 

  // Stop timer
  if ( timerSvc() ) m_timerLink->stop();

  // Start timer of loop
  if ( timerSvc() ) m_timerLoopOnCellCollections->start();

  for(int iRoI=0; iRoI<NRoIs; iRoI++) {

    m_data->RegionSelector(0, Eta_MinMax[0][iRoI], Eta_MinMax[1][iRoI], Phi_MinMax[0][iRoI], Phi_MinMax[1][iRoI], TILE);

    for(unsigned int iR=0; iR!=m_data->TileContSize();++iR) {

      if(m_data->LoadCollections(m_itBegin,m_itEnd,iR,!iR).isFailure()){
        ATH_MSG_FATAL("Error LoadCollections");
        return HLT::TOOL_FAILURE;
      }
      m_error|=m_data->report_error();
      if ( 0x0FFFFFFF & m_error ) m_conversionError++;
      if ( 0xF0000000 & m_error ) m_algorithmError++;
      if( m_error ) {
        ATH_MSG_DEBUG("Monitoring error found");
        return HLT::ERROR;
      }

      // Iterate over all cells
      for(m_itT = m_itBegin;m_itT != m_itEnd; ++m_itT) {      

        here_NCellsAccess++;

        const TileCell* nowcell = (*m_itT);
 
	ATH_MSG_VERBOSE("Collection info: ID: " << nowcell->ID()
		 << "  eta: " << nowcell->eta()
		 << "  phi: " << nowcell->phi()
		 << "  module nr: " << m_tileID->module(nowcell->ID())
		 << "  Energy: " << nowcell->energy()
		 << "  CellTy: " <<  m_tileID->sample(nowcell->ID()));

        // Fill Matrix (to be optimized) 

        // D cells
        if(m_tileID->sample(nowcell->ID()) == 2) {
	  for(j=0;j<13;++j) {
	    if(nowcell->eta()<(m_eta2[j]+m_deta2) && nowcell->eta()>(m_eta2[j]-m_deta2)) {
	      m2[j][m_tileID->module(nowcell->ID())] = nowcell->energy();
	      ph[m_tileID->module(nowcell->ID())] = nowcell->phi();
	    }
	  }
        }

        // BC cells
        if(m_tileID->sample(nowcell->ID()) == 1) { 
	  for(j=0;j<30;++j) {
	    if(nowcell->eta()<(m_eta1[j]+m_deta1) && nowcell->eta()>(m_eta1[j]-m_deta1)) {	      
	      m1[j][m_tileID->module(nowcell->ID())] = nowcell->energy();
	    }
	  }
        }

        // A cells
        if(m_tileID->sample(nowcell->ID()) == 0) {
	  for(j=0;j<30;++j) {
	    if(nowcell->eta()<(m_eta0[j]+m_deta1) && nowcell->eta()>(m_eta0[j]-m_deta1)) {
	      m0[j][m_tileID->module(nowcell->ID())] = nowcell->energy();
	    }
	  } 
        }

        // ITC 
        if(m_tileID->sample(nowcell->ID()) == 3) {
	  for(j=4;j<30;j+=21) {
	    if(nowcell->eta()<=(m_eta0[j]+m_deta2) && nowcell->eta()>=(m_eta0[j]-m_deta2)) {
	      m0[j][m_tileID->module(nowcell->ID())] = nowcell->energy();
	    }
	  }
        }

      } // end loop over cells
    } // end loop over superdrawers
  } // end loop over RoI
 
  // Stop timer of loop
  if ( timerSvc() ) m_timerLoopOnCellCollections->stop();

  // Start timer of muon search 
  if ( timerSvc() ) m_timerCandSearch->start();
  
  int    i, cand=0, ntag=0;
  int    phc3[30]={0}, etc3[30]={0}, splited[30]={0};
  double tpeta3[30]={0}; 

  // Energy
  double enedp3[30]={0};
  double enedp2[30]={0};
  double enedp1[30]={0};

  // Quality flags
  int cquality[30]={0};
  int flquality[30]={0};
  int aquality[30]={0}; 

  //array to be changed  with  std::vector as in offline 
  // Apr. 22, 2008 (H. Lim) : double --> float
  std::vector<float> enetr(4);
  float etatr=0; 
  float phitr=0;

  // define candidates
  for ( j=0; j<64; ++j) {

    for (i=0;i<13; ++i) {

      bool OK_HighThr = true;
      if (!m_LooseSelection && (m2[i][j] >= m_thres2[i])) OK_HighThr = false;
	
      if ( (m2[i][j]>= m_th2d) && OK_HighThr ) {

	phc3[cand] = j;
	etc3[cand] = i;
	splited[cand]=-1;
	enedp3[cand]=m2[i][j];
        cquality[cand] = (m2[i][j] < m_thres2[i] ? 0:1);

	ATH_MSG_VERBOSE(" found a muon candidate=  " << cand   
                        << "i= " << i
                        << ",j= " << j
                        << ",ene= " << m2[i][j]
                        << " m_thres2= " << m_thres2[i]);

        cand++;
      }
      if(cand >= NMax_Muons) break;
    }
    if(cand >= NMax_Muons) break;
  }

  m_NCellsAccess = std::min(here_NCellsAccess, 599); 

  // Repaire for muons splitted between cells in the 3rd layer 

  int nsplit=0;

  for ( i=0; i<cand; ++i ) {

    tpeta3[i] = m_eta2[etc3[i]];

    for (j=0;j<=i-1;++j) {
      if ( (cquality[i]+cquality[j]) <= 1 ) {
        if ( ( abs(phc3[i]-phc3[j]) == 0 ) && // same phi bin
	     ( abs(etc3[i]-etc3[j]) <= 1 ) && // neighbor's eta bin 
	     ( (m2[etc3[i]] [phc3[i]] + m2[etc3[j]] [phc3[j]]) < ( (m_thres2[etc3[i]] > m_thres2[etc3[j]]) ? m_thres2[etc3[i]] : m_thres2[etc3[j]] ) ) ) {
	  splited[i] = j;
	  enedp3[i] = m2[etc3[i]] [phc3[i]] + m2[etc3[j]] [phc3[j]];
	  tpeta3[i] = (m_eta2[etc3[i]] + m_eta2[etc3[j]])/2;
	  nsplit = nsplit+1;
        }
      }
    }

  } // end loop on candidates
  
  if ( timerSvc() ) m_timerCandSearch->stop();
  
  if ( timerSvc() ) m_timerLoopOnCand->start();

  // loop on the candidates
  
  int ci3, found[30]={0};  

  for(ci3=0; (ci3<cand && splited[ci3]<0) || ((ci3<cand && splited[ci3]>=0) && found[splited[ci3]]==0) ;++ci3) {

    found[ci3] = 0;
    j = phc3[ci3];

    ATH_MSG_VERBOSE("loop on  muon candidates: ci3,j " << ci3 << ", " << j);

    int ksto2=m_cell32[6*etc3[ci3]+0];

    for (int ks=1; ks<=ksto2 && found[ci3]!=1; ++ks) {

      int k2 = m_cell32[6*etc3[ci3]+ks];  

      bool OK_HighThr = true;
      if (!m_LooseSelection && (m1[k2][j]>= m_thres1[k2]) ) OK_HighThr = false;
 
      if (m1[k2][j]>m_th1d && OK_HighThr) {

        flquality[ci3] = (m1[k2][j] < m_thres1[k2] ? 0:1);
	enedp2[ci3] = m1[k2][j];

	int j1 = j;
	int ksto1 = m_cell21[6*k2+0];    

	for(int kp=1; kp<=ksto1 && found[ci3]!=1; ++kp) {

	  int k1 = m_cell21[6*k2+kp];  

          OK_HighThr = true;
          if (!m_LooseSelection && (m0[k1][j1]>=m_thres0[k1]) ) OK_HighThr=false;

          if ( ( ( (k1!=4 || k1!=25) && m0[k1][j1]>m_th0d ) || ( (k1==4 || k1==25) && m0[k1][j1]>m_thitcd ) ) && OK_HighThr ) { 

	    enedp1[ci3] = m0[k1][j1];
	    aquality[ci3]=(m0[k1][j1] < m_thres0[k1] ? 0:1);  

	    if ( cquality[ci3]+flquality[ci3]+aquality[ci3] <= 1 ) { // We have some tracks muon like

	      found[ci3] = 1;
	      etatr = (tpeta3[ci3]+m_eta1[k2]+m_eta0[k1])/3;
	      phitr = ph[j1];
	      enetr.at(0) = enedp1[ci3];
	      enetr.at(1) = enedp2[ci3];
	      enetr.at(2) = enedp3[ci3];
	      enetr.at(3) = enedp1[ci3]+enedp2[ci3]+enedp3[ci3]; // temporary

	      // Quality flag for tight and loose selection
              float qf = cquality[ci3]+flquality[ci3]+aquality[ci3];  

	      // Fill TileMuFeature
	      TileMuFeature* muon = new TileMuFeature(etatr, phitr, enetr, qf+1000.);

              Eta_Mus[ntag] = etatr;
              Phi_Mus[ntag] = phitr;

	      ntag = ntag+1;

	      ATH_MSG_DEBUG("Muon tagged:  Ntag = " << ntag
                       << "  Etatag = " << etatr
		       << "  Phitag = " << phitr
                       << "  EnergyVec[0] = " << enetr.at(0)
                       << "  EnergyVec[1] = " << enetr.at(1)
                       << "  EnergyVec[2] = " << enetr.at(2)
                       << "  EnergyVec[3] = " << enetr.at(3)
                       << "  qaulityfactor= " << qf);
                
	      // Fill histograms for tight and loose selection
	      m_allEta.push_back(etatr);
	      m_allPhi.push_back(phitr);
              m_allEne.push_back(enetr.at(3)/1000.); // MeV --> GeV

	      // Fill histograms for loose selection
	      if(qf > 0.) {
	        m_allEta_LS.push_back(etatr);
	        m_allPhi_LS.push_back(phitr);
		m_allEne_LS.push_back(enetr.at(3)/1000.); // MeV --> GeV
              }

	      // roiDescriptor position
	      double eta = etatr;
	      double phi = phitr;
	      float roiwidth=0.1;
	      TrigRoiDescriptor *tmproi = new
		TrigRoiDescriptor(eta, eta-roiwidth,eta+roiwidth,
				  phi, phi-roiwidth,phi+roiwidth);

	      // Note: addRoI is defined in AllTEAlgo base class
	      HLT::TriggerElement* te = addRoI(type_out, tmproi);

	      // Set this trigger element to be active
	      te->setActiveState(true);
	      //Flg_TrigFire[0]	= 1;

	      // (2007-07-12, HL) For testing recording TE 
              HLT::ErrorCode HLTStatus=attachFeature(te, muon);
	      if( HLTStatus != HLT::OK ) {
	        ATH_MSG_ERROR("REGTEST Write of TrigTileLookForMu feature into outputTE failed"); 
	        return HLTStatus;
	      } else {
		ATH_MSG_DEBUG("Write of TrigTileLookForMu feature into outputTE OK!"); 
              }

            } // end if QF <= 1
	  } // end if m0 is in the range
	} // end loop on pattern 1st layer
      } // end if m1 is in the range
    } // end loop on pattern 2nd layer
  } // end loop on candidates

  // Fill monitored variable for TileCal muon candidates
  m_allNMuon = ntag;

  if ( m_ReadRoIsFromL1 ) { // Running seeded by a L1 Muon RoI instead of unseeded mode

    // Fill monitored variables for computing TileMuId efficiency wrt L1 in |eta|<0.7
    if( NRoIs_LimitedEta > 0) {
      if( ntag > 0 ) {
	m_EffEvtswRoI_LimitedEta.push_back( 1.0 );
      }else{
	m_EffEvtswRoI_LimitedEta.push_back( 0.0 );
      }
    }
 
    if ( ntag > 0 ) { // TileCal muon candidates found (ntag > 0)

      m_EffEvtswRoI = 1.0;
      float DelRMin = 9999.9;
      int Id_RoI = -1, Id_Mu= -1;

      //////////////////////////////////////
      // Matching between TileCal and RoI //
      //////////////////////////////////////

      for(int iMu=0; iMu<ntag; iMu++) {

        for (int iRoI=0; iRoI<NRoIs; iRoI++) {

	  // Compute DeltaPhi between RoI and TileCal
          float DelPhi = Phi_Mus[iMu] - Phi_RoIs[iRoI];
          if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
          if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;

	  // Compute DeltaEta between RoI and TileCal
          float DelEta = Eta_Mus[iMu] - Eta_RoIs[iRoI];

	  // Compute DeltaR between RoI and TileCal
          float DelR = sqrt( pow(DelEta,2) + pow(DelPhi,2) );
          if (DelR < DelRMin) {
              Id_Mu = iMu;
              Id_RoI = iRoI;
              DelRMin = DelR;
          }

        }

      }

      ////////////////////////////////////////////
      // After matching between TileCal and RoI //
      ////////////////////////////////////////////

      if ( Id_RoI > -1 && Id_Mu > -1 ) { // Matching OK

	// Fill monitored variables when L1 Muon RoIs are confirmed in TileCal
        m_EtaTileMu_wRoI.push_back( Eta_Mus[Id_Mu] );
        m_EtaRoI_wTileMu.push_back( Eta_RoIs[Id_RoI] );
        m_PhiTileMu_wRoI.push_back( Phi_Mus[Id_Mu]);
        m_PhiRoI_wTileMu.push_back( Phi_RoIs[Id_RoI] );

	ATH_MSG_DEBUG("Matched Eta(TileMu)=" << Eta_Mus[Id_Mu]
	         << " Eta(RoI)=" << Eta_RoIs[Id_RoI]
		 << " Phi(TileMu)=" << Phi_Mus[Id_Mu]
		 << " Phi(RoI)=" << Phi_RoIs[Id_RoI]
		 << " DelR=" << DelRMin);

      } else { // No matching

	ATH_MSG_DEBUG("No Matching between TileMus and RoIs! Something Wrong?");

      }

    } else { // No TileCal muon candidates (ntag = 0)

      // Fill monitored variables when L1 Muon RoIs are not confirmed in TileCal
      for(int iRoI=0; iRoI<NRoIs; iRoI++) {
        m_EtaRoI_NoTileMu.push_back( Eta_RoIs[iRoI] );
        m_PhiRoI_NoTileMu.push_back( Phi_RoIs[iRoI] ); 
      } 

    } // end if ( ntag > 0 )

  } // end if ( m_ReadRoIsFromL1 )

  // Stop timers
  if ( timerSvc() ) m_timerLoopOnCand->stop();
  if ( timerSvc() ) m_timerTotal->stop();  

  // Stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;	   
}



HLT::ErrorCode TrigTileLookForMuAlg::hltFinalize()
{ 
  ATH_MSG_DEBUG("TrigTileLookForMuAlg finalization completed");

  return HLT::OK;
}
