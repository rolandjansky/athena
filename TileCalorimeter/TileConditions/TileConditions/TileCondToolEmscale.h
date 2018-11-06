/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLEMSCALE_H
#define TILECONDITIONS_TILECONDTOOLEMSCALE_H

/**
 * @brief Calibration of TileCal channels and access to calibration factors.
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
 * The central function of this AlgTool is channelCalib(), which
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
 * TileCondToolConf.py provices a getter function for this TileCondTool.
 * TileCondToolEmscale needs the TileCalibEms input data.
 */

// Tile includes
#include "TileConditions/TileEMScale.h"
#include "TileIdentifier/TileRawChannelUnit.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"


#include <vector>
#include <array>

class TileCondToolEmscale: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolEmscale(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolEmscale();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**
     * @brief Calibrates a Tile channel.
     * @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     * Note that "upstream" calibrations are not supported, i.e. calibration requests from eg. MeV to ADC counts
     * will result in an exception being thrown.
     * @returns The input amplitude after calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of rawDataUnitIn
     * @param rawDataUnitIn Input unit, see TileIdentifier/TileRawChannelUnit.h
     * @param rawDataUnitOut Output unit, see TileIdentifier/TileRawChannelUnit.h
     */
    float channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                       , TileRawChannelUnit::UNIT rawDataUnitIn, TileRawChannelUnit::UNIT rawDataUnitOut) const;

    /**
     * @brief Calibration to EM scale: Equalized pC -> MeV.
     * @details Applies the pC to MeV calibration constant
     * @returns The input amplitude after calibration [MeV]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Application of cesium calibration constant.
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     * @param applyLasCorr Apply the (not yet implemented) relative laser correction
     */
    float doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;

    /**
     * @brief Application of laser (PMT linearity) correction.
     * @details Applies both the linear and non-linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Application of CIS calibration: ADC counts -> pC.
     * @details Applies both the linear and non-linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    /**
     * @brief Calibrates an online Tile channel from OnlineADCcounts to an arbitrary Online unit, no NLN corrections applied
     * @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     * Note that "upstream" calibrations are not supported, i.e. calibration requests from eg. MeV to ADC counts
     * will result in an exception being thrown.
     * @returns The total online calibration constant
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input amplitude in OnlineADCcounts unit
     * @param onlUnit Output online unit, see TileIdentifier/TileRawChannelUnit.h
     */
    float channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                          , TileRawChannelUnit::UNIT onlUnit) const;

    /**
     * @brief Online calibration to EM scale: Equalized pC -> MeV.
     * @details Applies the pC to MeV calibration constant
     * @returns The input amplitude after calibration [MeV]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Online application of cesium calibration constant.
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     * @param applyLasCorr Apply the (not yet implemented) relative laser correction
     */
    float doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;

    /**
     * @brief Online application of laser (PMT linearity) correction.
     * @details Applies only the linear part of the laser calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param amplitude Input ampitude in pC
     */
    float doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /**
     * @brief Online Application of CIS calibration: ADC counts -> pC.
     * @details Applies only the linear part of the CIS calibration
     * @returns The input amplitude after calibration [pC]
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in units of ADC counts
     */
    float doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    /**
     * @brief Undoes the calibration applied in ROD signal reconstruction
     * @details At the run configuration stage, calibration constants are loaded into TileCal RODs, enabling
     * the output of the signal reconstruction run online in DSPs to be sent in units of MeV. However, in this
     * first raw calibration only the linear part of corrections are applied. In order to apply the full
     * (and maybe in the meantime updated) calibration including all non-linear corrections,
     * the calibrations applied in the ROD need to be undone first.
     * @returns The signal as reconstructed in the DSP in units of ADC counts.
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     * @param amplitude Input ampitude in online units onlUnit
     * @param onlUnit The output unit of the DSP, preserved in the bytestream
     */
    float undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                       , TileRawChannelUnit::UNIT onlUnit) const;

    /**
     * @brief Returns the PMT gain reference as measured by the laser system.
     * @details Continous PMT gain measurements with the laser system enable to correct for PMT
     * gain drifts between cesium calibration runs. Corrections are applied relative to the PMT
     * gain measured by the laser system directly after the last cesium equalization.
     * @returns The PMT gain reference at the last cesium scan.
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     * @param adc Gain
     */
    float getCesRefLas(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    /**
     * @brief Returns the reference PMT HV set in last cesium calibration.
     * @returns The PMT HV applied as result of last cesium calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getCesRefHv(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Returns the drawer temperature as measured during last cesium equalization.
     * @returns The drawer temperature during last cesium equalization
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getCesRefTemp(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Returns laser fiber relative variation
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getLasFiber(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Returns laser partition relative variation
     * @details The partition is defined by the drawerIdx which belongs to this partition.
     * The relative partition variation values are stored in m_pryOflLasFib proxy in the first
     * drawer of each partition at tile channel LAS_PART_CHAN (it is 43), which is empty both
     * in barrel and in extended barrel tile modules. Hence its value is not useful
     * for getLasFiber, which reads from the same proxy.
     * @param drawerIdx Drawer index
     */
    float getLasPartition(unsigned int drawerIdx) const;

    /**
     * @brief Returns the reference PMT HV from appropriate laser calibration.
     * @returns The PMT HV applied as result of laser calibration
     * @param drawerIdx Drawer index
     * @param channel Tile channel
     */
    float getLasRefHv(unsigned int drawerIdx, unsigned int channel) const;

    /**
     * @brief Returns the unit of cached online calibration constants.
    */
    TileRawChannelUnit::UNIT getOnlCacheUnit() const;

  protected:

    SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
        "TileEMScale", "TileEMScale", "Input Tile EMS calibration constants"};


};

#endif
