/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of ByteStreamEmonInputSvc
//===================================================================

// Include files.
#include "ByteStreamEmonSvc/ByteStreamEmonInputSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "eformat/Version.h"
#include "eformat/HeaderMarker.h"
#include "eformat/DetectorMask.h"

#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "oh/OHRootProvider.h"

#include "TrigConfL1ItemsNamed.h"

#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "PersistentDataModel/DataHeader.h"

#include "rc/RunParamsNamed.h"

#include <cstdlib>
#include <csignal>

#include <boost/algorithm/string.hpp>
#include <boost/preprocessor/repetition.hpp>

namespace {


    // pure madness...

#define PARSE_ITEM(z, n, unused) \
    {  \
        vector<string> result;  \
        split(result, l1info.Item##n, algorithm::is_space()); \
        if(!result.empty()) { \
            trim_if(result[0], algorithm::is_any_of("'"));      \
            l1map[result[0]] = n;                               \
        } \
    }

    std::map<std::string,int> 
    convert_names_bits(const ByteStreamEmon::TrigConfL1ItemsNamed& l1info)
    {
        using namespace std;
        using namespace boost;

        map<string,int> l1map;
        BOOST_PP_REPEAT(256, PARSE_ITEM, ~)
        return l1map;
    }
#undef PARSE_ITEM

}

namespace {
    
    void handle_terminate(int)
    {
        // online wants 
        ::exit(0);
    }
}

// Constructor.
ByteStreamEmonInputSvc::ByteStreamEmonInputSvc(const std::string& name, ISvcLocator* svcloc)
    :   
    ByteStreamInputSvc(name,svcloc), 
    m_totalEventCounter(0),
    m_re(0),
    m_key_count(0),
    m_publish("Athena"),
    m_exit(false),
    m_frequency(60),
    m_updatePeriod(0),
    m_updatePeriodRange(0.2),
    m_clearHistograms(true),
    m_l1logic("Ignore"),
    m_l1origin("TAV"),
    m_stream_type("physics"),
    m_stream_logic("Ignore"),
    m_trigger_type(256),
    m_buffer_size(2),
    m_readDetectorMask(true),
    m_timeout(3600000),
    m_corrupted_events(false),
    m_state("None"),
    m_connect(false),
    m_histSvc(0),
    m_provider(0),
    m_frequency_counter(60),
    m_publish_target(0),
    m_inputMetaDataStore("StoreGateSvc/InputMetaDataStore", name ),
    m_sgSvc("StoreGateSvc", name),
    m_robProvider("ROBDataProviderSvc", name)
{

    declareProperty("Partition", m_partition, "Partitin name, default taken from $TDAQ_PARTITION if not set")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("Key", m_key , "EMON Selection key, e.g. SFI")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("KeyValue", m_value, "EMON key values, e.g. [SFI-1, SFI-2]; if empty all SFIs")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("KeyCount", m_key_count, "EMON key count, e.g. 5 to get five random SFIs")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("ExitOnPartitionShutdown", m_exit, "If true, exit if partition shuts down")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("ISServer", m_is_server ,"IS Server to publish histograms to")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("PublishName", m_publish, "Name under which to publish histograms")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("Include", m_include, "Regular expression to select histograms to publish")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("Exclude", m_exclude, "Regular expression to select histograms not to publish")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("Frequency", m_frequency, "Frequency (in number of events) of publishing histograms")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("UpdatePeriod", m_updatePeriod, "Frequency (in secods) of publishing histograms")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("UpdatePeriodRange", m_updatePeriodRange, "A percentage number to determine how close we have to be to the next update time to publish now");
    declareProperty("ClearHistograms", m_clearHistograms, "If true, clear histograms when new run starts (default: True)")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("LVL1Names", m_l1names, "A list of L1 bit names")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("LVL1Items", m_l1items, "A list of L1 bit numbers")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("LVL1Logic", m_l1logic, "'And', 'Or' or 'Ignore' (default: Ignore)")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("LVL1Origin", m_l1origin, "TBP, TAP, TAV (default: TAV")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("StreamType", m_stream_type, "HLT stream type (e.g. physics or calibration)")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("StreamNames", m_stream_names, "List of HLT stream names")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("StreamLogic", m_stream_logic, "'And', 'Or' or 'Ignore' (default: Ignore)")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);

    declareProperty("TriggerType", m_trigger_type, "LVL1 8 bit trigger type")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);

    declareProperty("GroupName", m_groupName, "Name of the monitoring group")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("BufferSize", m_buffer_size, "Number of buffers");
    declareProperty("ReadDetectorMaskFromIS", m_readDetectorMask, "Read detector mask from IS");
    declareProperty("Timeout", m_timeout, "Timeout in seconds, -1 == infinity")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("ProcessCorruptedEvents", m_corrupted_events, "Process corrupted events not passing check_tree()")->declareUpdateHandler(&ByteStreamEmonInputSvc::updateHandler, this);
    declareProperty("State",m_state,"Read-only property showing the state");
}

// Destructor.
ByteStreamEmonInputSvc::~ByteStreamEmonInputSvc()
{
    delete m_re; 
}

// Open the first input file and read the first event.
StatusCode ByteStreamEmonInputSvc::initialize()
{

    setProperty("State", "Init");

    StatusCode sc = ByteStreamInputSvc::initialize();
    if(!sc.isSuccess())   return sc;

    MsgStream log(msgSvc(), name() );

    // check properties
    if(m_partition.empty() && getenv("TDAQ_PARTITION") != 0) {
        m_partition = getenv("TDAQ_PARTITION");
    }

    if(m_partition.empty()){
        log << MSG::ERROR << " initialize: No partition name specified" << endmsg;
        return StatusCode::FAILURE;
    }
	 
    if(m_key.empty()) {
        log << MSG::ERROR << " initialize: No emon key " <<endmsg;
        return StatusCode::FAILURE;
    } 

    m_connect = true;

    if (service("THistSvc", m_histSvc, true).isFailure()) {
        log << MSG::ERROR << "Unable to locate THistSvc" << endmsg;
        m_is_server.clear();
    } else {
        if(!m_include.empty()) m_include_rex = m_include;
        if(!m_exclude.empty()) m_exclude_rex = m_exclude;
        if(m_frequency < 0) m_frequency = 100000;
        m_frequency_counter = m_frequency;

        // if time based update, disable frequency based update
        if(m_updatePeriod) {
            m_frequency = 0;
            // calculate our next target for publishing
            time_t now = time(0);
            m_publish_target = now - (now % m_updatePeriod) + m_updatePeriod;
        }
    }

    {
        // register incident handler for begin run
        IIncidentSvc* incsvc;
      
        if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc)) {
            log << MSG::FATAL << "Incident service not found" << endmsg;
            return StatusCode::FAILURE;
        }
      
        long int prio = 100;
        incsvc->addListener(this,"BeginRun", prio);
    }

    //-------------------------------------------------------------------------
    // Setup the InputMetaDataStore
    //-------------------------------------------------------------------------
    sc = m_inputMetaDataStore.retrieve();
    if( !sc.isSuccess() ) {
        log << MSG::FATAL << "Error retrieving InputMetaDataStore"+m_inputMetaDataStore << endmsg;
        return sc;
    }

    if(!m_robProvider.retrieve().isSuccess()) {
        ATH_MSG_FATAL("Cannot get rob data provider");
        return StatusCode::FAILURE;
    }

    signal(SIGTERM, handle_terminate);

    log << MSG::INFO << " initialized for: " << m_partition << " " << m_key << "/" << m_value << endmsg;

    return StatusCode::SUCCESS;
}

bool ByteStreamEmonInputSvc::getIterator()
{
    using namespace std;

    MsgStream log(msgSvc(), name() );
    setProperty("State", "Init");

    // log << MSG::INFO << "getIterator(): " << m_partition << endmsg;

    if(!IPCCore::isInitialised()) {
	static char *argv[2] = { 0 };
	int argc = 0;
	IPCCore::init(argc, argv);
    }

    IPCPartition partition(m_partition);
    while(!partition.isValid()) {
	if(m_exit) {
	    log << MSG::ERROR << "No such partition: " << m_partition << endmsg;
	    return false;
	} 
        log << MSG::INFO << "No such partition (yet?): " << m_partition << endmsg;
        sleep(20);
    }

    delete m_provider;
    m_provider = 0;

    std::auto_ptr<emon::SamplingAddress> address;

    if(m_key_count > 0) {
        address.reset(new emon::SamplingAddress(m_key, m_key_count));
    }  else {
        address.reset(new emon::SamplingAddress(m_key, m_value));
    }

    emon::Logic  l1_logic = emon::logic::IGNORE;
    emon::Logic  stream_logic = emon::logic::IGNORE;
    emon::Origin l1_origin = emon::origin::AFTER_VETO;

    if(m_l1logic == "Or") {
        l1_logic = emon::logic::OR;
    } else if (m_l1logic == "And") {
        l1_logic = emon::logic::AND;
    } else if (m_l1logic == "Ignore") {
        l1_logic = emon::logic::IGNORE;
    }

    if(m_l1origin == "TBP") {
        l1_origin = emon::origin::BEFORE_PRESCALE;
    } else if (m_l1origin == "TAP") {
        l1_origin = emon::origin::AFTER_PRESCALE;
    } else if(m_l1origin == "TAV") {
        l1_origin = emon::origin::AFTER_VETO;
    } else {
        log << MSG::FATAL << "Invalid L1 origin" << endmsg;
    }

    if(m_stream_logic == "Or") {
        stream_logic = emon::logic::OR;
    } else if (m_stream_logic == "And") {
        stream_logic = emon::logic::AND;
    } else if (m_stream_logic == "Ignore") {
        stream_logic = emon::logic::IGNORE;
    }

    // now put together the bit mask from all
    // three sources: l1 bit mask, l1 names, l1 items

    vector<unsigned short> l1bits(m_l1items.begin(), m_l1items.end());

    // if names are given, try to read information from IS.
    if(m_l1names.size() > 0) {
        bool retry = true;
        ByteStreamEmon::TrigConfL1ItemsNamed l1info(partition, "L1CT.TrigConfL1Items");

        while (retry) {
            try {
                l1info.checkout();
                retry = false;
            } catch(...) {
                // might not exist yet...
                log << MSG::INFO << "No L1CT information, waiting 5 sec" << endmsg;
                sleep(5);
            }
        }

        // Now translate names into bits. 
        map<string,int> l1mapping = convert_names_bits(l1info);

        for(vector<string>::const_iterator it = m_l1names.begin();
            it != m_l1names.end();
            ++it) {
            if(l1mapping.find(*it) == l1mapping.end()) {
                log << MSG::ERROR << "Invalid L1 name in trigger mask: " << *it << endmsg;
                continue;
            }
            int bit = l1mapping[*it];
            l1bits.push_back(bit);
        }
    }

    typedef emon::MaskedValue<unsigned char>	L1TriggerType;
    
    emon::SmartBitValue l1pattern(l1bits, l1_logic, l1_origin);
    emon::SmartStreamValue streamTags(m_stream_type, m_stream_names, stream_logic);

    emon::L1TriggerType l1triggerType(static_cast<unsigned char>(m_trigger_type), m_trigger_type > 255);

    emon::SelectionCriteria criteria(l1triggerType,
                                     l1pattern,
                                     streamTags,
                                     emon::StatusWord());

    while (true) {

        if (partition.name() != m_partition) {
            log << MSG::INFO << "Partition name changed - reconnecting to " << m_partition << endmsg;
            return getIterator();
        }

	try {
            m_eventIt.reset(0);
	    m_eventIt.reset(new emon::EventIterator(partition, *address, criteria, m_buffer_size, m_groupName));
            if(m_readDetectorMask) {
                get_runparams();
            }
            setProperty("State", "Connected");
	    return true;
	} catch(ers::Issue& ex) {
	    log << MSG::INFO << "Cannot connect to sampler (will wait 20s): " << m_key << "/" << m_value 
                << " Reason: " << ex.what() << endmsg;
	    sleep(20);
	}
    }
}

// Read previous event should not be called for this version of input svc
const RawEvent* ByteStreamEmonInputSvc::previousEvent()
{
  MsgStream log(msgSvc(), name() );
  log << MSG::WARNING << "previousEvent not implemented for ByteStreamEmonInputSvc" << endmsg;

  return 0;
}

// Read the next event.
const RawEvent* ByteStreamEmonInputSvc::nextEvent()
{
    MsgStream log(msgSvc(), name() );

    if (m_re) {
        OFFLINE_FRAGMENTS_NAMESPACE::PointerType st = 0;
        m_re->start(st);
        if (st) delete [] st;
        delete m_re;
        m_re = 0 ;
    }

    // log << MSG::INFO << " nextEvent()" << endmsg;

    while(m_re == 0) {

        if(m_connect) {
	   
            if(!getIterator()) {
                // log << MSG::WARNING << "No iterator..." << endmsg;
                return 0;
            }  else {
                log << MSG::INFO << "Got sampler..." << endmsg;
                m_connect = false;
            }
        }
       
        emon::Event event;
        try {
            unsigned int timeout = m_timeout;
            if(m_updatePeriod) {
                // try to hit lower end of update period
                timeout = (m_publish_target - time(0) - int(m_updatePeriodRange * m_updatePeriod)) * 1000;
                if(timeout <= 0) timeout = 1000;
            }
            event = m_eventIt->nextEvent(timeout);
        } catch(emon::NoMoreEvents& ex) {
            if(m_exit) {
                return 0;
            }
            check_publish();
            continue;
        } catch (ers::Issue& ex) {
            m_connect = true;
            continue;
        }

        setProperty("State", "Processing");
       
        OFFLINE_FRAGMENTS_NAMESPACE::DataType* buf = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[event.size()];
        memcpy(buf, event.data(), event.size() * sizeof(OFFLINE_FRAGMENTS_NAMESPACE::DataType));


        if (buf[0] == eformat::FULL_EVENT) {

            // We got a full levent
            m_re = new RawEvent(buf);
            try {
                m_re->check_tree();
                log << MSG::INFO << "nextEvent: Got valid fragment of size:" << event.size() << endmsg;
                m_robProvider->setNextEvent(m_re);
                m_robProvider->setEventStatus(0);
            } catch (ers::Issue& ex) {
                // log in any case
                log << MSG::ERROR << "nextEvent: Invalid event fragment" << endmsg;
               
                if(!m_corrupted_events) {

                    delete [] buf;
                    delete m_re;
                    m_re = 0;
                    continue;
                } // else fall through
            }

        } else {
            // We got something we didn't expect.
            log << MSG::ERROR << "nextEvent: Got invalid fragment of unknown type: 0x" 
                << std::hex << buf[0] << std::dec << endmsg;
            delete [] buf;
            continue;
        }
        ++m_totalEventCounter;
    }

    // generate DataHeader
    DataHeader* Dh = new DataHeader();
    
   // Declare header primary
    Dh->setStatus(DataHeader::Input);

    // Now add ref to EventInfo objects
    IOpaqueAddress* iop = new ByteStreamAddress(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo", "");
    StatusCode ioc = m_sgSvc->recordAddress("ByteStreamEventInfo",iop);
    if (ioc.isSuccess()) {
        const SG::DataProxy* ptmp = m_sgSvc->transientProxy(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo");
        if (ptmp !=0) {
            DataHeaderElement DheEI(ptmp, nullptr, "ByteStreamEventInfo");
            Dh->insert(DheEI);
        }
        //else ATH_MSG_ERROR("Failed to create EventInfo proxy " << ptmp);
    }
    // Record new data header.Boolean flags will allow it's deletion in case
    // of skipped events.
    StatusCode rec_sg = m_sgSvc->record<DataHeader>(Dh, "ByteStreamDataHeader", true, false, true);
    if (rec_sg != StatusCode::SUCCESS) {
        ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
    }

    // we got an event, check if we have to publish
    check_publish();

    return m_re; 

}
   
// Get a pointer to the current event. 
const RawEvent* ByteStreamEmonInputSvc::currentEvent() const 
{
    return m_re;
}

void ByteStreamEmonInputSvc::check_publish()
{
    using namespace std;

    // is a server specified ?
    if(m_is_server.empty()) return;

    // a time based update ?
    if(m_updatePeriod) {
        time_t now = time(0);

        if(now < m_publish_target - int(m_updatePeriodRange * m_updatePeriod)) {
            // it's too early to publish
            return;
        }

        if(now > m_publish_target + int(m_updatePeriodRange * m_updatePeriod)) {
            // it's too late
            unsigned int missed_publications = 0;
            while(m_publish_target < now) {
                missed_publications++;
                m_publish_target += m_updatePeriod;
            }

            // do NOT publish, but print a warning
            MsgStream log(msgSvc(), name() );
            log << MSG::WARNING << " check_publish: missed " << missed_publications << " publications to OH" << endmsg;

            return;
        }
    }

    // an event based update ?
    if(m_frequency && --m_frequency_counter > 0) return;

    m_frequency_counter = m_frequency;

    try { 

	if(m_provider == 0) {
	    IPCPartition part(m_partition);
	    if(!part.isValid()) return;
	    // might throw...
	    m_provider = new OHRootProvider(part, m_is_server, m_publish);
	}

	vector<string> histnames = m_histSvc->getHists();
	for(vector<string>::iterator it = histnames.begin();
	    it != histnames.end();
	    ++it) {

	    if(!m_include.empty() && !regex_match(*it, m_include_rex)) {
		continue;
	    }

	    if(!m_exclude.empty() && regex_match(*it, m_exclude_rex)) {
		continue;
	    }

	    TH1 *h = 0;
	    if(m_histSvc->getHist(*it, h)) {
		// might throw...
		m_provider->publish(*h, *it);
	    };
	}
    } catch (daq::oh::Exception& ex) {
	MsgStream log(msgSvc(), name() );
	log << MSG::ERROR << ex.what() << endmsg;
    } 

    m_publish_target += m_updatePeriod;
}

void ByteStreamEmonInputSvc::get_runparams()
{
    using namespace std;

    MsgStream log(msgSvc(), name() );

    IPCPartition p(m_partition);
    
    RunParamsNamed runParams(p, "RunParams.RunParams");
    try {
        runParams.checkout();


	eformat::helper::DetectorMask mask(runParams.det_mask);
        
        ByteStreamMetadata* metadata = new ByteStreamMetadata( runParams.run_number,
                                                               0,
                                                               0,
                                                               runParams.recording_enabled,
                                                               runParams.trigger_type,
                                                               mask.serialize().second,
                                                               runParams.beam_type,
                                                               runParams.beam_energy,
                                                               std::string(),
							       std::string(""),
							       runParams.T0_project_tag,
							       0,
                                                               std::vector<std::string>() );
        m_inputMetaDataStore->clearStore().ignore();



        if (m_inputMetaDataStore->record(metadata,"ByteStreamMetadata").isFailure()) {
            delete metadata;
            metadata = 0;
            log << MSG::WARNING << " Unable to record MetaData in InputMetaDataStore." << endmsg;
        } else {
            log << MSG::INFO << " Recorded MetaData in InputMetaDataStore." << endmsg;
        }
    } catch(ers::Issue& ex) {
        log << MSG::ERROR << "Cannot get run parameters:" << ex.what() << endmsg;
    }
}

//
// BeginRun incident handler
//
void ByteStreamEmonInputSvc::handle(const Incident& /* incident */)
{
    using namespace std;

    MsgStream log(msgSvc(), name() );

    if(m_readDetectorMask) {
        get_runparams();
    }

    if(m_clearHistograms) {
        log << MSG::INFO << "Resetting histograms at BeginRun incidient" << endmsg;
        vector<string> histnames = m_histSvc->getHists();
        for(vector<string>::iterator it = histnames.begin();
            it != histnames.end();
            ++it) {
            
            TH1 *h = 0;
            if(m_histSvc->getHist(*it, h)) {
                h->Reset();
            };
        }    
    }
}

void ByteStreamEmonInputSvc::updateHandler(Property& /* p */)
{
    if(! m_connect) {
        m_connect = true;
        sleep(2);
        setProperty("State","Reconnect");
    }
}


StatusCode ByteStreamEmonInputSvc::finalize() 
{
    setProperty("State","Shutdown");
    m_inputMetaDataStore.release().ignore();
    m_robProvider.release().ignore();
    m_sgSvc.release().ignore();
    return StatusCode::SUCCESS;
}
