/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MmRdoToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMmRdoToPrepDataTool_H
#define MUONMmRdoToPrepDataTool_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"

#include <string>
#include <vector>
class AtlasDetectorID;
class Identifier;
class MmIdHelper;
class MuonIdHelper;
class MM_RawDataCollection;
class StoreGateSvc;

namespace MuonGM
{    
    class MuonDetectorManager;
    class MMReadoutElement;
}


namespace Muon 
{

  class IMuonRawDataProviderTool;
  class MuonIdHelperTool;
  class IMMClusterBuilderTool;

  class MmRdoToPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
  {
  public:
    MmRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataTool ();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();
    
    /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
    StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect );
    //new decode methods for Rob based readout
    StatusCode decode( const std::vector<uint32_t>& robIds, const std::vector<IdentifierHash>& chamberHashInRobs );
    StatusCode decode( const std::vector<uint32_t>& robIds );
    
    StatusCode processCollection(const MM_RawDataCollection *rdoColl, 
   				 std::vector<IdentifierHash>& idWithDataVect);

    void printInputRdo();
    void printPrepData();
    
  private:
    
    enum SetupMM_PrepDataContainerStatus {
      FAILED = 0, ADDED, ALREADYCONTAINED
    };

    SetupMM_PrepDataContainerStatus setupMM_PrepDataContainer();

    const MM_RawDataContainer* getRdoContainer();

    void processRDOContainer( std::vector<IdentifierHash>& idWithDataVect );

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager * m_muonMgr;
    
    /// MM and muon identifier helper
    const MmIdHelper * m_mmIdHelper;
    ToolHandle <Muon::MuonIdHelperTool> m_idHelperTool; 
    
    bool m_fullEventDone;
    
    /// MdtPrepRawData containers
    Muon::MMPrepDataContainer * m_mmPrepDataContainer;
    SG::WriteHandleKey<Muon::MMPrepDataContainer> m_mmPrepDataContainerKey;
    SG::ReadHandleKey< MM_RawDataContainer >         m_rdoContainerKey;

    std::string m_outputCollectionLocation;            
    bool m_merge; 

    ToolHandle<IMMClusterBuilderTool> m_clusterBuilderTool;

  }; 
} // end of namespace

#endif 
