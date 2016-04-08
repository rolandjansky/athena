// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionFillerTool.h 734655 2016-04-07 17:09:12Z ssnyder $
#ifndef TRIGGERD3PDMAKER_TRIGDECISIONFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGDECISIONFILLERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigSvc.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Forward declaration(s):
namespace TrigConf {
   class HLTChain;
}
namespace Trig {
   class TrigDecisionTool;
}

namespace D3PD {

   /**
    *  @short Tool saving the "encoded" trigger decision into the D3PD
    *
    *         This tool can be used to save the trigger decision in a "raw" form
    *         into the D3PD. This means that for LVL1 it saves the CTP bits directly,
    *         and for the HLT it saves the "chain IDs" of the chains that passed
    *         the event. When you want to save the decision from all the LVL1 items
    *         and all the HLT chains, this is way more organized than having hundreds
    *         of branches in the output TTree, each representing one item/chain.
    *
    *         To make sense of this information in the D3PD, you need to know the
    *         trigger configuration for your events. You can get this information by
    *         using the D3PD::TrigConfMetadataTool in this package.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 734655 $
    * $Date: 2016-04-07 19:09:12 +0200 (Thu, 07 Apr 2016) $
    */
   class TrigDecisionFillerTool : public BlockFillerTool< void > {

      /// Type definition of the chain IDs
      typedef short chainId_t;
      /// Type definition for the BG code
      typedef char bgCode_t;

   public:
      /// Regular AlgTool constructor
      TrigDecisionFillerTool( const std::string& type, const std::string& name,
                              const IInterface* parent );

      /// Function initializing the tool
      virtual StatusCode initialize();

      /// Function creating the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill();

   private:
      /// Handle for the TrigDecisionTool
      ToolHandle< Trig::TrigDecisionTool > m_trigDec;
      /// Handle for the trigger configuration service
      ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConf;

      bool m_saveLVL1Raw; ///< Save the "RAW" LVL1 information
      bool m_saveLVL1Physics; ///< Save the "Physics" LVL1 information
      bool m_saveHLTRaw; ///< Save the "RAW" HLT information
      bool m_saveHLTPhysics; ///< Save the "Physics" HLT information
      bool m_saveHLTResurrected; ///< Save the "resurrected" HLT information
      bool m_saveHLTPassThrough; ///< Save the "passThrough" HLT information
      bool m_saveHLTChainState; ///< Save the "chain state" HLT information
      bool m_saveBGCode; ///< Save the bunchgroup code of the event
      bool m_saveTruncated; ///< Save the info whether HLT result was truncated

      std::vector< uint32_t >* m_tbp; ///< Trigger Before Prescale words
      std::vector< uint32_t >* m_tap; ///< Trigger After Prescale words
      std::vector< uint32_t >* m_tav; ///< Trigger After Veto words

      std::vector< chainId_t >* m_L2_passedRAW; ///< IDs of the LVL2 chains passing the RAW requirements
      std::vector< chainId_t >* m_L2_passedPhysics; ///< IDs of the LVL2 chains passing the Physics requirements
      std::vector< chainId_t >* m_L2_resurrected; ///< IDs of the LVL2 chains that passed after being resurrected
      std::vector< chainId_t >* m_L2_passedThrough; ///< IDs of the LVL2 chains that have the passThrough bit set
      std::vector< chainId_t >* m_L2_wasPrescaled; ///< IDs of the LVL2 chains that got prescaled
      std::vector< chainId_t >* m_L2_wasResurrected; ///< IDs of the LVL2 chains that got resurrected

      std::vector< chainId_t >* m_EF_passedRAW; ///< IDs of the EF chains passing the RAW requirements
      std::vector< chainId_t >* m_EF_passedPhysics; ///< IDs of the EF chains passing the Physics requirements
      std::vector< chainId_t >* m_EF_resurrected; ///< IDs of the EF chains that passed after being resurrected
      std::vector< chainId_t >* m_EF_passedThrough; ///< IDs of the EF chains that have the passThrough bit set
      std::vector< chainId_t >* m_EF_wasPrescaled; ///< IDs of the EF chains that got prescaled
      std::vector< chainId_t >* m_EF_wasResurrected; ///< IDs of the EF chains that got resurrected

      std::vector< chainId_t >* m_HLT_passedRAW; ///< IDs of the HLT chains passing the RAW requirements
      std::vector< chainId_t >* m_HLT_passedPhysics; ///< IDs of the HLT chains passing the Physics requirements
      std::vector< chainId_t >* m_HLT_resurrected; ///< IDs of the HLT chains that passed after being resurrected
      std::vector< chainId_t >* m_HLT_passedThrough; ///< IDs of the HLT chains that have the passThrough bit set
      std::vector< chainId_t >* m_HLT_wasPrescaled; ///< IDs of the HLT chains that got prescaled
      std::vector< chainId_t >* m_HLT_wasResurrected; ///< IDs of the HLT chains that got resurrected

      bgCode_t* m_bgCode; ///< Bit-pattern of the active BunchGroup(s) for the event

      bool* m_l2Truncated; ///< Flag showing whether LVL2 result was truncated
      bool* m_efTruncated; ///< Flag showing whether EF result was truncated
      bool* m_HLTTruncated; ///< Flag showing whether HLT result was truncated

   }; // class TrigDecisionFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGDECISIONFILLERTOOL_H
