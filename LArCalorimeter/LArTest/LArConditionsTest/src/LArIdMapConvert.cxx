/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArConditionsTest/LArIdMapConvert.h"
          
//#include "LArTools/LArCablingBlob.h"
//#include "LArTools/LArOnOffIdMap.h"
#include "LArIdentifier/LArOnlineID.h"
//#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include <fstream>

LArIdMapConvert::LArIdMapConvert( const std::string& name, 
				  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_onlineID(NULL),
  m_caloCellID(NULL)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

LArIdMapConvert::~LArIdMapConvert() 

{}

StatusCode LArIdMapConvert::initialize() {

  StatusCode sc = detStore()->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_caloCellID, "CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get CaloCell_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_CLKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode LArIdMapConvert::finalize()
{

  return StatusCode::SUCCESS;
}


StatusCode LArIdMapConvert::execute() {  

  const uint32_t onlHashMax=m_onlineID->channelHashMax();

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKey};
  const LArCalibLineMapping *clCont {*clHdl};
  if(!clCont){
     ATH_MSG_ERROR("Do not have calib mapping object " << m_CLKey.key() );
     return StatusCode::FAILURE;
  }

  coral::AttributeListSpecification* spec_onOff = new coral::AttributeListSpecification();
  spec_onOff->extend("OnlineHashToOfflineId", "blob");
  spec_onOff->extend<unsigned>("version");
  AthenaAttributeList* al_onOff = new AthenaAttributeList(*spec_onOff);
  coral::Blob& blobOnOff=(*al_onOff)["OnlineHashToOfflineId"].data<coral::Blob>();
  (*al_onOff)["version"].setValue(0U);
  blobOnOff.resize(onlHashMax*sizeof(uint32_t));

  spec_onOff->release();
  // cppcheck-suppress memleak
  spec_onOff = nullptr;

  coral::AttributeListSpecification* spec_calib = new coral::AttributeListSpecification();
  spec_calib->extend("OnlineHashToCalibIds", "blob");
  spec_calib->extend<unsigned>("version");
  AthenaAttributeList* al_calib = new AthenaAttributeList(*spec_calib);
  coral::Blob& blobCalib=(*al_calib)["OnlineHashToCalibIds"].data<coral::Blob>();
  (*al_calib)["version"].setValue(0U);
  blobCalib.resize(onlHashMax*sizeof(uint32_t)*3); //Bigger than necessary 

  spec_calib->release();
  // cppcheck-suppress memleak
  spec_calib = nullptr;

  uint32_t* pBlobOnOff=static_cast<uint32_t*>(blobOnOff.startingAddress());
  uint32_t* pBlobCalib=static_cast<uint32_t*>(blobCalib.startingAddress());

  size_t index=0;
  size_t calibIndex=0;

  std::vector<unsigned> calibHist(5,0);

  std::ofstream outfile("identifiers.txt");

  outfile << "hash id bec pn FT SL chan id calo pn sampl reg eta phi calib" << std::endl;

  for (uint32_t onlHash=0;onlHash<onlHashMax;++onlHash) {
    const HWIdentifier hwid=m_onlineID->channel_Id(onlHash);
    print(hwid,outfile,cabling,clCont);
    const Identifier id=cabling->cnvToIdentifierFromHash(onlHash);
    pBlobOnOff[index++]=id.get_identifier32().get_compact();

    const std::vector<HWIdentifier>& calibIDs=clCont->calibSlotLine(hwid);
    const size_t nCalibLines=calibIDs.size();
    (calibHist[nCalibLines])++;
    pBlobCalib[calibIndex++]=nCalibLines;
    for(uint32_t iCalib=0;iCalib<nCalibLines;++iCalib) 
      pBlobCalib[calibIndex++]=calibIDs[iCalib].get_identifier32().get_compact();
  } 

  blobCalib.resize(calibIndex*sizeof(uint32_t)); //Size down to actual size

  
  outfile.close();

  msg(MSG::INFO) << "BlobSize OnOffId:" << index << endmsg;
  msg(MSG::INFO) << "BlobSize CalibId:" << calibIndex << endmsg;
  msg(MSG::INFO) << "nCalib[i] ";
  for (unsigned j=0;j<5;++j) 
    msg() << calibHist[j] << "/";
  msg() << endmsg;

  StatusCode sc=detStore()->record(al_onOff,"/LAR/Identifier/OnOffIdMap");
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Failed to record LArCablingMap" << endmsg;
    return sc;
  }

  sc=detStore()->record(al_calib,"/LAR/Identifier/CalibIdMap");
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Failed to record LArCablingMap" << endmsg;
    return sc;
  }
  

  return StatusCode::SUCCESS;
}
    
  
void LArIdMapConvert::print (const HWIdentifier& hwid, std::ostream& out, const LArOnOffIdMapping* cabling, const LArCalibLineMapping *clCont) {
  const IdentifierHash hwid_hash=m_onlineID->channel_Hash(hwid);
  out << hwid_hash << " " << std::hex << "0x" << hwid.get_identifier32().get_compact() << std::dec << " " 
      << m_onlineID->barrel_ec(hwid) << " " 
      << m_onlineID->pos_neg(hwid) << " "
      << m_onlineID->feedthrough(hwid) << " " 
      << m_onlineID->slot(hwid) << " "
      << m_onlineID->channel(hwid) << " : ";
  if (cabling->isOnlineConnected(hwid)) {
    const Identifier id=cabling->cnvToIdentifier(hwid);
    out   << std::hex << "0x" << id.get_identifier32().get_compact() << std::dec << " " 
	  << m_caloCellID->sub_calo(id) << " "
	  << m_caloCellID->pos_neg(id) << " " 
	  << m_caloCellID->sampling(id) << " "
	  << m_caloCellID->region(id) << " "
	  << m_caloCellID->eta(id) << " "
	  << m_caloCellID->phi(id) << " ";
  }
  else
    out << " disconnected ";

  const std::vector<HWIdentifier>& calibIDs=clCont->calibSlotLine(hwid);
  for (size_t i=0;i<calibIDs.size();++i) {
    out << std::hex << "0x" << calibIDs[i].get_identifier32().get_compact() << " ";
  }
  out << std::dec << std::endl;
}


/*
void LArIdMapConvert::writeElement(uint8_t* dest, const HWIdentifier& onId, const Identifier& offId) {
  
  

  dest[0]= (src.pn==-1 ? 0 : 0x80) | (src.det & 0x7F);
  dest[1]= src.region;
  dest[2]= src.sample;
  static_cast<uint16_t*>(dest[3])=src.eta;
  static_cast<uint16_t*>(dest[5])=src.phi;
  dest[7]= src.ft_num;
  dest[8]= src.ft_slot;
  dest[9]= src.feb_chan;
  dest[10]= src.calib_slot;
  dest[11]= src.calib_line;

  static_cast<uint32_t>dest dest[12]= 
   
}
*/
