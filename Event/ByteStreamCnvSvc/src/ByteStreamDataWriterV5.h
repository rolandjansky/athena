/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMDATAWRITERV5_H
#define BYTESTREAMCNVSVC_BYTESTREAMDATAWRITERV5_H

/** @file ByteStreamDataWriterV5.h
 *  @brief This file contains the class definition for the ByteStreamDataWriterV5 class.
 *  @author Andy Salnikov
 **/

#include "ByteStreamDataWriter.h"

namespace offline_EventStorage_v5 {
class DataWriter;
}

/** @class ByteStreamDataWriterV5
 *
 *  @brief Implementation of ByteStreamDataWriter for version 5 (run1) of EventStorage.
 */

class ByteStreamDataWriterV5: public ByteStreamDataWriter {
public:

    ByteStreamDataWriterV5(const std::string& writingPath,
                           const std::string& fileNameCore,
                           const EventStorage::run_parameters_record& rPar,
                           const std::vector<std::string>& fmdStrings,
                           unsigned int maxFileNE,
                           unsigned int maxFileMB,
                           unsigned int startIndex,
                           EventStorage::CompressionType compression,
                           unsigned int compLevel);

    ByteStreamDataWriterV5(const std::string& writingPath,
                           boost::shared_ptr<EventStorage::FileNameCallback> theFNCB,
                           const EventStorage::run_parameters_record& rPar,
                           const std::string& project,
                           const std::string& streamType,
                           const std::string& streamName,
                           const std::string& stream,
                           unsigned int lumiBlockNumber,
                           const std::string& applicationName,
                           const std::vector<std::string>& fmdStrings,
                           unsigned int maxFileNE,
                           unsigned int maxFileMB,
                           EventStorage::CompressionType compression,
                           unsigned int compLevel);

    virtual ~ByteStreamDataWriterV5();

    // cannot be copied
    ByteStreamDataWriterV5(const ByteStreamDataWriterV5&) = delete;
    ByteStreamDataWriterV5& operator=(const ByteStreamDataWriterV5&) = delete;

    virtual EventStorage::DWError putData(unsigned int dataSize, const void *data) override;

    virtual bool good() const override;

private:

    std::unique_ptr<offline_EventStorage_v5::DataWriter> m_writer;
};


#endif // BYTESTREAMCNVSVC_BYTESTREAMDATAWRITERV5_H
