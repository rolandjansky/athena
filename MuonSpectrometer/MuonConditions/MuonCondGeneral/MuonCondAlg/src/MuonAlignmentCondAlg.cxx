/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/MuonAlignmentCondAlg.h"

#include "MuonCondSvc/MdtStringUtils.h"
#include "GaudiKernel/ConcurrencyFlags.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <string>
#include <map>

MuonAlignmentCondAlg::MuonAlignmentCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_condSvc{"CondSvc", name}
{
  m_geometryVersion = "";
  m_AsBuiltRequested = false;
  m_ILineRequested = false;

  m_parlineFolder.clear();
  declareProperty("DumpALines",      m_dumpALines=false);
  declareProperty("DumpBLines",      m_dumpBLines=false);
  declareProperty("DumpILines",      m_dumpILines=false);
  declareProperty("ILinesFromCondDB",m_ILinesFromDb=false);
  declareProperty("ALinesFile",      m_aLinesFile="");
  declareProperty("AsBuiltFile",     m_asBuiltFile="");
}

StatusCode MuonAlignmentCondAlg::initialize(){

  ATH_MSG_INFO("Initilalizing");
  ATH_MSG_INFO("In initialize ---- # of folders registered is " << m_parlineFolder.size());

  ATH_CHECK(m_condSvc.retrieve());

  // =======================
  // Loop on folders requested in configuration and check if /MUONALIGN/CSC/ILINES and /MUONALIGN/MDT/ASBUILTPARAMS are requested
  // =======================
  for (const std::string& currentFolderName : m_parlineFolder) {
    if(currentFolderName.find("ILINES") != std::string::npos) m_ILineRequested = true;
    if(currentFolderName.find("ASBUILTPARAMS") != std::string::npos) m_AsBuiltRequested = true;
  }

  // Read Handles Keys
  ATH_CHECK(m_readMdtBarrelKey.initialize());
  ATH_CHECK(m_readMdtEndcapSideAKey.initialize());
  ATH_CHECK(m_readMdtEndcapSideCKey.initialize());
  ATH_CHECK(m_readTgcSideAKey.initialize());
  ATH_CHECK(m_readTgcSideCKey.initialize());
  ATH_CHECK(m_readCscILinesKey.initialize(m_ILinesFromDb and m_ILineRequested));
  ATH_CHECK(m_readMdtAsBuiltParamsKey.initialize(m_AsBuiltRequested));

  // Write Handles
  ATH_CHECK(m_writeALineKey.initialize());
  if(m_condSvc->regHandle(this, m_writeALineKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeALineKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_writeBLineKey.initialize());
  if(m_condSvc->regHandle(this, m_writeBLineKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeBLineKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_writeILineKey.initialize());
  if(m_condSvc->regHandle(this, m_writeILineKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeILineKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_writeAsBuiltKey.initialize());
  if(m_condSvc->regHandle(this, m_writeAsBuiltKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeAsBuiltKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK(detStore()->retrieve(m_muonDetMgrDS));
  m_geometryVersion = m_muonDetMgrDS->geometryVersion();
  ATH_MSG_INFO("geometry version from the MuonDetectorManager = " << m_geometryVersion);
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentCondAlg::execute(){
  ATH_MSG_DEBUG( "execute " << name() ); 
  ATH_CHECK(loadParameters());
  return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentCondAlg::loadParameters() {

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG( "In LoadParameters " << name() );

  // =======================
  // Load ILine parameters if requested in the joboptions and /MUONALIGN/CSC/ILINES folder given in the joboptions
  // =======================
  if( m_ILinesFromDb and m_ILineRequested) {
    sc = loadAlignILines("/MUONALIGN/CSC/ILINES");
  } 
  else if (m_ILineRequested and not m_ILinesFromDb) {
    ATH_MSG_INFO("DB folder for I-Lines given in job options however loading disabled ");
    sc = StatusCode::SUCCESS;
  }
  if(sc.isFailure()) return StatusCode::FAILURE;

  // =======================
  // Load AsBuilt parameters if /MUONALIGN/MDT/ASBUILTPARAMS folder given in the joboptions
  // =======================
  if(m_AsBuiltRequested) sc = loadAlignAsBuilt("/MUONALIGN/MDT/ASBUILTPARAMS");
  if(sc.isFailure()) return StatusCode::FAILURE;

  // =======================
  // Load A- and B-Lines
  // =======================
  sc = loadAlignABLines();
  if(sc.isFailure()) return StatusCode::FAILURE;

  return sc;
}

StatusCode MuonAlignmentCondAlg::loadAlignABLines() {

  // =======================
  // Write ALine Cond Handle
  // =======================
  SG::WriteCondHandle<ALineMapContainer> writeALineHandle{m_writeALineKey};
  if (writeALineHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeALineHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<ALineMapContainer> writeALineCdo{std::make_unique<ALineMapContainer>()};

  // =======================
  // Write BLine Cond Handle
  // =======================
  SG::WriteCondHandle<BLineMapContainer> writeBLineHandle{m_writeBLineKey};
  if (writeBLineHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeBLineHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<BLineMapContainer> writeBLineCdo{std::make_unique<BLineMapContainer>()};


  // =======================
  // Loop on folders requested in configuration and process the ones that have A- and B-lines
  // =======================

  EventIDRange rangeALineW;
  EventIDRange rangeBLineW;
  for (const std::string& currentFolderName : m_parlineFolder) {
    if(currentFolderName.find("ILINES") != std::string::npos) continue;
    if(currentFolderName.find("ASBUILTPARAMS") != std::string::npos) continue;
    // Process the current folder for the A- and B-lines
    if(loadAlignABLines(currentFolderName, 
			writeALineCdo.get(), 
			writeBLineCdo.get(),
			rangeALineW,
			rangeBLineW).isSuccess()) {
      ATH_MSG_INFO("A- and B-Lines parameters from DB folder <" << currentFolderName << "> loaded");
    } else {
      ATH_MSG_ERROR("A- and B-Lines parameters from DB folder <" << currentFolderName << "> failed to load");
      return StatusCode::FAILURE; 
    }
  }

  // currently, the RPC/TGCRecRoiSvc are services and thus do not participate the scheduling of Read/Write(Cond)Handle(Key)
  // thus, RPC/TGCRecRoiSvc are not able to use the MuonDetectorCondAlg. To have aligned Roi, here the nominal MuonDetectorManager 
  // (in the detector store) is updated which is *not* thread-safe. These lines have to be removed as soon as the trigger
  // group has decided on how to migrate the RPC/TGCRecRoiSvc to MT. Now, only running for serial trigger jobs.
  if (m_doRecRoiSvcUpdate && Gaudi::Concurrency::ConcurrencyFlags::numThreads()==1) {
    if (const_cast<MuonGM::MuonDetectorManager*>(m_muonDetMgrDS)->updateAlignment(*writeALineCdo).isFailure()) ATH_MSG_ERROR("Unable to updateAlignment" );
    else ATH_MSG_DEBUG("updateAlignment DONE" );
  }

  if (writeALineHandle.record(rangeALineW, std::move(writeALineCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record ALineMapContainer " << writeALineHandle.key() 
		  << " with EventRange " << rangeALineW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeALineHandle.key() << " with range " << rangeALineW << " into Conditions Store");

  if (writeBLineHandle.record(rangeBLineW, std::move(writeBLineCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record BLineMapContainer " << writeBLineHandle.key() 
		  << " with EventRange " << rangeBLineW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeBLineHandle.key() << " with range " << rangeBLineW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentCondAlg::loadAlignABLines(std::string folderName,
						ALineMapContainer* writeALineCdo,
						BLineMapContainer* writeBLineCdo,
						EventIDRange& rangeALineW,
						EventIDRange& rangeBLineW) {

  ATH_MSG_INFO("Load alignment parameters from DB folder <" << folderName << ">");

  bool hasBLine = true;
  if (folderName.find("TGC")!=std::string::npos)  {
    hasBLine = false;
    ATH_MSG_INFO("No BLines decoding will be attempted for folder named " << folderName);
  }
   
  // =======================
  // Read Cond Handles and ranges for current folder
  // =======================
  EventIDRange rangeALinesTemp;
  EventIDRange rangeBLinesTemp;
  const CondAttrListCollection* readCdo; 
  if (folderName == "/MUONALIGN/MDT/BARREL") {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readMdtBarrelKey};
    readCdo = *readHandle; 
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    if ( !readHandle.range(rangeALinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve ALines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    if ( !readHandle.range(rangeBLinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve BLines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    
    ATH_MSG_INFO("Size of /MUONALIGN/MDT/BARREL CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of /MUONALIGN/MDT/BARREL input is, ALines: " << rangeALinesTemp << " BLines: " << rangeBLinesTemp);

  } else if (folderName == "/MUONALIGN/MDT/ENDCAP/SIDEA") {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readMdtEndcapSideAKey};
    readCdo = *readHandle; 
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    if ( !readHandle.range(rangeALinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve ALines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    if ( !readHandle.range(rangeBLinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve BLines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    
    ATH_MSG_INFO("Size of /MUONALIGN/MDT/ENDCAP/SIDEA CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of /MUONALIGN/MDT/ENDCAP/SIDEA input is, ALines: " << rangeALinesTemp << " BLines: " << rangeBLinesTemp);

  } else if (folderName == "/MUONALIGN/MDT/ENDCAP/SIDEC") {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readMdtEndcapSideCKey};
    readCdo = *readHandle; 
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    if ( !readHandle.range(rangeALinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve ALines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    if ( !readHandle.range(rangeBLinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve BLines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    
    ATH_MSG_INFO("Size of /MUONALIGN/MDT/ENDCAP/SIDEC CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of /MUONALIGN/MDT/ENDCAP/SIDEC input is, ALines: " << rangeALinesTemp << " BLines: " << rangeBLinesTemp);

  } else if (folderName == "/MUONALIGN/TGC/SIDEA") {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readTgcSideAKey};
    readCdo = *readHandle; 
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    if ( !readHandle.range(rangeALinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve ALines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 

    // !!!!!!!! NO BLINES FOR TGCs !!!!!!!!!!!!!!
    
    ATH_MSG_INFO("Size of /MUONALIGN/TGC/SIDEA CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of /MUONALIGN/TGC/SIDEA input is, ALines: " << rangeALinesTemp);

  } else if (folderName == "/MUONALIGN/TGC/SIDEC") {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readTgcSideCKey};
    readCdo = *readHandle; 
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read ALINES conditions object");
      return StatusCode::FAILURE; 
    } 
    
    if ( !readHandle.range(rangeALinesTemp) ) {
      ATH_MSG_ERROR("Failed to retrieve ALines validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    } 
    // !!!!!!!! NO BLINES FOR TGCs !!!!!!!!!!!!!!
    
    ATH_MSG_INFO("Size of /MUONALIGN/TGC/SIDEC CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of /MUONALIGN/TGC/SIDEC input is, ALines: " << rangeALinesTemp);

  }

  // Combine the validity ranges for ALines
  EventIDRange rangeIntersection{EventIDRange::intersect(rangeALineW, rangeALinesTemp)};     
  ATH_MSG_DEBUG("rangeIntersection: " << rangeIntersection << " Previous range: " << rangeALineW << " Current range : " << rangeALinesTemp);
  if (rangeIntersection.stop().isValid() and rangeIntersection.start()>rangeIntersection.stop()) {       
    ATH_MSG_FATAL("Invalid intersection range: " << rangeIntersection);       
    return StatusCode::FAILURE;     }     
  rangeALineW = rangeIntersection;

  // Combine the validity ranges for BLines
  if (hasBLine) {
    EventIDRange rangeIntersection{EventIDRange::intersect(rangeBLineW, rangeBLinesTemp)};     
    ATH_MSG_DEBUG("rangeIntersection: " << rangeIntersection << " Previous range: " << rangeALineW << " Current range: " << rangeALinesTemp);
    if (rangeIntersection.stop().isValid() and rangeIntersection.start()>rangeIntersection.stop()) {       
      ATH_MSG_FATAL("Invalid intersection range: " << rangeIntersection);       
      return StatusCode::FAILURE;     }     
    rangeBLineW = rangeIntersection;  
  }

  // =======================
  // retreive the collection of strings read out from the DB
  // =======================

  // unpack the strings in the collection and update the 
  // ALlineContainer in TDS
  int nLines = 0;
  int nDecodedLines = 0;
  int nNewDecodedALines = 0;
  int nNewDecodedBLines = 0;
  CondAttrListCollection::const_iterator itr;
  for (itr = readCdo->begin(); itr != readCdo->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    ATH_MSG_DEBUG("Data load is " << data << " FINISHED HERE ");
  
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string since_str;
    std::string till_str;
    std::string delimiter = "\n";
      
    
    std::vector<std::string> lines;
    MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
    for (std::string blobline : lines) {
      ++nLines;

      std::string delimiter = ":";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      //Check if tokens is not empty
      if (tokens.empty()) {
	ATH_MSG_FATAL("Empty string retrieved from DB in folder " << folderName);
	return  StatusCode::FAILURE;
      }
      type = tokens[0];
      //Parse line
      if (type.find("#")==0) {
	//skip it
	continue;
      }

      if (type.find("Header")==0) {
	std::string delimiter = "|";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	since_str = tokens[1];
	till_str = tokens[2];
      }
	
      if (type.find("IOV")==0) {
	std::string delimiter = " ";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	int ival = 1;
	long int iovThisBlob=0;
	
	std::string str_iovThisBlob = tokens[ival];
	sscanf(str_iovThisBlob.c_str(),"%80ld",&iovThisBlob);
	ATH_MSG_INFO("Data read from folder " << folderName <<
		     " have IoV = " << iovThisBlob);
      }
	
      if (type.find("Corr")==0) {
//#: Corr line is counter typ,  jff,  jzz, job,                         * Chamber information 
//#:                       svalue,  zvalue, tvalue,  tsv,  tzv,  ttv,   * A lines 
//#:                       bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en   * B lines 
//#:                       chamber                                      * Chamber name 
//.... example
//Corr: EMS  4   1  0     2.260     3.461    28.639 -0.002402 -0.002013  0.000482    -0.006    -0.013 -0.006000  0.000000  0.000000     0.026    -0.353  0.000000  0.070000  0.012000    -0.012    EMS1A08

	std::string delimiter = " ";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);

	//Check if tokens has the right length
	// if (tokens.size() != 12 and tokens.size() != 23) {
	if (tokens.size() != 25) {
	  ATH_MSG_FATAL("Invalid length in string retrieved from DB in folder " << folderName << " String length is " << tokens.size());
	  return  StatusCode::FAILURE;
	}

	ATH_MSG_VERBOSE("Parsing Line = ");
	for (std::string token : tokens) ATH_MSG_VERBOSE( token << " | ");
	ATH_MSG_VERBOSE( " " );

	bool thisRowHasBLine = true;
	if (tokens.size()<15) {
	  // only A-lines ..... old COOL blob convention for barrel 
	  thisRowHasBLine = false;
	  ATH_MSG_VERBOSE("(old COOL blob convention for barrel) skipping B-line decoding ");
	}

	// Start parsing
	int ival=1;
	
	// Station Component identification 
	int jff; 
	int jzz;
	int job;
	std::string stationType = tokens[ival++];	  
	std::string jff_str = tokens[ival++]; 
	sscanf(jff_str.c_str(),"%80d",&jff);
	std::string jzz_str = tokens[ival++];
	sscanf(jzz_str.c_str(),"%80d",&jzz);
	std::string job_str = tokens[ival++];
	sscanf(job_str.c_str(),"%80d",&job);
	
	// A-line
	float s;
	float z;
	float t;
	float ths;
	float thz;
	float tht;	  
	std::string s_str = tokens[ival++];
	sscanf(s_str.c_str(),"%80f",&s);
	std::string z_str = tokens[ival++];
	sscanf(z_str.c_str(),"%80f",&z);
	std::string t_str = tokens[ival++];
	sscanf(t_str.c_str(),"%80f",&t);
	std::string ths_str = tokens[ival++];
	sscanf(ths_str.c_str(),"%80f",&ths);
	std::string thz_str = tokens[ival++];
	sscanf(thz_str.c_str(),"%80f",&thz);
	std::string tht_str = tokens[ival++];
	sscanf(tht_str.c_str(),"%80f",&tht);

	// B-line
	float bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en;
	std::string ChamberHwName="";
	  
	if (hasBLine && thisRowHasBLine) {	      
	  std::string tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bz);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bp);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&bn);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&sp);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&sn);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&tw);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&pg);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&tr);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&eg);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&ep);
	  tmp_str = tokens[ival++];
	  sscanf(tmp_str.c_str(),"%80f",&en);	  

	  // ChamberName (hardware convention)
	  ChamberHwName = tokens[ival++];
	}
	  
	ATH_MSG_VERBOSE("Station type "  << stationType);
	ATH_MSG_VERBOSE("jff / jzz / job "  << jff << " / " << jzz << " / " << job);
	if (hasBLine) {
	  ATH_MSG_VERBOSE(" HardwareChamberName " << ChamberHwName);
	}
	else ATH_MSG_VERBOSE(" ");
	ATH_MSG_VERBOSE("A-line: s,z,t "  << s << " " << z << " " << t);
	ATH_MSG_VERBOSE(" ts,tz,tt "  << ths << " " << thz << " " << tht);
	if (hasBLine) {
	  if (thisRowHasBLine) {
	    ATH_MSG_VERBOSE("B-line:  bz,bp,bn " << bz << " " << bp << " " << bn);
	    ATH_MSG_VERBOSE(" sp,sn " << sp << " " << sn << " tw,pg,tr " << tw << " " << pg << " " << tr
			    << " eg,ep,en " << eg << " " << ep << " " << en);
	  }
	  else ATH_MSG_VERBOSE("No B-line found");
	} 
	  
	int stationName = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationType);
	Identifier id;
	ATH_MSG_VERBOSE("stationName  " << stationName);
	// if (stationType.substr(0,1)=="T") {
	if (stationType.at(0) == 'T') {
	  // tgc case
	  int stPhi = MuonGM::stationPhiTGC(stationType, jff, jzz, m_geometryVersion); // !!!!! The stationPhiTGC implementation in this package is NOT used !!!!!
	  int stEta = 1;
	  if (jzz<0) stEta = -1;			 
	  if (job != 0) {
	    // this should become the default now 
	    stEta=job;
	    if (jzz<0) stEta = -stEta;
	  }
	  id = m_idHelperSvc->tgcIdHelper().elementID(stationName, stEta, stPhi);
	  ATH_MSG_VERBOSE("identifier being assigned is " << m_idHelperSvc->tgcIdHelper().show_to_string(id));
	}
	else if (stationType.substr(0,1)=="C") {
	  // csc case
	  id = m_idHelperSvc->cscIdHelper().elementID(stationName, jzz, jff);
	  ATH_MSG_VERBOSE("identifier being assigned is " << m_idHelperSvc->cscIdHelper().show_to_string(id));
	}
	else if (stationType.substr(0,3)=="BML" && abs(jzz)==7) {
	  // rpc case
	  id = m_idHelperSvc->rpcIdHelper().elementID(stationName, jzz, jff, 1);
	  ATH_MSG_VERBOSE("identifier being assigned is " << m_idHelperSvc->rpcIdHelper().show_to_string(id));
	}
	else {
	  id = m_idHelperSvc->mdtIdHelper().elementID(stationName, jzz, jff);
	  ATH_MSG_VERBOSE("identifier being assigned is " << m_idHelperSvc->mdtIdHelper().show_to_string(id));
	}
          

	// new Aline
	++nDecodedLines;
	++nNewDecodedALines;
	ALinePar newALine;
	newALine.setAmdbId(stationType, jff, jzz, job);
	newALine.setParameters(s,z,t,ths,thz,tht);
	newALine.isNew(true);
        if (!writeALineCdo->insert_or_assign (id, std::move(newALine)).second) {
	  ATH_MSG_WARNING("More than one (A-line) entry in folder " << folderName << 
			  " for  " << stationType <<
			  " at Jzz/Jff " << jzz << "/" << jff <<
			  " --- keep the latest one");
	  --nNewDecodedALines;
	}

	if (hasBLine && thisRowHasBLine) {
	  // new Bline
	  ++nNewDecodedBLines;
	  BLinePar newBLine;
	  newBLine.setAmdbId(stationType, jff, jzz, job);
	  newBLine.setParameters(bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en);
	  newBLine.isNew(true);
          if (!writeBLineCdo->insert_or_assign (id, std::move(newBLine)).second) {
	    ATH_MSG_WARNING("More than one (B-line) entry in folder " << folderName <<
			    " for  " << stationType <<
			    " at Jzz/Jff " << jzz << "/" << jff <<
			    " --- keep the latest one");
	    --nNewDecodedBLines;
	  }
	}
      }
    }
  }
  ATH_MSG_DEBUG("In folder < " << folderName << 
		  "> # lines/decodedLines/newDecodedALines/newDecodedBLines= " << nLines <<
		  "/" << nDecodedLines <<
		  "/" << nNewDecodedALines <<
		  "/" << nNewDecodedBLines);

  // set A-lines from ASCII file
  if (m_aLinesFile!="" && (int)writeALineCdo->size()>0 ) setALinesFromAscii(writeALineCdo);
  
  // dump A-lines to log file
  if (m_dumpALines && (int)writeALineCdo->size()>0) dumpALines(folderName, writeALineCdo);
   
  // dump B-lines to log file
  if (m_dumpBLines && (int)writeBLineCdo->size()>0) dumpBLines(folderName, writeBLineCdo);

  // !!!!!!!!!!!!!! In the MuonAlignmentDbSvc this was here. I moved it to loadAlignAsBuilt
  // if ( m_asBuiltFile!="" ) setAsBuiltFromAscii();

  ATH_MSG_VERBOSE("Collection CondAttrListCollection CLID " << readCdo->clID());
      
  return  StatusCode::SUCCESS; 
}

StatusCode MuonAlignmentCondAlg::loadAlignILines(std::string folderName) 
{

  // =======================
  // Write ILine Cond Handle
  // =======================
  SG::WriteCondHandle<CscInternalAlignmentMapContainer> writeHandle{m_writeILineKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<CscInternalAlignmentMapContainer> writeCdo{std::make_unique<CscInternalAlignmentMapContainer>()};

  ATH_MSG_INFO("Load alignment parameters from DB folder <" << folderName << ">");

  // =======================
  // Read CSC/ILINES Cond Handle
  // =======================
  SG::ReadCondHandle<CondAttrListCollection> readCscILinesHandle{m_readCscILinesKey};
  const CondAttrListCollection* readCscILinesCdo{*readCscILinesHandle}; 
  if(readCscILinesCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read CSC/ILINES conditions object");
    return StatusCode::FAILURE; 
  } 
  
  EventIDRange rangeCscILinesW;
  if ( !readCscILinesHandle.range(rangeCscILinesW) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readCscILinesHandle.key());
    return StatusCode::FAILURE;
  } 

  ATH_MSG_INFO("Size of CSC/ILINES CondAttrListCollection " << readCscILinesHandle.fullKey() << " readCscILinesCdo->size()= " << readCscILinesCdo->size());
  ATH_MSG_INFO("Range of CSC/ILINES input is " << rangeCscILinesW);

  // =======================
  // Retrieve the collection of strings read out from the DB
  // =======================
  // unpack the strings in the collection and update the 
  // ALlineContainer in TDS
  int nLines = 0;
  int nDecodedLines = 0;
  int nNewDecodedILines = 0;
  CondAttrListCollection::const_iterator itr;
  for (itr = readCscILinesCdo->begin(); itr != readCscILinesCdo->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    ATH_MSG_DEBUG("Data load is " << data << " FINISHED HERE ");
    
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string since_str;
    std::string till_str;
    std::string delimiter = "\n";
      
    
    std::vector<std::string> lines;
    MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
    for (std::string blobline : lines) {
      ++nLines;
      
      std::string delimiter = ":";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      //Check if tokens is not empty
      if (tokens.empty()) {
	ATH_MSG_FATAL("Empty string retrieved from DB in folder " << folderName);
	return  StatusCode::FAILURE;
      }
      type = tokens[0];
      //Parse line
      if (type.find("#")==0) {
	//skip it
	continue;
      }

      if (type.find("Header")==0) {
	std::string delimiter = "|";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	since_str = tokens[1];
	till_str = tokens[2];
      }
	
      if (type.find("IOV")==0) {
	std::string delimiter = " ";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	int ival = 1;
	long int iovThisBlob=0;

	std::string str_iovThisBlob = tokens[ival];
	sscanf(str_iovThisBlob.c_str(),"%80ld",&iovThisBlob);
	ATH_MSG_INFO("Data read from folder " << folderName <<
		     " have IoV = " << iovThisBlob);
      }
	
      if (type.find("Corr")==0) {
        //# Amdb like clob for ilines using geometry tag ISZT-R06-02 
        //# ISZT_DATA_ID VERS TYP JFF JZZ JOB JLAY TRAS TRAZ TRAT ROTS ROTZ ROTT
        //
        //.... example
        //Corr:  CSL 1 -1 3 1 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

	std::string delimiter = " ";
	std::vector<std::string> tokens;
	MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
	if (tokens.size() != 12) {
	  ATH_MSG_FATAL("Invalid length in string retrieved from DB in folder " << folderName << " String length is " << tokens.size());
	  return  StatusCode::FAILURE;
	}

	ATH_MSG_VERBOSE("Parsing Line = ");
	for (std::string token : tokens) ATH_MSG_VERBOSE( token << " | ");
	ATH_MSG_VERBOSE( " " );

	// Start parsing
	int ival=1;
	
	// Station Component identification 
	int jff; 
	int jzz;
	int job;
	int jlay;
	std::string stationType = tokens[ival++];	  
	std::string jff_str = tokens[ival++]; 
	sscanf(jff_str.c_str(),"%80d",&jff);
	std::string jzz_str = tokens[ival++];
	sscanf(jzz_str.c_str(),"%80d",&jzz);
	std::string job_str = tokens[ival++];
	sscanf(job_str.c_str(),"%80d",&job);
	std::string jlay_str = tokens[ival++];
	sscanf(jlay_str.c_str(),"%80d",&jlay);
	
	// I-line
	float tras;
	float traz;
	float trat;
	float rots;
	float rotz;
	float rott;	
	std::string tras_str = tokens[ival++];
	sscanf(tras_str.c_str(),"%80f",&tras);
	std::string traz_str = tokens[ival++];
	sscanf(traz_str.c_str(),"%80f",&traz);
	std::string trat_str = tokens[ival++];
	sscanf(trat_str.c_str(),"%80f",&trat);
	std::string rots_str = tokens[ival++];
	sscanf(rots_str.c_str(),"%80f",&rots);
	std::string rotz_str = tokens[ival++];
	sscanf(rotz_str.c_str(),"%80f",&rotz);
	std::string rott_str = tokens[ival++];
	sscanf(rott_str.c_str(),"%80f",&rott);

	ATH_MSG_VERBOSE("Station type "  << stationType);
	ATH_MSG_VERBOSE("jff / jzz / job / jlay "  <<jff <<" / "<< jzz<<" / "<< job<<" / "<<jlay);
	ATH_MSG_VERBOSE("I-line: tras,traz,trat "  << tras <<" "<< traz <<" "<< trat);
	ATH_MSG_VERBOSE(" rots,rotz,rott "  << rots <<" "<< rotz <<" "<< rott);
	  
	int stationName = m_idHelperSvc->cscIdHelper().stationNameIndex(stationType);
	Identifier id;
	ATH_MSG_VERBOSE("stationName  " << stationName);
	// if (stationType.substr(0,1)=="C") {
	if (stationType.at(0) == 'C') {
	  // csc case
	  int chamberLayer = 2;
	  if (job != 3) ATH_MSG_WARNING("job = "<<job<<" is not 3 => chamberLayer should be 1 - not existing ! setting 2");
	  id = m_idHelperSvc->cscIdHelper().channelID(stationType, jzz, jff, chamberLayer, jlay, 0, 1);
	  ATH_MSG_VERBOSE("identifier being assigned is " << m_idHelperSvc->cscIdHelper().show_to_string(id));
	}
	else {
	  ATH_MSG_ERROR("There is a non CSC chamber in the list of CSC internal alignment parameters.");
	}
	// new Iline
	++nDecodedLines;
	++nNewDecodedILines;
	CscInternalAlignmentPar newILine;
	newILine.setAmdbId(stationType, jff, jzz, job, jlay);
	newILine.setParameters(tras,traz,trat,rots,rotz,rott);
	newILine.isNew(true);
        if (!writeCdo->insert_or_assign (id, newILine).second) {
	  ATH_MSG_WARNING("More than one (I-line) entry in folder " << folderName << 
			  " for  " << stationType <<
			  " at Jzz/Jff/Jlay " << jzz << "/" << jff << "/" << jlay <<
			  " --- keep the latest one");
	  --nNewDecodedILines;
	}
      }
    }
  }
  ATH_MSG_DEBUG("In folder <" << folderName << "> # lines/decodedLines/newDecodedILines= "
		<<nLines<<"/"<<nDecodedLines<<"/"<<nNewDecodedILines<<"/");

   // dump I-lines to log file TBA
  if (m_dumpILines && (int)writeCdo->size()>0) dumpILines(folderName, writeCdo.get());

  if (writeHandle.record(rangeCscILinesW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record CscInternalAlignmentMapContainer " << writeHandle.key() 
		  << " with EventRange " << rangeCscILinesW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeCscILinesW << " into Conditions Store");
  
  ATH_MSG_VERBOSE("Collection CondAttrListCollection CLID " << readCscILinesCdo->clID());
   
  return  StatusCode::SUCCESS;; 
}

StatusCode MuonAlignmentCondAlg::loadAlignAsBuilt(std::string folderName) 
{

  // =======================
  // Write AsBuilt Cond Handle
  // =======================
  SG::WriteCondHandle<MdtAsBuiltMapContainer> writeHandle{m_writeAsBuiltKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  std::unique_ptr<MdtAsBuiltMapContainer> writeCdo{std::make_unique<MdtAsBuiltMapContainer>()};

  ATH_MSG_INFO( "Load alignment parameters from DB folder <"<<folderName<<">" );

  // =======================
  // Read MDT/ASBUILTPARAMS Cond Handle
  // =======================
  SG::ReadCondHandle<CondAttrListCollection> readMdtAsBuiltHandle{m_readMdtAsBuiltParamsKey};
  const CondAttrListCollection* readMdtAsBuiltCdo{*readMdtAsBuiltHandle}; 
  if(readMdtAsBuiltCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MDT/ASBUILTPARAMS conditions object");
    return StatusCode::FAILURE; 
  } 
  
  EventIDRange rangeMdtAsBuiltW;
  if ( !readMdtAsBuiltHandle.range(rangeMdtAsBuiltW) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readMdtAsBuiltHandle.key());
    return StatusCode::FAILURE;
  } 

  ATH_MSG_INFO("Size of MDT/ASBUILTPARAMS CondAttrListCollection " << readMdtAsBuiltHandle.fullKey() << " ->size()= " << readMdtAsBuiltCdo->size());
  ATH_MSG_INFO("Range of MDT/ASBUILTPARAMS input is " << rangeMdtAsBuiltW);

  // =======================
  // Retrieve the collection of strings read out from the DB
  // =======================
  // unpack the strings in the collection and update the 
  // AsBuiltContainer in TDS
  int nLines = 0;
  int nDecodedLines = 0;
  int nNewDecodedAsBuilt = 0;
  MdtAsBuiltPar xPar;
  xPar.isNew(true);
  CondAttrListCollection::const_iterator itr;
  for (itr = readMdtAsBuiltCdo->begin(); itr != readMdtAsBuiltCdo->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    std::string data;
    data=*(static_cast<const std::string*>((atr["data"]).addressOfData()));
    
    ATH_MSG_DEBUG( "Data load is " << data << " FINISHED HERE " );
    
    // Check the first word to see if it is a correction
    std::string type;
    
    //Parse corrections
    std::string delimiter = "\n";
    
    std::vector<std::string> lines;
    MuonCalib::MdtStringUtils::tokenize(data,lines,delimiter);
    for (std::string blobline : lines) {
      ++nLines;
      
      std::string delimiter = ":";
      std::vector<std::string> tokens;
      MuonCalib::MdtStringUtils::tokenize(blobline,tokens,delimiter);
      //Check if tokens is not empty
      if (tokens.empty()) {
	ATH_MSG_FATAL("Empty string retrieved from DB in folder " << folderName);
	return  StatusCode::FAILURE;
      }
      type = tokens[0];
      //Parse line
      if (type.find("#")==0) {
         //skip it
         continue;
      }
	
      if (type.find("Corr")==0) {
	if (!xPar.setFromAscii(blobline)) {
          ATH_MSG_ERROR( "Unable to parse AsBuilt params from Ascii line: " << blobline  );
          continue;
	}

        std::string stationType="XXX";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        xPar.getAmdbId(stationType, jff, jzz, job);
	Identifier id = m_idHelperSvc->mdtIdHelper().elementID(stationType, jzz, jff);
	
	ATH_MSG_VERBOSE("Station type jff jzz "  << stationType  << jff << " " << jzz  );
        ++nDecodedLines;
        ++nNewDecodedAsBuilt;
        if (!writeCdo->insert_or_assign (id, xPar).second) {
	  ATH_MSG_WARNING( "More than one (As-built) entry in folder "<<folderName<<" for  "
                           << stationType<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- keep the latest one" );
          --nNewDecodedAsBuilt;
        }
      }
    }
  }
  ATH_MSG_DEBUG("In folder <"<<folderName<<"> # lines/decodedLines/newDecodedILines= "
		<<nLines<<"/"<<nDecodedLines<<"/"<<nNewDecodedAsBuilt<<"/" );

  // !!!!!!!!!!!!!! In the MuonAlignmentDbTool this was in loadAlignABLines. I moved it here
  if ( m_asBuiltFile!="" ) setAsBuiltFromAscii(writeCdo.get());

  if (writeHandle.record(rangeMdtAsBuiltW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record MdtAsBuiltMapContainer " << writeHandle.key() 
		  << " with EventRange " << rangeMdtAsBuiltW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }		  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeMdtAsBuiltW << " into Conditions Store");

  ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID " << readMdtAsBuiltCdo->clID() );
  
  return  StatusCode::SUCCESS;; 
}

void MuonAlignmentCondAlg::dumpALines(const std::string& folderName,
				    ALineMapContainer* writeALineCdo)
{

  ATH_MSG_INFO("dumping A-lines for folder " << folderName);
  ATH_MSG_INFO("A type jff jzz job s(cm)  z(cm)  t(cm)  ths(rad)  thz(rad)  tht(rad)  ID");

  for (const auto& [ALineId, ALine] : *writeALineCdo) {
    std::string stationType;
    int jff,jzz,job;
    ALine.getAmdbId(stationType,jff,jzz,job);
    float s,z,t,ths,thz,tht;
    ALine.getParameters(s,z,t,ths,thz,tht);
    
    ATH_MSG_INFO("A " << std::setiosflags(std::ios::fixed|std::ios::right)
		 << std::setw(4) << stationType  <<" " 
		 << std::setw(2) << jff  <<"  " 
		 << std::setw(2) << jzz  <<" " 
		 << std::setw(2) << job  <<" "
		 << std::setw(6) << std::setprecision(3) <<  s   <<" "  // here cm !
		 << std::setw(6) << std::setprecision(3) <<  z   <<" "  // here cm !
		 << std::setw(6) << std::setprecision(3) <<  t   <<" "  // here cm !
		 << std::setw(6) << std::setprecision(6) << ths  <<" " 
		 << std::setw(6) << std::setprecision(6) << thz  <<" " 
		 << std::setw(6) << std::setprecision(6) << tht  <<" "
		 << ALineId);

  }
  //std::cout<<std::endl;
}

void MuonAlignmentCondAlg::dumpBLines(const std::string& folderName,
				    BLineMapContainer* writeBLineCdo)
{

  ATH_MSG_INFO( "dumping B-lines for folder " << folderName);
  ATH_MSG_INFO( "B type jff jzz job bs       bp        bn        sp        sn        tw        pg        tr        eg        ep        en        ID");
  
  for (const auto& [BLineId, BLine] : *writeBLineCdo) {
    std::string stationType;
    int jff,jzz,job;
    BLine.getAmdbId(stationType,jff,jzz,job);
    float bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en;
    BLine.getParameters(bs,bp,bn,sp,sn,tw,pg,tr,eg,ep,en);
    
    ATH_MSG_INFO( "B " << std::setiosflags(std::ios::fixed|std::ios::right)
		  << std::setw(4) << stationType  <<" " 
		  << std::setw(2) << jff  <<" " 
		  << std::setw(2) << jzz  <<" " 
		  << std::setw(2) << job  <<"  "
		  << std::setw(6) << std::setprecision(6) <<  bs  <<" "
		  << std::setw(6) << std::setprecision(6) <<  bp  <<" "
		  << std::setw(6) << std::setprecision(6) <<  bn  <<" "
		  << std::setw(6) << std::setprecision(6) <<  sp  <<" " 
		  << std::setw(6) << std::setprecision(6) <<  sn  <<" " 
		  << std::setw(6) << std::setprecision(6) <<  tw  <<" "
		  << std::setw(6) << std::setprecision(6) <<  pg  <<" "
		  << std::setw(6) << std::setprecision(6) <<  tr  <<" "
		  << std::setw(6) << std::setprecision(6) <<  eg  <<" "
		  << std::setw(6) << std::setprecision(6) <<  ep  <<" "
		  << std::setw(6) << std::setprecision(6) <<  en  <<" "
		  << BLineId);
  }
}

void MuonAlignmentCondAlg::dumpILines(const std::string& folderName, 
				    CscInternalAlignmentMapContainer* writeCdo)
{

  ATH_MSG_INFO( "dumping I-lines for folder " << folderName);
  ATH_MSG_INFO( "I \ttype\tjff\tjzz\tjob\tjlay\ttras\ttraz\ttrat\trots\trotz\trott");
  
  for (const auto& [ILineId, ILine] : *writeCdo) {
    std::string stationType;
    int jff,jzz,job,jlay;
    ILine.getAmdbId(stationType,jff,jzz,job,jlay);
    float tras,traz,trat,rots,rotz,rott;
    ILine.getParameters(tras,traz,trat,rots,rotz,rott);
    
    ATH_MSG_INFO( "I\t" << std::setiosflags(std::ios::fixed|std::ios::right)
		  << std::setw(4) << stationType  <<"\t" 
		  << std::setw(2) << jff  <<"\t" 
		  << std::setw(2) << jzz  <<"\t" 
		  << std::setw(2) << job  <<"\t"
		  << std::setw(2) << jlay  <<"\t"
		  << std::setw(6) << std::setprecision(6) <<  tras  <<"\t"
		  << std::setw(6) << std::setprecision(6) <<  traz  <<"\t"
		  << std::setw(6) << std::setprecision(6) <<  trat  <<"\t"
		  << std::setw(6) << std::setprecision(6) <<  rots  <<"\t" 
		  << std::setw(6) << std::setprecision(6) <<  rotz  <<"\t" 
		  << std::setw(6) << std::setprecision(6) <<  rott  <<"\t"
		  << ILineId);
  }
  //std::cout<<std::endl;
}

void MuonAlignmentCondAlg::setALinesFromAscii(ALineMapContainer* writeALineCdo) const
{

  ATH_MSG_INFO( " Set alignment constants from text file " << m_aLinesFile);

  std::ifstream infile;
  infile.open(m_aLinesFile.c_str());
  
  char line[512] ;
  ATH_MSG_DEBUG( "reading file");
  
  while( infile.getline(line,512) ) {
    
    std::istringstream is(line) ;
    
    char AlineMarker[2];
    std::string name; int jff,jzz,obj;
    float tras,traz,trat,rots,rotz,rott;
    if (is >> AlineMarker >> name >> jff >> jzz >> obj
	>> tras >> traz >> trat >> rots >> rotz >> rott) {
      ATH_MSG_DEBUG( "SUCCESSFULY read line: " << line);
    } else {
      ATH_MSG_ERROR( "ERROR reading line: " << line);
    }
    
    if( AlineMarker[0] == '\0') {
      ATH_MSG_DEBUG( "read empty line!"); 
    }
    else {
      
      // loop through A-line container and find the correct one
      std::string testStationType;
      for (auto& [id, ALine] : *writeALineCdo) {
        int testJff,testJzz,testJob;
	ALine.getAmdbId(testStationType,testJff,testJzz,testJob);
	if (testStationType==name &&
	    testJff        == jff &&
	    testJzz        == jzz)
        {
          // set parameter if you found it
          ALine.setParameters(tras,traz,trat,rots,rotz,rott);
          break;
        }
      }
    }
  }
  return;
}

void MuonAlignmentCondAlg::setAsBuiltFromAscii(MdtAsBuiltMapContainer* writeCdo) const
{
  ATH_MSG_INFO (" Set alignment constants from text file "<< m_asBuiltFile);

  std::ifstream fin(m_asBuiltFile.c_str());
  std::string line;
  MdtAsBuiltPar xPar;
  xPar.isNew(true);
  int count = 0;
  while(getline(fin, line)) {
    if (line.find("Corr:")==0) {
      if (!xPar.setFromAscii(line)) {
        ATH_MSG_ERROR( "Unable to parse AsBuilt params from Ascii line: " << line  );
	} else {
        std::string stName="XXX";
        int jff = 0;
        int jzz = 0;
        int job = 0;
        xPar.getAmdbId(stName, jff, jzz, job);
        Identifier id = m_idHelperSvc->mdtIdHelper().elementID(stName, jzz, jff);
        if (!id.is_valid()) {
          ATH_MSG_ERROR( "Invalid MDT identifiers: sta=" << stName << " eta=" << jzz << " phi=" << jff  );
           continue;
        }
        if (writeCdo->insert_or_assign (id, xPar).second)
        {
          ATH_MSG_DEBUG( "New entry in AsBuilt container for Station "
                         <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff<<" --- in the container with key "<< m_idHelperSvc->mdtIdHelper().show_to_string(id) );
        }
        else {
          ATH_MSG_DEBUG( "Updating existing entry in AsBuilt container for Station " <<stName<<" at Jzz/Jff "<<jzz<<"/"<< jff  );
          ATH_MSG_DEBUG( "That is strange since it's read from ASCII so this station is listed twice!"  );
        }
        ++count;
      }
    }
  }
  ATH_MSG_INFO( "Parsed AsBuilt parameters: " << count  );

  return;
}
