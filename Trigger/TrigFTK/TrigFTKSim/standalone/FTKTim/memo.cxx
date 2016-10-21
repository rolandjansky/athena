/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cftkutils.h"

#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;

#include <TChain.h>
#include <TFile.h>
#include <TH2F.h>

// HH for timing simulation  HH//
#include "FTKTim/FTKTimeSim.cc"
#include "FTKTim/IBoardBase.hh"

/* Author: G. Volpi <guido.volpi@cern.ch> */
// HHH Mod by Naoki Kimura 2013/2/28 HHH //
// HHH <naoki.kimura@cern.ch> HHH //
// HHH change log memo HHH
// H 2014 Nov13 : start to imprement # of road of each LAMB and buffer size for the TF.
// H 2015 Feb20 : start to imprement 128 Pu and ind32 and buffer etc
/** 

Compile using:
g++ cftkutils.cxx -I$AtlasBaseDir/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/include/python2.6 -I../TrigFTKSim -m32 -shared -o cftkutils.so -L$AtlasBaseDir/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/lib $(root-config --cflags --libs) -L../standalone -lftk_classes -g

Set the environment with:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/../standalone

*/



/* This function can be called after the numbers content is filled. The goal
   is to use some of the numbers values to evaluate the execution time at the
   different stages.
   The method is designed to work also with partial output, in this case the 
   total execution time will be relative only to the emulated parts.
*/
PyObject *FTKTreeObject_ExecutionTime(FTKTreeObject* self, PyObject *args)
{
  int temp_sub=1;
  if(self->nsubregions) temp_sub=self->nsubregions;
  /* Prepare the reference for the lists of numbers*/
  PyObject **list_nclusters = new PyObject*[temp_sub*self->nregions*self->nlayers];
  PyObject **list_nss = new PyObject*[temp_sub*self->nregions*self->nlayers];
  PyObject **list_nroads = new PyObject*[temp_sub*self->nregions];
  PyObject **list_nfits = new PyObject*[temp_sub*self->nregions];
  PyObject **list_nfits_2nd = new PyObject*[temp_sub*self->nregions];
  PyObject **list_ntracks = new PyObject*[temp_sub*self->nregions];

  PyObject **key_roads_nckey = new PyObject*[self->nregions*temp_sub*self->nlayers];
  PyObject **key_roads_nsskey = new PyObject*[self->nregions*temp_sub*self->nlayers];
  PyObject **key_roads_narkey = new PyObject*[self->nregions*temp_sub];
  PyObject **key_tracks_nafkey = new PyObject*[self->nregions*temp_sub];
  PyObject **key_tracks_nafIkey = new PyObject*[self->nregions*temp_sub];
  PyObject **key_tracks_natkey = new PyObject*[self->nregions*temp_sub];

  PyObject **list_32roads_SecPatDCkey = new PyObject*[self->nregions*32]; //ind32
  PyObject **list_32roads_SecPatACkey = new PyObject*[self->nregions*32]; //i
  PyObject **list_8fits_SecPatDCkey = new PyObject*[self->nregions*8]; //ind8
  PyObject **list_8fits_SecPatACkey = new PyObject*[self->nregions*8]; //ind8

  PyObject **key_32roads_SecPatDCkey = new PyObject*[self->nregions*32]; //ind32
  PyObject **key_32roads_SecPatACkey = new PyObject*[self->nregions*32]; //i
  PyObject **key_8fits_SecPatDCkey = new PyObject*[self->nregions*8]; //ind8
  PyObject **key_8fits_SecPatACkey = new PyObject*[self->nregions*8]; //ind8



  Py_ssize_t nevents(0);

   /* HHHHH check which values were taken HHHH */
  for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
    // HHHHHHHHHHHHHHHH for key_fill HHHHHHHHHHHHHHHHHHHHH  //
    for (int isub=0;isub!=temp_sub;++isub) { // sub-region loop
      int ielem = ireg*temp_sub+isub;
      // HHH road ind32 HHH //
      for(int ind32=0; ind32<32; ind32++){
	int ielem_ind32 = ielem*32+ind32;
	if(temp_sub==1) key_32roads_SecPatDCkey[ielem_ind32] = PyString_FromFormat("NRoadsLambR%dLin%d_SecPatDC",ielem,ind32);
	if(temp_sub==1) key_32roads_SecPatACkey[ielem_ind32] = PyString_FromFormat("NRoadsLambR%dLin%d_SecPatAC",ielem,ind32);
	//	if(PyDict_Contains(self->numbers,key_32roads_SecPatDCkey[ielem])==1){
	  list_32roads_SecPatDCkey[ielem_ind32] = PyDict_GetItem(self->numbers,key_32roads_SecPatDCkey[ielem_ind32]);
	  list_32roads_SecPatACkey[ielem_ind32] = PyDict_GetItem(self->numbers,key_32roads_SecPatACkey[ielem_ind32]);
	  // Py_ssize_t tmpnevts = PyList_Size(list_nroads[ielem]);
	  // if (nevents && tmpnevts!=nevents) {
	  //   return NULL;
	  // }else {
	  //   nevents = tmpnevts;
	  // }
	  //	cout<<"yes road"<<endl;
	// }else{
	//   list_nroads[ielem] = NULL;
	// }
      }

      // HHH fit ind8 HHH //
      for(int ind8=0; ind8<8; ind8++){
	int ielem_ind8 = ielem*8+ind8;
	if(temp_sub==1) key_8fits_SecPatDCkey[ielem_ind8] = PyString_FromFormat("NFitsLambR%dLin%d_SecPatDC",ielem,ind8);
	if(temp_sub==1) key_8fits_SecPatACkey[ielem_ind8] = PyString_FromFormat("NFitsLambR%dLin%d_SecPatAC",ielem,ind8);
	  list_8fits_SecPatDCkey[ielem_ind8] = PyDict_GetItem(self->numbers,key_8fits_SecPatDCkey[ielem_ind8]);
	  list_8fits_SecPatACkey[ielem_ind8] = PyDict_GetItem(self->numbers,key_8fits_SecPatACkey[ielem_ind8]);
      }

      // HHH road HHH //
      if(temp_sub!=1) key_roads_narkey[ielem] = PyString_FromFormat("NRoadsR%dS%d",ireg,isub);
      if(temp_sub==1) key_roads_narkey[ielem] = PyString_FromFormat("NRoadsR%d",ireg);
      if(PyDict_Contains(self->numbers,key_roads_narkey[ielem])==1){
	list_nroads[ielem] = PyDict_GetItem(self->numbers,key_roads_narkey[ielem]);
 	Py_ssize_t tmpnevts = PyList_Size(list_nroads[ielem]);
 	if (nevents && tmpnevts!=nevents) {
 	  return NULL;
 	}else {
 	  nevents = tmpnevts;
 	}
	//	cout<<"yes road"<<endl;
      }else{
 	list_nroads[ielem] = NULL;
      }
      // HHH fit HHH //
      if(temp_sub!=1) key_tracks_nafIkey[ielem] = PyString_FromFormat("NFitsIR%dS%d",ireg,isub); // What is "I"....
      if(temp_sub==1) key_tracks_nafIkey[ielem] = PyString_FromFormat("NFitsIR%d",ireg); // What is "I"....
      if(PyDict_Contains(self->numbers,key_tracks_nafIkey[ielem])==1){
	list_nfits[ielem] = PyDict_GetItem(self->numbers,key_tracks_nafIkey[ielem]);
 	Py_ssize_t tmpnevts = PyList_Size(list_nfits[ielem]);
 	if (nevents && tmpnevts!=nevents) {
 	  return NULL;
 	}else {
 	  nevents = tmpnevts;
 	}
	//	cout<<"yes fit"<<endl;
      }else{
 	list_nfits[ielem] = NULL;
      }
      // HHH fit 2ndHHH //
      if(temp_sub!=1) key_tracks_nafkey[ielem] = PyString_FromFormat("NFitsR%dS%d",ireg,isub); // What is "I"....
      if(temp_sub==1) key_tracks_nafkey[ielem] = PyString_FromFormat("NFitsR%d",ireg); // What is "I"....
      if(PyDict_Contains(self->numbers,key_tracks_nafkey[ielem])==1){
	list_nfits_2nd[ielem] = PyDict_GetItem(self->numbers,key_tracks_nafkey[ielem]);
 	Py_ssize_t tmpnevts = PyList_Size(list_nfits_2nd[ielem]);
 	if (nevents && tmpnevts!=nevents) {
 	  return NULL;
 	}else {
 	  nevents = tmpnevts;
 	}
	//	cout<<"yes fit_2nd"<<endl;
      }else{
 	list_nfits_2nd[ielem] = NULL;
      }
      // HHH track HHH //
      if(temp_sub!=1) key_tracks_natkey[ielem] = PyString_FromFormat("NTracksR%dS%d",ireg,isub); // What is "I".... 
      if(temp_sub==1) key_tracks_natkey[ielem] = PyString_FromFormat("NTracksR%d",ireg); // What is "I".... 
      if(PyDict_Contains(self->numbers,key_tracks_natkey[ielem])==1){
	list_ntracks[ielem] = PyDict_GetItem(self->numbers,key_tracks_natkey[ielem]);
 	Py_ssize_t tmpnevts = PyList_Size(list_ntracks[ielem]);
 	if (nevents && tmpnevts!=nevents) {
 	  return NULL;
 	}else {
 	  nevents = tmpnevts;
 	}
	//	cout<<"yes track"<<endl;
      }else{
 	list_ntracks[ielem] = NULL;
      }
      // IIIIIIII layer loop IIIIIIII //
      for (int il=0;il!=self->nlayers;++il) { // layer loop
	// HHH cluster HHH //
      if(temp_sub!=1) key_roads_nckey[ielem*self->nlayers+il] = PyString_FromFormat("NClustersReg%dSub%dL%d",ireg,isub,il);
      if(temp_sub==1) key_roads_nckey[ielem*self->nlayers+il] = PyString_FromFormat("NClustersReg%dL%d",ireg,il);
	if(PyDict_Contains(self->numbers,key_roads_nckey[ielem*self->nlayers+il])==1){
	  list_nclusters[ielem*self->nlayers+il] = PyDict_GetItem(self->numbers,key_roads_nckey[ielem*self->nlayers+il]);
	  Py_ssize_t tmpnevts = PyList_Size(list_nclusters[ielem*self->nlayers+il]);
	  if (nevents && tmpnevts!=nevents) {
	    return NULL;
	  }else {
	    nevents = tmpnevts;
	  }
	  //	  cout<<"yes cluster"<<endl;
	}else{
	  list_nclusters[ielem*self->nlayers+il] = NULL;
	}
	// HHH ss  HHH //
      if(temp_sub!=1) key_roads_nsskey[ielem*self->nlayers+il] = PyString_FromFormat("NSSReg%dSub%dL%d",ireg,isub,il);
      if(temp_sub==1) key_roads_nsskey[ielem*self->nlayers+il] = PyString_FromFormat("NSSReg%dL%d",ireg,il);
	if(PyDict_Contains(self->numbers,key_roads_nsskey[ielem*self->nlayers+il])==1){
	  list_nss[ielem*self->nlayers+il] = PyDict_GetItem(self->numbers,key_roads_nsskey[ielem*self->nlayers+il]);
	  Py_ssize_t tmpnevts = PyList_Size(list_nss[ielem*self->nlayers+il]);
	  if (nevents && tmpnevts!=nevents) {
	    return NULL;
	  }else {
	    nevents = tmpnevts;
	  }
	  //	  cout<<"yes ss"<<endl;
	}else{
	  list_nss[ielem*self->nlayers+il] = NULL;
	}
      } // end layer loop
    } // end sug-region loop
    // HHHHHHHHHHHHHHHH for key_fill HHHHHHHHHHHHHHHHHHHHH  //
  } // end region loop

  // HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH //
  // H Start of Event Simulation                    H //      ------------------
  // HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH //
  // HHHHH Special HHHHHH //
  int endcap_balance=1;
  int endcap_balance_check=1;
  // HHH time simulation valiables HHH //
  FTKTimeSim* sim = new FTKTimeSim();
  InputValues inv[NPU];
  //#define FTKTimeSim_Debug 3                // duplicate with STKTimeSim  3 summary 4 full
  //#define NASSOCIATIVEMEMORY 4       // new board            // duplicate with STKTimeSim
  //#define NREGION 8                         // duplicate with STKTimeSim
  //#define NLAYER 11 // pixel 0-2 sct 3-10   // duplicate with STKTimeSim
  //#define NLAYERA 7 // pixel 0-2 sct 3-10   // duplicate with STKTimeSim
#define L1rate 100   //kHz (100 kHz) LVL1 rate
#define SOVERLAP 1 // 1 : consider events overlap
  //#define time_kill_TF_proc 0 // 1msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
  //#define time_kill_TF_proc 1000000 // 1msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
  //#define time_kill_TF_proc 500000 // 1msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
  int nrbin=2; // new for 64 tower
  int switchEvent=3;  // 0:VBFtautaulh 3E 1: VBF 1E 2:DiJet 3E 3: DiJet 1E
  int nloop = 0;  // # events for loop (0 is full)
  int debug = 0;
  int s_plot = 1; // 1 : plot,  0 : no plot
  int dump_event = 1; // event # for dump information (0 non)
//   int n_dump=21;
//   int array_dump_event[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,23,24,25,26,99};
  int n_dump=50;
  int array_dump_event[]={0,1,2,3,4,5,6,7,8,9,23,24,25,26,27,28,29,30,31,32,100,101,102,103,104,105,106,107,108,109,800,801,802,803,804,805,806,807,808,809,980,981,982,983,984,985,986,987,988,989};
  //  int n_dump=13;
  //  int array_dump_event[]={24,188,272,342,371,462,513,671,695,744,827,903,914};

  int show_special_table=0; // table for the ind32 and ind8 AM and TF number check!!!!

  int dump; 
  //  double sf_hit_overlapregion = 1.43; // for use hit/sf, to minimum overlap region study for DO hits. // 1 is no sf // 1.43 from Anton
  double dumy = -999;
  int s_split = 0;
  int s_Chi2SecondStage = 1;
  //  double sec_sf = 1; // if 2 : secstage road and fit is 1/2 of first stage //
  int nloopsloop = 1; // def=1; if nloop!=0, it is not work //for loop of event loop for check overlap 
  //  int sub =1; // use subregion reduction? 
  int first_stage_layer = self->nlayers;//7;  // artitecture
  int temp_denom=1000; // 1000 nano 1 micro

  int ntrack_tot=0, ntrack_ok=0;
  int divi=0;

  // normal region
  double sf_nregion = 1;
  double sf_nregion_hit = 1;
  double hit_div[NLAYER] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  // HH temp parameter for buffer study HH // // int->double Mar5Naoki
  double nroad_rw_integ_temp[NPU]={0}, nroad_norw_integ_temp[NPU]={0};// ..cofusing norw is after rw..... // no use nroad_rw....
  double nroad_norw_AM_integ_temp[NPU][NASSOCIATIVEMEMORY]={{0}};
  double nroad_rw_AM_integ_temp[NPU][NASSOCIATIVEMEMORY]={{0}};
  double ncomb_integ_temp[NPU]={0};
  double ntrack_integ_temp[NPU]={0};
  double ntrack_hw_integ_temp[NPU]={0};
  double nfit_integ_temp[NPU]={0};
  double nfit_2nd_integ_temp[NPU]={0};
  double nhit_raw_integ_temp[NPU][NLAYER]={{0}};
  double nhit_hw_integ_temp[NPU][NLAYER]={{0}}, nhit_nohw_integ_temp[NPU][NLAYER]={{0}};  // ..cofusing nohw is after hw.....
  double nhit_raw_tot_integ_temp[NPU]={0};  // Creep! =[0]+....[NLAYER]
  double nhit_hw_tot_integ_temp[NPU]={0}, nhit_nohw_tot_integ_temp[NPU]={0}; // Creep!
  double nss_integ_temp[NPU][NLAYER]={{0}};  // ..cofusing nohw is after hw.....
  double nss_tot_integ_temp[NPU]={0}; // Creep!

  
  cout<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<endl;
  cout<<"H  Timing Simulation Part             H"<<endl;
  cout<<"H total # of events ="<<nevents<<endl;
  cout<<"H FTKSim output "<<endl;
  cout<<"H # of Region = "<<self->nregions<<endl;
  cout<<"H # of PU = "<<NPU<<endl;
  cout<<"H # of SubRegion (if(0) sub=1 mod )= "<<temp_sub<<endl;
  cout<<"H # of Layers = "<<self->nlayers<<endl;
  cout<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<endl;
  // HHH init HHH //
  for(int i=0; i<NPU ; i++){ // for first events
    for(int j=0; j<NLAYER ; j++)inv[i].nhit_nohw_integ_history[j].push_back(0);  
    inv[i].TF_ew_out_history.push_back(0);
    inv[i].data_exist=0;
  }
  
  /// ***** event loop ***** // ------------------- event loop start -----------------------------------------------  
  for (Py_ssize_t ievt=0;ievt!=nevents;++ievt) { // event loop
    // HH dump plot event HH //
    dump=0;
    if(dump_event == ievt)dump=1;
    for(int d=0 ; d<n_dump ; d++) if(array_dump_event[d] == ievt ) dump=1;
    // start //
    int counter_existregion=0; double ntrack_SecondStage=0; double nfit_SecondStage=0; double temp_secfit=0, temp_secfit_integ=0;
    if(FTKTimeSim_Debug>3)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
    if(debug>0 || FTKTimeSim_Debug>1)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLL event number = "
					 <<ievt<<"  LLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
    if(FTKTimeSim_Debug>3)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
    // HH set the inputs valiables HH // ---------- set start ---------
    for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
      inv[ireg*2].nentry=ievt;
      inv[ireg*2].LVL1Gap= (1000000/L1rate); // rate to gap
      inv[ireg*2].s_overlap = SOVERLAP;  // for event overlap need1!
      inv[ireg*2].t_kill_TF = time_kill_TF_proc;//time_kill_TF_proc;
      inv[ireg*2].tt = 0;
      inv[ireg*2].split=s_split;
      inv[ireg*2].Chi2SecondStage=s_Chi2SecondStage;
      inv[ireg*2].FirstStage=first_stage_layer;
      inv[ireg*2+1].nentry=ievt;
      inv[ireg*2+1].LVL1Gap= (1000000/L1rate); // rate to gap
      inv[ireg*2+1].s_overlap = SOVERLAP;  // for event overlap need1!
      inv[ireg*2+1].t_kill_TF = time_kill_TF_proc;//time_kill_TF_proc;
      inv[ireg*2+1].tt = 0;
      inv[ireg*2+1].split=s_split;
      inv[ireg*2+1].Chi2SecondStage=s_Chi2SecondStage;
      inv[ireg*2+1].FirstStage=first_stage_layer;

      double sum_cluster[NLAYER]={0}, sum_ss[NLAYER]={0};
      double sum_road=0, sum_fit=0, sum_fit_2nd=0, sum_track=0;
      int dataexist=0;
      for (int isub=0;isub!=temp_sub;++isub) { // sub-region loop
	int ielem = ireg*temp_sub+isub;
	if(list_nroads[ielem] !=NULL) dataexist=1;
	if(list_nroads[ielem] !=NULL) sum_road +=PyLong_AsLong(PyList_GetItem(list_nroads[ielem],ievt));
	if(list_nfits[ielem]  !=NULL) sum_fit  +=PyLong_AsLong(PyList_GetItem(list_nfits[ielem],ievt));
	if(list_nfits_2nd[ielem]  !=NULL) sum_fit_2nd  +=PyLong_AsLong(PyList_GetItem(list_nfits_2nd[ielem],ievt));
	if(list_ntracks[ielem]!=NULL) sum_track+=PyLong_AsLong(PyList_GetItem(list_ntracks[ielem],ievt));
	//	cout<<" Event # = "<<ievt<<"  ireg isub= "<<ireg<<" "<<isub<<endl;
	//	if(list_nroads[ielem] !=NULL)cout<<PyLong_AsLong(PyList_GetItem(list_nroads[ielem],ievt))<<endl;
	//	if(list_nfits[ielem] !=NULL)cout<<PyLong_AsLong(PyList_GetItem(list_nfits[ielem],ievt))<<endl;
	//	if(list_ntracks[ielem] !=NULL)cout<<PyLong_AsLong(PyList_GetItem(list_ntracks[ielem],ievt))<<endl;
	for (int il=0;il!=self->nlayers;++il) { // layer loop
	  if(list_nclusters[ielem*self->nlayers+il] != NULL) sum_cluster[il] += PyLong_AsLong(PyList_GetItem(list_nclusters[ielem*self->nlayers+il],ievt));
	  if(list_nss[ielem*self->nlayers+il] != NULL) sum_ss[il] += PyLong_AsLong(PyList_GetItem(list_nss[ielem*self->nlayers+il],ievt));
	  //	  if(list_nclusters[ielem*self->nlayers+il] != NULL) cout<<PyLong_AsLong(PyList_GetItem(list_nclusters[ielem*self->nlayers+il],ievt))<<endl;
	  //	  if(list_nss[ielem*self->nlayers+il] != NULL) cout<<PyLong_AsLong(PyList_GetItem(list_nss[ielem*self->nlayers+il],ievt))<<endl;
	} // end layer loop
      } // end sub reg loop
      // HHHH Fill to inv for FTKTim HHHH // =======================================================================
      if(dataexist){
 	if(FTKTimeSim_Debug>4) cout<<" Event # = "<<ievt<<"  ireg = "<<ireg<<" "<<endl;
 	if(FTKTimeSim_Debug>4) cout<<"road fit fit2nd track  ="<<sum_road<<" "<<sum_fit<<" "<<sum_fit_2nd<<"  "<<sum_track<<endl;
 	if(FTKTimeSim_Debug>4) cout<<"ss  ="<<sum_ss[0]<<" "<<sum_ss[1]<<" "<<sum_ss[2]<<" "<<sum_ss[3]<<" ..."<<endl;
 	if(FTKTimeSim_Debug>4) cout<<"clusters  ="<<sum_cluster[0]<<" "<<sum_cluster[1]<<" "<<sum_cluster[2]<<" "<<sum_cluster[3]<<" ..."<<endl;
 	cout<<" Event # = "<<ievt<<"  ireg = "<<ireg<<" "<<endl;
 	cout<<"road fit fit2nd track  ="<<sum_road<<" "<<sum_fit<<" "<<sum_fit_2nd<<"  "<<sum_track<<endl;
 	cout<<"ss  ="<<sum_ss[0]<<" "<<sum_ss[1]<<" "<<sum_ss[2]<<" "<<sum_ss[3]<<" ..."<<endl;
 	cout<<"clusters  ="<<sum_cluster[0]<<" "<<sum_cluster[1]<<" "<<sum_cluster[2]<<" "<<sum_cluster[3]<<" ..."<<endl;
      }
      // fill road fit trak each event // change at Mar3
      inv[ireg*2].nroad_rw=sum_road/2.; inv[ireg*2].nroad_norw=sum_road/2.; // temp
      inv[ireg*2].nfit=sum_fit/2.;
      inv[ireg*2].ntrack_hw=sum_track/2.; inv[ireg*2].ntrack=sum_track/2.; // temp

      inv[ireg*2+1].nroad_rw=sum_road/2.; inv[ireg*2+1].nroad_norw=sum_road/2.; // temp
      inv[ireg*2+1].nfit=sum_fit/2.;
      inv[ireg*2+1].ntrack_hw=sum_track/2.; inv[ireg*2+1].ntrack=sum_track/2.; // temp
      // adding event total road fit track 
      nroad_rw_integ_temp[ireg*2]+=sum_road/2.; nroad_norw_integ_temp[ireg*2]+=sum_road/2.;
      nfit_integ_temp[ireg*2]+=sum_fit/2.;
      nfit_2nd_integ_temp[ireg*2]+=sum_fit_2nd/2.;
      ntrack_hw_integ_temp[ireg*2]+=sum_track/2.; ntrack_integ_temp[ireg*2]+=sum_track/2.;

      nroad_rw_integ_temp[ireg*2+1]+=sum_road/2.; nroad_norw_integ_temp[ireg*2+1]+=sum_road/2.;
      nfit_integ_temp[ireg*2+1]+=sum_fit/2.;
      nfit_2nd_integ_temp[ireg*2+1]+=sum_fit_2nd/2.;
      ntrack_hw_integ_temp[ireg*2+1]+=sum_track/2.; ntrack_integ_temp[ireg*2+1]+=sum_track/2.;
      // fill integral
      inv[ireg*2].nroad_rw_integ=nroad_rw_integ_temp[ireg*2];
      inv[ireg*2].nroad_norw_integ=nroad_norw_integ_temp[ireg*2];
      inv[ireg*2].nfit_integ=nfit_integ_temp[ireg*2];
      inv[ireg*2].ntrack_integ=ntrack_integ_temp[ireg*2];
      inv[ireg*2].ntrack_hw_integ=ntrack_hw_integ_temp[ireg*2];
      inv[ireg*2+1].nroad_rw_integ=nroad_rw_integ_temp[ireg*2+1];
      inv[ireg*2+1].nroad_norw_integ=nroad_norw_integ_temp[ireg*2+1];
      inv[ireg*2+1].nfit_integ=nfit_integ_temp[ireg*2+1];
      inv[ireg*2+1].ntrack_integ=ntrack_integ_temp[ireg*2+1];
      inv[ireg*2+1].ntrack_hw_integ=ntrack_hw_integ_temp[ireg*2+1];
      if(dataexist){
	counter_existregion++;
	ntrack_SecondStage+=sum_track/2.;
	nfit_SecondStage+=sum_fit_2nd/2.;
	inv[ireg*2].SecStage_nfit=temp_secfit;  // temporallyy solution
	inv[ireg*2].SecStage_nfit_integ=temp_secfit_integ;  // temporallyy solution
	inv[ireg*2+1].SecStage_nfit=temp_secfit;  // temporallyy solution
	inv[ireg*2+1].SecStage_nfit_integ=temp_secfit_integ;  // temporallyy solution
	//	cout<<"Specail Check 0 "<<counter_existregion<<"%"<<nmergeSec<<counter_existregion%nmergeSec<<"     "<<ntrack_SecondStage<<"    sum_track"<<sum_track<<endl;
	if(counter_existregion%nmergeSec==0){
	  for(int bb=0; bb<nmergeSec; bb++){
	    inv[ireg*2-bb].SecStage_nfit=nfit_SecondStage;
	    nfit_2nd_integ_temp[ireg*2-bb]+=nfit_SecondStage;
	    inv[ireg*2-bb].SecStage_nfit_integ=nfit_2nd_integ_temp[ireg*2-bb];
	    //	    cout<<"Special Check 1  inv["<<ireg-bb<<"].SecStage_nfit="<<nfit_SecondStage<<endl;
	  }
	  temp_secfit=nfit_SecondStage;
	  temp_secfit_integ=nfit_2nd_integ_temp[ireg*2];
	  temp_secfit_integ=nfit_2nd_integ_temp[ireg*2+1];
	  nfit_SecondStage=0;
	  ntrack_SecondStage=0;
	}
      }


      double cluster_tot=0, ss_tot=0; 
      for (int il=0;il!=self->nlayers;++il) { // layer loop for fill
	// fill
	inv[ireg*2].nhit_nohw[il]=sum_cluster[il];
	inv[ireg*2].nss[il]=sum_ss[il];
	inv[ireg*2+1].nhit_nohw[il]=sum_cluster[il];
	inv[ireg*2+1].nss[il]=sum_ss[il];
	// sum of layer
	cluster_tot+=sum_cluster[il];
	ss_tot+=sum_ss[il];
	// sum 0 to current event for buffer study (integrall)
	nhit_nohw_integ_temp[ireg*2][il]+=sum_cluster[il];
	nss_integ_temp[ireg*2][il]+=sum_ss[il];
	nhit_nohw_integ_temp[ireg*2+1][il]+=sum_cluster[il];
	nss_integ_temp[ireg*2+1][il]+=sum_ss[il];
	// fill integral
	inv[ireg*2].nhit_nohw_integ[il]=nhit_nohw_integ_temp[ireg*2][il];
	inv[ireg*2].nss_integ[il]=nss_integ_temp[ireg*2][il];
	inv[ireg*2+1].nhit_nohw_integ[il]=nhit_nohw_integ_temp[ireg*2+1][il];
	inv[ireg*2+1].nss_integ[il]=nss_integ_temp[ireg*2+1][il];
      } // end layer loop for fill
      if(cluster_tot!=0) inv[ireg*2].data_exist=1; else inv[ireg*2].data_exist=0;
      if(cluster_tot!=0) inv[ireg*2+1].data_exist=1; else inv[ireg*2+1].data_exist=0;
      inv[ireg*2].nss_tot=ss_tot;
      inv[ireg*2].nhit_nohw_tot=cluster_tot;
      inv[ireg*2+1].nss_tot=ss_tot;
      inv[ireg*2+1].nhit_nohw_tot=cluster_tot;
      // #road for each AM //
      //      for(int i=0 ; i<NPU; i++){
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
	//	  inv[ireg].nroad_norw_AM[j]=inv[ireg].nroad_norw/NASSOCIATIVEMEMORY;
	//	  inv[ireg].nroad_norw_AM_integ[j]=inv[ireg].nroad_norw_integ/NASSOCIATIVEMEMORY;

	// inv[ireg*2].nroad_norw_AM[j]=sum_road/(NASSOCIATIVEMEMORY*2.);  //*2 is the 2pu/reg
	// inv[ireg*2].nroad_norw_AM_integ[j]=nroad_norw_integ_temp[ireg*2]/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2].nroad_rw_AM[j]=sum_road/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2].nroad_rw_AM_integ[j]=nroad_norw_integ_temp[ireg*2]/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2+1].nroad_norw_AM[j]=sum_road/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2+1].nroad_norw_AM_integ[j]=nroad_norw_integ_temp[ireg*2+1]/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2+1].nroad_rw_AM[j]=sum_road/(NASSOCIATIVEMEMORY*2.);
	// inv[ireg*2+1].nroad_rw_AM_integ[j]=nroad_norw_integ_temp[ireg*2+1]/(NASSOCIATIVEMEMORY*2.);


	// New ind32 roads number
	nroad_norw_AM_integ_temp[ireg*2][j]+=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j],ievt));
	nroad_rw_AM_integ_temp[ireg*2][j]  +=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j],ievt));
	nroad_norw_AM_integ_temp[ireg*2+1][j]+=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j+16],ievt));
	nroad_rw_AM_integ_temp[ireg*2+1][j]  +=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j+16],ievt));

	inv[ireg*2].nroad_norw_AM[j]=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j],ievt));
	inv[ireg*2+1].nroad_norw_AM[j]=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j+16],ievt));

	inv[ireg*2].nroad_rw_AM[j]=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j],ievt));
	inv[ireg*2+1].nroad_rw_AM[j]=PyLong_AsLong(PyList_GetItem(list_32roads_SecPatDCkey[ireg*32+j+16],ievt));

	inv[ireg*2].nroad_rw_AM_integ[j]=(double)nroad_norw_AM_integ_temp[ireg*2][j];
	inv[ireg*2+1].nroad_norw_AM_integ[j]=(double)nroad_norw_AM_integ_temp[ireg*2+1][j];

	inv[ireg*2].nroad_rw_AM_integ[j]=(double)nroad_norw_AM_integ_temp[ireg*2][j];
	inv[ireg*2+1].nroad_rw_AM_integ[j]=(double)nroad_norw_AM_integ_temp[ireg*2+1][j];

	//	cout<<"special check before REBal AM inv["<<ireg*2<<"].nroad_norw_AM["<<j<<"]"<< inv[ireg*2].nroad_norw_AM[j]<<"   input is "<<inv[ireg*2].nroad_norw<<"  "<<sum_road<<endl;
	//	cout<<"special check before REBal AM inv["<<ireg*2+1<<"].nroad_norw_AM["<<j<<"]"<< inv[ireg*2+1].nroad_norw_AM[j]<<"   input is "<<inv[ireg*2+1].nroad_norw<<"  "<<sum_road<<endl;
	
      }// fill nAM endxx

      for(int jtf=0 ; jtf<NTRACKFITTER; jtf++){      
	inv[ireg*2].nfit_TF[jtf]= PyLong_AsLong(PyList_GetItem(list_8fits_SecPatDCkey[ireg*NTRACKFITTER*2+jtf],ievt));
	inv[ireg*2+1].nfit_TF[jtf]= PyLong_AsLong(PyList_GetItem(list_8fits_SecPatDCkey[ireg*NTRACKFITTER*2+jtf+NTRACKFITTER],ievt));
	//	cout<<"special check before REBal TF inv["<<ireg*2<<"].nfits_TF["<<jtf<<"]"<< inv[ireg*2].nfit_TF[jtf]<<"   input is "<<      inv[ireg*2].nfit<<endl;
	//	cout<<"special check before REBal TF inv["<<ireg*2+1<<"].nfits_TF["<<jtf<<"]"<< inv[ireg*2+1].nfit_TF[jtf]<<"   input is "<<     inv[ireg*2+1].nfit<<endl;


      }// fill nTF endxx

      // HHHH End Fill to inv for FTKTim HHHH // =======================================================================

      //      sim->SetInputValueForRegion(ireg,inv[ireg]); // tori
    } // end region loop



    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ//
    // Special table for check the inv
    if(show_special_table==1){
      cout<<"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"<<endl;
      cout<<"Z special tale "<<endl;
      cout<<"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"<<endl;
      double sum_AM=0;
      double sum_TF=0;

      for (int ireg=0;ireg!=NPU;++ireg){
	if(ireg%2==0) {sum_AM=0; sum_TF=0;}
	for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){      
	  cout<<"BeforeBEC special check"<<ievt<<" AM inv["<<ireg<<"].nroad_norw_AM["<<j<<"]"<< inv[ireg].nroad_norw_AM[j]<<"   input is "<<inv[ireg].nroad_norw/16.<<endl;
	  sum_AM+=inv[ireg].nroad_norw_AM[j];
	}
	cout<<"total AM / pu = "<<sum_AM<<"       orig tot"<<inv[ireg].nroad_norw*2<<endl;
	for(int jtf=0 ; jtf<NTRACKFITTER; jtf++){      
	  cout<<"BeforeBEC special check"<<ievt<<" TF inv["<<ireg<<"].nfits_TF["<<jtf<<"]"<< inv[ireg].nfit_TF[jtf]<<"   input is "<<inv[ireg].nfit/4.<<endl;
	  sum_TF+=inv[ireg].nfit_TF[jtf];
	}
	cout<<"total TF / pu = "<<sum_TF<<"      orig tot = "<<inv[ireg].nfit*2<<endl;

      }
    }
    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ//


    //  ReFill
    //  LLLLLLLLLL Correction for the remapping of barrel and endcaps LLLLLLLLLLLLLLLL  //
    double move_nroad_rw[NPU]={0};
    double move_nroad_norw[NPU]={0};
    double move_nfit[NPU]={0};
    double move_ntrack_hw[NPU]={0};
    double move_nroad_rw_integ[NPU]={0};
    double move_nroad_norw_integ[NPU]={0};
    double move_nfit_integ[NPU]={0};
    double move_ntrack_integ[NPU]={0};
    double move_ntrack_hw_integ[NPU]={0};
    double move_nroad_norw_AM[NPU][NASSOCIATIVEMEMORY]={{0}};
    double move_nroad_rw_AM[NPU][NASSOCIATIVEMEMORY]={{0}};
    double move_nfit_TF[NPU][NTRACKFITTER]={{0}};

    double weight=9999999;
    if(endcap_balance){
      weight=0.25;
    }else{
      weight=0.;
    }
    cout<<"New pu 128 version rebalancing"<<endl;
    cout<<"endcap_balance = "<<endcap_balance<<"   weight = "<<weight<<endl;
    //    for (int ireg=0;ireg!=self->nregions;++ireg) { // move number
    for (int ireg=0;ireg!=NPU;++ireg) { // move number
      move_nroad_rw[ireg]              = inv[ireg].nroad_rw	        *weight;
      move_nroad_norw[ireg]              = inv[ireg].nroad_norw	        *weight;
      move_nfit[ireg]		       = inv[ireg].nfit		        *weight;
      move_ntrack_hw[ireg]	       = inv[ireg].ntrack_hw	        *weight;
      move_nroad_rw_integ[ireg]	       = inv[ireg].nroad_rw_integ	*weight; 
      move_nroad_norw_integ[ireg]      = inv[ireg].nroad_norw_integ 	*weight;
      move_nfit_integ[ireg]	       = inv[ireg].nfit_integ	 	*weight;
      move_ntrack_integ[ireg]	       = inv[ireg].ntrack_integ	        *weight; 
      move_ntrack_hw_integ[ireg]       = inv[ireg].ntrack_hw_integ  	*weight;
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
	move_nroad_norw_AM[ireg][j] =	inv[ireg].nroad_norw_AM[j] *weight;
	move_nroad_rw_AM[ireg][j] =   inv[ireg].nroad_rw_AM[j]   *weight;
      }
      for(int j=0 ; j<NTRACKFITTER ; j++){
	move_nfit_TF[ireg][j] =	inv[ireg].nfit_TF[j] *weight;
      }

      if(endcap_balance_check)cout<<"BEC ReBalance CheckCheck nfit before["<<ireg<<"] = "<<inv[ireg].nfit<<endl;
    }

    for (int ireg=0;ireg!=NPU;++ireg) { // add
      //      if(ireg<16 || ireg>47){
      if(ireg<32 || ireg>95){
	inv[ireg].nroad_rw	        = inv[ireg].nroad_rw	               - move_nroad_rw[ireg]              ; 
	inv[ireg].nroad_norw	        = inv[ireg].nroad_norw	               - move_nroad_norw[ireg]              ; 
	inv[ireg].nfit		        = inv[ireg].nfit		               - move_nfit[ireg]		       ; 
	inv[ireg].ntrack_hw	        = inv[ireg].ntrack_hw	               - move_ntrack_hw[ireg]	       ; 
	inv[ireg].nroad_rw_integ	= inv[ireg].nroad_rw_integ	       - move_nroad_rw_integ[ireg]	       ; 
	inv[ireg].nroad_norw_integ 	= inv[ireg].nroad_norw_integ 	       - move_nroad_norw_integ[ireg]      ; 
	inv[ireg].nfit_integ	 	= inv[ireg].nfit_integ	 	       - move_nfit_integ[ireg]	       ; 
	inv[ireg].ntrack_integ	        = inv[ireg].ntrack_integ	               - move_ntrack_integ[ireg]	       ; 
	inv[ireg].ntrack_hw_integ  	= inv[ireg].ntrack_hw_integ  	       - move_ntrack_hw_integ[ireg]       ; 
	for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){				 				 
	  inv[ireg].nroad_norw_AM[j] 	= inv[ireg].nroad_norw_AM[j] 	       - move_nroad_norw_AM[ireg][j] ;	
	  inv[ireg].nroad_rw_AM[j]      = inv[ireg].nroad_rw_AM[j]             - move_nroad_rw_AM[ireg][j] ;   
	}
	for(int j=0 ; j<NTRACKFITTER ; j++){				 				 
	  inv[ireg].nfit_TF[j] 	= inv[ireg].nfit_TF[j] 	       - move_nfit_TF[ireg][j] ;	
	}
      }else{
	  int addreg = 0;
	  if( ireg >=64 ) addreg = ireg + 32;
	  else addreg = ireg - 32 ;
	  inv[ireg].nroad_rw	        = inv[ireg].nroad_rw	               + move_nroad_rw[addreg];
	  inv[ireg].nroad_norw	        = inv[ireg].nroad_norw	               + move_nroad_norw[addreg];
	  inv[ireg].nfit	        = inv[ireg].nfit		       + move_nfit[addreg];
	  inv[ireg].ntrack_hw	        = inv[ireg].ntrack_hw	               + move_ntrack_hw[addreg];
	  inv[ireg].nroad_rw_integ	= inv[ireg].nroad_rw_integ	       + move_nroad_rw_integ[addreg];
	  inv[ireg].nroad_norw_integ 	= inv[ireg].nroad_norw_integ 	       + move_nroad_norw_integ[addreg];
	  inv[ireg].nfit_integ	 	= inv[ireg].nfit_integ	 	       + move_nfit_integ[addreg];
	  inv[ireg].ntrack_integ        = inv[ireg].ntrack_integ	       + move_ntrack_integ[addreg];
	  inv[ireg].ntrack_hw_integ  	= inv[ireg].ntrack_hw_integ  	       + move_ntrack_hw_integ[addreg]; 
	for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){				 				 
	  inv[ireg].nroad_norw_AM[j] 	= inv[ireg].nroad_norw_AM[j] 	       + move_nroad_norw_AM[addreg][j] ;	
	  inv[ireg].nroad_rw_AM[j]        = inv[ireg].nroad_rw_AM[j]           + move_nroad_norw_AM[addreg][j] ;   
	}
	for(int j=0 ; j<NTRACKFITTER ; j++){				 				 
	  inv[ireg].nfit_TF[j] 	= inv[ireg].nfit_TF[j] 	       + move_nfit_TF[ireg][j] ;	
	}
      }

      if(endcap_balance_check)cout<<"BEC ReBalance Check nfit after ["<<ireg<<"] = "<<inv[ireg].nfit<<endl;
    }
    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ//
    // Special table for check the inv
    if(show_special_table==1){
      cout<<"ZZZZZZ"<<endl;
      cout<<"Z special tale "<<endl;
      double sum_AM=0;
      double sum_TF=0;

      for (int ireg=0;ireg!=NPU;++ireg){
	for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){      
	  cout<<"AfterBEC special check"<<ievt<<" AM inv["<<ireg<<"].nroad_norw_AM["<<j<<"]"<< inv[ireg].nroad_norw_AM[j]<<setw(10)<<"   input is "<<inv[ireg].nroad_norw/16.<<endl;
	  sum_AM+=inv[ireg].nroad_norw_AM[j];
	}
	cout<<"total AM / pu = "<<sum_AM<<" "<<inv[ireg].nroad_norw<<endl;
	for(int jtf=0 ; jtf<NTRACKFITTER; jtf++){      
	  cout<<"AfterBEC special check"<<ievt<<" TF inv["<<ireg<<"].nfits_TF["<<jtf<<"]"<< inv[ireg].nfit_TF[jtf]<<setw(10)<<"   input is "<<inv[ireg].nfit/4.<<endl;
	  sum_TF+=inv[ireg].nfit_TF[jtf];
	}
	cout<<"total TF / pu = "<<sum_TF<<"      ave = "<<inv[ireg].nfit<<endl;

      }
    }
    //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ//

    for (int ireg=0;ireg!=NPU;++ireg) sim->SetInputValueForRegion(ireg,inv[ireg]);
    //    for (int ireg=0;ireg!=self->nregions;++ireg) sim->SetInputValueForRegion(ireg,inv[ireg]);
    // LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL //

    // **** Fill of some paremeter  **** //   //It is in FTKTimeSim Func//
    sim->FillInputValue(); 

    // **** Calculation **** //   // FTKTimeSim -> Region -> DO,AM,SecDO,TF's Calc() //
    sim->Calc();   

    // **** Counter of Killed track and fill next event **** //
    for(int i=0 ; i<NPU ; i++){
      if(sim->GetNkill(i)){
	//	cout<<"region "<<i<<" is killed "<<sim->GetNkill(i)<<endl;
      }else{
	ntrack_ok+=inv[i].ntrack;
      }
      ntrack_tot+=inv[i].ntrack;
      for(int j=0; j<NLAYER ; j++)inv[i].nhit_nohw_integ_history[j].push_back(inv[i].nhit_nohw_integ[j]);  
      inv[i].TF_ew_out_history.push_back(sim->GetTF_EwOut(i));
    }

    // **** Fill results **** //
    sim->FillResults(dump, ievt, "FTKTimOutput", temp_denom, s_split);

  } // end event loop
  // ---------------------------- event loop end--------------------------------------  
  // **** Print some input parameter **** //
  cout<<"*** PlotInputValues ***"<<endl;
  if(s_plot)sim->PlotInputValues("FTKTimOutput", s_split,nrbin); //FTKTimeSim.cc func.
  cout<<"*** PlotResults ***"<<endl;
  if(s_plot)sim->PlotResults("FTKTimOutput", temp_denom, s_split,nrbin); //FTKTimeSim.cc func.

  // summary
  //  sim->PrintResults(sf_hit_overlapregion, nloop, nloopsloop, sf_nregion, "FTKTimOutput", ntrack_ok , ntrack_tot, s_split); //FTKTimeSim.cc func.
  sim->PrintResults(dumy, nloop, nloopsloop, sf_nregion, "FTKTimOutput", ntrack_ok , ntrack_tot, s_split); //FTKTimeSim.cc func.

  //   /* clear the memory */
  //   for (int ireg=0;ireg!=self->nregions;++ireg) { // region loop
  //     if (list_nclusters[ireg]) Py_DECREF(time_DOtoAM[ireg]);
  //   } // end region loop


  // HHHH delete HHHH //
  delete [] list_nclusters;  
  delete [] list_nss;
  delete [] list_nroads;
  delete [] list_nfits;
  delete [] list_ntracks;
  
  delete [] key_roads_nckey ;
  delete [] key_roads_nsskey ;
  delete [] key_roads_narkey ;
  delete [] key_tracks_nafkey ;
  delete [] key_tracks_nafIkey ;
  delete [] key_tracks_natkey ;

  Py_RETURN_NONE;
}
