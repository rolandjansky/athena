/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2_MBTS_FEX_H
#define T2_MBTS_FEX_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "TileEvent/TileCellCollection.h"
#include "TrigT2MinBias/T2MbtsUtils.h"
#include <vector>

class TileTBID;

// namespace xAOD{
//   class TrigT2MbtsBits;
// }

/** @class T2MbtsFex
 
@author W. H. Bell <W.Bell@cern.ch>

A feature extraction algorithm to retrieve the MBTS TileCells from the DSP
*/
class T2MbtsFex: public HLT::AllTEAlgo, public T2MbtsUtils {
  public:
   T2MbtsFex(const std::string &name, ISvcLocator* pSvcLocator);
    /**
    * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
    *
    * @param input outer vector describeds the different input TE types,
    * here we expect: 1st LVL1 energy TE; 2nd Muons;
    * inner vector provides all TE instances of the given type
    *
    * @param output the output TE type
    */
   HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			     unsigned int output);

   HLT::ErrorCode hltInitialize();
   HLT::ErrorCode hltFinalize();

   /**
    * @brief This method overwrites the default one (doing nothing) in the algo.h class
    *
    * This is used to reset the internal caching mechanism of this algorithm
    */
   HLT::ErrorCode hltEndEvent();

  private:

   /** A data member to retain a connection to the MsgStream
       service. */
   MsgStream m_log;
   
   /** iterators to TileCells. To be used when on/offline
       TileCellCont/IDC (fast/slow ByteStream Decoder) are to be used.
       Used as input for TrigDataAccess::LoadCollections */
   TileCellCollection::const_iterator m_itBegin;
   TileCellCollection::const_iterator m_itEnd;
   TileCellCollection::const_iterator m_itt;

   //   /** DataHandle to TrigTimerSvc */
   //   ServiceHandle<ITrigTimerSvc> m_timersvc;

   //   /** Timing measures. 
   //       m_timer[0] is the complete exec timer. m_timer[1] is the
   //       LoadCollection timer (Data Preparation), m_timer[2] is the 
   //       real algorithm timer and m_timer[3] is the saving data 
   //       timer. */
   //   TrigTimer* m_timer[4];

   TrigTimer *m_timerLoadColl;
   TrigTimer *m_timerAlg;
   TrigTimer *m_timerSave;
   /** Object  that provides data access in a Region of
       Interest. See TrigDataAccess for more details. */
   ToolHandle<ITrigDataAccess> m_data;

   /** A data member to retain a connection to the Detector Store. */
   ServiceHandle<StoreGateSvc> m_detStore;
   
   /** A pointer for access to the Tile Test Beam Identifier helper. */
   const TileTBID* m_tileTBID;
   
   /** error control */
   uint32_t m_error;

   /** Energy deposited in each counter (pC) */ 
   std::vector<float> m_triggerEnergies;

   /** Relative trigger times with repect to collision timing */
   std::vector<float> m_triggerTimes;

   /** ID of MBTS ID  */ 
   std::vector<int> m_triggerID;

   bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode.
   xAOD::TrigT2MbtsBits *m_t2MbtsBits;    //!< internal caching: MBTS feature from the first execution.
   HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution.

   /** event info*/
   unsigned int m_BCID;
   
};

#endif
