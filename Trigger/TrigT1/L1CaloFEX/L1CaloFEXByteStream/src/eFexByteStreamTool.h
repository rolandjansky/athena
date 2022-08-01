/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFexByteStreamTool  -  description
//                              -------------------
//     begin                : 11 05 2022
//     email                : will@cern.ch
//  ***************************************************************************/

#ifndef EFEXBYTESTREAMTOOL_H 
#define EFEXBYTESTREAMTOOL_H



// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexTauRoIContainer.h"

#include "xAODTrigL1Calo/eFexTowerContainer.h"

#include "bytestreamDecoder/L1CaloRdoEfexTob.h"
#include "bytestreamDecoder/L1CaloBsDecoderRun3.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class eFEXRoIByteStreamTool
 *  @brief Implementation of a tool for L1 RoI conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 **/
class eFexByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
    public:
        eFexByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~eFexByteStreamTool() override = default;

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

    // Only write keys should be set to non-empty string in python configuration if the tool is in BS->xAOD mode of operation
    // Note that these are the keys for the triggered time slice. In multi-slice mode data from crossings
    // before or after this will be stored in separate containers whose keys have the suffix "OutOfTime"
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMWriteKey {
            this, "eEMContainerWriteKey", "", "Write handle key to eEM TOB container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUWriteKey {
            this, "eTAUContainerWriteKey", "", "Write handle key to eTAU TOB container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMxWriteKey {
            this, "eEMxContainerWriteKey", "", "Write handle key to eEM xTOB container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUxWriteKey {
            this, "eTAUxContainerWriteKey", "", "Write handle key to eTAU xTOB container for conversion from ByteStream"};
    // Additional keys for multi-slice TOB readout
    // Only decoding is implemented, encoding is not needed because simulation does not provide out-of time TOBs
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMSliceWriteKey {
            this, "eEMSliceContainerWriteKey", "", "Write handle key to eEM TOB out-of-time container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUSliceWriteKey {
            this, "eTAUSliceContainerWriteKey", "", "Write handle key to eTAU TOB out-of-time container for conversion from ByteStream"};
    // inputs (eTowers)
    SG::WriteHandleKey<xAOD::eFexTowerContainer> m_eTowerWriteKey {
            this, "eTowerContainerWriteKey", "", "Write handle key to ETower container for conversion from ByteStream"};

    // Only read keys should be set to non-empty string in python configuration if the tool is in xAOD->BS mode of operation
    SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEMReadKey {
            this, "eEMContainerReadKey", "", "Read handle key to eEM (x)TOB container for conversion to ByteStream"};
    SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eTAUReadKey {
            this, "eTAUContainerReadKey", "", "Read handle key to eTAU (x)TOB container for conversion to ByteStream"};





    std::unique_ptr<L1CaloBsDecoderRun3> m_decoder;

};

#endif // EFEXBYTESTREAMTOOL_H
