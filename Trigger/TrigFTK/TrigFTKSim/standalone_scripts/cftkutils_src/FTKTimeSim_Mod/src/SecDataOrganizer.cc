/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SecDataOrganizer.hh"

#include <iostream>

using namespace std;

void
SecDataOrganizer::Calc()
{
  int FTKTimeSim_DO_Debug = 0;
  //int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2,-99}; // 0-2:pixcel 3-:SCT to 6 DO  // Creep! goto header....
  double fw_in, ew_in, fw_out, ew_out;
  double  primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;
  // *** Delay *** //
  //  int Delay = (this->iv.nhit_nohw_tot*this->bd_t_in);  //
  int max_road_id = -1;
  double max_road = 0;
  for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
    if(max_road<this->iv.nroad_norw_AM[j]){
      max_road=this->iv.nroad_norw_AM[j];
      max_road_id=j;
    }
  }

  double InputDelay = this->iv.nroad_norw_AM[max_road_id]*this->bd_t_in;


//   int max_stream_id = -1;
//   double max_stream = 0;
//   for(int i=0 ; i<NDATAORGANIZER ; i++){
//     if(max_stream<this->pre_iotime[i].GetEwOut()){
//       max_stream=this->pre_iotime[i].GetEwOut();
//       max_stream_id=i;
//     }
//   }
//   double PreSecDO_clear =  pre_iotime[max_stream_id].GetEwOut() - this->iv.LVL1Gap;
  double PreSecDO_clear =  GetMaxEwOut(this->pre_iotime) - this->iv.LVL1Gap;

  vector <double> temp_io;
  vector <double> temp_priio;
  temp_io.clear();
  // *** fw_in *** //
  primary_fw_in = iotime[bd_index].GetPriFwOut();
  temp_io.push_back(iotime[bd_index].GetFwOut());
  temp_io.push_back(PreSecDO_clear);
  fw_in = GetMaxVec(temp_io);
  temp_io.clear();
  // *** ew_in *** //
  primary_ew_in = iotime[bd_index].GetPriEwOut() + InputDelay;  // <- why was InnputDelay added ?
  if(primary_ew_in < primary_fw_in+InputDelay) primary_ew_in = primary_fw_in+InputDelay;
  ew_in = iotime[bd_index].GetEwOut();
  if(ew_in < fw_in+InputDelay) ew_in = fw_in+InputDelay;// input delay

  

  /// *** overlap *** //
  //  if(PreSecDO_clear>fw_in && this->iv.s_overlap)fw_in = PreSecDO_clear;// + Delay; // overlap
  //  if(PreSecDO_clear>ew_in && this->iv.s_overlap)ew_in = PreSecDO_clear; // overlap

  // *** fw_out *** //
  fw_out = fw_in + this->bd_t_delay; // FIFO
  primary_fw_out = primary_fw_in + this->bd_t_delay; // FIFO
  // *** ew_out *** //
  // * proc time is bottleneck * //

  // * proc time is bottleneck * //
  temp_io.push_back(fw_out + this->iv.nroad_norw_AM[max_road_id] * this->bd_t_proc);
  temp_priio.push_back(primary_fw_out + this->iv.nroad_norw_AM[max_road_id] * this->bd_t_proc);

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
    cout<<"SecDataOrganizer::Calc() "<<endl;
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
