/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamDataWriter.h"

#include <stdexcept>

#include "ByteStreamDataWriterV5.h"
#include "EventStorage/DataWriter.h"

namespace {

void versionException(int version) {
    std::string msg = "Unexpected version number in ByteStreamDataWriter::makeWriter: " +
            std::to_string(version);
    throw std::invalid_argument(msg);
}

/*
 * Implementation of ByteStreamDataWriter for current version of EventStorage.
 */
class ByteStreamDataWriterCurrent: public ByteStreamDataWriter {
public:

    ByteStreamDataWriterCurrent(const std::string& writingPath,
                                const std::string& fileNameCore,
                                const EventStorage::run_parameters_record& rPar,
                                const std::vector<std::string>& fmdStrings,
                                unsigned int maxFileNE,
                                unsigned int maxFileMB,
                                unsigned int startIndex,
                                EventStorage::CompressionType compression,
                                unsigned int compLevel);

    ByteStreamDataWriterCurrent(const std::string& writingPath,
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

    virtual ~ByteStreamDataWriterCurrent() {}

    // abstract class cannot be copied
    ByteStreamDataWriterCurrent(const ByteStreamDataWriterCurrent&) = delete;
    ByteStreamDataWriterCurrent& operator=(const ByteStreamDataWriterCurrent&) = delete;

    virtual EventStorage::DWError putData(unsigned int dataSize, const void *data) override;

    virtual bool good() const override;

private:

    std::unique_ptr<EventStorage::DataWriter> m_writer;
};

} // namespace

std::unique_ptr<ByteStreamDataWriter>
ByteStreamDataWriter::makeWriter(int version,
                                 const std::string& writingPath,
                                 const std::string& fileNameCore,
                                 const EventStorage::run_parameters_record& rPar,
                                 const std::vector<std::string>& fmdStrings,
                                 unsigned int maxFileNE,
                                 unsigned int maxFileMB,
                                 unsigned int startIndex,
                                 EventStorage::CompressionType compression,
                                 unsigned int compLevel)
{
    ByteStreamDataWriter* res = nullptr;
    if (version == 0 or version == FILE_FORMAT_VERSION) {
        // Use current version
        res = new ByteStreamDataWriterCurrent(writingPath,
                                              fileNameCore,
                                              rPar,
                                              fmdStrings,
                                              maxFileNE,
                                              maxFileMB,
                                              startIndex,
                                              compression,
                                              compLevel);
    } else if (version == 5) {
        // Run1 version
        res = new ByteStreamDataWriterV5(writingPath,
                                         fileNameCore,
                                         rPar,
                                         fmdStrings,
                                         maxFileNE,
                                         maxFileMB,
                                         startIndex,
                                         compression,
                                         compLevel);
    } else {
        // Unexpected something
        versionException(version);
    }

    return std::unique_ptr<ByteStreamDataWriter>(res);
}

std::unique_ptr<ByteStreamDataWriter>
ByteStreamDataWriter::makeWriter(int version,
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
                                 unsigned int maxFileNE,
                                 unsigned int maxFileMB,
                                 EventStorage::CompressionType compression,
                                 unsigned int compLevel)
{
    ByteStreamDataWriter* res = nullptr;
    if (version == 0 or version == FILE_FORMAT_VERSION) {
        // Use current version
        res = new ByteStreamDataWriterCurrent(writingPath,
                                              theFNCB,
                                              rPar,
                                              project,
                                              streamType,
                                              streamName,
                                              stream,
                                              lumiBlockNumber,
                                              applicationName,
                                              fmdStrings,
                                              maxFileNE,
                                              maxFileMB,
                                              compression,
                                              compLevel);
    } else if (version == 5) {
        // Run1 version
        res = new ByteStreamDataWriterV5(writingPath,
                                         theFNCB,
                                         rPar,
                                         project,
                                         streamType,
                                         streamName,
                                         stream,
                                         lumiBlockNumber,
                                         applicationName,
                                         fmdStrings,
                                         maxFileNE,
                                         maxFileMB,
                                         compression,
                                         compLevel);
    } else {
        // Unexpected something
        versionException(version);
    }

    return std::unique_ptr<ByteStreamDataWriter>(res);
}

std::unique_ptr<ByteStreamDataWriter>
ByteStreamDataWriter::makeWriter(const DataWriterParameters& parameters) {
  // initiate with production file name
  if (parameters.theFNCB == nullptr)
    return makeWriter(
      parameters.version,
      parameters.writingPath,
      parameters.fileNameCore,
      parameters.rPar,
      parameters.fmdStrings,
      parameters.maxFileNE,
      parameters.maxFileMB,
      parameters.startIndex,
      parameters.compression,
      parameters.compLevel);

  // initiate for user-defined file name
  return makeWriter(
      parameters.version,
      parameters.writingPath,
      parameters.theFNCB,
      parameters.rPar,
      parameters.project,
      parameters.streamType,
      parameters.streamName,
      parameters.stream,
      parameters.lumiBlockNumber,
      parameters.applicationName,
      parameters.fmdStrings,
      parameters.maxFileNE,
      parameters.maxFileMB,
      parameters.compression,
      parameters.compLevel);
}

namespace {

ByteStreamDataWriterCurrent::ByteStreamDataWriterCurrent(const std::string& writingPath,
                                                         const std::string& fileNameCore,
                                                         const EventStorage::run_parameters_record& rPar,
                                                         const std::vector<std::string>& fmdStrings,
                                                         unsigned int maxFileNE,
                                                         unsigned int maxFileMB,
                                                         unsigned int startIndex,
                                                         EventStorage::CompressionType compression,
                                                         unsigned int compLevel)
{
  m_writer.reset(new EventStorage::DataWriter(writingPath,
                                              fileNameCore,
                                              rPar,
                                              fmdStrings,
                                              startIndex,
                                              compression,
                                              compLevel));
  m_writer->setMaxFileNE(maxFileNE);
  m_writer->setMaxFileMB(maxFileMB);
}

ByteStreamDataWriterCurrent::ByteStreamDataWriterCurrent(const std::string& writingPath,
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
                                                         unsigned int compLevel)
{
  m_writer.reset(new EventStorage::DataWriter(writingPath,
                                              theFNCB,
                                              rPar,
                                              project,
                                              streamType,
                                              streamName,
                                              stream,
                                              lumiBlockNumber,
                                              applicationName,
                                              fmdStrings,
                                              compression,
                                              compLevel));
  m_writer->setMaxFileNE(maxFileNE);
  m_writer->setMaxFileMB(maxFileMB);
}

EventStorage::DWError
ByteStreamDataWriterCurrent::putData(unsigned int dataSize, const void *data)
{
  return m_writer->putData(dataSize, data);
}

bool
ByteStreamDataWriterCurrent::good() const
{
  return m_writer->good();
}

} // namespace
