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

#include "bytestreamDecoder/L1CaloRdoEfexTob.h"

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

        // Write handle keys for the L1Calo EDMs for BS->xAOD mode of operation


    // Is multi-slice data decoding required?
    Gaudi::Property<bool> m_multiSlice {
            this, "multiSlice", true, "Decode data from all time slices (false = just from triggered BC)"};

    // Only write keys should be set to non-empty string in python configuration if the tool is in BS->xAOD mode of operation
    // Note that these are the keys for the triggered time slice. In multi-slice mode data from crossings
    // before or after this will be stored in separate containers whose keys have the suffix "OutOfTime"
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMTOBWriteKey {
            this, "eEMTOBContainerWriteKey", "", "Write handle key to eEM TOB (RoI) container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMxTOBWriteKey {
            this, "eEMxTOBContainerWriteKey", "", "Write handle key to eEM xTOB container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUTOBWriteKey {
            this, "eTAUTOBContainerWriteKey", "", "Write handle key to eTAU TOB (RoI) container for conversion from ByteStream"};
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUxTOBWriteKey {
            this, "eTAUxTOBContainerWriteKey", "", "Write handle key to eTAU xTOB container for conversion from ByteStream"};
    // Only read keys should be set to non-empty string in python configuration if the tool is in xAOD->BS mode of operation
    SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEMTOBReadKey {
            this, "eEMTOBContainerReadKey", "", "Read handle key to eEM TOB (RoI) container for conversion from ByteStream"};
    SG::ReadHandleKey<xAOD::eFexEMRoIContainer> m_eEMxTOBReadKey {
            this, "eEMxTOBContainerReadKey", "", "Read handle key to eEM xTOB container for conversion from ByteStream"};
    SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eTAUTOBReadKey {
            this, "eTAUTOBContainerReadKey", "", "Read handle key to eTAU TOB (RoI) container for conversion from ByteStream"};
    SG::ReadHandleKey<xAOD::eFexTauRoIContainer> m_eTAUxTOBReadKey {
            this, "eTAUxTOBContainerReadKey", "", "Read handle key to eTAU xTOB container for conversion from ByteStream"};
    // Additional keys for multi-slice TOB and xTOB readout (values to be defined in initialize based on the keys specified above)
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMTOBSliceKey;
    SG::WriteHandleKey<xAOD::eFexEMRoIContainer> m_eEMxTOBSliceKey;
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUTOBSliceKey;
    SG::WriteHandleKey<xAOD::eFexTauRoIContainer> m_eTAUxTOBSliceKey;

};

#endif // EFEXBYTESTREAMTOOL_H
