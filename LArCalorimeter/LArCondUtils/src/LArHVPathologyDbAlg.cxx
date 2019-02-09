/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "LArCabling/LArCablingLegacyService.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"

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
  , m_cablingService("LArCablingLegacyService")
  , m_larem_id(0)
  , m_larhec_id(0)
  , m_larfcal_id(0)
  , m_laronline_id(0)
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
  msg(MSG::INFO) << " in initialize()" << endmsg;

  if(m_folder.value().empty()) {
    msg(MSG::ERROR) << "Folder property not set. Exiting ... " << endmsg;
    return StatusCode::FAILURE;
  }

  // Get HVPathology tool
  StatusCode sc = m_pathologyTool.retrieve();
  if(!sc.isSuccess()) {
    msg(MSG::ERROR) << "Unable to initialize LArHVPathologyDbTool" << endmsg;
    return sc;
  }
  else {
    msg(MSG::INFO) << "Retrieved LArHVPathologyDbTool"  << endmsg;
  }

  // Get the IOVRegistrationSvc when needed
  if(m_writeCondObjs) {
    sc = m_regSvc.retrieve();
    if(!sc.isSuccess()) {
      msg(MSG::ERROR) << "Unable to find IOVRegistrationSvc" << endmsg;
      m_writeCondObjs = false;
      return sc;
    }  
    else {
      msg(MSG::INFO) << "Retrieved IOVRegistrationSvc "  << endmsg;
    }
  }

// retrieve LArEM id helpers

  sc = detStore()->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
   msg(MSG::ERROR) << "Unable to retrieve CaloIdMgr " << endmsg;
   return sc;
  }

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

//  retrieve CaloDetDescrMgr 
  sc = detStore()->retrieve(m_calodetdescrmgr);
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Unable to retrieve CaloDetDescrMgr " << endmsg;
     return sc;
  }

  sc = detStore()->retrieve(m_laronline_id,"LArOnlineID");
  if (sc.isFailure()) {
     msg(MSG::ERROR) << "Unable to retrieve  LArOnlineID from DetectorStore" 
          << endmsg;
     return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode LArHVPathologyDbAlg::execute()
{
  msg(MSG::INFO) <<" in execute()" <<endmsg;

  int nevt = getContext().eventID().event_number();

  if(m_writeCondObjs && nevt==1) {
    msg(MSG::INFO) << "Creating conditions objects" << endmsg;

    // Create cond objects
    if(!createCondObjects().isSuccess()) {
      msg(MSG::ERROR) << "Could not create cond objects " << endmsg;
      m_writeCondObjs = false;
      return StatusCode::FAILURE;
    }
  }

  // Dump cond objects
  StatusCode sc = printCondObjects();
  if(!sc.isSuccess()) {
    msg(MSG::ERROR) << "Could not print out cond objects" << endmsg;
    return sc;
  }

  return sc;
}

StatusCode LArHVPathologyDbAlg::stop()
{
  msg(MSG::INFO) <<" in stop()" <<endmsg;
  StatusCode sc= StatusCode::SUCCESS;
 
  if(m_writeCondObjs) {
    sc = registerCondObjects();
    if(!sc.isSuccess()) {
      msg(MSG::ERROR) << "Could not register objects" << endmsg;
      return sc;
    } 
    else {
      msg(MSG::INFO) << "Register OK" << endmsg;
    }
  }

  return sc;
}

StatusCode LArHVPathologyDbAlg::createCondObjects()
{
  msg(MSG::INFO) <<" in createCondObjects() " <<endmsg;

  if(detStore()->contains<AthenaAttributeList>(m_folder)) {
    msg(MSG::INFO) << "EMB Pathologies already in SG, skipping " <<endmsg;
  }
  else {
    // Read input file and construct LArHVPathologiesDb for given folder
    std::ifstream infile;
    infile.open(m_inpFile.value().c_str());

    if(!infile.is_open()) {
      msg(MSG::ERROR) << "Unable to open " << m_inpFile << " for reading" << endmsg;
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
      msg(MSG::ERROR) << "Unable to find data for the folder " << m_folder 
		      << " in the input file" << endmsg;
      return StatusCode::FAILURE;
    }
    else
      msg(MSG::INFO) << "Found folder " << foldername << " in the input file" << endmsg;
      
    // Get data corresponding to the folder and put it into LArHVPathologiesDb object
    LArHVPathologiesDb pathologies;

    msg(MSG::INFO) << " start reading input file " << endmsg;
    while(!infile.eof()) {
      // Number or string?
      checkChar = static_cast<char> (infile.get());
      msg(MSG::INFO) << " checChar " << checkChar << endmsg;
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
          msg(MSG::INFO) << " read " << bec << " " << pos_neg << " " << FT << " " << slot << " " << channel << " " << hvModule << " " << hvLine << " " << pathologyType << endmsg;
          HWIdentifier hwid = m_laronline_id->channel_Id(bec,pos_neg,FT,slot,channel);
          Identifier id = m_cablingService->cnvToIdentifier( hwid);
          cellID = (unsigned int)(id.get_identifier32().get_compact());
          elecList=getElectInd(id,hvModule,hvLine);
          msg(MSG::INFO) << " cellId , elecList size " << cellID << " " << elecList.size() << endmsg;
        }
        for (unsigned int i=0;i<elecList.size();i++) {
	 LArHVPathologiesDb::LArHVElectPathologyDb electPath;
	 electPath.cellID = cellID;
	 electPath.electInd = elecList[i];
         if(m_mode==2) electPath.pathologyType = ((pathologyType&0x0FFF)<<4);
         else electPath.pathologyType = pathologyType;
	 pathologies.m_v.push_back(electPath);
	 msg(MSG::INFO) << "Created electrode pathology (" << cellID
		       << "," << elecList[i]
		       << "," << pathologyType << ")" << endmsg;
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
    msg(MSG::INFO) << "Finished parsing input file" << endmsg;    

    AthenaAttributeList* attrlist = m_pathologyTool->hvPathology2AttrList(pathologies);
    msg(MSG::INFO) << "Created Attribute List" << endmsg;

    StatusCode sc = detStore()->record(attrlist,m_folder);
    if(!sc.isSuccess()) {
      msg(MSG::ERROR) << "Could not record " << m_folder << endmsg;
      return sc;
    }
    else
      msg(MSG::INFO) << "Recorded " << m_folder << endmsg;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode LArHVPathologyDbAlg::printCondObjects()
{
  msg(MSG::INFO) <<" in printCondObjects() " <<endmsg;
  std::ofstream *fout=0;
  const AthenaAttributeList* attrlist;
  StatusCode sc = detStore()->retrieve(attrlist,m_folder);

  if(sc.isFailure())
    msg(MSG::WARNING) << "Could not find object for " << m_folder << endmsg;
  else {
    LArHVPathologiesDb* pathologyContainer = m_pathologyTool->attrList2HvPathology(*attrlist);
    msg(MSG::INFO) << "Unpacked pathologies from Attribute List for " << m_folder << endmsg;
    if(m_outFile.value().size()>0) {
       fout = new std::ofstream(m_outFile.value().c_str());
       if((!fout) || (fout && !(fout->good()))) {
             msg(MSG::WARNING) << "Could not open output file: " << m_outFile.value() << endmsg;
             fout=0;
             }
       if(fout) *fout<<m_folder.value()<<std::endl;
    }
    for(unsigned i=0; i<pathologyContainer->m_v.size(); ++i) {
      LArHVPathologiesDb::LArHVElectPathologyDb electPath = pathologyContainer->m_v[i];
      if(m_mode==0) {
         msg(MSG::INFO) << "Got pathology for cell ID: " << electPath.cellID
      	     << "(" << electPath.electInd 
      	     << "," << electPath.pathologyType << ") " << endmsg;
         if(fout) *fout<<electPath.cellID<<"\t"<<electPath.electInd<<"\t"<<electPath.pathologyType<<std::endl;    
      } else {
         msg(MSG::INFO) << "Got pathology for cell ID: " << electPath.cellID << endmsg;
         HWIdentifier hwid = m_cablingService->createSignalChannelID(Identifier32(electPath.cellID));
         int HVLine=getHVline(Identifier(electPath.cellID),electPath.electInd);
         if(HVLine<0) {
            msg(MSG::ERROR) << "No HVline for cell "<<electPath.cellID<< endmsg;
         } else {
            int hvmodule=HVLine/1000;
            int hvline=HVLine%1000;
            if(m_mode==1) {
              msg(MSG::INFO) << m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << electPath.pathologyType << endmsg;
              if(fout) *fout << m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << electPath.pathologyType << std::endl;
            } else if (m_mode==2){
              msg(MSG::INFO) << m_laronline_id->barrel_ec(hwid) << " " << m_laronline_id->pos_neg(hwid) << " " << m_laronline_id->feedthrough(hwid) << " " << m_laronline_id->slot(hwid) << " " << m_laronline_id->channel(hwid) << " " << hvmodule << " " << hvline << " " << ((electPath.pathologyType&0xFF0)>>4) << endmsg;
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
  msg(MSG::INFO) << "entering registerCondObject()"  << endmsg;

  std::string objname("AthenaAttributeList");

  StatusCode sc = m_regSvc->registerIOV(objname, m_folder, m_outpTag);
  if(!sc.isSuccess()) 
    msg(MSG::ERROR) << "Could not register (" << objname << ", " << m_outpTag << ") in IOV DB " << endmsg;
  else
    msg(MSG::INFO) << "Successfully registered" << endmsg;

  return sc;
}
 
std::vector<unsigned int> LArHVPathologyDbAlg::getElectInd(const Identifier & id,unsigned int module, unsigned int line)
{

  std::vector<unsigned int> list;
  list.clear();
  int HVline = 1000*module + line;
// EM calo
  if (m_larem_id->is_lar_em(id)) {
// LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id) > 0)  {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         for (unsigned int i=0;i<nelec;i++) {
            const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
            for (unsigned int igap=0;igap<2;igap++) {
              if (electrode->hvLineNo(igap)==HVline) {
                  list.push_back(2*i+igap);
              }
            } 
         }
       }
     }
// LAr EMEC
     if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0) {
       if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         for (unsigned int i=0;i<nelec;i++) {
            const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
            for (unsigned int igap=0;igap<2;igap++) {
              if (electrode->hvLineNo(igap)==HVline) {
                  list.push_back(2*i+igap);
              }       
            }       
         }    
       }
     }
// EMBPS
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id)==0) {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
        const EMBCellConstLink cell = embElement->getEMBCell();
        const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule ();
        for (unsigned int igap=0;igap<2;igap++) {
           if (hvmodule->hvLineNo(igap)==HVline) {
             list.push_back(igap);
           }
        }
       }
     }
// EMECPS
    if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id)==0) {
      if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
       for (unsigned int igap=0;igap<2;igap++) {
        if (hvmodule->hvLineNo(igap)==HVline) {
          list.push_back(igap);
        }
       }
      }
    }
  }
//HEC
  if (m_larhec_id->is_lar_hec(id)) {
    if (const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      for (unsigned int i=0;i<nsubgaps;i++) {
          const HECHVSubgapConstLink subgap = cell->getSubgap(i);
          if (subgap->hvLineNo()==HVline) {
            list.push_back(i);
          }
      }
    }
  }
//FCAL
  if (m_larfcal_id->is_lar_fcal(id)) {
    if (const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const FCALTile* tile = fcalElement->getFCALTile();
       unsigned int nlines = tile->getNumHVLines();
       for (unsigned int i=0;i<nlines;i++) {
         const FCALHVLineConstLink line2 = tile->getHVLine(i);
         if (line2) {
            if (line2->hvLineNo()==HVline) {
               list.push_back(i);
            }
         }
       }
    }
  }

  return list;

}

int LArHVPathologyDbAlg::getHVline(const Identifier & id, short unsigned int ElectInd)
{

  unsigned int igap, ielec;
// EM calo
  if (m_larem_id->is_lar_em(id)) {
// LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id) > 0)  {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         igap = ElectInd % 2;
         ielec = std::div(ElectInd - igap, 2).quot;
         if (ielec > nelec) {
            msg(MSG::ERROR) << "Wrong electrode number " << ielec << " for cell "<< id.get_identifier32().get_compact() <<endmsg;
            return -1;
         } else { 
            return cell->getElectrode(ielec)->hvLineNo(igap);
         }
       }
     }
// LAr EMEC
     if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0) {
       if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         igap = ElectInd % 2;
         ielec = std::div(ElectInd - igap, 2).quot;
         if (ielec > nelec) {
            msg(MSG::ERROR) << "Wrong electrode number " << ielec << " for cell "<< id.get_identifier32().get_compact() <<endmsg;
            return -1;
         } else { 
            return cell->getElectrode(ielec)->hvLineNo(igap);
         }
       }
     }
// EMBPS
     if (abs(m_larem_id->barrel_ec(id))==1 &&  m_larem_id->sampling(id)==0) {
       if (const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
        const EMBCellConstLink cell = embElement->getEMBCell();
        const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule ();
        if(ElectInd >= 2) {
            msg(MSG::ERROR) << "Wrong igap "<<ElectInd<<" for EMBPS cell "<<id.get_identifier32().get_compact() <<endmsg;
            return -1;
        } else {
            return hvmodule->hvLineNo(ElectInd);
        }
       }
     }
// EMECPS
    if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id)==0) {
      if (const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const EMECCellConstLink cell = emecElement->getEMECCell();
       const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
        if(ElectInd >= 2) {
            msg(MSG::ERROR) << "Wrong igap "<<ElectInd<<" for EMECPS cell "<<id.get_identifier32().get_compact() <<endmsg;
            return -1;
        } else {
            return hvmodule->hvLineNo(ElectInd);
        }
      }
    }
  }
//HEC
  if (m_larhec_id->is_lar_hec(id)) {
    if (const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
      const HECCellConstLink cell = hecElement->getHECCell();
      unsigned int nsubgaps = cell->getNumSubgaps();
      if( ElectInd >= nsubgaps) {
         msg(MSG::ERROR) << "Wrong igap "<<ElectInd<<" for HEC cell "<<id.get_identifier32().get_compact() <<endmsg;
         return -1;
      } else {
         return cell->getSubgap(ElectInd)->hvLineNo();
      }
    }
  }
//FCAL
  if (m_larfcal_id->is_lar_fcal(id)) {
    if (const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id))) {
       const FCALTile* tile = fcalElement->getFCALTile();
       unsigned int nlines = tile->getNumHVLines();
      if( ElectInd >= nlines) {
         msg(MSG::ERROR) << "Wrong line "<<ElectInd<<" for FCAL cell "<<id.get_identifier32().get_compact() <<endmsg;
         return -1;
      } else {
         const FCALHVLineConstLink line2 = tile->getHVLine(ElectInd);
         if(line2) {
            return line2->hvLineNo();
         } else {
            msg(MSG::ERROR) << "Do not have HVLine for "<<ElectInd<<" for FCAL cell "<<id.get_identifier32().get_compact() <<endmsg;
            return -1;
         }
      }
    }
  }

  // should not get up to this point....
  return -1;

}
