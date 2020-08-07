/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  ISecondaryEventSelector.h
 * @author Tadej Novak <tadej@cern.ch>
 * @brief Abstract interface for secondary event selectors
 */

#ifndef ATHENAKERNEL_ISECONDARYEVENTSELECTOR_H
#define ATHENAKERNEL_ISECONDARYEVENTSELECTOR_H

#include <GaudiKernel/IEvtSelector.h>
#include <AthenaKernel/IEvtSelectorSeek.h>
#include <AthenaKernel/SourceID.h>

namespace coral
{
  class AttributeList;
}

/**
 * @class ISecondaryEventSelector
 * @brief Abstract interface for secondary event selectors
 */
class ISecondaryEventSelector : virtual public extend_interfaces<IEvtSelector, IEvtSelectorSeek>
{
public:
  DeclareInterfaceID(ISecondaryEventSelector, 1, 0);

  /// Handle file transition at the next iteration
  virtual StatusCode nextHandleFileTransition(IEvtSelector::Context& ctxt) const = 0;
  /// Sync event count
  virtual void syncEventCount(int count) const = 0;
  /// Record AttributeList in StoreGate
  virtual StatusCode recordAttributeList() const = 0;
  /// Fill AttributeList with specific items from the selector and a suffix
  virtual StatusCode fillAttributeList(coral::AttributeList *attrList, const std::string &suffix, bool copySource) const = 0;
  // Disconnect DB if all events from the source FID were processed and the Selector moved to another file
  virtual bool disconnectIfFinished(const SG::SourceID &fid) const = 0;
};

#endif // ATHENAKERNEL_ISECONDARYEVENTSELECTOR_H
