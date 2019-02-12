/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEINFO_H
#define TILECONDITIONS_TILEINFO_H
//****************************************************************************
// Filename : TileInfo.h
// Author   : F. Merritt, Z. Wu
// Date     : May, 2002
//
// :DESCRIPTION:
//     This is the first step toward creating an interface to Tile Conditions
// Database. TileInfo should be used to return any detector-dependent
// constants or information regarding the Tile Calorimeter.
//
// HISTORY:
// Jul 2007: Major clearup and reorganization, including COOL DB access (NGO)
// 10th Oct 06: add comments on Covariance matrix addition ( F Spano')
// 08May02: Add NchannelsPerCell to return # channels belonging to each cell.
//***************************************************************************

#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TileCablingSvc.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloGain.h"
#include "StoreGate/DataHandle.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileFragHash.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <vector>
#include <map>
#include <stdint.h>
#include "TMatrixD.h"

class TileID;
class TileTBID;
class TileHWID;
class HWIdentifier;
class TileInfoLoader;
class TileCablingService;
class IdContext;
class TileCalibDrawerFlt;

static const CLID CLID_TileInfo = 2910;

class TileInfo : public DataObject {
  friend class TileInfoLoader;

 public:

  typedef std::vector<const TileCalibDrawerFlt*> DrawerVecFlt;

  TileInfo(ISvcLocator *svcLocator);
  virtual ~TileInfo();
  static const CLID& classID(void) { return CLID_TileInfo;}
  virtual const CLID& clID(void) const { return CLID_TileInfo;}


  //==================================================================
  //==
  //== Accessor methods for simple constants
  //==
  //==================================================================

  /** Returns the number of readout channels connected to a given cell */
  int NchannelsPerCell(const Identifier& cell_id) const
    { return m_tileID->is_tile_gapscin(cell_id) ? 1:2; }

  /** Returns the maximum ADC output (10 bits --> 1023) */
  int ADCmax() const {return m_adcmax;}
  /** Returns the number of sammples (digits) per event */
  int NdigitSamples() const {return m_nSamples;}
  /** The sample at which the pulse should ideally peak */
  int ItrigSample() const {return m_iTrigSample;}
  /** Number of photo electrons */
  int NPhElec() const { return m_nPhElec; }
  /** Number of photo electrons per sample */
  int NPhElec(int samp) const { return m_nPhElecVec[samp]; }


  /** TileHit to TileCell energy conversion.
      Returns the factor for converting TileHit amplitude to TileCell energy in simulation*/
  double HitCalib(const Identifier& pmt_id) const;

  //==================================================================
  //==
  //== Accessor methods for pedestal noise and thresholds
  //==
  //==================================================================


  /** Noise switched on/off? */
  bool TileNoise() const {return m_tileNoise;}
  /** Coherent noise switched on/off? */
  bool TileCoherNoise() const {return m_tileCoherNoise;}
  /** Zero suppression switched on/off? */
  bool TileZeroSuppress() const {return m_tileZeroSuppress;}

  /** Returns the decomposed covariance matrix*/
  const TMatrixD * DecoCovariance(int ros, int drawer, int hilo) const;

  /** Return the threshold value for good (filtered) TileRawChannels
      @param gain is ignored for now */
  double ThresholdRawChannel(int /* gain */) const {return m_thresholdRawChannel;}

  /** Return the threshold value for good TileDigits (cut applied to in-time digit only)
      @param gain is ignored for now */
  double ThresholdDigits(int /* gain */) const {return m_thresholdDigits;}


  //==================================================================
  //==
  //== Accessor methods for TTL1 parameters
  //==
  //==================================================================

  /** Returns the factor which converts amplitude in pCb to mV in TTL1 */
  double TTL1Calib(const Identifier& /* tt_id */) const { return m_ttL1Calib; }
  /** Returns the factor which converts amplitude in pCb to ADC counts in Muon Receiver */
  double MuRcvCalib(const Identifier& /* channel_id */) const { return m_MuRcvCalib; }
  /** Returns the factor which converts amplitude in pCb to mV in muon L1 output  */
  double MuL1Calib(const Identifier& /* tt_id */) const { return m_muL1Calib; }
  /** Returns the factor which converts amplitude in pCb to mV in TTL1 for MBTS */
  double MBTSL1Calib(const Identifier& /* tt_id */) const { return m_mbtsL1Calib; }

  /**  Returns the sigma (in mV) of Noise in TTL1 adcs */
  double TTL1NoiseSigma(const Identifier& /* tt_id */) const { return m_ttL1NoiseSigma; }
  /**  Returns the sigma (in ADC counts) of Noise in Muon Receiver adcs */
  double MuRcvNoiseSigma(const Identifier& /* channel_id */) const { return m_MuRcvNoiseSigma; }
  /** Returns the sigma (in mV) of Noise in muon L1 output */
  double MuL1NoiseSigma(const Identifier& /* tt_id */) const { return m_muL1NoiseSigma; }
  /** Returns the sigma (in mV) of Noise in TTL1 adcs for MBTS */
  double MBTSL1NoiseSigma(const Identifier& /* tt_id */) const { return m_mbtsL1NoiseSigma; }

  /** Returns the threshold (in mV) for TTL1 adcs */
  double TTL1Thresh(const Identifier& /* tt_id */) const { return m_ttL1Thresh; }
  /** Returns the threshold (in ADC counts) for Muon Receiver adcs */
  double MuRcvThresh(const Identifier& /* channel_id */) const { return m_MuRcvThresh; }
  /** Returns the threshold (in mV) for muon L1 output */
  double MuL1Thresh(const Identifier& /* tt_id */) const { return m_muL1Thresh; }
  /** Returns the threshold (in mV) for TTL1 adcs for MBTS */
  double MBTSL1Thresh(const Identifier& /* tt_id */) const { return m_mbtsL1Thresh; }

  /** Returns the pedestal (in mV) for TTL1 adcs */
  double TTL1Ped(const Identifier& /* tt_id */) const { return m_ttL1Ped; }
  /** Returns the pedestal (in ADC counts) for Muon Receiver adcs */
  double MuRcvPed(const Identifier& /* channel_id */) const { return m_MuRcvPed; }
  /** Returns the pedestal (in mV) for muon L1 output */
  double MuL1Ped(const Identifier& /* tt_id */) const { return m_muL1Ped; }
  /** Returns the pedestal (in mV) for TTL1 adcs for MBTS */
  double MBTSL1Ped(const Identifier& /* tt_id */) const { return m_mbtsL1Ped; }

  /** Returns max possible value (in mV) for TTL1 adcs */
  double TTL1Max(const Identifier& /* tt_id */) const { return m_ttL1Max; }
  /** Returns max possible value (in ADC counts) for Muon Receiver adcs */
  double MuRcvMax(const Identifier& /* channel_id */) const { return m_MuRcvMax; }
  /** Returns max possible value  (in mV) for muon L1 output */
  double MuL1Max(const Identifier& /* tt_id */) const { return m_muL1Max; }
  /** Returns max possible value  (in mV) for TTL1 adcs for MBTS */
  double MBTSL1Max(const Identifier& /* tt_id */) const { return m_mbtsL1Max; }

  /** Returns the factor which converts e_dep (HitAmp) to mV in Cosmics board*/
  double TTL1CosmicsCalib(const Identifier& /* tt_id */) const { return (m_ttL1Calib * m_ttL1CosmicsGain); }
  /** Returns the sigma (in mV) of Noise in TTL1 Cosmics*/
  double TTL1CosmicsNoiseSigma(const Identifier& /* tt_id */) const { return (m_ttL1NoiseSigma * m_ttL1CosmicsGain); }
  /** Returns the gain of the Cosmics Trigger Board*/
  double TTL1CosmicsGain(const Identifier& /* tt_id */) const { return m_ttL1CosmicsGain; }
  /** Returns the threshold of the Cosmics Trigger Board */
  double TTL1CosmicsThresh(const Identifier& /* tt_id */) const { return m_ttL1CosmicsThresh; }
  /** Returns the pedestal of the Cosmics Trigger signal */
  double TTL1CosmicsPed(const Identifier& /* tt_id */) const { return (m_ttL1Ped * m_ttL1CosmicsGain); }

  //==================================================================
  //==
  //== Accessor methods for shaping functions and paramters
  //==
  //==================================================================

  /** Return high gain shape vector to multiply the amplitude of a TileHit to produce TileDigit */
  const std::vector<double>& digitsShapeHi() const {return m_digitsShapeHi;}
  /** Return low gain shape vector to multiply the amplitude of a TileHit to produce TileDigit */
  const std::vector<double>& digitsShapeLo() const {return m_digitsShapeLo;}

  /** Return derivative of bins in high gain DigitShape */
  const std::vector<double>& digitsDerivativeHi() const {return m_digitsDerivativeHi;}
  /** Return derivative of bins in log gain DigitShape */
  const std::vector<double>& digitsDerivativeLo() const {return m_digitsDerivativeLo;}

  /** Return shape vector with full binning to produce the TileDigits from sub-hits */
  const std::vector<double>& digitsFullShapeHi() const {return m_digitsFullShapeHi;}
  /** Return shape vector with full binning to produce the TileDigits from sub-hits */
  const std::vector<double>& digitsFullShapeLo() const {return m_digitsFullShapeLo;}

  /** Return number of bins in high gain DigitShape */
  int digitsNBinsHi() const {return m_digitsNBinsHi;}
  /** Return number of bins in low gain DigitShape */
  int digitsNBinsLo() const {return m_digitsNBinsLo;}

  /** Return index of in-time bin in high gain DigitShape */
  int digitsTime0BinHi() const {return m_digitsTime0BinHi;}
  /** Return index of in-time bin in low gain DigitShape */
  int digitsTime0BinLo() const {return m_digitsTime0BinLo;}

  /** Return number of bins per bunch-crossing NGO high/lo makes no sense, right?*/
  int digitsBinsPerXHi() const {return m_digitsBinsPerXHi;}
  /** Return number of bins per bunch-crossing */
  int digitsBinsPerXLo() const {return m_digitsBinsPerXLo;}

  /** Return bunch-X TTL1 shape vector */
  const std::vector<double>& ttl1Shape() const {return m_TTL1Shape;}
  /** Return bunch-X Muon Receiver shape vector */
  const std::vector<double>& MuRcvShape() const {return m_MuRcvShape;}
  /** Return bunch-X shape vector for MBTS */
  const std::vector<double>& MuL1Shape() const {return m_MuL1Shape;}

  /** Return full TTL1 shape vector */
  const std::vector<double>& ttl1FullShape() const {return m_TTL1FullShape;}
  /** Return full Muon Receiver shape vector */
  const std::vector<double>& MuRcvFullShape() const {return m_MuRcvFullShape;}
  /** Return full TTL1 shape vector for MBTS */
  const std::vector<double>& MuL1FullShape() const {return m_MuL1FullShape;}

  /** Return number of bins in TTL1Shape */
  int ttl1NBins() const {return m_TTL1NBins;}
  /** Return number of bins in Muon Receiver shape*/
  int MuRcvNBins() const {return m_MuRcvNBins;}
  /** Return number of bins in TTL1Shape for MBTS */
  int MuL1NBins() const {return m_MuL1NBins;}

  /** Return index of in-time bin in TTL1Shape */
  int ttl1Time0Bin() const {return m_TTL1Time0Bin;}
  /** Return index of in-time bin in Muon Receiver shape*/
  int MuRcvTime0Bin() const {return m_MuRcvTime0Bin;}
  /** Return index of in-time bin in TTL1Shape for MBTS */
  int MuL1Time0Bin() const {return m_MuL1Time0Bin;}

  /** Return number of TTL1 bins per bunch-crossing */
  int ttl1BinsPerX() const {return m_TTL1BinsPerX;}
  /** Return number of Muon Receiver bins per bunch-crossing */
  int MuRcvBinsPerX() const {return m_MuRcvBinsPerX;}
  /** Return number of TTL1 bins per bunch-crossing for MBTS */
  int MuL1BinsPerX() const {return m_MuL1BinsPerX;}

  /** NGO need description */
  void ttl1Shape(const int nsamp, const int itrig, const double phase,
		 std::vector<double> &ttl1shape) const;

  void muRcvShape(const int nsamp, const int itrig, const double phase,
                 std::vector<double> &murcvshape) const;


  //==================================================================
  //==
  //== Accessor methods for external structures
  //==
  //==================================================================

  /** Return pointer to TilePulseShapes*/
  TilePulseShapesStruct * getPulseShapes() const {return m_pulseShapes->TilePSstruct();}

  /** Return pointer to TileOptFilterWeight */
  TileOptFilterWeightsStruct * getOptFilterWeights() const {return m_OptFilterWeights->TileOFWstruct();}

  /** Return pointer to TileOptFilterCorrelation */
  TileOptFilterCorrelationStruct * getOptFilterCorrelation() const {return m_OptFilterCorrelation->TileOFCorrstruct();}


  //==================================================================
  //==
  //== Temporary backwards compatible functions
  //==
  //==================================================================

  /** TileHit to TileCell energy conversion for ancillary testbeam detectors.
      Return the factor for converting TileHit amplitude to
      TileCell energy in the simulation for ancillary detectors at the testbeam
      This method always returned 1. in the past, keep it for compatibility for now. */
  double BeamElemHitCalib(const Identifier& /* pmt_id */) const {return 1.;}

  /** Method returns and returned  always 1.
      Kept for compatibility for now. */
  double BeamElemChannelCalib(const HWIdentifier& /* channel_id */) const {return 1.;}

  /** NGO Need description. Do we need this function? */
  double ttl1Phase(const int /* posneg */, const int /* ieta */, const int /* iphi */) const {return 0.;}

  /**
   * @brief Conversion from ADC sigma noise to OF sigma noise
   * @return Conversion factor from ADC sigma noise to OF sigma noise
   */
  float getNoiseScaleFactor(void) const;

 private:

  /** Initialization of the TileInfo object. */
  StatusCode initialize();

  /** Pointer to ServiceLocator */
  ISvcLocator *m_svcLocator;

  /** Returns a handle to the MessageSvc */
  IMessageSvc* msgSvc() const;

  const TileID*             m_tileID;
  const TileHWID*           m_tileHWID;
  const TileTBID*           m_tileTBID;
  const TileCablingService* m_cabling;
  const IdContext*          m_channel_context;
  const IdContext*          m_drawer_context;


  double m_emscaleA;     //!< 1/sampling fraction for all normal cells
  double m_emscaleC[64]; //!< 1/sampling fraction for all C10 cells
  double m_emscaleE[16]; //!< 1/sampling fraction for special C10 and E1-E4 cells [9]=C10, [10]=E1, [11]=E2, [13]=E3, [15]=E4
  double m_emscaleMBTS[3]; //!< 1/sampling fraction for inner/outer MBTS and also for E4'

  int    m_adcmax;
  int    m_nSamples;
  int    m_iTrigSample;

  /** scale factor from digital noise to Opt Filter noise
      for various opt filter algoritms
      0 - reserved
      1 - opt filter without iterations,
      2 - opt filter with iterations
      3 - fit method */

  double m_noiseScaleFactor[4];
  int m_noiseScaleIndex;

  bool   m_tileNoise;
  bool   m_tileCoherNoise;
  bool   m_tileZeroSuppress;
  double m_thresholdRawChannel;
  double m_thresholdDigits;

  double m_ttL1Calib;
  double m_ttL1NoiseSigma;
  double m_ttL1Thresh;
  double m_ttL1Ped;
  double m_ttL1Max;

  double m_MuRcvCalib;
  double m_MuRcvNoiseSigma;
  double m_MuRcvThresh;
  double m_MuRcvPed;
  double m_MuRcvMax;

  double m_muL1Calib;
  double m_muL1NoiseSigma;
  double m_muL1Thresh;
  double m_muL1Ped;
  double m_muL1Max;

  double m_mbtsL1Calib;
  double m_mbtsL1NoiseSigma;
  double m_mbtsL1Thresh;
  double m_mbtsL1Ped;
  double m_mbtsL1Max;

  double m_ttL1CosmicsGain;
  double m_ttL1CosmicsThresh;

  std::vector<double> m_digitsShapeHi;
  std::vector<double> m_digitsDerivativeHi;
  std::vector<double> m_digitsShapeLo;
  std::vector<double> m_digitsDerivativeLo;
  std::vector<double> m_digitsFullShapeHi;
  std::vector<double> m_digitsFullShapeLo;
  int m_digitsNBinsHi;
  int m_digitsNBinsLo;
  int m_digitsTime0BinHi;
  int m_digitsTime0BinLo;
  int m_digitsBinsPerXHi;
  int m_digitsBinsPerXLo;

  std::vector<double> m_TTL1Shape;
  std::vector<double> m_TTL1FullShape;
  int m_TTL1NBins;
  int m_TTL1Time0Bin;
  int m_TTL1BinsPerX;

  std::vector<double> m_MuRcvShape;
  std::vector<double> m_MuRcvFullShape;
  int m_MuRcvNBins;
  int m_MuRcvTime0Bin;
  int m_MuRcvBinsPerX;

  std::vector<double> m_MuL1Shape;
  std::vector<double> m_MuL1FullShape;
  int m_MuL1NBins;
  int m_MuL1Time0Bin;
  int m_MuL1BinsPerX;

  TilePulseShapes *m_pulseShapes;

  TileOptFilterWeights *m_OptFilterWeights;
  TileOptFilterWeights *m_OptFilterCorrelation;

  double m_emscaleTB[32];
  double m_mev2adcTB[32];

  ServiceHandle<TileCablingSvc>       m_tileCablingSvc;

  int    m_nPhElec;
  int    m_nPhElecVec[7];

  // Three dimensional vector of the
  // decomposed covariance matrices (partition x module x gain)
  //
  std::vector <std::vector <std::vector <TMatrixD *> > > m_decoCovaria;

};


// inlines
inline
float TileInfo::getNoiseScaleFactor(void) const {
  return m_noiseScaleFactor[m_noiseScaleIndex];
}

#endif  //TILECONDITIONS_TILEINFO_H
