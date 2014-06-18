/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOTIMEBASE_HH
#define IOTIMEBASE_HH

#include <vector>

using namespace std;

class IOTimeBase {

protected:
  // unit in nano sec.
  double t_fw_in;
  double t_fw_out;
  double t_ew_in;
  double t_ew_out;
  double primary_fw_in;
  double primary_fw_out;
  double primary_ew_in;
  double primary_ew_out;
  double buff;
  double buff2;
  double buff3;
  double buff4;
  double n_killTF;
  double temp_proc;
  // characteristic values of board.
  double bd_t_in;
  double bd_t_out;
  double bd_t_proc;
  double bd_t_delay;

  // The number of board
  int bd_index;




  IOTimeBase(int bd_index, double bd_t_in, double bd_t_out, double bd_t_proc, double bd_t_delay) {
    this->bd_index = bd_index;
    this->bd_t_in = bd_t_in;
    this->bd_t_out = bd_t_out;
    this->bd_t_proc = bd_t_proc;
    this->bd_t_delay = bd_t_delay;
  }


public:
  IOTimeBase() {}
  ~IOTimeBase() {}

  // Setter
  void SetFwIn (double t_fw_in)  {this->t_fw_in = t_fw_in;}
  void SetFwOut(double t_fw_out) {this->t_fw_out = t_fw_out;}
  void SetEwIn (double t_ew_in)  {this->t_ew_in = t_ew_in;}
  void SetEwOut(double t_ew_out) {this->t_ew_out = t_ew_out;}
  void SetPriFwIn (double primary_fw_in)  {this->primary_fw_in = primary_fw_in;}
  void SetPriFwOut(double primary_fw_out) {this->primary_fw_out = primary_fw_out;}
  void SetPriEwIn (double primary_ew_in)  {this->primary_ew_in = primary_ew_in;}
  void SetPriEwOut(double primary_ew_out) {this->primary_ew_out = primary_ew_out;}
  void SetBuff(double buff) {this->buff = buff;}
  void SetBuff2(double buff2) {this->buff2 = buff2;}
  void SetBuff3(double buff3) {this->buff3 = buff3;}
  void SetBuff4(double buff4) {this->buff4 = buff4;}
  void SetNKillTF(double n_killTF) {this->n_killTF = n_killTF;} 
  void Settemp_proc(double temp_proc) {this->temp_proc = temp_proc;} 
  // Getter
  double GetFwIn()  {return this->t_fw_in;}
  double GetFwOut() {return this->t_fw_out;}
  double GetEwIn()  {return this->t_ew_in;}
  double GetEwOut() {return this->t_ew_out;}
  double GetPriFwIn()  {return this->primary_fw_in;}
  double GetPriFwOut() {return this->primary_fw_out;}
  double GetPriEwIn()  {return this->primary_ew_in;}
  double GetPriEwOut() {return this->primary_ew_out;}
  double GetBuff() {return this->buff;}
  double GetBuff2() {return this->buff2;}
  double GetBuff3() {return this->buff3;}
  double GetBuff4() {return this->buff4;}   
  double GetNKillTF() {return this->n_killTF;}
  double Gettemp_proc() {return this->temp_proc;}   

  int GetBoardNumber() {return this->bd_index;}

  double Getbd_t_in() {return this->bd_t_in;}
  double Getbd_t_out() {return this->bd_t_out;}
  double Getbd_t_proc() {return this->bd_t_proc;}
  double Getbd_t_delay() {return this->bd_t_delay;}
//   void Setbd_t_in(bd_t_in) {this->bd_t_in = bd_t_in;}
//   void Setbd_t_out(bd_t_out) {this->bd_t_out = bd_t_out;}
//   void Setbd_t_proc(bd_t_proc) {this->bd_t_proc = bd_t_proc;}
//   void Setbd_t_delay(bd_t_delay) {this->bd_t_delay = bd_t_delay;}

  // Get Max and Min // Very very very Cleep!!!!!!
  double GetMaxVec(vector<double> io){
    double temp=0; int temp_id=0;
    for(int i=0; i<(int)io.size() ; i++){
      if(temp<io[i]){
	temp=io[i];
	temp_id=i;
      }
    }
    return io[temp_id];
  }
  int GetMaxRegion(vector<double> io){
    double temp=0; int temp_id=0;
    for(int i=0; i<(int)io.size() ; i++){
      if(temp<io[i]){
	temp=io[i];
	temp_id=i;
      }
    }
    return temp_id;
  }

  double GetMinEwOut(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetEwOut()){
  	temp=io[i].GetEwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetEwOut();
  }
  double GetMaxEwOut(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetEwOut()){
  	temp=io[i].GetEwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetEwOut();
  }
  double GetMinFwOut(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetFwOut()){
  	temp=io[i].GetFwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetFwOut();
  }
  double GetMaxFwOut(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetFwOut()){
  	temp=io[i].GetFwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetFwOut();
  }


  double GetMinEwIn(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetEwIn()){
  	temp=io[i].GetEwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetEwIn();
  }
  double GetMaxEwIn(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetEwIn()){
  	temp=io[i].GetEwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetEwIn();
  }
  double GetMinFwIn(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetFwIn()){
  	temp=io[i].GetFwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetFwIn();
  }
  double GetMaxFwIn(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetFwIn()){
  	temp=io[i].GetFwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetFwIn();
  }
  


















  double GetPriMinEwOut(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetPriEwOut()){
  	temp=io[i].GetPriEwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriEwOut();
  }
  double GetPriMaxEwOut(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetPriEwOut()){
  	temp=io[i].GetPriEwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriEwOut();
  }
  double GetPriMinFwOut(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetPriFwOut()){
  	temp=io[i].GetPriFwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriFwOut();
  }
  double GetPriMaxFwOut(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetPriFwOut()){
  	temp=io[i].GetPriFwOut();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriFwOut();
  }


  double GetPriMinEwIn(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetPriEwIn()){
  	temp=io[i].GetPriEwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriEwIn();
  }
  double GetPriMaxEwIn(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetPriEwIn()){
  	temp=io[i].GetPriEwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriEwIn();
  }
  double GetPriMinFwIn(vector<IOTimeBase> io){
    double temp=99999999; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp>io[i].GetPriFwIn()){
  	temp=io[i].GetPriFwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriFwIn();
  }
  double GetPriMaxFwIn(vector<IOTimeBase> io){
    double temp=0; int temp_id=0;
    for(int i=0 ; i<(int)io.size() ; i++){
      if(temp<io[i].GetPriFwIn()){
  	temp=io[i].GetPriFwIn();
  	temp_id=i;
      }
    }
    return io[temp_id].GetPriFwIn();
  }
  






  
};

// blow class comparing IOTimeBase...
class CompFwIn
{
public:
  bool operator()( IOTimeBase& lhs, IOTimeBase& rhs ) const
  {
    return lhs.GetFwIn() < rhs.GetFwIn();
  }
};

class CompFwOut
{
public:
  bool operator()( IOTimeBase& lhs, IOTimeBase& rhs ) const
  {
    return lhs.GetFwOut() < rhs.GetFwOut();
  }
};

class CompEwIn
{
public:
  bool operator()( IOTimeBase& lhs, IOTimeBase& rhs ) const
  {
    return lhs.GetEwIn() < rhs.GetEwIn();
  }
};

class CompEwOut
{
public:
  bool operator()( IOTimeBase& lhs, IOTimeBase& rhs ) const
  {
    return lhs.GetEwOut() < rhs.GetEwOut();
  }
};

class CompIndex
{
public:
  bool operator()( IOTimeBase& lhs, IOTimeBase& rhs ) const
  {
    return lhs.GetBoardNumber() > rhs.GetBoardNumber();
  }
};
#endif
