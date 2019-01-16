/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class LumiBlockRange_p2
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKRANGE_P2_H 
#define LUMIBLOCKRANGE_P2_H 

 #include <stdint.h>

  class LumiBlockRange_p2 {
  // Public methods: 

  public: 

      ///////////////////////////////////////////////////////////////////////
      //  This class is the persistent form of LB_IOVRange that will be put
      //  into LumiBlockCollection_p2 (the persistent form of 
      //  LumiBlockCollection)
      //  Nota Bene:
      //   For the IOVTime, we are only going to stream the (run,lumiblock)  datamembers
      //   In LumiBlockCollection_p2, we require that the IOVTime objects
      //   in the IOVRange have isRunEvent()==true and isTimestamp()==false
      //   this means we only need 2 unsigned int64_t in the persistent representation
      ///////////////////////////////////////////////////////////////////////

   unsigned long long m_start;
   unsigned long long m_stop;
   unsigned int m_NumExpected;
   unsigned int m_NumSeen;
   LumiBlockRange_p2(unsigned long long start, unsigned long long stop): 
                   m_start(start),m_stop(stop),m_NumExpected(0),m_NumSeen(0) {};
   LumiBlockRange_p2(unsigned long long start, unsigned long long stop, unsigned int nExpect, unsigned int nSeen): 
                   m_start(start),m_stop(stop),m_NumExpected(nExpect),m_NumSeen(nSeen) {};
   LumiBlockRange_p2() : m_start(0),m_stop(0),m_NumExpected(0),m_NumSeen(0) {};
      
 }; 

#endif //> LUMIBLOCKRANGE_P2_H
