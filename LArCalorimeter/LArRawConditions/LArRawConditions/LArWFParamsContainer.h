//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* author : Marcello Fanti */

/* date of creation : 26/02/2004 */

/*  modification :   
 */

#ifndef LARWFPARAMSCONTAINER_H
#define LARWFPARAMSCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LArRawConditions/LArWFParams.h"

/**
  *  Liquid Argon Wave Parameters Container <br><br>
  * 
  * Publicly inherits from ATHENA ObjectVector<br>
  */
class LArWFParamsContainer : public DataVector<LArWFParams> {

 public:
  
  LArWFParamsContainer() : DataVector<LArWFParams> () {};

  virtual ~LArWFParamsContainer() {};

  virtual StatusCode initialize();

};

CLASS_DEF(LArWFParamsContainer,1235873721,0)

#endif
