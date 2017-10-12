/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTGC_RDOtoPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

#include "MuonRDO/TgcRdoContainer.h"

class AtlasDetectorID;
class Identifier;

class ITGCcablingSvc;
class TgcIdHelper;

namespace MuonGM 
{
  class MuonDetectorManager;
  class TgcReadoutElement; 
}

namespace Muon 
{
  class IMuonRawDataProviderTool;
  class TgcCoinData;

  /** @class sTGC_RDOtoPrepDataTool 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class sTGC_RDOtoPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
    {
    public:
      /** Constructor */
      sTGC_RDOtoPrepDataTool(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~sTGC_RDOtoPrepDataTool();
      
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


      /** Resolve possible conflicts with IProperty::interfaceID() */
      static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }
      
    private:
      
      void processPRDHashes( const std::vector<IdentifierHash>& chamberHashInRobs, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds );
      bool handlePRDHash( IdentifierHash hash, const sTGC_RawDataContainer& rdoContainer, std::vector<IdentifierHash>& idWithDataVect, sTgcPrepDataContainer& prds );
      
      /** muon detector manager */
      const MuonGM::MuonDetectorManager * m_muonMgr;

      /** TGC identifier helper */
      const TgcIdHelper* m_tgcHelper;

      /** TgcPrepRawData container key for current BC */ 
      std::string m_outputCollectionLocation;      
            
      /** ToolHandle of the TGC_RawDataProviderTool */
      ToolHandle<IMuonRawDataProviderTool> m_rawDataProviderTool;

      SG::ReadHandleKey<TgcRdoContainer> m_RDO_Key;//"TGCRDO"
      SG::WriteHandleKey<TgcPrepDataContainer> m_PRD_Key;

      /** Aboid compiler warning **/
      virtual StatusCode decode( const std::vector<uint32_t>& /*robIds*/ ) override {return StatusCode::FAILURE;}
   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOL_H
