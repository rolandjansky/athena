/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef fgetranges_H
#define fgetranges_H
extern "C" void fstationtyperange_( int& JtypBeg , int& JtypEnd );
extern "C" void fstationgeomrange_( int& JgeoBeg , int& JgeoEnd );
extern "C" void fpositionphirange_( int& JffBeg  , int& JffEnd  );
extern "C" void fpositionzrange_(   int& JzzBeg  , int& JzzEnd  );
extern "C" void fcutoutrange_(      int& JzzBeg  , int& JzzEnd  );
#endif
