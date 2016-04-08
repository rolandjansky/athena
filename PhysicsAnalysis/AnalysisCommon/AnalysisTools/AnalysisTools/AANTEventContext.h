/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_AANTEVENTCONTEXT_H
#define ANALYSISTOOLS_AANTEVENTCONTEXT_H


#include "GaudiKernel/IEvtSelector.h"

class IOpaqueAddress;

class AANTEventContext : virtual public IEvtSelector::Context {

public:
   /// Constructor
   AANTEventContext(const IEvtSelector* selector);
   /// Copy constructor
   AANTEventContext(const AANTEventContext& ctxt);
   /// Assignment
   AANTEventContext& operator= (const AANTEventContext& ctxt);
   /// Destructor
   virtual ~AANTEventContext();

   /// Inequality operator.
   virtual void* identifier() const;

private:
   const IEvtSelector* m_evtSelector;
};
#endif
