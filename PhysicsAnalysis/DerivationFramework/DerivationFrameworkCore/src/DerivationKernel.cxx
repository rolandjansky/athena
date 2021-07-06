/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DerivationKernel.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Based on the Integrated Simulation Framework
// This code loops over tools defining skimming, slimming and thinning
// operations, sets the final filter decision, applies the overall slimming
// and thinning, and passes on to the persistency

#include "DerivationFrameworkCore/DerivationKernel.h"

#include <sstream>                                      // C++ utilities
#include <string>
#include <algorithm>
#include <fstream>

#include "GaudiKernel/ISvcLocator.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ToolVisitor.h"

#include "StoreGate/StoreGateSvc.h"             // Storegate stuff
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/DefaultKey.h"
#include "SGTools/StlVectorClids.h"

///////////////////////////////////////////////////////////////////////////////
namespace {
   inline void renounceInputs([[maybe_unused]] const std::unordered_set<std::string> &outputs, std::vector< const DataObjID *> &output_ids, AlgTool *a_tool) {
      for ( const DataObjID *a_data_id : output_ids ) {
         a_tool->renounceInput( *a_data_id );
      }
   }
   inline void collectOutputs(std::unordered_set<std::string> &outputs, std::vector< const DataObjID *> &output_ids, const AlgTool *a_tool) {
      for ( const DataObjID &a_data_id : a_tool->outputDataObjs() ) {
         if (outputs.insert( a_data_id.key() ).second) {
            output_ids.push_back( &a_data_id );
         }
      }
   }
   template <typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<void, Callable, IAlgTool*>>>
   inline void visitTools(IAlgTool &a_tool_interface, Callable &func) {
      const AlgTool *alg_tool = dynamic_cast<AlgTool *>(&a_tool_interface);
      if (alg_tool) {
         func(&a_tool_interface);
         std::vector<IAlgTool *> &non_const_tools ATLAS_THREAD_SAFE = const_cast< std::vector<IAlgTool *> &>( alg_tool->tools() );
         ToolVisitor::visit(  non_const_tools, func);
      }
   }
}

DerivationFramework::DerivationKernel::DerivationKernel(const std::string& name, ISvcLocator* pSvcLocator) :
AthFilterAlgorithm(name, pSvcLocator),
m_chronoSvc("ChronoStatSvc", name),
m_runSkimmingFirst(false)
{
    // ------- Python changeable properties -------
    declareProperty("SkimmingTools",         m_skimmingTools);
    declareProperty("ThinningTools",         m_thinningTools);
    declareProperty("AugmentationTools",     m_augmentationTools);
    declareProperty("RunSkimmingFirst",      m_runSkimmingFirst);
    //declareProperty("PrintCutFlow",          m_printCutFlow);
    //declareProperty("PrintMemoryAndCPU",     m_printMemCPU)
    //---------------------------------------------
    
    // Counters
    m_eventCounter = 0;
    m_acceptCntr = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::DerivationKernel::initialize() {
    
    ATH_MSG_INFO("Initializing the derivation framework kernel " << name());
    
    // get the tool service
    IToolSvc* toolSvc;
    StatusCode sc = service("ToolSvc",toolSvc);
    if (StatusCode::SUCCESS != sc) {
        ATH_MSG_FATAL("Unable to retrieve ToolSvc");
        return StatusCode::FAILURE;
    }
    
    // get the skimming tools
    if( m_skimmingTools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve skimming tools");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("The following skimming tools will be applied....");
        ATH_MSG_INFO(m_skimmingTools);
    }
    // get the thinning tools
    if( m_thinningTools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve thinning tools");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("The following thinning tools will be applied");
        ATH_MSG_INFO(m_thinningTools);
    }
    // get the augmentation tools
    if( m_augmentationTools.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve augmentation tools");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("The following augmentation tools will be applied....");
        ATH_MSG_INFO(m_augmentationTools);
    }
 
    // get the chrono auditor
    if ( m_chronoSvc.retrieve().isFailure() ) {
	    ATH_MSG_FATAL("Failed to retrieve service " << m_chronoSvc);
	    return StatusCode::FAILURE;
    } 

    if (m_runSkimmingFirst) {
	ATH_MSG_INFO("Skimming will be run before augmentation. Make sure your skimming does not depend on variables calculated in the augmentation step!");
    }

    std::unordered_set<std::string> outputs;
    std::vector<const DataObjID *> output_ids;
    auto output_collector = [&outputs, &output_ids](IAlgTool *a_tool_interface) {
       const AlgTool *alg_tool = dynamic_cast<AlgTool *>(a_tool_interface);
       if (alg_tool) {
          collectOutputs(outputs,output_ids, alg_tool);
       }
    };
    auto renounce_and_collect_outputs = [&outputs, &output_ids](IAlgTool *a_tool_interface) {
       AlgTool *alg_tool = dynamic_cast<AlgTool *>(a_tool_interface);
       if (alg_tool) {
          renounceInputs(outputs,output_ids, alg_tool);
          collectOutputs(outputs,output_ids, alg_tool);
       }
    };
    auto renouncer = [&outputs, &output_ids](IAlgTool *a_tool_interface) {
       AlgTool *alg_tool = dynamic_cast<AlgTool *>(a_tool_interface);
       if (alg_tool) {
          renounceInputs(outputs,output_ids, alg_tool);
       }
    };

    // collection and renouncing has to happen in the order the tools are called
    // during execute.
    if (m_runSkimmingFirst) {
       for (ToolHandle<ISkimmingTool> &a_tool_handle : m_skimmingTools ) {
          visitTools(*a_tool_handle, output_collector);
       }
       for (ToolHandle<IAugmentationTool> &a_tool_handle : m_augmentationTools ) {
          visitTools(*a_tool_handle, renounce_and_collect_outputs);
       }
    }
    else {
       for (ToolHandle<IAugmentationTool> &a_tool_handle : m_augmentationTools ) {
          visitTools(*a_tool_handle, output_collector);
       }
       for (ToolHandle<ISkimmingTool> &a_tool_handle : m_skimmingTools ) {
          visitTools(*a_tool_handle, renounce_and_collect_outputs);
       }
    }
    for (ToolHandle<IThinningTool> &a_tool_handle : m_thinningTools ) {
       visitTools(*a_tool_handle, renouncer);
    }
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::DerivationKernel::execute() {
    
    // On your marks.... get set....
    Chrono chrono( &(*m_chronoSvc), name() ); 
    // GO!!!

    ATH_MSG_DEBUG(name() << " is processing next event...");
    
    // Increment the event counter
    m_eventCounter++;
  
    //=============================================================================
    // AUGMENTATION ===============================================================
    //=============================================================================

    if (!m_runSkimmingFirst) {
        ToolHandleArray<IAugmentationTool>::iterator augmentationTool(m_augmentationTools.begin());
        ToolHandleArray<IAugmentationTool>::iterator endOfAugmentationTools(m_augmentationTools.end());
        while (augmentationTool != endOfAugmentationTools) {
            if ( (**augmentationTool).addBranches().isFailure() ) {
                    ATH_MSG_ERROR("Augmentation failed!");
                    return StatusCode::FAILURE;
            }
            ++augmentationTool;
        }
    }    
 
    //=============================================================================
    //SKIMMING ===================================================================
    //=============================================================================

    // Set master flag to true
    bool acceptEvent(true);

    // Loop over the filters
    ToolHandleArray<ISkimmingTool>::iterator skimmingTool(m_skimmingTools.begin());
    ToolHandleArray<ISkimmingTool>::iterator endOfSkimmingTools(m_skimmingTools.end());
    while (skimmingTool != endOfSkimmingTools) {
        if (!((**skimmingTool).eventPassesFilter())) {
            acceptEvent=false;
            ATH_MSG_DEBUG("This event failed the " << (**skimmingTool).name() << " filter. Therefore it will not be recorded.");
            break;
        }
        ++skimmingTool;
    }

    // Increment local counters if event to be accepted
    if (acceptEvent) ++m_acceptCntr;
    
    // Set the setFilterPassed flag 
    setFilterPassed(acceptEvent);

    // Return if event didn't pass
    if (!acceptEvent) return StatusCode::SUCCESS;

    // If user requested skimming first, run augmentation now...
    if (m_runSkimmingFirst) {
        ToolHandleArray<IAugmentationTool>::iterator augmentationTool(m_augmentationTools.begin());
        ToolHandleArray<IAugmentationTool>::iterator endOfAugmentationTools(m_augmentationTools.end());
        while (augmentationTool != endOfAugmentationTools) {
            if ( (**augmentationTool).addBranches().isFailure() ) {
                    ATH_MSG_ERROR("Augmentation failed!");
                    return StatusCode::FAILURE;
            }
            ++augmentationTool;
        }
    }
 

    //=============================================================================
    // THINNING ===================================================================
    //=============================================================================

    ToolHandleArray<IThinningTool>::iterator thinningTool(m_thinningTools.begin());
    ToolHandleArray<IThinningTool>::iterator endOfThinningTools(m_thinningTools.end());
    while (thinningTool != endOfThinningTools) {
	if ( (**thinningTool).doThinning().isFailure() ) {
		ATH_MSG_ERROR("Thinning failed!");
                return StatusCode::FAILURE;
	}
	++thinningTool;
    }


    return StatusCode::SUCCESS;
    
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DerivationFramework::DerivationKernel::finalize() {
    
    ATH_MSG_INFO( "============================================================================");
    ATH_MSG_INFO( "|| SUMMARY OF THE DERIVATION FRAMEWORK KERNEL WITH NAME " << name() << " || ");
    ATH_MSG_INFO( "============================================================================");
    ATH_MSG_INFO( "============================================================================");
    ATH_MSG_INFO( "Events analyzed: " << m_eventCounter);
    ATH_MSG_INFO( "Events accepted: " << m_acceptCntr);
    ATH_MSG_INFO( "============================================================================");
    
    return StatusCode::SUCCESS;
    
}
