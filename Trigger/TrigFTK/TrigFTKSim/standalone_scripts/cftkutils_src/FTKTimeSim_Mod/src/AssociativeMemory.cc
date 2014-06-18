/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AssociativeMemory.hh"

#include <iostream>

using namespace std;

void
AssociativeMemory::Calc()
{

  double spsf_AM_TSP = 1.0; // special scale factor for AM vs TSP, need check! if use other bank size


  int FTKTimeSim_AM_Debug = 0;

  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;
  //  double nhit_max=0;
  int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2,-99}; // 0-2:pixcel 3-:SCT to 6 DO  // Creep! goto header.... it is allowed in 6 DO.....
  int layer_layout_7L[] = {0,0,1,2,3,5,7,9}; // 7L  // Creep! goto header.... it is allowed in 6 DO.....
  int layer_layout_8L[] = {3,4,5,6,7,8,9,10}; // 8L  // Creep! goto header.... it is allowed in 6 DO.....



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

  vector<double> nhit_DO;
  nhit_DO.clear();
  double buff=0;
  // for add 
  if(NDATAORGANIZER==6){
    if(layer_layout[(this->bd_index)*2+1]<0) nhit_DO.push_back(this->iv.nhit_nohw[layer_layout[(this->bd_index)*2]]);
    if(layer_layout[(this->bd_index)*2+1]>0) nhit_DO.push_back(this->iv.nhit_nohw[layer_layout[(this->bd_index)*2]]+this->iv.nhit_nohw[layer_layout[(this->bd_index)*2+1]]);
  }
  else if(NDATAORGANIZER==11){
    nhit_DO.push_back(this->iv.nhit_nohw[this->bd_index]);
  }
  else if(NDATAORGANIZER==8 && this->iv.FirstStage==8){
    //    cout<<"I'm 8L"<<endl;
    nhit_DO.push_back(this->iv.nhit_nohw[layer_layout_8L[this->bd_index]]);
  }
  else if(NDATAORGANIZER==8 && this->iv.FirstStage==7){
    //    cout<<"I'm 7L"<<endl;
    if(layer_layout_7L[this->bd_index]==0){ // pixel 0 is processed by two DO
      nhit_DO.push_back(this->iv.nhit_nohw[layer_layout_7L[this->bd_index]]/2);
    }else{  // normal
      nhit_DO.push_back(this->iv.nhit_nohw[layer_layout_7L[this->bd_index]]);
    }
  }
  

  
  // *** AM clear signal *** //
  //  double AM_clear = GetMaxEwOut(pre_iotime) - this->iv.LVL1Gap;  // worst case, max AM board Ewout is used
  double PreAM_clear = GetMaxEwOut(this->pre_iotime) - this->iv.LVL1Gap;  // using same AM boad Ewout
  //  double PreAM_clear = 0;
  double PrePreAM_clear = GetMaxEwOut(this->prepre_iotime) - 2*this->iv.LVL1Gap;  // using same AM boad Ewout
  //double PrePreAM_clear = 0;
  // *** InputDelay *** //  
  //  int Delay = 0;
  //  double InputDelay = (nhit_max*this->bd_t_in); // almost pararel AM. It is ok. more real
  double InputDelay = (GetMaxVec(nhit_DO)*this->bd_t_in); // almost pararel AM. It is ok. more real  GetMaxVec(nhit_DO)
  vector <double> temp_io;
  vector <double> temp_priio;

  
  // *** fw_in *** //
  primary_fw_in = iotime[bd_index].GetPriFwOut();
  temp_io.push_back(iotime[bd_index].GetFwOut());
  temp_io.push_back(PreAM_clear);
  temp_io.push_back(PrePreAM_clear);  
  fw_in = GetMaxVec(temp_io);
  temp_io.clear();

  // *** ew_in *** //
  primary_ew_in = iotime[bd_index].GetPriEwOut();
  if(primary_ew_in < primary_fw_in+InputDelay) primary_ew_in = primary_fw_in+InputDelay;
  
  temp_io.push_back(iotime[bd_index].GetPriEwOut());
  temp_io.push_back(iotime[bd_index].GetPriEwOut()+InputDelay); //EwOut??Really?
  //temp_io.push_back(iotime[bd_index].GetPriFwOut()+InputDelay); //Hideyoshi EwOut->FwOut. This is DO's time. 
  temp_io.push_back(GetMaxEwIn(this->pre_iotime) - this->iv.LVL1Gap + InputDelay); // new 
  temp_io.push_back(PreAM_clear);  // old
  temp_io.push_back(PrePreAM_clear + InputDelay);
  ew_in = GetMaxVec(temp_io);
  temp_io.clear();
  //  if(ew_in < fw_in+InputDelay) ew_in = fw_in+InputDelay;// input delay



  //  EwIn = Max(
  //	     Max(DO_EwOut(i)),
  //	     INTIME * Max(Nin(i)) + Max(DO_FwOut(i)),
  //	     Max(PRE_EwOut(j))-LVL1Gap,
  //	     INTIME * Max(Nin(i)) + Max(PRE_PRE_EwOut(j))- 2*LVL1Gap,
  //	     ) 



  // *** fw_out *** //
  fw_out = ew_in + this->bd_t_delay;  // N.B. it is ew_in +, because event synclo here!!
  primary_fw_out = primary_ew_in + this->bd_t_delay;  // N.B. it is ew_in +, because event synclo here!!
  // *** ew_out *** //
  // * proc time is bottleneck * //
  ew_out = fw_out + this->iv.nroad_norw_AM[this->bd_index] * this->bd_t_proc*spsf_AM_TSP;
  primary_ew_out = primary_fw_out + this->iv.nroad_norw_AM[this->bd_index] * this->bd_t_proc*spsf_AM_TSP;;
  // * other * //
  // currently only upper one.....

  // **** Set iotime of DO***** //
  this->iotime[bd_index].SetFwIn(fw_in);
  this->iotime[bd_index].SetFwOut(fw_out);
  this->iotime[bd_index].SetEwIn(ew_in);
  this->iotime[bd_index].SetEwOut(ew_out);
  //  iotime[bd_index].SetBuff(this->iv.nhit_nohw_tot);
  iotime[bd_index].SetBuff(0);
  this->iotime[bd_index].SetPriFwIn(primary_fw_in);
  this->iotime[bd_index].SetPriFwOut(primary_fw_out);
  this->iotime[bd_index].SetPriEwIn(primary_ew_in);
  this->iotime[bd_index].SetPriEwOut(primary_ew_out);
  this->iotime[bd_index].SetBuff(buff);
  this->iotime[bd_index].Settemp_proc(this->bd_t_proc);
  
  // ***** debug ***** //
  if(FTKTimeSim_AM_Debug){
    cout<<"AssociativeMemory::Calc() "<<endl;
    cout<<"**** parameter information **** "<<endl;
    cout<<"bd_index = "<<this->bd_index<<endl;
    cout<<"bd_t_in = "<<this->bd_t_in<<endl;
    cout<<"bd_t_out = "<<this->bd_t_out<<endl;
    cout<<"bd_t_proc = "<<this->bd_t_proc<<endl;
    cout<<"bd_t_delay = "<<this->bd_t_delay<<endl;
    cout<<"***** iotime of  privious event ****"<<endl;
    cout<<"LVL1Gap = "<<this->iv.LVL1Gap<<endl;
    cout<<"previous PreAM_clear = "<<PreAM_clear<<endl;
    cout<<"fw_in = "<<this->pre_iotime[bd_index].GetFwIn()<<endl;
    cout<<"fw_out = "<<this->pre_iotime[bd_index].GetFwOut()<<endl;
    cout<<"ew_in = "<<this->pre_iotime[bd_index].GetEwIn()<<endl;
    cout<<"ew_out = "<<this->pre_iotime[bd_index].GetEwOut()<<endl;
    cout<<"**** input information **** "<<endl;
    cout<<"#of road = "<<this->iv.nroad_norw_AM[this->bd_index]<<endl;
    cout<<"#of this for delay = "<<this->iv.nhit_nohw_tot<<endl;
    cout<<"InputDelay for max input rate of hits = "<<InputDelay<<endl;
    cout<<"AM wrote    = "<<GetMaxEwIn(this->pre_iotime) - this->iv.LVL1Gap + InputDelay<<endl; // new 
    cout<<"PreAM_clear = "<<GetMaxEwOut(this->pre_iotime) - this->iv.LVL1Gap;  //

    cout<<"**** AM time information **** "<<endl;
    cout<<"fw_in  = "<<setw(10)<<this->iotime[bd_index].GetFwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwIn() <<endl;
    cout<<"fw_out = "<<setw(10)<<this->iotime[bd_index].GetFwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwOut()<<endl;
    cout<<"ew_in  = "<<setw(10)<<this->iotime[bd_index].GetEwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwIn() <<endl; //Pri is DO's time?
    cout<<"ew_out = "<<setw(10)<<this->iotime[bd_index].GetEwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwOut()<<endl;
    cout<<" -----------------------------"<<endl;
  }
}
