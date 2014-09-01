/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMonitoringTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISF_INTERFACES_IMONITORINGTOOL_H
#define ISF_INTERFACES_IMONITORINGTOOL_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// stl
#include <string>


namespace ISF {

      /** Declaration of the interface ID ( interface id, major version, minor version) */
      static const InterfaceID IID_IMonitoringTool("IMonitoringTool", 1 , 0);

  /** @ class IMonitoringTool
  
  
      @ author Elmar.Ritsch -at- cern.ch
     */
    class IMonitoringTool : virtual public IAlgTool { 

      public: 
        /** Retrieve interface ID */
        static const InterfaceID& interfaceID() { return IID_IMonitoringTool; }

        /** virtual destructor */
        virtual ~IMonitoringTool() { }

        /** return current monitoring information */
        virtual double getCurrent() const = 0;

        /** dump current monitoring information */
        virtual void dumpCurrent(const char *infoStr,
                                 bool considerInSummary=false) = 0;

        /** store the current monitoring information internally */
        virtual void recordCurrent(const char *infoStr) = 0;

        /** dump all internally stored monitoring information */
        virtual void dumpSummary(const char *dumpCallDescr = 0) const = 0;
  }; 
}

#endif //> !ISF_INTERFACES_IMONITORINGOOL_H

