/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIP_DIGITIZATION_STRIP_FRONTEND_H
#define STRIP_DIGITIZATION_STRIP_FRONTEND_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_FrontEnd.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h"

#include "SiDigitization/SiChargedDiodeCollection.h"

class ISCT_Amp;
class ISCT_ReadCalibChipDataSvc;
class SCT_ID;

namespace InDetDD {
  class SCT_DetectorManager;
}

namespace CLHEP {
  class HepRandomEngine;
}
/**
 * @brief simulation of the Strip front-end electronics
 * working to response of ABC chip amplifiers to
 * collected charges, also does cross-talk, offset
 * variation and gain variation, in a correlated way
**/

class  StripFrontEnd : public AthAlgTool, virtual public ISCT_FrontEnd {

 public:

  /**  constructor */
  StripFrontEnd(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** Destructor */
  virtual ~StripFrontEnd();

  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();

  /**
   * process the collection of pre digits: needed to go through all single-strip pre-digits to calculate
   * the amplifier response add noise (this could be moved elsewhere later) apply threshold do clustering
   */
  virtual void process(SiChargedDiodeCollection &collection) const;
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) {  m_rndmEngine = rndmEngine; };
  StatusCode doSignalChargeForHits(SiChargedDiodeCollection &collectione) const;
  StatusCode doThresholdCheckForRealHits(SiChargedDiodeCollection &collectione) const;
  StatusCode doThresholdCheckForCrosstalkHits(SiChargedDiodeCollection &collection) const;
  StatusCode doClustering(SiChargedDiodeCollection &collection) const;
  StatusCode prepareGainAndOffset(SiChargedDiodeCollection &collection, const Identifier & moduleId) const;
  StatusCode prepareGainAndOffset(SiChargedDiodeCollection &collection, const int & side, const Identifier & moduleId) const;
  StatusCode randomNoise(SiChargedDiodeCollection &collection, const Identifier & moduleId) const;
  StatusCode randomNoise(SiChargedDiodeCollection &collection, const Identifier & moduleId, const int & side) const;
  StatusCode addNoiseDiode(SiChargedDiodeCollection &collection, const int & strip, const int & tbin) const;
  float meanValue(std::vector<float> & calibDataVect) const;
  StatusCode initVectors(const int & strips) const;

 private:
  enum CompressionMode { Level_X1X=1, Edge_01X=2, AnyHit_1XX_X1X_XX1=3 }; // Used for m_data_compression_mode (DataCompressionMode)
  enum ReadOutMode { Condensed=0, Expanded=1 }; // Used for m_data_readout_mode (DataReadOutMode)

  bool m_PulseAveragingFlag;

  float m_NoiseBarrel ;                     //!< Noise factor, Barrel  (in the case of no use of calibration data)
  float m_NoiseBarrel3 ;                    //!< Noise factor, Barrel3  (in the case of no use of calibration data)
  float m_NoiseInners ;                     //!< Noise factor, EC Inners  (in the case of no use of calibration data)
  float m_NoiseMiddles ;                    //!< Noise factor, EC Middles (in the case of no use of calibration data)
  float m_NoiseShortMiddles ;               //!< Noise factor, EC Short Middles (in the case of no use of calibration data)
  float m_NoiseOuters ;                     //!< Noise factor, Ec Outers  (in the case of no use of calibration data)
  double m_NOBarrel ;                       //!< Noise factor, Barrel  (in the case of no use of calibration data)
  double m_NOBarrel3 ;                      //!< Noise factor, Barrel3  (in the case of no use of calibration data)
  double m_NOInners ;                       //!< Noise Occupancy, EC Inners  (in the case of no use of calibration data)
  double m_NOMiddles ;                      //!< Noise Occupancy, EC Middles (in the case of no use of calibration data)
  double m_NOShortMiddles ;                 //!< Noise Occupancy, EC Short Middles (in the case of no use of calibration data)
  double m_NOOuters ;                       //!< Noise Occupancy, Ec Outers  (in the case of no use of calibration data)
  bool m_NoiseOn ;                          //!< To know if Noise is on or off when using calibration data)
  bool m_addNoiseDiode;                     //!< To switch off the creation of noise diodes
  bool m_analogueNoiseOn ;                  //!< To know if analogue noise is on or off
  float m_GainRMS ;                         //!< Gain factor for gain generation
  float m_Ospread ;                         //!< Offset factor for offset generation
  float m_OGcorr ;                          //!< Gain-offset correlation factor
  float m_Threshold ;                       //!< Threshold
  float m_timeOfThreshold ;                 //!< Time for the threshold factor
  short m_data_compression_mode;            //!< To set the data compression mode
  short m_data_readout_mode;                //!< To set the data read out mode
  bool m_useCalibData;                      //!< Flag to set the use of calibration data for noise, Gain,offset etc.
  mutable int m_strip_max;                  //!< For SLHC studies

  mutable std::vector<float> m_Offset;      //!< generate offset per channel
  mutable std::vector<float> m_GainFactor;  //!< generate gain per channel  (added to the gain per chip from calib data)
  mutable std::vector<float> m_NoiseFactor; //!< Kondo: 31/08/07 noise per channel (actually noise per chip from calib data)
  mutable std::vector<double> m_Analogue[3];  //!< To hold the noise and amplifier response
  mutable std::vector<int> m_StripHitsOnWafer; //!< Info about which strips are above threshold

  StatusCode                          m_sc;               //!< Help variable to take care of returned status codes
  const InDetDD::SCT_DetectorManager* m_SCTdetMgr;        //!< Handle to SCT detector manager
  const SCT_ID*                       m_sct_id;           //!< Handle to SCT ID helper
  ToolHandle<ISCT_Amp> m_sct_amplifier;                   //!< Handle the Amplifier tool
  ServiceHandle<ISCT_ReadCalibChipDataSvc> m_ReadCalibChipDataSvc; //!< Handle to the Calibration ConditionsService

  CLHEP::HepRandomEngine   *m_rndmEngine;        //!< Random number generation engine

};

#endif
