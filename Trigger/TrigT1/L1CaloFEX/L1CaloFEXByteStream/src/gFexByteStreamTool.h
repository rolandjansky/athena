/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           gFexByteStreamTool  -  description
//                              -------------------
//     begin                : 20 07 2022
//     email                : cecilia.tosciri@cern.ch
//  ***************************************************************************/

#ifndef GFEXBYTESTREAMTOOL_H 
#define GFEXBYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIAuxContainer.h"

#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIAuxContainer.h"
#include "TrigConfData/L1Menu.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class gFEXRoIByteStreamTool
 *  @brief Implementation of a tool for L1 RoI conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 **/
class gFexByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
    public:
        gFexByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~gFexByteStreamTool() override = default;

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
        Gaudi::Property<bool> m_saveExtendedTOBs {this, "SaveExtendedTOBs", false, "Decode and write xTOBs instead of TOBs"};

        // Write handle keys for the L1Calo EDMs for BS->xAOD mode of operation
        SG::WriteHandleKey< xAOD::gFexJetRoIContainer    > m_gFexRhoWriteKey                {this,"gFexRhoOutputContainerWriteKey","L1_gFexRhoRoI","Write gFEX EDM gFexRho container"};
        SG::WriteHandleKey< xAOD::gFexJetRoIContainer    > m_gFexBlockWriteKey              {this,"gFexSRJetOutputContainerWriteKey","L1_gFexSRJetRoI","Write gFEX EDM gFexBlock container"};
        SG::WriteHandleKey< xAOD::gFexJetRoIContainer    > m_gFexJetWriteKey                {this,"gFexLRJetOutputContainerWriteKey","L1_gFexLRJetRoI","Write gFEX EDM gFexJet container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojWriteKey           {this,"gScalarEJwojOutputContainerWriteKey","L1_gScalarEJwoj","Write gFEX EDM Scalar MET and SumET (JwoJ) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojWriteKey     {this,"gMETComponentsJwojOutputContainerWriteKey","L1_gMETComponentsJwoj","Write gFEX EDM total MET components (JwoJ) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojWriteKey     {this,"gMHTComponentsJwojOutputContainerWriteKey","L1_gMHTComponentsJwoj","Write gFEX EDM hard MET components (JwoJ) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojWriteKey     {this,"gMSTComponentsJwojOutputContainerWriteKey","L1_gMSTComponentsJwoj","Write gFEX EDM soft MET components (JwoJ) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsNoiseCutWriteKey {this,"gMETComponentsNoiseCutOutputContainerWriteKey","L1_gMETComponentsNoiseCut","Write gFEX EDM total MET components (NoiseCut) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsRmsWriteKey      {this,"gMETComponentsRmsOutputContainerWriteKey","L1_gMETComponentsRms","Write gFEX EDM total MET components (RMS) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarENoiseCutWriteKey       {this,"gScalarENoiseCutOutputContainerWriteKey","L1_gScalarENoiseCut","Write gFEX EDM Scalar MET and SumET (NoiseCut) container"};
        SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarERmsWriteKey            {this,"gScalarERmsOutputContainerWriteKey","L1_gScalarERms","Write gFEX EDM Scalar MET and SumET (RMS) container"};


        // Read handle keys for the L1Calo EDMs for xAOD->BS mode of operation
        SG::ReadHandleKey< xAOD::gFexJetRoIContainer    > m_gFexRhoReadKey                {this,"gFexRhoOutputContainerReadKey","L1_gFexRhoRoI","Read gFEX EDM gFexRho container"};
        SG::ReadHandleKey< xAOD::gFexJetRoIContainer    > m_gFexBlockReadKey              {this,"gFexSRJetOutputContainerReadKey","L1_gFexSRJetRoI","Read gFEX EDM gFexBlock container"};
        SG::ReadHandleKey< xAOD::gFexJetRoIContainer    > m_gFexJetReadKey                {this,"gFexLRJetOutputContainerReadKey","L1_gFexLRJetRoI","Read gFEX EDM gFexJet container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojReadKey           {this,"gScalarEJwojOutputContainerReadKey","L1_gScalarEJwoj","Read gFEX EDM Scalar MET and SumET (JwoJ) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojReadKey     {this,"gMETComponentsJwojOutputContainerReadKey","L1_gMETComponentsJwoj","Read gFEX EDM total MET components (JwoJ) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojReadKey     {this,"gMHTComponentsJwojOutputContainerReadKey","L1_gMHTComponentsJwoj","Read gFEX EDM hard MET components (JwoJ) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojReadKey     {this,"gMSTComponentsJwojOutputContainerReadKey","L1_gMSTComponentsJwoj","Read gFEX EDM soft MET components (JwoJ) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsNoiseCutReadKey {this,"gMETComponentsNoiseCutOutputContainerReadKey","L1_gMETComponentsNoiseCut","Read gFEX EDM total MET components (NoiseCut) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsRmsReadKey      {this,"gMETComponentsRmsOutputContainerReadKey","L1_gMETComponentsRms","Read gFEX EDM total MET components (RMS) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarENoiseCutReadKey       {this,"gScalarENoiseCutOutputContainerReadKey","L1_gScalarENoiseCut","Read gFEX EDM Scalar MET and SumET (NoiseCut) container"};
        SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarERmsReadKey            {this,"gScalarERmsOutputContainerReadKey","L1_gScalarERms","Read gFEX EDM Scalar MET and SumET (RMS) container"};

        //Read handle key for the L1Menu
        SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey   {this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu","Name of the L1Menu object to read configuration from"}; 

        
};

#endif // GFEXBYTESTREAMTOOL_H
