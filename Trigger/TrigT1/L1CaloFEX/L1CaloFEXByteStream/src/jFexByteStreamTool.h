/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFexByteStreamTool  -  description
//                              -------------------
//     begin                : 01 01 2022
//     email                : Sergi.Rodriguez@cern.ch
//  ***************************************************************************/

#ifndef JFEXBYTESTREAMTOOL_H 
#define JFEXBYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIAuxContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIAuxContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIAuxContainer.h"
#include "xAODTrigger/jFexMETRoIContainer.h"
#include "xAODTrigger/jFexMETRoIAuxContainer.h"
#include "xAODTrigger/jFexSumETRoIContainer.h"
#include "xAODTrigger/jFexSumETRoIAuxContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class jFEXRoIByteStreamTool
 *  @brief Implementation of a tool for L1 RoI conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 **/
class jFexByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
    public:
        jFexByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~jFexByteStreamTool() override = default;

        // ------------------------- IAlgTool methods --------------------------------
        virtual StatusCode initialize() override;

        // ------------------------- IL1TriggerByteStreamTool methods ----------------------
        /// BS->xAOD conversion
        virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf, const EventContext& eventContext)const override;

        /// xAOD->BS conversion
        virtual StatusCode convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& vrobf, const EventContext& eventContext) override;

        /// Declare ROB IDs for conversion
        virtual const std::vector<uint32_t>& robIds() const override {
            return m_robIds.value();
        }

    private:
        // ------------------------- Properties --------------------------------------
        // ROBIDs property required by the interface
        Gaudi::Property<std::vector<uint32_t>> m_robIds {this, "ROBIDs", {}, "List of ROB IDs required for conversion to/from xAOD RoI"};

        // Write handle keys for the L1Calo EDMs for BS->xAOD mode of operation
        SG::WriteHandleKey< xAOD::jFexSRJetRoIContainer> m_jJWriteKey   {this,"jJRoIContainerWriteKey"  ,"L1_jFexSRJetRoI","Write jFexEM container"};
        SG::WriteHandleKey< xAOD::jFexLRJetRoIContainer> m_jLJWriteKey  {this,"jLJRoIContainerWriteKey" ,"L1_jFexLRJetRoI","Write jFexEM container"};
        SG::WriteHandleKey< xAOD::jFexTauRoIContainer  > m_jTauWriteKey {this,"jTauRoIContainerWriteKey","L1_jFexTauRoI"  ,"Write jFexEDM tau container"};
        SG::WriteHandleKey< xAOD::jFexSumETRoIContainer> m_jTEWriteKey  {this,"jTERoIContainerWriteKey" ,"L1_jFexSumETRoI","Write jFexEDM SumET container"};
        SG::WriteHandleKey< xAOD::jFexMETRoIContainer  > m_jXEWriteKey  {this,"jXERoIContainerWriteKey" ,"L1_jFexMETRoI"  ,"Write jFexEDM Met container"};
        
        // Read handle keys for the L1Calo EDMs for xAOD->BS mode of operation
        SG::ReadHandleKey< xAOD::jFexSRJetRoIContainer> m_jJReadKey   {this,"jJRoIContainerReadKey"  ,"L1_jFexSRJetRoI","Read jFexEM container"};
        SG::ReadHandleKey< xAOD::jFexLRJetRoIContainer> m_jLJReadKey  {this,"jLJRoIContainerReadKey" ,"L1_jFexLRJetRoI","Read jFexEM container"};
        SG::ReadHandleKey< xAOD::jFexTauRoIContainer  > m_jTauReadKey {this,"jTauRoIContainerReadKey","L1_jFexTauRoI"  ,"Read jFexEDM tau container"};
        SG::ReadHandleKey< xAOD::jFexSumETRoIContainer> m_jTEReadKey  {this,"jTERoIContainerReadKey" ,"L1_jFexSumETRoI","Read jFexEDM SumET container"};
        SG::ReadHandleKey< xAOD::jFexMETRoIContainer  > m_jXEReadKey  {this,"jXERoIContainerReadKey" ,"L1_jFexMETRoI"  ,"Read jFexEDM Met container"};        

        std::array<uint32_t,6> TOBCounterTrailer (uint32_t word) const;
        std::array<uint32_t,4> xTOBCounterTrailer(uint32_t word) const;
        std::array<uint32_t,3> jFEXtoRODTrailer  (uint32_t word0, uint32_t word1) const;
        void     jFEXtoRODHeader   (uint32_t word0, uint32_t word1) const;

};

#endif // JFEXBYTESTREAMTOOL_H
