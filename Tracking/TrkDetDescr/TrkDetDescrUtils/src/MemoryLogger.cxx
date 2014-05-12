/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MemoryLogger.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/MemoryLogger.h"
#include <fstream>
#include <sstream>


Trk::MemoryLogger::MemoryLogger() :
 m_vsize(-1),
 m_rss(-1)
{}

void Trk::MemoryLogger::refresh(int pid) const
{

#ifndef __APPLE__
    // memory monitoring
    std::ostringstream filename;
    filename << "/proc/" << pid << "/status";

    std::ifstream status(filename.str().c_str());
    std::string buff;
    // loop over the files in the input
      while (!status.eof()){
         status >> buff;
         if ( buff == "VmSize:" ) status >> m_vsize;
         if ( buff == "VmRSS:" )  status >> m_rss;
       }
     // close the file again
     status.close();
#else
#warning MemoryLogger does not yet work on the mac.
#endif
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::MemoryLogger& mlg)
{
  sl << "[ memory usage ] in kB ( VmSize | VmRSS )  : " << mlg.vmSize() << '\t' << mlg.vmRss();
  return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::MemoryLogger& mlg)
{
  sl << "[ memory usage ] in kB ( VmSize | VmRSS )  : " << mlg.vmSize() << '\t' << mlg.vmRss();
  return sl;
}
