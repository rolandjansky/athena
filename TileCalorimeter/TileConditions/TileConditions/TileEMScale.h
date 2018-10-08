/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEEMSCALE_H
#define TILECONDITIONS_TILEEMSCALE_H

/**
 * @brief Condition object to keep calibration factors of TileCal channels
 * @author Nils Gollub <nils.gollub@cern.ch>
 *
 * @details In order to calibrate signals from TileCal to the
 * electromagnetic (EM) scale, a set of calibration constants need to
 * be applied. They are obtained by the independent CIS, Laser and
 * Cesium calibration system. The final calibration constant from pC
 * to the EM scale is determinded in dedicated electron and muon
 * testbeam analyses. This AlgTool provides access to the calibration
 * constants stored in the COOL database and functions to perform the
 * actual calibrations.
 *
 * The central function of this AlgTool is calibrateChannel(), which
 * enables the user to calibrate any TileCal channel from a given
 * input unit (usually ADC counts or the online unit stored in the
 * bytestream) to the desired output unit (usually MeV). In addition,
 * functions performing the individual calibration steps are
 * provided. They also give access to the calibration constants
 * stored in the COOL DB by using unit amplitude as input.
 *
 * A first "online" calibration is already applied during the signal
 * reconstruction in the ROD. The used calibration constants are
 * preserved in special database folders that do not allow to modify
 * calibration constants for runs already taken. This ensures that
 * the calibrations applied online in a given run can always be
 * undone later in order to apply updated calibration constants in a
 * reprocessing step. In addition, CIS and Laser calibrations might
 * contain non-linear parts which are only applied during the offline
 * calibration using this AlgTool.
 *
 *
 * @b Configuration
 *
 *  TileCalibEms needs the following TileCalibData input data
 *  set up via corresponding methods
 * - setCalibOflCisLin(): Linear part of the the CIS calibration
 * - setCalibOflCisNln(): Non-linear part of the the CIS calibration
 * - setCalibOflLasLin(): Linear part of the the Laser calibration
 * - setCalibOflLasNln(): Non-linear part of the the Laser calibration
 * - setCalibOflCes(): Cesim calibration constants (inlcuding Laser, HV and temperature references)
 * - setCalibOflEms(): pC to MeV calibration constants
 *
 * The calibration constants applied in the ROD reconstruction have
 * to be supplied through the "online" TileCalibData:
 *
 * - setCalibOnlCis(): Linear part of CIS calibration applied in ROD
 * - setCalibOnlLas(): Linear part of Laser calibration applied in ROD
 * - setCalibOnlCes(): Cesium factor applied in ROD
 * - setCalibOnlEms(): EM calibration factor applied in ROD
 *
 *
 * Since the calibration factor applied in the ROD is the product of
 * up to four numbers, associated to the four calibration steps
 * listed above, the total ROD calibration factor is cached. The unit
 * of the cache is set up by setOnlCacheUnit() method and
 * should be set to the ROD output unit present in the bytestream
 * (otherwise the total factor will be recomputed event by event).
 * The following online cache units can be used here:
 *
 * - TileRawChannelUnit::OnlineADCcounts
 * - TileRawChannelUnit::OnlinePicoCoulombs
 * - TileRawChannelUnit::OnlineCesiumPicoCoulombs
 * - TileRawChannelUnit::OnlineMegaElectronVolts
 * - TileRawChannelUnit::Invalid
 *
 * If the ROD calibration should not be undone (or is not available
 * as for simulated data), online cache unit should be set to
 * "Invalid" (which is the default).
 */

// Tile includes
#include "TileConditions/TileCalibData.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

#include <vector>
#include <array>

class TileEMScale {
  public:

    TileEMScale();
    virtual ~TileEMScale();

    bool initialize();

    /**
     * @brief Calibrate a Tile channel.
     * @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     *          Note that "upstream" calibrations are not supported, i.e. calibration requests from eg.
     *          MeV to ADC counts will result in an exception being thrown.
     * @returns The input amplitude after calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of rawDataUnitIn
     * @param rawDataUnitIn Input unit, see TileIdentifier/TileRawChannelUnit.h
     * @param rawDataUnitOut Output unit, see TileIdentifier/TileRawChannelUnit.h */
    float calibrateChannel(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude,
                           TileRawChannelUnit::UNIT rawDataUnitIn, TileRawChannelUnit::UNIT rawDataUnitOut) const;

    /**
     * @brief Calibrate a Tile channel to EM scale: Equalized pC -> MeV.
     * @details Apply the pC to MeV calibration constant
     * @returns The input amplitude after calibration [MeV]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC */
    float applyEMScaleCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply cesium calibration constant.
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     * @param applyLasCorr Apply the (not yet implemented) relative laser correction
     */
    float applyCesiumCalibration(unsigned int drawerIdx, unsigned int channel,
                                 float amplitude, bool applyLasCorr = true) const;

    /**
     * @brief Apply laser correction.
     * @details Apply both the linear and non-linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float applyLaserCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply laser (PMT linearity) correction.
     * @details Apply only the linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float applyLaserLinearCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply laser (PMT nonlinearity) correction.
     * @details Apply only the non-linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float applyLaserNonLinearCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply CIS calibration: ADC counts -> pC.
     * @details Apply both the linear and non-linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float applyChargeCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                 float amplitude) const;

    /**
     * @brief Apply CIS linear calibration: ADC counts -> pC.
     * @details Apply only the linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float applyChargLinearCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                      float amplitude) const;

    /**
     * @brief Apply CIS non-linear calibration: ADC counts -> pC.
     * @details Apply only the non-linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float applyChargeNonLinearCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                          float amplitude) const;


    /**
     * @brief Calibrate an online Tile channel from OnlineADCcounts to an arbitrary Online unit,
     *        no non-linear corrections are applied
     * @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     *          Note that "upstream" calibrations are not supported, i.e. calibration requests from eg.
     *          MeV to ADC counts will result in an exception being thrown.
     * @returns The total online calibration constant
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input amplitude in OnlineADCcounts unit
     * @param onlUnit Output online unit, see TileIdentifier/TileRawChannelUnit.h
     */
    float calibrateOnlineChannel(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                 float amplitude, TileRawChannelUnit::UNIT onlUnit) const;

    /**
     * @brief  Calibrate online chnnel to EM scale: Equalized pC -> MeV.
     * @details Apply the pC to MeV calibration constant
     * @returns The input amplitude after calibration [MeV]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float applyOnlineEMScaleCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply online cesium calibration constant.
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     * @param applyLasCorr Apply the (not yet implemented) relative laser correction
     */
    float applyOnlineCesiumCalibration(unsigned int drawerIdx, unsigned int channel,
                                       float amplitude, bool applyLasCorr = true) const;

    /**
     * @brief Apply online laser (PMT linearity) correction.
     * @details Apply only the linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float applyOnlineLaserCalibration(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Apply online CIS calibration: ADC counts -> pC.
     * @details Apply only the linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float applyOnlineChargeCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                       float amplitude) const;

    /**
     * @brief Undo the calibration applied in ROD signal reconstruction
     * @details At the run configuration stage, calibration constants are loaded into TileCal RODs, enabling
     *          the output of the signal reconstruction run online in DSPs to be sent in units of MeV.
     *          However, in this first raw calibration only the linear part of corrections are applied.
     *          In order to apply the full (and maybe in the meantime updated) calibration including
     *          all non-linear corrections, the calibrations applied in the ROD need to be undone first.
     * @returns The signal as reconstructed in the DSP in units of ADC counts.
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in online units onlUnit
     * @param onlUnit The output unit of the DSP, preserved in the bytestream
     */
    float undoOnlineChannelCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                                       float amplitude, TileRawChannelUnit::UNIT onlUnit) const;

    /**
     * @brief Return the PMT gain reference as measured by the laser system.
     * @details Continous PMT gain measurements with the laser system enable to correct for PMT
     *          gain drifts between cesium calibration runs. Corrections are applied relative to the PMT
     *          gain measured by the laser system directly after the last cesium equalization.
     * @returns The PMT gain reference at the last cesium scan.
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     */
    float getCesiumReferenceLaserGain(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    /**
     * @brief Return the reference PMT HV set in last cesium calibration.
     * @returns The PMT HV applied as result of last cesium calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getCesiumReferenceHV(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Return the drawer temperature as measured during last cesium equalization.
     * @returns The drawer temperature during last cesium equalization
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getCesiumReferenceTemperature(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Return laser fiber relative variation
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getLaserFiberVariation(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Return laser partition relative variation
     * @details The partition is defined by the drawerIdx which belongs to this partition.
     *          The relative partition variation values are stored in the first drawer
     *          of each partition at tile channel LAS_PART_CHAN (it is 43), which is empty both
     *          in barrel and in extended barrel tile modules. Hence its value is not useful
     *          for getLasFiber, which reads from the same source.
     * @param drawerIdx Drawer index
     */
    float getLaserPartitionVariation(unsigned int drawerIdx) const;

    /**
     * @brief Returns the reference PMT HV from appropriate laser calibration.
     * @returns The PMT HV applied as result of laser calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getLaserReferenceHV(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Return the unit of cached online calibration constants.
     */
    TileRawChannelUnit::UNIT getOnlineCacheUnit() const;

    /**
     * @brief Set online cache unit.
     * @param onlCacheUnit online cache unit
     */
    void setOnlineCacheUnit(TileRawChannelUnit::UNIT onlCacheUnit);

    void setCalibOflCisLin(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflCisNln(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflLasLin(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflLasNln(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflLasFib(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflCes(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOflEms(std::unique_ptr<TileCalibDataFlt> calibData);

    void setCalibOnlCis(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOnlLas(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOnlCes(std::unique_ptr<TileCalibDataFlt> calibData);
    void setCalibOnlEms(std::unique_ptr<TileCalibDataFlt> calibData);

    void setMaxChannels(unsigned int maxChannels);
    void setMaxGains(unsigned int maxGains);

  protected:

    /**
     * @brief Recache the total online calibration constant applied in ROD.
     * @details This callback is called if any of the "online" proxies callback function
     *          has been called due to an updated COOL DB IOV. The cache is cleared and
     *          refilled with the current valid values of the unit specified in the
     *          \ref TileEMScale::m_onlCacheUnit "OnlCacheUnit" property.
     */
    void resetOnlineCache();

    /**
     * @brief Return the calibration constant applied in the DSP.
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param onlUnit The output unit of the DSP
     */
    float getOnlineCalibration(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                               TileRawChannelUnit::UNIT onlUnit) const;

    /**
     * @brief Check if offline linear laser calibration will be used
     * @details This callback is called if offline linear laser of the proxies callback function
     *          has been called due to an updated COOL DB IOV. \ref TileEMScale::m_isOflLasLinUsed  property.
     */
    bool checkIfOflLaserLinearCalibrationUsed() const;

    /**
     * @brief Check if offline nonlinear laser calibration will be used
     * @details This callback is called if offline nonlinear laser of the proxies callback function
     *          has been called due to an updated COOL DB IOV.\ref TileEMScale::m_isOflLasNlnUsed  property.
     */
    bool checkIfOflLaserNonLinearCalibrationUsed() const;

    /**
     * @brief Return cache index used for online calibration constants.
     * @details Return cache index used for the calibration constant applied in the DSP
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     */

    unsigned int getCacheIndex(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    //=== cache of total online calibration factors
    TileRawChannelUnit::UNIT m_onlCacheUnit;
    std::vector<float> m_onlCache;

    bool m_isOflLasLinUsed;
    bool m_isOflLasNlnUsed;

    //=== TileCondProxies are needed by the TileCalibEms
    //--- offline TileCalibData
    std::unique_ptr<TileCalibDataFlt> m_calibOflCisLin;
    std::unique_ptr<TileCalibDataFlt> m_calibOflCisNln;
    std::unique_ptr<TileCalibDataFlt> m_calibOflLasLin;
    std::unique_ptr<TileCalibDataFlt> m_calibOflLasNln;
    std::unique_ptr<TileCalibDataFlt> m_calibOflLasFib;
    std::unique_ptr<TileCalibDataFlt> m_calibOflCes;
    std::unique_ptr<TileCalibDataFlt> m_calibOflEms;

    //--- online TileCalibData
    std::unique_ptr<TileCalibDataFlt> m_calibOnlCis;
    std::unique_ptr<TileCalibDataFlt> m_calibOnlLas;
    std::unique_ptr<TileCalibDataFlt> m_calibOnlCes;
    std::unique_ptr<TileCalibDataFlt> m_calibOnlEms;

    bool m_useOflLasFib;

    unsigned int m_maxChannels;
    unsigned int m_maxGains;
    unsigned int m_drawerCacheSize;

    const float m_epsilon = 1.e-5;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF(TileEMScale, 166690091, 0)
CONDCONT_DEF(TileEMScale, 177875727);


// inlines

inline
TileRawChannelUnit::UNIT TileEMScale::getOnlineCacheUnit() const {
  return m_onlCacheUnit;
}

inline
void TileEMScale::setOnlineCacheUnit(TileRawChannelUnit::UNIT onlCacheUnit) {
  m_onlCacheUnit = onlCacheUnit;
}

inline
void TileEMScale::setMaxChannels(unsigned int maxChannels) {
  m_maxChannels = maxChannels;
}

inline
void TileEMScale::setMaxGains(unsigned int maxGains) {
  m_maxGains = maxGains;
}

inline
unsigned int TileEMScale::getCacheIndex(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
  return m_drawerCacheSize * drawerIdx + m_maxChannels * adc + channel;
}

#endif // TILECONDITIONS_TILEEMSCALE_H
