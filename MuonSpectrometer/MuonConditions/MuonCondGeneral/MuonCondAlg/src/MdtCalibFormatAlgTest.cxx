/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/MdtCalibFormatAlgTest.h"


// constructor
MdtCalibFormatAlgTest::MdtCalibFormatAlgTest(const std::string& name, ISvcLocator* pSvcLocator ) :
                         AthReentrantAlgorithm( name, pSvcLocator ){
}

// destructor
MdtCalibFormatAlgTest::~MdtCalibFormatAlgTest(){
	ATH_MSG_VERBOSE( "Calling destructor"  );
}

// initialize
StatusCode
MdtCalibFormatAlgTest::initialize(){ 
	ATH_MSG_INFO("Calling initialize");
	ATH_CHECK(m_readKey_RT_old.initialize());
	ATH_CHECK(m_readKey_RT_new.initialize());
	ATH_CHECK(m_readKey_T0_old.initialize());
	ATH_CHECK(m_readKey_T0_new.initialize());
	return StatusCode::SUCCESS;
}

// execute
StatusCode
MdtCalibFormatAlgTest::execute(const EventContext& ctx) const {
	ATH_MSG_INFO("Calling execute");

	// setup parameters
	std::chrono::duration<double> retrieving_RT_old;
	std::chrono::duration<double> retrieving_RT_new;
	std::chrono::duration<double> retrieving_T0_old;
	std::chrono::duration<double> retrieving_T0_new;

	// retrieve all folders
	if(!retrieve(ctx, "RT", "old", retrieving_RT_old).isSuccess()) return StatusCode::FAILURE;
	if(!retrieve(ctx, "RT", "new", retrieving_RT_new).isSuccess()) return StatusCode::FAILURE;
	if(!retrieve(ctx, "T0", "old", retrieving_T0_old).isSuccess()) return StatusCode::FAILURE;
	if(!retrieve(ctx, "T0", "new", retrieving_T0_new).isSuccess()) return StatusCode::FAILURE;

	// postprocess
	ATH_MSG_INFO("Retrieving time for (RT, old) = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_RT_old).count()*1.0) << "s ");
	ATH_MSG_INFO("Retrieving time for (RT, new) = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_RT_new).count()*1.0) << "s ");
	ATH_MSG_INFO("Retrieving time for (T0, old) = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_T0_old).count()*1.0) << "s ");
	ATH_MSG_INFO("Retrieving time for (T0, new) = " << (std::chrono::duration_cast<std::chrono::microseconds>(retrieving_T0_new).count()*1.0) << "s ");

	ATH_MSG_INFO("MADE IT TO THE END!!");
	return StatusCode::SUCCESS;
}

// finalize
StatusCode
MdtCalibFormatAlgTest::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}


// retrieve
StatusCode
MdtCalibFormatAlgTest::retrieve(const EventContext& ctx, std::string folder, std::string setup, std::chrono::duration<double>& timer) const {

	ATH_MSG_INFO("Starting with "<<folder<<" and "<<setup<<" at "<<timestamp());

    //Start with an infinte range and narrow it down as needed
    EventIDRange rangeW=IOVInfiniteRange::infiniteMixed();

	SG::ReadCondHandleKey<CondAttrListCollection> readKey = m_readKey_RT_old;
	if     (setup=="new" && folder=="T0") readKey = m_readKey_T0_new;
	else if(setup=="new" && folder=="RT") readKey = m_readKey_RT_new;
	else if(setup=="old" && folder=="T0") readKey = m_readKey_T0_old;

    SG::ReadCondHandle<CondAttrListCollection> readHandle{readKey, ctx};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==0){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE; 
    } 
  
    EventIDRange range;
    if ( !readHandle.range(range) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
  
    //intersect validity range of this obj with the validity of already-loaded objs
    rangeW=EventIDRange::intersect(range,rangeW);

    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << range << ", range of output is " << rangeW);

    CondAttrListCollection::const_iterator itr;
    for(itr = readCdo->begin(); itr != readCdo->end(); ++itr) {

		auto start1 = std::chrono::high_resolution_clock::now();
        const coral::AttributeList& atr = itr->second;
		auto end1 = std::chrono::high_resolution_clock::now();
		timer += end1-start1;
		//m_retrieving[folder][setup] += end1-start1;

		std::string data;
		if(atr["data"].specification().type() == typeid(coral::Blob)){
			ATH_MSG_VERBOSE("Loading data as a BLOB, uncompressing...");
			Bytef* decompression_buffer = uncompressInMyBuffer(atr["data"].data<coral::Blob>());
			if(!decompression_buffer){
				ATH_MSG_FATAL("Cannot uncompress BLOB! Aborting...");
				return StatusCode::FAILURE;
			}
			data = (reinterpret_cast<char*>(decompression_buffer));
		}
		else {
			ATH_MSG_VERBOSE("Loading data as a STRING");
			data = *(static_cast<const std::string*>((atr["data"]).addressOfData()));
		}
	
		// write it to the buffer to compare it
		if(processBlob(folder, setup, data).isFailure()) return StatusCode::FAILURE;
	}
	ATH_MSG_INFO("Ending at "<<timestamp());
	return StatusCode::SUCCESS;
}


// processBlob
StatusCode
MdtCalibFormatAlgTest::processBlob(std::string folder, std::string setup, std::string data) const {

	// ATH_CHECK( extractString(istr, header, "\n") );
	// ATH_CHECK( extractString(istr, payload, "\n") );
	// if( istr.size() ) ATH_CHECK( extractString(istr, trailer, "\n") );
    ATH_MSG_DEBUG("processing Blob " << folder << "," << setup << "," << data); // leave for now
	return StatusCode::SUCCESS;
}


// uncompressInMyBuffer
Bytef* 
MdtCalibFormatAlgTest::uncompressInMyBuffer(const coral::Blob &blob) const {
	/* stolen from here: MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibDbCoolStrTool/src/MdtCalibDbCoolStrTool.cxx */

  uLongf buffer_length = 50000;
  Bytef *decompression_buffer = new Bytef[buffer_length];
  Bytef *empty = nullptr;
  uLongf actual_length;	
  while(1) {
    actual_length=buffer_length;
    int res(uncompress(decompression_buffer, &actual_length, reinterpret_cast<const Bytef *>(blob.startingAddress()), static_cast<uLongf>(blob.size())));
    if (res == Z_OK) break;
    //double buffer if it was not big enough
    if( res == Z_BUF_ERROR) {
      buffer_length*=2;
      ATH_MSG_VERBOSE(  "Increasing buffer to " << buffer_length);
      delete [] decompression_buffer;
      decompression_buffer = new Bytef[buffer_length];
      continue;
    }
    //something else is wrong
    return empty;
  }
  //append 0 to terminate string, increase buffer if it is not big enough
  if (actual_length >= buffer_length)	{
    Bytef * old_buffer=decompression_buffer;
    size_t old_length=buffer_length;
    buffer_length*=2;
    decompression_buffer = new Bytef[buffer_length];
    memcpy(decompression_buffer, old_buffer, old_length);
    delete [] old_buffer;
  }
  decompression_buffer[actual_length]=0;
  return decompression_buffer;
}


// extractString
StatusCode 
MdtCalibFormatAlgTest::extractString(std::string &input, std::string &output, std::string separator) const {
	/* stolen from here: MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibDbCoolStrTool/src/MdtCalibDbCoolStrTool.cxx */
  unsigned long int pos = 0;
  std::string::size_type start = input.find_first_not_of(separator.c_str(),pos);
  if(start == std::string::npos) {
    ATH_MSG_ERROR("Cannot extract string in a proper way!");
    return StatusCode::FAILURE;
  }
  std::string::size_type stop = input.find_first_of(separator.c_str(),start+1);
  if (stop == std::string::npos) stop = input.size();
  output = input.substr(start,stop-start);
  input.erase(pos,stop-pos);

  return StatusCode::SUCCESS;
} 

std::string
MdtCalibFormatAlgTest::timestamp() const {
	const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	const boost::posix_time::time_duration td = now.time_of_day();
	const long hours        = td.hours();
	const long minutes      = td.minutes();
	const long seconds      = td.seconds();
	const long milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, milliseconds);
    return buf;
}


