/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVPathologyDbCondAlg.h" 
#include "LArRecConditions/LArHVPathologiesDb.h"
#include "LArRecConditions/LArHVPathology.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListException.h"
#include "CoralBase/Blob.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"

#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"

#include "TBufferFile.h"
#include "TClass.h"


LArHVPathologyDbCondAlg::LArHVPathologyDbCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name,pSvcLocator)
{ }

LArHVPathologyDbCondAlg::~LArHVPathologyDbCondAlg()
{ }

StatusCode LArHVPathologyDbCondAlg::initialize()
{
  ATH_CHECK(m_pathologyFolderKey.initialize());
  ATH_CHECK(m_hvMappingKey.initialize());
  ATH_CHECK(m_hvPAthologyKey.initialize());
  
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );

  m_larem_id   = idHelper->em_idHelper();
  m_larhec_id   = idHelper->hec_idHelper();
  m_larfcal_id   = idHelper->fcal_idHelper();

  ATH_CHECK( detStore()->retrieve (m_calodetdescrmgr, "CaloMgr") );
  ATH_CHECK( detStore()->retrieve(m_laronline_id,"LArOnlineID") );
  ATH_CHECK(detStore()->retrieve(m_hvlineHelper,"LArHVLineID"));

  return StatusCode::SUCCESS;
}

StatusCode LArHVPathologyDbCondAlg::finalize()
{
  return StatusCode::SUCCESS;
}

/*
AthenaAttributeList* LArHVPathologyDbCondAlg::hvPathology2AttrList(const LArHVPathologiesDb& pathologyContainer)
{
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();

  spec->extend("blobVersion","unsigned int");   //Should allow schema evolution if needed
  spec->extend("Constants","blob");             //Holds the container
 
  AthenaAttributeList* attrList = new AthenaAttributeList(*spec);
     
  (*attrList)["blobVersion"].data<unsigned int>()=(unsigned int)0;
  coral::Blob& blob=(*attrList)["Constants"].data<coral::Blob>();
     
  TClass* klass = TClass::GetClass("LArHVPathologiesDb");
  if (klass==NULL) {
    ATH_MSG_ERROR ( "Can't find TClass LArHVPathologiesDb" );
    return 0;
  }
  else
    ATH_MSG_DEBUG ( "Got TClass LArHVPathologiesDb" );
 
  TBufferFile buf(TBuffer::kWrite);
 
  if(buf.WriteObjectAny(&pathologyContainer, klass)!=1) {
    ATH_MSG_ERROR ( "Failed to stream LArHVPathologiesDb" );
    return 0;
  }
  
  blob.resize(buf.Length());
  void* adr = blob.startingAddress();
  memcpy(adr,buf.Buffer(),buf.Length());
  return attrList;
}
*/

StatusCode LArHVPathologyDbCondAlg::execute(const EventContext& ctx) const {

  SG::WriteCondHandle<LArHVPathology > writeHandle{m_hvPAthologyKey, ctx};

     if (writeHandle.isValid()) {
       ATH_MSG_DEBUG("Found valid write handle");
       return StatusCode::SUCCESS;
     }

     //Get Conditions input
     SG::ReadCondHandle<AthenaAttributeList> fldrHdl(m_pathologyFolderKey, ctx);
     const AthenaAttributeList *attrList = *fldrHdl;
     if (!attrList) {
        ATH_MSG_ERROR("Do not have AthenaAttributeList for pathology");
        return StatusCode::FAILURE;
     }

     try {
       const unsigned blobVersion=(*attrList)["blobVersion"].data<unsigned int>();
       const coral::Blob& blob = (*attrList)["Constants"].data<coral::Blob>();
    
       if (blobVersion!=0) {
         ATH_MSG_ERROR ( "Can't interpret BLOB version " << blobVersion );
         return  StatusCode::FAILURE;
       }
        
       TClass* klass = TClass::GetClass("LArHVPathologiesDb");
       if(klass==NULL){
         ATH_MSG_ERROR ( "Can't find TClass LArHVPathologiesDb" );
         return  StatusCode::FAILURE;
       }
       else
         ATH_MSG_DEBUG ( "Got TClass LArHVPathologiesDb" );
    
       TBufferFile buf(TBuffer::kRead, blob.size(), (void*)blob.startingAddress(), false);
       LArHVPathologiesDb* hvpathdb = (LArHVPathologiesDb*)buf.ReadObjectAny(klass);

       std::unique_ptr<LArHVPathology> hvpath=std::make_unique<LArHVPathology>(hvpathdb);
       fillElectMap(hvpath.get());

       // Define validity of the output cond object and record it
       EventIDRange rangeW;
       if(!fldrHdl.range(rangeW)) {
          ATH_MSG_ERROR("Failed to retrieve validity range for " << fldrHdl.key());
          return StatusCode::FAILURE;
       }

       if(writeHandle.record(rangeW,hvpath.release()).isFailure()) {
          ATH_MSG_ERROR("Could not record LArHVPathology  object with "
                  << writeHandle.key()
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
          return StatusCode::FAILURE;
       }

       return StatusCode::SUCCESS;
    
  }catch (coral::AttributeListException &e) {
    ATH_MSG_ERROR ( e.what() );
  }
  // should not come here, but syntactically
  return StatusCode::SUCCESS;
}

void LArHVPathologyDbCondAlg::fillElectMap(LArHVPathology* hvpath) const {

  SG::ReadCondHandle<LArHVIdMapping> cHdl{m_hvMappingKey};
  const LArHVIdMapping* hvCabling = *cHdl;
  if(!hvCabling) {
     ATH_MSG_WARNING("Do not have HV mapping, will not fill LArHVPathology electIndMap !!!");
     return;
  }

  std::map<std::pair<Identifier, unsigned int>, std::vector<unsigned short> > &elecMap = hvpath->getElecMap(); //shorthand

  std::vector<unsigned short> list;
  std::vector<HWIdentifier> hwlineId;
  unsigned int HVline;
  // loop over all EM Identifiers
  for (auto id: m_larem_id->channel_ids()) {
     hwlineId.clear();
     hvCabling->getHVLineInCell(id,hwlineId);
     // LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id) > 0)  {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         for(auto hwid:hwlineId) {
           list.clear();
           HVline = m_hvlineHelper->hv_line(hwid);
           for (unsigned int i=0;i<nelec;i++) {
             const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
             for (unsigned int igap=0;igap<2;igap++) {
               if ((unsigned)electrode->hvLineNo(igap)==HVline) {
                  list.push_back(2*i+igap);
               }
             } 
           }
         }
         elecMap.insert(std::make_pair(std::make_pair(id,HVline) ,list));
         continue;
       }
     }
     // LAr EMEC
     if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0) {
       if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         for(auto hwid:hwlineId) {
           list.clear();
           HVline = m_hvlineHelper->hv_line(hwid);
           for (unsigned int i=0;i<nelec;i++) {
             const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
             for (unsigned int igap=0;igap<2;igap++) {
               if ((unsigned)electrode->hvLineNo(igap)==HVline) {
                  list.push_back(2*i+igap);
               }       
             }       
           }    
         }
         elecMap.insert(std::make_pair(std::make_pair(id,HVline),list));
         continue;
       }
     }
     // EMBPS
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id)==0) {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
        const EMBCellConstLink cell = embElement->getEMBCell();
        const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule ();
        for(auto hwid:hwlineId) {
          list.clear();
          HVline = m_hvlineHelper->hv_line(hwid);
          for (unsigned int igap=0;igap<2;igap++) {
            if ((unsigned)hvmodule->hvLineNo(igap)==HVline) {
              list.push_back(igap);
            }
          }
          elecMap.insert(std::make_pair(std::make_pair(id,HVline),list));
          continue;
        }
       }
     }
    // EMECPS
    if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id)==0) {
      if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
       for(auto hwid:hwlineId) {
         list.clear();
         HVline = m_hvlineHelper->hv_line(hwid);
         for (unsigned int igap=0;igap<2;igap++) {
           if ((unsigned)hvmodule->hvLineNo(igap)==HVline) {
             list.push_back(igap);
           }
         }
         elecMap.insert(std::make_pair(std::make_pair(id,HVline),list));
         continue;
       }
      }
    }
  }
  // loop over all HEC Identifiers
  for (auto const id: m_larhec_id->channel_ids()) {
     hwlineId.clear();
     hvCabling->getHVLineInCell(id,hwlineId);
     if (const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      for(auto hwid:hwlineId) {
        list.clear();
        HVline = m_hvlineHelper->hv_line(hwid);
        for (unsigned int i=0;i<nsubgaps;i++) {
          const HECHVSubgapConstLink subgap = cell->getSubgap(i);
          if ((unsigned)subgap->hvLineNo()==HVline) {
            list.push_back(i);
          }
        }
        elecMap.insert(std::make_pair(std::make_pair(id,HVline),list));
        continue;
      }
     }
  }
  // loop over all FCAL Identifiers
  for (auto const id: m_larfcal_id->channel_ids()) {
     hwlineId.clear();
     hvCabling->getHVLineInCell(id,hwlineId);
     if (const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const FCALTile* tile = fcalElement->getFCALTile();
       unsigned int nlines = tile->getNumHVLines();
       for(auto hwid:hwlineId) {
         list.clear();
         HVline = m_hvlineHelper->hv_line(hwid);
         for (unsigned int i=0;i<nlines;i++) {
           const FCALHVLineConstLink line2 = tile->getHVLine(i);
           if (line2) {
             if ((unsigned)line2->hvLineNo()==HVline) {
               list.push_back(i);
             }
           }
         }
         elecMap.insert(std::make_pair(std::make_pair(id,HVline),list));
         continue;
       }
     }
  }

  return;

}
