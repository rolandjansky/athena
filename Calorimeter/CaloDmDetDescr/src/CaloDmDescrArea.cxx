/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file Calorimeter/src/CaloDmDescrArea.cxx
 * @brief This object is used to compute calibration energy deposits in the dead material
 */
#include "CaloDmDetDescr/CaloDmDescrArea.h"
#include <cmath>
#include <iostream>


CaloDmDescrArea::CaloDmDescrArea()
{ }


CaloDmDescrArea::~CaloDmDescrArea()
{ }


/* ****************************************************************************
Return number of DM area for given dead material region 'key' and dead material
calibration hit eta. They should be obtained with help of CaloDmDescrManager
**************************************************************************** */
int CaloDmDescrArea::getArea(const int &key, const float &xeta)
{
  int nsmp=DMA_UNCLASS;

  float eta = fabs(xeta);

  if(key >=4101 && key <=4105) { // inner detector
    if(eta <1.5) {
      nsmp = DMA_EMB0;
    } else if (eta>=1.5 && eta<1.6){
      nsmp = DMA_SCN;
    } else if (eta>=1.6 && eta<1.8){
      nsmp = DMA_EME0;
    } else if (eta>=1.8 && eta<2.5){
      nsmp = DMA_EME13;
    } else if (eta>=2.5 && eta<3.2){
      nsmp = DMA_EME14;
    } else if (eta>=3.2) {
      nsmp = DMA_FCAL0;
    }
  } else if(key==4110 || key==4111 || key==4112) {
    nsmp = DMA_EMB0;
  } else if(key==4113) {
    nsmp = DMA_EMB1;
  }else if(key==4114 || key==4122 || key==4121 || key==5110 || key==5111) {
    nsmp = DMA_SCN;
  } else if(key==4115) {
    nsmp = DMA_EME0;
  } else if(key==4116) {
    if(eta<1.5){
      nsmp = DMA_EME11;
    }else if(eta<1.8){
      nsmp = DMA_EME12;
    }else if(eta<2.5){
      nsmp = DMA_EME13;
    }else if(eta<3.2){
      nsmp = DMA_EME14;
    }
  } else if(key==4120 || key==5100) {
    nsmp = DMA_EMB3_TILE0;
  } else if(key==4123) {
    nsmp = DMA_EME3_HEC0;
  } else if(key==4125 || key==5101) {
    nsmp = DMA_EME_EXT;
  } else if((key >=4130 && key<=4132) || key==5130) {
    nsmp = DMA_LEAK;
  } else if(key == 4202 || key == 4203 || key == 4212 || key == 4213
     || key == 4222 || key == 4223 || key == 4232 || key == 4233 ) {
    nsmp=DMA_HEC_AZIMUTH;
  } else if(key == 4204 || key == 4214 || key==4224 || key == 4234) {
    nsmp=DMA_HEC_FCAL;
  } else if(key == 4117 || key==4205 || key==4215 ) {
    nsmp = DMA_FCAL0;
  } else if(key == 4225) {
    nsmp = DMA_FCAL0_FCAL1;
  } else if(key == 4235) {
    nsmp = DMA_FCAL1_FCAL2;
  }else if(key == 4100) {
    nsmp = DMA_DEFCALC;
  } else if(key == 4124) {
    nsmp = DMA_HECWHEEL;
  }else if(key == 4200 || key == 4210|| key == 4220|| key == 4230) {
    nsmp = DMA_EMB_HALVES;
  }else if(key == 4201 || key == 4211|| key == 4221|| key == 4231) {
    nsmp = DMA_EME_INNER_OUTER;
  } else if(key == 5120 || key == 5121|| key == 5122 || key == 5112) {
    nsmp = DMA_LEAK;
  } else if(key == 5200) {
    nsmp = DMA_LEAK; // actually it's TILE default calculator, let it be inside DMA_LEAK
  }

  if(nsmp == DMA_UNCLASS ) {
    std::cout << " CaloDmDescrArea::getArea() -> Warning! Can not classify dead material hit,"  << " key:" << key << " eta:" << eta << std::endl;
  }

  return nsmp;
}
