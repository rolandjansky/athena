/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_FrontEnd.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandPoisson.h"

#include "SiDigitization/SiHelper.h"

#include "InDetIdentifier/SCT_ID.h"

#include "SCT_Digitization/ISCT_Amp.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

#include <algorithm>

// Calibration Cond DB data
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h"

// #define SCT_DIG_DEBUG


#include <iostream>
using namespace std;
// ---------------

using namespace InDetDD;

// constructor
SCT_FrontEnd::SCT_FrontEnd(const std::string &type, const std::string &name,
                           const IInterface *parent)
    : AthAlgTool(type, name, parent),
    m_sc(0),
    m_SCTdetMgr(0),
    m_sct_id(0),
    m_sct_amplifier("SCT_Amp"),
    m_ReadCalibChipDataSvc("SCT_ReadCalibChipDataSvc", name) {
    declareInterface< ISCT_FrontEnd >(this);

    declareProperty("NoiseBarrel", m_NoiseBarrel = 1500.0, "NoiseBarrel");
    declareProperty("NoiseBarrel3", m_NoiseBarrel3 = 1541.0, "NoiseBarrel3");
    declareProperty("NoiseInners", m_NoiseInners = 1090.0, "NoiseInners");
    declareProperty("NoiseMiddles", m_NoiseMiddles = 1557.0, "NoiseMiddles");
    declareProperty("NoiseShortMiddles", m_NoiseShortMiddles = 940.0,
                    "NoiseShortMiddles");
    declareProperty("NoiseOuters", m_NoiseOuters = 1618.0, "NoiseOuters");
    declareProperty("NOBarrel", m_NOBarrel = 1.5e-5, "NoiseOccupancyBarrel");
    declareProperty("NOBarrel3", m_NOBarrel3 = 2.1e-5, "NoiseOccupancyBarrel3");
    declareProperty("NOInners", m_NOInners = 5.0e-9, "NoiseOccupancyInners");
    declareProperty("NOMiddles", m_NOMiddles = 2.7e-5, "NoiseOccupancyMiddles");
    declareProperty("NOShortMiddles", m_NOShortMiddles = 2.0e-9,
                    "NoiseOccupancyShortMiddles");
    declareProperty("NOOuters", m_NOOuters = 3.5e-5, "NoiseOccupancyOuters");
    declareProperty("NoiseOn", m_NoiseOn = true,
                    "To know if noise is on or off when using calibration data");
    declareProperty("GainRMS", m_GainRMS = 0.031,
                    "Gain spread parameter within the strips for a given Chip gain");
    declareProperty("Ospread", m_Ospread = 0.0001,
                    "offset spread within the strips for a given Chip offset");
    declareProperty("OffsetGainCorrelation", m_OGcorr = 0.00001,
                    "Gain/offset correlation for the strips");
    declareProperty("Threshold", m_Threshold = 1.0, "Threshold");
    declareProperty("TimeOfThreshold", m_timeOfThreshold = 30.0,
                    "Threshold time");
    declareProperty("PulseAveragingFlag", m_PulseAveragingFlag = false,
                    "Flag to set the Pulse Averaging");
    declareProperty("DataCompressionMode", m_data_compression_mode = 1,
                    "Front End Data Compression Mode");
    declareProperty("NoiseExpandedMode", m_noise_expanded_mode = false,
                    "Front End Noise Expanded Mode");
    declareProperty("UseCalibData", m_useCalibData = true,
                    "Flag to use Calib Data");
    declareProperty("MaxStripsPerSide", m_strip_max = 768, "For SLHC studies");
    declareProperty("SCT_ReadCalibChipDataSvc", m_ReadCalibChipDataSvc);
    declareProperty("SCT_Amp", m_sct_amplifier);
}

// Destructor:
SCT_FrontEnd::~SCT_FrontEnd() {
}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::initialize() {
    m_sc = AthAlgTool::initialize();
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("SCT_FrontEnd::initialize() failed");
        return m_sc;
    }
    ATH_MSG_INFO("SCT_FrontEnd::initialize()");

    // Get SCT helper
    m_sc = detStore()->retrieve(m_sct_id, "SCT_ID");
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("Failed to get SCT ID helper");
        return m_sc;
    }

    // Get SCT detector manager
    m_sc = detStore()->retrieve(m_SCTdetMgr, "SCT");
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("Failed to get SCT detector manager");
        return m_sc;
    }

    // Get the amplifier tool
    m_sc = m_sct_amplifier.retrieve();
    if (m_sc.isFailure()) {
        ATH_MSG_ERROR("\tCan't retrieve SCT Amplifier tool " <<
            m_sct_amplifier);
        return m_sc;
    }
    else {
        ATH_MSG_DEBUG("SCT Amplifier tool located ");
    }

    // Get the SCT_ReadCaliDataSvc
    if (m_useCalibData) {
        m_sc = m_ReadCalibChipDataSvc.retrieve();
        if (m_sc.isFailure()) {
            ATH_MSG_FATAL("Cannot locate CalibChipData service");
            return m_sc;
        }
        else {
            ATH_MSG_DEBUG("CalibChipData Service located ");
        }
    }

    // Get the maximum number of strips of any module
    m_strip_max = m_SCTdetMgr->numerology().maxNumStrips();

    // Allocate the appropiate memory for vectors
    m_sc = initVectors(m_strip_max, m_data_compression_mode);
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL(
            "Can't setup class variable vectors, compression mode has to be 1 or 2");
        return m_sc;
    }
    else {
        ATH_MSG_DEBUG("Class variable vectors allocated");
    }


    const float fC = 6242.2;
    m_Threshold = m_Threshold * fC;


#ifdef SCT_DIG_DEBUG
    ATH_MSG_INFO("\tNoise factors:");
    ATH_MSG_INFO("\tBarrel = " << m_NoiseBarrel << " Outer Barrel = " <<
        m_NoiseBarrel3 << " EC, inners = " << m_NoiseInners << " EC, middles = "
                               << m_NoiseMiddles << " EC, short middles = " <<
        m_NoiseShortMiddles << " EC, outers = " << m_NoiseOuters);
    ATH_MSG_INFO("\tPerforming averaging " << (m_PulseAveragingFlag           ?
                                               "TRUE" : "FALSE"));
    ATH_MSG_INFO("\tThreshold=" << m_Threshold << " fC, time of Threshold=" <<
        m_timeOfThreshold);
#endif

    return m_sc;
}

StatusCode SCT_FrontEnd::finalize() {
    m_sc = AthAlgTool::finalize();
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("SCT_FrontEnd::finalize() failed");
        return m_sc;
    }
#ifdef SCT_DIG_DEBUG
    ATH_MSG_INFO("SCT_FrontEnd::finalize()");
#endif
    return m_sc;
}

// ----------------------------------------------------------------------
// Init the class variable  vectors
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::initVectors(const int &strips, const
                                     short &comp_mode) const {
    m_Offset.assign(strips, 0.0);
    m_GainFactor.assign(strips, 0.0);
    m_NoiseFactor.assign(strips, 0.0);

    if (comp_mode == 1) {
        m_Analogue[0].reserve(1);
        m_Analogue[1].reserve(strips);
    }
    else if (comp_mode == 2) {
        m_Analogue[0].reserve(strips);
        m_Analogue[1].reserve(strips);
    }
    else if (comp_mode == 3) {
        m_Analogue[0].reserve(strips);
        m_Analogue[1].reserve(strips);
        m_Analogue[2].reserve(strips);
    }
    else {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::prepareGainAndOffset(
    SiChargedDiodeCollection &collection, const Identifier &moduleId) const {
    // now we need to generate gain and offset channel by channel: some algebra
    // for generation of partially correlated random numbers
    float W = m_OGcorr * m_GainRMS * m_Ospread / (m_GainRMS * m_GainRMS -
                                                  m_Ospread * m_Ospread);
    float A = 4 * W * W + 1.0;
    float x1 = (A - sqrt(A)) / (2.0 * A);
    float sinfi = sqrt(x1);
    float cosfi = sqrt(1.0 - x1);

    sinfi = sinfi * m_OGcorr / fabs(m_OGcorr);
    float S = m_GainRMS * m_GainRMS + m_Ospread * m_Ospread;
    float D = (m_GainRMS * m_GainRMS - m_Ospread * m_Ospread) / (cosfi * cosfi -
                                                                 sinfi * sinfi);
    float S1 = sqrt((S + D) / 2.0);
    float S2 = sqrt((S - D) / 2.0);
    float Noise = 0;
    int mode = 1;

    // To set noise values for different module types, barrel, EC, inners,
    // middles,
    // short middles, and outers
    if (m_NoiseOn) {
        if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to
                                                  // barrel
            if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10
                                                       // degrees warmer
                Noise = m_NoiseBarrel3;
            }
            else {
                Noise = m_NoiseBarrel;
            }
        }
        else {
            int moduleType = m_sct_id->eta_module(moduleId);
            switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers,
                                  // middles and inners?! (at least in the
                                  // offline world)
            case 0:
            {
                Noise = m_NoiseOuters;
                break;
            }

            case 1:
            {
                if (m_sct_id->layer_disk(moduleId) == 7) {
                    Noise = m_NoiseShortMiddles;
                }
                else {
                    Noise = m_NoiseMiddles;
                }
                break;
            }

            case 2:
            {
                Noise = m_NoiseInners;
                break;
            }

            default:
            {
                Noise = m_NoiseBarrel;
                ATH_MSG_ERROR("moduleType(eta): " << moduleType <<
                    " unknown, using barrel");
            }
            }// end of switch structure
        }
    }

    // Loop over collection and setup gain/offset/noise for the hit and
    // neighbouring strips
    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();

    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();
                int i = std::max(strip - 1, 0);
                int i_end = std::min(strip + 2, m_strip_max);

                // loop over strips
                for (; i < i_end; i++) {
                    // Need to check if strip is already setup
                    if (m_Analogue[1][i] <= 0.0) {
                        float g = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0, S1);
                        float o = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0, S2);

                        m_GainFactor[i] = 1.0 + (cosfi * g + sinfi * o);
                        m_Offset[i] = (cosfi * o - sinfi * g);
                        m_NoiseFactor[i] = Noise * mode;

                        // Fill the noise and offset values into the Analogue
                        if (m_data_compression_mode == 1) {
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else if (m_data_compression_mode == 2) { // edge mode
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else if (m_data_compression_mode == 3) { // any hit mode
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[2][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module using
// Cond Db data to get the chip calibration data
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::prepareGainAndOffset(
    SiChargedDiodeCollection &collection, const int &side, const
    Identifier &moduleId) const {
    // Get chip data from calib DB
    std::vector<float> gainByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(
        moduleId, side, "GainByChip");
    std::vector<float> gainRMSByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side, "GainRMSByChip");
    std::vector<float> offsetByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side, "OffsetByChip");
    std::vector<float> offsetRMSByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                               "OffsetRMSByChip");
    std::vector<float> noiseByChipVect(6, 0.0);

    if (m_NoiseOn) { // Check if noise should be on or off
        noiseByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                                                 "NoiseByChip");
    }

    // Need to check if empty, most should have data, but a few old DEAD modules
    // don't
    if (gainByChipVect.empty() || noiseByChipVect.empty()) {
        ATH_MSG_DEBUG("No calibration data in cond DB for module " <<
            moduleId << " using JO values");
        if (StatusCode::SUCCESS != prepareGainAndOffset(collection, moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }

    // Don't really need to set up values for each chip...
    float gainMeanValue = meanValue(gainByChipVect);
    if (gainMeanValue < 0.0) {
        ATH_MSG_DEBUG("All chip gain values are 0 for module " << moduleId <<
            " using JO values");
        if (StatusCode::SUCCESS != prepareGainAndOffset(collection, moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }

    std::vector<float> gain(6, 0.0);
    std::vector<float> offset(6, 0.0);
    std::vector<float> S1(6, 0.0);
    std::vector<float> S2(6, 0.0);
    std::vector<float> sinfi(6, 0.0);
    std::vector<float> cosfi(6, 0.0);
    float gainRMS = 0.0;
    float offsetRMS = 0.0;

    for (int i = 0; i < 6; ++i) {
        // Some very few chips have 0 values, dead/bypassed/etc, so check and
        // use some fixed values instead
        if (gainByChipVect[i] > 0.1) {
            gain[i] = gainByChipVect[i] / gainMeanValue;
            offset[i] = offsetByChipVect[i] / m_Threshold;
            gainRMS = gainRMSByChipVect[i] / gainMeanValue;
            offsetRMS = offsetRMSByChipVect[i] / m_Threshold;
        }
        else {
            gain[i] = 55.0 / gainMeanValue;
            offset[i] = 42.0 / m_Threshold;
            gainRMS = 1.3 / gainMeanValue;
            offsetRMS = 2.0 / m_Threshold;
        }

        float W = m_OGcorr * gainRMS * offsetRMS / (gainRMS * gainRMS -
                                                    offsetRMS * offsetRMS);
        float A = 4 * W * W + 1.0;
        float x1 = (A - sqrt(A)) / (2.0 * A);
        sinfi[i] = sqrt(x1);
        cosfi[i] = sqrt(1.0 - x1);
        sinfi[i] = sinfi[i] * m_OGcorr / fabs(m_OGcorr);
        float S = gainRMS * gainRMS + offsetRMS * offsetRMS;
        float D = (gainRMS * gainRMS - offsetRMS * offsetRMS) / (cosfi[i] *
                                                                 cosfi[i] -
                                                                 sinfi[i] *
                                                                 sinfi[i]);
        S1[i] = sqrt((S + D) / 2.0);
        S2[i] = sqrt((S - D) / 2.0);
    }

    // Loop over collection and setup gain/offset/noise for the hit and
    // neighbouring strips
    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();

    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();
                int i = std::max(strip - 1, 0);
                int i_end = std::min(strip + 2, m_strip_max);

                // loop over strips
                for (; i < i_end; i++) {
                    // Need to check if strip is already setup
                    if (m_Analogue[1][i] <= 0.0) {
                        // Values depends on which chip the strip is on (complex
                        // when
                        // strip is on chip edge)
                        int chip = i / 128;
                        float g = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0,
                                                                  S1[chip]);
                        float o = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0,
                                                                  S2[chip]);

                        m_GainFactor[i] = gain[chip] + (cosfi[chip] * g +
                                                        sinfi[chip] * o);
                        m_Offset[i] = offset[chip] + (cosfi[chip] * o -
                                                      sinfi[chip] * g);
                        m_NoiseFactor[i] = noiseByChipVect[chip];

                        // Fill the noise and offset values into the Analogue
                        if (m_data_compression_mode == 1) {
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else if (m_data_compression_mode == 2) { // edge mode
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else if (m_data_compression_mode == 3) { // any hit mode
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[2][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::randomnoise(const Identifier &moduleId) const {
    double occupancy = 0.0;
    double NoiseOccupancy = 0.0;
    float Noise = 0.0;
    int nNoisyStrips = 0;
    double mode = 1.;

    // Will give 3 times as much noise occupancy if running in any hit expanded
    // mode
    if (m_noise_expanded_mode) {
        mode = 3.;
    }

    // Sets fixed noise occupancy values for different module types, barrel, EC,
    // inners, middles
    // short middles, and outers
    if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to
                                              // barrel
        if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10
                                                   // degrees warmer
            NoiseOccupancy = m_NOBarrel3;
            Noise = m_NoiseBarrel3;
        }
        else {
            NoiseOccupancy = m_NOBarrel;
            Noise = m_NoiseBarrel;
        }
    }
    else {
        int moduleType = m_sct_id->eta_module(moduleId);
        switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers, middles
                              // and inners?! (at least in the offline world)
        case 0:
        {
            NoiseOccupancy = m_NOOuters;
            Noise = m_NoiseOuters;
            break;
        }

        case 1:
        {
            if (m_sct_id->layer_disk(moduleId) == 7) {
                NoiseOccupancy = m_NOShortMiddles;
                Noise = m_NoiseShortMiddles;
            }
            else {
                NoiseOccupancy = m_NOMiddles;
                Noise = m_NoiseMiddles;
            }
            break;
        }

        case 2:
        {
            NoiseOccupancy = m_NOInners;
            Noise = m_NoiseInners;
            break;
        }

        default:
        {
            NoiseOccupancy = m_NOBarrel;
            Noise = m_NoiseBarrel;
            ATH_MSG_ERROR("moduleType(eta): " << moduleType <<
                " unknown, using barrel");
        }
        }// end of switch structure
    }

    // Calculate the number of "free strips"
    int nEmptyStrips = 0;
    for (int i = 0; i < m_strip_max; i++) {
        if (m_Analogue[1][i] < 0.0000001) {
            ++nEmptyStrips;
        }
    }

    // if no empty strips on wafer do nothing
    if (nEmptyStrips != 0) {
        // Should randomize the fixed NO values, so we get some differences per
        // wafer
        occupancy = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                    NoiseOccupancy,
                                                    NoiseOccupancy * 0.1);

        // Modify the occupancy if threshold is not 1.0 fC
        if (m_Threshold > 6242.3 || m_Threshold < 6242.1) {
            const float fC = 6242.2;
            occupancy = occupancy * exp(-(0.5 / (Noise * Noise) * (m_Threshold *
                                                                   m_Threshold -
                                                                   fC * fC)));
        }
        nNoisyStrips = CLHEP::RandPoisson::shoot(m_rndmEngine, m_strip_max *
                                                 occupancy * mode);

        // Check and adapt the number of noisy strips to the number of free
        // strips
        if (nEmptyStrips < nNoisyStrips) {
            nNoisyStrips = nEmptyStrips;
        }

        for (int i = 0; i < nNoisyStrips; i++) {
            int strip = CLHEP::RandFlat::shootInt(m_rndmEngine, nEmptyStrips -
                                                  i);
            while (strip < m_strip_max) {
                if (m_Analogue[1][strip] < 0.0000001) {
                    break;
                }
                ++strip;
            }
            // and then fills 767 even though it shouldn't since all strip are
            // already filled from real hits
            m_Analogue[1][strip] = 2 * m_Threshold;
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode SCT_FrontEnd::randomnoise(const Identifier &moduleId, const
                                     int &side) const {
    int n_chips = 6;
    int chipStripmax = m_strip_max / n_chips;
    std::vector<float> NOByChipVect(n_chips, 0.0);
    std::vector<float> ENCByChipVect(n_chips, 0.0);
    std::vector<int> nNoisyStrips(n_chips, 0);
    double mode = 1.;

    // Will give 3 times as much noise occupancy if running in any hit expanded
    // mode
    if (m_noise_expanded_mode) {
        mode = 3.;
    }

    // Get chip data from calib DB
    NOByChipVect = m_ReadCalibChipDataSvc->getNoiseOccupancyData(moduleId, side,
                                                                 "OccupancyByChip");
    ENCByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                                           "NoiseByChip");

    // Need to check if empty, most should have data, but a few old DEAD modules
    // don't, and 9C...
    if (NOByChipVect.empty()) {
        ATH_MSG_DEBUG("No calibration data in cond DB for module " <<
            moduleId << " using JO values");
        if (StatusCode::SUCCESS != randomnoise(moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }
    else {
        for (int i = 0; i < n_chips; i++) {
            // A 0 value can mean two things now, chip out of config for long
            // time and no value was uploaded
            // or its short middles and inners and the value is for all purposes
            // 0! so ok.

            // Modify the occupancy if threshold is not 1.0 fC
            if (m_Threshold > 6242.3 || m_Threshold < 6242.1) {
                const float fC = 6242.2;
                NOByChipVect[i] = NOByChipVect[i] * exp(-(0.5 /
                                                          (ENCByChipVect[i] *
                                                           ENCByChipVect[i]) *
                                                          (m_Threshold * m_Threshold - fC * fC)));
            }

            nNoisyStrips[i] = CLHEP::RandPoisson::shoot(m_rndmEngine,
                                                        chipStripmax *
                                                        NOByChipVect[i] * mode);
        }
    }

    for (int index = 0; index < n_chips; ++index) {
        int chipstrips = chipStripmax * index;

        // Calculate the number of "free strips"
        int nEmptyStrips = 0;
        for (int i = 0; i < chipStripmax; i++) {
            if (m_Analogue[1][i + chipstrips] < 0.0000001) {
                ++nEmptyStrips;
            }
        }

        // if no empty strips on chip do nothing
        if (nEmptyStrips != 0) {
            // Check and adapt the number of noisy strips to the number of free
            // strips
            if (nEmptyStrips < nNoisyStrips[index]) {
                nNoisyStrips[index] = nEmptyStrips;
            }

            for (int i = 0; i < nNoisyStrips[index]; i++) {
                int strip = CLHEP::RandFlat::shootInt(m_rndmEngine,
                                                      nEmptyStrips - i);
                while (strip < chipStripmax) {
                    if (m_Analogue[1][strip + chipstrips] < 0.0000001) {
                        break;
                    }
                    ++strip;
                }
                int strip_on_wafer = strip + chipstrips;
                m_Analogue[1][strip_on_wafer] = 2 * m_Threshold;
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// process the collection of pre digits this will need to go through
// all single-strip pre-digits calculate the amplifier response add noise
// (this could be moved elsewhere later) apply threshold do clustering
// ----------------------------------------------------------------------
void SCT_FrontEnd::process(SiChargedDiodeCollection &collection) const {
    // get SCT module side design and check it
    const SCT_ModuleSideDesign *p_design = dynamic_cast<const
                                                        SCT_ModuleSideDesign *>(
        &(collection.design()));

    if (!p_design) {
        return;
    }

    // Check number of strips in design and from manager(max number of strips on
    // any module)
    // The design value should always be equal or lower than the manager one
    // However, no resising is now done in case of a lower value
    const int strip_max = p_design->cells();
    if (strip_max > m_strip_max) {
        ATH_MSG_ERROR("The number of strips from the design, " << strip_max
                                                               <<
            "are higher than the manager max value, " << m_strip_max <<
            ", will use design and continue...");

        // Resize vectors
        if (StatusCode::SUCCESS != initVectors(strip_max,
                                               m_data_compression_mode)) {
            ATH_MSG_FATAL(
                "Can't resize class variable vectors, compression mode has to be 1 or 2");
        }
        else {
            m_strip_max = strip_max;
            ATH_MSG_DEBUG("Class variable vectors resized");
        }
    }

    // Initialize the FirstDiode vector, need to be global or passed along
    std::vector<SiChargedDiode *> FirstDiode(m_strip_max);// ,NULL);

    // Need to reset the m_Analogue[1][strip] or all of them since used in
    // doClustering
    if (m_data_compression_mode == 3) {
        for (int i = 0; i < m_strip_max; ++i) {
            m_Analogue[0][i] = 0.0;
            m_Analogue[1][i] = 0.0;
            m_Analogue[2][i] = 0.0;
        }
    }
    else {
        for (int i = 0; i < m_strip_max; ++i) {
            m_Analogue[1][i] = 0.0;
        }
    }

    // Get wafer, moduleId and side
    Identifier waferId = collection.identify();
    Identifier moduleId = m_sct_id->module_id(waferId);
    const int side = m_sct_id->side(waferId);

    // Check if collection empty
    if (!collection.empty()) {
        // Setup gain/offset/noise to the hit and neighbouring strips
        if (m_useCalibData) { // Use calib cond DB data
            if (StatusCode::SUCCESS != prepareGainAndOffset(collection, side,
                                                            moduleId)) {
                ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
            }
        }
        else { // Use JO values
            if (StatusCode::SUCCESS != prepareGainAndOffset(collection,
                                                            moduleId)) {
                ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
            }
        }

        if (StatusCode::SUCCESS != doSignalChargeForHits(collection,
                                                         FirstDiode)) {
            ATH_MSG_ERROR("\tCan't process hits");
        }
    }

    // Add random noisy strips
    if (m_NoiseOn) {
        if (m_useCalibData) { // Check if using DB or not
            if (StatusCode::SUCCESS != randomnoise(moduleId, side)) {
                ATH_MSG_ERROR("\tCan't randmize noies on wafer?!");
            }
        }
        else { // Use JO fixed values
            if (StatusCode::SUCCESS != randomnoise(moduleId)) {
                ATH_MSG_ERROR("\tCan't randmize noies on wafer?!");
            }
        }
    }

    // Check for strips above threshold and do clustering
    if (StatusCode::SUCCESS != doClustering(collection, FirstDiode)) {
        ATH_MSG_ERROR("\tCan't process hits");
    }
}

StatusCode SCT_FrontEnd::doSignalChargeForHits(
    SiChargedDiodeCollection &collection,
    std::vector<SiChargedDiode *> &FirstDiode) const {
    typedef SiTotalCharge::list_t list_t;

    // *****************************************************************************
    // Loop over the diodes (strips ) and for each of them define the total
    // signal
    // *****************************************************************************

    // set up number of needed bins depending on the compression mode
    const short bin_max = m_data_compression_mode;

    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();

                const list_t &ChargesOnStrip =
                    diode.totalCharge().chargeComposition();

                // Accumulate averages !!!
                if (m_PulseAveragingFlag) {
                    m_sct_amplifier->AccumulateAverages(ChargesOnStrip);
                }

                if (m_data_compression_mode == 1) {
                    // Amplifier response
                    m_Analogue[1][strip] += m_GainFactor[strip] *
                                            m_sct_amplifier->response(
                        ChargesOnStrip,
                        m_timeOfThreshold);

                    // Add Crosstalk signal for neighboring strip
                    float response = m_sct_amplifier->crosstalk(ChargesOnStrip,
                                                                m_timeOfThreshold);
                    if (strip + 1 < m_strip_max) {
                        m_Analogue[1][strip + 1] += m_GainFactor[strip + 1] *
                                                    response;
                    }
                    if (strip > 0) {
                        m_Analogue[1][strip - 1] += m_GainFactor[strip - 1] *
                                                    response;
                    }
                }
                else if (m_data_compression_mode == 2) { // edge mode
                    // Amplifier response
                    float *response = new float[bin_max];
                    for (short bin = 0; bin < bin_max; ++bin) {
                        response[bin] = 0.0;
                    }
                    m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold,
                                              response, bin_max);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        m_Analogue[bin][strip] += m_GainFactor[strip] *
                                                  response[bin];
                        response[bin] = 0.0;
                    }
                    // Add Crosstalk signal for neighboring strip
                    m_sct_amplifier->crosstalk(ChargesOnStrip,
                                               m_timeOfThreshold, response,
                                               bin_max);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        if (strip + 1 < m_strip_max) {
                            m_Analogue[bin][strip + 1] += m_GainFactor[strip +1] *
                                                          response[bin];
                        }
                        if (strip > 0) {
                            m_Analogue[bin][strip - 1] += m_GainFactor[strip -1] *
                                                          response[bin];
                        }
                    }
                    delete[] response;
                }
                else if (m_data_compression_mode == 3) { // any hit mode
                    // Amplifier response
                    float *response = new float[bin_max];
                    for (short bin = 0; bin < bin_max; ++bin) {
                        response[bin] = 0.0;
                    }
                    m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold,
                                              response, bin_max);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        m_Analogue[bin][strip] += m_GainFactor[strip] *
                                                  response[bin];
                        response[bin] = 0.0;
                    }
                    // Add Crosstalk signal for neighboring strip
                    m_sct_amplifier->crosstalk(ChargesOnStrip,
                                               m_timeOfThreshold,
                                               response, bin_max);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        if (strip + 1 < m_strip_max) {
                            m_Analogue[bin][strip + 1] += m_GainFactor[strip +1] *
                                                          response[bin];
                        }
                        if (strip > 0) {
                            m_Analogue[bin][strip - 1] += m_GainFactor[strip -1] *
                                                          response[bin];
                        }
                    }
                    delete[] response;
                }
            }
            else { // if roCell not valid
                ATH_MSG_WARNING("\t Cannot get the cell ");
            }
        }
        else {// If diode is disconnected/disabled skip it
            ATH_MSG_WARNING("\tDisabled or disconnected diode (strip)");
        }
    }

    // ********************************************************************
    // Flag strips below threshold and set the signal to 0 for the strips
    // ********************************************************************
    for (i_chargedDiode = collection.begin(); i_chargedDiode !=
         i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode &diode = (*i_chargedDiode).second;
        SiReadoutCellId roCell = diode.getReadoutCell();
        if (roCell.isValid()) {
            int strip = roCell.strip();
            if (strip > -1 && strip < m_strip_max) {
                if (m_data_compression_mode == 1) {// edge mode
                    if (m_Analogue[1][strip] < m_Threshold) {
                        SiHelper::belowThreshold(diode, true);   // Below strip
                                                                 // diode signal
                                                                 // threshold
                        m_Analogue[1][strip] = 0.0;
                    }
                    else if (((0x10 & diode.flag()) == 0x10) ||
                             ((0x4 & diode.flag()) == 0x4)) {
                        // previously a crazy strip number could have screwed
                        // things up here.
                        m_Analogue[1][strip] = 0.0;// Disabled, disconnected ?
                    }
                    else {
                        FirstDiode[strip] = &((*i_chargedDiode).second);
                        SiHelper::SetTimeBin(diode, 2); // set timebin info
                    }
                }// if(m_data_compression_mode == 1)
                else if (m_data_compression_mode == 2) {// level mode
                    if ((m_Analogue[0][strip] >= m_Threshold ||
                         m_Analogue[1][strip] < m_Threshold)) {
                        SiHelper::belowThreshold(diode, true);   // Below strip
                                                                 // diode signal
                                                                 // threshold
                        m_Analogue[1][strip] = 0.0;
                    }
                    else if (((0x10 & diode.flag()) == 0x10) ||
                             ((0x4 & diode.flag()) == 0x4)) {
                        // previously a crazy strip number could have screwed
                        // things up here.
                        for (short bin = 0; bin < bin_max; ++bin) {
                            m_Analogue[bin][strip] = 0.0;// Disabled,
                                                         // disconnected ?
                        }
                    }
                    else {
                        FirstDiode[strip] = &((*i_chargedDiode).second);
                        SiHelper::SetTimeBin(diode, 2); // set timebin info
                    }
                }// if(m_data_compression_mode == 2)
                else if (m_data_compression_mode == 3) {// any hit mode
                    int have_hit_bin = 0;
                    // double tot_charge = 0.0;
                    if (m_Analogue[0][strip] >= m_Threshold) {
                        have_hit_bin = 4;
                        // tot_charge += m_Analogue[0][strip];
                    }
                    if (m_Analogue[1][strip] >= m_Threshold) {
                        have_hit_bin += 2;
                        // tot_charge += m_Analogue[1][strip];
                    }
                    if (m_Analogue[2][strip] >= m_Threshold) {
                        have_hit_bin += 1;
                        // tot_charge += m_Analogue[2][strip];
                    }
                    if (have_hit_bin == 0) {
                        SiHelper::belowThreshold(diode, true);   // Below strip
                                                                 // diode signal
                                                                 // threshold
                        for (short bin = 0; bin < bin_max; ++bin) {
                            m_Analogue[bin][strip] = 0.0;
                        }
                    }
                    else if (((0x10 & diode.flag()) == 0x10) ||
                             ((0x4 & diode.flag()) == 0x4)) {
                        // previously a crazy strip number could have screwed
                        // things up here.
                        for (short bin = 0; bin < bin_max; ++bin) {
                            m_Analogue[bin][strip] = 0.0;// Disabled,
                                                         // disconnected ?
                        }
                    }
                    else {
                        FirstDiode[strip] = &((*i_chargedDiode).second);
                        SiHelper::SetTimeBin(diode, have_hit_bin); // set
                                                                   // timebin
                                                                   // info
                        // m_Analogue[1][strip] = tot_charge; //For easier
                        // clustering put all charge in bin one
                    }
                }// if(m_data_compression_mode == 3)
            }
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode SCT_FrontEnd::doClustering(SiChargedDiodeCollection &collection,
                                      std::vector<SiChargedDiode *> &FirstDiode)
const {
    // ********************************
    // now do clustering
    // ********************************
    int strip = 0;             
    int clusterSize = 0;      
    int numberOfClusters = 0; // !< number of cluster in each SCT module 
                              // (collection)
    const SCT_ModuleSideDesign &sctDesign = dynamic_cast<const
                                                         SCT_ModuleSideDesign &> (
        collection.design());
    Identifier waferId(collection.identify());
    Identifier idStrip;

    const int nStripsInWafer = sctDesign.diodes();

    if (m_data_compression_mode != 3) {
        do {
            if (m_Analogue[1][strip] >= m_Threshold) {
                int clusterFirstStrip = strip;         

                ++numberOfClusters;

                // Find end of cluster. In multi-row sensors, cluster cannot
                // span rows.
                int row = sctDesign.row(strip);
                if (row < 0) {
                    row = 0;
                }
                int lastStrip1DInRow(0);
                for (int i = 0; i < row + 1; ++i) {
                    lastStrip1DInRow += sctDesign.diodesInRow(i);
                }
                while (strip < lastStrip1DInRow - 1 && m_Analogue[1][strip +1] >=
                       m_Threshold) {
                    ++strip;
                }
                int clusterLastStrip = strip;

                clusterSize = (clusterLastStrip - clusterFirstStrip) + 1;   

                // ==== Second step: if the FirstDiode does not exist it's a
                // noisy strip.
                // Create the corresponding diode
                // ======================================
                for (int i = clusterFirstStrip; i <= clusterLastStrip; ++i) {
                  if (FirstDiode[i] == NULL) {
                    // Create a new diode
                    SiCellId diode(i);
                    // Add noise to it
                    SiCharge noiseCharge(m_Analogue[1][i], 0,
                                         SiCharge::noise);      
                    // Add it to the collection
                    collection.add(diode, noiseCharge);                             

                    // get the strip back...
                    int row = sctDesign.row(i);
                    if (row < 0) {
                      ATH_MSG_VERBOSE("! Negative 1st row: "<< row << " --> It's normal for Run2-like strips, but may indicate a problem for Phase-2 sensors.");
                      idStrip = m_sct_id->strip_id(waferId, i);
                    }
                    else {
                      int strip2D = sctDesign.strip(i);
                      idStrip = m_sct_id->strip_id(waferId, row, strip2D);
                    }
                    
                    FirstDiode[i] = collection.find(idStrip);
                    if (FirstDiode[i] == NULL) {
                      ATH_MSG_FATAL(
                        "There is a problem with the FirstDiode for strip "
                        << i << ", goodbye");
                      return StatusCode::FAILURE;
                    }
                    // Get the diode in collection
                    SiChargedDiode &ndiode = *(collection.find(idStrip));
                    // Set timebin info
                    SiHelper::SetTimeBin(ndiode, 2);                                
                  }
                  if (i > clusterFirstStrip) {
                    SiHelper::ClusterUsed((*(FirstDiode[i])), true);
                  }
                }
                SiHelper::SetStripNum(*(FirstDiode[clusterFirstStrip]),
                                      clusterSize);
            } //  if(m_Analogue[strip] >= m_Threshold)

            ++strip;  // !< This is the starting point of the next cluster
                      // within this collection
        } while (strip < nStripsInWafer);
    }
    else {
        // Still need to change for Upgrade sensors ... but maybe we never use
        // this mode in upgrade?
        int have_hit_bin;
        double tot_charge;
        do {
            have_hit_bin = 0;
            tot_charge = 0.0;
            if (m_Analogue[0][strip] >= m_Threshold) {
                have_hit_bin = 4;
                // tot_charge += m_Analogue[0][strip];
            }
            if (m_Analogue[1][strip] >= m_Threshold) {
                have_hit_bin += 2;
                // tot_charge += m_Analogue[1][strip];
            }
            if (m_Analogue[2][strip] >= m_Threshold) {
                have_hit_bin += 1;
                // tot_charge += m_Analogue[2][strip];
            }
            if (have_hit_bin > 0) {
                // ====== First step: Get the cluster size
                int clusterFirstStrip = strip;         

                bool have_hit_next_strip;
                while (strip < m_strip_max - 1) {
                    have_hit_next_strip = false;
                    if (m_Analogue[0][strip + 1] >= m_Threshold) {
                        have_hit_next_strip = true;
                    }
                    if (m_Analogue[1][strip + 1] >= m_Threshold) {
                        have_hit_next_strip = true;
                    }
                    if (m_Analogue[2][strip + 1] >= m_Threshold) {
                        have_hit_next_strip = true;
                    }
                    if (have_hit_next_strip) {
                        ++strip;
                    }
                    else {
                        break;
                    }
                }

                int clusterLastStrip = strip;                                
                ++numberOfClusters;
                clusterSize = (clusterLastStrip - clusterFirstStrip) + 1;   

#ifdef SCT_DIG_DEBUG
                ATH_MSG_DEBUG("RDO Strip = " << strip <<
                    ", RDO Cluster size = " << clusterSize);
#endif

                // ==== Second step: if the FirstDiode does not exist it's a
                // noisy strip.
                // Create the corresponding diode
                // ======================================
                for (int i = clusterFirstStrip; i <= clusterLastStrip; ++i) {
                  if (FirstDiode[i] == NULL) {
                    SiCellId diode(i);
                    
                    // There are two types of noise hits, random and
                    // neighbours to
                    // real hits not checked yet
                    if (m_Analogue[1][i] < 2.0001 * m_Threshold ||
                        m_Analogue[1][i] > 1.9999 * m_Threshold) {
                      SiCharge noiseCharge(m_Analogue[1][i], 0,
                                           SiCharge::noise);      
                      collection.add(diode, noiseCharge);                             

                      // get the strip back...
                      int row = sctDesign.row(i);
                      if (row < 0) {
                        ATH_MSG_VERBOSE("! Negative 2nd row: "<< row << " --> It's normal for Run2-like strips, but may indicate a problem for Phase-2 sensors.");
                        idStrip = m_sct_id->strip_id(waferId, i);
                      }
                      else {
                        int strip2D = sctDesign.strip(i);
                        idStrip = m_sct_id->strip_id(waferId, row,
                                                     strip2D);
                      }
                      
                      FirstDiode[i] = collection.find(idStrip);
                      if (FirstDiode[i] == NULL) {
                        ATH_MSG_WARNING(
                          "There is a problem with the FirstDiode for strip "
                          << i <<
                          ", goodbye");
                      }
                      
                      SiChargedDiode &ndiode = *(collection.find(
                                                   idStrip));

                      // Random tbin or fixed?!
                      // Random number 0, 1 or 2
                      int noise_tbin = CLHEP::RandFlat::shootInt(
                        m_rndmEngine, 2);          
                      if (noise_tbin == 0) {
                        noise_tbin = 4; // now 1, 2 or 4                           
                      }
                      SiHelper::SetTimeBin(ndiode, noise_tbin);
                    }
                    else {
                      // Waste of time, already checked if above threshold
                      // before in
                      // cluster...
                      if (m_Analogue[0][i] >= m_Threshold) {
                        have_hit_bin = 4;
                        tot_charge += m_Analogue[0][i];
                      }
                      if (m_Analogue[1][i] >= m_Threshold) {
                        have_hit_bin += 2;
                        tot_charge += m_Analogue[1][i];
                      }
                      if (m_Analogue[2][i] >= m_Threshold) {
                        have_hit_bin += 1;
                        tot_charge += m_Analogue[2][i];
                      }
                      
                      SiCharge noiseCharge(tot_charge, 0,
                                           SiCharge::noise);            
                      collection.add(diode, noiseCharge);
                      

                      // get the strip back...
                      int row = sctDesign.row(i);
                      if (row < 0) {
                        ATH_MSG_VERBOSE("! Negative 3rd row: "<< row << " --> It's normal for Run2-like strips, but may indicate a problem for Phase-2 sensors.");
                        idStrip = m_sct_id->strip_id(waferId, i);
                      }
                      else {
                        int strip2D = sctDesign.strip(i);
                        idStrip = m_sct_id->strip_id(waferId, row,
                                                     strip2D);
                      }
                      
                      FirstDiode[i] = collection.find(idStrip);
                      if (FirstDiode[i] == NULL) {
                        ATH_MSG_WARNING(
                          "There is a problem with the FirstDiode for strip "
                          << i <<
                          ", goodbye");
                      }

                      SiChargedDiode &ndiode = *(collection.find(
                                                   idStrip));          
                      SiHelper::SetTimeBin(ndiode, have_hit_bin);                     
                    }
                  }
                  if (i > clusterFirstStrip) {
                    SiHelper::ClusterUsed((*(FirstDiode[i])), true);
                  }
                }
                SiHelper::SetStripNum(*(FirstDiode[clusterFirstStrip]),
                                      clusterSize);
            } //  if(m_Analogue[strip|bin] >= m_Threshold)
            
            ++strip;  // !< This is the starting point of the next cluster
                      // within this
                      // collection
        } while (strip < m_strip_max);
    }
    
    // clusters below threshold, only from pre-digits that existed before no
    // pure noise clusters below threshold will be made
    // D. Costanzo. I don't see why we should cluster the strips below
    // threshold. I'll pass on the info of strips below threshold
    // to the SDOs. I'll leave the SCT experts the joy to change this if they
    // don't like what I did or if this requires too much memory/disk

    return StatusCode::SUCCESS;
}

float SCT_FrontEnd::meanValue(std::vector<float> &calibDataVect) const {
    float mean_value = 0.0;
    int nData = 0;
    unsigned vec_size = calibDataVect.size();

    for (unsigned int i = 0; i < vec_size; ++i) {
        if (calibDataVect[i] > 0.1) {
            mean_value += calibDataVect[i];
            ++nData;
        }
    }
    if (nData == 0) {
        return -1;
    }
    else {
        return mean_value / nData;
    }
}
