/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFexInputByteStreamTool  -  This tool decodes Run3 jFEX input data!
//                              -------------------
//     begin                : 01 07 2022
//     email                : Sergi.Rodriguez@cern.ch
//  ***************************************************************************/

#ifndef JFEXINPUTBYTESTREAMTOOL_H 
#define JFEXINPUTBYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"


// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class jFexInputByteStreamTool
 *  @brief Implementation of a tool for L1 input data conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 **/
class jFexInputByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
    public:
        jFexInputByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~jFexInputByteStreamTool() override = default;

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
        //SG::WriteHandleKey< xAOD::jFexSRJetRoIContainer> m_jJWriteKey   {this,"jJRoIContainerWriteKey"  ,"L1_jFexSRJetRoI","Write jFexEDM SRjet container"};
        
        // Read handle keys for the L1Calo EDMs for xAOD->BS mode of operation
        //SG::ReadHandleKey< xAOD::jFexSRJetRoIContainer> m_jJReadKey   {this,"jJRoIContainerReadKey"  ,"L1_jFexSRJetRoI","Read jFexEDM SRjet container"};

        std::array<uint32_t,3> jFEXtoRODTrailer  (uint32_t, uint32_t) const;
        void BulkStreamTrailer (uint32_t , uint32_t ) const;
        std::array<uint16_t,3>  Dataformat1 (uint32_t ) const;
        std::array<uint16_t,4>  Dataformat2 (uint32_t ) const;
        bool m_verbose = 1;
        
};

#endif // JFEXINPUTBYTESTREAMTOOL_H
