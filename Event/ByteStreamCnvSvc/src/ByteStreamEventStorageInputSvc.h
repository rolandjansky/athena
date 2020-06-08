/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMEVENTSTORAGEINPUTSVC_H
#define BYTESTREAMEVENTSTORAGEINPUTSVC_H

/** @file ByteStreamEventStorageInputSvc.h
 *  @brief This file contains the class definition for the ByteStreamEventStorageInputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  @author Frank Berghaus <fberghaus@anl.gov>
 **/

// Include files.
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "AthenaKernel/SlotSpecificObj.h"

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

namespace EventStorage
{
  class DataReader;
}
class StoreGateSvc;
class DataHeaderElement;


/** @class ByteStreamEventStorageInputSvc
 *  @brief This class is the ByteStreamInputSvc for reading events written by EventStorage.
 **/
class ByteStreamEventStorageInputSvc
: public ByteStreamInputSvc
{
public:
  /// Constructors:
  ByteStreamEventStorageInputSvc(const std::string& name,
      ISvcLocator* pSvcLocator);

  /// Destructor.
  virtual ~ByteStreamEventStorageInputSvc();

  /// Required of all Gaudi Services
  virtual StatusCode initialize();
  virtual StatusCode stop      ();
  virtual StatusCode finalize  ();

  /// Required of all Gaudi services:  see Gaudi documentation for details
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Implementation of the ByteStreamInputSvc interface methods.
  virtual const RawEvent* currentEvent () const;
  virtual const RawEvent* nextEvent    (); //!< ++, new
  virtual const RawEvent* previousEvent(); //!< --, old
  virtual void            setEvent     (void* data, unsigned int eventStatus);

  /// Return the current event status
  virtual unsigned int currentEventStatus() const;
  virtual void         validateEvent     (); 

  virtual long positionInBlock   ();
  virtual std::pair<long,std::string> getBlockIterator(const std::string fileName);
  void         closeBlockIterator(bool clearMetadata=true);
  bool         setSequentialRead ();
  bool         ready             () const;
  StatusCode   generateDataHeader();


private: // data
  std::mutex m_readerMutex;

  struct EventCache {
    std::unique_ptr<RawEvent> rawEvent    = nullptr; //!< current event
    unsigned int              eventStatus = 0;       //!< check_tree() status of the current event
    long long int             eventOffset = 0;       //!< event offset within a file, can be -1
    void                      releaseEvent();        //!< deletes fragments and raw event
    virtual                   ~EventCache();         //!< calls releaseEvent
  };

  SG::SlotSpecificObj<EventCache> m_eventsCache;

  std::unique_ptr<EventStorage::DataReader>  m_reader; //!< DataReader from EventStorage

  mutable std::vector<long long int> m_evtOffsets;  //!< offset for event i in that file
  unsigned int       m_evtInFile;
  long long int      m_evtFileOffset;   //!< last read in event offset within a file, can be -1     
  // Event back navigation info
  std::string        m_fileGUID;      //!< current file GUID



private: // properties
  /// Pointer to StoreGate
  ServiceHandle<StoreGateSvc>                m_storeGate;     //!< StoreGateSvc
  ServiceHandle<StoreGateSvc>                m_inputMetadata; //!< StoreGateSvc
  ServiceHandle<IROBDataProviderSvc>         m_robProvider;
  Gaudi::Property<bool>                      m_sequential;    //!< enable sequential reading.
  Gaudi::Property<bool>                      m_dump;
  Gaudi::Property<float>                     m_wait;
  Gaudi::Property<bool>                      m_valEvent;
  Gaudi::Property<std::string>               m_eventInfoKey;


private: // internal helper functions
  StatusCode loadMetadata    ();
  void       buildFragment   (EventCache* cache, char* data, uint32_t eventSize, bool validate) const;
  bool       readerReady     () const;
  bool       ROBFragmentCheck(const RawEvent*) const;
  unsigned   validateEvent   (const RawEvent* const rawEvent) const;
  void       setEvent        (const EventContext& context, void* data, unsigned int eventStatus);

  enum Advance{ PREVIOUS = -1, NEXT = 1 };
  const RawEvent* getEvent( Advance step );
  std::unique_ptr<DataHeaderElement> makeBSProvenance() const;

  template<typename T>
  StatusCode deleteEntry(const std::string& key)
  {
    if (m_storeGate->contains<T>(key)) {
      const T* tmp = m_storeGate->tryConstRetrieve<T>(key);
      if (tmp != nullptr) ATH_CHECK(m_storeGate->remove<T>(tmp));
    }
    return StatusCode::SUCCESS;
  }

};

#endif // BYTESTREAMEVENTSTORAGEINPUTSVC_H
