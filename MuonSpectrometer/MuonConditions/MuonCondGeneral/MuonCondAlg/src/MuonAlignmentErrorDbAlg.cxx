/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/MuonAlignmentErrorDbAlg.h"

MuonAlignmentErrorDbAlg::MuonAlignmentErrorDbAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator), m_condSvc{"CondSvc", name} {}

StatusCode MuonAlignmentErrorDbAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());
    ATH_CHECK(m_condSvc.retrieve());
    ATH_CHECK(m_readKey.initialize());
    ATH_CHECK(m_writeKey.initialize());
    if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
        ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentErrorDbAlg::execute() {
    ATH_MSG_DEBUG("execute " << name());

    // Write Cond Handle

    SG::WriteCondHandle<MuonAlignmentErrorData> writeHandle{m_writeKey};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << ". In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }
    std::unique_ptr<MuonAlignmentErrorData> writeCdo{std::make_unique<MuonAlignmentErrorData>()};

    // Read Cond Handle

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
    const CondAttrListCollection* readCdo{*readHandle};
    // const CondAttrListCollection* atrc(0);
    // readCdo = *readHandle;
    if (readCdo == nullptr) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return StatusCode::FAILURE;
    }

    EventIDRange rangeW;
    if (!readHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeW);

    // like MuonAlignmentErrorDbTool::loadAlignmentError() after retrieving atrc (readCdo)

    std::string errorlistValue;
    CondAttrListCollection::const_iterator itr;
    for (itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
        const coral::AttributeList& atr = itr->second;
        std::string list_error, version;
        list_error = *(static_cast<const std::string*>((atr["syserrors"]).addressOfData()));
        version = *(static_cast<const std::string*>((atr["version"]).addressOfData()));
        errorlistValue = list_error;
    }

    // like AlignmentErrorTool::initializeAlignmentDeviationsList()

    std::istringstream indata(errorlistValue);
    if (!indata) {
        ATH_MSG_ERROR("Alignment error configuration invalid");
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("***********************************");
    ATH_MSG_DEBUG("PARSING LIST OF DEVIATIONS...");

    std::string line;
    while (getline(indata, line)) {
        // READING COMMENTS
        if (line.compare(0, 1,"#") == 0) {
            // ATH_MSG_DEBUG("Reading a commented line saying " << line);
            continue;
        }

        // READING FROM INPUT FILE:                                //
        std::string flag("");
        double version_tag = -1.;
        std::string name_sstring("");
        std::string multilayer_sstring("");
        double traslation(0.);
        double rotation(0.);

        // GET INPUT FILE VERSION
        if (line.compare(0, 7, "version") == 0) {
            std::istringstream(line) >> flag >> version_tag;
            ATH_MSG_INFO("*****************************************");
            ATH_MSG_INFO("Input file version " << version_tag);
            ATH_MSG_INFO("*****************************************");
            continue;
        }

        // A FLAG CHARACTERIZING THE TYPE OF ERROR
        // A REGULAR EXPRESSION FOR THE STATION NAME (EX: BIL.*)   //
        // TWO DOUBLES FOR THE TRANSLATION AND ROTATION DEVIATIONS //
        if (std::istringstream(line) >> flag >> name_sstring >> multilayer_sstring >> traslation >> rotation) {
            ATH_MSG_DEBUG(flag << " " << name_sstring << " " << multilayer_sstring << " " << traslation << " " << rotation);

            // TEMPORARY PER STATION DEVIATION STRUCT //
            deviationStr tmpStr = deviationStr();

            // SAVING A REGULAR EXPRESSION WITH THE STATION NAME //
            const boost::regex name_regexp(name_sstring);

            // SAVING THE STATION DEVIATIONS IN THE STRUCT //
            tmpStr.stationName = name_regexp;
            tmpStr.multilayer = multilayer_sstring;
            tmpStr.traslation = traslation;
            tmpStr.rotation = rotation;

            // FILLING THE VECTOR OF STRUCT CONTAINING THE STATION DEVIATIONS //
            writeCdo->m_vec.push_back(tmpStr);

        }  // check stream is not at the end

    }  // end of loop on input file lines

    if (writeCdo->m_vec.empty()) {
        ATH_MSG_WARNING("Could not read any alignment error configuration");
        return StatusCode::FAILURE;
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
        ATH_MSG_FATAL("Could not record MuonAlignmentErrorData " << writeHandle.key() << " with EventRange " << rangeW
                                                                 << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentErrorDbAlg::finalize() {
    ATH_MSG_DEBUG("finalize " << name());
    return StatusCode::SUCCESS;
}
