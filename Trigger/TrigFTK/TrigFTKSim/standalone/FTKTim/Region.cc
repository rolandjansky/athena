/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Region.hh"
//#include "../headerRegion.hh"



Region::Region(int region_index)
  : IOTimeBase(bd_index, RG_T_IN, RG_T_OUT, RG_T_PROC, RG_T_DELAY)  //Instance set initialize
{
  this->region_index = region_index;
  std::vector<IOTimeBase> temp;
  IOTimeBase time;
  time.SetFwIn (-1);
  time.SetFwOut(-1);
  time.SetEwIn (-1);
  time.SetEwOut(-1);
  time.SetPriFwIn (-1);
  time.SetPriFwOut(-1);
  time.SetPriEwIn (-1);
  time.SetPriEwOut(-1);
  time.SetBuff(-1);
  time.SetBuff2(-1);
  //for 2nd stage
  std::vector<IOTimeBase> temp2;
  IOTimeBase time2; 
  time2.SetFwIn (0);
  time2.SetFwOut(0);
  time2.SetEwIn (0);
  time2.SetEwOut(0);
  time2.SetPriFwIn (0);
  time2.SetPriFwOut(0);
  time2.SetPriEwIn (0);
  time2.SetPriEwOut(0);
  time2.SetBuff(0);
  time2.SetBuff2(0);
  
  for(int idx=0;idx<NDATAFORMATTER;idx++) {
    DataFormatter sDF(idx);
    cDF.push_back(sDF);          //cDF is decleared at Region.hh
    temp.push_back(time);  
    cDF[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V // init
    cDF[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V // init
    cDF[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V // init
    cDF[idx].SetNextBoardPre(temp);
    cDF[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();

  
  for(int idx=0;idx<NDATAORGANIZER;idx++) {
    DataOrganizer sDO(idx);
    cDO.push_back(sDO);     //cDO is IBoardBase's Member Func.
    temp.push_back(time);  
    cDO[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cDO[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cDO[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cDO[idx].SetNextBoardPre(temp);
    cDO[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();

  for(int idx=0;idx<NASSOCIATIVEMEMORY;idx++) {
    AssociativeMemory sAM(idx);
    cAM.push_back(sAM);
    temp.push_back(time);  cAM[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cAM[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cAM[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cAM[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cAM[idx].SetNextBoardPre(temp);
    cAM[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();
  
  for(int idx=0;idx<NDATAORGANIZER;idx++) {
    SecDataOrganizer sSecDO(idx);
    cSecDO.push_back(sSecDO);
    temp.push_back(time);  
    cSecDO[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecDO[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecDO[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecDO[idx].SetNextBoardPre(temp);
    cSecDO[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();
    
  for(int idx=0;idx<NTRACKFITTER;idx++) {
    TrackFitter sTF(idx);
    cTF.push_back(sTF);
    temp.push_back(time);  
    cTF[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cTF[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cTF[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cTF[idx].SetNextBoardPre(temp);
    cTF[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();

  for(int idx=0;idx<NSECSTAGEDATAORGANIZER;idx++) {
    SecStageSecDataOrganizer sSecStageSecDO(idx);
    cSecStageSecDO.push_back(sSecStageSecDO);
    temp.push_back(time);  
    cSecStageSecDO[idx].Set(temp); //
    cSecStageSecDO[idx].SetPre(temp); //
    cSecStageSecDO[idx].SetPrePre(temp); // 
    cSecStageSecDO[idx].SetNextBoardPre(temp);
    cSecStageSecDO[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();
    
  for(int idx=0;idx<NSECSTAGETRACKFITTER;idx++) {
    SecStageTrackFitter sSecStageTF(idx);
    cSecStageTF.push_back(sSecStageTF);
    temp.push_back(time);  
    cSecStageTF[idx].Set(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecStageTF[idx].SetPre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecStageTF[idx].SetPrePre(temp); // Dont use cDF[n].Get(n+1) Seg V //init
    cSecStageTF[idx].SetNextBoardPre(temp);
    cSecStageTF[idx].SetNextBoardPrePre(temp);
  }
  temp.clear();

  //  make_heap(cTF.begin(), cTF.end(), CompEwOut()); // currently not useing
}

void
//Region::Calc(int s_split)
Region::Calc()
{
  std::vector<IOTimeBase> temp;
  IOTimeBase time;
  double temp_MinFwOut;
  double temp_MaxEwOut;
  double temp_MinFwIn;
  double temp_MaxEwIn;
  double temp_PriMinFwOut;
  double temp_PriMaxEwOut;
  double temp_PriMinFwIn;
  double temp_PriMaxEwIn;
  // for 2nd stage hideyoshi //
  std::vector<IOTimeBase> temp2;
  IOTimeBase time2;
  double temp2_MinFwOut;
  double temp2_MaxEwOut;
  double temp2_MinFwIn;
  double temp2_MaxEwIn;
  double temp2_PriMinFwOut;
  double temp2_PriMaxEwOut;
  double temp2_PriMinFwIn;
  double temp2_PriMaxEwIn;


  // **** set initial time parameter **** //
  for(int idx=0 ; idx<NDATAORGANIZER ; idx++){
    //time.SetFwIn(-999+idx); // for test
    //time.SetFwOut(-888+idx);// for test
    //time.SetEwIn(-777+idx);// for test
    //time.SetEwOut(-666+idx);// for test
    time.SetFwIn(1000.); // all 1 micro sec for the delay of DF... 
    time.SetFwOut(1000.);
    time.SetEwIn(1000.);
    time.SetEwOut(1000.);
    time.SetPriFwIn(1000.);
    time.SetPriFwOut(1000.);
    time.SetPriEwIn(1000.);
    time.SetPriEwOut(1000.);
    temp.push_back(time);
  }
  
  for(int idx=0 ; idx<NSECSTAGEDATAORGANIZER ; idx++){
    time2.SetFwIn(0);
    time2.SetFwOut(0);
    time2.SetEwIn(0);
    time2.SetEwOut(0);
    time2.SetPriFwIn(0);
    time2.SetPriFwOut(0);
    time2.SetPriEwIn(0);
    time2.SetPriEwOut(0);
    temp2.push_back(time2);
  }

  
  //  cout<<"check Mar3 00"<<endl;
  // *
  // ***** DataFormatter ***** //
 // *
  //    for(int idx=0;idx<NDATAFORMATTER;idx++) {
  //      cDF[idx].SetInputValue(this->iv);
  //      cDF[idx].Calc();
  //    }
  //    sort_heap(cDF.begin(), cDF.end(), CompEwOut());
  //    temp.clear();
  //    copy(cDF.begin(), cDF.end(), back_inserter(temp));

  // *** set the temp privious event's full borad iotime for buffer study *** //
  vector<IOTimeBase> temp_DO;
  for(int i=0 ; i<NDATAORGANIZER; i++) temp_DO.push_back(cDO[i].Get(i)); //Get()is in IBoardBase.hh//
  vector<IOTimeBase> temp_AM;
  for(int i=0 ; i<NASSOCIATIVEMEMORY; i++) temp_AM.push_back(cAM[i].Get(i));
  vector<IOTimeBase> temp_SecDO;
  for(int i=0 ; i<NDATAORGANIZER; i++) temp_SecDO.push_back(cSecDO[i].Get(i));
  vector<IOTimeBase> temp_TF;
  for(int i=0 ; i<NTRACKFITTER; i++) temp_TF.push_back(cTF[i].Get(i));
  vector<IOTimeBase> temp_SecStageSecDO;
  for(int i=0 ; i<NSECSTAGEDATAORGANIZER; i++) temp_SecStageSecDO.push_back(cSecStageSecDO[i].Get(i));
  vector<IOTimeBase> temp_SecStageTF;
  for(int i=0 ; i<NSECSTAGETRACKFITTER; i++) temp_SecStageTF.push_back(cSecStageTF[i].Get(i));
  // *** set the temp privious event's full borad iotime for buffer study *** //
  vector<IOTimeBase> prepre_DO;
  for(int i=0 ; i<NDATAORGANIZER; i++) prepre_DO.push_back(cDO[i].GetPre(i));
  vector<IOTimeBase> prepre_AM;
  for(int i=0 ; i<NASSOCIATIVEMEMORY; i++) prepre_AM.push_back(cAM[i].GetPre(i));
  vector<IOTimeBase> prepre_SecDO;
  for(int i=0 ; i<NDATAORGANIZER; i++) prepre_SecDO.push_back(cSecDO[i].GetPre(i));
  vector<IOTimeBase> prepre_TF;
  for(int i=0 ; i<NTRACKFITTER; i++) prepre_TF.push_back(cTF[i].GetPre(i));
  vector<IOTimeBase> prepre_SecStageSecDO;
  for(int i=0 ; i<NSECSTAGEDATAORGANIZER; i++) prepre_SecStageSecDO.push_back(cSecStageSecDO[i].GetPre(i));
  vector<IOTimeBase> prepre_SecStageTF;
  for(int i=0 ; i<NSECSTAGETRACKFITTER; i++) prepre_SecStageTF.push_back(cSecStageTF[i].GetPre(i));
  //  cout<<"check Mar3 01"<<endl;  
  // *
  // ***** DataOrganizer ***** //
  // *
  if(FTKTimeSim_Debug>3) cout<<"llllllllllll  DataOrganizer llllllllllll"<<endl;
  for(int idx=0;idx<NDATAORGANIZER;idx++) {
    if(FTKTimeSim_Debug>3)
      cout<<"calc Data Organizer!  --  DATAORGANIZER# is "<<idx<<endl;
    cDO[idx].SetInputValue(this->iv);  // Is it need? Sure! Nn??????    //cDO is IBoardBase's Member Func.
    //    cDO[idx].SetPre(cDO[idx].GetV()); // iotime of previous events
    cDO[idx].SetPre(temp_DO); // iotime of previous events
    cDO[idx].SetPrePre(prepre_DO); // iotime of previous events
    cDO[idx].SetNextBoardPre(temp_AM); // iotime of previous events
    cDO[idx].SetNextBoardPrePre(prepre_AM); // iotime of previous events
    cDO[idx].SetSPPre(prepre_SecDO); // for buffer size study, iotime of privious TF ew out!!!
    cDO[idx].SetSP2Pre(temp_TF); // for buffer size study, iotime of privious TF ew out!!!
    cDO[idx].Set(temp);
    cDO[idx].Calc();
  }
  // ** sort **//
  //  sort_heap(cDO.begin(), cDO.end(), CompEwOut());
  temp.clear();
  //  copy(cDO[0].iotime.begin(), cDO[0].iotime.end(), back_inserter(temp));
  for(int i=0 ; i<NDATAORGANIZER ; i++) temp.push_back(cDO[i].Get(i));
  temp_MinFwOut=GetMinFwOut(temp);
  temp_MaxEwOut=GetMaxEwOut(temp);
  temp_MinFwIn=GetMinFwIn(temp);
  temp_MaxEwIn=GetMaxEwIn(temp);
  temp_PriMinFwOut=GetPriMinFwOut(temp);
  temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  temp_PriMinFwIn= GetPriMinFwIn(temp);
  temp_PriMaxEwIn= GetPriMaxEwIn(temp);

  // ** conection ** // for if each AM have delay....
  temp.clear();
  for(int idx=0 ; idx<NASSOCIATIVEMEMORY ; idx++){
    time.SetFwIn(0);
    time.SetFwOut(temp_MinFwOut);
    time.SetEwIn(0);
    time.SetEwOut(temp_MaxEwOut);
    time.SetPriFwIn(0);
    time.SetPriFwOut(temp_PriMinFwOut);
    time.SetPriEwIn(0);
    time.SetPriEwOut(temp_PriMaxEwOut);
    temp.push_back(time);
  }
  // ** Debug ** //
  if(FTKTimeSim_Debug>3){
    cout<<"============ iotime conection DO to AM ============="<<endl;
    for(int i=0 ; i<NDATAORGANIZER ; i++){
      cout<<"DO "<<i<<"  fw_out = "<<cDO[i].Get(i).GetFwOut()<<"   ew_out = "<<cDO[i].Get(i).GetEwOut()<<endl;
    }
    cout<<" DO (for AM in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
    cout<<"===================================================="<<endl;
  }

  // * 
  // ***** AssociativeMemory ***** //
  // *
  if(FTKTimeSim_Debug>3) cout<<"llllllllllll  AssociativeMemory llllllllllll"<<endl;
  for(int idx=0;idx<NASSOCIATIVEMEMORY;idx++) {
    if(FTKTimeSim_Debug>3)
      cout<<"calc AssociativeMemory!  --  ASSOCIATIVEMEMORY# is "<<idx<<endl;
    cAM[idx].SetInputValue(this->iv);
    cAM[idx].SetPre(temp_AM); // iotime of previous events
    cAM[idx].SetPrePre(prepre_AM); // iotime of previous events
    cAM[idx].SetNextBoardPre(temp_SecDO); // iotime of previous events
    cAM[idx].SetNextBoardPrePre(prepre_SecDO); // iotime of previous events
    cAM[idx].Set(temp);
    cAM[idx].Calc();
  }
  // ** sort **//
  temp.clear();
  for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++) temp.push_back(cAM[i].Get(i));
  temp_MinFwOut=GetMinFwOut(temp);
  temp_MaxEwOut=GetMaxEwOut(temp);
  temp_MinFwIn=GetMinFwIn(temp);
  temp_MaxEwIn=GetMaxEwIn(temp);
  temp_PriMinFwOut=GetPriMinFwOut(temp);
  temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  temp_PriMinFwIn= GetPriMinFwIn(temp);
  temp_PriMaxEwIn= GetPriMaxEwIn(temp);


  // ** conection ** //
  temp.clear();
  for(int idx=0 ; idx<NDATAORGANIZER ; idx++){
    time.SetFwIn(0);
    time.SetFwOut(temp_MinFwOut);
    time.SetEwIn(0);
    time.SetEwOut(temp_MaxEwOut);
    time.SetPriFwIn(0);
    time.SetPriFwOut(temp_PriMinFwOut);
    time.SetPriEwIn(0);
    time.SetPriEwOut(temp_PriMaxEwOut);
    temp.push_back(time);
  }
  // ** Debug ** //
  if(FTKTimeSim_Debug>3){
    cout<<"============ iotime conection AM to DO2 ============="<<endl;
    for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++){
      cout<<"AM "<<i<<"  fw_out = "<<cAM[i].Get(i).GetFwOut()<<"   ew_out = "<<cAM[i].Get(i).GetEwOut()<<endl;
    }
    cout<<" AM (for DO2 in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
    cout<<"===================================================="<<endl;
  }

  // * 
  // ***** Second DataOrganizer ***** //
  // *
  if(FTKTimeSim_Debug>3) cout<<"llllllllllll  Second DataOrganizer llllllllllll"<<endl;
  for(int idx=0;idx<NDATAORGANIZER;idx++) {
    if(FTKTimeSim_Debug>3)
      cout<<"calc Second DataOrganizer!  --  SecDATAORGANIZER# is "<<idx<<endl;
    cSecDO[idx].SetInputValue(this->iv);
    cSecDO[idx].SetPre(temp_SecDO); // iotime of previous events
    cSecDO[idx].SetPrePre(prepre_SecDO); // iotime of previous events
    cSecDO[idx].SetNextBoardPre(temp_TF); // iotime of previous events
    cSecDO[idx].SetNextBoardPrePre(prepre_TF); // iotime of previous events
    cSecDO[idx].Set(temp);
    cSecDO[idx].Calc();
  }
  // ** sort **//
  temp.clear();
  for(int i=0 ; i<NDATAORGANIZER ; i++) temp.push_back(cSecDO[i].Get(i));
  temp_MinFwOut=GetMinFwOut(temp);
  temp_MaxEwOut=GetMaxEwOut(temp);
  temp_MinFwIn=GetMinFwIn(temp);
  temp_MaxEwIn=GetMaxEwIn(temp);
  temp_PriMinFwOut=GetPriMinFwOut(temp);
  temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  temp_PriMinFwIn= GetPriMinFwIn(temp);
  temp_PriMaxEwIn= GetPriMaxEwIn(temp);

  // ** conection ** //
  temp.clear();
  for(int idx=0 ; idx<NTRACKFITTER ; idx++){
    time.SetFwIn(0);
    time.SetFwOut(temp_MinFwOut);
    time.SetEwIn(0);
    time.SetEwOut(temp_MaxEwOut);
    time.SetPriFwIn(0);
    time.SetPriFwOut(temp_PriMinFwOut);
    time.SetPriEwIn(0);
    time.SetPriEwOut(temp_PriMaxEwOut);
    temp.push_back(time);
  }
  // ** Debug ** //
  if(FTKTimeSim_Debug>3){
    cout<<"============ iotime conection SecDO to TF ============="<<endl;
    for(int i=0 ; i<NDATAORGANIZER ; i++){
      cout<<"SecDO "<<i<<"  fw_out = "<<cSecDO[i].Get(i).GetFwOut()<<"   ew_out = "<<cSecDO[i].Get(i).GetEwOut()<<endl;
    }
    cout<<" SecDO (for TF in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
    cout<<"===================================================="<<endl;
  }

  // * 
  // ***** TrackFitter ***** //
  // *
  for(int idx=0;idx<NTRACKFITTER;idx++) {
    cTF[idx].SetInputValue(this->iv);
    cTF[idx].SetPre(temp_TF); // iotime of previous events
    cTF[idx].SetPrePre(prepre_TF); // iotime of previous events
    cTF[idx].SetNextBoardPre(temp_SecStageTF); // iotime of previous events // now only think about chi2 sec stage
    cTF[idx].SetNextBoardPrePre(prepre_SecStageTF); // iotime of previous events
    cTF[idx].Set(temp);
    cTF[idx].Calc();
  }
  // ** sort **//
  temp.clear();
  for(int i=0 ; i<NTRACKFITTER ; i++) temp.push_back(cTF[i].Get(i));
  temp_MinFwOut=GetMinFwOut(temp);
  temp_MaxEwOut=GetMaxEwOut(temp);
  temp_MinFwIn=GetMinFwIn(temp);
  temp_MaxEwIn=GetMaxEwIn(temp);
  temp_PriMinFwOut=GetPriMinFwOut(temp);
  temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  temp_PriMinFwIn= GetPriMinFwIn(temp);
  temp_PriMaxEwIn= GetPriMaxEwIn(temp);

  // ***
  // ******** Second Stage Chi2 for 3 SCT layer  *******
  // ***
  if(this->iv.Chi2SecondStage){
    if(FTKTimeSim_Debug>4)    cout<<"HHHHHHHHHHH Chi2 SecondStage for 3 SCT layer HHHHHHHHHHH"<<endl;
    // ** conection 1st to 2nd ** //
    // origine //
    temp.clear();
    for(int idx=0 ; idx<NSECSTAGETRACKFITTER ; idx++){
      time.SetFwIn(0);
      time.SetFwOut(temp_MinFwOut);
      time.SetEwIn(0);
      time.SetEwOut(temp_MaxEwOut);
      time.SetPriFwIn(0);
      time.SetPriFwOut(temp_PriMinFwOut);
      time.SetPriEwIn(0);
      time.SetPriEwOut(temp_PriMaxEwOut);
      temp.push_back(time);
    }

    // ** Debug ** //
    if(FTKTimeSim_Debug>3){
      cout<<"============ iotime conection TF to SecStage  ============="<<endl;
      for(int i=0 ; i<NSECSTAGETRACKFITTER ; i++){
	cout<<"TF "<<i<<"  fw_out = "<<cTF[i].Get(i).GetFwOut()<<"   ew_out = "<<cTF[i].Get(i).GetEwOut()<<endl;
      }
      cout<<" TF (for Second Stage in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
      cout<<"===================================================="<<endl;
    }
    // * 
    // ***** Second Stage ***** //
    // *
    for(int idx=0;idx<NSECSTAGETRACKFITTER;idx++) {
      cSecStageTF[idx].SetInputValue(this->iv);
      cSecStageTF[idx].SetPre(temp_SecStageTF); // iotime of previous events
      cSecStageTF[idx].SetPrePre(prepre_SecStageTF); // iotime of previous events
      cSecStageTF[idx].SetNextBoardPre(temp_SecStageTF); // iotime of previous events
      cSecStageTF[idx].SetNextBoardPrePre(prepre_SecStageTF); // iotime of previous events
      cSecStageTF[idx].Set(temp);
      cSecStageTF[idx].Calc();
    }
    // ** sort **//
    temp.clear();
    for(int i=0 ; i<NSECSTAGETRACKFITTER ; i++) temp.push_back(cSecStageTF[i].Get(i));
    temp_MinFwOut=GetMinFwOut(temp);
    temp_MaxEwOut=GetMaxEwOut(temp);
    temp_MinFwIn=GetMinFwIn(temp);
    temp_MaxEwIn=GetMaxEwIn(temp);
    temp_PriMinFwOut=GetPriMinFwOut(temp);
    temp_PriMaxEwOut=GetPriMaxEwOut(temp);
    temp_PriMinFwIn= GetPriMinFwIn(temp);
    temp_PriMaxEwIn= GetPriMaxEwIn(temp);



  }// end Chi2 Second Stage



  // ***
  // ******** Second Stage (4L Picel 3 + SCT fit info 1  , etc ) *******
  // ***

  // if(this->iv.split){
  //   cout<<"SSSSSSSS Split SSSSSSSS"<<endl;
  //   // ** conection 1st to 2nd ** //
  //   // origine //
  //   temp.clear();
  //   for(int idx=0 ; idx<NDATAORGANIZER ; idx++){
  //     time.SetFwIn(0);
  //     time.SetFwOut(temp_MinFwOut);
  //     time.SetEwIn(0);
  //     time.SetEwOut(temp_MaxEwOut);
  //     time.SetPriFwIn(0);
  //     time.SetPriFwOut(temp_PriMinFwOut);
  //     time.SetPriEwIn(0);
  //     time.SetPriEwOut(temp_PriMaxEwOut);
  //     temp.push_back(time);
  //   }

  //   // ** Debug ** //
  //   if(FTKTimeSim_Debug>3){
  //     cout<<"============ iotime conection TF to SecStage SecDO ============="<<endl;
  //     for(int i=0 ; i<NTRACKFITTER ; i++){
  // 	cout<<"TF "<<i<<"  fw_out = "<<cTF[i].Get(i).GetFwOut()<<"   ew_out = "<<cTF[i].Get(i).GetEwOut()<<endl;
  //     }
  //     cout<<" TF (for Second Stage in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
  //     cout<<"===================================================="<<endl;
  //   }
    
  //   // *
  //   // **** SecStage Data Organaizer **** //
  //   // *

  //   // *
  //   // **** SecStage Associative Memory **** //
  //   // *
    
    
  //   // * 
  //   // ***** SecStage Second DataOrganizer ***** //
  //   // *
  //   if(FTKTimeSim_Debug>3) cout<<"llllllllllll  SecStageSecond DataOrganizer llllllllllll"<<endl;
  //   for(int idx=0;idx<NSECSTAGEDATAORGANIZER;idx++){
  //     if(FTKTimeSim_Debug>3)
  // 	cout<<"calc SecStageSecond DataOrganizer!  --  SecStageSecDATAORGANIZER# is "<<idx<<endl;
  //     cSecStageSecDO[idx].SetInputValue(this->iv);
  //     cSecStageSecDO[idx].SetPre(temp_SecStageSecDO); // iotime of previous events
  //     cSecStageSecDO[idx].SetPrePre(prepre_SecStageSecDO); // iotime of previous events
  //     cSecStageSecDO[idx].Set(temp); //origine
  //     //cSecStageSecDO[idx].Set(temp2); //hideyoshi
  //     cSecStageSecDO[idx].Calc();
  //   }
  //   // ** sort **//
  //   temp.clear();
  //   for(int i=0 ; i<NSECSTAGEDATAORGANIZER ; i++) temp.push_back(cSecStageSecDO[i].Get(i));
  //   temp_MinFwOut=GetMinFwOut(temp);
  //   temp_MaxEwOut=GetMaxEwOut(temp);
  //   temp_MinFwIn=GetMinFwIn(temp);
  //   temp_MaxEwIn=GetMaxEwIn(temp);
  //   temp_PriMinFwOut=GetPriMinFwOut(temp);
  //   temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  //   temp_PriMinFwIn= GetPriMinFwIn(temp);
  //   temp_PriMaxEwIn= GetPriMaxEwIn(temp);
    



 
    
  //   // ** conection  AM to TF ** //  
  //   // need to compare time of 2nd AM-DO with time of 1st TF //
    
    
  //   temp.clear();
  //   for(int idx=0 ; idx<NSECSTAGETRACKFITTER ; idx++){
  //     time.SetFwIn(0);
  //     time.SetFwOut(temp_MinFwOut);
  //     time.SetEwIn(0);
  //     time.SetEwOut(temp_MaxEwOut);
  //     time.SetPriFwIn(0);
  //     time.SetPriFwOut(temp_PriMinFwOut);
  //     time.SetPriEwIn(0);
  //     time.SetPriEwOut(temp_PriMaxEwOut);
  //     temp.push_back(time);
  //   }
  //   // ** Debug ** //
  //   if(FTKTimeSim_Debug>3){
  //     cout<<"============ iotime conection SecStageSecStage SecStageSecDO to TF ============="<<endl;
  //     for(int i=0 ; i<NSECSTAGEDATAORGANIZER ; i++){
  // 	cout<<"SecStageSecDO "<<i<<"  fw_out = "<<cSecStageSecDO[i].Get(i).GetFwOut()<<"   ew_out = "<<cSecStageSecDO[i].Get(i).GetEwOut()<<endl;
  //     }
  //     cout<<" SecStageSecDO (for TF in) : MinFwOut = "<<temp_MinFwOut<<",   MaxEwOut = "<<temp_MaxEwOut<<endl;
  //     cout<<"===================================================="<<endl;
  //   }
    
  //   // * 
  //   // ***** Second Stage TrackFitter ***** //
  //   // *
  //   for(int idx=0;idx<NSECSTAGETRACKFITTER;idx++) {
  //     cSecStageTF[idx].SetInputValue(this->iv);
  //     cSecStageTF[idx].SetPre(temp_SecStageTF); // iotime of previous events
  //     cSecStageTF[idx].SetPrePre(prepre_SecStageTF); // iotime of previous events
  //     cSecStageTF[idx].Set(temp);
  //     cSecStageTF[idx].Calc();
  //   }
  //   // ** sort **//
  //   temp.clear();
  //   for(int i=0 ; i<NTRACKFITTER ; i++) temp.push_back(cSecStageTF[i].Get(i));
  //   temp_MinFwOut=GetMinFwOut(temp);
  //   temp_MaxEwOut=GetMaxEwOut(temp);
  //   temp_MinFwIn=GetMinFwIn(temp);
  //   temp_MaxEwIn=GetMaxEwIn(temp);
  //   temp_PriMinFwOut=GetPriMinFwOut(temp);
  //   temp_PriMaxEwOut=GetPriMaxEwOut(temp);
  //   temp_PriMinFwIn= GetPriMinFwIn(temp);
  //   temp_PriMaxEwIn= GetPriMaxEwIn(temp);
  // }
  
  //cout<<temp_MaxEwOut<<"   checkkkkkkkkkkkkkkkkkkkkkkkkkkkkk "<<endl;
  this->t_fw_out=temp_MinFwOut;
  this->t_ew_out=temp_MaxEwOut;
  this->t_fw_in=temp_MinFwIn;
  this->t_ew_in=temp_MaxEwIn;
  this->primary_fw_out=temp_PriMinFwOut;
  this->primary_ew_out=temp_PriMaxEwOut;
  this->primary_fw_in= temp_PriMinFwIn;
  this->primary_ew_in= temp_PriMaxEwIn;

  
}
