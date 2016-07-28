/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackFitter.hh"

#include <iostream>

using namespace std;

void
TrackFitter::Calc()
{
  //  int FTKTimeSim_TF_Debug = 5; // goto iboardbase 
  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;

  //  double ave_hit_ss = 1.15;
  double ave_hit_ss = 1.0;

//   double nhit_max=0;
//   int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2,-99}; // 0-2:pixcel 3-:SCT to 6 DO  // Creep! goto header.... it is allowed in 6 DO.....
//   int layer_layout_7L[] = {0,0,1,2,3,5,7,9}; // 7L  // Creep! goto header.... it is allowed in 6 DO.....
//   int layer_layout_8L[] = {3,4,5,6,7,8,9,10}; // 8L  // Creep! goto header.... it is allowed in 6 DO.....

//   if(this->iv.FirstStage==7) ave_hit_ss = 1.07;
//   //  if(this->iv.FirstStage==7) ave_hit_ss = 0.7;
//   if(this->iv.FirstStage==8) ave_hit_ss = 1.15;


//   if(NDATAORGANIZER==6){// temp not used
//     for(int i=0 ; i<NDATAORGANIZER ; i ++ ){ 
//       if(nhit_max<this->iv.nhit_nohw[layer_layout[NDATAORGANIZER]]+this->iv.nhit_nohw[layer_layout[NDATAORGANIZER+1]]) nhit_max=this->iv.nhit_nohw[layer_layout[NDATAORGANIZER]]+this->iv.nhit_nohw[layer_layout[NDATAORGANIZER+1]];
//     }
//   }else if(NDATAORGANIZER==11){// ok
//     for(int i=0 ; i<NDATAORGANIZER ; i ++ ){ 
//       if(nhit_max<this->iv.nhit_nohw[NDATAORGANIZER]) nhit_max=this->iv.nhit_nohw[NDATAORGANIZER];
//     }
//   }else if(NDATAORGANIZER==8 && this->iv.split){ //ok
//     for(int i=0 ; i<NDATAORGANIZER ; i ++ ){ 
//       if(layer_layout_7L[NDATAORGANIZER]==0){
// 	if(nhit_max<this->iv.nhit_nohw[layer_layout_7L[NDATAORGANIZER]]/2) nhit_max=this->iv.nhit_nohw[layer_layout_7L[NDATAORGANIZER]]/2;
//       }else{
// 	if(nhit_max<this->iv.nhit_nohw[layer_layout_7L[NDATAORGANIZER]]) nhit_max=this->iv.nhit_nohw[layer_layout_7L[NDATAORGANIZER]];
//       }
//     }
//   }else if(NDATAORGANIZER==8 && !this->iv.split){// ok
//     for(int i=0 ; i<NDATAORGANIZER ; i ++ ){
//       if(nhit_max<this->iv.nhit_nohw[layer_layout_8L[NDATAORGANIZER]]) nhit_max=this->iv.nhit_nohw[layer_layout_8L[NDATAORGANIZER]];
//     }
//   }


  //  double InputDelay = ((this->iv.nhit_nohw_tot+this->iv.nroad_norw)*this->bd_t_in);
  //  double InputDelay = (nhit_max*this->bd_t_in); // more reallistic
  //  double InputDelay = (ave_hit_ss*this->iv.nroad_norw_AM[this->bd_index]*this->bd_t_in); // more reallistic  // bad
  double InputDelay = (ave_hit_ss*this->iv.nroad_norw_AM[0]*this->bd_t_in); // more reallistic  
  //  double InputDelay = ((this->iv.nss_tot)*this->bd_t_in); // more reallistic  2013

  //  double InputDelay = 0;
  double PreTF_clear = pre_iotime[bd_index].GetEwOut() - this->iv.LVL1Gap;  // using same AM boad Ewout 
  double nkill=0;
  vector <double> temp_io;
  vector <double> temp_priio;

  // *** fw_in *** //
  primary_fw_in = iotime[bd_index].GetPriFwOut();// + Delay;
  temp_io.push_back(iotime[bd_index].GetFwOut());
  temp_io.push_back(PreTF_clear);
  fw_in = GetMaxVec(temp_io);
  temp_io.clear();

  // *** ew_in *** //
  ew_in = iotime[bd_index].GetEwOut();
  if(ew_in < fw_in+InputDelay) ew_in = fw_in+InputDelay;// input delay
  primary_ew_in = iotime[bd_index].GetPriEwOut();
  if(primary_ew_in < primary_fw_in+InputDelay) primary_ew_in = primary_fw_in+InputDelay;

  // *** overlap ***//
  //  if(PreTF_clear>fw_in && this->iv.s_overlap)fw_in = PreTF_clear;// + InputDelay; // overlap
  //  if(PreTF_clear>ew_in && this->iv.s_overlap)ew_in = PreTF_clear;// + InputDelay; // overlap

  // *** fw_out *** //
  fw_out = fw_in + this->bd_t_delay;  // FIFO
  primary_fw_out = primary_fw_in + this->bd_t_delay;  // FIFO
  // *** ew_out *** //
  // * proc time is bottleneck * //
  double limited_nfit=0;
  if((int)TF_max_fit && ((double)TF_max_fit < this->iv.nfit)){
    limited_nfit=(double)TF_max_fit;
  }else{
    //    limited_nfit=this->iv.nfit;
    limited_nfit=this->iv.nfit_TF[this->bd_index];
  }

  // temp_io.push_back(fw_out + (limited_nfit/NTRACKFITTER) * this->bd_t_proc);    // temp before ind8
  // temp_priio.push_back(primary_fw_out + (limited_nfit/NTRACKFITTER) * this->bd_t_proc); // temp before ind8
  temp_io.push_back(fw_out + (limited_nfit) * this->bd_t_proc);
  temp_priio.push_back(primary_fw_out + (limited_nfit) * this->bd_t_proc);
  // * privious board delay is bottleneck * //
  temp_io.push_back(ew_in + this->bd_t_delay);
  temp_priio.push_back(primary_ew_in + this->bd_t_delay);
  // max //
  ew_out = GetMaxVec(temp_io);
  primary_ew_out = GetMaxVec(temp_priio);
  // **** kill signal **** //
  if(ew_out-fw_in > this->iv.t_kill_TF && this->iv.t_kill_TF){
    cout<<"-TF process was killed by over time("<<ew_out-fw_in<<" nsec)  KILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILL"<<endl;
    ew_out=fw_in+this->iv.t_kill_TF;
    primary_ew_out = primary_fw_in+this->iv.t_kill_TF;
    nkill++;
  }

  
  // **** Set iotime of DO***** //
  this->iotime[bd_index].SetFwIn(fw_in);
  this->iotime[bd_index].SetFwOut(fw_out);
  this->iotime[bd_index].SetEwIn(ew_in);
  this->iotime[bd_index].SetEwOut(ew_out);
  this->iotime[bd_index].SetPriFwIn(primary_fw_in);
  this->iotime[bd_index].SetPriFwOut(primary_fw_out);
  this->iotime[bd_index].SetPriEwIn(primary_ew_in);
  this->iotime[bd_index].SetPriEwOut(primary_ew_out);

  this->iotime[bd_index].SetNKillTF(nkill);
  this->iotime[bd_index].Settemp_proc(this->bd_t_proc);
  
  // ***** debug ***** //
  if(FTKTimeSim_TF_Debug && FTKTimeSim_Debug>3){
    cout<<"TrackFitter::Calc() "<<endl;
    cout<<"**** parameter information **** "<<endl;
    cout<<"bd_index = "<<this->bd_index<<endl;
    cout<<"bd_t_in = "<<this->bd_t_in<<endl;
    cout<<"bd_t_out = "<<this->bd_t_out<<endl;
    cout<<"bd_t_proc = "<<this->bd_t_proc<<endl;
    cout<<"bd_t_delay = "<<this->bd_t_delay<<endl;
    cout<<"***** iotime of  privious event ****"<<endl;
    cout<<"LVL1Gap = "<<this->iv.LVL1Gap<<endl;
    cout<<"fw_in = "<<this->pre_iotime[bd_index].GetFwIn()<<endl;
    cout<<"fw_out = "<<this->pre_iotime[bd_index].GetFwOut()<<endl;
    cout<<"ew_in = "<<this->pre_iotime[bd_index].GetEwIn()<<endl;
    cout<<"ew_out = "<<this->pre_iotime[bd_index].GetEwOut()<<endl;
    cout<<"**** input information **** "<<endl;
    cout<<"max_fit = "<<TF_max_fit<<endl;
    //    cout<<"#of fit = "<<limited_nfit<<"   ("<<(limited_nfit/NTRACKFITTER)*this->bd_t_proc<<" nsec)"<<endl;
    cout<<"#of fit = "<<limited_nfit<<"   ("<<(limited_nfit)*this->bd_t_proc<<" nsec)"<<endl;
    cout<<"InputDelay = "<<InputDelay<<endl;
    cout<<"# InputRoad = "<<this->iv.nroad_norw_AM[this->bd_index]<<endl;
    cout<<"problem # InputRoad = "<<this->iv.nroad_norw_AM[this->bd_index]<<"     this->iv.nroad_norw_AM["<<this->bd_index<<"]"<<endl;
    cout<<"time of kill signal = "<<this->iv.t_kill_TF<<endl;
    cout<<"**** TF time information **** "<<endl;
    cout<<"Kill process "<<nkill<<endl;
    cout<<"fw_in  = "<<setw(10)<<this->iotime[bd_index].GetFwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwIn() <<endl;
    cout<<"fw_out = "<<setw(10)<<this->iotime[bd_index].GetFwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwOut()<<endl;
    cout<<"ew_in  = "<<setw(10)<<this->iotime[bd_index].GetEwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwIn() <<endl;
    cout<<"ew_out = "<<setw(10)<<this->iotime[bd_index].GetEwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwOut()<<endl;
    cout<<" -----------------------------"<<endl;
  }






  
}
