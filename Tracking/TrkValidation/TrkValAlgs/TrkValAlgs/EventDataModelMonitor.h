/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// EventDataModelMonitor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKVALALGS_EVENTDATAMODELMONITOR_H
#define TRK_TRKVALALGS_EVENTDATAMODELMONITOR_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

namespace Trk {

/** @class EventDataModelMonitor
    Prints the number of EDM objects currently in memory. Designed to help look for memory leaks, and only runnable on DEBUG code 
    (i.e. will not work in opt mode)
 
   @author Edward.Moyse@cern.ch
*/

class EventDataModelMonitor : public AthAlgorithm {
public:
    /** Standard Athena-Algorithm Constructor */
    EventDataModelMonitor(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~EventDataModelMonitor();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:
};

} // close of namespace

#endif // TRK_TRKVALALGS_EVENTDATAMODELMONITOR_H

