/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class LumiBlockRange_p1
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKRANGE_P1_H 
#define LUMIBLOCKRANGE_P1_H 

 #include <stdint.h>

  class LumiBlockRange_p1 {
  // Public methods: 

  public: 

      ///////////////////////////////////////////////////////////////////////
      //  This class is the persistent form of IOVRange that will be put
      //  into LumiBlockCollection_p1 (the persistent form of 
      //  LumiBlockCollection)
      //  Nota Bene:
      //   We are only going to stream the (run,lumiblock)  datamembers
      //   In LumiBlockCollection_p1, we require that the IOVTime objects
      //   in the IOVRange have isRunEvent()==true and isTimestamp()==false
      //   this means we only need 2 uint64_t in the persistent representation
      ///////////////////////////////////////////////////////////////////////

   unsigned long long m_start;
   unsigned long long m_stop;
   LumiBlockRange_p1(unsigned long long start, unsigned long long stop): 
                   m_start(start),m_stop(stop) {};
   LumiBlockRange_p1() : m_start(0),m_stop(0) {};
      
 }; 

#endif //> LUMIBLOCKRANGE_P1_H
