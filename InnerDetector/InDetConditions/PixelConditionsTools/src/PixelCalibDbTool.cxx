/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibDbTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelCalibDbTool.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "SGTools/TransientAddress.h" 
#include "CoralBase/Attribute.h" 
#include "CoralBase/AttributeListSpecification.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "AthenaKernel/IIOVDbSvc.h"

#include "PixelCoralClientUtils/PCDDb.h"

// Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include <fstream>
#include <string>
//Includes related to determining presence of ITK
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"



static bool isIBL(false);
static bool isRUN1(false);
static bool isITK(false);
 
//namespace PixelCalib
//{

//================ Constructor =================================================

PixelCalibDbTool::PixelCalibDbTool(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_toolsvc(nullptr),
  m_IOVSvc(nullptr),
  m_pixid(nullptr),
  m_pixman(nullptr),
  m_par_calibfolder("/PIXEL/PixCalib"),
  m_par_caliblocation("PixCalibKey"),
  m_par_writedb(false),
  // added -- A.X.
  m_useCoral(0),
  m_connString(""),
  m_tableName(""),
  m_dbTag(""),
  m_dbRevision(0),
  m_calibData(0),
  m_geoModelSvc("GeoModelSvc",name)
{
  declareInterface< IPixelCalibDbTool >(this); 

  //  template for property decalration
  declareProperty("CalibFolder", m_par_calibfolder); 
  declareProperty("CalibLocation", m_par_caliblocation); 
  declareProperty("WriteDB",m_par_writedb);
  // added -- A.X.
  declareProperty("useCoral",m_useCoral);
  declareProperty("connString",m_connString);
  declareProperty("tableName",m_tableName);
  declareProperty("dbTag",m_dbTag);
  declareProperty("dbRevision",m_dbRevision);
  declareProperty("GeoModelService",m_geoModelSvc);

}
//================ Address update =============================================

StatusCode PixelCalibDbTool::updateAddress(StoreID::type /*storeID*/,SG::TransientAddress* tad,
                                           const EventContext& /*ctx*/)
{ 
  CLID clid = tad->clID(); 
  std::string key = tad->name(); 
  if(146316417 == clid && m_par_caliblocation == key)
    { 
      if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) <<" OK with PixelCalibDataColl "<<endmsg; 
      return StatusCode::SUCCESS; 
    }
  return StatusCode::FAILURE; 
}

//================ Destructor =================================================

 PixelCalibDbTool::~PixelCalibDbTool()
{delete m_calibData;
}

//================ Initialisation =================================================

StatusCode  PixelCalibDbTool::initialize()
{
  // Code entered here will be executed once at program start.
  
  //m_log.setLevel(outputLevel());
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << name() << " initialize()" << endmsg;

  CHECK(service("ToolSvc",m_toolsvc));

  // Get interface to IOVSvc 
  m_IOVSvc = 0; 
  bool CREATEIF(true); 
  CHECK(service("IOVSvc",m_IOVSvc,CREATEIF));


  // determine if RUN1 or RUN2 used 
  if (m_geoModelSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endmsg;
    return (StatusCode::FAILURE);
  }
  if(m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN2) { isIBL =true; }
  if(m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN1) { isRUN1=true; }
  if(m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN4) { isITK =true; }

  // Get the geometry 
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax; 
  CHECK(detStore()->retrieve(m_pixman,"Pixel"));
  CHECK(detStore()->retrieve(m_pixid,"PixelID"));

  if(isRUN1&&(m_pixid->wafer_hash_max()==2048)){
    isIBL = true;
    isRUN1 = false;
  }
  itermin = m_pixman->getDetectorElementBegin(); 
  itermax = m_pixman->getDetectorElementEnd();

  // setup list of TDS objects from geometry description 
  
  m_calibobjs.clear(); 

  // Check all detector elements in the present geometry setup 
  for( iter=itermin; iter !=itermax; ++iter){ 
    const InDetDD::SiDetectorElement* element = *iter; 
    if(element !=0){ 
      Identifier ident = element->identify(); 
      if(m_pixid->is_pixel(ident)){  // OK this Element is included 
        const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
        if(!design)continue;
        unsigned int mchips = design->numberOfCircuits();
        if (!isITK) {
          if(mchips==8||abs(m_pixid->barrel_ec(ident))==2||(m_pixid->barrel_ec(ident)==0&&m_pixid->layer_disk(ident)>0))mchips *=2; // guess numberOfCircuits() 
        }
        m_calibobjs.push_back(std::make_pair(ident,mchips)); 
        // write up the dump calibration here with default value:
      }
    }
  }

  //std::sort(m_calibobjs.begin(), m_calibobjs.end());
  
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" m_calibobjs size "<< m_calibobjs.size()<<" writedb ?" <<m_par_writedb<<endmsg; 

  // if the folder exists, register a callback, if we read from textfile and 
  // the folder already exists, read the textfile in the callback. If the folder does not 
  // yet exist, create and record it. 

    // register the callback--fix later
  if(!m_par_writedb){
    const DataHandle<CondAttrListCollection> calibData;
    if( (detStore()->regFcn(&IPixelCalibDbTool::IOVCallBack, dynamic_cast<IPixelCalibDbTool*>(this), calibData, m_par_calibfolder)).isFailure()){ 
      if(msgLvl(MSG::FATAL))msg(MSG::FATAL) << " PixelCalibDbTool: cannot register callback for folder " <<m_par_calibfolder << endmsg; 
      return StatusCode::FAILURE; 
    } 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " PixelCalibDbTool: The Pixel/Calib keys and chips are: "<<m_calibobjs.size()<<endmsg;
  }
  else{
    CHECK(createPixelCalibObjects());
  }
  return StatusCode::SUCCESS; 
}

//================ Finalisation =================================================

StatusCode  PixelCalibDbTool::finalize()
{
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "PixelCalibDbTool finalize method called" << endmsg; 
  return StatusCode::SUCCESS; 
} 

StatusCode PixelCalibDbTool::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  static ServiceHandle<IIOVDbSvc> lIOVDbSvc("IOVDbSvc",this->name());
  StatusCode sc = StatusCode::SUCCESS; 
  std::list<std::string>::const_iterator itr; 
  for( itr=keys.begin(); itr !=keys.end(); ++itr)
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "IOVCALLBACK for key "<< *itr << " number " << I << endmsg; 

  for(itr=keys.begin(); itr !=keys.end(); ++itr){ 
    if(*itr ==m_par_calibfolder){ 
      if(msgLvl(MSG::INFO))msg(MSG::INFO) <<" Load PixelCalibData from DB" <<endmsg; 
      // reintialize the PixelCalibDataColl 
      sc = createPixelCalibObjects(); 
      if(sc.isFailure()) return StatusCode::FAILURE; 

      if (m_useCoral==0) {

	//
	// standard way to retrieve the constants -- A.X.
	//

      // retrieve the collection of strings read out from the DB 

      const CondAttrListCollection* atrc; 
      sc = detStore()->retrieve(atrc, m_par_calibfolder); 

      if(sc.isFailure()) { 
	if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<<"could not retreive CondAttrListCollection from DB folder "<<m_par_calibfolder<<endmsg; 
	return sc; 
      }

      // unpack the string in the collection and update the PixelCalibDbData in TDS 

      CondAttrListCollection::const_iterator itrx; 
      int kx(0);
      for(itrx = atrc->begin(); itrx !=atrc->end(); ++itrx){
	const coral::AttributeList& atr = itrx->second; 
	std::string data; 
	data =*(static_cast<const std::string*>((atr["data"]).addressOfData())); 
	kx++;
	//	int ix;
	//unsigned int ix;
	std::string ix;
	char* dataPar = const_cast<char*>(data.c_str());
	char buffer[4001],c;
	strncpy(buffer, dataPar,4000);
	char* parameters;
	parameters= strtok(buffer,"I");
	std::istringstream istr(parameters);
 
	int component, eta;
	unsigned int layer,phi;
  if(!isRUN1){
    // RUN-2 or RUN-4 IOVs
//    if(!isIBL)continue;   // Need to think better protection mechanism.
    istr>>component>>c>>layer>>c>>phi>>c>>eta;
  }
  else{	 
	  istr>>ix;
	  unsigned int hashID = atoi(ix.c_str());
	  component =static_cast<int>((hashID & (3 << 25)) / 33554432) * 2 - 2 ;
	  layer =(hashID & (3 << 23)) / 8388608 ;
	  phi = (hashID & (63 << 17)) / 131072 ;
	  eta = static_cast<int>((hashID & (15 << 13)) / 8192) - 6;
	}

	Identifier ident = m_pixid->wafer_id(component, layer, phi, eta);
	PixelCalib::PixelCalibData* datamod = 0;
	datamod = getCalibPtr(ident);
	  
	int item[12];
	float fitem[8];
	int n; 
	int mchips = datamod->getNFEIX();
	for(int k = 0; k<mchips; k++){
	  char* pch;
	  pch= strtok(NULL,"I"); 
	  PixelCalib::PixelCalibData::PixelChipSummaryData datachip;
	  std::istringstream istrx(pch);
	  istrx>>n>>item[0]>>item[1]>>item[2]>>item[3]>>item[4]>>item[5]>>item[6]>>item[7]>>item[8]>>item[9]>>item[10]>>item[11]>>fitem[0]>>fitem[1]>>fitem[2]>>fitem[3]>>fitem[4]>>fitem[5]>>fitem[6]>>fitem[7];
	  datachip.setChipID(k); 
	  datachip.setThreshold(0,item[0],item[1],item[2],item[3]); 
	  datachip.setThreshold(1,item[4],item[5],item[6],item[7]); 
	  datachip.setThreshold(2,item[8],item[9],item[10],item[11]); 
	  datachip.setQ2Tot(0,fitem[0],fitem[1],fitem[2]); 
	  datachip.setQ2Tot(2,fitem[3],fitem[4],fitem[5]); 
	  datachip.setTotRes(fitem[6],fitem[7]); 
	  datamod->add(datachip);
	}
      }
      //update the IOV for the PixelCalibDataColl
      //using the IOV from the string class 
      // finally record PixelCalibDataColl
      if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" Collection CondAttrListCollection CLID "<<atrc->clID()<<endmsg;

      } else {
	//
	// load constants from new db -- A.X. // make sure PCDDb working ok
	//
	if(!isRUN1){
          if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "not implemented loading PixelCalibDb with IBL from Coral Db " << endmsg;
          return StatusCode::FAILURE;
	}
	else{
	  PCDDb db(m_connString, m_tableName, true, m_useCoral>1);
	  if (!db.init(m_dbTag, m_dbRevision)) {
	    if(msgLvl(MSG::ERROR))msg(MSG::ERROR) << "Could not load PixelCalibDb" << endmsg;
	    return StatusCode::FAILURE;
	  }

	  int ix;

	  int nDet = 0;

	  while ( (ix = db.next()) ) {

	    // find charmber ID 
	    Identifier ident = Identifier(ix);
	    //if(std::binary_search(m_calibobjs.begin(), m_calibobjs.end(), ident)){
	      PixelCalib::PixelCalibData* datamod = getCalibPtr(ident);
	      if (datamod) {
		db.set(ix, *datamod);
		++nDet;
	      }
	      //}
	  }
	  if(msgLvl(MSG::INFO))msg(MSG::INFO) << "loaded " << nDet << " detectors, done with PixelCalibDb" << endmsg;
	}
      }
    }
    lIOVDbSvc->dropObject(*itr,true);
  }

  return sc; 
} 

//-------------------------------------------------------------------------------------------
StatusCode PixelCalibDbTool::createPixelCalibObjects() const 
{
  // Create PixelCalib objects when constants are read in from textfile 
  // Make sure that the objects are not read in by IOV before 
  
  if(m_calibData)delete m_calibData;  //remove the existing PixelCalibDataColl objects 
  m_calibData = new PixelCalibDataColl;
  m_calibData->resize(m_pixid->wafer_hash_max());
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " createPixelCalibObjects method called size " << m_calibobjs.size()<<endmsg;

  for(unsigned int i=0; i<m_calibobjs.size(); ++i) {
    /*
    int nfeix = 16;
    int nbc = m_pixid->barrel_ec(m_calibobjs[i]);
    int nl = m_pixid->layer_disk(m_calibobjs[i]);
    int eta = m_pixid->eta_module(m_calibobjs[i]);
    if(abs(nbc)>2)nfeix = 1; // DBM
    if(m_pixid->wafer_hash_max()>1744){
      if(nbc==0&&nl==0){
	nfeix = 2; // FEI4 chip
	if(m_pixid->wafer_hash_max()>2000&&(eta<-6||eta>5))nfeix = 1; // single chip module 
      }
    }
    */
    PixelCalib::PixelCalibData* pat = new PixelCalib::PixelCalibData(m_calibobjs[i].first,m_calibobjs[i].second);
    IdentifierHash id_hash = m_pixid->wafer_hash(m_calibobjs[i].first); 
    (*m_calibData)[id_hash] = pat; 
    if(i<10&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " added empty PixelCalibData for key "<< m_calibobjs[i].first<<" "<<m_calibobjs[i].second<<" "<<pat->getNFEIX()<<endmsg;
  }

  return StatusCode::SUCCESS; 
}

void PixelCalibDbTool::printPixelCalibObjects() const { 
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " PixelCalibDb contains " << m_calibobjs.size()<< " PixelCalibData: "<<endmsg;
  const PixelCalib::PixelCalibData *pat; 
  for (std::vector<std::pair<Identifier,int> >::const_iterator iobj=m_calibobjs.begin(); iobj !=m_calibobjs.end();
       ++iobj){ 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " Module Identifier " <<iobj->first << endmsg; 
    pat=cgetCalibPtr(iobj->first); 
    if(pat){ 
      pat->print(); 
    } else { 
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<< " Could not find key: "<< iobj->first<<endmsg; 
    }
  }
}
 
//============================================================================================
StatusCode PixelCalibDbTool::readPixelCalibDBtoTextFile(std::string file) const
{ 
  std::ofstream* outfile=0; 
  if(msgLvl(MSG::INFO))msg(MSG::INFO) << " read PixelCalibData to text file: " 
	<< file << endmsg; 
  outfile = new std::ofstream(file.c_str()); 
  
  int nchips = 0; 
  int nobj = 0; 
  for(std::vector<std::pair<Identifier,int> >::const_iterator iobj = m_calibobjs.begin(); 
      iobj !=m_calibobjs.end(); ++iobj) {
    const PixelCalib::PixelCalibData* pat = cgetCalibPtr(iobj->first); 
    Identifier key =iobj->first; 
    IdentifierHash wafsh = m_pixid->wafer_hash(key);
    if(!pat){ 
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<< " Cannot find PixelCalibData for module wafer_hash  " << wafsh << endmsg; 
      return StatusCode::FAILURE; 
    }
    if(nobj%100==0&&msgLvl(MSG::INFO)) msg(MSG::INFO) << " ith Module written:"<<nobj<<" with wafer_hash: " << wafsh<<endmsg; 
   
    if(!isRUN1){
      if(!isIBL)continue;
      *outfile<<m_pixid->barrel_ec(key)<<","<<m_pixid->layer_disk(key)<<","<<m_pixid->phi_module(key)<<","<<m_pixid->eta_module(key)<<std::endl;
    }
    else{
      unsigned int hashID = ( (((m_pixid->barrel_ec(key) + 2) / 2) << 25 ) + ( m_pixid->layer_disk(key) << 23) +( m_pixid->phi_module(key) << 17) + ((m_pixid->eta_module(key) + 6) << 13));
      *outfile<<hashID<<std::endl;
    }
    ++nobj; 
    // loop over all data in the PixelCalibData object 

    for (PixelCalib::PixelCalibData::PixelChipSummaryData_citer cit=pat->cbegin(); 
	 cit !=pat->cend(); ++cit) {
      int ichip = cit->getChipID(); 
      ++nchips; 

      *outfile<<"I"<<ichip<<" "<<cit->getThreshold(0)<<" "<<cit->getThresholdSigma(0)<<" "
	      <<cit->getNoise(0)<<" "<<cit->getTimeWalk(0)<<" "<<cit->getThreshold(1)<<" "
	      <<cit->getThresholdSigma(1)<<" "<<cit->getNoise(1)<<" "<<cit->getTimeWalk(1)<<" "
	      <<cit->getThreshold(2)<<" "<<cit->getThresholdSigma(2)<<" "<<cit->getNoise(2)<<" "
	      <<cit->getTimeWalk(2)<<" "<<cit->getQ2TotA(0)<<" "<<cit->getQ2TotE(0)<<" "
	      <<cit->getQ2TotC(0)<<" "<<cit->getQ2TotA(2)<<" "<<cit->getQ2TotE(2)<<" "
	      <<cit->getQ2TotC(2)<<" "<<cit->getTotP1()<<" "<<cit->getTotP2()<<std::endl; 

    }
  }
  outfile->close(); 
  delete outfile; 
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Written "<< nobj <<" PixelCalibData objects" << 
    " with " << nchips << " chips to text file "<<endmsg; 
  return StatusCode::SUCCESS; 
} 

//===================================================================================================
StatusCode PixelCalibDbTool::writePixelCalibTextFiletoDB(std::string file) const{ 
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "WritePixelCalib constants from text file to DB: "<< file<<endmsg; 

  CondAttrListCollection* atrc=0; 
  // check if collection ready exits 
  if(!detStore()->contains<CondAttrListCollection>(m_par_calibfolder)) { 
    if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Creating new CondAttrListCollection for folder "<<m_par_calibfolder <<endmsg; 
    CondAttrListCollection* atrc = new CondAttrListCollection(true); 

    if(StatusCode::SUCCESS !=detStore()->record(atrc,m_par_calibfolder)){ 
      if(msgLvl(MSG::ERROR))msg(MSG::ERROR) <<" Could not create CondAttrListCollection "<<m_par_calibfolder<<endmsg; 
      return StatusCode::FAILURE; 
    }
  }
  //do const cast here so we can add to already existing collections 
  const CondAttrListCollection* catrc = 0; 
  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " Attempting to retrieve collection (const) "<<endmsg; 
  if(StatusCode::SUCCESS !=detStore()->retrieve(catrc, m_par_calibfolder)){ 
    if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<<" Could not retrieve CondAttrListCollection "<<m_par_calibfolder<<endmsg; 
    return StatusCode::FAILURE; 
  }
  atrc = const_cast<CondAttrListCollection*>(catrc); 

  if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<< " About to create AttributeListSpecification "<<endmsg; 
  
  coral::AttributeListSpecification* aspec = new coral::AttributeListSpecification();

  if(aspec==0){
    if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<<" Could not retrieve non-const pointer to aspec"<<endmsg;
    return StatusCode::FAILURE;
  }

  aspec->extend("data", "string");

  int k(0); // count for channels
  bool lprint(0);

  FILE* f = fopen(file.c_str(), "rb"); 
  if(!f){
    if(msgLvl(MSG::INFO))msg(MSG::INFO)<< "Cannot open file "<< file<<endmsg;
    return StatusCode::FAILURE;
  }
  fseek (f, 0L, SEEK_END); 
  int size = ftell (f); 
  fseek(f, 0L, SEEK_SET); 
  
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<< " Input file size is "<<size<<endmsg; 
  Identifier ident_save(0);
  while(!feof(f)){ 
    char header[4001];
    char headerx[250]; 
    lprint = k<10||(k%100==0); 
    
    fgets(headerx, 250, f);

    if(lprint&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"ith "<<k<<" header: "<< headerx <<endmsg;

    // need to unpact for identifier or Hash 
    char* pch;

    pch = strtok(headerx, "NULL"); 

    if(lprint&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Write File to DB: Module Identifier "<<pch <<endmsg; 
    if (strlen(pch)>4000) { 
      fclose(f);
      f = nullptr;
      msg(MSG::ERROR) << "Length of the string exceed 4000 characters (pch) : " << pch << endmsg;
      return StatusCode::FAILURE;
    }
    strncpy(header,pch,250);
    int component, eta;
    unsigned int layer,phi;
    char c;
    if(!isRUN1){
      if(!isIBL)continue;
      std::istringstream istr(pch);
      istr>>component>>c>>layer>>c>>phi>>c>>eta;
    }
    else{
      unsigned int identx = atoi(pch);
      component = static_cast<int>((identx & (3 << 25)) / 33554432) * 2 - 2 ;
      layer = (identx & (3 << 23)) / 8388608 ;
      phi = (identx & (63 << 17)) / 131072 ;
      eta = static_cast<int>((identx & (15 << 13)) / 8192) - 6;
    }
    Identifier ident = m_pixid->wafer_id(component, layer, phi, eta);
    IdentifierHash id_hash = m_pixid->wafer_hash(ident);
    if(ident==ident_save)continue; // make sure there are no two identical modules
    if(lprint&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Write File to DB: Module Identifier ibc "<<component <<" layer-disk "<<layer<<" ph-module "<<phi<<" eta-module "<<eta<<endmsg;
    PixelCalib::PixelCalibData* datamod = getCalibPtr(ident);

    for(int i=0; i<datamod->getNFEIX(); ++i){
      char pack[250];
      fgets(pack, 250, f); 
      if(lprint&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"i "<<i<<" chip data: "<< pack<<endmsg;
      strncat(header, pack, 250);
    }
    std::string sdata = header; //strcat(header, " end"); 
    // fill in the database 
    AthenaAttributeList alist(*aspec);
    if(lprint&&msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<" ident_hash "<<id_hash<<" [ " <<component<<","<<layer<<","<<phi<<","<<eta<<"] "<< " data string: "<< sdata<<endmsg; 
    alist["data"].setValue(sdata); 
    CondAttrListCollection::ChanNum channum = id_hash; 
    atrc->add(channum, alist); 
    ident_save = ident;
    k++; 
  }
  
  fclose(f); 
  f = nullptr;
  if(msgLvl(MSG::INFO))msg(MSG::INFO)<<" Write File to DB with modules "<< k <<endmsg; 

  return StatusCode::SUCCESS; 
}


//============================================================================================
PixelCalib::PixelCalibData* PixelCalibDbTool::getCalibPtr(const Identifier& key) const {
  return const_cast<PixelCalib::PixelCalibData*>(cgetCalibPtr(key)); 
}

//============================================================================================
const PixelCalib::PixelCalibData* PixelCalibDbTool::cgetCalibPtr(const Identifier& key) const { 
  // Retrieve PixelCalibData pointer for a given key - const version 
  //  const PixelCalib::PixelCalibData* pat = 0; 
  //const PixelCalibDataColl* patc = m_calibData; 
  IdentifierHash id_hash = m_pixid->wafer_hash(key); 
  return (*m_calibData)[id_hash]; 
}

//==============================================================================================
//}










