/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVPathologyDbAlg.h"
#include "LArElecCalib/ILArHVPathologyDbTool.h"
#include "LArRecConditions/LArHVPathologiesDb.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArIdentifier/LArOnlineID.h"

#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLine.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <fstream>
#include <cstdlib>

LArHVPathologyDbAlg::LArHVPathologyDbAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator)
  , m_writeCondObjs(false)
  , m_inpFile("LArHVPathology.inp")
  , m_outFile("")
  , m_folder("")
  , m_outpTag("HVPathologies-TEST")
  , m_regSvc("IOVRegistrationSvc",name)
  , m_pathologyTool("LArHVPathologyDbTool")
  , m_mode(0)
{
  declareProperty("WriteCondObjs", m_writeCondObjs);
  declareProperty("InpFile",       m_inpFile);
  declareProperty("OutFile",       m_outFile);
  declareProperty("Folder",        m_folder);
  declareProperty("TagName",       m_outpTag);
  declareProperty("Mode",  m_mode,"Mode to read file (0=offlineID/elecID, 1=online ID fields + HV module/line, 2=type is HV value to overwrite)");
}

LArHVPathologyDbAlg::~LArHVPathologyDbAlg()
{
}

StatusCode LArHVPathologyDbAlg::initialize()
{
  ATH_MSG_INFO(" in initialize()");

  if(m_folder.value().empty()) {
    ATH_MSG_ERROR("Folder property not set. Exiting ... ");
    return StatusCode::FAILURE;
  }

  // Get HVPathology tool
  ATH_CHECK(m_pathologyTool.retrieve());

  // Get the IOVRegistrationSvc when needed
  if(m_writeCondObjs) ATH_CHECK(m_regSvc.retrieve());

  // retrieve LArEM id helpers
  ATH_CHECK(detStore()->retrieve(m_caloIdMgr));

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

  ATH_CHECK(detStore()->retrieve(m_laronline_id,"LArOnlineID"));

  ATH_CHECK( m_hvCablingKey.initialize() );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_caloMgrKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode LArHVPathologyDbAlg::execute()
{
  ATH_MSG_INFO(" in execute()");

  const EventContext& ctx = Gaudi::Hive::currentContext();

  int nevt = ctx.eventID().event_number();

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey, ctx};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrManager* calodetdescrmgr = *caloMgrHandle;

  if(m_writeCondObjs && nevt==1) {
    ATH_MSG_INFO("Creating conditions objects");

    // Create cond objects
    if(!createCondObjects(ctx,calodetdescrmgr).isSuccess()) {
      ATH_MSG_ERROR("Could not create cond objects ");
      m_writeCondObjs = false;
      return StatusCode::FAILURE;
    }
  }

  // Dump cond objects
  StatusCode sc = printCondObjects (ctx,calodetdescrmgr);
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not print out cond objects");
    return sc;
  }

  return sc;
}

StatusCode LArHVPathologyDbAlg::stop()
{
  ATH_MSG_INFO(" in stop()");
  StatusCode sc= StatusCode::SUCCESS;
 
  if(m_writeCondObjs) {
    sc = registerCondObjects();
    if(!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not register objects");
      return sc;
    } 
    else {
      ATH_MSG_INFO("Register OK");
    }
  }

  return sc;
}

StatusCode LArHVPathologyDbAlg::createCondObjects (const EventContext & ctx, const CaloDetDescrManager* calodetdescrmgr)
{
  ATH_MSG_INFO(" in createCondObjects() ");

  if(detStore()->contains<AthenaAttributeList>(m_folder)) {
    ATH_MSG_INFO("EMB Pathologies already in SG, skipping ");
  }
  else {
    SG::ReadCondHandle<LArHVIdMapping> hvIdMapping (m_hvCablingKey, ctx);

    SG::ReadCondHandle<LArOnOffIdMapping> cabHdl (m_cablingKey, ctx);
    const LArOnOffIdMapping *cabling = *cabHdl;
    if(!cabling) {
       ATH_MSG_ERROR("Do not have cabling object with key " << m_cablingKey.key());
       return StatusCode::FAILURE;
    }
    // Read input file and construct LArHVPathologiesDb for given folder
    std::ifstream infile;
    infile.open(m_inpFile.value().c_str());

    if(!infile.is_open()) {
      ATH_MSG_ERROR("Unable to open " << m_inpFile << " for reading");
      return StatusCode::FAILURE;
    }

    char checkChar;
    char commentSign('#');
    std::string commentLine(""), foldername("");
    unsigned int cellID;
    unsigned short electInd, pathologyType;

    // Look for the folder name in the file
    while(!infile.eof()) {
      infile >> foldername;
      if(foldername==m_folder.value())
	break;
    }

    if(foldername!=m_folder.value()) {
      ATH_MSG_ERROR("Unable to find data for the folder " << m_folder 
		      << " in the input file");
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("Found folder " << foldername << " in the input file");
      
    // Get data corresponding to the folder and put it into LArHVPathologiesDb object
    LArHVPathologiesDb pathologies;
    
    ATH_MSG_INFO(" start reading input file ");
    while(!infile.eof()) {
      // Number or string?
      checkChar = static_cast<char> (infile.get());
      ATH_MSG_INFO(" checChar " << checkChar);
      if(checkChar=='\n')
	continue;
      if((checkChar >= '0') && (checkChar <= '9')) {
	// Number - read three values
	infile.unget();
        std::vector<unsigned int> elecList;
        elecList.reserve(2);
        if (m_mode==0) {
	  infile >> cellID >> electInd >> pathologyType;
          elecList.push_back(electInd);
        } else {
          unsigned int bec,pos_neg,FT,slot,channel,hvModule,hvLine;
          infile >> bec >> pos_neg >> FT >> slot >> channel >> hvModule >> hvLine >> pathologyType;
          ATH_MSG_INFO(" read " << bec << " " << pos_neg << " " << FT << " " << slot << " " << channel << " " << hvModule << " " << hvLine << " " << pathologyType);
          HWIdentifier hwid = m_laronline_id->channel_Id(bec,pos_neg,FT,slot,channel);
          Identifier id = cabling->cnvToIdentifier( hwid);
          cellID = (unsigned int)(id.get_identifier32().get_compact());
          elecList=getElectInd(**hvIdMapping, id,hvModule,hvLine,calodetdescrmgr);
          ATH_MSG_INFO(" cellId , elecList size " << cellID << " " << elecList.size());
        }
        for (unsigned int i=0;i<elecList.size();i++) {
	 LArHVPathologiesDb::LArHVElectPathologyDb electPath{};
	 electPath.cellID = cellID;
	 electPath.electInd = elecList[i];
         if(m_mode==2) electPath.pathologyType = ((pathologyType&0x0FFF)<<4);
         else electPath.pathologyType = pathologyType;
	 pathologies.m_v.push_back(electPath);
	 ATH_MSG_INFO("Created electrode pathology (" << cellID
		       << "," << elecList[i]
		       << "," << pathologyType << ")");
        }
      }
      else if(checkChar==commentSign) {
	// Skip the comment
	std::getline(infile,commentLine);
      }
      else {
	// We found another folder. Stop parsing
	break;
      }
    }

    infile.close();
    ATH_MSG_INFO("Finished parsing input file");    

    AthenaAttributeList* attrlist = m_pathologyTool->hvPathology2AttrList(pathologies);
    ATH_MSG_INFO("Created Attribute List");

    StatusCode sc = detStore()->record(attrlist,m_folder);
    if(!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not record " << m_folder);
      return sc;
    }
    else
      ATH_MSG_INFO("Recorded " << m_folder);
  }
 
  return StatusCode::SUCCESS;
}

StatusCode LArHVPathologyDbAlg::printCondObjects (const EventContext& ctx, const CaloDetDescrManager* calodetdescrmgr)
{
  ATH_MSG_INFO(" in printCondObjects() ");

  SG::ReadCondHandle<LArHVIdMapping> hvIdMapping (m_hvCablingKey, ctx);
  SG::ReadCondHandle<LArOnOffIdMapping> cabHdl (m_cablingKey, ctx);
  const LArOnOffIdMapping *cabling = *cabHdl;
  if(!cabling) {
     ATH_MSG_ERROR("Do not have cabling object with key " << m_cablingKey.key());
     return StatusCode::FAILURE;
  }

  std::ofstream *fout=0;
  const AthenaAttributeList* attrlist = nullptr;
  StatusCode sc = detStore()->retrieve(attrlist,m_folder);

  if(sc.isFailure())
    ATH_MSG_WARNING("Could not find object for " << m_folder);
  else {
    LArHVPathologiesDb* pathologyContainer = m_pathologyTool->attrList2HvPathology(*attrlist);
    ATH_MSG_INFO("Unpacked pathologies from Attribute List for " << m_folder);
    if(m_outFile.value().size()>0) {
       fout = new std::ofstream(m_outFile.value().c_str());
       if((!fout) || (fout && !(fout->good()))) {
             ATH_MSG_WARNING("Could not open output file: " << m_outFile.value());
             fout=0;
             }
       if(fout) *fout<<m_folder.value()<<std::endl;
    }
    for(unsigned i=0; i<pathologyContainer->m_v.size(); ++i) {
      LArHVPathologiesDb::LArHVElectPathologyDb electPath = pathologyContainer->m_v[i];
      if(m_mode==0) {
         ATH_MSG_INFO("Got pathology for cell ID: " << electPath.cellID
      	     << "(" << electPath.electInd 
      	     << "," << electPath.pathologyType << ") ");
         if(fout) *fout<<electPath.cellID<<"\t"<<electPath.electInd<<"\t"<<electPath.pathologyType<<std::endl;    
      } else {
         ATH_MSG_INFO("Got pathology for cell ID: " << electPath.cellID);
         HWIdentifier hwid = cabling->createSignalChannelID(Identifier32(electPath.cellID));
         int HVLine=getHVline(**hvIdMapping,Identifier(electPath.cellID),electPath.electInd,calodetdescrmgr);
         if(HVLine<0) {
	   ATH_MSG_ERROR("No HVline for cell "<<electPath.cellID);
         } else {
            int hvmodule=HVLine/1000;
            int hvline=HVLine%1000;
            if(m_mode==1) {
              ATH_MSG_INFO(m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << electPath.pathologyType);
              if(fout) *fout << m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << electPath.pathologyType << std::endl;
            } else if (m_mode==2){
              ATH_MSG_INFO(m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << ((electPath.pathologyType&0xFF0)>>4));
              if(fout) *fout << m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << ((electPath.pathologyType&0xFFF0)>>4) << std::endl;

            }
         }
      }
    }
    delete pathologyContainer;
  }
  if(fout) fout->close();
  return sc;
}

StatusCode LArHVPathologyDbAlg::registerCondObjects()
{
  ATH_MSG_INFO("entering registerCondObject()" );

  std::string objname("AthenaAttributeList");

  StatusCode sc = m_regSvc->registerIOV(objname, m_folder, m_outpTag);
  if(!sc.isSuccess()) 
    ATH_MSG_ERROR("Could not register (" << objname << ", " << m_outpTag << ") in IOV DB ");
  else
    ATH_MSG_INFO("Successfully registered");

  return sc;
}
 
std::vector<unsigned int>
LArHVPathologyDbAlg::getElectInd(const LArHVIdMapping& hvIdMapping,
                                 const Identifier & id,
                                 unsigned int module,
                                 unsigned int line,
				 const CaloDetDescrManager* calodetdescrmgr)
{

  std::vector<unsigned int> list;
  list.clear();
  int HVline = 1000*module + line;
// EM calo
  if (m_larem_id->is_lar_em(id)) {
// LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id) > 0)  {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id))) {
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         for (unsigned int i=0;i<nelec;i++) {
            const EMBHVElectrode& electrode = cell->getElectrode(i);
            for (unsigned int igap=0;igap<2;igap++) {
              if (electrode.hvLineNo(igap, &hvIdMapping)==HVline) {
                  list.push_back(2*i+igap);
              }
            } 
         }
       }
     }
// LAr EMEC
     if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0) {
       if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id))) {
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         for (unsigned int i=0;i<nelec;i++) {
            const EMECHVElectrode& electrode = cell->getElectrode(i);
            for (unsigned int igap=0;igap<2;igap++) {
              if (electrode.hvLineNo(igap, &hvIdMapping)==HVline) {
                  list.push_back(2*i+igap);
              }       
            }       
         }    
       }
     }
// EMBPS
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id)==0) {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id))) {
        const EMBCellConstLink cell = embElement->getEMBCell();
        const EMBPresamplerHVModule& hvmodule =  cell->getPresamplerHVModule ();
        for (unsigned int igap=0;igap<2;igap++) {
           if (hvmodule.hvLineNo(igap, &hvIdMapping)==HVline) {
             list.push_back(igap);
           }
        }
       }
     }
// EMECPS
    if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id)==0) {
      if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id))) {
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModule& hvmodule = cell->getPresamplerHVModule ();
       for (unsigned int igap=0;igap<2;igap++) {
        if (hvmodule.hvLineNo(igap, &hvIdMapping)==HVline) {
          list.push_back(igap);
        }
       }
      }
    }
  }
//HEC
  if (m_larhec_id->is_lar_hec(id)) {
    if (const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(calodetdescrmgr->get_element(id))) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      for (unsigned int i=0;i<nsubgaps;i++) {
          const HECHVSubgap& subgap = cell->getSubgap(i);
          if (subgap.hvLineNo(&hvIdMapping)==HVline) {
            list.push_back(i);
          }
      }
    }
  }
//FCAL
  if (m_larfcal_id->is_lar_fcal(id)) {
    if (const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(calodetdescrmgr->get_element(id))) {
       const FCALTile* tile = fcalElement->getFCALTile();
       unsigned int nlines = tile->getNumHVLines();
       for (unsigned int i=0;i<nlines;i++) {
         const FCALHVLine* line2 = tile->getHVLine(i);
	 if(line2) {
	   if (line2->hvLineNo(&hvIdMapping)==HVline) {
	     list.push_back(i);
	   }
	 }
       }
    }
  }

  return list;

}

int LArHVPathologyDbAlg::getHVline(const LArHVIdMapping& hvIdMapping,
                                   const Identifier & id,
                                   short unsigned int ElectInd,
				   const CaloDetDescrManager* calodetdescrmgr)
{

  unsigned int igap, ielec;
// EM calo
  if (m_larem_id->is_lar_em(id)) {
// LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id) > 0)  {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id))) {
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         igap = ElectInd % 2;
         ielec = std::div(ElectInd - igap, 2).quot;
         if (ielec > nelec) {
            ATH_MSG_ERROR("Wrong electrode number " << ielec << " for cell "<< id.get_identifier32().get_compact());
            return -1;
         } else { 
            return cell->getElectrode(ielec).hvLineNo(igap, &hvIdMapping);
         }
       }
     }
// LAr EMEC
     if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0) {
       if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id))) {
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         igap = ElectInd % 2;
         ielec = std::div(ElectInd - igap, 2).quot;
         if (ielec > nelec) {
            ATH_MSG_ERROR("Wrong electrode number " << ielec << " for cell "<< id.get_identifier32().get_compact());
            return -1;
         } else { 
            return cell->getElectrode(ielec).hvLineNo(igap, &hvIdMapping);
         }
       }
     }
// EMBPS
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id)==0) {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(calodetdescrmgr->get_element(id))) {
        const EMBCellConstLink cell = embElement->getEMBCell();
        const EMBPresamplerHVModule& hvmodule =  cell->getPresamplerHVModule ();
        if(ElectInd >= 2) {
            ATH_MSG_ERROR("Wrong igap "<<ElectInd<<" for EMBPS cell "<<id.get_identifier32().get_compact());
            return -1;
        } else {
            return hvmodule.hvLineNo(ElectInd, &hvIdMapping);
        }
       }
     }
// EMECPS
    if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id)==0) {
      if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(calodetdescrmgr->get_element(id))) {
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModule& hvmodule = cell->getPresamplerHVModule ();
        if(ElectInd >= 2) {
            ATH_MSG_ERROR("Wrong igap "<<ElectInd<<" for EMECPS cell "<<id.get_identifier32().get_compact());
            return -1;
        } else {
            return hvmodule.hvLineNo(ElectInd, &hvIdMapping);
        }
      }
    }
  }
//HEC
  if (m_larhec_id->is_lar_hec(id)) {
    if (const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(calodetdescrmgr->get_element(id))) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      if( ElectInd >= nsubgaps) {
         ATH_MSG_ERROR("Wrong igap "<<ElectInd<<" for HEC cell "<<id.get_identifier32().get_compact());
         return -1;
      } else {
         return cell->getSubgap(ElectInd).hvLineNo(&hvIdMapping);
      }
    }
  }
//FCAL
  if (m_larfcal_id->is_lar_fcal(id)) {
    if (const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(calodetdescrmgr->get_element(id))) {
       const FCALTile* tile = fcalElement->getFCALTile();
       unsigned int nlines = tile->getNumHVLines();
      if( ElectInd >= nlines) {
         ATH_MSG_ERROR("Wrong line "<<ElectInd<<" for FCAL cell "<<id.get_identifier32().get_compact());
         return -1;
      } else {
         const FCALHVLine* line2 = tile->getHVLine(ElectInd);
         if(line2) {
	   return line2->hvLineNo(&hvIdMapping);
         } else {
	   ATH_MSG_ERROR("Do not have HVLine for "<<ElectInd<<" for FCAL cell "<<id.get_identifier32().get_compact());
	   return -1;
	 }
      }
    }
  }

  // should not get up to this point....
  return -1;

}
