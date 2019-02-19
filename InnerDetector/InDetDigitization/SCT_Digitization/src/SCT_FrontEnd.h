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
class  SCT_FrontEnd : public AthAlgTool, virtual public ISCT_FrontEnd {

 public:

  /**  constructor */
  SCT_FrontEnd(const std::string& type, const std::string& name, const IInterface* parent);
  /** Destructor */
  virtual ~SCT_FrontEnd() = default;
  //PJ not needed after merging?!
  /** AlgTool InterfaceID */
  //static const InterfaceID& interfaceID();
  
  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();

  /**
   * process the collection of pre digits: needed to go through all single-strip pre-digits to calculate
   * the amplifier response add noise (this could be moved elsewhere later) apply threshold do clustering
   */
  virtual void process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const;
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

  float m_NoiseBarrel;                      //!< Noise factor, Barrel  (in the case of no use of calibration data)
  float m_NoiseBarrel3;                     //!< Noise factor, Barrel3  (in the case of no use of calibration data)
  float m_NoiseInners;                      //!< Noise factor, EC Inners  (in the case of no use of calibration data)
  float m_NoiseMiddles;                     //!< Noise factor, EC Middles (in the case of no use of calibration data)
  float m_NoiseShortMiddles;                //!< Noise factor, EC Short Middles (in the case of no use of calibration data)
  float m_NoiseOuters;                      //!< Noise factor, Ec Outers  (in the case of no use of calibration data)
  double m_NOBarrel;                        //!< Noise factor, Barrel  (in the case of no use of calibration data)
  double m_NOBarrel3;                       //!< Noise factor, Barrel3  (in the case of no use of calibration data)
  double m_NOInners;                        //!< Noise Occupancy, EC Inners  (in the case of no use of calibration data)
  double m_NOMiddles;                       //!< Noise Occupancy, EC Middles (in the case of no use of calibration data)
  double m_NOShortMiddles;                  //!< Noise Occupancy, EC Short Middles (in the case of no use of calibration data)
  double m_NOOuters;                        //!< Noise Occupancy, Ec Outers  (in the case of no use of calibration data)
  bool m_NoiseOn;                           //!< To know if Noise is on or off when using calibration data)
  bool m_analogueNoiseOn;                   //!< To know if analogue noise is on or off
  float m_GainRMS;                          //!< Gain factor for gain generation
  float m_Ospread;                          //!< Offset factor for offset generation
  float m_OGcorr;                           //!< Gain-offset correlation factor
  float m_Threshold;                        //!< Threshold
  float m_timeOfThreshold;                  //!< Time for the threshold factor
  short m_data_compression_mode;            //!< To set the data compression mode
  short m_data_readout_mode;                //!< To set the data read out mode
  bool m_useCalibData;                      //!< Flag to set the use of calibration data for noise, Gain,offset etc.
  mutable int m_strip_max;                  //!< For SLHC studies

  mutable std::mutex m_mutex;
  mutable std::vector<float> m_Offset;      //!< generate offset per channel
  mutable std::vector<float> m_GainFactor;  //!< generate gain per channel  (added to the gain per chip from calib data)
  mutable std::vector<float> m_NoiseFactor; //!< Kondo: 31/08/07 noise per channel (actually noise per chip from calib data)
  mutable std::vector<double> m_Analogue[3];  //!< To hold the noise and amplifier response
  mutable std::vector<int> m_StripHitsOnWafer; //!< Info about which strips are above threshold

  const InDetDD::SCT_DetectorManager* m_SCTdetMgr;        //!< Handle to SCT detector manager
  const SCT_ID*                       m_sct_id;           //!< Handle to SCT ID helper
  ToolHandle<ISCT_Amp> m_sct_amplifier{this, "SCT_Amp", "SCT_Amp", "Handle the Amplifier tool"}; //!< Handle the Amplifier tool
  ToolHandle<ISCT_ReadCalibChipDataTool> m_ReadCalibChipDataTool{this, "SCT_ReadCalibChipDataTool", "SCT_ReadCalibChipDataTool", "Tool to retrieve chip calibration information"}; //!< Handle to the Calibration ConditionsTool

};

#endif //SCT_FRONTEND_H
