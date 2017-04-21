// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMEVENTSTORAGEINPUTSVC_H 
#define BYTESTREAMCNVSVC_BYTESTREAMEVENTSTORAGEINPUTSVC_H 

/**
 @class ByteStreamEmonInputSvc
 @brief implements the interface ByteStreamInputSvc for reading events 
        from emon.

*/

// Include files.
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"

// To avoid compiler warning
#undef PACKAGE_VERSION
#include "emon/EventIterator.h"

#include <vector>
#include <stdint.h>
#include <memory>

#include <boost/regex.hpp>

class ITHistSvc;
class OHRootProvider;
class StoreGateSvc;

class ByteStreamEmonInputSvc: public ByteStreamInputSvc, public virtual IIncidentListener {
public:

   /// Constructors:
   ByteStreamEmonInputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Destructor.
   virtual ~ByteStreamEmonInputSvc();
    
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Implementation of the ByteStreamInputSvc interface methods.
   virtual const RawEvent* previousEvent();
   virtual const RawEvent* nextEvent();

   /// Implementation of the ByteStreamInputSvc interface methods.
   virtual const RawEvent* currentEvent() const;

public: // IIncidentListener
   virtual void handle(const Incident&);

private: 

   bool getIterator();
   void check_publish();
   void get_runparams();

   void updateHandler(Property& p);

   int                m_totalEventCounter; //!< event Counter 

   RawEvent*          m_re;                //!< current event 

   // Properties
   std::string               m_partition; //!< Partition name
   std::string               m_key;       //!< Emon key, e.g. "SFI"
   std::vector<std::string>  m_value;     //!< Emon values, e.g. "SFI-1", if empty, all SFIs
   unsigned int              m_key_count; //!< Emon key count, e.g. 5 to get five random SFIs
   std::string               m_publish;   //!< Name to publish histograms under
   bool                      m_exit;      //!< Exit if partition shuts down (default: false)
   std::string               m_is_server; //!< Name of IS server to publish to
   std::string               m_include;   //!< Regexpression of histograms to publish
   std::string               m_exclude;   //!< Regexpression of histograms not to publish
   int                       m_frequency; //!< After how many events to publish
   int                       m_updatePeriod; //!< After how many seconds to publish
   float                     m_updatePeriodRange; //!< Percentage of update window where allowed to publish.
   bool                      m_clearHistograms; //!< Clear histograms for each new run (default: true)
   std::vector<std::string>  m_l1names;   //!< Level 1 names
   std::vector<unsigned int> m_l1items;   //!< Level 1 items, as a numerical list
   std::string               m_l1logic;   //!< One of: 'Or', 'And', 'Ignore' (default: Ignore)
   std::string               m_l1origin;  //!< One of: 'TBP', 'TAP', 'TAV'
   std::string               m_stream_type;  //!< HLT stream type (physics or calibration)
   std::vector<std::string>  m_stream_names; //!< HLT stream names
   std::string               m_stream_logic; //!< One of 'Or', 'And', 'Ignore' (default: Ignore)

   unsigned int              m_trigger_type; //!< LVL1 8 bit trigger type
   bool                      m_dispersion;  //!< Dispersion
   unsigned int              m_buffer_size; //!< Number of events buffered internally.
   bool                      m_readDetectorMask; //!< Read detector mask and run params from IS
   int                       m_timeout; //!< Timeout in seconds, -1 == infinite
   bool                      m_corrupted_events; //!< process corrupted events ?
    std::string              m_state; 
   
   // internal
   bool                      m_connect; 
   std::auto_ptr<emon::EventIterator> m_eventIt;   //!< Event iterator

   ITHistSvc                 *m_histSvc;
   OHRootProvider            *m_provider;

   boost::regex              m_include_rex;
   boost::regex              m_exclude_rex;

   int                       m_frequency_counter;

   int                       m_publish_target;

   /// Reference to StoreGateSvc;
   ServiceHandle<StoreGateSvc>  m_inputMetaDataStore;
   ServiceHandle<StoreGateSvc>  m_sgSvc; 
   ServiceHandle<IROBDataProviderSvc>        m_robProvider;

};

#endif  
