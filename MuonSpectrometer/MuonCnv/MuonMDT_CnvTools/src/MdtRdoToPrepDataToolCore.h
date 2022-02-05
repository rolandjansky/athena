/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMdtRdoToPrepDataToolCore_H
#define MUONMdtRdoToPrepDataToolCore_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class MdtDigit;
class MdtCalibrationSvcSettings;
class MdtCalibHit;

namespace MuonGM {
    class MdtReadoutElement;
}

namespace Muon {
    /** @class MdtRdoToPrepDataToolCore

        This is for the Doxygen-Documentation.
        Please delete these lines and fill in information about
        the Algorithm!
        Please precede every member function declaration with a
        short Doxygen comment stating the purpose of this function.

        @author  Edward Moyse <Edward.Moyse@cern.ch>
    */

    class MdtRdoToPrepDataToolCore : public extends<AthAlgTool, IMuonRdoToPrepDataTool> {
    public:
        MdtRdoToPrepDataToolCore(const std::string&, const std::string&, const IInterface*);

        /** default destructor */
        virtual ~MdtRdoToPrepDataToolCore() = default;

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize() override;

        /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
        virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect) const override;
        // new decode method for Rob based readout
        virtual StatusCode decode(const std::vector<uint32_t>& robIds) const override;

        // dump methods for debugging
        virtual void printInputRdo() const override;

    protected:
        void printPrepDataImpl(const Muon::MdtPrepDataContainer* mdtPrepDataContainer) const;

        virtual StatusCode processCsm(Muon::MdtPrepDataContainer* mdtPrepDataContainer, const MdtCsm* rdoColl,
                                      std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr,
                                      const MdtCsm* rdoColl2 = nullptr) const;

        Muon::MdtDriftCircleStatus getMdtDriftRadius(const MdtDigit* digit, double& radius, double& errRadius,
                                                     const MuonGM::MdtReadoutElement* descriptor,
                                                     const MuonGM::MuonDetectorManager* muDetMgr) const;

        StatusCode processCsmTwin(Muon::MdtPrepDataContainer* mdtPrepDataContainer, const MdtCsm* rdoColll,
                                  std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr) const;
        // method to get the twin tube 2nd coordinate
        Muon::MdtDriftCircleStatus getMdtTwinPosition(const MdtDigit* prompt_digit, const MdtDigit* twin_digit, double& radius,
                                                      double& errRadius, double& zTwin, double& errZTwin, bool& twinIsPrompt,
                                                      const MuonGM::MuonDetectorManager* muDetMgr) const;

        // decode method for Rob based readout
        StatusCode decode(const std::vector<IdentifierHash>& chamberHashInRobs) const;

        // Overridden by subclasses to handle legacy and MT cases
        virtual Muon::MdtPrepDataContainer* setupMdtPrepDataContainer(unsigned int sizeVectorRequested, bool& fullEventDone) const = 0;

        void processRDOContainer(Muon::MdtPrepDataContainer* mdtPrepDataContainer, std::vector<IdentifierHash>& idWithDataVect) const;

        const MdtCsmContainer* getRdoContainer() const;
        void processPRDHashes(Muon::MdtPrepDataContainer* mdtPrepDataContainer, const std::vector<IdentifierHash>& chamberHashInRobs,
                              std::vector<IdentifierHash>& idWithDataVect) const;
        bool handlePRDHash(Muon::MdtPrepDataContainer* mdtPrepDataContainer, IdentifierHash hash, const MdtCsmContainer& rdoContainer,
                           std::vector<IdentifierHash>& idWithDataVect) const;

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        /// MDT calibration service
        ToolHandle<MdtCalibrationTool> m_calibrationTool{this, "CalibrationTool", "MdtCalibrationTool"};
        MdtCalibrationSvcSettings* m_mdtCalibSvcSettings;

        /// MdtPrepRawData containers
        SG::WriteHandleKey<Muon::MdtPrepDataContainer> m_mdtPrepDataContainerKey;

        SG::ReadHandleKey<MdtCsmContainer> m_rdoContainerKey;  // MDTCSM

        /** member variables for algorithm properties: */
        bool m_calibratePrepData;  //!< toggle on/off calibration of MdtPrepData
        bool m_decodeData;         //!< toggle on/off the decoding of MDT RDO into MdtPrepData
        bool m_sortPrepData = false;       //!< Toggle on/off the sorting of the MdtPrepData

        ToolHandle<Muon::IMDT_RDO_Decoder> m_mdtDecoder{this, "Decoder", "Muon::MdtRDO_Decoder/MdtRDO_Decoder"};

        bool m_BMEpresent;
        bool m_BMGpresent;
        int m_BMEid;
        int m_BMGid;

        // + TWIN TUBE
        bool m_useTwin;
        bool m_useAllBOLTwin;
        bool m_use1DPrepDataTwin;
        bool m_twinCorrectSlewing;
        bool m_discardSecondaryHitTwin;
        int m_twin_chamber[2][3][36]{};
        int m_secondaryHit_twin_chamber[2][3][36]{};
        // - TWIN TUBE

        std::map<Identifier, std::vector<Identifier> > m_DeadChannels;
        void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

        SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                         "Key of input MuonDetectorManager condition data"};
    };
}  // namespace Muon

#endif
