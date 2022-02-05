/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DOUBLEEVENTSELECTORATHENAPOOL_H
#define DOUBLEEVENTSELECTORATHENAPOOL_H

/**
 * @file DoubleEventSelectorAthenaPool.h
 * @brief This file contains the class definition for the DoubleEventSelectorAthenaPool class.
 * @author Peter van Gemmeren <gemmeren      -at- anl.gov>
 * @author John Detek Chapman <chapman       -at- hep.phy.cam.ac.uk>
 * @author Miha Muskinja      <miha.muskinja -at- cern.ch>
 * @author Tadej Novak        <tadej         -at- cern.ch>
 **/

#include "EventSelectorAthenaPool.h"

#include "AthenaKernel/ISecondaryEventSelector.h"
#include "AthenaKernel/SlotSpecificObj.h"

/**
 * @class DoubleEventSelectorAthenaPool
 * @brief This class is the EventSelector for event data.
 **/
class DoubleEventSelectorAthenaPool : public EventSelectorAthenaPool
{
public:
  /// Standard Service Constructor
  DoubleEventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~DoubleEventSelectorAthenaPool();

  /// Initialize function
  virtual StatusCode initialize() override;

  //-------------------------------------------------
  // IEventSelector
  /// @param ctxt [IN/OUT] current event context is interated to next event.
  virtual StatusCode next(IEvtSelector::Context& ctxt) const override;
  /// @param ctxt [IN/OUT] current event context is interated to next event.
  /// @param jump [IN] number of events to jump (currently not supported).
  virtual StatusCode next(IEvtSelector::Context& ctxt, int jump) const override;

  //-------------------------------------------------
  // IEventSelectorSeek
  /// Seek to a given event number.
  /// @param ctxt [IN/OUT] current event context.
  /// @param evtNum [IN]  The event number to which to seek.
  virtual StatusCode seek(Context& ctxt, int evtNum) const override;

  /// Return the size of the collection.
  /// @param ctxt [IN/OUT] current event context.
  virtual int size (Context& ctxt) const override;

  //-------------------------------------------------
  // IIncidentListener
  /// Incident service handle listening for BeginProcessing and EndProcessing
  virtual void handle(const Incident& incident) override;

private:
  /// Record AttributeList in StoreGate
  virtual StatusCode recordAttributeList() const override;

  ServiceHandle<ISecondaryEventSelector> m_secondarySelector{this, "SecondaryEventSelector", "SecondaryEventSelector", ""};

  Gaudi::Property<std::string> m_secondaryAttrListSuffix{this, "SecondaryAttrListSuffix", "secondary", ""};

  // Cache if secondary selector is ByteStream
  bool m_secondaryByteStream{};

  SG::SlotSpecificObj<SG::SourceID> m_sourceID1;
  SG::SlotSpecificObj<SG::SourceID> m_sourceID2;
};

#endif
