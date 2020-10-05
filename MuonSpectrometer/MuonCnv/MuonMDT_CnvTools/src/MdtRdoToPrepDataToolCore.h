/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMdtRdoToPrepDataToolCore_H
#define MUONMdtRdoToPrepDataToolCore_H

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MdtCalibSvc/MdtCalibrationTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>

class MdtDigit;
class MdtCalibrationSvcSettings;
class MdtCalibHit;

namespace MuonGM
{    
  class MdtReadoutElement;
}

namespace Muon 
{
  /** @class MdtRdoToPrepDataToolCore 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  

  class MdtRdoToPrepDataToolCore : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
  {
  public:
    MdtRdoToPrepDataToolCore(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~MdtRdoToPrepDataToolCore()=default;
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
      
    /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
    StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) override;
    //new decode method for Rob based readout
    StatusCode decode( const std::vector<uint32_t>& robIds ) override;

    // dump methods for debugging
    virtual void printInputRdo() override;
    virtual void printPrepData() override;
      
  protected:
    virtual StatusCode processCsm(const MdtCsm* rdoColl, std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr, const MdtCsm* rdoColl2=nullptr);

    Muon::MdtDriftCircleStatus getMdtDriftRadius(const MdtDigit* digit, double& radius, double& errRadius, const MuonGM::MdtReadoutElement* descriptor, const MuonGM::MuonDetectorManager* muDetMgr);
 
    virtual StatusCode processCsmTwin(const MdtCsm *rdoColll, std::vector<IdentifierHash>& idWithDataVect, const MuonGM::MuonDetectorManager* muDetMgr);
    // method to get the twin tube 2nd coordinate
    Muon::MdtDriftCircleStatus getMdtTwinPosition(const MdtDigit* prompt_digit, const MdtDigit* twin_digit, double& radius, double& errRadius, double& zTwin, double& errZTwin, bool& twinIsPrompt, const MuonGM::MuonDetectorManager* muDetMgr);

    // adds the container to StoreGate, return false is the adding false. 
    enum SetupMdtPrepDataContainerStatus {
      FAILED = 0, ADDED, ALREADYCONTAINED
    };

    // decode method for Rob based readout
    StatusCode decode( const std::vector<IdentifierHash>& chamberHashInRobs );

    // Overridden by subclasses to handle legacy and MT cases
    virtual SetupMdtPrepDataContainerStatus setupMdtPrepDataContainer() { return SetupMdtPrepDataContainerStatus::FAILED; }

    void processRDOContainer( std::vector<IdentifierHash>& idWithDataVect );

    const MdtCsmContainer* getRdoContainer();
    void processPRDHashes( const std::vector<IdentifierHash>& chamberHashInRobs, std::vector<IdentifierHash>& idWithDataVect );
    bool handlePRDHash( IdentifierHash hash, const MdtCsmContainer& rdoContainer, std::vector<IdentifierHash>& idWithDataVect );    
        
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        
    /// MDT calibration service
    ToolHandle<MdtCalibrationTool> m_calibrationTool{this,"CalibrationTool","MdtCalibrationTool"};
    MdtCalibrationSvcSettings* m_mdtCalibSvcSettings;

    /// MdtPrepRawData containers
    Muon::MdtPrepDataContainer* m_mdtPrepDataContainer;
    SG::WriteHandleKey<Muon::MdtPrepDataContainer> m_mdtPrepDataContainerKey;

    SG::ReadHandleKey< MdtCsmContainer>         m_rdoContainerKey;//MDTCSM

        
    /** member variables for algorithm properties: */
    bool m_calibratePrepData; //!< toggle on/off calibration of MdtPrepData
    bool m_decodeData; //!< toggle on/off the decoding of MDT RDO into MdtPrepData
    bool m_sortPrepData; //!< Toggle on/off the sorting of the MdtPrepData

    ToolHandle<Muon::IMDT_RDO_Decoder> m_mdtDecoder{this,"Decoder","Muon::MdtRDO_Decoder/MdtRDO_Decoder"};
    
    //keepTrackOfFullEventDecoding
    bool m_fullEventDone;

    bool m_BMEpresent;
    bool m_BMGpresent;
    int m_BMEid;
    int m_BMGid;

    // + TWIN TUBE
    bool   m_useTwin;
    bool   m_useAllBOLTwin;
    bool   m_use1DPrepDataTwin;
    bool   m_twinCorrectSlewing;
    bool   m_discardSecondaryHitTwin;
    int m_twin_chamber[2][3][36];
    int m_secondaryHit_twin_chamber[2][3][36];
    // - TWIN TUBE

    std::map<Identifier, std::vector<Identifier> > m_DeadChannels;
    void initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl);

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 

  }; 
} // end of namespace

#endif 
