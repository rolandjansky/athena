/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef controlaline_H
#define controlaline_H
extern "C" void controlaline_(int& IControlALine);
extern "C" void controliline_(int& IControlILine);
extern "C" void truncatealine_();
extern "C" void truncatepline_();
extern "C" void reformataline_(double& TrS,double& TrZ,double& TrT,double& RoS,double& RoZ,double& RoT);
#endif
