/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MemoryLogger.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_MEMORYLOGGER_H
#define TRKDETDESCRUTILS_MEMORYLOGGER_H

// STL
#include <iostream>
//Gaudi
#include "GaudiKernel/MsgStream.h"

namespace Trk {

  /**
    @class MemoryLogger

    A small helper class looking at
    /proc/<PID>status

    to monitor the memory usage of the Geometry objects

    @author Andreas.Salzburger@cern.ch
    */

    class MemoryLogger {

      public:
        /** Default constructor*/
        MemoryLogger();
        /** update the memory logger */
        void refresh(int pid) const;

        /** vsize/rss returnal */
        float vmSize() const;
        float vmRss()  const;


      private:
        mutable float           m_vsize;  //!< virtual memory size
        mutable float           m_rss;    //!< real memory size

    };

inline float MemoryLogger::vmSize() const { return m_vsize; }

inline float MemoryLogger::vmRss() const { return m_rss; }

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream& operator << ( MsgStream& sl, const MemoryLogger& oac);
std::ostream& operator << ( std::ostream& sl, const MemoryLogger& oac);

} // end of namespace Trk

#endif

