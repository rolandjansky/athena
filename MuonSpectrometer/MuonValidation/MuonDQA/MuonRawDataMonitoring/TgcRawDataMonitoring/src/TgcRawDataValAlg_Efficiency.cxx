/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

// Cabling Service
//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
//#include "MuonDQAUtils/TGCDQAUtils.h"

// Eigen
//#include "GeoPrimitives/GeoPrimitives.h"
//#include "EventPrimitives/EventPrimitives.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <math.h>

StatusCode 
TgcRawDataValAlg::bookHistogramsEfficiency(){
  //Following Histograms are booked
  //
  //break down to layer
  // Efficiency_{A,C} {,_Numerator,_Denominator}
  //
  //break down to gasgap
  // Wire_Efficiency_Map_{A,C} {,_Numerator,_Denominator} 
  // Strip_Efficiency_Map_{A,C} {,_Numerator,_Denominator}

  //break down to layer*sector (7*12)
  // Wire_Efficiency_A
  // Wire_Efficiency_C
  // Strip_Efficiency_A
  // Strip_Efficiency_C

  // Summary_Of_Wire_Efficiency_Per_Chamber_Type_Station[1-3]_{E,F[1-5]}_T[1-9]
  // Summary_Of_Strip_Efficiency_Per_Chamber_Type_Station[1-3]_{E,F[1-5]}_T[1-9]

  ATH_MSG_INFO( "bookHistogramsEfficiency"  );

  MonGroup tgcprd_eff_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Efficiency", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_eff_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Efficiency", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_eff_ac[2] = { &tgcprd_eff_a, &tgcprd_eff_c}; 

  MonGroup tgcprd_eff_expert_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Efficiency", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_eff_expert_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Efficiency", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_eff_expert_ac[2] = { &tgcprd_eff_expert_a, &tgcprd_eff_expert_c}; 

  MonGroup tgcprd_effnumdenom_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Efficiency/NumDenom", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_effnumdenom_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Efficiency/NumDenom", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_effnumdenom_ac[2] = { &tgcprd_effnumdenom_a, &tgcprd_effnumdenom_c}; 

  MonGroup tgcprd_effnumdenom_expert_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Efficiency/NumDenom", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_effnumdenom_expert_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Efficiency/NumDenom", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_effnumdenom_expert_ac[2] = { &tgcprd_effnumdenom_expert_a, &tgcprd_effnumdenom_expert_c}; 

  MonGroup tgcprd_summary( this, m_generic_path_tgcmonitoring + "/Global/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_summary_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_summary_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Summary", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_summary_ac[2] = { &tgcprd_summary_a, &tgcprd_summary_c};

  std::stringstream ss;
  std::string side[2] = {"A","C"};
  std::string wseff[2] = {"Wire_Efficiency", "Strip_Efficiency"};

  for(int ac=0;ac<2;ac++){
    //efficiency for A/C side, wire 1-7, strip 8-13
    ss.str(""); ss << "Efficiency_" << side[ac];
    m_tgceff[ac]=new TH1F(ss.str().c_str(), (ss.str() + ";;Efficiency").c_str(),13,0,13); 
    ATH_CHECK( tgcprd_eff_ac[ac]->regHist(m_tgceff[ac]) );
    
    //efficiency for A/C side numerator
    ss.str(""); ss << "Efficiency_" << side[ac] << "_Numerator";
    m_tgceffnum[ac]=new TH1F(ss.str().c_str(), (ss.str() + ";;Number of Events").c_str(),13,0,13);
    m_tgceffnum[ac]->Sumw2();
    ATH_CHECK( tgcprd_effnumdenom_ac[ac]->regHist(m_tgceffnum[ac]) );
    
    //efficiency for A side wire and stripdenominator
    ss.str(""); ss << "Efficiency_" << side[ac] << "_Denominator";
    m_tgceffdenom[ac]=new TH1F(ss.str().c_str(), (ss.str() + "Number of Events").c_str(),13,0,13);
    m_tgceffdenom[ac]->Sumw2();
    ATH_CHECK( tgcprd_effnumdenom_ac[ac]->regHist(m_tgceffdenom[ac]) );

    // no strip layer2
    //m_tgceffnum[ac]->Fill(8);
    //m_tgceffdenom[ac]->Fill(8);

    std::string sbc[2] = {"_Previous", "_Next"};

    for(int ws=0;ws<2;ws++){
      //Efficiency map
      ss.str(""); ss << wseff[ws] << "_Map_" << side[ac];
      m_tgceffmap[ac][ws] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
      ATH_CHECK( tgcprd_eff_ac[ac]->regHist(m_tgceffmap[ac][ws]) );
      
      //Efficiency map numerator
      ss.str(""); ss << wseff[ws] << "_Map_" << side[ac] << "_Numerator";
      m_tgceffmapnum[ac][ws] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
      ATH_CHECK( tgcprd_effnumdenom_ac[ac]->regHist(m_tgceffmapnum[ac][ws]) );
      
      //Efficiency map denominator
      ss.str(""); ss << wseff[ws] << "_Map_" << side[ac] << "_Denominator";
      m_tgceffmapdenom[ac][ws] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
      ATH_CHECK( tgcprd_effnumdenom_ac[ac]->regHist(m_tgceffmapdenom[ac][ws]) );
      
      //Efficiency map for previous and next when hits are not found in current BC
      for(int bc=0;bc<2;bc++){
        //Efficiency map
        ss.str(""); ss << wseff[ws] << "_Map" << sbc[bc] << "_" << side[ac];
        m_tgceffmapbc[ac][ws][bc] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
        ATH_CHECK( tgcprd_eff_expert_ac[ac]->regHist(m_tgceffmapbc[ac][ws][bc]) );
        
        // Numerator
        ss.str(""); ss << wseff[ws] << "_Map" << sbc[bc] << "_" << side[ac] << "_Numerator";
        m_tgceffmapnumbc[ac][ws][bc] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
        ATH_CHECK( tgcprd_effnumdenom_expert_ac[ac]->regHist(m_tgceffmapnumbc[ac][ws][bc]) );
      }
    }//ws
  }//ac

  for(int ac=0;ac<2;ac++){//side
    int k=0;
    for(int ws=0;ws<2;ws++)//wire/strip
      for(int layer=1;layer<=7;layer++){//layer
        if(ws==0){
          ss.str(""); ss << "Wire_";
        }else{
          if(layer==2)continue;
          ss.str(""); ss << "Strip_";
        }
        k++;
        ss<<"L"<<layer;
        m_tgceff[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
        m_tgceffnum[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
        m_tgceffdenom[ac]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
      }
  }

  std::string schamberT1[6]={"E1", "E2", "E3", "E4", "F"};
  std::string schamberT3[6]={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string schamberEF[2]={"EI", "FI"};

  for(int ac=0;ac<2;ac++){
    for(int ws=0;ws<2;ws++){
      BlankStationMap(m_tgceffmap[ac][ws]);  
      if(ws==1) BlankStripStationMap(m_tgceffmap[ac][ws]);
      for(int bc=0; bc<2; bc++){
        BlankStationMap(m_tgceffmapbc[ac][ws][bc]);
        BlankStationMap(m_tgceffmapnumbc[ac][ws][bc]);                                                                                                     
        if(ws==1){
          BlankStripStationMap(m_tgceffmapbc[ac][ws][bc]);
          BlankStripStationMap(m_tgceffmapnumbc[ac][ws][bc]);
        }
      }
      for(int sec=1;sec<=12;sec++){
        for(int phi=0;phi<=3;phi+=4){
          ss.str(""); ss << side[ac];
          if(sec<10)ss << "0";
          ss << sec << "phi" << phi;
          int k=(sec-1)*4+phi+1;
          for(int bc=0;bc<2;bc++){
            m_tgceffmapbc[ac][ws][bc]   ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
            m_tgceffmapnumbc[ac][ws][bc]->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
          }
          m_tgceffmap[ac][ws]     ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
          m_tgceffmapnum[ac][ws]  ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
          m_tgceffmapdenom[ac][ws]->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
        }
      }
      int x=1;
      ////layer1-3
      //for(int l=0;l<3;l++){
      //  for(int c=0;c<5;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberT1[c];
      //    for(int bc=0;bc<2;bc++){
      //      m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //      m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //    }
      //    m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}
      ////layer4-7
      //for(int l=3;l<7;l++){
      //  for(int c=0;c<6;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberT3[c]; 
      //    for(int bc=0;bc<2;bc++){
      //      m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //      m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //    }
      //    m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}
      ////layer8-9
      //for(int l=7;l<9;l++){
      //  for(int c=0;c<2;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberEF[c]; 
      //    for(int bc=0;bc<2;bc++){
      //      m_tgceffmapbc[ac][ws][bc] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //      m_tgceffmapnumbc[ac][ws][bc] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
      //    }
      //    m_tgceffmap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapnum[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgceffmapdenom[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}

      // chamber E1-E4,  layer1-7
      for(int c=0;c<4;c++){
	for(int l=1;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT1[c];
          for(int bc=0;bc<2;bc++){
            m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
            m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
          }
          m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );

	}
      }
      // chamber E5,  layer4-7
      for(int c=4;c<5;c++){
	for(int l=4;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT3[c];
          for(int bc=0;bc<2;bc++){
            m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
            m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
          }
          m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }
      // chamber F,  layer1-7
      for(int c=5;c<6;c++){
	for(int l=1;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT3[c];
          for(int bc=0;bc<2;bc++){
            m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
            m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
          }
          m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }
      // chamber EIFI,  layer8-9
      for(int c=0;c<2;c++){
	for(int l=8;l<=9;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberEF[c];
          for(int bc=0;bc<2;bc++){
            m_tgceffmapbc[ac][ws][bc]   ->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
            m_tgceffmapnumbc[ac][ws][bc]->GetXaxis()->SetBinLabel( x, ss.str().c_str() ); 
          }
          m_tgceffmap[ac][ws]     ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapnum[ac][ws]  ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgceffmapdenom[ac][ws]->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }



      m_tgceffmap[ac][ws] ->GetXaxis()->LabelsOption("v");
      m_tgceffmapnum[ac][ws] ->GetXaxis()->LabelsOption("v");
      m_tgceffmapdenom[ac][ws] ->GetXaxis()->LabelsOption("v");
      for(int bc=0;bc<2;bc++){
        m_tgceffmapbc[ac][ws][bc] ->GetXaxis()->LabelsOption("v");
        m_tgceffmapnumbc[ac][ws][bc] ->GetXaxis()->LabelsOption("v");
      }
    }//ws
  }//ac

  for(int i=0;i<2;i++){
    for(int ws=0;ws<2;ws++){
      // efficiency for each sector/layer
      ss.str(""); ss << wseff[ws] << "_" << side[i];
      m_tgceffsector[i][ws]=new TH1F(ss.str().c_str(), (ss.str() + ";;Efficiency").c_str(),84,0,84);
      ATH_CHECK( tgcprd_eff_expert_ac[i]->regHist(m_tgceffsector[i][ws]) );

      // efficiency numerator
      ss.str(""); ss << wseff[ws] << "_" << side[i] << "_Numerator";    
      m_tgceffsectornum[i][ws]=new TH1F(ss.str().c_str(), (ss.str() + ";;Efficiency" ).c_str(),84,0,84);       
      m_tgceffsectornum[i][ws]->Sumw2();
      ATH_CHECK( tgcprd_effnumdenom_expert_ac[i]->regHist(m_tgceffsectornum[i][ws]) );

      // efficiency denominator
      ss.str(""); ss << wseff[ws] << "_" << side[i] << "_Denominator";
      m_tgceffsectordenom[i][ws]=new TH1F(ss.str().c_str(),(ss.str() + ";;Efficiency" ).c_str(),84,0,84);
      m_tgceffsectordenom[i][ws]->Sumw2();
      ATH_CHECK( tgcprd_effnumdenom_expert_ac[i]->regHist(m_tgceffsectordenom[i][ws]) );
    }

    for(int ws=0;ws<2;ws++)
      for(int sector=1;sector<=12;sector++)
        for(int layer=1;layer<=7;layer++){
          ss.str(""); ss<<side[i];
          if(sector<10)ss<<0;
          ss<<sector<<"L"<<layer;
          int k=(sector-1)*7+layer;
          m_tgceffsector[i][ws]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
          m_tgceffsectornum[i][ws]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
          m_tgceffsectordenom[i][ws]->GetXaxis()->SetBinLabel(k,ss.str().c_str());
          ss.str();
        }

    //Summary
    for(int ws=0;ws<2;ws++){
      ss.str(""); ss<<"Summary_Of_"<<wseff[ws]<<"_Per_GasGap_"<<side[i];
      m_tgcsummaryofeffpergasgap[i][ws]=new TH1F(ss.str().c_str(), (ss.str() + ";Efficiency; Entries").c_str(), 101, 0., 1.01);
      ATH_CHECK( tgcprd_summary_ac[i]->regHist(m_tgcsummaryofeffpergasgap[i][ws]) );
    }      
  }

  std::string type[17]={"_F_T1", "_E4_T3", "_E3_T6", "_E2_T7", "_E1_T8",
                        "_F_T2", "_E5_T4", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9",
                        "_F_T2", "_E5_T5", "_E4_T6", "_E3_T7", "_E2_T8","_E1_T9" };
  
  for(int ws=0;ws<2;ws++){
    int ntype=0;
    for(int sta=0;sta<3;sta++){
      for(int eta=0;eta<6;eta++){
        if(sta==0 && eta==5 )continue;
        //Summary
        ss.str(""); ss<<"Summary_Of_"<<wseff[ws]<<"_Per_Chamber_Type_Station"<<sta+1<<type[ntype];
        m_tgcsummaryofeffperchambertype[ws][ntype]=new TH1F(ss.str().c_str(), (ss.str() + ";Efficiency; Entries").c_str(), 101, 0., 1.01);
        ATH_CHECK( tgcprd_summary.regHist(m_tgcsummaryofeffperchambertype[ws][ntype++]) );
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode
TgcRawDataValAlg::fillEfficiency(){

  vector<double> LptEta[2][2];//[ws][ac]        Eta associated lpt
  vector<double> LptPhi[2][2];//[ws][ac]        Phi associated lpt

  vector<double> EIFIEta[2][2];//[ws][ac]        Eta associated lpt
  vector<double> EIFIPhi[2][2];//[ws][ac]        Phi associated lpt

  //vector<double> MDTSegmentEta[2][3];//[ac][IMO]
  //vector<double> CSCSegmentEta[2][3];//[ac][IMO]

  // Retrieve current coincidence container from storegate
  const Muon::TgcCoinDataContainer* tgc_trg_container(0);
  ATH_CHECK( (*m_activeStore)->retrieve(tgc_trg_container, m_outputCoinCollectionLocation) );
  
  // Loop over TGCCoinContainer
  Muon::TgcCoinDataContainer::const_iterator it_end=tgc_trg_container->end();
  for( Muon::TgcCoinDataContainer::const_iterator it=tgc_trg_container->begin();
       it!=it_end;
       ++it){
    if(it == it_end || (*it)->size()==0)continue;  //check if there are counts 
    ATH_MSG_DEBUG( "size of tgccoin collection is " << (*it) -> size()  );
    
    // Loop over Collection
    Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
    for( Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
         itc!= itc_end;
         ++itc){
      const Muon::TgcCoinData* tcd=*itc;
      
      // Only interested in LpT coincidences
      if( tcd->type() == Muon::TgcCoinData::TYPE_TRACKLET ){
        // Get Side and Wire/Strip indexes
        int ac=(tcd->isAside()==false);//isNotAside   a:0, c:1
        int ws=(tcd->isStrip());       //isStrip      w:0, s:1
        
        // Get channel position
        Identifier id = tcd->channelIdIn();
        const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(id);
        const Amg::Vector3D channelPos = pReadoutElementTGC->channelPos(id); //global position 
        
        // Add position information to vectors
        LptEta[ws][ac].push_back(channelPos.eta());
        LptPhi[ws][ac].push_back(channelPos.phi());
      }

      if( tcd->type() == Muon::TgcCoinData::TYPE_TRACKLET_EIFI ){
        // Get Side and Wire/Strip indexes
        int ac=(tcd->isAside()==false);//isNotAside   a:0, c:1
        int ws=(tcd->isStrip());       //isStrip      w:0, s:1
        
        // Get channel position
        Identifier id = tcd->channelIdIn();
        const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(id);
        const Amg::Vector3D channelPos = pReadoutElementTGC->channelPos(id); //global position 
        
        // Add position information to vectors
        EIFIEta[ws][ac].push_back(channelPos.eta());
        EIFIPhi[ws][ac].push_back(channelPos.phi());
      }


    }//collection
  }//container
  
  ///////////////////////////////////////////////////////////////////////////
  // Fill Midstation Efficiency
  for(int ac=0;ac<2;ac++){
    int nLpT[2]; //[ws]
    nLpT[WIRE] = LptEta[WIRE][ac].size();
    nLpT[STRP] = LptEta[STRP][ac].size();

    int nEIFI[2]; //[ws]
    nEIFI[WIRE] = EIFIEta[WIRE][ac].size();
    nEIFI[STRP] = EIFIEta[STRP][ac].size();

    for(int eta=0;eta<6;eta++){
      for(int phi48=0;phi48<48;phi48++){
        
        // Initialize channel data variables
        // channel number +offset for wires and strips in each layer
        int chIds[2][9] = {{-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,},
                           {-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,}};//[ws][layer]
        // Eta and Phi positions of channels　(layer2 has no strips)
        double chEtas[2][9] = {{-99,-99,-99,-99,-99,-99,-99,-99,-99,},
                               {-99, -1,-99,-99,-99,-99,-99,-99,-99,}};//[ws][layer]
        double chPhis[2][9] = {{-99,-99,-99,-99,-99,-99,-99,-99,-99,},
                               {-99, -1,-99,-99,-99,-99,-99,-99,-99,}};//[ws][layer]
        
        // Flags for whether a chamber-layer contains hits at the edge of the chamber
        bool edgehit[2][9] = {{false,false,false,false,false,false,false,false,false},
                              {false,false,false,false,false,false,false,false,false}};//[ws][layer]
        
        // Fill channel variable arrays for chambers with exactly 1 hit per layer
        for(int ws=0;ws<2;ws++){
          for(int l=0;l<9;l++){
            // Skip non-existent chambers
            if(m_SLBoffset[ws][ac][eta][l]<0) continue;
            // If exactly one channel with hit
            if(m_hitIdVects[CURR][ac][ws][eta][phi48][l].size()==1){
              // Fill channel variables
              chIds[ws][l] = m_tgcIdHelper->channel(m_hitIdVects[CURR][ac][ws][eta][phi48][l].at(0)) + m_SLBoffset[ws][ac][eta][l];
              const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(m_hitIdVects[CURR][ac][ws][eta][phi48][l].at(0));
              const Amg::Vector3D channelPos = pReadoutElementTGC->channelPos(m_hitIdVects[CURR][ac][ws][eta][phi48][l].at(0)); //global position 
              chEtas[ws][l] = channelPos.eta();
              chPhis[ws][l] = channelPos.phi();
            }
          }//layer
        }//ws
        
        // Flag hits on the last 3 channels at the edge of a chamber as edge hits
        for(int l=0;l<9;l++){
          // Flag wire edge hits
          int wsize = m_hitIdVects[CURR][ac][WIRE][eta][phi48][l].size();
          for(int hit=0;hit<wsize;hit++){
            int stationName = m_tgcIdHelper->stationName(m_hitIdVects[CURR][ac][WIRE][eta][phi48][l].at(hit));
            int stationEta  = abs(m_tgcIdHelper->stationEta(m_hitIdVects[CURR][ac][WIRE][eta][phi48][l].at(hit)));
            int channel     = m_tgcIdHelper->channel(m_hitIdVects[CURR][ac][WIRE][eta][phi48][l].at(hit));
            int stationPhi  = m_tgcIdHelper->stationPhi(m_hitIdVects[CURR][ac][WIRE][eta][phi48][l].at(hit));
            // Flag as edge hit if within 3 channels of chamber edge
            if(channel<=3 || channel>=getNumberOfWires(stationName, l, stationEta, stationPhi)-3){
              edgehit[WIRE][l] = true;
              break;
            }
          }
          // Flag strip edge hits
          int ssize = m_hitIdVects[CURR][ac][STRP][eta][phi48][l].size();
          for(int hit=0;hit<ssize;hit++){
            int ch = m_tgcIdHelper->channel(m_hitIdVects[1][ac][1][eta][phi48][l].at(hit));
            // Flag as edge hit if within 3 channels of chamber edge
            if(ch<= 3 || ch>=30){
              edgehit[STRP][l] = true;
              break;
            }
          }
        }//layer
            
        
        
        ///////////////////////////////////////////////////////////////////////////
        // Check Triplet 2 out of 3 coincidence
        // Checking the efficiency of a triplet layer requires verification of a good signal in adjacent layers and
        // corroberation with a pair of LpT triggers at the same point.
        // This is done by checking for the existence of a hit in all adjacent layers, checking that those hits weren't on
        // the edge of the chamber, checking that they all line up along more or less the same eta line, then checking that
        // they all line up with both a wire LpT trigger and a strip LpT trigger
        
        // Set upper limits for LpTvsPRD comparison acceptance
        double deta[2][2] = {{ 0.03,  0.3 },
                             {  0.3,  0.3 }};//[wsPRD][wsLpT]
        double dphi[2][2] = {{  0.3, 0.15 },
                             { 0.15, 0.02 }};//[wsPRD][wsLpT]
        
        // Loop over Triplet layers
        for(int layer=0;layer<3;layer++){// layer= layer being checked for Efficiency
          bool canCheck = true;
          
          // Check all adjacent layers verify a hit
          for(int refLayer=0;refLayer<3;refLayer++){if(layer==refLayer) continue;// refLayer= adjacent layer
            // Check that both Wire and Strip channels were found and are not edge hits
            if(chIds[WIRE][refLayer]<0||edgehit[WIRE][refLayer]){canCheck=false;break;}
            if(refLayer!=1)// no strips in layer 2
              if(chIds[STRP][refLayer]<0||edgehit[STRP][refLayer]){canCheck=false;break;}
          }
          if(!canCheck)continue;
          
          // Check hits in all adjacent layers line up in eta (channel numbering is consistent in eta)
          int refLayerA, refLayerB;// get indexes of adjacent layers
          if(layer==0)     {refLayerA=1; refLayerB=2;}
          else if(layer==1){refLayerA=0; refLayerB=2;}
          else if(layer==2){refLayerA=0; refLayerB=1;}
          else break;
          
          // Get allowable differences for channel comparison
          int dchmin[2] = {m_dchmin[refLayerA][refLayerB][WIRE][ac], m_dchmin[refLayerA][refLayerB][STRP][ac]};
          int dchmax[2] = {m_dchmax[refLayerA][refLayerB][WIRE][ac], m_dchmax[refLayerA][refLayerB][STRP][ac]};
          
          // Compare channels in refLayerA and refLayerB, no strips in layer2 so can only compare adjacent strips if layer= layer2
          if(!compareID(chIds[WIRE][refLayerA], chIds[WIRE][refLayerB], dchmin[WIRE], dchmax[WIRE])){canCheck=false; continue;}
          if(layer==1) if(!compareID(chIds[STRP][refLayerA], chIds[STRP][refLayerB], dchmin[STRP], dchmax[STRP])){canCheck=false; continue;}
          if(!canCheck)continue;
          
          // Check LpT for match with current hits
          bool LpTmatch[2] = {false,false};//[ws] Flags for LpT match found
          int  refLayer[2] = {refLayerA, (refLayerA==1?refLayerB:refLayerA)};//[ws] Layers to get hits from comparing with LpT
          
          // Loop over LpT
          for(int wsLpT=0;wsLpT<2;wsLpT++){
            for(int i=0;i<nLpT[wsLpT];i++){
              // Flag wire if LpT matches layer 2&3 hits
              if((fabs(chEtas[WIRE][refLayer[WIRE]]-LptEta[wsLpT][ac][i]) <deta[WIRE][wsLpT])&&
                 (fabs(chPhis[WIRE][refLayer[WIRE]]-LptPhi[wsLpT][ac][i]) <dphi[WIRE][wsLpT])&&
                 (fabs(chEtas[STRP][refLayer[STRP]]-LptEta[wsLpT][ac][i]) <deta[STRP][wsLpT])&&
                 (fabs(chPhis[STRP][refLayer[STRP]]-LptPhi[wsLpT][ac][i]) <dphi[STRP][wsLpT])){
                LpTmatch[wsLpT]=true;
                break;
              }
            }
          }
          
          if(LpTmatch[WIRE] && LpTmatch[STRP]){
            // If both wire and strip LpT matches found, calculate efficiency for current layer
            calculateEfficiency(ac, WIRE, eta, phi48, layer);
            if(layer!=1)calculateEfficiency(ac, STRP, eta, phi48, layer);
          }
        }
        
        
        ///////////////////////////////////////////////////////////////////////////
        // Check Doublet pair, 3 out of 4 coincidence
        /////////////////////////////////////
        // Checking the efficiency of a doublet layer first requires the verification of a good signal in adjacent layers
        // This is done by checking for the existence of a hit in all adjacent layers, checking that those hits weren't on
        // the edge of the chamber, then checking that they all line up along more or less the same eta line.
        
        // Loop through Doublet Layers
        for(int layer=3;layer<7;layer++){// layer= layer being checked for Efficiency
          bool canCheck = true;
          
          // Check all adjacent layers verify a hit
          for(int refLayer=3;refLayer<7;refLayer++){if(layer==refLayer) continue;// refLayer= adjacent layer
            // Check that both Wire and Strip channels were found and are not edge hits
            if(chIds[WIRE][refLayer]<0||edgehit[WIRE][refLayer]){canCheck=false;break;}
            if(chIds[STRP][refLayer]<0||edgehit[STRP][refLayer]){canCheck=false;break;}
          }
          
          if(!canCheck)continue;
          // Check hits in all adjacent layers line up in eta (channel numbering is consistent in eta)
          for(int refLayerA=3;refLayerA<7;refLayerA++){if(layer==refLayerA) continue;// refLayerA= first adjacent layer
            for(int refLayerB=refLayerA+1;refLayerB<7;refLayerB++){if(layer==refLayerB) continue;// refLayerB= second adjacent layer
            
              // Get allowable differences for channel comparison
              int dchmin[2] = {m_dchmin[refLayerA][refLayerB][WIRE][ac], m_dchmin[refLayerA][refLayerB][STRP][ac]};
              int dchmax[2] = {m_dchmax[refLayerA][refLayerB][WIRE][ac], m_dchmax[refLayerA][refLayerB][STRP][ac]};
              
              // Compare channels in refLayerA and refLayerB
              if(!compareID(chIds[WIRE][refLayerA], chIds[WIRE][refLayerB], dchmin[WIRE], dchmax[WIRE])){canCheck=false;break;}
              if(!compareID(chIds[STRP][refLayerA], chIds[STRP][refLayerB], dchmin[STRP], dchmax[STRP])){canCheck=false;break;}
              
            }
          }
          
          if(canCheck){
            // If all necessary PRD exists and lines up, calculate wire and strip efficiency
            calculateEfficiency(ac, WIRE, eta, phi48, layer);
            calculateEfficiency(ac, STRP, eta, phi48, layer);
          }
        }

        ///////////////////////////////////////////////////////////////////////////
        // Check EIFI Doublet
        /////////////////////////////////////
        for(int layer=7;layer<9;layer++){// layer= layer being checked for Efficiency
          bool canCheck = true;
          
          // Check all adjacent layers verify a hit
          for(int refLayer=7;refLayer<9;refLayer++){if(layer==refLayer) continue;// refLayer= adjacent layer
            // Check that both Wire and Strip channels were found and are not edge hits
            if(chIds[WIRE][refLayer]<0||edgehit[WIRE][refLayer]){canCheck=false;break;}
            if(chIds[STRP][refLayer]<0||edgehit[STRP][refLayer]){canCheck=false;break;}
          }
          if(!canCheck) continue;
          // Check LpT for match with current hits
          bool EIFImatch[2] = {false,false};//[ws] Flags for LpT match found
          int  refLayerEIFI;
 	  if(layer==7) refLayerEIFI = 8;
          else if(layer==8) refLayerEIFI = 7;
	  else break;
          
          // Loop over EIFI trigger 
          for(int wsEIFI=0;wsEIFI<2;wsEIFI++){
            for(int i=0;i<nEIFI[wsEIFI];i++){
              // Flag wire if LpT matches layer 2&3 hits
              if((fabs(chEtas[WIRE][refLayerEIFI]-EIFIEta[wsEIFI][ac][i]) <deta[WIRE][wsEIFI])&&
                 (fabs(chPhis[WIRE][refLayerEIFI]-EIFIPhi[wsEIFI][ac][i]) <dphi[WIRE][wsEIFI])&&
                 (fabs(chEtas[STRP][refLayerEIFI]-EIFIEta[wsEIFI][ac][i]) <deta[STRP][wsEIFI])&&
                 (fabs(chPhis[STRP][refLayerEIFI]-EIFIPhi[wsEIFI][ac][i]) <dphi[STRP][wsEIFI])){
                EIFImatch[wsEIFI]=true;
                break;
              }
            }
          }

	  if(EIFImatch[WIRE] && EIFImatch[STRP]) {
            // If all necessary PRD exists and lines up, calculate wire and strip efficiency
            calculateEfficiency(ac, WIRE, eta, phi48, layer);
            calculateEfficiency(ac, STRP, eta, phi48, layer);
          }
        }
        
      }//phi
    }//eta
  }//ac

  return StatusCode::SUCCESS;
}

void
TgcRawDataValAlg::calculateEfficiency(int ac, int ws, int eta, int phi48, int layer){
  int layerws=-1, sectorlayer=-1;
  bool isEIFI = (layer>6);
  if(!isEIFI){
    //m_tgceff, m_tgceffsector not implemented for EIFI
    // Layer efficiency
    layerws=layer+ws*7;
    if(ws==1&&layer>1) layerws-=1;
    m_tgceffdenom[ac]->Fill(layerws);//fill denominator
    
    // Sector efficiency
    int sector12, phi4;
    phi48ToSectorPhi4(phi48, sector12, phi4);
    sectorlayer=sector12*7+layer;
    m_tgceffsectordenom[ac][ws]->Fill(sectorlayer);//fill denominator
  }
  
  // Chamber efficiency
  int binx, biny;
  getChamberMapIndexes(eta, phi48, layer, binx, biny);
  m_tgceffmapdenom[ac][ws]->Fill(binx, biny);//fill denominator
  
  // additional leeway for comparing channels, due to miscabling
  int dch_extra=2;
  
  // Get reference channel from adjacent layer
  int refLayer;
  if(layer==0)      refLayer = 2;
  else if(layer==1) refLayer = 2;
  else if(layer==2) refLayer = 0;
  else if(layer==3) refLayer = 4;
  else if(layer==4) refLayer = 3;
  else if(layer==5) refLayer = 6;
  else if(layer==6) refLayer = 5;
  else if(layer==7) refLayer = 8;
  else if(layer==8) refLayer = 7;
  else refLayer = 2;

  int referenceChannel = m_tgcIdHelper->channel(m_hitIdVects[CURR][ac][ws][eta][phi48][refLayer].at(0));
  referenceChannel+=m_SLBoffset[ws][ac][eta][refLayer];
  
  // Loop over Current PRD
  std::vector<Identifier>::const_iterator itc_end = m_hitIdVects[CURR][ac][ws][eta][phi48][layer].end();
  for(std::vector<Identifier>::const_iterator it=m_hitIdVects[CURR][ac][ws][eta][phi48][layer].begin();
      it!=itc_end;
      it++){
    int prdChannel = m_tgcIdHelper->channel(*it) + m_SLBoffset[ws][ac][eta][layer];
    int dmin       = m_dchmin[layer][refLayer][ws][ac] - dch_extra;
    int dmax       = m_dchmax[layer][refLayer][ws][ac] + dch_extra;
    if(compareID(prdChannel, referenceChannel, dmin, dmax)){
      ATH_MSG_DEBUG("calculate efficiency current layer"<<layer+1 << " eta" << eta << " phi" << phi48 << " fire"  );
      m_tgceffmapnum[ac][ws]->Fill(binx, biny);
      m_tgceffnum[ac]->Fill(layerws);//fill numerator
      if(!isEIFI){
        m_tgceffsectornum[ac][ws]->Fill(sectorlayer);//fill numerator
      }
      return;
    }
  }
  
  // Loop over Previous PRD
  std::vector<Identifier>::const_iterator itp_end = m_hitIdVects[PREV][ac][ws][eta][phi48][layer].end();
  for(std::vector<Identifier>::const_iterator it=m_hitIdVects[PREV][ac][ws][eta][phi48][layer].begin();
      it!=itp_end;
      it++){
    int prdChannel = m_tgcIdHelper->channel(*it) + m_SLBoffset[ws][ac][eta][layer];
    int dmin       = m_dchmin[layer][refLayer][ws][ac] - dch_extra;
    int dmax       = m_dchmax[layer][refLayer][ws][ac] + dch_extra;
    if(compareID(prdChannel, referenceChannel, dmin, dmax)){
      ATH_MSG_DEBUG("calculate efficiency previous layer"<<layer+1 << " eta" << eta << " phi" << phi48 << " fire"  );
      //if(!isEIFI){
        m_tgceffmapnumbc[ac][ws][PREV]->Fill(binx, biny);
      //}
      break;
    }
  }
  
  // Loop over Next PRD
  std::vector<Identifier>::const_iterator itn_end = m_hitIdVects[NEXT][ac][ws][eta][phi48][layer].end();
  for(std::vector<Identifier>::const_iterator it=m_hitIdVects[NEXT][ac][ws][eta][phi48][layer].begin();
      it!=itn_end;
      it++){
    int prdChannel = m_tgcIdHelper->channel(*it) + m_SLBoffset[ws][ac][eta][layer];
    int dmin       = m_dchmin[layer][refLayer][ws][ac] - dch_extra;
    int dmax       = m_dchmax[layer][refLayer][ws][ac] + dch_extra;
    if(compareID(prdChannel, referenceChannel, dmin, dmax)){
      ATH_MSG_DEBUG("calculate efficiency next layer"<<layer+1 << " eta" << eta << " phi" << phi48 << " fire"  );
      //if(!isEIFI){
        m_tgceffmapnumbc[ac][ws][NEXT-1]->Fill(binx, biny); // only prev/next defined, array index should be 1 for NEXT PRD
      //}
      break;
    }
  }
  
  ATH_MSG_DEBUG("no matching hit for efficiency calculation" );
  return;
}
