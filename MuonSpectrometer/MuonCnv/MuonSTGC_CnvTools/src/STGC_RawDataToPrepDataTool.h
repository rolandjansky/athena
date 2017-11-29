/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// STGC_RawDataToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONATGC_CNVTOOLS_STGC_RAWDATATOPREPDATATOOL
#define MUONATGC_CNVTOOLS_STGC_RAWDATATOPREPDATATOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

class AtlasDetectorID;
class Identifier;

class ITGCcablingSvc;
class sTgcIdHelper;

namespace MuonGM 
{
  class MuonDetectorManager;
  class TgcReadoutElement; 
}

namespace Muon 
{
  class IMuonRawDataProviderTool;

  /** @class STGC_RawDataToPrepDataTool 
   *  This is the algorithm that convert STGC Raw data  To STGC PRD  as a tool.
   */  

  class STGC_RawDataToPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
    {
    public:
      /** Constructor */
      STGC_RawDataToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~STGC_RawDataToPrepDataTool();
      
      /** Query the IMuonRdoToPrepDataTool interface */
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      /** Standard AthAlgTool finalize method */
      virtual StatusCode finalize() override;
      
      /** Decode RDO to PRD  
       *  A vector of IdentifierHash are passed in, and the data corresponding to this list (i.e. in a Region of Interest) are converted.  
       *  @param requestedIdHashVect          Vector of hashes to convert i.e. the hashes of ROD collections in a 'Region of Interest'  
       *  @return selectedIdHashVect This is the subset of requestedIdVect which were actually found to contain data   
       *  (i.e. if you want you can use this vector of hashes to optimise the retrieval of data in subsequent steps.) */ 
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;

      virtual void printPrepData() override;
      virtual void printInputRdo() override;

      /** Resolve possible conflicts with IProperty::interfaceID() */
      static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }
      
    private:
      
      void processPRDHashes( const std::vector<IdentifierHash>& chamberHashInRobs, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds );
      bool handlePRDHash( IdentifierHash hash, const STGC_RawDataContainer& rdoContainer, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds );
      
      /** muon detector manager */
      const MuonGM::MuonDetectorManager * m_muonMgr;

      /** TGC identifier helper */
      const sTgcIdHelper* m_sTGC_Helper;

      /** TgcPrepRawData container key for current BC */ 
      std::string m_outputCollectionLocation;      
            
      /** ToolHandle of the TGC_RawDataProviderTool */
      ToolHandle<IMuonRawDataProviderTool> m_rawDataProviderTool;

      SG::ReadHandleKey<STGC_RawDataContainer> m_RDO_Key;//"TGCRDO"
      SG::WriteHandleKey<sTgcPrepDataContainer> m_PRD_Key;

      /** Aboid compiler warning **/
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) override {return StatusCode::FAILURE;}
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
