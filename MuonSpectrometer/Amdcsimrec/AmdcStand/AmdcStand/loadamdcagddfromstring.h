/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef loadamdcagddfromstring_H
#define loadamdcagddfromstring_H
// IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
// IFLAG = 100  ===>  No conversion !
extern "C" void loadamdcagddfromstringn_(int& Namdc,char* Camdc,int& Nagdd,char* Cagdd,int& IFLAG);
extern "C" void loadamdcagddfromstring_( int& Namdc,char* Camdc,int& Nagdd,char* Cagdd);
#endif
