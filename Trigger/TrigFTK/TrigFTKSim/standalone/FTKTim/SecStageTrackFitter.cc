/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SecStageTrackFitter.hh"

#include <iostream>

using namespace std;

void
SecStageTrackFitter::Calc()
{
  //  int FTKTimeSim_TF_Debug = 5; // goto IBoardBase.hh
  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;
  //  double Delay = ((this->iv.nhit_nohw_tot+this->iv.nroad_norw)*this->bd_t_in); 
  double Delay = 0.;
  double TF_clear = pre_iotime[bd_index].GetEwOut() - this->iv.LVL1Gap;  // using same AM boad Ewout 
  double nkill=0;
  // *** fw_in *** //
  fw_in = iotime[bd_index].GetFwOut();
  primary_fw_in = iotime[bd_index].GetPriFwOut();// + Delay;
  if(TF_clear>fw_in && this->iv.s_overlap)fw_in = TF_clear;// + Delay; // overlap

  // *** ew_in *** //
  ew_in = iotime[bd_index].GetEwOut();
  if(ew_in < fw_in+Delay) ew_in = fw_in+Delay;// input delay
  primary_ew_in = iotime[bd_index].GetPriEwOut();
  if(primary_ew_in < primary_fw_in+Delay) primary_ew_in = primary_fw_in+Delay;
  
  if(TF_clear>ew_in && this->iv.s_overlap)fw_in = TF_clear;// + Delay; // overlap

  // *** fw_out *** //
  fw_out = fw_in + this->bd_t_delay;  // FIFO
  primary_fw_out = primary_fw_in + this->bd_t_delay;  // FIFO
  // *** ew_out *** //
  vector <double> temp_io;
  vector <double> temp_priio;
  // * proc time is bottleneck * //
  temp_io.push_back(fw_out + (this->iv.SecStage_nfit/NTRACKFITTER) * this->bd_t_proc);
  temp_priio.push_back(primary_fw_out + (this->iv.SecStage_nfit/NTRACKFITTER) * this->bd_t_proc);
  // * privious board delay is bottleneck * //
  temp_io.push_back(ew_in + this->bd_t_delay);
  temp_priio.push_back(primary_ew_in + this->bd_t_delay);
  // max //
  ew_out = GetMaxVec(temp_io);
  primary_ew_out = GetMaxVec(temp_priio);
  // **** kill signal **** //
  if(ew_out-fw_in > this->iv.t_kill_TF && this->iv.t_kill_TF){
    cout<<"SecStage TF process was killed by over time("<<ew_out-fw_in<<" nsec)  KILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILLKILL"<<endl;
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

  //  this->iotime[bd_index].SetNKillTF(nkill); // for 1st stage only no 2nd stage
  this->iotime[bd_index].Settemp_proc(this->bd_t_proc);
  
  // ***** debug ***** //
  if(FTKTimeSim_SecStageTF_Debug && FTKTimeSim_Debug>3){
    cout<<"SecStageTrackFitter::Calc() "<<endl;
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
    cout<<"#of fit_2nd = "<<this->iv.SecStage_nfit<<endl;
    cout<<"Delay = "<<Delay<<endl;
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
