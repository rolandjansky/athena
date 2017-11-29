/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArRODMonTool.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Mathieu Aurousseau
//
// 
//
// ********************************************************************

#include "LArMonTools/LArRODMonTool.h"

#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "CaloIdentifier/CaloGain.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

#include "LArRawEvent/LArFebHeader.h" 
#include "LArMonTools/LArOnlineIDStrHelper.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "AthenaKernel/errorcheck.h"

const int max_dump=100;
int ndump=0;

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) 
#   define CAN_REBIN(hist)  hist->SetCanExtend(TH1::kAllAxes)
#else
#   define CAN_REBIN(hist)  hist->SetBit(TH1::kCanRebin)
#endif

/*---------------------------------------------------------*/
LArRODMonTool::LArRODMonTool(const std::string& type, 
			     const std::string& name,
			     const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_LArOnlineIDHelper(NULL),
    m_counter(0),
    m_eventsCounter(0),
    m_histos(N_PARTITIONS),
    m_errcounters(N_PARTITIONS),
    m_adc2mevtool("LArADC2MeVToolDefault"),
    m_calo_noise_tool("CaloNoiseTool/CaloNoiseToolDefault"),
    m_cable_service_tool ( "LArCablingService" ),
    m_BC(0),
    m_dumpDigits(false)

{
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "HIGH");
  declareProperty("useEvtCounter", m_useEvtCounter = true);
  declareProperty("LArRawChannelKey_fromBytestream",m_channelKey_fromBytestream="LArRawChannels_fromBytestream");
  declareProperty("LArRawChannelKey_fromDigits",m_channelKey_fromDigits="LArRawChannels_fromDigits");
  declareProperty("KeyOFC",m_keyOFC="LArOFC") ;  
  declareProperty("KeyShape",m_keyShape="LArShape") ;  
  declareProperty("DigitsFileName",m_DigitsFileName = "digits.txt");
  declareProperty("EnergyFileName",m_EnergyFileName = "energy.txt");
  declareProperty("AiFileName",m_AiFileName = "Calib_ai.dat");
  declareProperty("DumpCellsFileName",m_DumpCellsFileName = "dumpCells.txt");
  declareProperty("DoDspTestDump",m_doDspTestDump = false);
  declareProperty("DoCellsDump",m_doCellsDump = false);
  declareProperty("ADC2MeVTool",m_adc2mevtool);
  declareProperty("LArPedestalKey",m_larpedestalkey = "Pedestal");
  declareProperty("DoCheckSum",m_doCheckSum = true);
  declareProperty("DoRodStatus",m_doRodStatus = true);
  declareProperty("PrintEnergyErrors",m_printEnergyErrors = true);
  declareProperty("RemoveNoiseBursts",m_removeNoiseBursts = true);

  // Ranges and precisions for energy
  declareProperty("ERange0",m_range_E_0 = 8192);
  declareProperty("ERange1",m_range_E_1 = 65536);
  declareProperty("ERange2",m_range_E_2 = 524288);
  declareProperty("ERange3",m_range_E_3 = 4194304);
  declareProperty("PrecisionERange0",m_precision_E_0 = 1);
  declareProperty("PrecisionERange1",m_precision_E_1 = 8);
  declareProperty("PrecisionERange2",m_precision_E_2 = 64);
  declareProperty("PrecisionERange3",m_precision_E_3 = 512);
  declareProperty("PrecisionERangeMax",m_precision_E_max = 8192);
  // Ranges and precisions for time
  declareProperty("TRange0",m_range_T_0);
  declareProperty("TRange1",m_range_T_1);
  declareProperty("TRange2",m_range_T_2);
  declareProperty("TRange3",m_range_T_3);
  declareProperty("PrecisionTRange0",m_precision_T_0 = 10);
  declareProperty("PrecisionTRange1",m_precision_T_1 = 20);
  declareProperty("PrecisionTRange2",m_precision_T_2 = 30);
  declareProperty("PrecisionTRange3",m_precision_T_3 = 50);
  declareProperty("PrecisionTRangeMax",m_precision_T_max = 50);
  // Ranges and precisions for quality factor
  declareProperty("QRange0",m_range_Q_0);
  declareProperty("QRange1",m_range_Q_1);
  declareProperty("QRange2",m_range_Q_2);
  declareProperty("QRange3",m_range_Q_3);
  declareProperty("PrecisionQRange0",m_precision_Q_0 = 1);
  declareProperty("PrecisionQRange1",m_precision_Q_1 = 1);
  declareProperty("PrecisionQRange2",m_precision_Q_2 = 1);
  declareProperty("PrecisionQRange3",m_precision_Q_3 = 1);
  declareProperty("PrecisionQRangeMax",m_precision_Q_max = 1);
  
  declareProperty("OnlineHistorySize",m_history_size = 20);
  declareProperty("OnlineHistoryGranularity",m_history_granularity = 5);

  declareProperty("NoiseTool",m_calo_noise_tool);

  declareProperty("TimeOFCUnitOnline",m_unit_online = 1);
  declareProperty("TimeOFCUnitOffline",m_unit_offline = 1);

  declareProperty("TimeOffset",m_timeOffset = 0);

  declareProperty("ADCthreshold",m_adc_th = 50); // Minimal number of ADC amplitude among samples required to compare online/offline
  declareProperty("peakTimeCut",m_peakTime_cut = 5.); // Cut on abs(peak time) to compare online/offline (all quantities)

  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("SkipKnownProblematicChannels",m_skipKnownProblematicChannels = false);
  declareProperty("SkipNullPed",m_skipNullPed = false);
  declareProperty("SkipNullQT",m_skipNullQT = false);

  declareProperty("Streams",m_streams);
  declareProperty("numberOfLB",m_nb_lb = 2000.);

  declareProperty("IsOnline",m_IsOnline = false);  //for online/offline histos

  m_count_gain[0]=0;
  m_count_gain[1]=0;
  m_count_gain[2]=0;

  m_fai   = NULL;
  m_fdump = NULL;

  m_hSummaryErrors_Energy = NULL;
  m_hSummaryErrors_Time = NULL;
  m_hSummaryErrors_Quality = NULL;

  m_hE_all = NULL;
  m_hT_all = NULL;
  m_hQ_all = NULL;

  m_hE_ranges_all = NULL;

  m_hEErrors_LB_part = NULL;
  m_hTErrors_LB_part = NULL;
  m_hQErrors_LB_part = NULL;

  m_hEErrors_LB_stream = NULL;
  m_hTErrors_LB_stream = NULL;
  m_hQErrors_LB_stream = NULL;

  m_last_lb = -1;
  m_curr_lb = -1;

  m_calo_description_mgr=nullptr;
  m_hsize=0;
}

/*---------------------------------------------------------*/
LArRODMonTool::~LArRODMonTool()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArRODMonTool::initialize() {
  ATH_MSG_VERBOSE( "In LArRODMonTool::initialize() ");

  StatusCode sc = detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get LArOnlineIDHelper" );
    return sc;
  }

  m_dumpDigits=(m_doDspTestDump || m_doCellsDump || (m_adc_th != 0));

//  For Eon/Eoff dump
//  if (m_doDspTestDump) {
    sc = detStore()->regHandle(m_dd_ofc,m_keyOFC);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL( "Cannot register DataHandle for OFC object with key " << m_keyOFC ); 
      return sc;
    }

    sc = detStore()->regHandle(m_dd_shape,m_keyShape);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL( "Cannot register DataHandle for Shape object with key " << m_keyShape ); 
      return sc;
    }

    sc = detStore()->regHandle(m_dd_HVScaleCorr,"LArHVScaleCorr");
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL( "Cannot register DataHandle for HVScaleCorr object with key LArHVScaleCorr" ); 
      return sc;
    }
    // ADC2MeV Tool
    sc = m_adc2mevtool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to find tool for LArADC2MeV" );
      return StatusCode::FAILURE;
    }

    sc = m_calo_noise_tool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to find calo noise tool" );
      return StatusCode::FAILURE;
    }
    sc = m_cable_service_tool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to find cabling service tool" );
      return StatusCode::FAILURE;
    }
    sc = detStore()->retrieve(m_calo_description_mgr);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to find CeloDetDescrManager " );
      return StatusCode::FAILURE;
    }
//  }
//  For Eon/Eoff dump
//  if (m_skipNullPed || m_doDspTestDump) {
    sc=detStore()->retrieve(m_larpedestal,m_larpedestalkey);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Cannot register DataHandle for Pedestal object with key " << m_larpedestalkey );
      return StatusCode::FAILURE;
    }
//  }
  if (m_skipKnownProblematicChannels) { 
    sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
      return StatusCode::FAILURE;
    }
  
  }
  // Open output files for DspTest
  if (m_doDspTestDump) {
    m_fai = std::fopen(m_AiFileName.c_str(),"w");
    m_fdig.open(m_DigitsFileName.c_str());
    m_fen.open(m_EnergyFileName.c_str());
  }

  // Output file
  if (m_doCellsDump) {
    m_fdump = std::fopen(m_DumpCellsFileName.c_str(),"w");
    fprintf(m_fdump,"febid        ");
    fprintf(m_fdump,"channel ");
    fprintf(m_fdump,"CellID       ");
    fprintf(m_fdump,"slot   ");
    fprintf(m_fdump,"FT     ");
    fprintf(m_fdump,"barrel_ec "); 
    fprintf(m_fdump,"posneg ");
    fprintf(m_fdump,"partition ");
    //    fprintf(m_fdump,"eta     ");
    //fprintf(m_fdump,"phi     ");
    fprintf(m_fdump,"gain     ");
    //fprintf(m_fdump,"isConnected     ");
    fprintf(m_fdump,"E_off     ");
    fprintf(m_fdump,"E_on     ");
    fprintf(m_fdump,"T_off     ");
    fprintf(m_fdump,"T_on     ");
    fprintf(m_fdump,"Q_off     ");
    fprintf(m_fdump,"Q_on     ");
    fprintf(m_fdump,"event   \n");
  }

  // Time unit :
  m_BC = 25000.; // picoseconds

  ATH_MSG_DEBUG("Setting an offset time of " << m_timeOffset << " BC, i.e. " << m_timeOffset*m_BC << " ps");


  ManagedMonitorToolBase::initialize().ignore();

  m_hsize = m_history_size/m_history_granularity;
  if(m_history_granularity <= 0 || m_history_size < 1 || m_hsize < 1) {
   ATH_MSG_ERROR("Wrong history size or granularity "<<m_history_granularity << " " <<m_history_size<<" "<<m_hsize);
   return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Resetting history size: "<<m_hsize);
  m_hdone.assign(m_hsize,false);
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArRODMonTool::bookHistograms()
{
  ATH_MSG_VERBOSE( "In LArRODMonTool::bookHistograms() ");

  StatusCode sc = StatusCode::SUCCESS;

  //  if(isNewRun){
    std::string hName ;
    std::string hTitle ;
    LArOnlineIDStrHelper strHelper(m_LArOnlineIDHelper);
    strHelper.setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);

    // EMB A and C
    MonGroup generalGroupBarrel( this, "/LAr/DSPMonitoring/EMB", run, ATTRIB_MANAGED );
    MonGroup generalGroupEndcap( this, "/LAr/DSPMonitoring/Emec", run, ATTRIB_MANAGED );
    MonGroup generalGroupHec( this, "/LAr/DSPMonitoring/Hec", run, ATTRIB_MANAGED );
    MonGroup generalGroupFcal( this, "/LAr/DSPMonitoring/Fcal", run, ATTRIB_MANAGED );

    m_histos[EMBA].m_partName="EMBA";
    m_histos[EMBA].m_monGroup=&generalGroupBarrel;

    m_histos[EMBC].m_partName="EMBC";
    m_histos[EMBC].m_monGroup=&generalGroupBarrel;

    m_histos[EMECA].m_partName="EMECA";
    m_histos[EMECA].m_monGroup=&generalGroupEndcap;

    m_histos[EMECC].m_partName="EMECC";
    m_histos[EMECC].m_monGroup=&generalGroupEndcap;

    m_histos[HECA].m_partName="HECA";
    m_histos[HECA].m_monGroup=&generalGroupHec;

    m_histos[HECC].m_partName="HECC";
    m_histos[HECC].m_monGroup=&generalGroupHec;

    m_histos[FCALA].m_partName="FcalA";
    m_histos[FCALA].m_monGroup=&generalGroupFcal;

    m_histos[FCALC].m_partName="FcalC";
    m_histos[FCALC].m_monGroup=&generalGroupFcal;


    for (unsigned p=0;p<N_PARTITIONS;++p) {
      HistGroup& hg=m_histos[p];
      const std::string& Part = hg.m_partName;
      hName = "DE_" + Part;
      hTitle = "E_{offline} - E_{online}-" + Part;
      hg.m_hDE = new TH1F(hName.c_str(), hTitle.c_str(),400,-40.,40.);
      hg.m_hDE->GetXaxis()->SetTitle("E_{offline} - E_{online}");
      CHECK(hg.m_monGroup->regHist(hg.m_hDE));
      
      hName = "DT_" + Part;
      hTitle = "T_{offline} - T_{online}-" + Part;
      hg.m_hDT = new TH1F(hName.c_str(), hTitle.c_str(),400,-800.,800.);
      hg.m_hDT->GetXaxis()->SetTitle("T_{offline} - T_{online}");
      CHECK(hg.m_monGroup->regHist(hg.m_hDT));
  
      hName = "DQ_" + Part;
      hTitle = "Q_{offline} - Q_{online} / #sqrt{Q_{offline}}-" + Part;
      hg.m_hDQ = new TH1F(hName.c_str(), hTitle.c_str(),400,-3000.,1000.);
      hg.m_hDQ->GetXaxis()->SetTitle("Q_{offline} - Q_{online} / #sqrt{Q_{offline}}");
      CHECK(hg.m_monGroup->regHist(hg.m_hDQ));

      hName = "Out_E_FT_vs_SLOT_" + Part;
      hTitle = "# of cells with E_{offline} - E_{online} > numerical precision ";
      hTitle = hTitle + Part;
      hg.m_hOut_E_FT_vs_SLOT = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
      hg.m_hOut_E_FT_vs_SLOT->GetXaxis()->SetTitle("Slot");
      hg.m_hOut_E_FT_vs_SLOT->GetYaxis()->SetTitle("Feedthrough");
      sc = sc && strHelper.definePartitionSummProp(hg.m_hOut_E_FT_vs_SLOT);   
      CHECK(hg.m_monGroup->regHist(hg.m_hOut_E_FT_vs_SLOT));

      if(m_IsOnline) { // book shadow histograms for subtracting
        hg.m_hOut_E_FT_vs_SLOT_shadow=new TH2F*[m_hsize];
        hName += "_shadow";
        for(int i=0; i<m_hsize; ++i) {
          hg.m_hOut_E_FT_vs_SLOT_shadow[i] = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
        }
      }

      hName = "Out_T_FT_vs_SLOT_" + Part;
      hTitle = "# of cells with T_{offline} - T_{online} > numerical precision ";
      hTitle = hTitle + Part;
      hg.m_hOut_T_FT_vs_SLOT = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
      hg.m_hOut_T_FT_vs_SLOT->GetXaxis()->SetTitle("Slot");
      hg.m_hOut_T_FT_vs_SLOT->GetYaxis()->SetTitle("FT");
      sc = sc && strHelper.definePartitionSummProp(hg.m_hOut_T_FT_vs_SLOT);   
      CHECK(hg.m_monGroup->regHist(hg.m_hOut_T_FT_vs_SLOT)); 

      if(m_IsOnline) { // book shadow histograms for subtracting
        hg.m_hOut_T_FT_vs_SLOT_shadow=new TH2F*[m_hsize];
        hName += "_shadow";
        for(int i=0; i<m_hsize; ++i) {
          hg.m_hOut_T_FT_vs_SLOT_shadow[i] = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
        }
      }

      hName = "Out_Q_FT_vs_SLOT_" + Part;
      hTitle = "# of cells with Q_{offline} - Q_{online} / #sqrt{Q_{offline}} > numerical precision ";
      hTitle = hTitle + Part;
      hg.m_hOut_Q_FT_vs_SLOT = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
      hg.m_hOut_Q_FT_vs_SLOT->GetXaxis()->SetTitle("Slot");
      hg.m_hOut_Q_FT_vs_SLOT->GetYaxis()->SetTitle("FT");
      sc = sc && strHelper.definePartitionSummProp(hg.m_hOut_Q_FT_vs_SLOT);
      CHECK(hg.m_monGroup->regHist(hg.m_hOut_Q_FT_vs_SLOT)); 

      if(m_IsOnline) { // book shadow histograms for subtracting
        hg.m_hOut_Q_FT_vs_SLOT_shadow=new TH2F*[m_hsize];
        hName += "_shadow";
        for(int i=0; i<m_hsize; ++i) {
          hg.m_hOut_Q_FT_vs_SLOT_shadow[i] = new TH2F(hName.c_str(), hTitle.c_str(),15,0.5,15.5,32,-0.5,31.5);
        }
      }

      hName = "Eon_VS_Eoff_" + Part;
      hTitle = "E_{online} VS E_{offline}-" + Part;
      hg.m_hEon_VS_Eoff = new TH2F(hName.c_str(), hTitle.c_str(),300,-20000.,20000.,300,-20000.,20000.);
      hg.m_hEon_VS_Eoff->GetXaxis()->SetTitle("E_{offline} (MeV)");
      hg.m_hEon_VS_Eoff->GetYaxis()->SetTitle("E_{online} (MeV)");
      CHECK(hg.m_monGroup->regHist(hg.m_hEon_VS_Eoff));
      
      hName = "Ton_VS_Toff_" + Part;
      hTitle = "T_{online} VS T_{offline}-" + Part;
      hg.m_hTon_VS_Toff = new TH2F(hName.c_str(), hTitle.c_str(),300,-80000.,80000.,300,-80000.,80000.);
      hg.m_hTon_VS_Toff->GetXaxis()->SetTitle("T_{offline} (ps)");
      hg.m_hTon_VS_Toff->GetYaxis()->SetTitle("T_{online} (ps)");
      CHECK(hg.m_monGroup->regHist(hg.m_hTon_VS_Toff));
      
      hName = "Qon_VS_Qoff_" + Part;
      hTitle = "Q_{online} VS Q_{offline}-" + Part;
      hg.m_hQon_VS_Qoff = new TH2F(hName.c_str(), hTitle.c_str(),300,0.,66000.,300,0.,66000.);
      hg.m_hQon_VS_Qoff->GetXaxis()->SetTitle("Q_{offline}");
      hg.m_hQon_VS_Qoff->GetYaxis()->SetTitle("Q_{online}");
      CHECK(hg.m_monGroup->regHist(hg.m_hQon_VS_Qoff));

      hName = "Sweet_cells_" + Part;
      hTitle = "Number of sweet Cells in LAr" + Part;
      hg.m_Sweet = new TH1F(hName.c_str(), hTitle.c_str(),128,-0.5,127.5); 
      hg.m_Sweet->GetXaxis()->SetTitle("Sweet cells per feb");
      CHECK(hg.m_monGroup->regHist(hg.m_Sweet));
    }//end loop over partitions

    // Summary histogram
    MonGroup generalGroup( this, "/LAr/DSPMonitoring/Summary", run, ATTRIB_MANAGED );   
    
    hName = "Summary_E";
    hTitle = "Summary of errors on Energy per partition and per gain";
    m_hSummaryErrors_Energy = new TH2F(hName.c_str(),hTitle.c_str(),8,0.,8.,3,0.,3.);
    m_hSummaryErrors_Energy->GetXaxis()->SetTitle("Partition");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(1,"EMBC");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(2,"EMBA");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(3,"EMECC");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(4,"EMECA");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(5,"HECC");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(6,"HECA");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(7,"FcalC");
    m_hSummaryErrors_Energy->GetXaxis()->SetBinLabel(8,"FcalA");
    m_hSummaryErrors_Energy->GetYaxis()->SetTitle("Gain");
    m_hSummaryErrors_Energy->GetYaxis()->SetBinLabel(3,"Low Gain");
    m_hSummaryErrors_Energy->GetYaxis()->SetBinLabel(2,"Medium Gain");
    m_hSummaryErrors_Energy->GetYaxis()->SetBinLabel(1,"High Gain");
    CHECK(generalGroup.regHist(m_hSummaryErrors_Energy));

    hName = "Summary_T";
    hTitle = "Summary of errors on Time per partition and per gain";
    m_hSummaryErrors_Time = new TH2F(hName.c_str(),hTitle.c_str(),8,0.,8.,3,0.,3.);
    m_hSummaryErrors_Time->GetXaxis()->SetTitle("Partition");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(1,"EMBC");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(2,"EMBA");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(3,"EMECC");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(4,"EMECA");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(5,"HECC");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(6,"HECA");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(7,"FcalC");
    m_hSummaryErrors_Time->GetXaxis()->SetBinLabel(8,"FcalA");
    m_hSummaryErrors_Time->GetYaxis()->SetTitle("Gain");
    m_hSummaryErrors_Time->GetYaxis()->SetBinLabel(3,"Low Gain");
    m_hSummaryErrors_Time->GetYaxis()->SetBinLabel(2,"Medium Gain");
    m_hSummaryErrors_Time->GetYaxis()->SetBinLabel(1,"High Gain");
    CHECK(generalGroup.regHist(m_hSummaryErrors_Time));

    hName = "Summary_Q";
    hTitle = "Summary of errors on Quality per partition and per gain";
    m_hSummaryErrors_Quality = new TH2F(hName.c_str(),hTitle.c_str(),8,0.,8.,3,0.,3.);
    m_hSummaryErrors_Quality->GetXaxis()->SetTitle("Partition");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(1,"EMBC");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(2,"EMBA");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(3,"EMECC");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(4,"EMECA");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(5,"HECC");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(6,"HECA");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(7,"FcalC");
    m_hSummaryErrors_Quality->GetXaxis()->SetBinLabel(8,"FcalA");
    m_hSummaryErrors_Quality->GetYaxis()->SetTitle("Gain");
    m_hSummaryErrors_Quality->GetYaxis()->SetBinLabel(3,"Low Gain");
    m_hSummaryErrors_Quality->GetYaxis()->SetBinLabel(2,"Medium Gain");
    m_hSummaryErrors_Quality->GetYaxis()->SetBinLabel(1,"High Gain");
    CHECK(generalGroup.regHist(m_hSummaryErrors_Quality));
    
    hName = "E_all";
    hTitle = "E_{offline} - E_{online} for all partitions";
    m_hE_all = new TH1F(hName.c_str(),hTitle.c_str(),400,-40.,40.);
    m_hE_all->GetXaxis()->SetTitle("E_{offline} - E_{online} (MeV)");
    CHECK(generalGroup.regHist(m_hE_all));  

    hName = "T_all";
    hTitle = "T_{offline} - T_{online} for all partitions";
    m_hT_all = new TH1F(hName.c_str(),hTitle.c_str(),400,-800.,800.);
    m_hT_all->GetXaxis()->SetTitle("T_{offline} - T_{online} (ps)");
    CHECK(generalGroup.regHist(m_hT_all));

    hName = "Q_all";
    hTitle = "Q_{offline} - Q_{online} / #sqrt{Q_{offline}} for all partitions";
    m_hQ_all = new TH1F(hName.c_str(),hTitle.c_str(),400,-3000.,1000.);
    m_hQ_all->GetXaxis()->SetTitle("Q_{offline} - Q_{online} / #sqrt{Q_{offline}}");
    CHECK(generalGroup.regHist(m_hQ_all));

    MonGroup debugGroup( this, "/LAr/DSPMonitoring/DQMD", run, ATTRIB_MANAGED ); // "shift" SHOULD BE CHANGED do "debug" ?

    for (unsigned p=0;p<N_PARTITIONS;++p) {
      HistGroup& hg=m_histos[p];
      const std::string& Part = hg.m_partName;
    
      hName = "DE_ranges_" + Part;
      hTitle = "E_{online} - E_{offline} for all ranges " + Part;
      hg.m_hDE_ranges = new TH2F(hName.c_str(), hTitle.c_str(),5000,-515,515,4,0,4);
      hg.m_hDE_ranges->GetXaxis()->SetTitle("E_{offline} - E_{online} (MeV)");
      hg.m_hDE_ranges->GetYaxis()->SetTitle("Energy range");
      hg.m_hDE_ranges->GetYaxis()->SetBinLabel(1,"E < 2^{13} MeV");
      hg.m_hDE_ranges->GetYaxis()->SetBinLabel(2,"E < 2^{16} MeV");
      hg.m_hDE_ranges->GetYaxis()->SetBinLabel(3,"E < 2^{19} MeV");
      hg.m_hDE_ranges->GetYaxis()->SetBinLabel(4,"E < 2^{22} MeV");
      CHECK(debugGroup.regHist(hg.m_hDE_ranges));
    }//end loop over partitions

    hName = "E_ranges_all";
    hTitle = "E_{online} - E_{offline} for all ranges and all partitions ";
    m_hE_ranges_all = new TH2F(hName.c_str(), hTitle.c_str(),5000,-515,515,4,0,4);
    m_hE_ranges_all->GetXaxis()->SetTitle("E_{offline} - E_{online} (MeV)");
    m_hE_ranges_all->GetYaxis()->SetTitle("Energy range");
    m_hE_ranges_all->GetYaxis()->SetBinLabel(1,"E < 2^{13} MeV");
    m_hE_ranges_all->GetYaxis()->SetBinLabel(2,"E < 2^{16} MeV");
    m_hE_ranges_all->GetYaxis()->SetBinLabel(3,"E < 2^{19} MeV");
    m_hE_ranges_all->GetYaxis()->SetBinLabel(4,"E < 2^{22} MeV");
    CHECK(debugGroup.regHist(m_hE_ranges_all));


    MonGroup infosGroup( this, "/LAr/DSPMonitoring/Infos", run, ATTRIB_MANAGED );
    
    hName = "EErrorsPerLB";
    std::string cut = "#delta ADC>"+std::to_string(m_adc_th)+" and |t_{offline}| < "+std::to_string(int(m_peakTime_cut))+"ns";
    hTitle = "Nb of errors in E per LB - "+ cut;
    m_hEErrors_LB_part = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,8,0,8);
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(1,"EMBC");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(2,"EMBA");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(3,"EMECC");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(4,"EMECA");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(5,"HECC");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(6,"HECA");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(7,"FcalC");
    m_hEErrors_LB_part->GetYaxis()->SetBinLabel(8,"FcalA");
    m_hEErrors_LB_part->GetXaxis()->SetTitle("Luminosity Block");
    CHECK(infosGroup.regHist(m_hEErrors_LB_part));

    hName = "TErrorsPerLB";
    hTitle = "Nb of errors in T per LB - "+cut;
    m_hTErrors_LB_part = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,8,0,8);
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(1,"EMBC");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(2,"EMBA");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(3,"EMECC");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(4,"EMECA");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(5,"HECC");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(6,"HECA");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(7,"FcalC");
    m_hTErrors_LB_part->GetYaxis()->SetBinLabel(8,"FcalA");
    m_hTErrors_LB_part->GetXaxis()->SetTitle("Luminosity Block");
    CHECK(infosGroup.regHist(m_hTErrors_LB_part));

    hName = "QErrorsPerLB";
    hTitle = "Nb of errors in Q per LB - "+cut;
    m_hQErrors_LB_part = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,8,0,8);
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(1,"EMBC");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(2,"EMBA");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(3,"EMECC");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(4,"EMECA");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(5,"HECC");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(6,"HECA");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(7,"FcalC");
    m_hQErrors_LB_part->GetYaxis()->SetBinLabel(8,"FcalA");
    m_hQErrors_LB_part->GetXaxis()->SetTitle("Luminosity Block");
    CHECK(infosGroup.regHist(m_hQErrors_LB_part));

    int nStreams = m_streams.size();

    hName = "EErrorsPerLBStream";
    hTitle = "Nb of errors in E per LB and per stream";
    m_hEErrors_LB_stream = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,nStreams+1,0,nStreams+1);
    for (int str = 0; str < nStreams; str++) {
      char n[50];
      sprintf(n,"%s",m_streams[str].c_str());
      m_hEErrors_LB_stream->GetYaxis()->SetBinLabel(str+1,n);
    }
    m_hEErrors_LB_stream->GetYaxis()->SetBinLabel(nStreams+1,"others");
    CHECK(infosGroup.regHist(m_hEErrors_LB_stream));  

    hName = "TErrorsPerLBStream";
    hTitle = "Nb of errors in T per LB and per stream";
    m_hTErrors_LB_stream = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,nStreams+1,0,nStreams+1);
    for (int str = 0; str < nStreams; str++) {
      char n[50];
      sprintf(n,"%s",m_streams[str].c_str());
      m_hTErrors_LB_stream->GetYaxis()->SetBinLabel(str+1,n);
    }
    m_hTErrors_LB_stream->GetYaxis()->SetBinLabel(nStreams+1,"others");
    CHECK(infosGroup.regHist(m_hTErrors_LB_stream));

    hName = "QErrorsPerLBStream";
    hTitle = "Nb of errors in Q per LB and per stream";
    m_hQErrors_LB_stream = new TH2I(hName.c_str(), hTitle.c_str(),m_nb_lb,-0.5,m_nb_lb-0.5,nStreams+1,0,nStreams+1);
    for (int str = 0; str < nStreams; str++) {
      char n[50];
      sprintf(n,"%s",m_streams[str].c_str());
      m_hQErrors_LB_stream->GetYaxis()->SetBinLabel(str+1,n);
    }
    m_hQErrors_LB_stream->GetYaxis()->SetBinLabel(nStreams+1,"others");
    CHECK(infosGroup.regHist(m_hQErrors_LB_stream));
    //  }

  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Bookhistograms failed" );
  }

  return sc;

}

bool LArRODMonTool::FebStatus_Check() {
  m_ignoreFEBs.clear();
  const LArFebHeaderContainer* febCont=NULL;
  StatusCode sc = evtStore()->retrieve(febCont);
  if (sc.isFailure() || !febCont) {
    ATH_MSG_WARNING( "No LArFEB container found in TDS" ); 
    return false;
  }
  LArFebHeaderContainer::const_iterator itFEB = febCont->begin(); 
  LArFebHeaderContainer::const_iterator itFEB_e = febCont->end(); 
  for ( ; itFEB!=itFEB_e;++itFEB) {
    if (((m_doCheckSum && (*itFEB)->ChecksumVerification()==false)) || 
	(m_doRodStatus && (*itFEB)->RodStatus()!=0)) 
      m_ignoreFEBs.insert((*itFEB)->FEBId());
  }
  return true;
}

StatusCode LArRODMonTool::fillHistograms() {
  ATH_MSG_VERBOSE( "In LArRODMonTool::fillHistograms()");;

  // Increment event counter
  m_eventsCounter++;

  /*
  // Expected precision for energy calculation, depending on energy (ranges) :
  // Between -213 and 213 MeV (~ 8 GeV)        2**0=1 MeV precision (range 0)
  // Between -216 and 216 MeV (~ 64 GeV)       2**3=8 MeV precision (range 1)
  // Between -219 and 219 MeV (~ 512 GeV)      2**6=64 MeV precision (range 2)
  // Between -222 and 222 MeV (~ 4 TeV)        2**9=512 MeV precision (range 3)
  */


  const xAOD::EventInfo* thisEventInfo;
  if (evtStore()->retrieve(thisEventInfo).isFailure()) {
    ATH_MSG_ERROR( "No EventInfo object found! Can't read run number!" );
    return StatusCode::FAILURE;
  }

  m_curr_lb=thisEventInfo->lumiBlock();   
  if(m_last_lb < 0) m_last_lb = m_curr_lb;
  bool isEventFlaggedByLArNoisyROAlg = false; // keep default as false
  bool isEventFlaggedByLArNoisyROAlgInTimeW = false; // keep deault as false

  if ( thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,0) ) {
    isEventFlaggedByLArNoisyROAlg = true;
    ATH_MSG_DEBUG( " !!! Noisy event found from LArNoisyROAlg !!!" );
  }

  if ( thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,3) ) {
    isEventFlaggedByLArNoisyROAlgInTimeW = true;
    ATH_MSG_DEBUG( " !!! Noisy event found by LArNoisyROAlg in Time window of 500ms!!!" );
  }
  
  // Noise bursts cleaning (LArNoisyRO_Std or TimeWindowVeto) added by B.Trocme - 19/7/12
  if (m_removeNoiseBursts && (isEventFlaggedByLArNoisyROAlg || isEventFlaggedByLArNoisyROAlgInTimeW)) return StatusCode::SUCCESS;
  
  // Retrieve stream info
  int nStreams = m_streams.size();
   //  if ((nStreams == 1 && m_streams[0] == "all") || nStreams <= 0) selectstreams = false; 
  
  
  //Fixme: Use LArTrigStreamMatching also here.
  const int streamsize = nStreams + 1;
  std::vector<int> hasStream(streamsize,0);
  //  for (int str = 0; str < nStreams + 1; str++) hasStream[str] = 0;

  bool hasstrlist = false;
  const std::vector< xAOD::EventInfo::StreamTag >& evtStreamTags=thisEventInfo->streamTags();
  for (const auto& evtStreamTag : evtStreamTags) {
    std::vector<std::string>::const_iterator evtStreamTagIt=std::find(m_streams.begin(),m_streams.end(),evtStreamTag.name());
    if (evtStreamTagIt!=m_streams.end()) {
      const unsigned str=evtStreamTagIt-m_streams.begin();
      ATH_MSG_VERBOSE( "Keeping Stream Tag: " << evtStreamTag.type() << "_" << evtStreamTag.name());
      hasStream[str] = 1;
      hasstrlist = true;
    }
  }
  if (! hasstrlist) hasStream[nStreams] = 1; 


  /*
  if(oldtriggerInfo!= 0){
    std::vector<TriggerInfo::StreamTag> oldstream = oldtriggerInfo->streamTags();
    std::vector<TriggerInfo::StreamTag>::const_iterator oldstreamI = oldstream.begin();
    std::vector<TriggerInfo::StreamTag>::const_iterator oldstreamE = oldstream.end();
    bool hasstrlist = false;
    for(;oldstreamI!=oldstreamE; ++oldstreamI){
      ATH_MSG_VERBOSE( "Keeping Stream Tag: " << oldstreamI->type() << "_" << oldstreamI->name());
      for (int str = 0; str < nStreams; str++) {
	if (m_streams[str] == oldstreamI->name()) {
	  hasStream[str] = 1;
	  hasstrlist = true;
	  break;
	}
      }
    }
    if (! hasstrlist) hasStream[nStreams] = 1; 
  }
  */

  // Retrieve LArRawChannels from Digits (via LArRawChannelBuilder)
  const LArRawChannelContainer* rawColl_fromDigits;
  StatusCode sc = evtStore()->retrieve(rawColl_fromDigits, m_channelKey_fromDigits);
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArRawChannelContainer with key " << m_channelKey_fromDigits );
    return StatusCode::FAILURE;
  }

  // Retrieve LArRawChannels from ByteStream (i.e. calculated by DSP)
  const LArRawChannelContainer* rawColl_fromBytestream;
  sc = evtStore()->retrieve(rawColl_fromBytestream, m_channelKey_fromBytestream);
  if(sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArRawChannelContainer with key " << m_channelKey_fromBytestream );
    return StatusCode::FAILURE;
  }

  const LArDigitContainer* pLArDigitContainer=NULL;
//  For Eon/Eoff dump
//  if (m_dumpDigits) {
    sc = evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Can't retrieve LArDigitContainer with key " << m_LArDigitContainerKey <<". Turn off digit dump." );
      closeDumpfiles();
      m_dumpDigits=false;
      m_doDspTestDump=false;
      m_doCellsDump=false;
    }
//  }
  if (m_doCheckSum || m_doRodStatus) {
    FebStatus_Check();
    ATH_MSG_DEBUG("Found " << m_ignoreFEBs.size() << " FEBs with checksum errors or statatus errors. Will ignore these FEBs.");
  }

  for (unsigned p=0;p<N_PARTITIONS;++p)
    m_errcounters[p].clear();
  
  m_errsPerFEB.clear();
  m_errsPerFEB.resize(m_LArOnlineIDHelper->febHashMax(),0);

  const bool ignoreFebs=(m_ignoreFEBs.size()>0);
  std::set<HWIdentifier>::const_iterator ignoreFebsEnd=m_ignoreFEBs.end();
  
  //Build an assoiciation of channels in the two LArRawChannelContainers.
  //The LArRawChannelContainers are unordered collections of LArRawChannels. 
  //But we know that they have the same order because they were built from the same source (namely the LArDigits and RawChannels in the Bytestream)
  //and we know that the LArRawChannels built offline are a subset of the LArRawChannelContainers read from Bytestream.
  //Therfore we can search much more efficiently
  LArRawChannelContainer::const_iterator rcDigIt=rawColl_fromDigits->begin();
  LArRawChannelContainer::const_iterator rcDigIt_e=rawColl_fromDigits->end();
  LArRawChannelContainer::const_iterator rcBSIt=rawColl_fromBytestream->begin();
  LArRawChannelContainer::const_iterator rcBSIt_e=rawColl_fromBytestream->end();

  //Loop over indices in LArRawChannelContainer built offline (the small one)
  ATH_MSG_DEBUG( "Entering the LArRawChannel loop." );

  for (;rcDigIt!=rcDigIt_e;++rcDigIt) {
    const HWIdentifier idDig=rcDigIt->hardwareID();
    const HWIdentifier febId=m_LArOnlineIDHelper->feb_Id(idDig);
    // Check if this FEB should be ignored
    if (ignoreFebs) { 
      if (m_ignoreFEBs.find(febId)!=ignoreFebsEnd) continue;
    }
    //Check if this is a bad channel
    if (m_skipKnownProblematicChannels && m_badChannelMask->cellShouldBeMasked(idDig)) continue;

    //Check pedestal if needed
    if (m_skipNullPed) {
      const CaloGain::CaloGain gain = rcDigIt->gain();
      const float ped = m_larpedestal->pedestal(idDig,gain);
      if(ped <= (1.0+LArElecCalib::ERRORCODE)) continue;
    }

    //Now look for corresponding channel in the LArRawChannelContainer read from Bytestream (the big one)
    LArRawChannelContainer::const_iterator currIt=rcBSIt; //Remember current position in container
    for (;rcBSIt!=rcBSIt_e && rcBSIt->hardwareID() != idDig; ++rcBSIt);
    if (rcBSIt==rcBSIt_e) {
      ATH_MSG_WARNING( "LArRawChannelContainer not in the expected order. Change of LArByteStream format?" );
      //Wrap-around
      for (rcBSIt=rawColl_fromBytestream->begin();rcBSIt!=currIt && rcBSIt->hardwareID() != idDig; ++rcBSIt);
      if (rcBSIt==currIt) {
	ATH_MSG_ERROR( "Channel " << m_LArOnlineIDHelper->channel_name(idDig) << " not found." );
	return StatusCode::FAILURE;
      }
    }

    const LArDigit* dig=NULL;
//  For Eon/Eoff dump
//    if (m_dumpDigits) {
      unsigned index=rcDigIt-rawColl_fromDigits->begin();
      const unsigned digContSize=pLArDigitContainer->size();
      for(;index<digContSize && pLArDigitContainer->at(index)->hardwareID()!=idDig;++index);
      if (index==digContSize) {
	ATH_MSG_ERROR( "Can't find LArDigit corresponding to channel " << m_LArOnlineIDHelper->channel_name(idDig) << ". Turn off digit dump" );
	closeDumpfiles();
	m_dumpDigits=false;
	m_doDspTestDump=false;
	m_doCellsDump=false;
      }
      else{
	dig=pLArDigitContainer->at(index);
      }
//  }
    short minSamples = 4095;
    short maxSamples = 0;
    if (dig){
      const std::vector<short>& samples=dig->samples();
      for (unsigned int k = 0; k<samples.size(); k++) {
	if (samples.at(k) > maxSamples) maxSamples = samples.at(k);
	if (samples.at(k) < minSamples) minSamples = samples.at(k);
      }
    }

    if ((maxSamples-minSamples) > m_adc_th || m_adc_th <= 0) compareChannels(idDig,(*rcDigIt),(*rcBSIt),dig).ignore();
    else {
      if (dig) ATH_MSG_DEBUG( "Samples : "<< maxSamples << " " << minSamples );
    }      

  }//end loop over rawColl_fromDigits
  ATH_MSG_DEBUG( "End of rawChannels loop" );

  if(m_IsOnline && m_last_lb < m_curr_lb && (m_curr_lb - m_last_lb) % m_history_granularity == m_history_granularity -1 ) { // if new LB subtract the old for Out_[E,Q,T]_FT_vs_SLOT 
    ATH_MSG_INFO( " Last LB: "<<m_last_lb<<" curr. LB: "<<m_curr_lb<<" subtracting histos");
    // subtract all previously not done bins....
    int hbin = (m_curr_lb % m_history_size) / m_history_granularity;
    for(int i=0; i<=hbin; ++i) {
      if(!m_hdone[i]) {
        ATH_MSG_INFO( " Going to subtract and reset the: "<< i << " bin of shadow histo");
        for(unsigned l=0; l<m_histos.size(); ++l) {// go through all histo groups
           m_histos[l].m_hOut_E_FT_vs_SLOT->Add(m_histos[l].m_hOut_E_FT_vs_SLOT_shadow[i], -1.);
           m_histos[l].m_hOut_E_FT_vs_SLOT_shadow[i]->Reset();
           m_histos[l].m_hOut_Q_FT_vs_SLOT->Add(m_histos[l].m_hOut_Q_FT_vs_SLOT_shadow[i], -1.);
           m_histos[l].m_hOut_Q_FT_vs_SLOT_shadow[i]->Reset();
           m_histos[l].m_hOut_T_FT_vs_SLOT->Add(m_histos[l].m_hOut_T_FT_vs_SLOT_shadow[i], -1.);
           m_histos[l].m_hOut_T_FT_vs_SLOT_shadow[i]->Reset();
        }// histos.size
        m_hdone[i]=true;
      }
    }// hbin
  }

  for (unsigned i=0;i<m_LArOnlineIDHelper->febHashMax();++i) {
    const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(i);
    const PARTITION p=getPartition(febid);
    m_histos[p].m_Sweet->Fill(m_errsPerFEB[i]);
  }

  ERRCOUNTER allEC;  
  unsigned allErr_E=0;
  unsigned allErr_T=0;
  unsigned allErr_Q=0;

  for (unsigned g=0;g<3;++g) {
    for (unsigned p=0;p<N_PARTITIONS;++p) {
      allEC.errors_E[g]+=m_errcounters[p].errors_E[g];
      allEC.errors_T[g]+=m_errcounters[p].errors_T[g];
      allEC.errors_Q[g]+=m_errcounters[p].errors_Q[g];
    }
    allErr_E+=allEC.errors_E[g];
    allErr_T+=allEC.errors_T[g];
    allErr_Q+=allEC.errors_Q[g];
  }



  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE( "*Number of errors in Energy Computation : " );
    ATH_MSG_VERBOSE( "*     Low Gain : " << MSG::dec << allEC.errors_E[2] << " / " << m_count_gain[2] );
    ATH_MSG_VERBOSE( "*     Medium Gain : " << MSG::dec << allEC.errors_E[1] << " / " << m_count_gain[1] );
    ATH_MSG_VERBOSE( "*     High Gain : " << MSG::dec << allEC.errors_E[0] << " / " << m_count_gain[0] );
    ATH_MSG_VERBOSE( "*Number of errors in Time Computation : " );
    ATH_MSG_VERBOSE( "*     Low Gain : " << MSG::dec << allEC.errors_T[2] << " / " << m_count_gain[2] );
    ATH_MSG_VERBOSE( "*     Medium Gain : " << MSG::dec << allEC.errors_T[1] <<  " / " << m_count_gain[1] );
    ATH_MSG_VERBOSE( "*     High Gain : " << MSG::dec << allEC.errors_T[0] << " / " << m_count_gain[0] );
    ATH_MSG_VERBOSE( "*Number of errors in Quality Computation : " << " / " << m_count_gain[2] );
    ATH_MSG_VERBOSE( "*     Low Gain : " << MSG::dec << allEC.errors_Q[2] << " / " << m_count_gain[2] );
    ATH_MSG_VERBOSE( "*     Medium Gain : " << MSG::dec << allEC.errors_Q[1] << " / " << m_count_gain[1] );
    ATH_MSG_VERBOSE( "*     High Gain : " << MSG::dec << allEC.errors_Q[0] << " / " << m_count_gain[0] );
  }// end if verbose
  

  for (unsigned p=0;p<N_PARTITIONS;++p) {
    unsigned allErrsPartE=0;
    unsigned allErrsPartT=0;
    unsigned allErrsPartQ=0;
    for (unsigned g=0;g<3;++g) {
      m_hSummaryErrors_Energy->Fill((float)p,(float)g,(float)m_errcounters[p].errors_E[g]);
      m_hSummaryErrors_Time->Fill((float)p,(float)g,(float)m_errcounters[p].errors_T[g]);
      m_hSummaryErrors_Quality->Fill((float)p,(float)g,(float)m_errcounters[p].errors_Q[g]);
      allErrsPartE+=m_errcounters[p].errors_E[g];
      allErrsPartT+=m_errcounters[p].errors_T[g];
      allErrsPartQ+=m_errcounters[p].errors_Q[g];
    }
    m_hEErrors_LB_part->Fill((float)m_curr_lb,(float)p,(float)allErrsPartE);
    m_hTErrors_LB_part->Fill((float)m_curr_lb,(float)p,(float)allErrsPartT);
    m_hQErrors_LB_part->Fill((float)m_curr_lb,(float)p,(float)allErrsPartQ);
  }

  for(int str = 0; str < nStreams + 1; str++) {
    if (hasStream[str] == 1) {
      m_hEErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_E);
      m_hTErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_T);
      m_hQErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_Q);
    }
  }
  
  if (m_IsOnline && m_curr_lb - m_last_lb >= m_history_size) {
      m_last_lb = m_curr_lb;
      for(int i=0; i<m_hsize; ++i) m_hdone[i]=false;
  }

  return StatusCode::SUCCESS;
}

void LArRODMonTool::DumpCellEvent(int count,                            // counter
				  ILArOFC::OFCRef_t ofc,                 // vector containing OFC's
				  ILArOFC::OFCRef_t ofcb,                // vector containing OFCb's
				  ILArShape::ShapeRef_t ofch,            // vector containing OFCh's
				  ILArShape::ShapeRef_t ofcd,            // vector containing OFCd's
				  float Escale,                         // ADC2MeV factor
				  float Ramp0,                         // ramp intercept
				  float pedestal,                       // pedestal
				  const std::vector<short>* digits,     // vector containing samples (digits)
				  int E_off,                            // Energy calculated offline (i.e. fromDigits)
				  int E_on,                             // Energy calculated online (i.e. fromBytestream)
				  FILE* ofcfile,                    // output file containing ofc's
				  ofstream& digitsfile,                 // output file containing digits
				  ofstream& energyfile,                 // output file containing energies
				  HWIdentifier chid,                    // Channel HW ID
				  int event)                            // Event #
{
  // Dumping file with coefficients
  const int channel=m_LArOnlineIDHelper->channel(chid);
  const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(chid);
  ATH_MSG_VERBOSE("Dumping OFC, energies and digits for cell " << channel << " of FEB " << febid);

  float sumai = 0.;
  for (unsigned int a = 0; a<ofc.size(); a++) {
    sumai += ofc.at(a);
  }
  float pedplusoffset;
  if (Escale*sumai != 0) pedplusoffset = pedestal - Ramp0/(Escale*sumai);
  else pedplusoffset = 0;

  fprintf(ofcfile,"\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
	  channel,  pedestal, pedplusoffset,  
	  ofc.at(0)*Escale,  ofc.at(1)*Escale,  ofc.at(2)*Escale,  ofc.at(3)*Escale,  ofc.at(4)*Escale,
	  ofcb.at(0)*Escale,  ofcb.at(1)*Escale,  ofcb.at(2)*Escale,  ofcb.at(3)*Escale,  ofcb.at(4)*Escale,
	  ofch.at(0)/Escale,  ofch.at(1)/Escale,  ofch.at(2)/Escale,  ofch.at(3)/Escale,  ofch.at(4)/Escale,
	  ofcd.at(0)/Escale,  ofcd.at(1)/Escale,  ofcd.at(2)/Escale,  ofcd.at(3)/Escale,  ofcd.at(4)/Escale);
  //	  0.0, 0.0, 0.0, 0.0, 0.0);

  // Dumping file with offline energy and online energ
  energyfile << count << "   " << E_off << "   " << E_on ;
  energyfile << "     // FEB " << febid.get_identifier32().get_compact() << " ( channel " << channel << " ), event " << event << std::endl;
 
  // Dumping file with digits
  digitsfile << channel << "   ";
  for (unsigned int k = 0; k<digits->size(); k++) {
    digitsfile << digits->at(k) << " ";
  }
  digitsfile << "     // FEB " << febid.get_identifier32().get_compact() << " ( channel " << channel << " ), event " << event << std::endl;
}

void LArRODMonTool::DumpCellInfo(HWIdentifier chid,                    // FEB HW ID 
				 //int channel,                           // channel #
				 //HWIdentifier CellID,                   // Channel HW ID
				 //int slot,                              // slot 
				 //int FT,                                // Feedthrough
				 //int barrel_ec,                         
				 //int posneg, 
				 //std::string part,
				 const char* partc,
				 int gain,
				 int event, 
				 int E_off,                             // Energy calculated offline (i.e. fromDigits)  
				 int E_on,                              // Energy calculated online (i.e. fromBytestream)
				 float T_off,                             // Time calculated offline (i.e. fromDigits)  
				 float T_on,                              // Time calculated online (i.e. fromBytestream)
				 float Q_off,                             // Quality Factor calculated offline (i.e. fromDigits)  
				 float Q_on,                              // Quality Factor calculated online (i.e. fromBytestream)
				 FILE* dumpfile)
{

  // Dumping cell information in a text file so that it can be compared to unhappy channels lists for instance ...

  const int barrel_ec=m_LArOnlineIDHelper->barrel_ec(chid);
  const int posneg=m_LArOnlineIDHelper->pos_neg(chid);
  const int slot = m_LArOnlineIDHelper->slot(chid);
  const int FT = m_LArOnlineIDHelper->feedthrough(chid);
  const int channel =  m_LArOnlineIDHelper->channel(chid);
  const HWIdentifier febid= m_LArOnlineIDHelper->feb_Id(chid);
  

  fprintf(dumpfile,"0x%8x     ",febid.get_identifier32().get_compact());
  fprintf(dumpfile,"%3d     ",channel);
  fprintf(dumpfile,"0x%8x     ",chid.get_identifier32().get_compact());
  fprintf(dumpfile,"%2d     ",slot);
  fprintf(dumpfile,"%2d     ",FT);
  fprintf(dumpfile,"%2d     ",barrel_ec); 
  fprintf(dumpfile,"%2d     ",posneg);
  fprintf(dumpfile,"%7s     ",partc);
  //fprintf(dumpfile,"%3d     ",(int)eta);
  //fprintf(dumpfile,"%3d     ",(int)phi);
  fprintf(dumpfile,"%3d     ",gain);
  //fprintf(dumpfile,"%3d     ",isConnected);
  fprintf(dumpfile,"%5d     ",E_off);
  fprintf(dumpfile,"%5d     ",E_on);
  fprintf(dumpfile,"%5f     ",T_off);
  fprintf(dumpfile,"%5f     ",T_on);
  fprintf(dumpfile,"%5f     ",Q_off);
  fprintf(dumpfile,"%5f     ",Q_on);
  fprintf(dumpfile,"%3d     \n",event);
}

StatusCode LArRODMonTool::finalize() {
  ATH_MSG_VERBOSE("Finalize LArRODMonTool");
  closeDumpfiles();
  return StatusCode::SUCCESS;
} 
 
void LArRODMonTool::closeDumpfiles() {
  // Close output files
  if (m_doDspTestDump) {
    std::fclose(m_fai);
    m_fdig.close();
    m_fen.close();
  }
  if (m_doCellsDump) {
    std::fclose(m_fdump);
  }
}

StatusCode LArRODMonTool::compareChannels(const HWIdentifier chid,const LArRawChannel& rcDig, const LArRawChannel& rcBS, const LArDigit* dig) {
  ATH_MSG_DEBUG( " I am entering compareChannels method" );
  const int slot_fD = m_LArOnlineIDHelper->slot(chid);
  const  int feedthrough_fD = m_LArOnlineIDHelper->feedthrough(chid);
  const float timeOffline = rcDig.time()/m_unit_offline - m_timeOffset*m_BC;

  const unsigned q_gain = (int)rcDig.gain();


  const float& en_fB=rcBS.energy();
  const float& en_fD=rcDig.energy();
  const float abs_en_fB=fabs(en_fB);

  if (fabs(timeOffline) > m_peakTime_cut*1000.){
    ATH_MSG_DEBUG( " timeOffline too large " << timeOffline );
    return StatusCode::SUCCESS;
  }
// Set the cuts corresponding to the range
  int energyrange, DECut, DTCut, DQCut;
  if (abs_en_fB < m_range_E_0) {
    DECut = m_precision_E_0;
    energyrange=0;
  }
  else if (abs_en_fB < m_range_E_1) {
    DECut = m_precision_E_1;
    energyrange=1;
  }
  else if (abs_en_fB < m_range_E_2) {
    DECut = m_precision_E_2;
    energyrange=2;
  }
  else if (abs_en_fB < m_range_E_3) {
    DECut = m_precision_E_3;
    energyrange=3;
  }
  else { //>= m_range_E_3
    DECut = m_precision_E_max;
    energyrange=4;
  } // not sure of this value ... seems the max, though ...

  const float& t_fB=rcBS.time();
  const float abs_t_fB=abs(t_fB);
  if (abs_t_fB < m_range_T_0) DTCut = m_precision_T_0;
  else if (abs_t_fB < m_range_T_1) 
    DTCut = m_precision_T_1;
  else if (abs_t_fB < m_range_T_2) 
    DTCut = m_precision_T_2;
  else if (abs_t_fB < m_range_T_3) 
    DTCut = m_precision_T_3;
  else DTCut = m_precision_T_max; // not sure of this value ... seems the max, though ...

  const float q_fB=rcBS.quality();
  const float abs_q_fB=abs(q_fB);
  const float q_fD=rcDig.quality();


  if (abs_q_fB < m_range_Q_0) 
    DQCut = m_precision_Q_0;
  else if (abs_q_fB < m_range_Q_1) 
    DQCut = m_precision_Q_1;
  else if (abs_q_fB < m_range_Q_2) 
    DQCut = m_precision_Q_2;
  else if (abs_q_fB < m_range_Q_3) 
    DQCut = m_precision_Q_3;
  else
    DQCut = m_precision_Q_max; // not sure of this value ... seems the max, though ...


  // Variable used to reset the histos ranges
  const float& maxE1 = en_fD;
  const float& maxE2 = en_fB;
  const float& maxT1 = timeOffline;
  const float& maxT2 = t_fB;
  const float& maxQ1 = q_fD;
  const float& maxQ2 = q_fB;
  const float DiffE = en_fD - en_fB;
  const float DiffT = timeOffline - t_fB; // because time offline in ps and time online in ns

  //Flags
  const bool keepE = true;
  bool keepT = true;
  bool keepQ = true;

  float DiffQ = 65535.0; 
  if (rcDig.quality() > 0.) DiffQ = (rcDig.quality() - q_fB)/TMath::Sqrt(rcDig.quality());

  // If q is not calculated online/offline, don't keep event ?
  if (m_skipNullQT && (q_fB <= 0 ||  rcDig.quality()<= 0)) {
    keepT = false;
    keepQ = false;
  } 

  // if overflow for T, skip the event
  if (fabs(timeOffline) > 327680) { // 2**15 * 10 ps
    keepT = false;
  }

  const PARTITION p=getPartition(chid);
  HistGroup& hg=m_histos[p];
  ERRCOUNTER& ec=m_errcounters[p];
  
  if (keepE) {//Energy histograms:
    //Partition histogram
    if (fabs(hg.m_hDE->GetXaxis()->GetXmax()) < fabs(DiffE) && m_IsOnline==true ) { // Set the histo axis
      CAN_REBIN(hg.m_hDE);
      hg.m_hDE->RebinAxis(DiffE,hg.m_hDE->GetXaxis());
    }
    hg.m_hDE->Fill(DiffE);
    hg.m_hDE_ranges->Fill(DiffE,energyrange);

    //'ALL' histogram
    if (fabs(m_hE_all->GetXaxis()->GetXmax()) < fabs(DiffE) && m_IsOnline==true ) {
      CAN_REBIN(m_hE_all);
      m_hE_all->RebinAxis(DiffE,m_hE_all->GetXaxis());
    }
    m_hE_all->Fill(DiffE);
    m_hE_ranges_all->Fill(DiffE,energyrange);
    
    //online vs offline histograms
    if (fabs(hg.m_hEon_VS_Eoff->GetXaxis()->GetXmax()) < fabs(maxE1) && m_IsOnline==true) {
      CAN_REBIN(hg.m_hEon_VS_Eoff);
      hg.m_hEon_VS_Eoff->RebinAxis(maxE1,hg.m_hEon_VS_Eoff->GetXaxis());
    }
    if (fabs(hg.m_hEon_VS_Eoff->GetYaxis()->GetXmax()) < fabs(maxE2) && m_IsOnline==true ) {
      CAN_REBIN(hg.m_hEon_VS_Eoff);
      hg.m_hEon_VS_Eoff->RebinAxis(maxE2,hg.m_hEon_VS_Eoff->GetYaxis());
    }
    hg.m_hEon_VS_Eoff->Fill(rcDig.energy(),en_fB);
    // For Eon/Eoff  or Qon/Qoff dump
    const float hvscale = m_dd_HVScaleCorr->HVScaleCorr(chid);
    //if ( ((fabs(DiffE) > DECut && hvscale == 1.) || (keepQ && (fabs(DiffQ) > DQCut)))  && dig) {
    if ( ((fabs(DiffE) > DECut) || (keepQ && (fabs(DiffQ) > DQCut)))  && dig) {
    // absolute cut on energy 1.MeV
    //if ( ((fabs(DiffE) > 1.) || (keepQ && (fabs(DiffQ) > DQCut)))  && dig) {
      ++ec.errors_E[q_gain];
      hg.m_hOut_E_FT_vs_SLOT->Fill(slot_fD,feedthrough_fD);
      if(m_IsOnline) {
        hg.m_hOut_E_FT_vs_SLOT_shadow[(m_curr_lb % m_history_size) / m_history_granularity ]->Fill(slot_fD,feedthrough_fD);
      }
      //adding dumper
      if(m_IsOnline && ndump<max_dump) {
         const int channel=m_LArOnlineIDHelper->channel(chid);
         const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(chid);
         ATH_MSG_INFO( "Channel: " << channel << " of FEB " << febid );
         //if(fabs(DiffE) > DECut && hvscale == 1.) { // only for channels without HV corrections
         if(fabs(DiffE) > DECut ) { 
         // absolute cut on energy 1.MeV
         //if(fabs(DiffE) > 1.) {
             ATH_MSG_INFO( "DSP Energy Error : " << m_LArOnlineIDHelper->channel_name(chid) );
             ATH_MSG_INFO( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
	     		        << " , Eoff - Eonl = " << rcDig.energy() - en_fB );
             ATH_MSG_INFO( "   Qonl = " << q_fB << " , Qoff = " << rcDig.quality()
	     		        << " (Qoff - Qnl)/sqrt(Qoff) = " << (rcDig.quality() - q_fB)/TMath::Sqrt(rcDig.quality())  );
         }
         if(keepQ && (fabs(DiffQ) > DQCut)) {
             ATH_MSG_INFO( "DSP Quality Error : " << m_LArOnlineIDHelper->channel_name(chid) );
             ATH_MSG_INFO( "   Qonl = " << q_fB << " , Qoff = " << rcDig.quality()
	     		        << " (Qoff - Qnl)/sqrt(Qoff) = " << (rcDig.quality() - q_fB)/TMath::Sqrt(rcDig.quality())  );
             ATH_MSG_INFO( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
	     		        << " , Eoff - Eonl = " << rcDig.energy() - en_fB );
         }
         ATH_MSG_INFO( " Tauonl = " << rcBS.time() << " , Tauoff = " << rcDig.time() );
         const std::vector<short>& samples=dig->samples();
         std::ostringstream os; 
         os << "Digits : ";
         for (unsigned int k = 0; k<samples.size(); k++) {os << samples.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArOFC::OFCRef_t this_OFC_a_test = m_dd_ofc->OFC_a(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "OFCa : ";
         for (unsigned int k = 0; k<this_OFC_a_test.size(); ++k) {os << this_OFC_a_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArOFC::OFCRef_t this_OFC_b_test = m_dd_ofc->OFC_b(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "OFCb : ";
         for (unsigned int k = 0; k<this_OFC_b_test.size(); ++k) {os << this_OFC_b_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArShape::ShapeRef_t this_Shape_test = m_dd_shape->Shape(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "Shape : ";
         for (unsigned int k = 0; k<this_Shape_test.size(); ++k) {os << this_Shape_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArShape::ShapeRef_t this_ShapeDer_test = m_dd_shape->ShapeDer(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "ShapeDer : ";
         for (unsigned int k = 0; k<this_ShapeDer_test.size(); ++k) {os << this_ShapeDer_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         const std::vector<float>& ramp=m_adc2mevtool->ADC2MEV(chid,rcDig.gain());
         const float escale = ramp[1];
         float ramp0 = ramp[0];
         if (q_gain == 0) ramp0 = 0.; // no ramp intercepts in HG
         const float ped = m_larpedestal->pedestal(chid,rcDig.gain());
         ATH_MSG_INFO( "Escale: "<<escale<<" intercept: "<<ramp0<<" pedestal: "<<ped<<" gain: "<<rcDig.gain() );
         const Identifier cellid=m_cable_service_tool->cnvToIdentifier(chid);
         CaloDetDescrElement* cellDDE = m_calo_description_mgr->get_element(cellid); 
         const float noise=m_calo_noise_tool->totalNoiseRMS(cellDDE,rcDig.gain(),20.);
         ATH_MSG_INFO( "Noise for mu=20: "<<noise);
         ATH_MSG_INFO( "HVScaleCorr: "<<hvscale);
         double emon=0.;
	 const unsigned nOFCSamp=std::min(samples.size(),this_OFC_a_test.size());
         for (unsigned k=0; k<nOFCSamp; ++k) emon += (samples.at(k)-ped)*this_OFC_a_test.at(k);
         emon *= escale;
         emon += ramp0;
         ATH_MSG_INFO( "intercept + Escale*Sum[(sample-ped)*OFCa] "<<emon);
       ++ndump;
      } // DE cut
    } // dumper
  }// end energy histograms

  if (keepT) { //Time histograms
    //partition histogram
    if (fabs(hg.m_hDT->GetXaxis()->GetXmax()) < fabs(DiffT) && m_IsOnline==true ) { // Set the histo axis
      CAN_REBIN(hg.m_hDT);
      hg.m_hDT->RebinAxis(DiffT,hg.m_hDT->GetXaxis());
    }
    hg.m_hDT->Fill(DiffT);

    //'ALL' histogram:
    if (fabs(m_hT_all->GetXaxis()->GetXmax()) < fabs(DiffT) && m_IsOnline==true) {
      CAN_REBIN(m_hT_all);
      m_hT_all->RebinAxis(DiffT,m_hT_all->GetXaxis());
    }
    m_hT_all->Fill(DiffT);

    //online vs offline histogram
    if (fabs(hg.m_hTon_VS_Toff->GetXaxis()->GetXmax()) < fabs(maxT1) && m_IsOnline==true) {
      CAN_REBIN(hg.m_hTon_VS_Toff);
      hg.m_hTon_VS_Toff->RebinAxis(maxT1,hg.m_hTon_VS_Toff->GetXaxis());
    }
    if (fabs(hg.m_hTon_VS_Toff->GetYaxis()->GetXmax()) < fabs(maxT2) && m_IsOnline==true) {
      CAN_REBIN(hg.m_hTon_VS_Toff);
      hg.m_hTon_VS_Toff->RebinAxis(maxT2,hg.m_hTon_VS_Toff->GetYaxis());
    }
    hg.m_hTon_VS_Toff->Fill(timeOffline,t_fB);
    
    if (fabs(DiffT) > DTCut) {
      ++(ec.errors_T[q_gain]);
      hg.m_hOut_T_FT_vs_SLOT->Fill(slot_fD,feedthrough_fD);
      if(m_IsOnline) {
        hg.m_hOut_T_FT_vs_SLOT_shadow[(m_curr_lb % m_history_size) / m_history_granularity]->Fill(slot_fD,feedthrough_fD);
      }
    }
  } //end time histos
  
  
  if (keepQ) { //Quality histograms
    //Partition histogram
    if (fabs(hg.m_hDQ->GetXaxis()->GetXmax()) < fabs(DiffQ) && m_IsOnline==true) { // Set the histo axis
      CAN_REBIN(hg.m_hDQ);
      hg.m_hDQ->RebinAxis(DiffQ,hg.m_hDQ->GetXaxis());
    }
    hg.m_hDQ->Fill(DiffQ);

    //'ALL' histograms
    if (fabs(m_hQ_all->GetXaxis()->GetXmax()) < fabs(DiffQ) && m_IsOnline==true) {
      CAN_REBIN(m_hQ_all);
      m_hQ_all->RebinAxis(DiffQ,m_hQ_all->GetXaxis());
    }
    m_hQ_all->Fill(DiffQ);

    //online vs offline histograms
    if (fabs(hg.m_hQon_VS_Qoff->GetXaxis()->GetXmax()) < fabs(maxQ1) && m_IsOnline==true) {
      CAN_REBIN(hg.m_hQon_VS_Qoff);
      hg.m_hQon_VS_Qoff->RebinAxis(maxQ1,hg.m_hQon_VS_Qoff->GetXaxis());
    }
    if (fabs(hg.m_hQon_VS_Qoff->GetYaxis()->GetXmax()) < fabs(maxQ2) && m_IsOnline==true) {
      CAN_REBIN(hg.m_hQon_VS_Qoff);
      hg.m_hQon_VS_Qoff->RebinAxis(maxQ2,hg.m_hQon_VS_Qoff->GetYaxis());
    }
    hg.m_hQon_VS_Qoff->Fill(rcDig.quality(),q_fB);

    if (fabs(DiffQ) > DQCut) {
      ++(ec.errors_Q[q_gain]);
      hg.m_hOut_Q_FT_vs_SLOT->Fill(slot_fD,feedthrough_fD);
      if(m_IsOnline) {
        hg.m_hOut_Q_FT_vs_SLOT_shadow[(m_curr_lb % m_history_size) / m_history_granularity]->Fill(slot_fD,feedthrough_fD);
      }
    }
  }

  if (m_printEnergyErrors && msgLvl(MSG::DEBUG)) {
    if (fabs(rcDig.energy() - en_fB) > DECut) {
      ATH_MSG_DEBUG( "DSP Energy Error : " << m_LArOnlineIDHelper->channel_name(chid) );
      ATH_MSG_DEBUG( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
			<< " , Eoff - Eonl = " << rcDig.energy() - en_fB );
    }
  }
 
  if (keepQ) {
    const HWIdentifier febId=m_LArOnlineIDHelper->feb_Id(chid);
    const IdentifierHash febHash=m_LArOnlineIDHelper->feb_Hash(febId);
    ++(m_errsPerFEB[febHash]);
  }



  //Detailed text dump if requested:
  if (dig) {
    if ((keepE && fabs(DiffE)>DECut) || (keepT && fabs(DiffT)>DTCut) || (keepQ && DiffQ > DQCut)) {
      if (m_doDspTestDump) {
	int Ind = 0;
	const float timeBinWidth_test=m_dd_ofc->timeBinWidth(chid,q_gain);
	unsigned delayIdx_test=(unsigned)floor(0.5+Ind/timeBinWidth_test);
	ILArOFC::OFCRef_t this_OFC_a_test = m_dd_ofc->OFC_a(chid,q_gain,delayIdx_test);
	ILArOFC::OFCRef_t this_OFC_b_test = m_dd_ofc->OFC_b(chid,q_gain,delayIdx_test);
	ILArShape::ShapeRef_t this_OFC_h_test = m_dd_shape->Shape(chid,q_gain,delayIdx_test);
	ILArShape::ShapeRef_t this_OFC_d_test = m_dd_shape->ShapeDer(chid,q_gain,delayIdx_test);	
	const std::vector<float>& ramp=m_adc2mevtool->ADC2MEV(chid,q_gain);
	const std::vector<short>& samples=dig->samples();
	const float escale = ramp[1];
	float ramp0 = ramp[0];
	if (q_gain == 0) ramp0 = 0.; // no ramp intercepts in HG
	const float ped = m_larpedestal->pedestal(chid,q_gain);
	this->DumpCellEvent((int)m_counter,this_OFC_a_test,this_OFC_b_test,this_OFC_h_test,this_OFC_d_test,escale,ramp0,ped,&samples,(int)(rcDig.energy()),(int)(en_fB),m_fai,m_fdig,m_fen, chid, (int)m_eventsCounter);
      }
      if (m_doCellsDump) {
	this->DumpCellInfo(chid,hg.m_partName.c_str(),(int)q_gain,(int)m_eventsCounter,(int)rcDig.energy(),(int)(en_fB),rcDig.time(),t_fB,rcDig.quality(),q_fB,m_fdump);
      }
      m_counter++;
    }//end if E,t or Q cut passed
  }//end if dig
  ATH_MSG_DEBUG( " I am leaving compareChannels method" );
  return StatusCode::SUCCESS;
}


void LArRODMonTool::ERRCOUNTER::clear() {
  for (unsigned i=0;i<3;++i) {
    errors_E[i]=0;
    errors_T[i]=0;
    errors_Q[i]=0;
  }
  return;
}
