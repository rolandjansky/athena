/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamDataWriterV5.h"

#include <stdexcept>

#include "ByteStreamCnvSvcLegacy/offline_EventStorage/v5_DataWriter.h"

namespace {

offline_EventStorage_v5::run_parameters_record
runPram_to_v5(const EventStorage::run_parameters_record& rPar)
{
    // convert current run_parameters_record to v5
    offline_EventStorage_v5::run_parameters_record rPar_v5 = {
        rPar.marker,
        rPar.record_size,
        rPar.run_number,
        rPar.max_events,
        rPar.rec_enable,
        rPar.trigger_type,
        rPar.detector_mask_LS,   /* v5 detector_mask = v6 detector_mask_LS, detector_mask_MS is lost */
        rPar.beam_type,
        rPar.beam_energy
    };
    return rPar_v5;
}

}


ByteStreamDataWriterV5::ByteStreamDataWriterV5(const std::string& writingPath,
                                               const std::string& fileNameCore,
                                               const EventStorage::run_parameters_record& rPar,
                                               const std::vector<std::string>& fmdStrings,
                                               unsigned int maxFileNE,
                                               unsigned int maxFileMB,
                                               unsigned int startIndex,
                                               EventStorage::CompressionType compression,
                                               unsigned int compLevel)
{
    // convert current run_parameters_record to v5
    offline_EventStorage_v5::run_parameters_record rPar_v5 = ::runPram_to_v5(rPar);

    // convert compression type
    offline_EventStorage_v5::CompressionType compression_v5 =
                    offline_EventStorage_v5::CompressionType(compression);

    m_writer.reset(new offline_EventStorage_v5::DataWriter(writingPath,
                                                           fileNameCore,
                                                           rPar_v5,
                                                           fmdStrings,
                                                           startIndex,
                                                           compression_v5,
                                                           compLevel));
    m_writer->setMaxFileNE(maxFileNE);
    m_writer->setMaxFileMB(maxFileMB);
}

ByteStreamDataWriterV5::ByteStreamDataWriterV5(const std::string& writingPath,
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
    // convert current run_parameters_record to v5
    offline_EventStorage_v5::run_parameters_record rPar_v5 = ::runPram_to_v5(rPar);

    // convert compression type
    offline_EventStorage_v5::CompressionType compression_v5 =
                    offline_EventStorage_v5::CompressionType(compression);

    m_writer.reset(new offline_EventStorage_v5::DataWriter(writingPath,
                                                           theFNCB,
                                                           rPar_v5,
                                                           project,
                                                           streamType,
                                                           streamName,
                                                           stream,
                                                           lumiBlockNumber,
                                                           applicationName,
                                                           fmdStrings,
                                                           compression_v5,
                                                           compLevel));
    m_writer->setMaxFileNE(maxFileNE);
    m_writer->setMaxFileMB(maxFileMB);
}

ByteStreamDataWriterV5::~ByteStreamDataWriterV5()
{
}

EventStorage::DWError
ByteStreamDataWriterV5::putData(unsigned int dataSize, const void *data)
{
    return m_writer->putData(dataSize, data);
}

bool
ByteStreamDataWriterV5::good() const
{
    return m_writer->good();
}
