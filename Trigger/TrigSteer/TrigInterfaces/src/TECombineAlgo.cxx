/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/TECombineAlgo.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/ComboIterator.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "GaudiKernel/GaudiException.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

using namespace HLT;

void getAllRoIs(TEVec& inputTEs, TEVec& rois) {
  if ( inputTEs.size() == 1 ) {
    rois.insert(rois.end(), HLT::Navigation::getRoINodes(inputTEs[0]).begin(), HLT::Navigation::getRoINodes(inputTEs[0]).end());
  } else {
    for ( TEVec::const_iterator r = inputTEs.begin(); r != inputTEs.end(); ++r ) {
      rois.insert(rois.end(), HLT::Navigation::getRoINodes(*r).begin(), HLT::Navigation::getRoINodes(*r).end() );
    }
    std::sort(rois.begin(), rois.end());
    rois.erase(std::unique(rois.begin(), rois.end()), rois.end());
  }
}



HLT::ErrorCode TECombineAlgo::createAndProcessTEs(std::vector<unsigned int>& inputs,
						  unsigned int outputType,
						  TEVec& outputTEs,
						  const std::vector<unsigned int>& topologicalStartFrom)
{
   // the caching of error code
   // we do not want to try another execution if this instance gave an error already in this event
   if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) {    
      ATH_MSG_DEBUG ( name() 
                      << " already failed in this event, doing nothing" );	    
      return m_returnErrorCode;
   }


   HLT::ErrorCode retCode = HLT::OK;
   if (!runAlgo()) {
      config()->getNavigation()->getAllOfType(outputType, outputTEs); 
      return retCode; 
   }
  
   // Initialize the iteration on the inputTEs
   ErrorCode initStatus = init_inputTE_iteration(inputs, topologicalStartFrom);
   if (initStatus.action() > HLT::Action::CONTINUE ) return initStatus;
   retCode = (initStatus > retCode) ? initStatus : retCode;

   beforeEventMonitors().ignore();
   unsigned combCounter=0;
   while (1) {
      TEVec inputTEs;
      ATH_MSG_DEBUG ( "pulling combination " << combCounter);
	
      ErrorCode iterStatus = next_inputTE_iteration(inputTEs, topologicalStartFrom);
      combCounter ++;
      retCode = (iterStatus > retCode) ? iterStatus : retCode;
    
      if (iterStatus.action() > HLT::Action::CONTINUE ) {
         afterEventMonitors().ignore();
         return iterStatus;
      }
    
      if (inputTEs.size() == 0) break; // that means no more iterations are available

      // caching
      TriggerElement* outputTE = config()->getNavigation()->addNode(inputTEs, outputType);
      //    outputTE->setActiveState(true);
      outputTE->setGhostState(false);

      TEConstVec constInputTEs = constify(inputTEs);


      if ( needsExecution( outputTE ) ) {
         // try if we that input now
         bool pass = false;

         beforeExecMonitors().ignore();
         // start collecting cache information
         if ( !startCaching(outputTE) ) {
            ATH_MSG_ERROR ( name() 
                            << " start of caching on TE " << outputTE->getId() << " failed " );

            return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
         }

         ErrorCode ec = plugin_acceptInputs(inputTEs, pass);

         if ( !pass ) { // input is rejected	
            //	outputTE->setActiveState(false); // make it inactive (as if rejected by hypo)
            outputTE->setGhostState(true);   // make it invisible (transient)

            // stop collecting cache information
            if ( !stopCaching(outputTE) ) {  
               ATH_MSG_ERROR ( name() 
                               << " stop of caching on TE " << outputTE->getId() << " failed ");
               return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
            }	
         }

         mapAndRecordError(ec);

         if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) { // stop because of an error
            afterExecMonitors().ignore();
            afterEventMonitors().ignore();
            return m_returnErrorCode;
         }

         if ( !pass ) {
            afterExecMonitors().ignore();
            if ( m_config->getSteeringOPILevel() > 0 ) {
               addSteeringOperationalInfo(true, outputTEs.size(), outputTE);
            }
            continue;
         }

         outputTEs.push_back(outputTE);

         ec = plugin_processTE(constInputTEs, outputTE);
         //      outputTE->setActiveState(true);
         outputTE->setGhostState(false);



         // stop collecting cache information
         if ( !stopCaching(outputTE) ) {  
            ATH_MSG_ERROR ( name() 
                            << " stop of caching on TE " << outputTE->getId()
                            << " failed " );
            return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
         }

         mapAndRecordError(ec, outputTE);

         afterExecMonitors().ignore();

         if ( m_config->getSteeringOPILevel() > 0 ) {
            addOperationalInfo(outputTE);
            addSteeringOperationalInfo(true, outputTEs.size(), outputTE);
         }
         if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) {
            afterEventMonitors().ignore();
            return m_returnErrorCode;
         }
         retCode = (m_returnErrorCode > retCode) ? m_returnErrorCode : retCode;


      
      } else { // does not need execution

         if ( ! outputTE->getGhostState() ) {  
            // if the inputs was accepted in the first pass then 
            // we need to cache the results	


            outputTEs.push_back(outputTE);

            // move cached results
            if (!cache(outputTE) ) {
               ATH_MSG_ERROR ( name() 
                               << " caching on TE " << outputTE->getId() << " failed " );
               return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
            }
            // we have to set active state irrespectivelly of the state 
            // from where the TE copied cached objects
            // this is because the source TE has undergone hypothesis execution
            // and may be set inactive, we can't assume that hypothesis running in this sequence
            // will take also negative decision
            //	outputTE->setActiveState(true);

            if ( m_config->getSteeringOPILevel() > 0 ) {
               addSteeringOperationalInfo(false, outputTEs.size(), outputTE);
            }
	
            ATH_MSG_DEBUG ( name() 
                            << " already ran (first) on RoIs of TE " << outputTE->getId()
                            << ", reattached features only." );
         } else {
            ATH_MSG_DEBUG ( name() 
                            << " already ran (first) on this TE and that input was NOT accepted" );	
         }
      }
   }
   afterEventMonitors().ignore();
   return retCode;
}
