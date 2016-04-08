/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once 
// PythiaBc.h
//
// Konstantin Toms <ktoms@mail.cern.ch>
//

extern void WritePythiaBcParam(int,int,double);
extern "C" void readpythiabcparamint_(int*,int*);
extern "C" void readpythiabcparamdbl_(int*,double*);
