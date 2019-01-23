// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/TileDQstatus.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2018
 * @brief Information produced by TileDQstatusAlg
 *        (used to be done by TileBeamInfoProvider).
 */


#ifndef TILEEVENT_TILEDQSTATUS_H
#define TILEEVENT_TILEDQSTATUS_H


#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


class TileDQstatusTool;
class TileRawChannelCollection;
class TileDigitsContainer;
class MsgStream;


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
*
* Also holds a few other pieces of information, such as trigger type,
* calibration mode, and CIS parameters.
*/
class TileDQstatus
{
  friend class TileDQstatusTool;

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

  /// A few extra items (from TileBeamInfoProvider).

  /** @brief True if not all digits are available. */
  bool incompleteDigits() const { return m_incompleteDigits;}
  void setIncompleteDigits (bool incomplete);

  /** @brief Calibration mode. */
  uint32_t      calibMode()  const { return m_calibMode; }
  void setCalibMode (uint32_t calibMode);

  /** @brief CIS parameters. */
  const uint32_t* cispar()  const { return m_cispar; }
  uint32_t* cispar();
  
  /** @brief Trigger type. */
  int            trigType()  const { return m_trigType; }
  void setTrigType (int trigType);

  uint32_t RODBCID() const   {return m_BCID;}


protected:

  /** @brief sets flag that DQ status instance has been filled for this event */
  void setFilled(bool filled) {m_isFilled=filled;}
  /** @brief sets flag of gain mode of run */
  void setBiGain(bool biGain) {m_isBiGain=biGain;}
  /** @brief mark all channels/ADC's as DQ good */
  void setAllGood();
  /** @brief sets the ROD BCID stored and used in DQStatus  */
  void setRODBCID(uint32_t BCID)   {m_BCID=BCID;}
  /** @brief sets flag that DMU sent an empty event (0xFFFFFFFF) */
  inline void setEmptyEvent(int partition, int drawer, int dmu, int gain, int isEmpty) {
    m_EmptyEventArray[partition][drawer][dmu][gain]=isEmpty;
    m_counter+=isEmpty;
  }
  /** @brief parses DQ fragments and fill error arrays for event*/
  void fillArrays(const TileRawChannelCollection * coll,
                  const TileDigitsContainer* digitsCnt, int gain);
  void fillBCIDErrDetail(const TileDigitsContainer* digitsCnt,
                         int frag, int gain);
  void fillTrips(unsigned int partition, const std::vector<float>& trips, double* rndmVec,
                 MsgStream& msg);

 private:
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

  /** True if not all digits are available. */
  bool m_incompleteDigits;

  /** Calibration mode. */
  uint32_t m_calibMode;

  /** CIS parameters. */
  uint32_t m_cispar[110];

  /** Trigger type. */
  int m_trigType;

  static const int s_ch2dmuLB[48];
  static const int s_ch2dmuEB[48];
  static const int s_ch2dmuEBspecial[48];
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


CLASS_DEF (TileDQstatus, 221767675, 0)


#endif // not TILEEVENT_TILEDQSTATUS_H
