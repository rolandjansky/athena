/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCOUNTER_H
#define EVENTCOUNTER_H

/** @file EventSelectorCounterTool.h
 *  @brief This file contains the class definition for the EventSelectorCounterTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventSelectorCounterTool.h,v 1.1 2009-03-06 21:41:59 gemmeren Exp $
 **/

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/ICutFlowSvc.h"

#include <string>

class StoreGateSvc;

/** @class EventSelectorCounterTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/
class EventSelectorCounterTool : public AlgTool, virtual public IAthenaSelectorTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   EventSelectorCounterTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~EventSelectorCounterTool();

public:
   /// IAthenaSelectorTool Interface method implementations:
   virtual StatusCode initialize();
   virtual StatusCode postInitialize();
   virtual StatusCode preNext();
   virtual StatusCode postNext();
   virtual StatusCode preFinalize();
   virtual StatusCode finalize();

private:
   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_metaDataStore;
   ServiceHandle<ICutFlowSvc> m_cutflowSvc;
   CutIdentifier m_allEvtsCutID;
   CutIdentifier m_allEvtsPostNextCutID;
   bool m_notInitialized;
};

#endif
