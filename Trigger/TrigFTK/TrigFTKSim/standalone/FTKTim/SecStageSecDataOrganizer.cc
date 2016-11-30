/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SecStageSecDataOrganizer.hh"

#include <iostream>

using namespace std;

void
SecStageSecDataOrganizer::Calc()
{
  int FTKTimeSim_DO_Debug = 0;
  //int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2,-99}; // 0-2:pixcel 3-:SCT to 6 DO  // Creep! goto header....
  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;
  double SecDO_clear =  pre_iotime[bd_index].GetEwOut() - this->iv.LVL1Gap;  // using same AM boad Ewout 
  // *** Delay *** //
  //  double InputDelay = (this->iv.nhit_nohw_tot*this->bd_t_in);  //

  // *** fw_in *** //
  primary_fw_in = this->iotime[bd_index].GetPriFwOut();
  fw_in = iotime[bd_index].GetFwOut();    // should Get 2ndAM FW_out   now-> Get from 1st TF_FWout
  if(SecDO_clear>fw_in && this->iv.s_overlap)fw_in = SecDO_clear;// + InputDelay; // overlap
  
  // *** ew_in *** //
  ew_in = iotime[bd_index].GetEwOut();
  primary_ew_in = iotime[bd_index].GetPriEwOut();
  if(SecDO_clear>ew_in && this->iv.s_overlap)ew_in = SecDO_clear;// + InputDelay; // overlap
  //if(ew_in < fw_in+InputDelay)ew_in = fw_in + InputDelay; hideyoshi

  // *** fw_out *** //
  fw_out = fw_in + this->bd_t_delay; // FIFO
  if(fw_out < iotime[bd_index].GetFwOut()+this->bd_t_delay)fw_out=iotime[bd_index].GetFwOut()+this->bd_t_delay; //hideyoshi
  primary_fw_out = primary_fw_in + this->bd_t_delay; // FIFO
  // *** ew_out *** //
  // * proc time is bottleneck * //
  vector <double> temp_io;
  vector <double> temp_priio;
  // * proc time is bottleneck * //
  temp_io.push_back(fw_out + this->iv.nroad_norw * this->bd_t_proc);
  temp_priio.push_back(primary_fw_out + this->iv.nroad_norw * this->bd_t_proc);

  // * privious board delay is bottleneck * //
  temp_io.push_back(ew_in + this->bd_t_delay);
  temp_priio.push_back(primary_ew_in + this->bd_t_delay);
  // max //
  ew_out = GetMaxVec(temp_io);
  primary_ew_out = GetMaxVec(temp_priio);

  
  // **** Set iotime of DO***** //
  this->iotime[bd_index].SetFwIn(fw_in);
  this->iotime[bd_index].SetFwOut(fw_out);
  this->iotime[bd_index].SetEwIn(ew_in);
  this->iotime[bd_index].SetEwOut(ew_out);
  this->iotime[bd_index].SetPriFwIn(primary_fw_in);
  this->iotime[bd_index].SetPriFwOut(primary_fw_out);
  this->iotime[bd_index].SetPriEwIn(primary_ew_in);
  this->iotime[bd_index].SetPriEwOut(primary_ew_out);
  this->iotime[bd_index].SetBuff(2);
  this->iotime[bd_index].SetBuff2(2);
  this->iotime[bd_index].Settemp_proc(this->bd_t_proc);
  
    // ***** debug ***** //
  if(FTKTimeSim_DO_Debug){
    cout<<"SecStageSecDataOrganizer::Calc() "<<endl;
    cout<<"**** parameter information **** "<<endl;
    cout<<"bd_index = "<<this->bd_index<<endl;
    cout<<"bd_t_in = "<<this->bd_t_in<<endl;
    cout<<"bd_t_out = "<<this->bd_t_out<<endl;
    cout<<"bd_t_proc = "<<this->bd_t_proc<<endl;
    cout<<"bd_t_delay = "<<this->bd_t_delay<<endl;
    cout<<"***** iotime of  privious event ****"<<endl;
    cout<<"fw_in = "<<this->pre_iotime[bd_index].GetFwIn()<<endl;
    cout<<"fw_out = "<<this->pre_iotime[bd_index].GetFwOut()<<endl;
    cout<<"ew_in = "<<this->pre_iotime[bd_index].GetEwIn()<<endl;
    cout<<"ew_out = "<<this->pre_iotime[bd_index].GetEwOut()<<endl;
    cout<<"**** input information **** "<<endl;
    cout<<"#of road = "<<this->iv.nroad_norw<<endl;
    cout<<"**** DO time information **** "<<endl;
    cout<<"fw_in  = "<<setw(10)<<this->iotime[bd_index].GetFwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwIn() <<endl;
    cout<<"fw_out = "<<setw(10)<<this->iotime[bd_index].GetFwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwOut()<<endl;
    cout<<"ew_in  = "<<setw(10)<<this->iotime[bd_index].GetEwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwIn() <<endl;
    cout<<"ew_out = "<<setw(10)<<this->iotime[bd_index].GetEwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwOut()<<endl;
    cout<<" -----------------------------"<<endl;

  }

}
