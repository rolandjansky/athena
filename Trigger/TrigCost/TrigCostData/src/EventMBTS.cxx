/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

// Local
#include "TrigCostData/EventMBTS.h"

using namespace std;

//----------------------------------------------------------------------------------
Anp::EventMBTS::EventMBTS() : fChargeVec(32, 0.0),
                              fTimeVec(32, 0.0) 
{  
}

//----------------------------------------------------------------------------------
int Anp::EventMBTS::GetIndex(int type, int ch, int mod) const
{

  if(std::abs(type) != 1) { 
    cout << "WARNING EventMBTS::GetIndex - Failed id check" << endl;
    cout << type << endl;
    return 0;
  }
  if(ch < 0 || ch > 1) { 
    cout << "WARNING EventMBTS::GetIndex - Failed id check" << endl;
    cout << ch << endl;
    return 0;
  }
  if(mod < 0  || mod > 7) {
    cout << "WARNING EventMBTS::GetIndex - Failed id check" << endl;
    cout << mod << endl;
    return 0;
  }
  // if type == 0 && ch == 0, index = 0-7
  // if type == 0 && ch == 1, index = 8-15
  // if type == 1 && ch == 0 , index = 16-23
  // if type == 1 && ch == 1 , index = 24-31
  
  //int index = type*16 + ch*8 + mod;
  int index = (type + ch + 1)*8 + mod;
  
  if(index < 0 || index > 31) {
    cout << "WARNING EventMBTS::GetIndex - Computed Index is out of range" << endl;
    return 0;
  }
  
  return index;
} 

//----------------------------------------------------------------------------------
int Anp::EventMBTS::GetType(int index) const
{

  if(index < 0 || index > 31) {
    cout << "WARNING EventMBTS::GetType - Index is out of range" << endl;
    return 0;
  }
  return index < 16 ? -1 : 1;

}

//----------------------------------------------------------------------------------
int Anp::EventMBTS::GetChannel(int index) const
{
  if(index < 0 || index > 31) {
    cout << "WARNING EventMBTS::GetChannel - Index is out of range" << endl;
    return 0;
  }

  return (index/8)%2;  
}

//----------------------------------------------------------------------------------
int Anp::EventMBTS::GetModule(int index) const
{
  if(index < 0 || index > 31) {
    cout << "WARNING EventMBTS::GetModule - Index is out of range" << endl;
    return 0;
  }

  return index%8;
}

//----------------------------------------------------------------------------------
void Anp::EventMBTS::SetCharge(int type, int ch, int mod, float charge)
{
  fChargeVec[GetIndex(type, ch, mod)] = charge;
}

//----------------------------------------------------------------------------------
void Anp::EventMBTS::SetTime(int type, int ch, int mod, float time)
{
  fTimeVec[GetIndex(type, ch, mod)] = time;
} 

//----------------------------------------------------------------------------------
float Anp::EventMBTS::GetCharge(int type, int ch, int mod) const
{
  if(ch == -1 && mod == -1) {
    
    float accum_charge = 0.0;
    for(int i = 0; i < 2; ++i) {
      for(int j = 0; j < 8; ++j) { 
        accum_charge += fChargeVec[GetIndex(type, i, j)];
      }
    }
    return accum_charge;
  }
  else if(mod == -1) {
    float accum_charge = 0.0;
    for(unsigned int i = 0; i < 8; ++i) {
      accum_charge += fChargeVec[GetIndex(type, ch, i)];
    }
    return accum_charge;
  }
  else {
    return fChargeVec[GetIndex(type, ch, mod)];
  }
}

//----------------------------------------------------------------------------------
float Anp::EventMBTS::GetChargeCut(float cut, int type, int ch, int mod) const
{
  if(ch == -1 && mod == -1) {
    
    float accum_charge = 0.0;
    for(int i = 0; i < 2; ++i) {
      for(int j = 0; j < 8; ++j) { 
        float charge = fChargeVec[GetIndex(type, i, j)]; 
        if(charge >= cut) { 
          accum_charge += charge;
        }
      }
    }
    return accum_charge;
  }
  else if(mod == -1) {
    float accum_charge = 0.0;
    for(unsigned int i = 0; i < 8; ++i) {
      float charge = fChargeVec[GetIndex(type, ch, i)];
      if(charge >= cut) { 
        accum_charge += charge;
      }
    }
    return accum_charge;
  }
  else {
    if(fChargeVec[GetIndex(type, ch, mod)] >= cut) {
      return fChargeVec[GetIndex(type, ch, mod)];
    }
    else {
      return 0.0;
    }
  }
}

//----------------------------------------------------------------------------------
float Anp::EventMBTS::GetTime(int type, int ch, int mod) const
{

  if(ch == -1 && mod == -1) {
    
    float accum_time = 0.0;
    int count = 0;
    for(int i = 0; i < 2; ++i) {
      for(int j = 0; j < 8; ++j) { 
        accum_time += fTimeVec[GetIndex(type, i, j)];
        if (fTimeVec[GetIndex(type, i, j)] > 0.0) {
          count++;
        }
      }
    }
    if( count > 0 ) return accum_time/count;
    else            return 0.0;
  }
  else if(mod == -1) {
    float accum_time = 0.0;
    int count = 0;
    for(unsigned int i = 0; i < 8; ++i) {
      accum_time += fTimeVec[GetIndex(type, ch, i)];
      if (fTimeVec[GetIndex(type, ch, i)] > 0.0) {
        count++;
      }
    }
    if( count > 0 ) return accum_time/count;
    else            return 0.0;
  }
  else {
    return fTimeVec[GetIndex(type, ch, mod)];
  }
}

//----------------------------------------------------------------------------------
float Anp::EventMBTS::GetWeightedTime(int type, int ch, int mod) const
{

  if(ch == -1 && mod == -1) {
    
    float accum_time_w = 0.0;
    for(int i = 0; i < 2; ++i) {
      for(int j = 0; j < 8; ++j) {
        int index = GetIndex(type, i, j); 
        accum_time_w += fTimeVec[index]*fChargeVec[index];
      }
    }
    float charge = GetCharge(type, ch, mod);
    if( charge > 0.0 ) return accum_time_w/charge;
    else               return 0.0;
  }
  else if(mod == -1) {
    float accum_time_w = 0.0;
    for(unsigned int i = 0; i < 8; ++i) {
      int index = GetIndex(type, ch, i);
      accum_time_w += fTimeVec[index]*fChargeVec[index];
    }
    float charge = GetCharge(type, ch, mod);
    if( charge > 0.0 ) return accum_time_w/charge;
    else               return 0.0;
  }
  else {
    int index = GetIndex(type, ch, mod);
    return fTimeVec[index];
  }

}
