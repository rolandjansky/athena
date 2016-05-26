/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArConditionsTest/LArCablingTest.h"

#include "LArCabling/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include <fstream>

LArCablingTest::LArCablingTest(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), m_larCablingSvc("LArCablingService"), m_print(true),
  m_onlineId(0),
  m_caloCellId(0)
{
  declareProperty("Print",m_print=true);
  declareProperty("TestMode",m_mode=0xF,
		  "1: On->Off, 2: Off->On 4: disconnected, 8: FEB->ROD");
}

LArCablingTest::~LArCablingTest() {
}

StatusCode LArCablingTest::initialize() {
  detStore()->retrieve(m_onlineId,"LArOnlineID").ignore();
  detStore()->retrieve(m_caloCellId,"CaloCell_ID").ignore();
  m_larCablingSvc.retrieve().ignore();

  return StatusCode::SUCCESS;
}



StatusCode LArCablingTest::execute() {

  std::ofstream outfile;
  if (m_print) {
    outfile.open("identifiers.txt");
    outfile << "hash id bec pn FT SL chan id calo pn sampl reg eta phi calib" << std::endl;
  }
  if (m_mode & 1) {
    std::vector<HWIdentifier>::const_iterator it1 = m_onlineId->channel_begin();
    std::vector<HWIdentifier>::const_iterator it1_e = m_onlineId->channel_end();
    for (;it1!=it1_e;it1++) {
      const HWIdentifier hwid = *it1;
      if (m_print)
	print(hwid,outfile);
      const Identifier id=m_larCablingSvc->cnvToIdentifier(hwid);
      if (m_print)
	if (id!=Identifier())
	  std::cout << "ON:OFF 0x" << hwid.get_identifier32().get_compact() << ":0x"<<id.get_identifier32().get_compact() << std::dec << std::endl;
    }
  }
  if (m_mode & 2) {
    std::vector<Identifier>::const_iterator it2=m_caloCellId->cell_begin();
    std::vector<Identifier>::const_iterator it2_e=m_caloCellId->cell_end();
    for(;it2!=it2_e;it2++) {
      const Identifier id=*it2;
      if (!m_caloCellId->is_tile(id)) {
	const HWIdentifier hwid=m_larCablingSvc->createSignalChannelID(id);
	if (m_print)
	  std::cout << "OFF:ON 0x" << std::hex << id.get_identifier32().get_compact() << ":0x"<<hwid.get_identifier32().get_compact() << std::dec << std::endl;
      }
    }
  }
  if (m_mode & 4) {
    //     std::vector<Identifier> ids(200000);
    std::vector<HWIdentifier>::const_iterator it1 = m_onlineId->channel_begin();
    std::vector<HWIdentifier>::const_iterator it1_e = m_onlineId->channel_end();
    size_t nId=0;
    for (;it1!=it1_e;it1++,nId++) {
      const HWIdentifier hwid = *it1;
      bool result=m_larCablingSvc->isOnlineConnected(hwid);
      if (m_print)
	std::cout << "Connected 0x" << std::hex << hwid.get_identifier32().get_compact() << ":"<<result << std::dec << std::endl;
    }
  }

  if (m_mode & 8) {
    std::vector<HWIdentifier>::const_iterator it3=m_onlineId->feb_begin();
    std::vector<HWIdentifier>::const_iterator it3_e=m_onlineId->feb_end();
    for(;it3!=it3_e;++it3) {
      const HWIdentifier FEBID=*it3;
#ifdef LARREADOUTMODULEID_H //Old version
      const uint32_t RODID=m_larCablingSvc->getReadoutModuleID(FEBID).id();
#else //New version, LArReadoutModuleID replaced my HWIdentifier
      const uint32_t RODID=m_larCablingSvc->getReadoutModuleID(FEBID).get_identifier32().get_compact();
#endif
      if (m_print) {
	std::cout << "FEB:ROD 0x" << FEBID.get_identifier32().get_compact() << ":0x" << 
	  RODID << std::dec << std::endl;
      }
    }//end loop over FEBs
  }
  if (m_print)
    outfile.close();
  m_print=false;
  return StatusCode::SUCCESS;
}



void LArCablingTest::print (const HWIdentifier& hwid, std::ostream& out) {
  const IdentifierHash hwid_hash=m_onlineId->channel_Hash(hwid);
  out << hwid_hash << " " << std::hex << "0x" << hwid.get_identifier32().get_compact() << std::dec << " " 
      << m_onlineId->barrel_ec(hwid) << " " 
      << m_onlineId->pos_neg(hwid) << " "
      << m_onlineId->feedthrough(hwid) << " " 
      << m_onlineId->slot(hwid) << " "
      << m_onlineId->channel(hwid) << " : ";
  if (m_larCablingSvc->isOnlineConnected(hwid)) {
    const Identifier id=m_larCablingSvc->cnvToIdentifier(hwid);
    out   << std::hex << "0x" << id.get_identifier32().get_compact() << std::dec << " " 
	  << m_caloCellId->sub_calo(id) << " "
	  << m_caloCellId->pos_neg(id) << " " 
	  << m_caloCellId->sampling(id) << " "
	  << m_caloCellId->region(id) << " "
	  << m_caloCellId->eta(id) << " "
	  << m_caloCellId->phi(id) << " ";
  }
  else
    out << " disconnected ";

  const std::vector<HWIdentifier>& calibIDs=m_larCablingSvc->calibSlotLine(hwid);
  for (size_t i=0;i<calibIDs.size();++i) {
    out << std::hex << "0x" << calibIDs[i].get_identifier32().get_compact() << " ";
  }
  out << std::dec << std::endl;
}

