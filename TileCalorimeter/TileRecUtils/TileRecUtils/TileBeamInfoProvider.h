/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILEBEAMINFOPROVIDER_H
#define TILERECUTILS_TILEBEAMINFOPROVIDER_H

/********************************************************************
 *
 * NAME:     TileBeamInfoProvider
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  June 06, 2004
 *
 * PURPOSE:  Re
 *
 *  Input: TileBeamElemContainer
 *  Output: contents of cispar fragment, header, status and trigType
 *  Parameters:
 *    TileBeamElemContainer - Name of input container
 *   
 ********************************************************************/

// Tile includes
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileTriggerContainer.h"
#include "TileEvent/TileLaserObject.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>
#include <inttypes.h>


class TileHWID;
class TileRawChannelCollection;
class StoreGateSvc;
class TileBeamInfoProvider;
class TileDCSSvc;
class ITileBadChanTool;
class IAtRndmGenSvc;
namespace CLHEP {
  class HepRandomEngine;
}



/**
@class coincBoard
@brief Small class holding the information for a cosmic trigger coincidence board 
*/
class coincBoard {
public:
  bool trig[96];  //!< each component of this array correspond to a single tower of a module (12 modules x 8 towers = 96 components)
  uint32_t amp;
};

/**
* @class TileDQstatus
* @brief Class that holds Data Quality fragment information and provides functions to extract the data quality status for channels
* 		 Checks for the following types of errors stored in the Data Quality fragment:
*		 	- Global CRC
*		 	- ROD CRC
*		 	- Front End CRC
*			- BCID Mismatch
*		 	- Header Format
*		 	- Header Parity
*		 	- Sample Format 
*		 	- Sample Parity
*		 	- Single Strobe
*		 	- Double Strobe
*/
class TileDQstatus {

 friend class TileBeamInfoProvider;
 public:

  /** @brief Default constructor */
  TileDQstatus();
  /** @brief Destructor */
  ~TileDQstatus();

  /** @brief returns whether TileDQStatus has been filled with data from TileDQ fragment */
  inline bool   isFilled        ()              const           { return m_isFilled; }
  /** @brief returns gain mode of run */
  inline bool   isBiGain        ()              const           { return m_isBiGain; }
  /** @brief returns True if there are any errors in event */
  inline bool   nonZeroCounter  ()              const           { return m_counter;  }
  /** @brief returns status of single ADC 
  			 returns False if there are any errors*/
  bool          isAdcDQgood     (int partition, int drawer, int ch, int gain) const;
  /** @brief returns status of single channel (if bigain, returns AND of ADCs' status*/
  bool          isChanDQgood    (int partition, int drawer, int ch) const;
  /** @brief True if channel is not fully implemented */
  static int    isChEmpty       (int partition, int drawer, int ch);
  /** @brief returns 0 if event is ok, 1 if empty LG event, 2 if empty HG event, 3 if empty event in both gains */
  inline int checkEmptyEvent(int partition, int drawer, int dmu) const {
	return (m_EmptyEventArray[partition][drawer][dmu][0]+((m_EmptyEventArray[partition][drawer][dmu][1]) << 1));
  }
  
  /** @brief returns 1 if adc channel has any global error */
  inline int checkGlobalErr(int partition, int drawer, int gain) const {
    return m_GlobalCRCErrArray[partition][drawer][gain];
  }
  /** @brief returns 1 if adc channel has global CRC error */
  inline int checkGlobalCRCErr(int partition, int drawer, int gain) const {
    return m_GlobalCRCErrArray[partition][drawer][gain] & 1;
  }
  /** @brief returns 1 if DMU has CRC error originating in ROD */
  inline int checkROD_CRCErr(int partition, int drawer, int dmu, int gain) const {
    return (1 - ((m_ROD_DMUmaskArray[partition][drawer][gain] >> dmu) & 1));
  }
/** @brief returns 1 if DMU has CRC error originating in FE electronics */
  inline int checkFE_CRCErr(int partition, int drawer, int dmu, int gain) const {
      return (1 - ((m_FE_DMUmaskArray[partition][drawer][gain] >> dmu) & 1)); 
  }
  /** @brief returns 1 if DMU has BCID mismatch between DMU and ROD  
  * Since BCID errors in the DQ fragment are determined by comparison of each DMU to DMU1, 
  * if DMU1 is bad and DMUX is also bad, we must compare the individual DMU's BCID to the 
  * actual (ROD) BCID. This is due to the fact that DMU1 is checked against the ROD, while 
  * the others DMUs are checked against DMU1.  This requires access to the digits in the 
  * function checkBCIDErrDetail.  If this behavior is undesired set m_checkDigi to false in 
  * TileBeamInfoProvider.  Setting m_checkDigi=false would cause any BCID errors 
  * simultaneous with DMU1 to be flagged as BAD (conservative approach)
  */
  inline int checkBCIDErr(int partition, int drawer, int dmu, int gain) const {
      return ((m_BCIDErrArray[partition][drawer][gain] & 0x2) && m_checkDigi) 
          ? this->checkBCIDErrDetail(partition,drawer,dmu,gain) :
          ((m_BCIDErrArray[partition][drawer][gain] >> dmu) & 1);
  }
  /** @brief returns 1 if DMU has BCID mismatch between DMU and ROD
  *   This method accesses the TileDigitsContainer in order to explicitly
  *   check the DMU BCID with the ROD BCID.
  **/
  inline int checkBCIDErrDetail(int partition, int drawer, int dmu, int gain) const {
      return (m_BCIDErrArrayDetail[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has header word format error */
  inline int checkHeaderFormatErr(int partition, int drawer, int dmu, int gain) const {
    return (m_HeaderFormatErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has header word parity error */
  inline int checkHeaderParityErr(int partition, int drawer, int dmu, int gain) const {
    return (m_HeaderParityErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has data word format error */
  inline int checkSampleFormatErr(int partition, int drawer, int dmu, int gain) const {
    return (m_SampleFormatErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has data word parity error */
  inline int checkSampleParityErr(int partition, int drawer, int dmu, int gain) const {
    return (m_SampleParityErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has memory parity error */
  inline int checkMemoryParityErr(int partition, int drawer, int dmu, int gain) const {
    return (m_MemoryParityErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has single strobe error */
  inline int checkSingleStrobeErr(int partition, int drawer, int dmu, int gain) const {
    return (m_SingleStrobeErrArray[partition][drawer][gain] >> dmu) & 1;
  }
  /** @brief returns 1 if DMU has double strobe error */
  inline int checkDoubleStrobeErr(int partition, int drawer, int dmu, int gain) const {
    return (m_DoubleStrobeErrArray[partition][drawer][gain] >> dmu) & 1;
  }
 
 protected:

  /** @brief sets flag that DQ status instance has been filled for this event */
  void setFilled(bool filled) {m_isFilled=filled;}
  /** @brief sets flag of gain mode of run */
  void setBiGain(bool biGain) {m_isBiGain=biGain;}
  /** @brief mark all channels/ADC's as DQ good */
  void setAllGood();
  /** @brief sets the ROD BCID stored and used in DQStatus  */
  void setRODBCID(uint32_t BCID)   {m_BCID=BCID;}
  /** @brief sets flag to control the access of TileDigitsContainer  */
  void setCheckDigi(bool checkDigi) {m_checkDigi=checkDigi;}
  /** @brief sets flag that DMU sent an empty event (0xFFFFFFFF) */
  inline void setEmptyEvent(int partition, int drawer, int dmu, int gain, int isEmpty) {
    m_EmptyEventArray[partition][drawer][dmu][gain]=isEmpty;
    m_counter+=isEmpty;
  }
  /** @brief sets TileBeamInfoProvider instance to argument 
   *  @param[in] TBIP Pointer to a TileBeamInfoProvider instance 
   */
  void setTBIP(TileBeamInfoProvider* TBIP) {m_TBIP=TBIP;}
  /** @brief parses DQ fragments and fill error arrays for event*/
  void fillArrays(const TileRawChannelCollection * coll, int gain);
  void fillBCIDErrDetail(int frag, int gain);
  void fillTrips(unsigned int partition, const std::vector<float>& trips, double* rndmVec);

 private:

  /** Pointer to TileBeamInfoProvider that contains TileDQStatus instance */
  TileBeamInfoProvider* m_TBIP;
  /** Boolean storing if DQ fragment has been parsed already */
  bool  m_isFilled;
  /** Boolean storing gain mode of run */
  bool  m_isBiGain;
  /** Boolean flag to control TileDigitsContainer access  */
  bool  m_checkDigi;
  /** Event bunch crossing identification */
  uint32_t   m_BCID;
  /** Counter of non-zero elements in all error arrays */
  int   m_counter;
  /** Array storing whether event is empty */
  short m_EmptyEventArray     [5][64][16][2];
  /** Array of bit masks storing CRC errors for all DMUs*/
  short m_GlobalCRCErrArray   [5][64][2];
  /** Array of bit masks storing CRC errors for all DMUs */
  short m_FE_DMUmaskArray     [5][64][2];
  /** Array of bit masks storing CRC errors for all DMUs */
  short m_ROD_DMUmaskArray    [5][64][2];
  /** Array of bit masks storing BCID errors for all DMUs (from comparison with DMU1) */
  short m_BCIDErrArray        [5][64][2];
  /** Array of bit masks storing BCID errors for all DMUs (from comparison with Digits) */
  short m_BCIDErrArrayDetail  [5][64][2];
  /** Array of bit masks storing Header Format errors for all DMUs */
  short m_HeaderFormatErrArray[5][64][2];
  /** Array of bit masks storing Header Parity errors for all DMUs */
  short m_HeaderParityErrArray[5][64][2];
  /** Array of bit masks storing Sample Format errors for all DMUs */
  short m_SampleFormatErrArray[5][64][2];
  /** Array of bit masks storing Sample Parity errors for all DMUs */
  short m_SampleParityErrArray[5][64][2];
  /** Array of bit masks storing Memory Parity errors for all DMUs */
  short m_MemoryParityErrArray[5][64][2];
  /** Array of bit masks storing Single Strobe errors for all DMUs */
  short m_SingleStrobeErrArray[5][64][2];
  /** Array of bit masks storing Double Strobe errors for all DMUs */
  short m_DoubleStrobeErrArray[5][64][2];

  static const int s_ch2dmuLB[48];
  static const int s_ch2dmuEB[48];
  static const int s_ch2dmuEBspecial[48];
};

/**
@class TileBeamInfoProvider
@brief This class provides the contents of cispar fragment, header, status and trigType from the objects in a TileBeamElemContainer
*/
class TileBeamInfoProvider: public AthAlgTool
                          , virtual public IIncidentListener {
 friend class TileDQstatus;
 public:
  TileBeamInfoProvider(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor

  virtual ~TileBeamInfoProvider(); //!< Destructor

  virtual StatusCode initialize();  //!< intialize method
  virtual StatusCode finalize();    //!< finalize method
  
  virtual void handle(const Incident&) ;   //!< Callback for Incident service 

  static const InterfaceID& interfaceID( ) ; //!< AlgTool InterfaceID
  
  const TileDQstatus* getDQstatus(); //<! Creates TileDQstatus object and fills arrays from DQ fragment

  bool          isChanDCSgood   (int partition, int drawer, int channel) const; 

  inline uint32_t   eventCounter(void)  const { return m_evt; }  
  inline uint32_t      calibMode(void)  const { return m_calibMode; }
  inline int            trigType(void)  const { return m_trigType; }
  inline uint32_t      laserFlag(void)  const { return m_laserFlag; }
  inline uint32_t       digiSize(void)  const { return m_digiSize; }
  inline uint32_t           BCID(void)  const { return m_BCID; }
  inline const uint32_t * cispar(void)  const { return m_cispar; }
  inline const uint32_t * laspar(void)  const { return m_laspar; }
  uint32_t        checkCalibMode(void);
  inline const std::vector<coincBoard> & getCoincBoard(void) const { return m_coincTrig; }

  void setContainers(const TileDigitsContainer * digitsCnt = NULL,
                     const TileRawChannelContainer * rcCnt = NULL,
                     const TileBeamElemContainer * beamElemCnt = NULL);

  const TileRawChannelContainer * dspContainer() const { return m_rcCnt;}
  bool noiseFilterApplied() const { return m_noiseFilterApplied;}
  void setNoiseFilterApplied(bool val=true) {m_noiseFilterApplied=val;}
  bool incompleteDigits() const { return m_incompleteDigits;}

 protected:

  bool m_checkDQ;   //!< if false, skip DQ checks (set to false if container is not found in first event)
  bool m_checkDigi; //!< if false, skip reading of TileDigitsContainer ( /-/-/-/ )
  bool m_checkBeam; //!< if false, skip reading of TileBeamContainer   ( /-/-/-/ )
  bool m_checkDCS;  //!< if false, do not use TileDCSSvc at all
  bool m_simulateTrips;  //! if true simulate drawer trips

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo", 
                                                    "EventInfo", "Input Event info key"};

  SG::ReadHandleKey<TileBeamElemContainer> m_beamElemContainerKey{this, "TileBeamElemContainer", 
                                                                  "", "Input Tile beam elements container key"};

  SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this, "TileDigitsContainer", 
                                                              "", "Input Tile digits container key"};

  SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this, "TileRawChannelContainer", 
                                                                      "", "Input Tile raw channel container key"};

  SG::WriteHandleKey<TileTriggerContainer> m_triggerContainerKey{this, "TileTriggerContainer", 
                                                                 "", "Output Tile trigger container key"};

  SG::WriteHandleKey<TileLaserObject> m_laserObjectKey{this, "TileLaserObject", 
                                                       "", "Output Tile laser object key"};
  

  ServiceHandle<TileDCSSvc>   m_tileDCSSvc; //!< Pointer to TileDCSSvc
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;  //!< Random number service to use
  ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tool which provides trips probabilities also

  const TileHWID* m_tileHWID; //!< Pointer to TileHWID

  int      m_trigType;     //!< Trigger type
  uint32_t m_laserFlag;    //!< Laser Flag
  uint32_t m_cispar[110];  //!< Parameter for CIS
  uint32_t m_laspar[32];   //!< Parameter for LASER

  uint32_t m_calibMode;    //!< Calibration mode

  uint32_t m_digiSize;     //!< number of samples in digits

  uint32_t m_BCID;         //!< BCID in LASTROD

  uint32_t m_evt;          //!< number of event

  std::vector<coincBoard> m_coincTrig; // vector with instances of the coincBoard class

  TileDQstatus m_DQstatus; //<! DQStatus

  const TileDigitsContainer *     m_digitsCnt;
  const TileRawChannelContainer * m_rcCnt;
  const TileBeamElemContainer *   m_beamElemCnt;
  
  CLHEP::HepRandomEngine* m_pHRengine;    //!< Random number generator engine to use
  double* m_rndmVec;

  bool m_noiseFilterApplied;    //!< True if noise filter is applied to DSP container

  bool m_incompleteDigits;    //!< True if not all digits are available (taken from frag 1)

};

// if PMT does not exist, returns 1 
// if DMU does not exist, returns 2
__attribute__((always_inline)) inline
int TileDQstatus::isChEmpty(int partition, int drawer, int ch) {
  
  switch (partition) {
    case 1:
      return s_ch2dmuLB[ch];
    case 2:
      return s_ch2dmuLB[ch];    
    case 3:
      return (drawer != 14) ? s_ch2dmuEB[ch] : s_ch2dmuEBspecial[ch];
    case 4:
      return (drawer != 17) ? s_ch2dmuEB[ch] : s_ch2dmuEBspecial[ch];
    default:
      return 0;
  }  
}

#endif
