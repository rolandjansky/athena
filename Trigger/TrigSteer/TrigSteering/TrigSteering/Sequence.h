/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Sequence
 *
 * @brief  one sequence node, i.e.
 *         input TE type(s) -> PESA Algorithm(s) -> output TE type
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Sequence.h,v 1.20 2008-12-08 13:22:18 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_SEQUENCE_H
#define TRIGSTEERING_SEQUENCE_H

#include <vector>
#include "TrigSteeringEvent/Enums.h"

#include "GaudiKernel/SmartIF.h"
#include "AthenaBaseComps/AthService.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc_RTT.h" //used for ROS data access test
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

class TrigTimer;

namespace Trig {
  class TrigDecisionTool;
}

namespace HLT {
   class Algo;
   class AlgoConfig;
   class TriggerElement;

   namespace seqns {
      static const std::vector<unsigned int> emptyVector;
   }

   /**
      @class Sequence
      This class represents one sequence (node), i.e.
      input TriggerElement (TE) type(s) -> PESA Algorithm(s) -> output TE type = Sequence

      Note: PESA Algorithms should be: N x FEX Algorithm + [HYPO Algorithm]

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class Sequence
   {

   public:

      //    Sequence(){};

      Sequence(std::vector<unsigned int> inputTypes,
               unsigned int outputType,
               Algo* firstAlgo, const std::vector<HLT::Algo*>& algos,
               HLT::AlgoConfig* config,
               const std::vector<unsigned int>& topoTypes = seqns::emptyVector); //!< constructor to be called in normal initialization

      virtual ~Sequence() {} //!< destructor, does nothing

      /** @brief
          execute this sequence (if not done before) by running PESA algorithms
          results:
          - number of active TEs
          - number of produced TEs
          are saved.
      */
      virtual HLT::ErrorCode execute(); //!< execute this sequece if not done before

      /** @brief execute the ROB request preparation for all algorithms of this sequence
          - algorithm errors are propagated upward
      */
      HLT::ErrorCode prepareRobRequests(); //!< execute the ROB request preparation for this sequence

      bool reset(); //!< call at begin of event, set all results to 0 and alreadyExecuted to false
      bool resetROBRequestPreparation();  //!< call at begin of event, set m_prepRobReqAlreadyExecuted to false and m_prepRobReqErrorCode to OK

      int producedOutputTEs() const; //!< -1 if not already executed
      int activeOutputTEs() const;   //!< -1 if not already executed

      unsigned int outputTEType() const { return m_outputTeType; } //!< get output TriggerElement type

      const std::vector<unsigned int>& inputTETypes() const { return m_inputTeTypes; } //!< const reference getter for input types

      const std::vector<TriggerElement*>& outputTEs() const { return m_outputTEs; } //!< const reference getter for all produced TEs

      friend class Trig::TrigDecisionTool;    //!< make TrigDecisionTool a friend
      friend class TrigSteer;                 //!< make TrigSteer a friend

   protected:

      bool m_alreadyExecuted;                       //!< was this Sequence object executed before?
      HLT::ErrorCode m_execErrorCode;               //!< Cached ErrorCode of execution

      std::vector<TriggerElement*> m_outputTEs;     //!< Cached output TEs of last algo

      Algo* m_firstAlgo;                            //!< The first algorithm in the sequence (creates the output TEs therefore kept separatelly/another call is made on it)
      std::vector< Algo*> m_nextAlgos;              //!< all other PESA algorithms belonging to this sequence

      std::vector< unsigned int > m_inputTeTypes;   //!< input TriggerElement types for HLT Algorithm(s)
      unsigned int m_outputTeType;                  //!< output TE type for HLT Algorithm(s)

      HLT::AlgoConfig* m_config;                    //!< config object, holding common variables
      std::vector< unsigned int > m_topoStartFrom;  //!< TE types to start search for refined input TEs in topological cases

   private:
    
      unsigned int activeOutputTEsInternal() const; //!< count number of active output TEs
      bool m_prepRobReqAlreadyExecuted;             //!< was the ROB request preparation of this Sequence object executed before?
      HLT::ErrorCode m_prepRobReqErrorCode;         //!< Cached ErrorCode of ROB request preparation execution
      /*   std::vector<TriggerElement*> m_outputTEs;     //!< Cached output TEs of last algo

      Algo* m_firstAlgo;                            //!< The first algorithm in the sequence (creates the output TEs therefore kept separatelly/another call is made on it)
      std::vector< Algo*> m_nextAlgos;              //!< all other PESA algorithms belonging to this sequence

      std::vector< unsigned int > m_inputTeTypes;   //!< input TriggerElement types for HLT Algorithm(s)
      unsigned int m_outputTeType;                  //!< output TE type for HLT Algorithm(s)

      HLT::AlgoConfig* m_config;                    //!< config object, holding common variables*/

      std::vector< Sequence* > m_previousSequences; //!< list of pointers to HLT Sequences which are responsible for producing the input TEs of this Sequence

      Sequence*            m_topoStartFromSequence; //!< sequence to start search for refined input TEs in topological cases, needed to make sure it was run


      TrigTimer *m_timer;

      ServiceHandle<IROBDataProviderSvc>      m_robDataProvider;        //!< ROB data provider (for ROB pre-fetching)
      SmartIF <ITrigROBDataProviderSvc_RTT>   m_trigROBDataProviderRTT; //!< ROB data provider with extensions for RTT tests
   };
}

#endif
