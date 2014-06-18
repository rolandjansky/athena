/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataOrganizer.hh"

void
DataOrganizer::Calc()
{

  int FTKTimeSim_DO_Debug = 0;
  int layer_layout[] = {0,1, 3,10, 4,9, 5,8, 6,7, 2,-99}; // 0-2:pixcel 3-:SCT to 6 DO  // Creep! goto header.... it is allowed in 6 DO.....
  int layer_layout_7L[] = {0,0,1,2,3,5,7,9}; // 7L  // Creep! goto header.... it is allowed in 6 DO.....
  int layer_layout_8L[] = {3,4,5,6,7,8,9,10}; // 8L  // Creep! goto header.... it is allowed in 6 DO.....
  double nhit_DO=0, nhit_DO_integ=0;
  double nhit_DO_stacked=0;
  double buff=0, buff2=0;
  vector<double> nhit_DO_stacked_history;
  // for add //Set Value from IBoard to DO
  if(NDATAORGANIZER==6){
    if(layer_layout[(this->bd_index)*2+1]<0) nhit_DO=this->iv.nhit_nohw[layer_layout[(this->bd_index)*2]];
    if(layer_layout[(this->bd_index)*2+1]>0) nhit_DO=this->iv.nhit_nohw[layer_layout[(this->bd_index)*2]]+this->iv.nhit_nohw[layer_layout[(this->bd_index)*2+1]];
    if(layer_layout[(this->bd_index)*2+1]<0) nhit_DO_integ=this->iv.nhit_nohw_integ[layer_layout[(this->bd_index)*2]];
    if(layer_layout[(this->bd_index)*2+1]>0) nhit_DO_integ=this->iv.nhit_nohw_integ[layer_layout[(this->bd_index)*2]]+this->iv.nhit_nohw_integ[layer_layout[(this->bd_index)*2+1]];
    for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
      if(layer_layout[(this->bd_index)*2+1]<0) nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[layer_layout[(this->bd_index)*2]][ii]);
      if(layer_layout[(this->bd_index)*2+1]>0) nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[layer_layout[(this->bd_index)*2]][ii]+this->iv.nhit_nohw_integ_history[layer_layout[(this->bd_index)*2+1]][ii]);
    }
  }
  else if(NDATAORGANIZER==11){
    nhit_DO=this->iv.nhit_nohw[this->bd_index];
    nhit_DO_integ=this->iv.nhit_nohw_integ[this->bd_index];
    for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
      nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[this->bd_index][ii]);
    }
  }
  else if(NDATAORGANIZER==8 && this->iv.FirstStage==8){
    //    cout<<"I'm 8L"<<endl;
    nhit_DO=this->iv.nhit_nohw[layer_layout_8L[this->bd_index]];    //Set Value from IBoard to DO
    nhit_DO_integ=this->iv.nhit_nohw_integ[layer_layout_8L[this->bd_index]];
    for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
      nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[layer_layout_8L[this->bd_index]][ii]);
    }
  }
  else if(NDATAORGANIZER==8 && this->iv.FirstStage==7){
    //    cout<<"I'm 7L"<<endl;
    if(layer_layout_7L[this->bd_index]==0){ // pixel 0 is processed by two DO
      nhit_DO=this->iv.nhit_nohw[layer_layout_7L[this->bd_index]]/2;
      nhit_DO_integ=this->iv.nhit_nohw_integ[layer_layout_7L[this->bd_index]]/2;
      for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
	nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[layer_layout_7L[this->bd_index]][ii]/2);
      }
    }else{  // normal
      nhit_DO=this->iv.nhit_nohw[layer_layout_7L[this->bd_index]];
      nhit_DO_integ=this->iv.nhit_nohw_integ[layer_layout_7L[this->bd_index]];
      for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
	nhit_DO_stacked_history.push_back(this->iv.nhit_nohw_integ_history[layer_layout_7L[this->bd_index]][ii]);
      }
    }
  }


  //  for(int i=0; i<11 ; i++)cout<<this->iv.nhit_nohw_integ_history[i][0]<<"       vvvv "<<endl;
  
  nhit_DO_stacked = nhit_DO_integ-(this->iv.nentry*this->iv.LVL1Gap/this->bd_t_proc);
  if(nhit_DO_stacked<nhit_DO) nhit_DO_stacked=nhit_DO;

  double fw_in, ew_in, fw_out, ew_out;
  double primary_fw_in, primary_ew_in, primary_fw_out, primary_ew_out;

  //  cout<<"check --------------------"<<endl;
  int max_stream_id = -1;
  double max_stream = 0;
  for(int i=0 ; i<NDATAORGANIZER ; i++){
    if(max_stream<this->pre_iotime[i].GetEwOut()){
      max_stream=this->pre_iotime[i].GetEwOut();
      max_stream_id=i;
    }
    //    cout<<this->pre_iotime[i].GetEwOut()<<endl;
  }
  //  cout<<"max is = "<<max_stream<<" ("<<max_stream_id<<")"<<endl;
  //  cout<<"check ===================="<<endl;


  double PreDO_clear = GetMaxEwOut(this->pre_iotime) - this->iv.LVL1Gap; //  goto the AM
  double PrePreSecDO_clear = GetMaxEwOut(this->sppre_iotime) - 2*this->iv.LVL1Gap; //  goto the AM
  //  double PrePreSecDO_clear = 0;
  double InputDelay = (nhit_DO*this->bd_t_in);
  vector <double> temp_io;
  vector <double> temp_priio;

  // *** fw_in *** // ----------------------------------------------------------------------
  primary_fw_in = this->iotime[bd_index].GetFwOut();
  temp_io.push_back(this->iotime[bd_index].GetFwOut());
  temp_io.push_back(PreDO_clear);
  temp_io.push_back(PrePreSecDO_clear);
  fw_in = GetMaxVec(temp_io); //get max time  
  temp_io.clear();
  // *** ew_in *** // -----------------------------------------------------------------------
  primary_ew_in = this->iotime[bd_index].GetEwOut();
  temp_io.push_back(this->iotime[bd_index].GetEwOut());
  temp_io.push_back(PreDO_clear + InputDelay);
  temp_io.push_back(PrePreSecDO_clear + InputDelay);
  ew_in = GetMaxVec(temp_io);
  temp_io.clear();

  // **** AM check **** //
  //  int AM_clear = GetMaxEwOut(sppre_iotime) - this->iv.LVL1Gap; //  goto the AM

  // *** fw_out *** // -----------------------------------------------------------------------
  // *** ew_out *** // -----------------------------------------------------------------------
  // * proc time is bottleneck * //
  // * other * //
  primary_fw_out = primary_fw_in + this->bd_t_delay; 
  primary_ew_out = primary_fw_out + nhit_DO * this->bd_t_proc;
  fw_out = fw_in + this->bd_t_delay; // no overlap
  temp_io.push_back(fw_out + nhit_DO * this->bd_t_proc); //process time-> depend on #hit//
  temp_io.push_back(ew_in + this->bd_t_delay);
  ew_out = GetMaxVec(temp_io);
//   if(fw_out < AM_clear && this->iv.s_overlap){             //  goto the AM
//     fw_out=AM_clear; // overlap                            //  goto the AM
//     ew_out=fw_out + nhit_DO * this->bd_t_proc; //  overlap //  goto the AM
//   }


  // *** buffer counter *** // ==============================================================
  //  buff = (ew_out - this->bd_t_delay) / this->bd_t_proc;
  if(fw_in-this->bd_t_delay*this->iv.nentry>0){
    buff = ((fw_in-this->bd_t_delay*this->iv.nentry) / this->bd_t_proc) + nhit_DO;
  }else{
    buff = nhit_DO;
  }
  // * track fitter finish timing! * //
  double TF_clear = GetMaxEwOut(sp2pre_iotime) - this->iv.LVL1Gap;
  if(TF_clear>ew_out){
    buff2 = TF_clear / this->bd_t_proc;
  }
  buff2=0;


  double  nhit_cleared=0;
  //  cout<<"CHECKCHECKCHECKCHECKCHECKCHECKCHECKCHECKCHECK"<<endl;
  //  cout<<"now"<< fw_in <<endl;
  //  cout<<"XXX"<<this->bd_index<<" size of tfewout = "<<iv.TF_ew_out_history.size()<<endl;
  for(int ii=0; ii<(int)iv.TF_ew_out_history.size() ; ii++){
    if(iv.TF_ew_out_history[ii] - (iv.TF_ew_out_history.size()-ii)*this->iv.LVL1Gap){
      nhit_cleared=nhit_DO_stacked_history[ii];
    }
    //    cout<<setw(3)<<ii<<" "<<setw(10)<<iv.TF_ew_out_history[ii]<<"  -  "<<setw(10)<<(iv.TF_ew_out_history.size()-ii)*this->iv.LVL1Gap<<endl;
    //    cout<<nhit_DO_stacked_history[ii]<<endl;
  }
  buff2=nhit_DO_integ-nhit_cleared;
  //  cout<<"CHECKCHECKCHECKCHECKCHECKCHECKCHECKCHECKCHECK"<<endl;


  //this->sppre_iotime[bd_index]-this->iv.LVL1Gap
  //  cout<<"FFFFFFFFFFFFFFFFFFFF"<<endl;
  //Region regtemp(-99);
  //  cout<<GetMaxEwOut(this->iotime)<<endl;
//   cout<<"sp"<<endl;
//   cout<<this->sppre_iotime[0].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[1].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[2].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[3].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[4].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[5].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[6].GetEwOut()<<endl;
//   cout<<this->sppre_iotime[7].GetEwOut()<<endl;

//   cout<<"pre"<<endl;
//   cout<<this->pre_iotime[0].GetEwOut()<<endl;
//   cout<<this->pre_iotime[1].GetEwOut()<<endl;
//   cout<<this->pre_iotime[2].GetEwOut()<<endl;
//   cout<<this->pre_iotime[3].GetEwOut()<<endl;
//   cout<<this->pre_iotime[4].GetEwOut()<<endl;
//   cout<<this->pre_iotime[5].GetEwOut()<<endl;
//   cout<<this->pre_iotime[6].GetEwOut()<<endl;
//   cout<<this->pre_iotime[7].GetEwOut()<<endl;

//   cout<<"prepre"<<endl;
//   cout<<this->prepre_iotime[0].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[1].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[2].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[3].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[4].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[5].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[6].GetEwOut()<<endl;
//   cout<<this->prepre_iotime[7].GetEwOut()<<endl;
//   cout<<"FFFFFFFFFFFFFFFFFFFF"<<endl;


  // **** Set iotime of DO***** //
  this->iotime[bd_index].SetFwIn(fw_in);
  this->iotime[bd_index].SetFwOut(fw_out);
  this->iotime[bd_index].SetEwIn(ew_in);
  this->iotime[bd_index].SetEwOut(ew_out);
  this->iotime[bd_index].SetPriFwIn(primary_fw_in);
  this->iotime[bd_index].SetPriFwOut(primary_fw_out);
  this->iotime[bd_index].SetPriEwIn(primary_ew_in);
  this->iotime[bd_index].SetPriEwOut(primary_ew_out);
  this->iotime[bd_index].SetBuff(buff);
  this->iotime[bd_index].SetBuff2(buff2);
  this->iotime[bd_index].Settemp_proc(this->bd_t_proc);

  // ***** debug ***** //
  if(FTKTimeSim_DO_Debug){
    cout<<"DataOrganizer::Calc() "<<endl;
    cout<<"**** parameter information **** "<<endl;
    cout<<"bd_index = "<<this->bd_index<<endl;
    cout<<"bd_t_in = "<<this->bd_t_in<<endl;
    cout<<"bd_t_out = "<<this->bd_t_out<<endl;
    cout<<"bd_t_proc = "<<this->bd_t_proc<<endl;
    cout<<"bd_t_delay = "<<this->bd_t_delay<<endl;
    cout<<"***** iotime of  privious event ****"<<endl;
    cout<<"LVL1Gap = "<<this->iv.LVL1Gap<<endl;
    cout<<"Max stream"<<endl;
    cout<<"fw_in = "<<this->pre_iotime[max_stream_id].GetFwIn()<<endl;
    cout<<"fw_out = "<<this->pre_iotime[max_stream_id].GetFwOut()<<endl;
    cout<<"ew_in = "<<this->pre_iotime[max_stream_id].GetEwIn()<<endl;
    cout<<"ew_out = "<<this->pre_iotime[max_stream_id].GetEwOut()<<endl;
    cout<<"prepre   events SecDO ew_out = "<<this->sppre_iotime[bd_index].GetEwOut()<<endl; // should be max boad
    cout<<"previous events TF ew_out = "<<this->sp2pre_iotime[bd_index].GetEwOut()<<endl;  // should be max boad
    cout<<"**** special information **** "<<endl;
    cout<<" preDO_clear = "<<PreDO_clear<<"      PrePreSecDO_clear = "<<PrePreSecDO_clear<<endl;
    cout<<"**** input information **** "<<endl;
    
    if(NDATAORGANIZER==8 && !this->iv.split)    cout<<" I have layer "<<layer_layout_7L[(this->bd_index)]<<endl;
    if(NDATAORGANIZER==11)cout<<" I have layer "<<layer_layout[(this->bd_index)*2]<<" and "<<layer_layout[(this->bd_index)*2+1]<<endl;
    cout<<"nhit for this DO = "<<nhit_DO<<"  integral is "<<nhit_DO_integ<<" ("<<this->iv.nentry<<")"<<endl;
    cout<<"stacked nhit for this DO = "<<nhit_DO_stacked<<"   <---- miss!!!!!!"<<endl;
    cout<<"stacked nhit for this DO = "<<buff<<"   <---- ok!!!!!!"<<endl;
    cout<<"stacked nhit for this DO = "<<buff2<<"   <---- between in to TF clear signal ok!!!!!!"<<endl;
    cout<<"**** DO time information **** "<<endl;
    cout<<"fw_in  = "<<setw(10)<<this->iotime[bd_index].GetFwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwIn() <<endl;
    cout<<"fw_out = "<<setw(10)<<this->iotime[bd_index].GetFwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriFwOut()<<endl;
    cout<<"ew_in  = "<<setw(10)<<this->iotime[bd_index].GetEwIn() <<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwIn() <<endl;
    cout<<"ew_out = "<<setw(10)<<this->iotime[bd_index].GetEwOut()<<"  primary "<<setw(10)<<this->iotime[bd_index].GetPriEwOut()<<endl;
    cout<<" -----------------------------"<<endl;

  }

}
