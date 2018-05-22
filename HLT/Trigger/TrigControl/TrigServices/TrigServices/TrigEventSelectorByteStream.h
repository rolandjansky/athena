/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTSELECTORBYTESTREAM_H
#define TRIGEVENTSELECTORBYTESTREAM_H

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/Service.h"

/**
 * @class  TrigEventSelectorByteStream
 * @brief  online implementation of IEvtSelector for ByteStream
 * 
 * Unlike the offline EventSelectorByteStream, this implementation does not use a ByteStreamInputSvc,
 * but directly requests events from the TDAQ infrastructure throught the hltinterface::DataCollector interface
 */

class TrigEventSelectorByteStream : public Service,
                                    virtual public IEvtSelector {
public:
  /// Standard constructor
  TrigEventSelectorByteStream(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigEventSelectorByteStream();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  // ------------------------- IEvtSelector methods ---------------------------
  virtual StatusCode createContext(Context*& c) const override;
  virtual StatusCode next(Context& c) const override;
  virtual StatusCode next(Context& c, int jump) const override;
  virtual StatusCode previous(Context& c) const override;
  virtual StatusCode previous(Context& c, int jump) const override;
  virtual StatusCode last(Context& refContext) const override;
  virtual StatusCode rewind(Context& c) const override;
  virtual StatusCode createAddress(const Context& c, IOpaqueAddress*& iop) const override;
  virtual StatusCode releaseContext(Context*& c) const override;
  virtual StatusCode resetCriteria(const std::string& cr, Context& c) const override;

  // ------------------------ Extra methods -----------------------------------
  bool noMoreEvents() const {return m_noMoreEvents;}

private:
  /// true if no more events available for next()
  bool m_noMoreEvents;
};

#endif // TRIGEVENTSELECTORBYTESTREAM_H