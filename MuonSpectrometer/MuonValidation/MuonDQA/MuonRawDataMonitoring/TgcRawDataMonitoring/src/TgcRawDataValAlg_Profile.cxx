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
TgcRawDataValAlg::bookHistogramsProfile(){
  //Following histograms are booked
  //
  // Doublet_Triplet_Hits_A
  // Doublet_Triplet_Hits_C

  // Profile_Map_A
  // Profile_Map_C

  // Wire_Strip_Coincidence_Per_GasGap_A
  // Wire_Strip_Coincidence_Per_GasGap_C

  // Wire_Hit_Profile_[AC][01-12]_Layer[1-7]_phi[0-3]
  // Wire_Hit_Profile_[AC][01-12]_Layer[1-7]_phi[0-3]

  // Summary_Of_Wire_Occupancy_Per_GasGap_A
  // Summary_Of_Wire_Occupancy_Per_GasGap_C
  // Summary_Of_Strip_Occupancy_Per_GasGap_A
  // Summary_Of_Strip_Occupancy_Per_GasGap_C

  // Summary_Of_Wire_Occupancy_Per_Chamber_Type_Station[1-3]_{E,F[0-5]}_T[1-9]
  // Summary_Of_Strip_Occupancy_Per_Chamber_Type_Station[1-3]_{E,F[0-5]}_T[1-9]
  
  ATH_MSG_INFO( "bookHistogramsProfile("  );
  
  MonGroup tgcprd_shift( this, m_generic_path_tgcmonitoring + "/Global", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_shift_a( this, m_generic_path_tgcmonitoring + "/TGCEA", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_shift_c( this, m_generic_path_tgcmonitoring + "/TGCEC", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_shift_ac[2] = { &tgcprd_shift_a, &tgcprd_shift_c };
  
  MonGroup tgcprd_profile_s_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Profile", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_profile_s_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Profile", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_profile_s_ac[2] = {&tgcprd_profile_s_a, &tgcprd_profile_s_c};
  
  MonGroup tgcprd_profile_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Profile", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_profile_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Profile", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_profile_ac[2] = {&tgcprd_profile_a, &tgcprd_profile_c};
  
  MonGroup tgcprd_occupancy_s_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Occupancy", run, ATTRIB_UNMANAGED  ); 
  MonGroup tgcprd_occupancy_s_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Occupancy", run, ATTRIB_UNMANAGED  ); 
  MonGroup* tgcprd_occupancy_s_ac[2] = {&tgcprd_occupancy_s_a, &tgcprd_occupancy_s_c};
  
  MonGroup tgcprd_occupancy_a( this, m_generic_path_tgcmonitoring + "/TGCEA/Occupancy", run, ATTRIB_UNMANAGED  ); 
  MonGroup tgcprd_occupancy_c( this, m_generic_path_tgcmonitoring + "/TGCEC/Occupancy", run, ATTRIB_UNMANAGED  ); 
  //MonGroup* tgcprd_occupancy_ac[2] = {&tgcprd_occupancy_a, &tgcprd_occupancy_c};
  
  std::stringstream ss;
  std::string side[2] = {"A","C"};
  std::string sws[2]  = {"Wire_","Strip_"};
  
  for(int ac=0;ac<2;ac++){
    //number of hits for each SSW in 2LBs
    ss.str(""); ss << "Doublet_Triplet_Hits_"<<side[ac];
    m_tgctripletdoublethitsinlbvssect[ac]=new TH2F(ss.str().c_str(), (ss.str() + "; LB;" ).c_str(),1250,1.,2501., 148, 0, 148 );       
    ATH_CHECK( tgcprd_shift_ac[ac]->regHist(m_tgctripletdoublethitsinlbvssect[ac]) );
    
    int n=1;
    
    string tdname[12] = {"ETphi0","ETphi1","ETphi2","ETphi3","FTphi0","FTphi1",
                         "EDphi0","EDphi1","EDphi2","EDphi3","FDphi0","FDphi1"};
    for(int sec=1;sec<=12;sec++){
      for(int id=0;id<12;id++){
        ss.str(""); ss << side[ac];
        if(sec < 10 ) ss<<"0";
        ss << sec << tdname[id];
        
        if(id==0 || id==6 )
          m_tgctripletdoublethitsinlbvssect[ac]->GetYaxis()->SetBinLabel(n,ss.str().c_str());
        n++;
      }
    }
    m_tgctripletdoublethitsinlbvssect[ac]->GetYaxis()->SetBinLabel(n++, (side[ac] + "02EIFI").c_str() );
    //m_tgctripletdoublethitsinlbvssect[ac]->GetYaxis()->SetBinLabel(n++, (side[ac] + "05EIFI").c_str() );
    //m_tgctripletdoublethitsinlbvssect[ac]->GetYaxis()->SetBinLabel(n++, (side[ac] + "08EIFI").c_str() );
    //m_tgctripletdoublethitsinlbvssect[ac]->GetYaxis()->SetBinLabel(n++, (side[ac] + "11EIFI").c_str() );
    
    for(int ws=0;ws<2;ws++){
      //Profile map
      ss.str(""); ss << sws[ws] << "Profile_Map_" << side[ac];
      m_tgcprofilemap[ac][ws] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
      ATH_CHECK( tgcprd_profile_s_ac[ac]->regHist(m_tgcprofilemap[ac][ws]) );
      m_tgcprofilemap[ac][ws]->SetMaximum(1.2);
      m_tgcprofilemap[ac][ws]->SetMinimum(1e-8);
      
      //Occupancy map
      ss.str(""); ss << sws[ws] << "Occupancy_Map_" << side[ac];
      m_tgcoccupancymap[ac][ws] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
      ATH_CHECK( tgcprd_occupancy_s_ac[ac]->regHist(m_tgcoccupancymap[ac][ws]) );
      m_tgcoccupancymap[ac][ws]->SetMaximum(1.2);
      m_tgcoccupancymap[ac][ws]->SetMinimum(1e-8);
    }//ws
    
    //Wire Strip Coincidence per GasGap
    ss.str(""); ss << "Wire_Strip_Coincidence_Per_GasGap_" << side[ac];
    m_tgcwirestripcoin[ac] = new TH2F( ss.str().c_str(), ss.str().c_str(),43, 0, 43, 48, 1, 49 );
    ATH_CHECK( tgcprd_shift_ac[ac]->regHist(m_tgcwirestripcoin[ac]) );
  }//ac
  
  std::string schamberT1[6]={"E1", "E2", "E3", "E4", "F"};
  std::string schamberT3[6]={"E1", "E2", "E3", "E4", "E5", "F"};
  std::string schamberEF[2]={"EI", "FI"};
  
  for(int ac=0;ac<2;ac++){
    // Blank out chambers that don't exist
    BlankStationMap(m_tgcwirestripcoin[ac]); 
    for(int ws=0;ws<2;ws++){
      // Blank out chambers that don't exist
      BlankStationMap(m_tgcprofilemap[ac][ws]);
      if(ws==1) BlankStripStationMap(m_tgcprofilemap[ac][ws]);
      // Blank out chambers that don't exist
      BlankStationMap(m_tgcoccupancymap[ac][ws]);
      if(ws==1) BlankStripStationMap(m_tgcoccupancymap[ac][ws]);
      
      for(int sec=1;sec<=12;sec++){
        for(int phi=0;phi<=3;phi+=4){
          ss.str(""); ss << side[ac];
          if(sec<10)ss << "0";
          ss << sec << "phi" << phi;
          int k=(sec-1)*4+phi+1;
          m_tgcprofilemap[ac][ws] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
          m_tgcoccupancymap[ac][ws] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
        }
      }
      int x=1;
      // old binning strategy
      ////layer1-3
      //for(int l=0;l<3;l++){
      //  for(int c=0;c<5;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberT1[c];
      //    m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}
      ////layer4-7
      //for(int l=3;l<7;l++){
      //  for(int c=0;c<6;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberT3[c];
      //    m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}
      ////layer8-9
      //for(int l=7;l<9;l++){
      //  for(int c=0;c<2;c++){
      //    ss.str(""); ss << "L" << l+1 << "_" << schamberEF[c];
      //    m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
      //    m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      //  }
      //}
      // chamber E1-E4,  layer1-7
      for(int c=0;c<4;c++){
	for(int l=1;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT1[c];
          m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }
      // chamber E5,  layer4-7
      for(int c=4;c<5;c++){
	for(int l=4;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT3[c];
          m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }
      // chamber F,  layer1-7
      for(int c=5;c<6;c++){
	for(int l=1;l<=7;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberT3[c];
          m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }
      // chamber EIFI,  layer8-9
      for(int c=0;c<2;c++){
	for(int l=8;l<=9;l++){
	  ss.str(""); ss << "L" << l << "_" << schamberEF[c];
          m_tgcprofilemap[ac][ws] ->GetXaxis()->SetBinLabel( x, ss.str().c_str() );
          m_tgcoccupancymap[ac][ws] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
	}
      }


      m_tgcprofilemap[ac][ws] ->GetXaxis()->LabelsOption("v");
      m_tgcoccupancymap[ac][ws] ->GetXaxis()->LabelsOption("v");
    }//ws
  }//ac
  
  for(int ac=0;ac<2;ac++){
    for(int sec=1;sec<=12;sec++){
      for(int phi=0;phi<=3;phi+=4){
        ss.str(""); ss << side[ac];
        if(sec<10)ss << "0";
        ss << sec << "phi" << phi;
        int k=(sec-1)*4+phi+1;
        m_tgcwirestripcoin[ac] ->GetYaxis()->SetBinLabel( k, ss.str().c_str() );
      }
    }
    int x=1;
    ////layer1-3
    //for(int l=0;l<3;l++){
    //  for(int c=0;c<5;c++){
    //    ss.str(""); ss << "L" << l+1 << "_" << schamberT1[c];
    //    m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
    //  }
    //}
    ////layer4-7
    //for(int l=3;l<7;l++){
    //  for(int c=0;c<6;c++){
    //    ss.str(""); ss << "L" << l+1 << "_" << schamberT3[c];
    //    m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
    //  }
    //}
    ////layer8-9
    //for(int l=7;l<9;l++){
    //  for(int c=0;c<2;c++){
    //    ss.str(""); ss << "L" << l+1 << "_" << schamberEF[c];
    //    m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
    //  }
    //}
    // chamber E1-E4,  layer1-7
    for(int c=0;c<4;c++){
      for(int l=1;l<=7;l++){
        ss.str(""); ss << "L" << l << "_" << schamberT1[c];
        m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    // chamber E5,  layer4-7
    for(int c=4;c<5;c++){
      for(int l=4;l<=7;l++){
        ss.str(""); ss << "L" << l << "_" << schamberT3[c];
        m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    // chamber F,  layer1-7
    for(int c=5;c<6;c++){
      for(int l=1;l<=7;l++){
        ss.str(""); ss << "L" << l << "_" << schamberT3[c];
        m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }
    // chamber EIFI,  layer8-9
    for(int c=0;c<2;c++){
      for(int l=8;l<=9;l++){
        ss.str(""); ss << "L" << l << "_" << schamberEF[c];
        m_tgcwirestripcoin[ac] ->GetXaxis()->SetBinLabel( x++, ss.str().c_str() );
      }
    }

    m_tgcwirestripcoin[ac] ->GetXaxis()->LabelsOption("v");
  }
  
  // not monitor these profiles at GM
  // not monitor also at Tier0
  if( m_mon_profile == true ){
    if( m_environment != AthenaMonManager::online ) {
      //Wire and Strip Hit Profile for each layer
      for(int ac=0;ac<2;ac++){
        std::string wireprof="Wire_Hit_Profile_";
        std::string stripprof="Strip_Hit_Profile_";
        std::string wireoccu="Wire_Occupancy_";
        std::string stripoccu="Strip_Occupancy_";
        std::string slayer="_Layer";
        std::string ssector="_Sector";
        std::string sphi="_Phi";
        
        for(int i=0;i<7;i++){
          int layer=i+1;
          if(i==0)layer=1;
          int tgc=0;
          if(layer>3)tgc=1;
          else if(layer>5)tgc=2;
          
          for(int phi48=0;phi48<48;phi48++){
            int sector, phi4;
            phi48ToSectorPhi4(phi48, sector, phi4);
            
            // Wire Hit Profile phi48
            ss.str(""); ss<<wireprof<<side[ac]<<std::setw(2)<<std::setfill('0')<<sector<<slayer<<layer<<sphi<<phi4;
            m_tgcwireprofilephi48[ac][i][phi48] = new TH1F(ss.str().c_str(), (ss.str() + ";Wire #; Events").c_str(),m_nWireTGCSingleLayerInPhi48Division[tgc] , 0, m_nWireTGCSingleLayerInPhi48Division[tgc]);
            ATH_CHECK( tgcprd_profile_ac[ac]->regHist(m_tgcwireprofilephi48[ac][i][phi48]) );
            
            if(i!=1){
              // Strip Hit Profile phi48
              ss.str(""); ss<<stripprof<<side[ac]<<std::setw(2)<<std::setfill('0')<<sector<<slayer<<layer<<sphi<<phi4;
              m_tgcstripprofilephi48[ac][i][phi48] = new TH1F(ss.str().c_str(), (ss.str() + ";Strip #; Events" ).c_str(),m_nStripTGCSingleLayerInPhi48Division[tgc] , 0, m_nStripTGCSingleLayerInPhi48Division[tgc]);
              ATH_CHECK( tgcprd_profile_ac[ac]->regHist(m_tgcstripprofilephi48[ac][i][phi48]) );
            }
            else{
              m_tgcstripprofilephi48[ac][i][phi48] = 0;
            }
          }
        }
        
      }//ac
    }//GM
    else
      for(int ac=0;ac<2;ac++)
        for(int i=0;i<7;i++)
          for(int phi48=0;phi48<48;phi48++){
            m_tgcwireprofilephi48[ac][i][phi48] = 0;
            m_tgcstripprofilephi48[ac][i][phi48] = 0;
          }
    
  }//mon_profile
  
  return StatusCode::SUCCESS;
}


StatusCode 
TgcRawDataValAlg::fillProfile(){
  
  // Fill Profile Histograms from Hit Vectors
  for(int ac=0;ac<2;ac++){
    for(int ws=0;ws<2;ws++){
      // Loop over hits
      int nHits = m_hitLayer[ac][ws].size();
      for(int iHit=0;iHit<nHits;iHit++){
        int stationName = m_hitStation[ac][ws].at(iHit);
        int layer       = m_hitLayer[ac][ws].at(iHit);
        int channel     = m_hitChannel[ac][ws].at(iHit);
        int sector      = m_hitSector[ac][ws].at(iHit);
        int sectorPhi   = m_hitSectorPhi[ac][ws].at(iHit);
        int etaIndex    = m_hitEtaIndex[ac][ws].at(iHit);
        int phi48       = m_hitPhi48[ac][ws].at(iHit);
        
        // Get TGC index
        int tgcIndex = StationName2TgcIndex(stationName);
        
        // Fill Triplet-Doublet Hits per lumiblock
        m_tgctripletdoublethitsinlbvssect[ac]->Fill(m_lumiblock, getTripletDoubletID(stationName, sector, sectorPhi));
        
        // Fill Online Profile Histograms
        if(m_mon_profile==true){
          if(m_environment!=AthenaMonManager::online){
            // Get TGC channel number 
            if(tgcIndex!=3){// EIFI not implemented
              int numberphi48 = getWireNumberPerPhi48(tgcIndex, ws, channel, etaIndex);
              if(ws==WIRE){//wire
                m_tgcwireprofilephi48[ac][layer][phi48]->Fill(numberphi48);
              }else{//strip
                m_tgcstripprofilephi48[ac][layer][phi48]->Fill(numberphi48);
              }
            }
          }// !online
        }// doProfile
        
      }//hit
    }//ws
  }//ac
  
  
  //Wire Strip Coincidence
  for(int ac=0;ac<2;ac++){
    for(int eta=0;eta<6;eta++){
      for(int phi48=0;phi48<48;phi48++){
        for(int layer=0;layer<9;layer++){
          int nhitw = m_hitIdVects[1][ac][0][eta][phi48][layer].size();
          int nhits = m_hitIdVects[1][ac][1][eta][phi48][layer].size();
          
          int binx, biny;
          getChamberMapIndexes(eta, phi48, layer, binx, biny);
          
          if(nhitw>0) m_tgcprofilemap[ac][0]->Fill(binx, biny, nhitw);
          if(nhits>0) m_tgcprofilemap[ac][1]->Fill(binx, biny, nhits);
          
          if(layer==1) nhits=1;
          if(nhitw!=0&&nhits!=0){
            m_tgcwirestripcoin[ac]->Fill(binx, biny, nhitw*nhits);
            // not monitor these profiles at GM
            if(m_environment!=AthenaMonManager::online)
              m_tgcwirestripcoinlowstat[ac]->Fill(binx, biny, nhitw*nhits);
          }
          
        }//layer
      }//phi48
    }//eta
  }//ac
  
  return StatusCode::SUCCESS;
}
