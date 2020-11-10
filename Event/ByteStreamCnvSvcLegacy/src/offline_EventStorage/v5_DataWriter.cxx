/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <time.h>
#include <sstream>
#include <stdlib.h>
#include "v5_Guid.h"
#include "ers/ers.h"

#include <boost/shared_ptr.hpp>

#include "ByteStreamCnvSvcLegacy/offline_EventStorage/v5_DataWriter.h"
#include "EventStorage/DataWriterCallBack.h"
#include "EventStorage/FileNameCallback.h"
#include "EventStorage/RawFileName.h"
#include "EventStorage/EventStorageIssues.h"
#include "EventStorage/SimpleFileName.h"
#include "v5_DataBuffer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

using namespace std; 
using namespace offline_EventStorage_v5;
using EventStorage::DataWriterCallBack;
using EventStorage::DWError;
using EventStorage::FileNameCallback;
using EventStorage::SimpleFileName;

// constructors
DataWriter::
DataWriter(const string& writingPath,   
	   boost::shared_ptr<FileNameCallback> theFNCB,   
	   const run_parameters_record& rPar, 
	   const std::string& project,
	   const std::string& streamType,
	   const std::string& streamName,
	   const std::string& stream,
	   const unsigned int lumiBlockNumber,
	   const std::string& applicationName,
	   const std::vector<std::string>& fmdStrings,
	   const CompressionType compression,
	   const unsigned int compLevel) 
{
  initDW(writingPath, theFNCB, rPar, project, streamType, streamName, stream, lumiBlockNumber, applicationName, fmdStrings, compression, compLevel);

}

DataWriter::
DataWriter(const string& writingPath,   
	   const string& fileNameCore,     
	   const run_parameters_record& rPar, 
	   const freeMetaDataStrings& fmdStrings, 
	   const unsigned int startIndex,
	   const CompressionType compression,
	   const unsigned int compLevel)
{
  boost::shared_ptr<daq::RawFileName> 
    fileName(new daq::RawFileName(fileNameCore));

  if ( fileName->hasValidCore() ) {
    m_project         = fileName->project();
    m_streamType      = fileName->streamType();
    m_streamName      = fileName->streamName();
    m_stream          = fileName->stream();
    m_lumiBlockNumber = fileName->lumiBlockNumber();
    m_applicationName = fileName->applicationName();
  } else {
    std::stringstream mystream;
    mystream <<  "FileName " << fileName->fileName() 
	     << " has failed interpretation."
	     << " The file header will have no values for "
	     << " project, stream, lumiBlock and appName";
    ERS_DEBUG(1,mystream.str());
    // EventStorage::RawFileNameIssue myissue(ERS_HERE, mystream.str().c_str() );
    // ers::warning(myissue);
    m_project         = "";
    m_streamType      = "";
    m_streamName      = "";
    m_stream          = ""; 
    m_lumiBlockNumber = 0;
    m_applicationName = "";
  }

  fileName->setFileSequenceNumber(startIndex);

  initDW(writingPath, fileName, rPar, m_project, m_streamType, m_streamName,
	 m_stream, m_lumiBlockNumber, m_applicationName, 
	 fmdStrings, compression,compLevel);

}


DataWriter::
DataWriter(const string& writingPath,   
	   const string& fileNameCore,     
	   const run_parameters_record& rPar, 
	   const std::string& project,
	   const std::string& streamType,
	   const std::string& streamName,
	   const std::string& stream,
	   const unsigned int lumiBlockNumber,
	   const std::string& applicationName,
	   const std::vector<std::string>& fmdStrings,
	   const CompressionType compression,
	   const unsigned int compLevel)
{
  boost::shared_ptr<FileNameCallback> fileName(new SimpleFileName(fileNameCore));
  initDW(writingPath, fileName, rPar, project, streamType, streamName, stream, lumiBlockNumber, applicationName, fmdStrings, compression, compLevel);
  
}

// destructor    
DataWriter::~DataWriter()
{
  ERS_DEBUG(1,"DataWriter::~DataWriter() called.");
  
  m_file_end_record.status = 1; // This identifies the end of the sequence.
  if(m_cFileOpen) closeFile();
  
  delete m_compressed;

  ERS_DEBUG(1,"DataWriter::~DataWriter() finished.");
}


void DataWriter::
initDW(const string& writingPath,   
       boost::shared_ptr<FileNameCallback> theFNCB,   
       const run_parameters_record& rPar, 
       const std::string& project,
       const std::string& streamType,
       const std::string& streamName,
       const std::string& stream,
       const unsigned int lumiBlockNumber,
       const std::string& applicationName,
       const std::vector<std::string>& fmdStrings,
       const CompressionType compression,
       const unsigned int compLevel)
{
  m_project         = project;
  m_streamType      = streamType;
  m_streamName      = streamName;
  m_stream          = stream;
  m_lumiBlockNumber = lumiBlockNumber;
  m_applicationName = applicationName;

  m_filenamecallback = theFNCB;

  reset_record(&m_file_start_record,&file_start_pattern);
  reset_record(&m_internal_run_parameters_record,&run_parameters_pattern);
  reset_record(&m_file_end_record,&file_end_pattern);
  m_cFileMB = 0.0;
  m_runMB = 0.0;
  m_latestPosition = -1;

  m_writePath = writingPath;
  m_nextWritePath = "";
  //m_fileNameCore = fileNameCore;

  m_file_name_strings.appName = m_applicationName;
  // m_file_name_strings.appName = getAppName(fileNameCore);
  m_file_name_strings.fileNameCore = m_filenamecallback->getCoreName();

  setRunParamsRecord(rPar);

  m_file_start_record.sizeLimit_MB = 0;
  m_file_start_record.sizeLimit_dataBlocks = 0;
  date_timeAsInt(m_file_start_record.date,m_file_start_record.time);
  
  m_cFileOpen=false;
  m_openFailed = false;
  
  m_callBack = NULL;

  m_fmdStrings = fmdStrings;

  m_compression = compression;
  m_complevel = compLevel;
  m_compressed = new DataBuffer();

  // add stream, project, compression, lumiBlock 
  // (if they are valid) to meta-data:
  m_fmdStrings.push_back("Stream=" + m_stream );
  m_fmdStrings.push_back("Project=" + m_project );
  std::ostringstream o;
  o << m_lumiBlockNumber;
  m_fmdStrings.push_back("LumiBlock=" + o.str());
  
  if(m_compression != offline_EventStorage_v5::NONE){
    m_fmdStrings.push_back(offline_EventStorage_v5::compressiontag +
			   "=" + offline_EventStorage_v5::type_to_string(m_compression));
  }

  m_check = ::adler32(0L, Z_NULL, 0);
  
  m_guid = "";
  m_next_guid = "";
  this->spaceForGuid();
  this->openNextFile();

}


// */


//date and time utility
void DataWriter::date_timeAsInt(uint32_t &getDate, uint32_t &getTime) const
{
    long a_time;
    time(&a_time);
    
    struct tm  t;
    localtime_r( &a_time, &t);
    
    getDate= 1000000*t.tm_mday+
	     10000*(1+t.tm_mon)+
	     1900+t.tm_year;

    getTime= 10000*t.tm_hour+
	     100*t.tm_min+
	     t.tm_sec ;
}


bool DataWriter::good() const
{

    if(m_openFailed) return false;

    if(m_cFileOpen) 
    {
      return m_cFile.good();
    }
    else
    {
	return true;
    }

}


// configuration
DWError DataWriter::setMaxFileMB(const unsigned int& maxFileMB)
{
    m_file_start_record.sizeLimit_MB = maxFileMB;
    return EventStorage::DWOK;
}

DWError DataWriter::setMaxFileNE(const unsigned int& maxFileNE)
{
    m_file_start_record.sizeLimit_dataBlocks = maxFileNE;
    return EventStorage::DWOK;
}

void DataWriter::setRunParamsRecord(const run_parameters_record& rPar)
{
    // accept the params, whatever they are
    // run number is in this record (and only there)
    m_internal_run_parameters_record.marker = rPar.marker; 
    m_internal_run_parameters_record.record_size = rPar.record_size; 
    m_internal_run_parameters_record.run_number = rPar.run_number; 
    m_internal_run_parameters_record.max_events = rPar.max_events;
    m_internal_run_parameters_record.rec_enable = rPar.rec_enable;
    m_internal_run_parameters_record.trigger_type = rPar.trigger_type;
    m_internal_run_parameters_record.detector_mask_1of2 = rPar.detector_mask & 0xFFFFFFFF;
    m_internal_run_parameters_record.detector_mask_2of2 = (rPar.detector_mask>> 32); 
    m_internal_run_parameters_record.beam_type = rPar.beam_type;
    m_internal_run_parameters_record.beam_energy = rPar.beam_energy;
}

// output of data
DWError DataWriter::putData(const unsigned int& dataSize, const void *event)
{

  uint32_t todisk;
  iovec_const iov;
  iov.iov_base = event;
  iov.iov_len = dataSize;
  return this->putData(1, &iov, todisk);
}

// output of scattered data using iovec
DWError DataWriter::putData(const unsigned int& entries, const struct iovec * my_iovec) 
{
  uint32_t todisk;
  return this->putData(entries, my_iovec, todisk);
}

DWError DataWriter::putData(const unsigned int& dataSize, const void *event, uint32_t& sizeToDisk){
  
  iovec_const iov;
  iov.iov_base = event;
  iov.iov_len = dataSize;
  return this->putData(1, &iov, sizeToDisk);
}

DWError DataWriter::putData(const unsigned int& entries, const iovec_const * my_iovec, uint32_t& sizeToDisk){
  return this->putData_implementation(entries, my_iovec, sizeToDisk);
}


DWError DataWriter::putPrecompressedData(const unsigned int& dataSize, 
					 const void *event){

  iovec_const iov;
  iov.iov_base = event;
  iov.iov_len = dataSize;
  return this->putPrecompressedData(1, &iov);
}


DWError DataWriter::putPrecompressedData(const unsigned int& entries, 
					 const iovec_const* my_iovec){

  uint32_t todisk;
  return this->putData_implementation(entries, my_iovec, todisk, true);
}


DWError DataWriter::putData_implementation(const unsigned int& entries, const iovec_const * my_iovec, uint32_t& sizeToDisk, bool precompressed){

  ERS_DEBUG(3,"DataWriter::putData called for an iovec.");
  if(!m_cFileOpen) openNextFile();

  if(!m_cFileOpen) return EventStorage::DWNOOK;
  
  if(!m_cFile.good()) return EventStorage::DWNOOK;
  
  // calculate events size
  unsigned int dataSize = 0;
  for(unsigned int i = 0; i<entries; i++) {
    dataSize += my_iovec[i].iov_len;
  }


  
  sizeToDisk = dataSize;
  
  
  // apply limits 
  if(m_file_start_record.sizeLimit_MB>0) { 
    double eventMB=static_cast<double>(dataSize)/(1024*1024);
    double dataMB=m_cFileMB+eventMB;
    if(static_cast<unsigned int>(dataMB+0.5) > 
       m_file_start_record.sizeLimit_MB) {
      ERS_DEBUG(3,"Approaching file size limit in MB.");
      nextFile();
    }
  }
  
  if((m_file_start_record.sizeLimit_dataBlocks>0) && 
     (m_file_end_record.events_in_file >= m_file_start_record.sizeLimit_dataBlocks)){
    ERS_DEBUG(3,"Approaching file size limit in number of events.");
    nextFile();
  }

  //Here we should compress, so that we have the effective size
  //for the event record 
  if(m_compression == ZLIB && !precompressed){
    try{
        offline_EventStorage_v5::zlibcompress(*m_compressed, sizeToDisk,
				 entries, my_iovec, dataSize, m_complevel);
    }catch(offline_EventStorage_v5::CompressionIssue& ex){
      EventStorage::WritingIssue ci(ERS_HERE, "Data compression failed.", ex);
      ers::error(ci);
      return EventStorage::DWNOOK;
    }
  }
  
  // prepare the event record
  data_separator_record dsR = data_separator_pattern;
  dsR.data_block_number = m_file_end_record.events_in_run + 1;
  dsR.data_block_size = sizeToDisk;
  
  // remember the position
  m_latestPosition = m_cFile.tellg();
  
  // write the event record followed by event data    
  file_record(&dsR,&data_separator_pattern);
  
  
  if(m_compression != ZLIB || precompressed){
    for (unsigned int i =0; i<entries; i++){
      m_cFile.write(static_cast<const char*>(my_iovec[i].iov_base),
		    my_iovec[i].iov_len);
      
      m_check = ::adler32(m_check, 
			  static_cast<const Bytef *>(my_iovec[i].iov_base), 
			  my_iovec[i].iov_len);
    }
  }else{
    m_cFile.write(static_cast<const char *>(m_compressed->handle()), sizeToDisk);
    m_check = ::adler32(m_check, 
			static_cast<const Bytef *>(m_compressed->handle()), 
			sizeToDisk);
  }
  
  // update statistics
  m_cFileMB += static_cast<double>(sizeToDisk)/(1024*1024);
  m_runMB += static_cast<double>(sizeToDisk)/(1024*1024);
  m_file_end_record.events_in_file++;
  m_file_end_record.events_in_run++;
  
  ERS_DEBUG(3,"Event "<< m_file_end_record.events_in_run<<" is written at offset "<<m_latestPosition);
  
  if(!m_cFile.good()) return EventStorage::DWNOOK;
  
  return EventStorage::DWOK;
}


int64_t DataWriter::getPosition() const
{
  return m_latestPosition;
}

// statistics
unsigned int DataWriter::eventsInFile() const
{
  return m_file_end_record.events_in_file;
}

unsigned int DataWriter::eventsInFileSequence() const
{
  return m_file_end_record.events_in_run;
}

unsigned int DataWriter::dataMB_InFile() const
{
  return static_cast<unsigned int>(m_cFileMB+0.5);
}

unsigned int DataWriter::dataMB_InFileSequence() const
{
  return static_cast<unsigned int>(m_runMB+0.5);
}

// closes the current file    
DWError DataWriter::closeFile()
{
  ERS_DEBUG(3,"DataWriter::closeFile() called.");

  // do statistics
  m_file_end_record.data_in_file = static_cast<unsigned int>(m_cFileMB+0.5);
  m_file_end_record.data_in_run  = static_cast<unsigned int>(m_runMB+0.5);

  date_timeAsInt(m_file_end_record.date,m_file_end_record.time);

  // write eof record
  file_record(&m_file_end_record,&file_end_pattern);

  m_cFile.close();
  std::ostringstream oss;
  oss.width(8);
  oss.fill('0');
  oss << std::hex << std::uppercase <<m_check;
  std::string checksum = oss.str();
  m_check = ::adler32(0L, Z_NULL, 0);

  // rename the file
  ERS_DEBUG(3,"Rename file " << nameFile(UNFINISHED) << endl
	    << "to " << nameFile(FINISHED));
  if (::rename(nameFile(UNFINISHED).c_str(), nameFile(FINISHED).c_str()) == -1) {
    
    EventStorage::WritingIssue ci(ERS_HERE, "Rename failed.");
    ers::error(ci);
    return EventStorage::DWNOOK;
  }

  // check if the file is there
  if(fileExists(nameFile(FINISHED)) && m_cFileOpen) 
    {
      ERS_DEBUG(2,"Finished file "<<nameFile(FINISHED));
      if(m_callBack != NULL) {
	ERS_DEBUG(3,"Execute callback from DataWriter.");
	m_callBack->FileWasClosed(
				   m_filenamecallback->getCurrentFileName(),
				   nameFile(FINISHED),
				   m_streamType,
				   m_streamName,
				   m_applicationName,
				   m_guid,
				   checksum,
				   m_file_end_record.events_in_file, 
				   m_internal_run_parameters_record.run_number,
				   m_file_start_record.file_number,
				   m_lumiBlockNumber,
				   getFileSize(nameFile(FINISHED))
				   );
	  }
      
      // now we can substitute new directory, if specified
      if(m_nextWritePath != "") {
	m_writePath=m_nextWritePath;
	m_nextWritePath="";
	ERS_DEBUG(3,"New writing path became effective. The path is "
		  <<m_writePath);
      }
      m_cFileOpen = false;

      try{
	m_filenamecallback->advance();
      }catch(EventStorage::ES_SingleFile &e){
	//If opennext file is called, it 
	// will fail there
      }
      return EventStorage::DWOK;
    }
  return EventStorage::DWNOOK;
}

// closes the current file and opens another one   
DWError DataWriter::nextFile()
{
  ERS_DEBUG(2,"DataWriter::nextFile() called.");

  DWError we=closeFile();
   
/*  if (m_freename)
  {
    EventStorage::ES_SingleFile ci(ERS_HERE, "DataWriter called with freename = 1. DataWriter wants to go to next file, but cannot, since indexing is forbidden. Either deactive freename (so files can be indexed, or increase the file size limit (or turn it off)");
    throw ci;  
  }*/

  openNextFile();
  if(m_openFailed) we=EventStorage::DWNOOK;
  return we;
}

bool DataWriter::fileExists(const string& name) const
{
  ifstream test(name.c_str(),ios::binary | ios::in);
  bool isThere = test.good();
  test.close();
    
  return isThere;
}

void DataWriter::openNextFile()
{
  m_cFileMB=0.0;
  m_file_end_record.events_in_file=0;
  
  try{
    m_file_start_record.file_number = m_filenamecallback->getIndex();
  }catch(EventStorage::ES_SingleFile &e){
    m_file_start_record.file_number++;
  }
  date_timeAsInt(m_file_start_record.date,m_file_start_record.time);

  ERS_DEBUG(2,"Trying to open file "<<nameFile(UNFINISHED));
  
  while(fileExists(nameFile(UNFINISHED)) 
	|| fileExists(nameFile(FINISHED))) 
    {
    
      if(fileExists(nameFile(UNFINISHED))) {
	string err = "found an existing file with name " + nameFile(UNFINISHED);
	EventStorage::WritingIssue ci(ERS_HERE, err.c_str());
	ers::warning(ci);
	
      } else {
	
	string err = "found an existing file with name " + nameFile(FINISHED);
	EventStorage::WritingIssue ci(ERS_HERE, err.c_str());
	ers::warning(ci);
      }
      m_filenamecallback->fileAlreadyExists();
      
      m_file_start_record.file_number = m_filenamecallback->getIndex();
      
      ostringstream oss;
      oss << "increase file number to ";
      oss << m_file_start_record.file_number;
      EventStorage::WritingIssue ci(ERS_HERE, oss.str().c_str());
      ers::warning(ci);
    }
  ERS_DEBUG(2,"OK to write file with number "<<m_file_start_record.file_number);

  ERS_DEBUG(2,"Opening file: " << nameFile(UNFINISHED));

  m_cFile.clear();
  m_cFile.open(nameFile(UNFINISHED).c_str(),
	       ios::out | ios::app | ios::binary);

  m_check = ::adler32(0L, Z_NULL, 0);

  file_record(&m_file_start_record,&file_start_pattern);
  file_record(m_file_name_strings);

  this->replaceGuid();

  ERS_DEBUG(3,"Writing "<<m_fmdStrings.size()<<" metadata strings.");
  if(m_fmdStrings.size()>0) file_record(m_fmdStrings);
  file_record(&m_internal_run_parameters_record,&run_parameters_pattern);
  ERS_DEBUG(3,"Status of the file. good() returns "<<m_cFile.good());
  m_cFileOpen = m_cFile.good();
  m_openFailed = !m_cFile.good();
  
  if((m_callBack != NULL) && m_cFileOpen) {
    ERS_DEBUG(3,"Execute callback from DataWriter.");
    m_callBack->FileWasOpened(
			       m_filenamecallback->getCurrentFileName(),
			       nameFile(UNFINISHED),
			       m_streamType,
			       m_streamName,
			       m_applicationName,
			       m_guid,
			       m_internal_run_parameters_record.run_number,
			       m_file_start_record.file_number,
			       m_lumiBlockNumber
			       );
      }
}

string DataWriter::nameFile(const FileStatus& fs) const
{
  ostringstream n;
  n << m_writePath << "/";
  n << m_filenamecallback->getCurrentFileName((FINISHED == fs)?false:true);
  string name = n.str();
  return name;	    
}

void DataWriter::cd(const string& writingPath) {
  m_nextWritePath = writingPath;
  ERS_DEBUG(3,"Directory changed to "<<m_nextWritePath
	    <<" (effective for hext file).");
}

bool DataWriter::inTransition() const {

  bool inT=false;
  if((m_nextWritePath != "") && 
     (m_nextWritePath != m_writePath)) inT=true;
  return inT;

}

void DataWriter::file_record(void *ri, const void *pi) {

  uint32_t *record = reinterpret_cast<uint32_t *>(ri);
  const uint32_t *pattern = reinterpret_cast<const uint32_t *>(pi);
  int size=pattern[1];

  for(int i=0; i<size; i++) {
    if(pattern[i] != 0) {
      m_cFile.write((char *)(pattern+i),sizeof(uint32_t));
      m_check = ::adler32(m_check,(const Bytef*)(pattern+i),sizeof(uint32_t));
    } else {
      m_cFile.write((char *)(record+i),sizeof(uint32_t));
      m_check = ::adler32(m_check,(const Bytef*)(record+i),sizeof(uint32_t));
    }
  }
}


void DataWriter::file_record(const offline_EventStorage_v5::file_name_strings& nst) {
  
  const char *cName = nst.appName.c_str();
  const char *cTag = nst.fileNameCore.c_str();

  uint32_t sizeName =  nst.appName.size();
  uint32_t sizeTag =  nst.fileNameCore.size();

  m_cFile.write((char *)(&offline_EventStorage_v5::file_name_strings_marker),sizeof(uint32_t));
  m_check =  ::adler32(m_check,(const Bytef*)
		     (&offline_EventStorage_v5::file_name_strings_marker),
		     sizeof(uint32_t));

  m_cFile.write((char *)(&sizeName),sizeof(uint32_t));
  m_check =  ::adler32(m_check,(const Bytef*)
		     (&sizeName),
		     sizeof(uint32_t));
  m_cFile.write(cName,sizeName);
  m_check =  ::adler32(m_check,(const Bytef*)
		     (cName),
		     sizeName);

  char ns = sizeName % 4;
  if(ns){ 
    m_cFile.write("    ",4-ns);
    m_check = ::adler32(m_check,(const Bytef*)"    ",4-ns);
  }

  m_cFile.write((char *)(&sizeTag),sizeof(uint32_t));
  m_check = ::adler32(m_check,(const Bytef*)(&sizeTag),sizeof(uint32_t));
  m_cFile.write(cTag,sizeTag);
  m_check = ::adler32(m_check,(const Bytef*)cTag,sizeTag);

  ns = sizeTag % 4;
  if(ns){ 
    m_cFile.write("    ",4-ns);
    m_check = ::adler32(m_check,(const Bytef*)"    ",4-ns);
  }

}

void DataWriter::file_record(const offline_EventStorage_v5::freeMetaDataStrings& fmdStrings)
{
  ERS_DEBUG(2,"Writing the metadata strings.");

  m_cFile.write((char *)(&offline_EventStorage_v5::free_strings_marker),sizeof(uint32_t));
  m_check = ::adler32(m_check, (const Bytef*)
		    (&offline_EventStorage_v5::free_strings_marker),
		    sizeof(uint32_t));

  uint32_t nstrings = fmdStrings.size();
  m_cFile.write((char *)(&nstrings),sizeof(uint32_t));
  m_check = ::adler32(m_check, (const Bytef*)
		    (&nstrings),
		    sizeof(uint32_t));

  vector<string>::const_iterator it;
  for(it=fmdStrings.begin(); it!=fmdStrings.end(); ++it) {

    const char *cst = it->c_str();
    uint32_t slen =  it->size();
   
    m_cFile.write((char *)(&slen),sizeof(uint32_t));
    m_check = ::adler32(m_check,(const Bytef*)(&slen),sizeof(uint32_t));
    m_cFile.write(cst,slen);
    m_check = ::adler32(m_check,(const Bytef*)cst,slen);
    char ns = slen % 4;
    if(ns){ 
      m_cFile.write("    ",4-ns);
      m_check = ::adler32(m_check,(const Bytef*)"    ",4-ns);
    }
  }
}

void DataWriter::registerCallBack(DataWriterCallBack *pUserCBclass) {
  m_callBack = pUserCBclass;
  if(m_callBack != NULL) {
    m_callBack->FileWasOpened(
			       m_filenamecallback->getCurrentFileName(),
			       nameFile(UNFINISHED),
			       m_streamType,
			       m_streamName,
			       m_applicationName,
			       m_guid,
			       m_internal_run_parameters_record.run_number,
			       m_file_start_record.file_number,
			       m_lumiBlockNumber
			       );
  }
}

void DataWriter::spaceForGuid() {
  string e("");
  m_fmdStrings.insert(m_fmdStrings.begin(),e);
}

void DataWriter::replaceGuid() {
  using namespace std;
  
  if (m_next_guid == ""){
    offline_poolCopy_v5::Guid g(true);
    m_guid = g.toString();
  }else{
    m_guid = m_next_guid;
    m_next_guid = "";
  }
  
  string e1="GUID=" + m_guid;
  m_fmdStrings[0]=e1;
}

void DataWriter::setGuid(const std::string& Guid){
  m_next_guid = Guid;
}


uint64_t DataWriter::getFileSize(const std::string& fileNameCore)  const
{
  struct stat64 tmp;
  if (::stat64(fileNameCore.c_str(), &tmp) == -1) {
    EventStorage::WritingIssue ci(ERS_HERE, "stat64 failed.");
    ers::error(ci);
    return 0;
  }
  
  return tmp.st_size;
}


offline_EventStorage_v5::CompressionType DataWriter::getCompression() const
{
  return m_compression;
}
