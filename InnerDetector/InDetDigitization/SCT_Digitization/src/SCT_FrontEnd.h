// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Header file for SCT_FrontEnd Class 
 * @brief simulation of the SCT front-end electronics
 * working as a SiPreDigitsProcessor
 * models response of ABCD chip amplifiers to 
 * collected charges, also does cross-talk, offset
 * variation and gain variation, in a correlated way
 * Version 1.0 04.05.2001 Szymon Gadomski
 * 1.1 07.06.2001 pass amplifier and design as pointers
 * 1.2, 15/06/2001, compiles with framework and tested
 * 31.07.2001 changes, constructor now without design
 * design is only known to process method (from the collection)
 * 10.08.2001 dedided not to use AlwaysSame flag
 * Implementation for using it may be provided later
 * @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,
 * tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch, Jorgen.Dalmau@cern.ch,
 * 23/08/2007 - Kondo.Gnanvo@cern.ch
 *            - Conversion of the SCT_Front code Al gTool 
 */

#ifndef SCT_DIGITIZATION_SCT_FRONTEND_H
#define SCT_DIGITIZATION_SCT_FRONTEND_H

// Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_FrontEnd.h"

// Athena
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SCT_ConditionsTools/ISCT_ReadCalibChipDataTool.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h"

// STL
#include <atomic>
#include <mutex>

class ISCT_Amp;
class SCT_ID;

namespace InDetDD {
  class SCT_DetectorManager;
}

namespace CLHEP {
  class HepRandomEngine;
}
/**
 * @brief simulation of the SCT front-end electronics
 * working as a SiPreDigitsProcessor
 * models response of ABCD chip amplifiers to 
 * collected charges, also does cross-talk, offset
 * variation and gain variation, in a correlated way
 */
class  SCT_FrontEnd : public extends<AthAlgTool, ISCT_FrontEnd> {

 public:

  /**  constructor */
  SCT_FrontEnd(const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCT_FrontEnd() = default;
  //PJ not needed after merging?!
  /** AlgTool InterfaceID */
  //static const InterfaceID& interfaceID();
  
  /** AlgTool initialize */
  virtual StatusCode initialize() override;
  /** AlgTool finalize */
  virtual StatusCode finalize() override;

  /**
   * process the collection of pre digits: needed to go through all single-strip pre-digits to calculate
   * the amplifier response add noise (this could be moved elsewhere later) apply threshold do clustering
   */
  virtual void process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const override;
  StatusCode doSignalChargeForHits(SiChargedDiodeCollection& collectione) const;
  StatusCode doThresholdCheckForRealHits(SiChargedDiodeCollection& collectione) const;
  StatusCode doThresholdCheckForCrosstalkHits(SiChargedDiodeCollection& collection) const;
  StatusCode doClustering(SiChargedDiodeCollection& collection) const;
  StatusCode prepareGainAndOffset(SiChargedDiodeCollection& collection, const Identifier& moduleId, CLHEP::HepRandomEngine * rndmEngine) const;
  StatusCode prepareGainAndOffset(SiChargedDiodeCollection& collection, int side, const Identifier& moduleId, CLHEP::HepRandomEngine * rndmEngine) const;
  StatusCode randomNoise(SiChargedDiodeCollection& collection, const Identifier& moduleId, CLHEP::HepRandomEngine * rndmEngine) const;
  StatusCode randomNoise(SiChargedDiodeCollection& collection, const Identifier& moduleId, int side, CLHEP::HepRandomEngine * rndmEngine) const;
  StatusCode addNoiseDiode(SiChargedDiodeCollection& collection, int strip, int tbin) const;
  float meanValue(std::vector<float>& calibDataVect) const;
  StatusCode initVectors(int strips) const;

 private:

  FloatProperty m_NoiseBarrel{this, "NoiseBarrel", 1500.0, "Noise factor, Barrel  (in the case of no use of calibration data)"};
  FloatProperty m_NoiseBarrel3{this, "NoiseBarrel3", 1541.0, "Noise factor, Barrel3  (in the case of no use of calibration data)"};
  FloatProperty m_NoiseInners{this, "NoiseInners", 1090.0, "Noise factor, EC Inners  (in the case of no use of calibration data)"};
  FloatProperty m_NoiseMiddles{this, "NoiseMiddles", 1557.0, "Noise factor, EC Middles (in the case of no use of calibration data)"};
  FloatProperty m_NoiseShortMiddles{this, "NoiseShortMiddles", 940.0, "Noise factor, EC Short Middles (in the case of no use of calibration data)"};
  FloatProperty m_NoiseOuters{this, "NoiseOuters", 1618.0, "Noise factor, Ec Outers  (in the case of no use of calibration data)"};
  DoubleProperty m_NOBarrel{this, "NOBarrel", 1.5e-5, "Noise factor, Barrel  (in the case of no use of calibration data)"};
  DoubleProperty m_NOBarrel3{this, "NOBarrel3", 2.1e-5, "Noise factor, Barrel3  (in the case of no use of calibration data)"};
  DoubleProperty m_NOInners{this, "NOInners", 5.0e-9, "Noise Occupancy, EC Inners  (in the case of no use of calibration data)"};
  DoubleProperty m_NOMiddles{this, "NOMiddles", 2.7e-5, "Noise Occupancy, EC Middles (in the case of no use of calibration data)"};
  DoubleProperty m_NOShortMiddles{this, "NOShortMiddles", 2.0e-9, "Noise Occupancy, EC Short Middles (in the case of no use of calibration data)"};
  DoubleProperty m_NOOuters{this, "NOOuters", 3.5e-5, "Noise Occupancy, Ec Outers  (in the case of no use of calibration data)"};
  BooleanProperty m_NoiseOn{this, "NoiseOn", true, "To know if noise is on or off when using calibration data"};
  BooleanProperty m_analogueNoiseOn{this, "AnalogueNoiseOn", true, "To know if analogue noise is on or off"};
  FloatProperty m_GainRMS{this, "GainRMS", 0.031, "Gain spread parameter within the strips for a given Chip gain"};
  FloatProperty m_Ospread{this, "Ospread", 0.0001, "offset spread within the strips for a given Chip offset"};
  FloatProperty m_OGcorr{this, "OffsetGainCorrelation", 0.00001, "Gain/offset correlation for the strips"};
  FloatProperty m_Threshold{this, "Threshold", 1.0, "Threshold"};
  FloatProperty m_timeOfThreshold{this, "TimeOfThreshold", 30.0, "Threshold time"};
  ShortProperty m_data_compression_mode{this, "DataCompressionMode", 1, "Front End Data Compression Mode"};
  ShortProperty m_data_readout_mode{this, "DataReadOutMode", 0, "Front End Data Read out mode Mode"};
  BooleanProperty m_useCalibData{this, "UseCalibData", true, "Flag to set the use of calibration data for noise, Gain,offset etc."};

  ToolHandle<ISCT_Amp> m_sct_amplifier{this, "SCT_Amp", "SCT_Amp", "Handle the Amplifier tool"}; //!< Handle the Amplifier tool
  ToolHandle<ISCT_ReadCalibChipDataTool> m_ReadCalibChipDataTool{this, "SCT_ReadCalibChipDataTool", "SCT_ReadCalibChipDataTool", "Tool to retrieve chip calibration information"}; //!< Handle to the Calibration ConditionsTool

  const InDetDD::SCT_DetectorManager* m_SCTdetMgr{nullptr}; //!< Handle to SCT detector manager
  const SCT_ID* m_sct_id{nullptr}; //!< Handle to SCT ID helper

  mutable std::atomic_int m_strip_max{768}; //!< For SLHC studies

  mutable std::recursive_mutex m_mutex{};
  mutable std::vector<float> m_Offset ATLAS_THREAD_SAFE; //!< generate offset per channel
  mutable std::vector<float> m_GainFactor ATLAS_THREAD_SAFE; //!< generate gain per channel  (added to the gain per chip from calib data)
  mutable std::vector<float> m_NoiseFactor ATLAS_THREAD_SAFE; //!< Kondo: 31/08/07 noise per channel (actually noise per chip from calib data)
  mutable std::vector<double> m_Analogue[3] ATLAS_THREAD_SAFE;  //!< To hold the noise and amplifier response
  mutable std::vector<int> m_StripHitsOnWafer ATLAS_THREAD_SAFE; //!< Info about which strips are above threshold

};

#endif //SCT_FRONTEND_H
