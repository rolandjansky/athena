/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataToolCore_H
#define MUONMmRdoToPrepDataToolCore_H

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "NSWCalibTools/INSWCalibTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>
#include <vector>

class MM_RawDataCollection;

namespace Muon 
{

  class MmRdoToPrepDataToolCore : public extends<AthAlgTool, IMuonRdoToPrepDataTool>
  {
  public:
    MmRdoToPrepDataToolCore(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataToolCore()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    
    /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
    virtual StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) const override;
    //new decode methods for Rob based readout
    StatusCode decode( const std::vector<uint32_t>& robIds, const std::vector<IdentifierHash>& chamberHashInRobs ) const;
    virtual StatusCode decode( const std::vector<uint32_t>& robIds ) const override;
    
    StatusCode processCollection(Muon::MMPrepDataContainer* mmPrepDataContainer,
                                 const std::vector<IdentifierHash>& idsToDecode,
                                 const MM_RawDataCollection *rdoColl, 
   				 std::vector<IdentifierHash>& idWithDataVect) const;

    virtual void printInputRdo() const override;
    virtual void printPrepData() const override;
    
  protected:
    
    virtual Muon::MMPrepDataContainer* setupMM_PrepDataContainer() const = 0;

    const MM_RawDataContainer* getRdoContainer() const;

    void processRDOContainer( Muon::MMPrepDataContainer* mmPrepDataContainer,
                              const std::vector<IdentifierHash>& idsToDecode,
                              std::vector<IdentifierHash>& idWithDataVect ) const;

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
    /// MdtPrepRawData containers
    SG::WriteHandleKey<Muon::MMPrepDataContainer> m_mmPrepDataContainerKey{this, "OutputCollection", "MM_Measurements"};
    SG::ReadHandleKey<MM_RawDataContainer> m_rdoContainerKey{this, "InputCollection", "MMRDO"};

    std::string m_outputCollectionLocation;            
    Gaudi::Property<bool> m_merge{this, "MergePrds", true}; 

    ToolHandle<IMMClusterBuilderTool> m_clusterBuilderTool{this,"ClusterBuilderTool","Muon::SimpleMMClusterBuilderTool/SimpleMMClusterBuilderTool"};
    ToolHandle<INSWCalibTool> m_calibTool{this,"NSWCalibTool", ""};

    // charge cut is temporarily disabled for comissioning studies. Should be reenabled at some point. pscholer 13.05.2022
    Gaudi::Property<float> m_singleStripChargeCut {this,"singleStripChargeCut", FLT_MIN /*6241 * 0.4*/}; // 0.4 fC from BB5 cosmics

  }; 
} // end of namespace

#endif 
