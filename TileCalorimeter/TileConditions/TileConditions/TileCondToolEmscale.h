/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLEMSCALE_H
#define TILECONDITIONS_TILECONDTOOLEMSCALE_H

/** 
 @brief Calibration of TileCal channels and access to calibration factors.
 @author Nils Gollub <nils.gollub@cern.ch>

 @details In order to calibrate signals from TileCal to the
 electromagnetic (EM) scale, a set of calibration constants need to
 be applied. They are obtained by the independent CIS, Laser and
 Cesium calibration system. The final calibration constant from pC
 to the EM scale is determinded in dedicated electron and muon
 testbeam analyses. This AlgTool provides access to the calibration
 constants stored in the COOL database and functions to perform the
 actual calibrations.

 The central function of this AlgTool is channelCalib(), which
 enables the user to calibrate any TileCal channel from a given
 input unit (usually ADC counts or the online unit stored in the
 bytestream) to the desired output unit (usually MeV). In addition,
 functions performing the individual calibration steps are
 provided. They also give access to the calibration constants
 stored in the COOL DB by using unit amplitude as input.

 A first "online" calibration is already applied during the signal
 reconstruction in the ROD. The used calibration constants are
 preserved in special database folders that do not allow to modify
 calibration constants for runs already taken. This ensures that
 the calibrations applied online in a given run can always be
 undone later in order to apply updated calibration constants in a
 reprocessing step. In addition, CIS and Laser calibrations might
 contain non-linear parts which are only applied during the offline
 calibration using this AlgTool.


 @b Configuration

 TileCondToolConf.py provices a getter function for this
 TileCondTool. TileCondToolEmscale needs the following input data
 sources, connected through TileCondProxies:

 - \ref TileCondToolEmscale::m_pryOflCisLin "ProxyOflCisLin": Linear part of the the CIS calibration
 - \ref TileCondToolEmscale::m_pryOflCisNln "ProxyOflCisNln": Non-linear part of the the CIS calibration
 - \ref TileCondToolEmscale::m_pryOflLasLin "ProxyOflLasLin": Linear part of the the Laser calibration
 - \ref TileCondToolEmscale::m_pryOflLasNln "ProxyOflLasNln": Non-linear part of the the Laser calibration
 - \ref TileCondToolEmscale::m_pryOflCes "ProxyOflCes": Cesim calibration constants (inlcuding Laser, HV and temperature references)
 - \ref TileCondToolEmscale::m_pryOflEms "ProxyOflEms": pC to MeV calibration constants

 The calibration constants applied in the ROD reconstruction have
 to be supplied through the "online" TileCondProxies:

 - \ref TileCondToolEmscale::m_pryOnlCis "ProxyOnlCis": Linear part of CIS calibration applied in ROD
 - \ref TileCondToolEmscale::m_pryOnlLas "ProxyOnlLas": Linear part of Laser calibration applied in ROD
 - \ref TileCondToolEmscale::m_pryOnlCes "ProxyOnlCes": Cesium factor applied in ROD
 - \ref TileCondToolEmscale::m_pryOnlEms "ProxyOnlEms": EM calibration factor applied in ROD


 Since the calibration factor applied in the ROD is the product of
 up to four numbers, associated to the four calibration steps
 listed above, the total ROD calibration factor is cached. The unit
 of the cache is configurable through the \ref
 TileCondToolEmscale::m_onlCacheUnit "OnlCacheUnit" property and
 should be set to the ROD output unit present in the bytestream
 (otherwise the total factor will be recomputed event by event).
 The \ref TileCondToolEmscale::m_onlCacheUnit "OnlCacheUnit"
 property recognizes the following string arguments:

 - "OnlineADCcounts"
 - "OnlinePicoCoulombs"
 - "OnlineCesiumPicoCoulombs"
 - "OnlineMegaElectronVolts"
 - "Invalid"

 If the ROD calibration should not be undone (or is not available
 as for simulated data), \ref TileCondToolEmscale::m_onlCacheUnit
 "OnlCacheUnit" should be set to "Invalid" (which is the default).
 */

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>
#include <array>

class TileCondToolEmscale: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolEmscale(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolEmscale();

    StatusCode initialize();
    StatusCode finalize();

    /** @brief Calibrates a Tile channel.
     @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     Note that "upstream" calibrations are not supported, i.e. calibration requests from eg. MeV to ADC counts
     will result in an exception being thrown.
     @returns The input amplitude after calibration
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param amplitude Input ampitude in units of rawDataUnitIn
     @param rawDataUnitIn Input unit, see TileIdentifier/TileRawChannelUnit.h
     @param rawDataUnitOut Output unit, see TileIdentifier/TileRawChannelUnit.h */
    float channelCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                       , TileRawChannelUnit::UNIT rawDataUnitIn, TileRawChannelUnit::UNIT rawDataUnitOut) const;

    /** @brief Calibration to EM scale: Equalized pC -> MeV.
     @details Applies the pC to MeV calibration constant
     @returns The input amplitude after calibration [MeV]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC */
    float doCalibEms(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /** @brief Application of cesium calibration constant.
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC
     @param applyLasCorr Apply the (not yet implemented) relative laser correction */
    float doCalibCes(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;

    /** @brief Application of laser (PMT linearity) correction.
     @details Applies both the linear and non-linear part of the laser calibration
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC */
    float doCalibLas(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /** @brief Application of CIS calibration: ADC counts -> pC.
     @details Applies both the linear and non-linear part of the CIS calibration
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param amplitude Input ampitude in units of ADC counts */
    float doCalibCis(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    /** @brief Calibrates an online Tile channel from OnlineADCcounts to an arbitrary Online unit, no NLN corrections applied
     @details Transforms the input amplitude with unit rawDataUnitIn to output amplitude with unit rawDataUnitOut.
     Note that "upstream" calibrations are not supported, i.e. calibration requests from eg. MeV to ADC counts
     will result in an exception being thrown.
     @returns The total online calibration constant
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param onlUnitIn Input unit, see TileIdentifier/TileRawChannelUnit.h */
    float channelCalibOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                          , TileRawChannelUnit::UNIT onlUnit) const;

    /** @brief Online calibration to EM scale: Equalized pC -> MeV.
     @details Applies the pC to MeV calibration constant
     @returns The input amplitude after calibration [MeV]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC */
    float doCalibEmsOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /** @brief Online application of cesium calibration constant.
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC
     @param applyLasCorr Apply the (not yet implemented) relative laser correction */
    float doCalibCesOnl(unsigned int drawerIdx, unsigned int channel, float amplitude, bool applyLasCorr = true) const;

    /** @brief Online application of laser (PMT linearity) correction.
     @details Applies only the linear part of the laser calibration
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param amplitude Input ampitude in pC */
    float doCalibLasOnl(unsigned int drawerIdx, unsigned int channel, float amplitude) const;

    /** @brief Online Application of CIS calibration: ADC counts -> pC.
     @details Applies only the linear part of the CIS calibration
     @returns The input amplitude after calibration [pC]
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param amplitude Input ampitude in units of ADC counts */
    float doCalibCisOnl(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude) const;

    /** @brief Undoes the calibration applied in ROD signal reconstruction
     @details At the run configuration stage, calibration constants are loaded into TileCal RODs, enabling
     the output of the signal reconstruction run online in DSPs to be sent in units of MeV. However, in this
     first raw calibration only the linear part of corrections are applied. In order to apply the full
     (and maybe in the meantime updated) calibration including all non-linear corrections,
     the calibrations applied in the ROD need to be undone first.
     @returns The signal as reconstructed in the DSP in units of ADC counts.
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param amplitude Input ampitude in online units onlUnit
     @param onlUnit The output unit of the DSP, preserved in the bytestream */
    float undoOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float amplitude
                       , TileRawChannelUnit::UNIT onlUnit) const;

    /** @brief Returns the PMT gain reference as measured by the laser system.
     @details Continous PMT gain measurements with the laser system enable to correct for PMT
     gain drifts between cesium calibration runs. Corrections are applied relative to the PMT
     gain measured by the laser system directly after the last cesium equalization.
     @returns The PMT gain reference at the last cesium scan.
     @param drawerIdx Drawer index
     @param channel Tile channel 
     @param adc Gain */
    float getCesRefLas(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const;

    /** @brief Returns the reference PMT HV set in last cesium calibration.
     @returns The PMT HV applied as result of last cesium calibration
     @param drawerIdx Drawer index
     @param channel Tile channel */
    float getCesRefHv(unsigned int drawerIdx, unsigned int channel) const;

    /** @brief Returns the drawer temperature as measured during last cesium equalization.
     @returns The drawer temperature during last cesium equalization
     @param drawerIdx Drawer index
     @param channel Tile channel */
    float getCesRefTemp(unsigned int drawerIdx, unsigned int channel) const;

    /** @brief Returns laser fiber relative variation
     @param drawerIdx Drawer index
     @param channel Tile channel */
    float getLasFiber(unsigned int drawerIdx, unsigned int channel) const;

    /** @brief Returns laser partition relative variation
     @details The partition is defined by the drawerIdx which belongs to this partition.
     The relative partition variation values are stored in m_pryOflLasFib proxy in the first
     drawer of each partition at tile channel LAS_PART_CHAN (it is 43), which is empty both
     in barrel and in extended barrel tile modules. Hence its value is not useful
     for getLasFiber, which reads from the same proxy.
     @param drawerIdx Drawer index */
    float getLasPartition(unsigned int drawerIdx) const;

    /** @brief Returns the reference PMT HV from appropriate laser calibration.
     @returns The PMT HV applied as result of laser calibration
     @param drawerIdx Drawer index
     @param channel Tile channel */
    float getLasRefHv(unsigned int drawerIdx, unsigned int channel) const;

    /** @brief Returns the unit of cached online calibration constants. */
    TileRawChannelUnit::UNIT getOnlCacheUnit() const {
      return m_onlCacheUnit;
    }

  protected:

    /** @brief Recaches the total online calibration constant applied in ROD.
     @details This callback is called if any of the "online" proxies callback function
     has been called due to an updated COOL DB IOV. The cache is cleared and refilled with
     the current valid values of the unit specified in the
     \ref TileCondToolEmscale::m_onlCacheUnit "OnlCacheUnit" property. */
    StatusCode resetOnlCache( IOVSVC_CALLBACK_ARGS);

    /** @brief Returns the calibration constant applied in the DSP.
     @param drawerIdx Drawer index
     @param channel Tile channel
     @param adc Gain
     @param onlUnit The output unit of the DSP */
    float getOnlCalib(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                      , TileRawChannelUnit::UNIT onlUnit) const;

    /** @brief Check if offline linear laser calibration will be used
     @details This callback is called if offline linear laser of the proxies callback function
     has been called due to an updated COOL DB IOV.
     \ref TileCondToolEmscale::m_OflLasLinUsed  property. */
    StatusCode checkIfOflLasLinCalibUsed( IOVSVC_CALLBACK_ARGS);

    /** @brief Check if offline nonlinear laser calibration will be used
     @details This callback is called if offline nonlinear laser of the proxies callback function
     has been called due to an updated COOL DB IOV.
     \ref TileCondToolEmscale::m_OflLasNlnUsed  property. */
    StatusCode checkIfOflLasNlnCalibUsed( IOVSVC_CALLBACK_ARGS);

    /** @brief Returns cache index used for online calibration constants.
        @details Returns cache index used for the calibration constant applied in the DSP
        @param drawerIdx Drawer index                                                                                                                                         @param channel Tile channel                                                                                                                                           @param adc Gain 
    */
    inline unsigned int cacheIndex(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
      return m_drawerCacheSize * drawerIdx + m_maxChannels * adc + channel;
    };

    //=== cache of total online calibration factors
    std::string m_onlCacheUnitStr;
    TileRawChannelUnit::UNIT m_onlCacheUnit;
    std::vector<float> m_onlCache;

    bool m_OflLasLinUsed;
    bool m_OflLasNlnUsed;

    //=== TileCondProxies are needed by the TileExpertToolEmscale
    //--- offline proxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflCisLin;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflCisNln;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflLasLin;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflLasNln;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflLasFib;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflCes;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOflEms;
    //--- online
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOnlCis;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOnlLas;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOnlCes;
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryOnlEms;

    bool m_useOflLasFib;

    unsigned int m_maxChannels;
    unsigned int m_maxGains;
    unsigned int m_drawerCacheSize;
    
};

#endif
