/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/BarrelRoadData.h"
#include <iostream>

const float MuonRoads[2][12][8] = {{ // LOW PT threshold
/*  HP data ... 96 % cut */
  { 11.23, 11.22, 10.80, 10.83,  9.73,  9.62,  9.43,  9.24}, // Large Inner  st.
  {  6.00,  5.72,  5.98,  5.18,  5.30,  5.38,  0.00,  0.00}, // Large Middle st.
  { 17.54, 17.90, 18.12, 21.48,  21.78, 22.19,  0.00,  0.00}, // Large Outer  st.
  { 11.24, 11.22, 10.80, 10.83,  9.74,  9.62,  9.44,  9.24}, // Large Inner  sp.
  {  6.02,  5.73,  5.98,  5.18,  5.03,  5.38,  0.00,  0.00}, // Large Middle sp.
  { 17.54, 17.90, 18.12, 21.48, 21.78, 22.19,  0.00,  0.00}, // Large Outer  sp.


  { 29.07, 28.54, 28.60, 28.32, 27.93, 27.78, 27.55, 27.18}, // Small Inner  st.
  {  5.75,  5.68,  5.50,  5.13,  5.33,  5.18,  0.00,  0.00}, // Small Middle st.
  { 36.66, 36.36, 36.56, 36.38, 36.44, 36.50,  0.00,  0.00}, // Small Outer  st.
  { 29.07, 28.55, 28.60, 28.32, 27.93, 27.80, 27.55, 27.18}, // Small Inner  sp.
  {  5.75,  5.68,  5.50,  5.13,  5.33,  5.18,  0.00,  0.00}, // Small Middle sp.
  { 36.66, 36.36, 36.56, 36.38, 36.44, 36.50,  0.00,  0.00}, // Small Outer  sp.

  },{                  // HI PT threshold 
/* HP data .... 96 % cut */
  {  5.52,  5.48,  5.45,  5.39,  5.00,  4.92,  5.00,  4.95}, // Large Inner  st.
  {  5.58,  5.58,  5.38,  5.40,  5.48,  5.65,  0.00,  0.00}, // Large Middle st.
  {  3.87,  3.84,  3.87,  3.90,  3.98,  3.95,  0.00,  0.00}, // Large Outer  st.
  {  5.52,  5.48,  5.45,  5.39,  4.98,  4.92,  4.98,  4.95}, // Large Inner  sp.
  {  5.58,  5.58,  5.38,  5.40,  5.48,  5.65,  0.00,  0.00}, // Large Middle sp.
  {  3.87,  3.84,  3.87,  3.90,  3.98,  3.95,  0.00,  0.00}, // Large Outer  sp.

  {  9.86,  9.71,  9.68,  9.71,  9.57,  9.53,  9.36,  9.29}, // Small Inner  st.
  {  5.60,  5.43,  5.43,  5.25,  5.58,  5.43,  0.00,  0.00}, // Small Middle st.
  {  3.89,  3.89,  3.96,  4.96,  5.07,  5.07,  0.00,  0.00}, // Small Outer  st.
  {  9.86,  9.71,  9.68,  9.71,  9.57,  9.53,  9.36,  9.29}, // Small Inner  sp.
  {  5.60,  5.43,  5.43,  5.25,  5.58,  5.43,  0.00,  0.00}, // Small Middle sp.
  {  3.89,  3.89,  3.96,  4.97,  5.07,  5.07,  0.00,  0.00}, // Small Outer  sp.

  }};


TrigL2MuonSA::BarrelRoadData::BarrelRoadData()
{
  for(int th=0;th<2;++th) {
    for(int pos=0;pos<4;++pos) {
      for(int ly=0;ly<8;++ly) {
	m_roads[th][pos*3][ly]   = MuonRoads[th][pos*3][ly];
	m_roads[th][pos*3+1][ly] = MuonRoads[th][pos*3+1][ly];   
	m_roads[th][pos*3+2][ly] = MuonRoads[th][pos*3+2][ly];   
      }
    }
  }
}

TrigL2MuonSA::BarrelRoadData::~BarrelRoadData(void)
{
}

