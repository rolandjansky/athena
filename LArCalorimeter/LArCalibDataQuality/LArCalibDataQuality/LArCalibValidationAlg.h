//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file  LArCalibValidatonAlg.h
 * @author Walter Lampl <walter.lampl @cern.ch>
 * @date Feb 2008
 * @brief Base-class for algorithms to validated LAr calibration constants
 */


#ifndef LARCALIBVALIDATIONALG_H
#define LARCALIBVALIDATIONALG_H
 
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecConditions/LArBadChannelMask.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"

#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"


/** 
  * @class LArCalibValidationAlg
  * Base class for algorithms  validating LAr Calibration Constants
  * Base class to validate Liquid Argon Calibration Constants by comparing them channel-by-channel
  * with a set reference constants. 
  * Any specific validation algorithm (eg. for Pedestals) should derive from this class and implement 
  * a @validateChannel method specific to the data type.
  */

template<class CONDITIONSCONTAINER>
class LArCalibValidationAlg:public AthAlgorithm {
 
public:
  /** 
   * @brief Regular algorithm constructor
   */
  LArCalibValidationAlg (const std::string& name, ISvcLocator* pSvcLocator);

  /** 
    * @brief Destructor
    */
  ~LArCalibValidationAlg();

 /**
   * @brief Standard initialization method.
   * This method is currently emty 
   */
  StatusCode initialize();

  /**
    * @brief Standard execute method
    * This method has to be emtpy since all the job is done in finalize
    */
  StatusCode execute() {return StatusCode::SUCCESS;}

  /**
    * @brief Standard finalize method
    * Loops over all channels and all gains and calls the @c validateChannel method.
    */
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

protected:
  typedef typename CONDITIONSCONTAINER::ConstConditionsMapIterator CONTIT;
  typedef typename CONDITIONSCONTAINER::LArCondObj LArCondObj;
  typedef typename CONDITIONSCONTAINER::MultChanCollection MULTICHANCOLL;

  /**
    * @brief Method executed before the loop over all channels
    * This method can be implemented by the deriving algorithm
    */
  virtual StatusCode preLoop();

  
  /**
    * @brief Abstract method to validate a single readout channels
    * @param ref The reference object
    * @param val The object to be validated
    * @param chid Online identifier of the channel in question. 
    * @param gain Gain in question
    * This method needs to be implemeted by the deriving algorithm
    */
  virtual bool validateChannel(const LArCondObj& ref, const LArCondObj& val, const HWIdentifier chid, const int gain, const LArOnOffIdMapping *cabling,const LArBadChannelCont *bcCont)=0;

  /**
    * @brief Method executed after the loop over all channels
    * The implementation in the base class writes out only the number of problematic channels.
    * Could be overloaded in the derived class for more specific about information about 
    * newly found bad channels.
    * The return value of this method is also the return value of the @c finalize method.
    */
  virtual StatusCode summary(const LArOnOffIdMapping *cabling,const LArBadChannelCont *bcCont);

 /**
    * @brief Textual representation of gain and location of a certain cell or FEB
    * @param chid Online identifier of the cell or FEB
    * @param gain Gain in question, a value >2 won't be printed
    * @param iFeb If @c chid should be interpreted as FEB identifer
    * @return string
    * Creates a string containing the Side, Barrel-EC, FT, Slot, 
    * FEB-Channel in human-readable form
    */
  const std::string channelDescription(const HWIdentifier& chid, const LArOnOffIdMapping *cabling, const LArBadChannelCont *bcCont, const unsigned gain=99, bool isFeb=false) const;


private:
  /** 
    * @brief Verify if all FEBs of each Cool-Channel is present 
    * @return False if at least one incomplete COOL channel is found, true otherwise
    * Calls @c patchMissingFEBs if @c m_patchMissingFebs is true
    */
  bool checkCoolChannelCompleteness(const LArOnOffIdMapping *cabling, const LArBadChannelCont *bcCont);
 
 /** 
    * @brief Verify if all COOL channels in the ref container are also in val
    * @return False if at least one incomplete COOL channel is found, true otherwise
    */
  bool checkNumberOfCoolChannels(const LArOnOffIdMapping *cabling, const LArBadChannelCont *bcCont) const;

  typedef std::vector<std::pair<HWIdentifier,unsigned> > FEBANDGAIN_t;
   /** 
    * @brief Fills channels of missing FEBs 
    * @param febAndGain A vector of (febId,gain) pairs
    * @return True on success, false otherwise
    * Inserts the corresponding channel from the reference container 
    * as correction channel to the validation container
    */
  bool patchMissingFEBs(const FEBANDGAIN_t& febAndGain, const LArOnOffIdMapping *cabling, const LArBadChannelCont *bcCont);

  void febOutput(const HWIdentifier& febid, const unsigned gain, const unsigned nGood, const unsigned nBad);

  void findFailedPatterns(const LArOnOffIdMapping *cabling,const LArBadChannelCont *bcCont);

  std::vector<std::string> m_gainMap;

protected:
  /** Message level for reporting deviations */
  MSG::Level m_myMsgLvl;
  int m_myMsgLvlProp;


  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};
  LArBadChannelMask m_bcMask;
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"};

  // Pointers to various identifier helper classes, not used her, but
  // probably useful for deriving algorithm
  const LArOnlineID* m_onlineHelper; 
  const LArEM_ID   * m_emId;  
  const LArHEC_ID  * m_hecId; 
  const LArFCAL_ID * m_fcalId;
  const CaloCell_ID* m_caloId;


  /** Pointer to container with reference values */
  const CONDITIONSCONTAINER* m_reference;  

  /** Pointer to container to be validated */
  const CONDITIONSCONTAINER* m_validation;  

  /** Non-const pointer to container to be validated (for FEB patching)*/
  CONDITIONSCONTAINER* m_nc_validation;  

  /** SG key of the container to be validated (job-property) */
  std::string    m_validationKey;
  /** SG key of the reference container (job-property) */
  std::string    m_referenceKey;

  /** SG key of container for deviating channels (job-property) */
  std::string m_thinValContKey;

  /** SG key of container for reference of deviating channels (job-property) */
  std::string m_thinRefContKey;


  /** Flag set to true in order to use bad-channel info for the channel description 
      as well as to ingore some types of bad-channels via the masking tool */
  bool m_useBCInfo;

  /** wether completeness shall be checked or not (job-property) */
  bool m_checkCompleteness;

  /** wether COOL channel numbers of validation and reference container shall be compared (job-property) */
  bool m_checkNumberOfCoolChannels;

  /** wether missing FEBS (incomplete COOL channels) should be patched */
  bool m_patchMissingFEBs;

  /** wether to check for FEBs and Calib-lines with many deviating channels */
  bool m_printFailedPattern;

  /** wether to compare feb-averages (used only by derived class)*/
  bool m_doFebAverages; 

  /** Limit for the number of messages about indiviual deviating channels (usd only by derived class)*/
  unsigned m_maxmessages; 

  //Count the various cases:

  /** Number of channels that where checked (grand-total)*/
  unsigned m_nChecked;

  /** Number of channels for which vaildateChannel returned false */
  unsigned m_nFailedValidation;

  /** Number of channels for which vaildateChannel returned true */
  unsigned m_nValidated;

  /** Number of channels known to be bad according to the bad-channel tool */
  unsigned m_nBad;

  /** Number of channels for which not reference channel could be found */
  unsigned m_nNoReference;

  /** Keeping track of channels failing the validation criteria (per gain) */
  typedef std::vector<std::pair<HWIdentifier, bool > > CHECKEDID_t;
  std::vector<CHECKEDID_t> m_checkedIds;

  /** File name to store list of deviating channels */
  std::string m_chanFileName;

  /** File name to store list of deviating channels */
  std::ofstream  m_chanFile;
  
  /** File name to store list of FEBs with deviating channels */
  std::string m_febFileName;

  /** File name to store list of FEBs with deviating channels */
  std::ofstream  m_febFile;

  bool m_useCorrChannel;

};

#include "LArCalibValidationAlg.icc"

#endif

