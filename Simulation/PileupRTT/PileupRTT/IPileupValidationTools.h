/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// IPileupValidationTools.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#ifndef IPILEUPRTTPILEUPVALIDATIONTOOLS_H
#define IPILEUPRTTPILEUPVALIDATIONTOOLS_H

#include "GaudiKernel/AlgTool.h"

#include "EventInfo/PileUpEventInfo.h"

#include <vector>

class TH2;
class EventInfo;

namespace PileupRTT
{

  /** @class PileupValidationTools

      Tools to help with Validation of digitized pileup samples. Runs as part of the RunTimeTester.

      @author  David W. Miller <David.W.Miller@cern.ch>
  */ 
  
  static const InterfaceID IID_IPileupValidationTools("PileupValidationTools", 1, 0); 

  class IPileupValidationTools : virtual public IAlgTool
  {

    /* Public Methods */
    public:

       static const InterfaceID& interfaceID();
       
       virtual void printEventInfo(const EventInfo *evt) = 0;
       virtual void printSubEventInfo(const PileUpEventInfo::SubEvent subEv) = 0;
       virtual void printPileupTriggerInfo(const PileUpEventInfo *pevt) = 0;
       virtual bool hasRecycledEvents(TH2* hist, int bcTime, int &Nrepeat) = 0;
       virtual bool hasRecycledEvents(std::vector< std::pair<int,int> > evtList, int bcTime, int &Nrepeat) = 0;

  };
  
  inline const InterfaceID& IPileupValidationTools::interfaceID() { return IID_IPileupValidationTools; }
 
} // end of namespace

#endif // IPILEUPRTTPILEUPVALIDATIONTOOLS_H
