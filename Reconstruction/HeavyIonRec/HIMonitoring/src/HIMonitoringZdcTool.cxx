/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */


#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoringZdcTool.h"
#include <xAODForward/ZdcModule.h>
#include <xAODForward/ZdcModuleContainer.h>

#include "TStyle.h"

#include "LWHists/TH2D_LW.h"

HIMonitoringZdcTool::
 HIMonitoringZdcTool(const std::string& type, const std::string& name,
                     const IInterface* parent) : ManagedMonitorToolBase(type, name, parent) {
}

HIMonitoringZdcTool::~HIMonitoringZdcTool() {
}

// Description: Used for rebooking unmanaged histograms
StatusCode HIMonitoringZdcTool::bookHistogramsRecurrent( ) {
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms
StatusCode HIMonitoringZdcTool::bookHistograms( ) {
  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  book_hist();

  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringZdcTool::fillHistograms() {
  StatusCode sc;

  const xAOD::ZdcModuleContainer* zdcs = 0;

  sc = evtStore()->retrieve(zdcs, "ZdcModules");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find Zdc");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Zdcs retrieved from StoreGate");
//		std::cout << "Number of zdcs " << zdcs->size() << std::endl;
  }

  xAOD::ZdcModuleContainer::const_iterator zdc_itr = zdcs->begin();
  xAOD::ZdcModuleContainer::const_iterator zdc_end = zdcs->end();

  for (; zdc_itr != zdc_end; ++zdc_itr) {
    if ((*zdc_itr)->type() != 0) continue;
    int thisSide = ((*zdc_itr)->side() == -1 ? 0 : 1);
    int imod = (*zdc_itr)->zdcModule();

    if (thisSide == 1 && imod == 0) m_NEW_AEM = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 1 && imod == 1) m_NEW_AHAD1 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 1 && imod == 2) m_NEW_AHAD2 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 1 && imod == 3) m_NEW_AHAD3 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 0 && imod == 0) m_NEW_CEM = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 0 && imod == 1) m_NEW_CHAD1 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 0 && imod == 2) m_NEW_CHAD2 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
    if (thisSide == 0 && imod == 3) m_NEW_CHAD3 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
/*
        if(thisSide==1&&imod==0)m_AEM =   ((*zdc_itr)->amplitude());
        if(thisSide==1&&imod==1)m_AHAD1 = ((*zdc_itr)->amplitude());
        if(thisSide==1&&imod==2)m_AHAD2 = ((*zdc_itr)->amplitude());
        if(thisSide==1&&imod==3)m_AHAD3 = ((*zdc_itr)->amplitude());
        if(thisSide==0&&imod==0)m_CEM =   ((*zdc_itr)->amplitude());
        if(thisSide==0&&imod==1)m_CHAD1 = ((*zdc_itr)->amplitude());
        if(thisSide==0&&imod==2)m_CHAD2 = ((*zdc_itr)->amplitude());
        if(thisSide==0&&imod==3)m_CHAD3 = ((*zdc_itr)->amplitude());
 */
    if (thisSide == 1 && imod == 0) m_G0AEM = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 1 && imod == 1) m_G0AHAD1 = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 1 && imod == 2) m_G0AHAD2 = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 1 && imod == 3) m_G0AHAD3 = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 0 && imod == 0) m_G0CEM = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 0 && imod == 1) m_G0CHAD1 = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 0 && imod == 2) m_G0CHAD2 = ((*zdc_itr)->amplitudeG0());
    if (thisSide == 0 && imod == 3) m_G0CHAD3 = ((*zdc_itr)->amplitudeG0());

    if (thisSide == 1 && imod == 0) m_G1AEM = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 1 && imod == 1) m_G1AHAD1 = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 1 && imod == 2) m_G1AHAD2 = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 1 && imod == 3) m_G1AHAD3 = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 0 && imod == 0) m_G1CEM = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 0 && imod == 1) m_G1CHAD1 = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 0 && imod == 2) m_G1CHAD2 = ((*zdc_itr)->amplitudeG1());
    if (thisSide == 0 && imod == 3) m_G1CHAD3 = ((*zdc_itr)->amplitudeG1());

    //	m_hamp_NEW[imod][thisSide]->Fill((*zdc_itr)->auxdataConst<float>("Amplitude"));
    //	m_hamp[imod][thisSide]->Fill((*zdc_itr)->amplitude());
    m_hamp[imod][thisSide]->Fill((*zdc_itr)->auxdataConst<float>("Amplitude"));
    m_hampG0[imod][thisSide]->Fill((*zdc_itr)->amplitudeG0());
    m_hampG1[imod][thisSide]->Fill((*zdc_itr)->amplitudeG1());

    //	m_hSumSideAmp_NEW[1]->Fill(m_NEW_AEM+m_NEW_AHAD1+m_NEW_AHAD2+m_NEW_AHAD3);
    //	hSumSideAmp_NEW[0]->Fill(m_NEW_CEM+m_NEW_CHAD1+m_NEW_CHAD2+m_NEW_CHAD3);
    //	m_hSumSideAmp[1]->Fill(m_AEM+m_AHAD1+m_AHAD2+m_AHAD3);
    //	hSumSideAmp[0]->Fill(m_CEM+m_CHAD1+m_CHAD2+m_CHAD3);


    //	if(thisSide==1) m_NEW_SumSideA = m_NEW_AEM+m_NEW_AHAD1+m_NEW_AHAD2+m_NEW_AHAD3;
    //	if(thisSide==0) m_NEW_SumSideC = m_NEW_CEM+m_NEW_CHAD1+m_NEW_CHAD2+m_NEW_CHAD3;
    //if(thisSide==1) m_SumSideA = m_AEM+m_AHAD1+m_AHAD2+m_AHAD3;
    //if(thisSide==0) m_SumSideC = m_CEM+m_CHAD1+m_CHAD2+m_CHAD3;
  }

  //if(thisSide==1) m_SumSideA = m_NEW_AEM+m_NEW_AHAD1+m_NEW_AHAD2+m_NEW_AHAD3;
  //if(thisSide==0) m_SumSideC = m_NEW_CEM+m_NEW_CHAD1+m_NEW_CHAD2+m_NEW_CHAD3;
  m_NEW_SumSideA = m_NEW_AEM + m_NEW_AHAD1 + m_NEW_AHAD2 + m_NEW_AHAD3;
  m_NEW_SumSideC = m_NEW_CEM + m_NEW_CHAD1 + m_NEW_CHAD2 + m_NEW_CHAD3;

  //m_hSumSideAmp[1]->Fill(m_NEW_AEM+m_NEW_AHAD1+m_NEW_AHAD2+m_NEW_AHAD3);
  //m_hSumSideAmp[0]->Fill(m_NEW_CEM+m_NEW_CHAD1+m_NEW_CHAD2+m_NEW_CHAD3);
  m_hSumSideAmp[1]->Fill(m_NEW_SumSideA);
  m_hSumSideAmp[0]->Fill(m_NEW_SumSideC);
  m_hSideAC->Fill(m_NEW_SumSideA, m_NEW_SumSideC);

  m_hEM_HAD1[0]->Fill(m_NEW_CEM, m_NEW_CHAD1);
  m_hHAD1_HAD2[0]->Fill(m_NEW_CHAD1, m_NEW_CHAD2);
  m_hHAD2_HAD3[0]->Fill(m_NEW_CHAD2, m_NEW_CHAD3);
  m_hEM_HAD1[1]->Fill(m_NEW_AEM, m_NEW_AHAD1);
  m_hHAD1_HAD2[1]->Fill(m_NEW_AHAD1, m_NEW_AHAD2);
  m_hHAD2_HAD3[1]->Fill(m_NEW_AHAD2, m_NEW_AHAD3);

  m_hSumSideAmpG0[1]->Fill(m_G0AEM + m_G0AHAD1 + m_G0AHAD2 + m_G0AHAD3);
  m_hSumSideAmpG0[0]->Fill(m_G0CEM + m_G0CHAD1 + m_G0CHAD2 + m_G0CHAD3);
  m_hSumSideAmpG1[1]->Fill(m_G1AEM + m_G1AHAD1 + m_G1AHAD2 + m_G1AHAD3);
  m_hSumSideAmpG1[0]->Fill(m_G1CEM + m_G1CHAD1 + m_G1CHAD2 + m_G1CHAD3);

  /*
     m_hEM_HAD1[0]->Fill(m_CEM, m_CHAD1);
     m_hHAD1_HAD2[0]->Fill(m_CHAD1, m_CHAD2);
     m_hHAD2_HAD3[0]->Fill(m_CHAD2, m_CHAD3);
     m_hEM_HAD1[1]->Fill(m_AEM, m_AHAD1);
     m_hHAD1_HAD2[1]->Fill(m_AHAD1, m_AHAD2);
     m_hHAD2_HAD3[1]->Fill(m_AHAD2, m_AHAD3);
     m_hSideAC->Fill(m_SumSideA, m_SumSideC);

     m_hEM_HAD1_NEW[0]->Fill(m_NEW_CEM, m_NEW_CHAD1);
     m_hHAD1_HAD2_NEW[0]->Fill(m_NEW_CHAD1, m_NEW_CHAD2);
     m_hHAD2_HAD3_NEW[0]->Fill(m_NEW_CHAD2, m_NEW_CHAD3);
     m_hEM_HAD1_NEW[1]->Fill(m_NEW_AEM, m_NEW_AHAD1);
     m_hHAD1_HAD2_NEW[1]->Fill(m_NEW_AHAD1, m_NEW_AHAD2);
     m_hHAD2_HAD3_NEW[1]->Fill(m_NEW_AHAD2, m_NEW_AHAD3);
     m_hSideAC_NEW->Fill(m_NEW_SumSideA, m_NEW_SumSideC);
   */
  return StatusCode::SUCCESS;
}

StatusCode HIMonitoringZdcTool::procHistograms( ) {
  //if( endOfLowStatFlag() || endOfLumiBlockFlag() ) {  }

  if (endOfRunFlag()) {
    for (int k = 0; k < s_Nside; k++) {
      if (m_hSumSideAmp[k]->GetEntries() > 0) m_hSumSideAmp[k]->Scale(1. / m_hSumSideAmp[k]->GetEntries());
      //       if(hSumSideAmp_NEW[k]->GetEntries() > 0) hSumSideAmp_NEW[k]->Scale(1./hSumSideAmp_NEW[k]->GetEntries());
      if (m_hSumSideAmpG0[k]->GetEntries() > 0) m_hSumSideAmpG0[k]->Scale(1. / m_hSumSideAmpG0[k]->GetEntries());
      if (m_hSumSideAmpG1[k]->GetEntries() > 0) m_hSumSideAmpG1[k]->Scale(1. / m_hSumSideAmpG1[k]->GetEntries());

      for (int i = 0; i < s_Nmod; i++) {
        if (m_hamp[i][k]->GetEntries() > 0) m_hamp[i][k]->Scale(1. / m_hamp[i][k]->GetEntries());
        //             if(m_hamp_NEW[i][k]->GetEntries() > 0)
        // m_hamp_NEW[i][k]->Scale(1./m_hamp_NEW[i][k]->GetEntries());
        if (m_hampG0[i][k]->GetEntries() > 0) m_hampG0[i][k]->Scale(1. / m_hampG0[i][k]->GetEntries());
        if (m_hampG1[i][k]->GetEntries() > 0) m_hampG1[i][k]->Scale(1. / m_hampG1[i][k]->GetEntries());
      }
    }
  }

  return StatusCode::SUCCESS;
}

/// *** FCal sum Et *** ///
void HIMonitoringZdcTool::book_hist() {
  std::string path = "HeavyIon/ZDC";

  std::stringstream nameEM_HAD1;
  std::stringstream nameHAD1_HAD2;
  std::stringstream nameHAD2_HAD3;
  std::stringstream histnameamp;
  std::stringstream histnameampG0;
  std::stringstream histnameampG1;
  std::stringstream histnameg0d0;
  std::stringstream histnameg1d0;
  std::stringstream histnameg0d1;
  std::stringstream histnameg1d1;
  std::stringstream nameSumSideAmp;
  std::stringstream nameSumSideAmpG1;
  std::stringstream nameSumSideAmpG0;
  std::stringstream nameSideAC;

  nameSideAC.str("");
  nameSideAC << "hSideA_sideC";
  m_hSideAC = TH2D_LW::create(nameSideAC.str().c_str(), nameSideAC.str().c_str(), 4097, -5, 40965, 4097, -5, 40965);
  regHist(m_hSideAC, path, run).ignore();
/*
    nameSideAC.str("");
    nameSideAC<<"h_NEW_SideA_sideC";
    m_hSideAC_NEW = TH2D_LW::create(nameSideAC.str().c_str(),
       nameSideAC.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
    regHist(m_hSideAC_NEW, path, run).ignore();
 */
  for (int k = 0; k < s_Nside; k++) {
    nameEM_HAD1.str("");
    nameEM_HAD1 << "hEM_HAD1_side" << k;
    m_hEM_HAD1[k] = TH2D_LW::create(nameEM_HAD1.str().c_str(),
                                    nameEM_HAD1.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
    regHist(m_hEM_HAD1[k], path, run).ignore();
/*
        nameEM_HAD1.str("");
        nameEM_HAD1<<"h_NEW_EM_HAD1_side"<<k;
        m_hEM_HAD1_NEW[k] =
           TH2D_LW::create(nameEM_HAD1.str().c_str(),nameEM_HAD1.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
        regHist(m_hEM_HAD1_NEW[k], path, run).ignore();
 */
    nameHAD1_HAD2.str("");
    nameHAD1_HAD2 << "hHAD1_HAD2_side" << k;
    m_hHAD1_HAD2[k] = TH2D_LW::create(nameHAD1_HAD2.str().c_str(),
                                      nameHAD1_HAD2.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
    regHist(m_hHAD1_HAD2[k], path, run).ignore();
/*
        nameHAD1_HAD2.str("");
        nameHAD1_HAD2<<"h_NEW_HAD1_HAD2_side"<<k;
        m_hHAD1_HAD2_NEW[k] =
           TH2D_LW::create(nameHAD1_HAD2.str().c_str(),nameHAD1_HAD2.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
        regHist(m_hHAD1_HAD2_NEW[k], path, run).ignore();
 */
    nameHAD2_HAD3.str("");
    nameHAD2_HAD3 << "hHAD2_HAD3_side" << k;
    m_hHAD2_HAD3[k] = TH2D_LW::create(nameHAD2_HAD3.str().c_str(),
                                      nameHAD2_HAD3.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
    regHist(m_hHAD2_HAD3[k], path, run).ignore();
/*
        nameHAD2_HAD3.str("");
        nameHAD2_HAD3<<"h_NEW_HAD2_HAD3_side"<<k;
        m_hHAD2_HAD3_NEW[k] =
           TH2D_LW::create(nameHAD2_HAD3.str().c_str(),nameHAD2_HAD3.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
        regHist(m_hHAD2_HAD3_NEW[k], path, run).ignore();
 */
    nameSumSideAmp.str("");
    nameSumSideAmp << "hSumSideAmp_side" << k;
    m_hSumSideAmp[k] = new TH1D(nameSumSideAmp.str().c_str(), nameSumSideAmp.str().c_str(), 4097, -5, 40965);
    regHist(m_hSumSideAmp[k], path, run).ignore();
/*
        nameSumSideAmp.str("");
        nameSumSideAmp<<"h_NEW_SumSideAmp_side"<<k;
        hSumSideAmp_NEW[k] = new TH1D(nameSumSideAmp.str().c_str(), nameSumSideAmp.str().c_str(),40961,-0.5,40960.5);
        regHist(hSumSideAmp_NEW[k], path, run).ignore();
 */
    nameSumSideAmpG0.str("");
    nameSumSideAmpG0 << "hSumSideAmpG0_side" << k;
    m_hSumSideAmpG0[k] = new TH1D(nameSumSideAmpG0.str().c_str(), nameSumSideAmpG0.str().c_str(), 4097, -0.5, 4096.5);
    regHist(m_hSumSideAmpG0[k], path, run).ignore();

    nameSumSideAmpG1.str("");
    nameSumSideAmpG1 << "hSumSideAmpG1_side" << k;
    m_hSumSideAmpG1[k] = new TH1D(nameSumSideAmpG1.str().c_str(), nameSumSideAmpG1.str().c_str(), 4097, -0.5, 4096.5);
    regHist(m_hSumSideAmpG1[k], path, run).ignore();

    for (int i = 0; i < s_Nmod; i++) {
      histnameamp.str("");
      histnameamp << "h_amplitude_mod" << i << "_side" << k;
      m_hamp[i][k] = new TH1D(histnameamp.str().c_str(), histnameamp.str().c_str(), 1025, -5, 10245);
      regHist(m_hamp[i][k], path, run).ignore();
      /*
              histnameamp.str("");
              histnameamp<<"h_NEW_amplitude_mod"<<i<<"_side"<<k;
              m_hamp_NEW[i][k] = new TH1D(histnameamp.str().c_str(), histnameamp.str().c_str(), 10241,-0.5,10240.5);
              regHist(m_hamp_NEW[i][k], path, run).ignore();
       */
      histnameampG0.str("");
      histnameampG0 << "h_amplitudeG0_mod" << i << "_side" << k;
      m_hampG0[i][k] = new TH1D(histnameampG0.str().c_str(), histnameampG0.str().c_str(), 1025, -0.5, 1024.5);
      regHist(m_hampG0[i][k], path, run).ignore();

      histnameampG1.str("");
      histnameampG1 << "h_amplitudeG1_mod" << i << "_side" << k;
      m_hampG1[i][k] = new TH1D(histnameampG1.str().c_str(), histnameampG1.str().c_str(), 1025, -0.5, 1024.5);
      regHist(m_hampG1[i][k], path, run).ignore();
    }
  }
}
