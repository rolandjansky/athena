/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  This is a test version of FTK timing simulation for the validation.
 *  We will update soon because the current code is not for general use.
 *  If you still want to run, you can get input files (and also output files) from here :
 *  /eos/atlas/unpledged/group-tokyo/users/tkaji/FTK/TimingSimulation/Validation/
 *  
 *  It will take about 3 hours with many error and warning massages due to old format of input files.
 *  If you feel annoyed, you can finish much shorter (about 10 minites) with settings below.
 *  
 *  asetup 17.8.0,here
 *  TrigFTKSim : revision 547408 (you need to copy and overwrite standalone directory from trunk version)
 *  TrigFTKPool-00-00-03
 * 
 *  source bootstrap.sh
 *  make -f Makefile.boost.mk timing_simulation
 *  ./timing_simulation
 */

#include <iostream>
#include <fstream>
using namespace std;

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKTrackStream.h"

#include "FTKTim/FTKTimeSim.cc"
#include "FTKTim/IBoardBase.hh"

int main(int argc, char **argv){
  //const string InputFile = (argc==2 ? argv[1] : "OUT.NTUP.root");
  const int nRegion=64;
  const double L1rate = 100.0;
  const int SOVERLAP = 1;
  int s_split = 0;
  int s_Chi2SecondStage = 1;
  int jordancut = 1;
  int debuglevel = 0;//kmomiyam

  // configuration for rebalancing
  static const bool RebFlag = true;
  static const double RebWeight = 0.25;

  FTKRoadStream *RoadStream[64];
  FTKTrackStream *TrackStream[64];

  TFile *tfile = new TFile("/data1/kmomiyam/ftk_explorer/RegionFile/ZHnunubb80_0.root", "READ");
  TTree *tree = (TTree *)tfile -> Get("ftkdata");

  RoadStream[0]  = new FTKRoadStream();
  TrackStream[0] = new FTKTrackStream();
  tree -> SetBranchAddress("FTKMergedTracksStream0", &TrackStream[0]);
  tree -> SetBranchAddress("FTKMergedRoadsStream0", &RoadStream[0]);

  for(int ireg=1; ireg<64; ireg++){
    tree->AddFriend(Form("ftkdata%d = ftkdata", ireg), Form("/data1/kmomiyam/ftk_explorer/RegionFile/ZHnunubb80_%d.root", ireg));

    RoadStream[ireg]  = new FTKRoadStream();
    TrackStream[ireg] = new FTKTrackStream();
    tree -> SetBranchAddress(Form("ftkdata%d.FTKMergedTracksStream%d", ireg, ireg), &TrackStream[ireg]);
    tree -> SetBranchAddress(Form("ftkdata%d.FTKMergedRoadsStream%d", ireg, ireg), &RoadStream[ireg]);
  }

  /*
    for(int iEntry=0;iEntry<5000;iEntry++){
    tree->GetEntry(iEntry);
    cout << "iEntry = "<<iEntry<<endl;
    for(int ireg=0;ireg<nRegion;ireg++){
    cout <<"ireg : "<<ireg<<"   m_event_number : "<<TrackStream[ireg]->eventNumber()<<endl;
    }
    }
  */
  double nroad_norw_AM_integ_temp[NPU][NASSOCIATIVEMEMORY]={{0}};
  double nroad_rw_AM_integ_temp[NPU][NASSOCIATIVEMEMORY]={{0}};
  double nhit_hw_integ_temp[NPU][NLAYER]={{0}};
  double nhit_nohw_integ_temp[NPU][NLAYER]={{0}};
  double nss_integ_temp[NPU][NLAYER]={{0}};
  InputValues TimInput[NPU];
  FTKTimeSim *TimSim = new FTKTimeSim();

  for(int i=0;i<NPU;i++){
    for(int j=0;j<NLAYER;j++){
      TimInput[i].nhit_nohw_integ_history[j].push_back(0);
    }// for j
    TimInput[i].TF_ew_out_history.push_back(0);
    TimInput[i].data_exist=0;
  }// for i

  double sum_nroad_rw[NPU]={0.0};
  double sum_nroad_norw[NPU]={0.0};
  double sum_nfit[NPU]={0.0};
  double sum_nfit_2nd[NPU]={0.0};
  double sum_ntrack_hw[NPU]={0.0};
  double sum_ntrack[NPU]={0.0};
  int ntrack_ok=0;
  int ntrack_tot=0;

  int nEvent = tree->GetEntries();
  for(int iev=0;iev<nEvent;iev++){
    //for(int iev=0;iev<1;iev++){
    tree->GetEntry(iev);
    int counter=0;
    double ntrack_2nd=0.0;
    double nfit_2nd=0.0;
    double temp_secfit=0.0;
    double temp_secfit_integ=0.0;

    for(int ireg=0;ireg<NREGION;ireg++){
      TimInput[ireg*2].nentry  = iev;
      TimInput[ireg*2].LVL1Gap = 1000000/L1rate;
      TimInput[ireg*2].s_overlap = SOVERLAP;
      TimInput[ireg*2].t_kill_TF = time_kill_TF_proc;
      TimInput[ireg*2].tt = 0;
      TimInput[ireg*2].split = s_split;
      TimInput[ireg*2].Chi2SecondStage = s_Chi2SecondStage;
      TimInput[ireg*2].FirstStage = 7;

      TimInput[ireg*2+1].nentry  = iev;
      TimInput[ireg*2+1].LVL1Gap = 1000000/L1rate;
      TimInput[ireg*2+1].s_overlap = SOVERLAP;
      TimInput[ireg*2+1].t_kill_TF = time_kill_TF_proc;
      TimInput[ireg*2+1].tt = 0;
      TimInput[ireg*2+1].split = s_split;
      TimInput[ireg*2+1].Chi2SecondStage = s_Chi2SecondStage;
      TimInput[ireg*2+1].FirstStage = 7;

      double sum_road = RoadStream[ireg]->naoGetNroadsAM();
      double sum_fit = TrackStream[ireg]->getNFitsI();
      double sum_fit_2nd = TrackStream[ireg]->getNFits();
      double sum_track = TrackStream[ireg]->getNTracks();

      TimInput[ireg*2].nroad_rw = sum_road/2.0;
      TimInput[ireg*2].nroad_norw = sum_road/2.0;
      TimInput[ireg*2].nfit = sum_fit/2.0;
      TimInput[ireg*2].ntrack_hw = sum_track/2.0;
      TimInput[ireg*2].ntrack = sum_track/2.0;

      TimInput[ireg*2+1].nroad_rw = sum_road/2.0;
      TimInput[ireg*2+1].nroad_norw = sum_road/2.0;
      TimInput[ireg*2+1].nfit = sum_fit/2.0;
      TimInput[ireg*2+1].ntrack_hw = sum_track/2.0;
      TimInput[ireg*2+1].ntrack = sum_track/2.0;

      sum_nroad_rw[ireg*2] += sum_road/2.0;
      sum_nroad_norw[ireg*2] += sum_road/2.0;
      sum_nfit[ireg*2] += sum_fit/2.0;
      sum_nfit_2nd[ireg*2] += sum_fit_2nd/2.0;
      sum_ntrack_hw[ireg*2] += sum_track/2.0;
      sum_ntrack[ireg*2] += sum_track/2.0;

      sum_nroad_rw[ireg*2+1] += sum_road/2.0;
      sum_nroad_norw[ireg*2+1] += sum_road/2.0;
      sum_nfit[ireg*2+1] += sum_fit/2.0;
      sum_nfit_2nd[ireg*2+1] += sum_fit_2nd/2.0;
      sum_ntrack_hw[ireg*2+1] += sum_track/2.0;
      sum_ntrack[ireg*2+1] += sum_track/2.0;

      TimInput[ireg*2].nroad_rw_integ = sum_nroad_rw[ireg*2];
      TimInput[ireg*2].nroad_norw_integ = sum_nroad_norw[ireg*2];
      TimInput[ireg*2].nfit_integ = sum_nfit[ireg*2];
      TimInput[ireg*2].ntrack_integ = sum_ntrack[ireg*2];
      TimInput[ireg*2].ntrack_hw_integ = sum_ntrack_hw[ireg*2];

      TimInput[ireg*2+1].nroad_rw_integ = sum_nroad_rw[ireg*2+1];
      TimInput[ireg*2+1].nroad_norw_integ = sum_nroad_norw[ireg*2+1];
      TimInput[ireg*2+1].nfit_integ = sum_nfit[ireg*2+1];
      TimInput[ireg*2+1].ntrack_integ = sum_ntrack[ireg*2+1];
      TimInput[ireg*2+1].ntrack_hw_integ = sum_ntrack_hw[ireg*2+1];

      counter++;
      ntrack_2nd += sum_track/2.0;
      nfit_2nd += sum_fit_2nd/2.0;

      TimInput[ireg*2].SecStage_nfit = temp_secfit;
      TimInput[ireg*2].SecStage_nfit_integ = temp_secfit_integ;
      TimInput[ireg*2+1].SecStage_nfit = temp_secfit;
      TimInput[ireg*2+1].SecStage_nfit_integ = temp_secfit_integ;

      // nmergeSec = 4 
      if(counter%nmergeSec==0){
	for(int bb=0;bb<nmergeSec;bb++){
	  TimInput[ireg*2-bb].SecStage_nfit = nfit_2nd;
	  sum_nfit_2nd[ireg*2-bb] += nfit_2nd;
	  TimInput[ireg*2-bb].SecStage_nfit_integ = sum_nfit_2nd[ireg*2-bb];
	}// for bb
	temp_secfit = nfit_2nd;
	temp_secfit_integ = sum_nfit_2nd[ireg*2];
	temp_secfit_integ = sum_nfit_2nd[ireg*2+1]; // what mean?
	nfit_2nd = 0.0;
	ntrack_2nd = 0.0;
      }// if

      double ncluster_tot=0.0;
      double nss_tot=0.0;
      for(int ilayer=0;ilayer<8;ilayer++){
	double ncluster = RoadStream[ireg]->naoGetNclus(ilayer);
	double nss = RoadStream[ireg]->naoGetNss(ilayer);
	TimInput[ireg*2].nhit_nohw[ilayer] = ncluster;
	TimInput[ireg*2].nss[ilayer] = nss;
	TimInput[ireg*2+1].nhit_nohw[ilayer] = ncluster;
	TimInput[ireg*2+1].nss[ilayer] = nss;
	ncluster_tot += ncluster;
	nss_tot += nss;

	nhit_nohw_integ_temp[ireg*2][ilayer] += ncluster;
	nss_integ_temp[ireg*2][ilayer] += nss;

	nhit_nohw_integ_temp[ireg*2+1][ilayer] += ncluster;
	nss_integ_temp[ireg*2+1][ilayer] += nss;

	TimInput[ireg*2].nhit_nohw_integ[ilayer] = nhit_nohw_integ_temp[ireg*2][ilayer];
	TimInput[ireg*2].nss_integ[ilayer] = nss_integ_temp[ireg*2][ilayer];
	TimInput[ireg*2+1].nhit_nohw_integ[ilayer] = nhit_nohw_integ_temp[ireg*2+1][ilayer];
	TimInput[ireg*2+1].nss_integ[ilayer] = nss_integ_temp[ireg*2+1][ilayer];
      }// for ilayer

      if(ncluster_tot!=0) TimInput[ireg*2].data_exist=1; else TimInput[ireg*2].data_exist=0;
      if(ncluster_tot!=0) TimInput[ireg*2+1].data_exist=1; else TimInput[ireg*2+1].data_exist=0;

      TimInput[ireg*2].nss_tot = nss_tot;
      TimInput[ireg*2].nhit_nohw_tot = ncluster_tot;
      TimInput[ireg*2+1].nss_tot = nss_tot;
      TimInput[ireg*2+1].nhit_nohw_tot = ncluster_tot;

      int AMroads[2*NASSOCIATIVEMEMORY]={0};
      double TFfits[2*NTRACKFITTER]={0};
      for(int iroad=0;iroad<sum_road;iroad++){
	RoadStream[ireg]->attachHits(iroad);
	FTKRoad *curroad = RoadStream[ireg]->getRoad(iroad);
	AMroads[(int)4*(curroad->getSectorID()%8)+curroad->getPatternID()%4]++;

	double nhits = 1;
	//for(int ilayer=1;ilayer<8;ilayer++){
	for(int ilayer=0;ilayer<8;ilayer++){
	  if(curroad->getNHits(ilayer)!=0)
	    if(jordancut && curroad->getNHits(ilayer)>8){
	      nhits *= 8;
	    }else{
	      nhits *= curroad->getNHits(ilayer);
	    }
	}// for ilayer
	//TFfits[curroad->getSectorID()%8] += (nhits<1000000 ? nhits : 1000000);
	TFfits[curroad->getSectorID()%8] += ((jordancut && nhits>=1024) ? 1024 : nhits);
      }// for iroad

      for(int imem=0;imem<NASSOCIATIVEMEMORY;imem++){
	nroad_norw_AM_integ_temp[ireg*2][imem] += AMroads[imem];
	nroad_rw_AM_integ_temp[ireg*2][imem]   += AMroads[imem];
	nroad_norw_AM_integ_temp[ireg*2+1][imem] += AMroads[imem+NASSOCIATIVEMEMORY];
	nroad_rw_AM_integ_temp[ireg*2+1][imem]   += AMroads[imem+NASSOCIATIVEMEMORY];

	TimInput[ireg*2].nroad_norw_AM[imem] = AMroads[imem];
	TimInput[ireg*2+1].nroad_norw_AM[imem] = AMroads[imem+NASSOCIATIVEMEMORY];
	TimInput[ireg*2].nroad_rw_AM[imem] = AMroads[imem];
	TimInput[ireg*2+1].nroad_rw_AM[imem] = AMroads[imem+NASSOCIATIVEMEMORY];
	TimInput[ireg*2].nroad_norw_AM_integ[imem] = (double)nroad_norw_AM_integ_temp[ireg*2][imem];
	TimInput[ireg*2+1].nroad_norw_AM_integ[imem] = (double)nroad_norw_AM_integ_temp[ireg*2+1][imem];
	TimInput[ireg*2].nroad_rw_AM_integ[imem] = (double)nroad_rw_AM_integ_temp[ireg*2][imem];
	TimInput[ireg*2+1].nroad_rw_AM_integ[imem] = (double)nroad_rw_AM_integ_temp[ireg*2+1][imem];
      }// for imem

      for(int ifit=0;ifit<NTRACKFITTER;ifit++){
	TimInput[ireg*2].nfit_TF[ifit] = TFfits[ifit];
	TimInput[ireg*2+1].nfit_TF[ifit] = TFfits[ifit+NTRACKFITTER];	
      }// for ifit
    }// for ireg

    //debug kmomiyam
    if(debuglevel){
      ofstream ofs("forDebug.txt");
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nroad_rw_before[%d] = ", ireg*2) << TimInput[ireg*2].nroad_rw <<endl;
	ofs << Form("nroad_rw_before[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nroad_rw <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nfit[%d] = ", ireg*2) << TimInput[ireg*2].nfit <<endl;
	ofs << Form("nfit[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nfit <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_hw[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_hw <<endl;
	ofs << Form("ntrack_hw[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_hw <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nroad_rw_integ[%d] = ", ireg*2) << TimInput[ireg*2].nroad_rw_integ << endl;
	ofs << Form("nroad_rw_integ[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nroad_rw_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nfit_integ[%d] = ", ireg*2) << TimInput[ireg*2].nfit_integ << endl;
	ofs << Form("nfit_integ[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nfit_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_integ[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_integ << endl;
	ofs << Form("ntrack_integ[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_hw_integ[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_hw_integ << endl;
	ofs << Form("ntrack_hw_integ[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_hw_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("SecStage_nfit[%d] = ", ireg*2) << TimInput[ireg*2].SecStage_nfit << endl;
	ofs << Form("SecStage_nfit[%d] = ", ireg*2+1) << TimInput[ireg*2+1].SecStage_nfit << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("SecStage_nfit_integ[%d] = ", ireg*2) << TimInput[ireg*2].SecStage_nfit_integ << endl;
	ofs << Form("SecStage_nfit_integ[%d] = ", ireg*2+1) << TimInput[ireg*2+1].SecStage_nfit_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nhit_nohw[%d][%d] = ", ireg, ilayer) << TimInput[ireg].nhit_nohw[ilayer] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nhit_nohw_integ[%d][%d] = ", ireg, ilayer) << TimInput[ireg].nhit_nohw_integ[ilayer] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nhit_nohw_tot[%d] = ", ireg) << TimInput[ireg].nhit_nohw_tot << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nss[%d][%d] = ", ireg, ilayer) << TimInput[ireg].nss[ilayer] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nss_integ[%d][%d] = ", ireg, ilayer) << TimInput[ireg].nss_integ[ilayer] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ilayer=0; ilayer<8; ilayer++){
	  ofs << Form("nss_tot[%d] = ", ireg) << TimInput[ireg].nss_tot << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM[%d][%d] = ", ireg*2, imem) << TimInput[ireg*2].nroad_norw_AM[imem] << endl;
	}
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM[%d][%d] = ", ireg*2+1, imem) << TimInput[ireg*2+1].nroad_norw_AM[imem] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_integ[%d][%d] = ", ireg*2, imem) << TimInput[ireg*2].nroad_norw_AM_integ[imem] << endl;
	}
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_integ[%d][%d] = ", ireg*2+1, imem) << TimInput[ireg*2+1].nroad_norw_AM_integ[imem] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ifit=0; ifit<NTRACKFITTER; ifit++){
	  ofs << Form("nfit_TF[%d][%d] = ", ireg*2, ifit) << TimInput[ireg*2].nfit_TF[ifit] << endl;
	}
	for(int ifit=0; ifit<NTRACKFITTER; ifit++){
	  ofs << Form("nfit_TF[%d][%d] = ", ireg*2+1, ifit) << TimInput[ireg*2+1].nfit_TF[ifit] << endl;
	}
      }
      ofs.close();
    }

    // Rebalancing
    if(RebFlag){
      int offset = 0;
      for(int i=0;i<64;i++){
	if(i==32)
	  offset = 64;
	TimInput[i+32    ].nroad_rw           +=    RebWeight*TimInput[i+offset].nroad_rw;
	TimInput[i+offset].nroad_rw           -=    RebWeight*TimInput[i+offset].nroad_rw;
	TimInput[i+32    ].nroad_norw         +=    RebWeight*TimInput[i+offset].nroad_norw;
	TimInput[i+offset].nroad_norw         -=    RebWeight*TimInput[i+offset].nroad_norw;
	TimInput[i+32    ].nfit               +=    RebWeight*TimInput[i+offset].nfit;
	TimInput[i+offset].nfit               -=    RebWeight*TimInput[i+offset].nfit;
	TimInput[i+32    ].ntrack_hw          +=    RebWeight*TimInput[i+offset].ntrack_hw;
	TimInput[i+offset].ntrack_hw          -=    RebWeight*TimInput[i+offset].ntrack_hw;
	TimInput[i+32    ].nroad_rw_integ     +=    RebWeight*TimInput[i+offset].nroad_rw_integ;
	TimInput[i+offset].nroad_rw_integ     -=    RebWeight*TimInput[i+offset].nroad_rw_integ;
	TimInput[i+32    ].nroad_norw_integ   +=    RebWeight*TimInput[i+offset].nroad_norw_integ;
	TimInput[i+offset].nroad_norw_integ   -=    RebWeight*TimInput[i+offset].nroad_norw_integ;
	TimInput[i+32    ].nfit_integ         +=    RebWeight*TimInput[i+offset].nfit_integ;
	TimInput[i+offset].nfit_integ         -=    RebWeight*TimInput[i+offset].nfit_integ;
	TimInput[i+32    ].ntrack_integ       +=    RebWeight*TimInput[i+offset].ntrack_integ;
	TimInput[i+offset].ntrack_integ       -=    RebWeight*TimInput[i+offset].ntrack_integ;
	TimInput[i+32    ].ntrack_hw_integ    +=    RebWeight*TimInput[i+offset].ntrack_hw_integ;
	TimInput[i+offset].ntrack_hw_integ    -=    RebWeight*TimInput[i+offset].ntrack_hw_integ;

	for(int imem=0;imem<NASSOCIATIVEMEMORY;imem++){
	  TimInput[i+32    ].nroad_norw_AM[imem]   +=   RebWeight*TimInput[i+offset].nroad_norw_AM[imem];
	  TimInput[i+offset].nroad_norw_AM[imem]   -=   RebWeight*TimInput[i+offset].nroad_norw_AM[imem];
	  TimInput[i+32    ].nroad_rw_AM[imem]     +=   RebWeight*TimInput[i+offset].nroad_rw_AM[imem];
	  TimInput[i+offset].nroad_rw_AM[imem]     -=   RebWeight*TimInput[i+offset].nroad_rw_AM[imem];
	}// for imem
	for(int ifit=0;ifit<NTRACKFITTER;ifit++){
	  TimInput[i+32    ].nfit_TF[ifit]   +=   RebWeight*TimInput[i+offset].nfit_TF[ifit];
	  TimInput[i+offset].nfit_TF[ifit]   -=   RebWeight*TimInput[i+offset].nfit_TF[ifit];
	}// for ifit
      }
    }// RebFlag

    if(debuglevel){
      ofstream ofs("forDebug.txt", ios::app);
      ofs << "~~~~~~~~~~~~~~~~Rebalanced~~~~~~~~~~~~~~" << endl;
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nroad_rw_after[%d] = ", ireg*2) << TimInput[ireg*2].nroad_rw <<endl;
	ofs << Form("nroad_rw_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nroad_rw <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nfit_after[%d] = ", ireg*2) << TimInput[ireg*2].nfit <<endl;
	ofs << Form("nfit_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nfit <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_hw_after[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_hw <<endl;
	ofs << Form("ntrack_hw_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_hw <<endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nroad_rw_integ_after[%d] = ", ireg*2) << TimInput[ireg*2].nroad_rw_integ << endl;
	ofs << Form("nroad_rw_integ_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nroad_rw_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("nfit_integ_after[%d] = ", ireg*2) << TimInput[ireg*2].nfit_integ << endl;
	ofs << Form("nfit_integ_aftr[%d] = ", ireg*2+1) << TimInput[ireg*2+1].nfit_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_integ_after[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_integ << endl;
	ofs << Form("ntrack_integ_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	ofs << Form("ntrack_hw_integ_after[%d] = ", ireg*2) << TimInput[ireg*2].ntrack_hw_integ << endl;
	ofs << Form("ntrack_hw_integ_after[%d] = ", ireg*2+1) << TimInput[ireg*2+1].ntrack_hw_integ << endl;
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_after[%d][%d] = ", ireg*2, imem) << TimInput[ireg*2].nroad_norw_AM[imem] << endl;
	}
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_after[%d][%d] = ", ireg*2+1, imem) << TimInput[ireg*2+1].nroad_norw_AM[imem] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_integ_after[%d][%d] = ", ireg*2, imem) << TimInput[ireg*2].nroad_norw_AM_integ[imem] << endl;
	}
	for(int imem=0; imem<NASSOCIATIVEMEMORY; imem++){
	  ofs << Form("nroad_norw_AM_integ_after[%d][%d] = ", ireg*2+1, imem) << TimInput[ireg*2+1].nroad_norw_AM_integ[imem] << endl;
	}
      }
      for(int ireg=0; ireg<NREGION; ireg++){
	for(int ifit=0; ifit<NTRACKFITTER; ifit++){
	  ofs << Form("nfit_TF_after[%d][%d] = ", ireg*2, ifit) << TimInput[ireg*2].nfit_TF[ifit] << endl;
	}
	for(int ifit=0; ifit<NTRACKFITTER; ifit++){
	  ofs << Form("nfit_TF_after[%d][%d] = ", ireg*2+1, ifit) << TimInput[ireg*2+1].nfit_TF[ifit] << endl;
	}
      }
    }

    for(int iPU=0;iPU<NPU;iPU++){
      TimSim->SetInputValueForRegion(iPU, TimInput[iPU]);
    }// for iPU
    TimSim->FillInputValue();
    TimSim->Calc();


    for(int iPU=0;iPU<NPU;iPU++){
      if(TimSim->GetNkill(iPU)){
      }else{
	ntrack_ok += TimInput[iPU].ntrack;
      }
      ntrack_tot += TimInput[iPU].ntrack;
      for(int ilayer=0;ilayer<NLAYER;ilayer++){
	TimInput[iPU].nhit_nohw_integ_history[ilayer].push_back(TimInput[iPU].nhit_nohw_integ[ilayer]);
      }// for ilayer
      TimInput[iPU].TF_ew_out_history.push_back(TimSim->GetTF_EwOut(iPU));
    }// for iPU    
    TimSim->FillResults(1, iev, "FTKTimOutput", 1000, s_split);

  }// for iev

  TimSim->PlotInputValues("FTKTimOutput", s_split, 2);
  TimSim->PlotResults("FTKTimOutput", 1000, s_split, 2);
  TimSim->PrintResults(-999, 0, 1, 1, "FTKTimOutput", ntrack_ok, ntrack_tot, s_split);


  return 0;
}// main
