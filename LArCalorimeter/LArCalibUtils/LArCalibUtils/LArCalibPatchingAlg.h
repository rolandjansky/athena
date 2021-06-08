/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArCalibPatchingAlg.h
 * @author Walter Lampl <walter.lampl @cern.ch>
 * @date Feb 2008
 * @brief Algorithm to patch LAr elec. calibration constants for channels with broken calibration line
 */


#ifndef LARCALIBPATCHINGALG_H
#define LARCALIBPATCHINGALG_H
 
#include <vector>
#include <string>
 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"



/** 
  * @class LArCalibValidationAlg
  * @brief Algorithm to patch LAr elec. calibration constants for channels with broken calibration line
  *
  * Loops over all channels in an LArConditionsContainer and checks their status in the bad-channel database.
  * Constants for 'bad' channels are patched,by one of the follwoing methods (steered by jobOptions): @n
  * - Neighboring FEB channel @n
  * - Neighbor in Phi @n
  * - Average over all channels with the same phi @n
  *
  * The @c LArBadChannelMask is used to determine what types of problems should be patched.
  * The patched value is added to the LArConditionsContainer as correction. The neighbor-patching is templated,
  * and works for any kind of payload object while the averaging needs knowledge about the payload object and
  * is implemented a specialized method @c getAverage. 
  */


template<class CONDITIONSCONTAINER>
class LArCalibPatchingAlg:public AthAlgorithm {
 
public:
  /**
   * @brief regular Algorithm constructor
   */
  LArCalibPatchingAlg (const std::string& name, ISvcLocator* pSvcLocator);

  /**
   * @brief Destructor
   */
  ~LArCalibPatchingAlg();

  /**
   * @brief Initialize method.
   * @return Gaudi status code.
   * Analyzes and verifies the jobOption settings
   */
  StatusCode initialize();

  /**
   * @brief Empty Execute method
   */
  StatusCode execute() {return StatusCode::SUCCESS;}

   /**
   * @brief Finalize method.
   * @return Gaudi status code.
   * All the job is done here
   */
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:

  typedef typename CONDITIONSCONTAINER::ConstConditionsMapIterator CONTIT;
  typedef typename CONDITIONSCONTAINER::LArCondObj LArCondObj;
  
 /**
   * @brief patch method 
   * @return bool to tell if patching suceeded
   * This method is called for every channel with broken calibration line
   */
  bool patch(const HWIdentifier chid, const int gain, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling); 

    
 /**
   * @brief Specialized method to average Ramps over a phi-ring
   * @param chid Online identifier of the channel to be patched
   * @param gain Gain in question
   * @patch [OUT] Reference to be filled by the average
   */
  bool getAverage(const HWIdentifier chid, const int gain, LArRampP1& patch, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling, bool isphi=true); 

  /**
   * @brief Specialized method to average OFCs over a phi-ring
   * @param chid Online identifier of the channel to be patched
   * @param gain Gain in question
   * @patch [OUT] Reference to be filled by the average
   */
  bool getAverage(const HWIdentifier chid, const int gain, LArOFCP1& patch, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling, bool isphi=true); 





  //The following #ifdef-logic is to make this package compile against 13.2.0. as well as 14.X.0
  //since the payload object of LArMphysOverMcal has changed from LArMphysOverMcalP1 to LArSingleFloatP


#ifdef LARRAWCONDITIONS_LARMPHYSOVERMCALP
  /**
   * @brief Specialized method to average MphysOverMcal over a phi-ring
   * @param chid Online identifier of the channel to be patched
   * @param gain Gain in question
   * @patch [OUT] Reference to be filled by the average
   */

  //For backward compatiblity only!!! Will be removed at some point
  bool getAverage(const HWIdentifier chid, const int gain, LArMphysOverMcalP1& patch, const LArBadChannelCont* bcCont, bool isphi=true);
#endif


#ifdef LARRAWCONDITIONS_LARSINGLEFLOATP
  /**
   * @brief Specialized method to average any single-float object over a phi-ring
   * @param chid Online identifier of the channel to be patched
   * @param gain Gain in question
   * @patch [OUT] Reference to be filled by the average
   */
  bool getAverage(const HWIdentifier chid, const int gain, LArSingleFloatP& patch, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling, bool isphi=true);
#endif




  bool getAverage(const HWIdentifier,const int, LArCaliWaveVec&, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling, bool isphi=true) ;

   /**
   * @brief Helper method to get a phi-ring
   * @return Reference to a vector of HWIdentfiers
   * @param chid Identifier of the channel to be patched
   * @param distance Step-with in phi. 1..every channel, 2..every second channel, etc.
   * Probably meaningless for the FCAL
   */ 
  std::vector<HWIdentifier>& getPhiRing(const HWIdentifier chid, const LArBadChannelCont* bcCont, const LArOnOffIdMapping* cabling, unsigned distance=1);
  
  /* Get list of channels in the same FEB. Used for FEBAverage method */
  std::vector<HWIdentifier>& getFEBChans(const HWIdentifier chid, const LArBadChannelCont* bcCont);

  StatusCode setSymlink(const LArRampComplete* ramp) const;
  StatusCode setSymlink(const LArOFCComplete* ofc) const;
  StatusCode setSymlink(const LArMphysOverMcalComplete* ramp) const;
  StatusCode setSymlink(const LArCaliWaveContainer* ) const 
  {return StatusCode::SUCCESS;};

  enum patchMethod{
    FEBNeighbor,
    PhiNeighbor,
    PhiAverage,
    FEBAverage
  };

  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  LArBadChannelMask m_bcMask;
  Gaudi::Property<std::vector<std::string> > m_problemsToPatch{this,"ProblemsToPatch",{}, "Bad-Channel categories to patch"};

  bool m_useCorrChannel;
  bool m_patchAllMissing;
  bool m_unlock;

  const LArOnlineID* m_onlineHelper;
  const LArEM_ID   * m_emId;
  const LArHEC_ID  * m_hecId;
  const LArFCAL_ID * m_fcalId;
  const CaloCell_ID* m_caloId;

  const CONDITIONSCONTAINER* m_contIn;  
  CONDITIONSCONTAINER* m_contOut;  

  patchMethod    m_patchMethod;
  std::string    m_containerKey;
  std::string    m_newContainerKey;
  std::string    m_patchMethodProp;

  std::vector<HWIdentifier> m_idList;

};

#include "LArCalibPatchingAlg.icc"

#endif

