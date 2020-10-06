/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMDATAWRITER_H
#define BYTESTREAMCNVSVC_BYTESTREAMDATAWRITER_H

/** @file ByteStreamDataWriter.h
 *  @brief This file contains the class definition for the ByteStreamDataWriter class.
 *  @author Andy Salnikov
 **/

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "EventStorage/DWError.h"
#include "EventStorage/ESCompression.h"
#include "EventStorage/EventStorageRecords.h"
#include "EventStorage/FileNameCallback.h"
#include "boost/shared_ptr.hpp"

struct DataWriterParameters;

/** @class ByteStreamDataWriter
 *  @brief This class defines abstract interface for data writing.
 *
 *  Main purpose of this class is to provide interface for writing different
 *  versions of output ByteStream files. This interface will be implemented
 *  on top of the EventStorage/DataWriter (from tdaq-common) and it will only
 *  provide few methods relevant to this package. The interface is defined in
 *  terms of data structures of the current version of the EventStoreage,
 *  implementations for older versions will need to translate those structures
 *  into their version-specific data.
 *
 *  In addition to abstract methods this class defines few factory methods
 *  which return instances implementing this interface.
 **/

class ByteStreamDataWriter {
public:

    /**
     *  Factory method returning data writer instance for specified version.
     *  Throws exception in case of problems, never returns zero pointer.
     *
     *  @param version: writer version to instantiate, 0 for most current version,
     *                  other supported values: 5 for run1-compatible output.
     *                  Exception is thrown if version is not supported.
     *  @param writingPath: directory where to write data
     *  @param fileNameCore: file name not including the ending "._NNNN.data"
     *  @param rPar: other parameters from IS including the run number
     *  @param fmdStrings: optional vector of strings containing metadata
     *  @param maxFileNE: max size of a file in number of data blocks (or events)
     *  @param maxFileMB: max size of a file in MB. The file is closed before this limit is overrun.
     *  @param startIndex: initial file sequence index
     *  @param compression: Compression type
     *  @param compLevel: Compression level
     *
     *  @throw std::exception (or sub-class) is thrown in case of problems.
     */
    static std::unique_ptr<ByteStreamDataWriter>
    makeWriter(int version,
               const std::string& writingPath,
               const std::string& fileNameCore,
               const EventStorage::run_parameters_record& rPar,
               const std::vector<std::string>& fmdStrings,
               unsigned int maxFileNE = 0,
               unsigned int maxFileMB = 0,
               unsigned int startIndex = 1,
               EventStorage::CompressionType compression = EventStorage::NONE,
               unsigned int compLevel = 1);

    /**
     *  Factory method returning data writer instance for specified version.
     *  Throws exception in case of problems, never returns zero pointer.
     *
     *  @param version: writer version to instantiate, 0 for most current version,
     *                  other supported values: 5 for run1-compatible output.
     *                  Exception is thrown is version is not supported.
     *  @param writingPath: directory where to write data
     *  @param theFNCB: callback method for generating file names
     *  @param rPar: other parameters from IS including the run number
     *  @param fmdStrings: optional vector of strings containing metadata
     *  @param maxFileNE: max size of a file in number of data blocks (or events)
     *  @param maxFileMB: max size of a file in MB. The file is closed before this limit is overrun.
     *  @param startIndex: initial file sequence index
     *  @param compression: Compression type
     *  @param compLevel: Compression level
     *
     *  @throw std::exception (or sub-class) is thrown in case of problems.
     */
    static std::unique_ptr<ByteStreamDataWriter>
    makeWriter(int version,
               const std::string& writingPath,
               boost::shared_ptr<EventStorage::FileNameCallback> theFNCB,
               const EventStorage::run_parameters_record& rPar,
               const std::string& project,
               const std::string& streamType,
               const std::string& streamName,
               const std::string& stream,
               unsigned int lumiBlockNumber,
               const std::string& applicationName,
               const std::vector<std::string>& fmdStrings,
               unsigned int maxFileNE = 0,
               unsigned int maxFileMB = 0,
               EventStorage::CompressionType compression = EventStorage::NONE,
               unsigned int compLevel = 1);

    /**
     *  Factory method returning data writer instance for specified version.
     *  Throws exception in case of problems, never returns zero pointer.
     *
     *  @param parameters: set of parameters needed to initiate DataWriter
     *
     *  @throw std::exception (or sub-class) is thrown in case of problems.
     */
    static std::unique_ptr<ByteStreamDataWriter>
        makeWriter(const DataWriterParameters& parameters);


    ByteStreamDataWriter() {}
    virtual ~ByteStreamDataWriter() {}

    // abstract class cannot be copied
    ByteStreamDataWriter(const ByteStreamDataWriter&) = delete;
    ByteStreamDataWriter& operator=(const ByteStreamDataWriter&) = delete;

    /**
        Write a single block of data.
        If one of the size limits would be exceeded, the current file
        will be closed and another one will be opened automatically.

        @param dataSize: size of the data block in bytes
        @param data: start of the event data in memory
    */
    virtual EventStorage::DWError putData(unsigned int dataSize, const void *data) = 0;

    /**
        Feedback to user, check if open and last write went OK
     */
     virtual bool good() const = 0;

};


/** Class containing parameters needed to initiayte DataWriter
 *
 *  This class is meant to given to the factory method creating data writer
 *  instances. The parameters need to be set to valid values.
 */
struct DataWriterParameters {
  /** Writer version to instantiate, 0 for most current version
   *
   *  Other supported values: 5 for run1-compatible output. Exception is thrown
   *  is version is not supported.
   **/
  int version{0};

  /// Directory where to write data
  std::string writingPath{""};

  /// File name not including the ending "._NNNN.data"
  std::string fileNameCore{""};

  /// Other parameters from IS including the run number
  EventStorage::run_parameters_record rPar{
    0, 0, 0, 0, 0, 0, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0, 0 };

  /// Optional vector of strings containing metadata
  std::vector<std::string> fmdStrings;

  /// Max size of a file in number of data blocks (or events)
  unsigned int maxFileNE{0};

  /// Max size of a file in MB. The file is closed before this limit is overrun
  unsigned int maxFileMB{0};

  /// Initial file sequence index
  unsigned int startIndex{1};

  /// Compression type
  EventStorage::CompressionType compression{EventStorage::NONE};

  /// Compression level
  unsigned int compLevel{1};

  /// callback method for generating file names
  boost::shared_ptr<EventStorage::FileNameCallback> theFNCB;

  /// project name for use with simple file name
  std::string project{""};

  /// stream type for use with simple file name
  std::string streamType{""};

  /// stream name for use with simple file name
  std::string streamName{""};

  /// not sure for use with simple file name
  std::string stream{""};
  
  /// luminosity block number for use with simple file name
  unsigned int lumiBlockNumber{0};

  /// application name for use with simple file name
  std::string applicationName{""};
};

#endif // BYTESTREAMCNVSVC_BYTESTREAMDATAWRITER_H
