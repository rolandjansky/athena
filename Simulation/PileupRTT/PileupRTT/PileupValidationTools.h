/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupValidationTools.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef PILEUPRTTPILEUPVALIDATIONTOOLS_H
#define PILEUPRTTPILEUPVALIDATIONTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "IPileupValidationTools.h"

#include "EventInfo/PileUpEventInfo.h"

#include <vector>

///----------------------
/// Forward declarations
///----------------------

class EventInfo;
class TH2;

namespace PileupRTT
{

  /** @class PileupValidationTools

      Tools to help with Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */  

  class PileupValidationTools : virtual public IPileupValidationTools, public AthAlgTool
  {

    /* Public Methods */
    public:

       PileupValidationTools(const std::string& t,
                             const std::string& n,
                             const IInterface* p);
       virtual ~PileupValidationTools();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       
       virtual void printEventInfo(const EventInfo *evt);
       virtual void printSubEventInfo(const PileUpEventInfo::SubEvent subEv);
       virtual void printPileupTriggerInfo(const PileUpEventInfo *pevt);
       virtual bool hasRecycledEvents(TH2* hist, int bcTime, int &Nrepeat);
       virtual bool hasRecycledEvents(std::vector< std::pair<int,int> > evtList, int bcTime, int &Nrepeat);

    }; 
} // end of namespace

#endif // PILEUPRTTPILEUPVALIDATIONTOOLS_H
