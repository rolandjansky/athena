/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
    \brief The API for writing of event data in ATLAS TDAQ applications.
    \author Szymon Gadomski
    \date May 2002 - Feb 2007
    \file EventStorage/EnhancedDataWriter.h

    The data is stored in a sequence of disk files.
    A maximal size of a single file can be given either in number of events
    or in MB. 
*/


#ifndef OFFLINE_EVENTSTORAGE_V5_DATAWRITER_H
#define OFFLINE_EVENTSTORAGE_V5_DATAWRITER_H

#include <string>
#include <fstream>
#include <sys/uio.h>
#include <stdint.h>
#include <zlib.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "EventStorage/DWError.h"
#include "ByteStreamCnvSvcLegacy/offline_EventStorage/v5_EventStorageRecords.h"
#include "EventStorage/FileNameCallback.h"
#include "EventStorage/RawFileName.h"


namespace EventStorage {
  class DataWriterCallBack;
}

namespace offline_EventStorage_v5 {
  
  class DataBuffer;

  enum FileStatus { FINISHED, UNFINISHED };

class DataWriter : boost::noncopyable
{
public:
  using iovec_const = offline_EventStorage_v5::iovec_const;

  /** 
      constructor with metadata strings 
      \param writingPath directory where to write data  
      \param fileNameCore file name not including the ending "._NNNN.data"
      \param run_parameters_record rPar other parameters from IS including the run number 
      \param fmdStrings optional vector of strings containing metadata 
      \param startIndex initial file sequence index
      \param compression Compression type
      \param compLevel   Compression level
   */
  DataWriter(const std::string writingPath,     
	     const std::string fileNameCore,    
	     const run_parameters_record rPar, 
	     const std::vector<std::string> fmdStrings, 
	     const unsigned int startIndex = 1,
	     const CompressionType compression = NONE,
	     const unsigned int compLevel = 1); 



  DataWriter(const std::string writingPath,   
	     boost::shared_ptr<EventStorage::FileNameCallback> theFNCB,
	     const run_parameters_record rPar, 
	     const std::string project,
	     const std::string streamType,
	     const std::string streamName,
	     const std::string stream,
	     const unsigned int lumiBlockNumber,
	     const std::string applicationName,
	     const std::vector<std::string> fmdStrings,
	     const CompressionType compression = NONE,
	     const unsigned int compLevel = 1);

  DataWriter(const std::string writingPath,   
	     const std::string fileNameCore,     
	     const run_parameters_record rPar, 
	     const std::string project,
	     const std::string streamType,
	     const std::string streamName,
	     const std::string stream,
	     const unsigned int lumiBlockNumber,
	     const std::string applicationName,
	     const std::vector<std::string> fmdStrings,
	     const CompressionType compression = NONE,
	     const unsigned int compLevel = 1);


private:
  DataWriter();


  void initDW(const std::string writingPath,   
	      boost::shared_ptr<EventStorage::FileNameCallback> theFNCB,
	      const run_parameters_record rPar, 
	      const std::string project,
	      const std::string streamType,
	      const std::string streamName,
	      const std::string stream,
	      const unsigned int lumiBlockNumber,
	      const std::string applicationName,
	      const std::vector<std::string> fmdStrings,
	      const CompressionType compression,
	      const unsigned int compLevel);


  EventStorage::DWError putData_implementation(const unsigned int& entries,
				 const iovec_const * my_iovec, 
				 uint32_t& sizeToDisk,
				 bool precompressed = false);
  
public:
  /** destructor,
      the way to close the sequence of files
  */
  ~DataWriter();
        
  /**
     max size of 1 file in number of data blocks (or events)
     1 putData call is one block
  */
  EventStorage::DWError setMaxFileNE(const unsigned int& maxFileNE);

  /**
     max size of 1 file in MB. The file is closed before this limit is overrun.
  */ 
  EventStorage::DWError setMaxFileMB(const unsigned int& maxFileMB);
  
  /**
    Set the GUID for the _next_ file in this sequence
  */
  void setGUID(const std::string& GUID);
  

  /**
     feedback to user
  */
  bool good() const; // check if open and last write went OK

  /** 
      set another writing path
  */
  void cd(const std::string& writingPath);

  /** 
      The file currently open is not closed immediately after cd()
      this method will return true if we are in transition phase,
      i.e. the file has not yet been closed after a cd()
  */
  bool inTransition() const;

  /** 
      Write a single block of data.
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param dataSize size of the event in bytes
      \param *event start of the event data in memory
  */
  EventStorage::DWError putData(const unsigned int& dataSize, const void *event);


  /** 
      Write a single block of data already compressed. 
      *WARNING* the library has now way to check if the compression
      algorithm matches the declared file type. The data is 
      directly written to the file without check, completely trusting
      the caller. Use with care!
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param dataSize size of the eveny in bytes
      \param *event start of the event data in memory
  */
  EventStorage::DWError putPrecompressedData(const unsigned int& dataSize,
			       const void *event);

 /** 
      Write a single block of data already compressed. 
      *WARNING* the library has now way to check if the compression
      algorithm matches the declared file type. The data is 
      directly written to the file without check, completely trusting
      the caller. Use with care!
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param number of the data chunks
      \param locations of the data chunks
  */
  EventStorage::DWError putPrecompressedData(const unsigned int& entries,
			       const iovec_const* my_iovec);


  /** 
      Write data when you have one event scattered in many blocks, 
      in memory, like in the SFI application.
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param number of the data chunks
      \param locations of the data chunks
  */
  EventStorage::DWError putData(const unsigned int& entries, const struct iovec* my_iovec);


    /** 
      Write a single block of data.
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param dataSize size of the eveny in bytes
      \param *event start of the event data in memory
      \param &sizeToDisk will be update with the actual chunck size on disk
  */
  EventStorage::DWError putData(const unsigned int& dataSize, const void *event, uint32_t& sizeToDisk);

  /** 
      Write data when you have one event scattered in many blocks, 
      in memory, like in the SFI application.
      If one of the size limits would be exceeded, the current file
      will be closed and another one will be opened automatically.
      \param entries number of the data chunks
      \param my_iovec locations of the data chunks
      \param &sizeToDisk will be update with the actual chunck size on disk
  */
  EventStorage::DWError putData(const unsigned int& entries, const iovec_const* my_iovec,
		  uint32_t& sizeToDisk);


  /**
     Close the currently open file. 
     Next file will be open anyway if you try to write again afterwards.
  */
  EventStorage::DWError closeFile();

  /** 
      close the file and open next one immediately
  */
  EventStorage::DWError nextFile();

  /**
     get the name of the current file
  */
  std::string nameFile(const FileStatus& fs) const;

  /** 
      get the offset of the latest written event;
      one should call it AFTER putData(...)
  */
  int64_t getPosition() const;

  /** number of events written to the currently open file */
  unsigned int eventsInFile() const; 

  /** number of events written to the file sequence */
  unsigned int eventsInFileSequence() const;
 
  /** number of mega bytes written to the current file */
  unsigned int dataMB_InFile() const; 

  /** number of mega bytes written to the file sequence */
  unsigned int dataMB_InFileSequence() const; 
 
  /** If you want to define actions to be taken when a file is 
      open or closed register your call-back class here with this
      method. */
  void registerCallBack(EventStorage::DataWriterCallBack *pUserCBclass);

  /** Returns the compression type for this file
   */
  CompressionType getCompression() const;

  /** Externally set the GUID for the _next_ file in this sequence
   */
  void setGuid(const std::string& Guid);

 private:
  // date and time utility, other private methods
  void date_timeAsInt(uint32_t &getDate, uint32_t &getTime) const;
  void openNextFile();
  bool fileExists(const std::string& name) const;
  void setRunParamsRecord(const run_parameters_record& rPar);

  //methods to write records to files
  void file_record(void *ri, const void *pi);
  void file_record(const file_name_strings& nst);
  void file_record(const freeMetaDataStrings& fmdStrings);

  // some parsing is needed to fill metadata
  std::string getAppName(const std::string& fileNameCore) const;
  std::string getElement(const std::string& fileNameCore, 
			 const std::string& element) const;
  uint64_t getFileSize(const std::string& fileNameCore) const;

  //methods to add the Global Unique ID to the files
  void spaceForGuid();
  void replaceGuid();
 
 private: //data members


  std::fstream m_cFile;   // current file

  bool m_cFileOpen;        // Is a file open at the moment?
  bool m_openFailed;       // Has the lest open operation failed?
    
  file_start_record m_file_start_record;          // parameters of the current file
  file_name_strings m_file_name_strings;          // two significant strings in file name 
  internal_run_parameters_record m_internal_run_parameters_record;  // parameters of the run
  file_end_record m_file_end_record;              // more parameters of the file

  std::string m_guid; //the GUID
  std::string m_next_guid;

  std::string m_writePath;       // path to write
  std::string m_nextWritePath;   // next path to write
  std::string m_fileNameCore;    // most of the file name 

  std::string        m_project;           // fields needed at some point
  std::string        m_streamType;        //
  std::string        m_streamName;        //
  std::string        m_stream;
  unsigned int       m_lumiBlockNumber;
  std::string        m_applicationName;

  CompressionType    m_compression;
  unsigned int       m_complevel;
  DataBuffer         *m_compressed;
  
  double             m_cFileMB; // megabytes so far in the file, for adding up
  long double        m_runMB;   // megabytes so far in the run
  uLong              m_check;   //running checksum

  int64_t            m_latestPosition; // offset of the latest written event

  EventStorage::DataWriterCallBack *m_callBack; // pointer to call-back, for reporting file names

  freeMetaDataStrings m_fmdStrings; // optional vector of strings, not written out if empty  

  boost::shared_ptr<EventStorage::FileNameCallback> m_filenamecallback;
};

}
#endif // OFFLINE_EVENTSTORAGE_V5_DATAWRITER_H




