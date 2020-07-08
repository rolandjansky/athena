/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLCORE
#define MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOLCORE

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "STgcClusterization/ISTgcClusterBuilderTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>
#include <vector>

namespace Muon 
{
  /** @class STGC_RawDataToPrepDataTool 
   *  This is the algorithm that convert STGC Raw data  To STGC PRD  as a tool.
   */  

  class sTgcRdoToPrepDataToolCore : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
    {
    public:
      /** Constructor */
      sTgcRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~sTgcRdoToPrepDataToolCore()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      
      /** Decode RDO to PRD  
       *  A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.  
       *  @param requestedIdHashVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'  
       *  @return selectedIdHashVect This is the subset of requestedIdVect which were actually found to contain data   
       *  (i.e. if you want you can use this vector of hashes to optimise the retrieval of data in subsequent steps.) */
      virtual
      StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;
      virtual
      StatusCode decode(const std::vector<uint32_t>& robIds) override;

      
      StatusCode processCollection(const STGC_RawDataCollection *rdoColl, 
				   std::vector<IdentifierHash>& idWithDataVect);

      virtual void printPrepData() override;
      virtual void printInputRdo() override;

      /** Resolve possible conflicts with IProperty::interfaceID() */
      static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }
      
    protected:
      
      enum SetupSTGC_PrepDataContainerStatus {
	FAILED = 0, ADDED, ALREADYCONTAINED
      };

      virtual SetupSTGC_PrepDataContainerStatus setupSTGC_PrepDataContainer();

      const STGC_RawDataContainer* getRdoContainer();

      void processRDOContainer(std::vector<IdentifierHash>& idWithDataVect);

      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      bool m_fullEventDone;

      /** TgcPrepRawData container key for current BC */ 
      std::string m_outputCollectionLocation;      
      Muon::sTgcPrepDataContainer* m_stgcPrepDataContainer;
      SG::ReadHandleKey<STGC_RawDataContainer> m_rdoContainerKey;//"TGCRDO"
      SG::WriteHandleKey<sTgcPrepDataContainer> m_stgcPrepDataContainerKey;
      bool m_merge; // merge Prds

      ToolHandle<ISTgcClusterBuilderTool> m_clusterBuilderTool;

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_STGCRDOTOPREPDATATOOL_H
