//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of a LArFEBTempTool.
   * @author Kirill Skovpen
*/

#ifndef LARFEBTEMP2NTUPLE_H
#define LARFEBTEMP2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"

class LArFEBTemp2Ntuple : public LArCond2NtupleBase
{
 public:
   LArFEBTemp2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
   ~LArFEBTemp2Ntuple();

   //standard algorithm methods
   StatusCode finalize(){return StatusCode::SUCCESS;}
   StatusCode stop();
 private:
 
};

#endif
