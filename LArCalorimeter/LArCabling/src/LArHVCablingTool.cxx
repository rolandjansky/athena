/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCabling/LArHVCablingTool.h"

/*
#include "LArHVCell.icc"
#include "LArHVDetector.icc"
#include "LArHVElectrode.icc"
#include "LArHVSpecialElectrode.icc"
*/

//#include "CaloIdentifier/CaloIdManager.h"
//#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"


#include "LArIdentifier/LArElectrodeID.h"

/* To go away:
#include "LArIdentifier/LArHVDetectorID.h"
#include "LArIdentifier/LArHVElectrodeID.h"
#include "LArIdentifier/LArHVSpecialElectrodeID.h"
*/

#include "LArIdentifier/LArHVLineID.h"
//#include "LArTools/LArHVMap.h"

#include "StoreGate/StoreGateSvc.h" 

#include <fstream>
#include <iostream>

#include "PathResolver/PathResolver.h"

//#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// initialization of static member variables to shift bit fields 
// of HardwareID in proper positions
/* 
const unsigned int LArHVCablingTool:: m_partition = 1 ; // bits 0-1
const unsigned int LArHVCablingTool:: m_canline   = 4 ; // bit  2-3
const unsigned int LArHVCablingTool:: m_cannode   = 16 ; // bits 3-9
const unsigned int LArHVCablingTool:: m_hvline    = 2048; // bits 10-12

const unsigned int LArHVCablingTool:: m_detector = 1;      // bits 0-2 
const unsigned int LArHVCablingTool:: m_module   = 8;      // bits 3-8
const unsigned int LArHVCablingTool:: m_hvphi    = 512;    // bits 9-11 
const unsigned int LArHVCablingTool:: m_hveta    = 4096;   // bits 12-15
const unsigned int LArHVCablingTool:: m_gaps     = 65536;  // bits 16-18

const unsigned int LArHVCablingTool:: m_elemin   = 1;        // bits 0-7
const unsigned int LArHVCablingTool:: m_elemax   = 256;      // bits 8-15
const unsigned int LArHVCablingTool:: m_nele     = 65536;    // bits 16-21

const unsigned int LArHVCablingTool:: m_nspecial = 1;        // bits 0-1
const unsigned int LArHVCablingTool:: m_elec1    = 4;        // bits 2-8
const unsigned int LArHVCablingTool:: m_elec2    = 512;      // bits 9-15
*/

//==================================================================================
LArHVCablingTool::LArHVCablingTool( const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent ) : AthAlgTool(type,name,parent),
								 m_caloHelper(0),
								 m_emHelper(0) ,
								 m_hecHelper(0) ,
								 m_fcalHelper(0) ,
								 m_hvlineHelper(0),
								 m_electrodeHelper(0), 
								 m_initHVMap(false)
{
  declareProperty("MappingFolder",m_mappingFolder="/LAR/IdentifierOfl/HVLineToElectrodeMap");
  // Declare additional interface
  declareInterface<LArHVCablingTool>(this);
}



//===========================================
StatusCode LArHVCablingTool::finalize ()
//============================================
{// finalize()
  ATH_MSG_DEBUG(" ====> Finalizing LArHVCablingTool... ");
  return StatusCode::SUCCESS;
}
  

//===========================================
StatusCode LArHVCablingTool::initialize ()
//============================================
{// initialize()
  ATH_MSG_DEBUG(" ====> Initializing LArHVCablingTool... ");
  
  // ==========
  //  calo_id_ID
  // ==========
  StatusCode status = detStore()->retrieve(m_caloHelper, "CaloCell_ID");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get Calo_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the Calo_ID helper. " );
  }
  
    // ==========
  //  LArEM_ID
  // ==========
  status = detStore()->retrieve(m_emHelper, "LArEM_ID");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArEM_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the LArEM_ID helper. ");
  }
  // =========
  //  HEC_ID
  // =========
  status = detStore()->retrieve(m_hecHelper, "LArHEC_ID");
  if (status.isFailure())  {
    msg(MSG::FATAL) << "Could not get LArHEC_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the LArHEC_ID helper. " );
  }
  // =========
  //  FCAL_ID
  // =========
  status = detStore()->retrieve(m_fcalHelper, "LArFCAL_ID");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArFCAL_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the LArFCAL_ID helper. " );
  }
 
  // ===========
  // LArHVLineID
  // ===========
  status = detStore()->retrieve(m_hvlineHelper, "LArHVLineID");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArHVLineID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the LArHVLineID helper. " );
  }
        
  // ==============
  // LArElectrodeID
  // ==============
  status = detStore()->retrieve(m_electrodeHelper, "LArElectrodeID");
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArElectrodeID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the LArElectrodeID helper. " );
  }

  
  status = detStore()->regFcn(&LArHVCablingTool::iovCallback,this,m_attrList,m_mappingFolder,true);
  if (status.isFailure()) {
    msg(MSG::ERROR) << "Failed to register callback on folder " << m_mappingFolder << endmsg;
  }
  else
    ATH_MSG_DEBUG("Successfully registered callback on folder " << m_mappingFolder);


  ATH_MSG_DEBUG(" ====> ...initialized LArHVCablingTool ");
  return StatusCode::SUCCESS;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%% NEW INTERFACE FOR HIGH VOLTAGE CODING %%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//============================================================================
const HWIdentifier LArHVCablingTool::getLArHVLineID(HWIdentifier& electrodeId) 
//============================================================================
{
  ATH_MSG_VERBOSE("[getLArHVLineID] entering getLArHVLineID..." );

  // Get maping from fillHVMap() 
  //-----------------------------
  if(!m_initHVMap)
    {// if no map then fill it first
      msg(MSG::INFO) << "[getLArHVLineID] no mapping defined..--> fillHVMap().." << endmsg;
      fillHVMap();
      m_initHVMap = true;
    }
  
  std::map<HWIdentifier,HWIdentifier>::iterator elecIt = m_electrode_hvline_map.find(electrodeId);
  if(elecIt != m_electrode_hvline_map.end()) {
    HWIdentifier hvlineId = (elecIt)->second;
    if (msgLvl(MSG::DEBUG)) {
	const int dete = m_electrodeHelper->detector(electrodeId);
	const int side = m_electrodeHelper->zside(electrodeId);
	const int modu = m_electrodeHelper->module(electrodeId);
	const int hvph = m_electrodeHelper->hv_phi(electrodeId);
	const int hvet = m_electrodeHelper->hv_eta(electrodeId);
	const int hvga = m_electrodeHelper->gap(electrodeId);
	const int elec = m_electrodeHelper->electrode(electrodeId);
	const int part = m_hvlineHelper->partition(hvlineId);
	const int canl = m_hvlineHelper->can_line(hvlineId);
	const int cann = m_hvlineHelper->can_node(hvlineId);
	const int hvli = m_hvlineHelper->hv_line(hvlineId);
	msg(MSG::DEBUG) << "[getLArHVLineID] input ElectrodeId=["
			<< dete << "." << side << "." << modu << "." << hvph 
			<< "." << hvet << "." << hvga << "." << elec << "]" 
			<< " corresponding hvlineID=[" 
			<< part << "." << canl << "." << cann << "." << hvli << "]... OK "
			<< endmsg;
      }//end if debug
    return hvlineId;
  } 
  else {
    int dete = m_electrodeHelper->detector(electrodeId);
    int side = m_electrodeHelper->zside(electrodeId);
    int modu = m_electrodeHelper->module(electrodeId);
    int hvph = m_electrodeHelper->hv_phi(electrodeId);
    int hvet = m_electrodeHelper->hv_eta(electrodeId);
    int hvga = m_electrodeHelper->gap(electrodeId);
    int elec = m_electrodeHelper->electrode(electrodeId);

    HWIdentifier invalidId =  m_hvlineHelper->HVLineId(0,0,0,0);    
    int part = m_hvlineHelper->partition(invalidId);
    int canl = m_hvlineHelper->can_line(invalidId);
    int cann = m_hvlineHelper->can_node(invalidId);
    int hvli = m_hvlineHelper->hv_line(invalidId);
    msg(MSG::WARNING) << "[getLArHVLineID] NO MAP for ElectrodeId=["
	<< dete << "." << side << "." << modu << "." << hvph 
	<< "." << hvet << "." << hvga << "." << elec << "] returning [" 
	<< part << "." << canl << "." << cann << "." << hvli << "]... OK " << endmsg;
    return (invalidId);

    //return m_hvmapHelper->getLArHVLineID( electrodeId ) ; 
  }
  // if the map exists, use the mapping object
}




//=================================================================================
int LArHVCablingTool::fillHVMap() 
//=================================================================================
{// Start of fillHVMap()  
  ATH_MSG_DEBUG("[fillHVMap] entering fillHVMap()..." );

  m_electrode_hvline_map.clear();
  m_hvline_electrode_map.clear();
  
  // Reads in the file
  // -------------------

  std::istringstream infile;

  if (!detStore()->contains<AthenaAttributeList>(m_mappingFolder)) {
    msg(MSG::ERROR) << " Cannot find /LAR/IdentifierOfl/HVLineToElectrodeMap from database, Use ASCII file indeed !!!" << endmsg;
    std::string tablename=PathResolver::find_file ("HVLineToElectrode.data", "DATAPATH");
    if (tablename == "") {
      msg(MSG::WARNING) << "[fillHVMap] Could not locate HVLineToElectrode.data file" <<  endmsg;
      return(-1) ;
     }
     else{
       ATH_MSG_DEBUG("[fillHVMap] have located HVLineToElectrode.data OK !");
     }
    const char * tablefile= tablename.c_str() ;
    std::ifstream myfile(tablefile);
    // get pointer to associated buffer object
    std::filebuf *pbuf;
    pbuf=myfile.rdbuf();
    int size=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
    pbuf->pubseekpos (0,std::ios::in);
    // allocate memory to contain file data
    char * buffer;
    buffer=new char[size];
    // get file data  
    pbuf->sgetn (buffer,size);
    myfile.close();
    infile.str(buffer);
    delete [] buffer; 
  }
  else {
    //const AthenaAttributeList*    attrList     = 0;
    //StatusCode sc=detStore()->retrieve(attrList,m_mappingFolder);
    //if (sc.isFailure()) {
    if (!m_attrList.isValid()) {
      msg(MSG::ERROR) << "Failed to retrieve AttributeList with key " << m_mappingFolder << " from DetectorStore" << endmsg;
      return -1;
    }

    msg(MSG::INFO) <<  " Found AttrList from " << m_mappingFolder << endmsg;
    std::ostringstream attrStr1;
    coral::AttributeList::const_iterator itr=m_attrList->begin();
    itr->toOutputStream(attrStr1);
    std::string::size_type pos = attrStr1.str().find(":");
    if (pos == std::string::npos)
      pos = 0;
    else
      pos++;
    std::string Line=attrStr1.str().substr(pos);
    ATH_MSG_DEBUG(" Line " << Line );
    infile.str(Line);
  }

  if(!infile) {
    msg(MSG::WARNING) << "[fillHVMap] Could not load HVLineToElectrode.data file" <<  endmsg;
    return(-1) ;
  }
  else{
    ATH_MSG_DEBUG("[fillHVMap] have loaded HVLineToElectrode.data OK !");
  }
  int dummy;
  int hvcannode;
  int hvline;
  int hvpartition;
  int hvcanline;
  int detect;
  int side;
  int mod;
  int hvphi;
  int hveta;
  int hvgap;
  int min;
  int max;
  ATH_MSG_DEBUG("start reading infile " );
  while ( infile 
	  >> dummy
	  >> hvcannode
	  >> hvline
	  >> hvpartition 
	  >> hvcanline
	  >> detect
	  >> side
	  >> mod
	  >> hvphi
	  >> hveta
	  >> hvgap
	  >> min
	  >> max )
    {// Fill Maps
      if (max > 1000)
        std::abort();
      ATH_MSG_DEBUG("[fillHVMap] listing HVLineToElectrode :(" 
		    << dummy << ") hvline=["
		    << hvpartition << "." 
		    << hvcanline << "."
		    << hvcannode << "." 
		    << hvline << "] electrode=["  
		    << detect << "." 
		    << side << "." 
		    << mod << "." 
		    << hvphi << "." 
		    << hveta << "." 
		    << hvgap << " min=" 
		    << min << " max=" 
		    << max << "]"  
		    );

      std::vector<HWIdentifier> electrodeIdvec;  
      electrodeIdvec.clear();
      HWIdentifier hvlineId = m_hvlineHelper->HVLineId(hvpartition,hvcanline,hvcannode,hvline);

      //std::cout <<  "  module, line  " << hvcannode << " " << hvline << "  ---- > hvLineId " << hvlineId << std::endl;

      if( max > min ){
        if (min < 0) min = 0;
        if (max < 0) max = 0;
        if (min > 1000) min = 1000; // Shouldn't happen.
	for( int ielec=min; ielec<max+1; ielec++){
	  ATH_MSG_VERBOSE("[fillHVMap] defining electrodeId: [det=" 
			  << detect << " side=" 
			  << side << " mod=" 
			  << mod << " phi=" 
			  << hvphi << " eta=" 
			  << hveta << " gap=" 
			  << hvgap << " ielec="
			  << ielec << "]" );
	  HWIdentifier electrodeId = 
	    m_electrodeHelper->ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,ielec);
	  //ATH_MSG_DEBUG("[fillHVMap] have defined electrodeId..." );
	  electrodeIdvec.push_back(electrodeId);
	  //ATH_MSG_DEBUG("[fillHVMap] push_back in electrodeId vector..." );

         // Check if electrode is already in map ?
         // std::map<HWIdentifier,HWIdentifier>::iterator testIt = m_electrode_hvline_map.find(electrodeId);
         // if (testIt != m_electrode_hvline_map.end()) {
         //    std::cout << " ----- Electrode Id " << side << " " << mod << " " << hvphi << " " << hveta << " " << hvgap << " " << ielec <<
         //     "   read for HvLine " << hvcannode << " " << hvline << " " << hvlineId << "    already exist for " << (testIt)->second  << std::endl;
         //  }

	  m_electrode_hvline_map.insert(std::pair<HWIdentifier,HWIdentifier>(electrodeId,hvlineId));
	}
      }
      else{
	//msg(MSG::INFO) << "[fillHVMap] max < min STOP !!!" << endmsg;
	HWIdentifier electrodeId = m_electrodeHelper->ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,min);
	electrodeIdvec.push_back(electrodeId);

         // Check if electrode is already in map ?
         // std::map<HWIdentifier,HWIdentifier>::iterator testIt = m_electrode_hvline_map.find(electrodeId);
         // if (testIt != m_electrode_hvline_map.end()) {
         //    std::cout << " ----- Electrode Id " << side << " " << mod << " " << hvphi << " " << hveta << " " << hvgap << " " << min <<
         //     "   read for HvLine " << hvcannode << " " << hvline << " " << hvlineId << "    already exist for " << (testIt)->second  << std::endl;
         // }

        m_electrode_hvline_map.insert(std::pair<HWIdentifier,HWIdentifier>(electrodeId,hvlineId));
      }

      std::map<HWIdentifier,std::vector<HWIdentifier> >::iterator elecIt=m_hvline_electrode_map.find(hvlineId);
      if( elecIt != m_hvline_electrode_map.end()) {
           std::vector<HWIdentifier>* list1 = &((elecIt)->second);
           for (unsigned int ii=0;ii<electrodeIdvec.size();ii++) {
               list1->push_back(electrodeIdvec[ii]);
           }
      }
      else {
	  m_hvline_electrode_map.insert(std::pair<HWIdentifier,std::vector<HWIdentifier> >(hvlineId,electrodeIdvec));
      }
    }  
  ATH_MSG_DEBUG(" end reading infile " );
  ATH_MSG_DEBUG("[fillHVMap] Mapping electrode-to-hvline : number of Ids= "<<m_electrode_hvline_map.size());
  ATH_MSG_DEBUG("[fillHVMap] Mapping hvline-to-electrode : number of Ids= "<<m_hvline_electrode_map.size());
  //GU infile.close();
  ATH_MSG_DEBUG("[fillHVMap] exiting fillHVMap ... OK" );
  return(0);  
} 


//========================================================================================
const std::vector<HWIdentifier>& LArHVCablingTool::getLArElectrodeIDvec(HWIdentifier &hvlineId) 
//========================================================================================
{// Start of getLArElectrodeIdvec()
  ATH_MSG_VERBOSE("Entering getLArElectrodeIDvec( hvlineId )" );
  // Get maping from fillHVMap() 
  //-----------------------------
  if(!m_initHVMap)
    {// if no map then fill it first
      msg(MSG::INFO) << "[getLArHVLineID] no mapping defined..--> fillHVMap().." << endmsg;
      fillHVMap();
      m_initHVMap = true;
    }
  // Use fillHVMap generated-objects  
  // if(!m_hvmapHelper){ 
  //  fillHVMap();
  //}
  std::map<HWIdentifier,std::vector<HWIdentifier> >::iterator elecIt=m_hvline_electrode_map.find(hvlineId);
  if( elecIt != m_hvline_electrode_map.end()){
    return (elecIt)->second;
  }
  else{
    int part = m_hvlineHelper->partition(hvlineId);
    int canl = m_hvlineHelper->can_line(hvlineId);
    int cann = m_hvlineHelper->can_node(hvlineId);
    int hvli = m_hvlineHelper->hv_line(hvlineId);
    msg(MSG::WARNING) << "LArHVCablingSvc: getLArElectrodeIDvec didnot find electrode to hvline [" 
	<< part << "." << canl << "." << cann << "." << hvli << "]" << endmsg;
    static std::vector<HWIdentifier> invalid;
    return(invalid);
  }
  //return m_hvmapHelper->getLArElectrodeIDvec( hvlineId ); 
}







//==========================================================================
void LArHVCablingTool::getElectrodeInCell( const Identifier& offId, std::vector<HWIdentifier> &electrodeIdVec )
//============================================================================
{//Start of getCellElectrodeIDvec
  ATH_MSG_VERBOSE("Entering getElectrodeInCell( offlineId ) " );
  
  // Get HV fields from offline fields
  //===================================
  int Detector = -1;
  int Side     = -9;
  int iGapMax  = -1;
  if( m_caloHelper->is_em( offId ) ){
    const int sampl = m_emHelper->sampling( offId );
    const int bec   = m_emHelper->barrel_ec( offId);
    if( bec > 0 ){ 
      Side = 0;}
    else{Side = 1;}
    if( m_caloHelper->is_em_barrel(offId) && sampl != 0 ){
      // EMB
      Detector = 0;
      iGapMax = 1;
    }
    if( m_caloHelper->is_em_barrel(offId) && sampl == 0 ){
      // EMBPS
      Detector = 1;
      iGapMax  = 0;
    }
    if( m_caloHelper->is_em_endcap(offId) && sampl != 0 ){ 
      // EMEC
      Detector = 2;
      iGapMax  = 1;
    }
    if( m_caloHelper->is_em_endcap(offId) && sampl == 0 ){ 
      // EMECPS
      Detector = 3;
      iGapMax  = 0;
    }
  }
  else if( m_caloHelper->is_hec( offId )  ){
    const int bec   = m_hecHelper->pos_neg(offId);
    if( bec > 0 ){ 
      Side = 0;}
    else{Side = 1;}
    Detector = 4;
    iGapMax  = 3;
  }
  else if( m_caloHelper->is_fcal( offId )  ){
    const int bec   = m_fcalHelper->pos_neg(offId);
    Detector = 5;
    iGapMax  = 3;
    if( bec > 0 ){ 
      Side = 0;}
    else{Side = 1;}
  }

  // ------------------------------------------------------------
  // Get cell localisation information in terms of HV description
  // ------------------------------------------------------------
  const int Module = getCellModule( offId );
  const int EtaBin = getCellEtaSector( offId );
  const int PhiBin = getCellPhiSector( offId );
  const int elecmin = getCellElectrodeMinMax(offId).first;
  const int elecmax = getCellElectrodeMinMax(offId).second;
  ATH_MSG_DEBUG("[getArHVLineID] ElectrodeId=["
      << Detector << "." 
      << Side << "." 
      << Module << "." 
      << PhiBin << "." 
      << EtaBin << ".gap_max=" 
      << iGapMax << ".min=" 
      << elecmin << ".max="  
      << elecmax << "]" );
  for( int ielec=elecmin; ielec<elecmax+1; ielec++){
    for( int iGap=0; iGap<iGapMax+1; iGap++){
      const HWIdentifier electrodeId = 
	m_electrodeHelper->ElectrodeId(Detector,Side,Module,PhiBin,EtaBin,iGap,ielec);
      electrodeIdVec.push_back(electrodeId);
    }
  }
  ATH_MSG_DEBUG("vector size = " << electrodeIdVec.size() );
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// End of getElectrodeInCell()
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<







//==========================================================================
void LArHVCablingTool::getHVLineInCell( const Identifier& offId, std::vector<HWIdentifier> &hvlineIdVec )
//============================================================================
{//Start of getHVLineInCell
  ATH_MSG_VERBOSE("Entering getHVLineInCell( offlineId ) " );

  hvlineIdVec.clear();  

  // Get maping from fillHVMap() 
  //-----------------------------
  if(!m_initHVMap)
    {// if no map then fill it first
      msg(MSG::INFO) << "[getHVLineInCell] no mapping defined..--> fillHVMap().." << endmsg;
      fillHVMap();
      m_initHVMap = true;
    }

  // Get HV fields from offline fields
  //===================================
  int Detector = -1;
  int Side     = -9;
  int iGapMax  = 0;
  if( m_caloHelper->is_em( offId ) ){
    const int sampl = m_emHelper->sampling( offId );
    const int bec   = m_emHelper->barrel_ec( offId);
    if( bec > 0 ){ 
      Side = 0;}
    else{Side = 1;
    }
    if( m_caloHelper->is_em_barrel(offId) && sampl != 0 ){
      // EMB
      Detector = 0;
      iGapMax = 1;
    }
    if( m_caloHelper->is_em_barrel(offId) && sampl == 0 ){
      // EMBPS
      Detector = 1;
      iGapMax  = 0;
    }
    if( m_caloHelper->is_em_endcap(offId) && sampl != 0 ){ 
      // EMEC
      Detector = 2;
      iGapMax  = 1;
    }
    if( m_caloHelper->is_em_endcap(offId) && sampl == 0 ){ 
      // EMECPS
      Detector = 3;
      iGapMax  = 0;
    }
  }
  else if( m_caloHelper->is_hec( offId )  ){
    const int bec   = m_hecHelper->pos_neg(offId);
    if( bec > 0 ){ 
      Side = 0;}
    else{Side = 1;}
    Detector = 4;
    iGapMax  = 3;
  }
  else if( m_caloHelper->is_fcal( offId )  ){
    const int bec   = m_fcalHelper->pos_neg(offId);
    if( bec > 0 )
      { Side = 0;}
    else{Side = 1;}
    Detector = 5;
    iGapMax  = 3;
  }

  ATH_MSG_DEBUG("[getHVLineInCell] identified detector det=" 
      << Detector << " (0=EMB, 1=EMBPS, 2=EMEC, 3=EMECPS, 4=HEC 5=FCAL)" 
      );


  // ------------------------------------------------------------
  // Get cell localisation information in terms of HV description
  // ------------------------------------------------------------
  const int Module = getCellModule( offId );
  const int EtaBin = getCellEtaSector( offId );
  const int PhiBin = getCellPhiSector( offId );
  const int elecmin = getCellElectrodeMinMax(offId).first;
  const int elecmax = getCellElectrodeMinMax(offId).second;
  for( int ielec=elecmin; ielec<elecmax+1; ielec++){
    // Loop over electrodes in cell
    ATH_MSG_VERBOSE("[getHVLineInCell] defining electrodeId: [det=" 
		    << Detector << " sid="
		    << Side << " mod=" 
		    << Module << " phi="
		    << PhiBin << " eta=" 
		    << EtaBin << " ielec="
		    << ielec  << "]" 
		    );
    for( int iGap=0; iGap<iGapMax+1; iGap++){ 
      // Loop over gaps
      const HWIdentifier electrodeId = 
	m_electrodeHelper->ElectrodeId(Detector,Side,Module,PhiBin,EtaBin,iGap,ielec);
      const int dete = m_electrodeHelper->detector(electrodeId);
      const int side = m_electrodeHelper->zside(electrodeId);
      const int modu = m_electrodeHelper->module(electrodeId);
      const int hvph = m_electrodeHelper->hv_phi(electrodeId);
      const int hvet = m_electrodeHelper->hv_eta(electrodeId);
      const int hvga = m_electrodeHelper->gap(electrodeId);
      const int elec = m_electrodeHelper->electrode(electrodeId);
      std::map<HWIdentifier,HWIdentifier>::iterator elecIt = 
	m_electrode_hvline_map.find(electrodeId);
      if(elecIt != m_electrode_hvline_map.end()){
	// found corresponding HVline 
	HWIdentifier hvlineId = (elecIt)->second;

	int part1 = m_hvlineHelper->partition(hvlineId);
	int canl1 = m_hvlineHelper->can_line(hvlineId);
	int cann1 = m_hvlineHelper->can_node(hvlineId);
	int hvli1 = m_hvlineHelper->hv_line(hvlineId);

	if (msgLvl(MSG::DEBUG)) {
	  msg(MSG::DEBUG) << "[getHVLineInCell] input ElectrodeId=["
			  << dete << "." << side << "." << modu << "." << hvph 
			  << "." << hvet << "." << hvga << "." << elec << "]" 
			  << " corresponding hvlineID=[" 
			  << part1 << "." << canl1 << "." 
			  << cann1 << "." << hvli1 << "] "  
			  << endmsg;
	}
	if( hvlineIdVec.size() == 0 ){
	  ATH_MSG_DEBUG("[getHVLineInCell] -> first fill of vector..." );
	  hvlineIdVec.push_back(hvlineId);
	}
	else{
	  bool foundHvId = false;	
	  std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
	  std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end(); 
	  for(; hv!=hvEnd;++hv){
	    HWIdentifier hvRef = *hv;
	    int part = m_hvlineHelper->partition(hvRef);
	    int canl = m_hvlineHelper->can_line(hvRef);
	    int cann = m_hvlineHelper->can_node(hvRef);
	    int hvli = m_hvlineHelper->hv_line(hvRef);
	    ATH_MSG_DEBUG("[getHVLineInCell] -> in loop : hvRef=[" 
		<< part << "." << canl << "." 
		<< cann << "." << hvli << "] " 
		);
	    if( hvlineId == hvRef ){
	      foundHvId = true;
	      ATH_MSG_DEBUG("[getHVLineInCell] -> BREAK !" );
	      break;
	    }
	  }
	  if( !foundHvId ){
	    hvlineIdVec.push_back(hvlineId);
	    ATH_MSG_DEBUG("[getHVLineInCell] -> new HV line added" 
		<< "=[" 
		<< part1 << "." << canl1 << "." 
		<< cann1 << "." << hvli1 << "] size of vector=" 
		<< hvlineIdVec.size()  
		);
	  }
	  else{
	    ATH_MSG_DEBUG("[getHVLineInCell] -> HV line already IN" 
		<< "=[" 
		<< part1 << "." << canl1 << "." 
		<< cann1 << "." << hvli1 << "] size of vector=" 
		<< hvlineIdVec.size()  
		);	    
	  }	  
	}
      } 
      else{
	msg(MSG::WARNING) << "[getHVLineInCell] couldnot find the electrodeID in map [" 
	    << Detector << " Sid="
	    << Side << " Mod="
	    << Module << " phi="
	    << PhiBin << " eta=" 
	    << EtaBin << " Gap=" 
	    << iGap   << " elec_min="
	    << elecmin <<" elec_max=" 
	    << elecmax << "]" 
	    << endmsg;
      }
    } // Loop over gaps
  }// Loop over electrodes in cell
  ATH_MSG_DEBUG("vector size = " << hvlineIdVec.size() );
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// End of getHVLineInCell()
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//=====================================================================
int LArHVCablingTool::getCellModule( const Identifier &offId )
//=====================================================================
//  Returns the LArModule index as an integer
//
//  Last Modified:
//  05-FEB-2007 AL fixed FCAL info --thanks to Zhaoyu Yang
//  02-FEB-2007 AL added Module info for FCAL 
//  31-JAN-2007 AL reversed ModuleID returned for EMEC C-side 
//
// =====================================================================
{// Start of getCellModule
  //======================================
  ATH_MSG_VERBOSE("Entering getCellModule " );

  int bec      =-99;
  int sampling =-99;
  int region   =-99;
  int phi      =-99;
  //int module   =-99;
  // Returned Value
  // =====================================
  int ModuleID =-99;

  // Identify the Helper to be used 
  // =====================================
  if( m_caloHelper->is_em( offId ) ){
    bec      = m_emHelper->barrel_ec(offId);
    sampling = m_emHelper->sampling(offId);
    region   = m_emHelper->region(offId);
    phi      = m_emHelper->phi(offId);
    ATH_MSG_DEBUG("[getCellModule] processing an EM cell.." );
  }
  else if( m_caloHelper->is_hec( offId )  ){
    bec      = m_hecHelper->pos_neg(offId)*10; 
    sampling = m_hecHelper->sampling(offId);
    region   = m_hecHelper->region(offId);
    phi      = m_hecHelper->phi(offId);
    ATH_MSG_DEBUG("[getCellModule] processing an HEC cell.." );
  }
  else if( m_caloHelper->is_fcal( offId )){
    bec      = m_fcalHelper->pos_neg(offId)*20; 
    sampling = m_fcalHelper->module(offId);
    //module   = m_fcalHelper->module(offId);
    phi      = m_fcalHelper->phi(offId);
    ATH_MSG_DEBUG("[getCellModule] processing an FCAL cell.." );
  }

  // Now determine the Module ID 
  if( abs(bec) == 1 ){
    // EM BARREL
    // ===========================================================================
    // x being the ATLAS x direction (pointing towards the center of the collider)
    // we have :             
    //                        SIDE - C (z<0)               SIDE - A (z>0)
    //                        -------------                --------------
    //                          C04                            A04
    //                      C03    C05                      A05   A03  
    //                     C02        C06                 A06       A02    
    //                    C01          C07               A07          A01
    //                    C00----------C08-->X           A08----------A00---> X
    //                    C15          C09               A09          A15
    //                      C14       C10                 A10        A14
    //                       C13    C11                     A11   A13      
    //                          C12                            A12 
    //============================================================================
    if( bec == 1 ){
      // A-WHEEL 
      // =======
      if( sampling == 0 ){
	// ------------------------------
	// 32 Modules of two cells in phi
	// ------------------------------
	int phi_module=int(phi/2);
	ModuleID = phi_module;
      }
      else if( sampling == 1 && region == 0 ){
	// -----------------------------
	// 16 Modules of 4 cells in phi 
	// -----------------------------
	int phi_module = int((phi+2)/4);
	if( phi_module == 16 ){
	  ModuleID = 0;
	}
	else{
	  ModuleID = phi_module;
	}
      }
      else if( sampling == 2 || sampling == 3 || (sampling == 1 && region == 1 ) ){
	// -----------------------------
	// 16 Modules of 16 cells in phi
	// -----------------------------
	int phi_module = int((phi+8)/16);
	if( phi_module == 16 ){
	  ModuleID=0;
	}
	else{
	  ModuleID=phi_module;
	}
      }
    }
    
    else if( bec == -1 ){
      // C-WHEEL 
      // =======
      if( sampling == 0 ){
	// ------------------------------
	// 32 Modules of two cells in phi
	// ------------------------------
	int phi_module=int(phi/2);
	if( phi_module <= 15 ){
	  phi_module = 15 - phi_module; 
	}
	else{
	  phi_module = 47 - phi_module;
	}
	ModuleID = phi_module;
      }
      else if( sampling == 1 && region == 0 ){
	// -----------------------------
	// 16 Modules of 4 cells in phi 
	// -----------------------------
	int phi_module = int((phi+2)/4);
	if( phi_module <= 8){
	  phi_module = 8 - phi_module;
	}
	else{
	  phi_module = 24 - phi_module;
	}
	ModuleID = phi_module;
      }
      else if( sampling == 2 || sampling == 3 || (sampling == 1 && region == 1 ) ){
	// -----------------------------
	// 16 Modules of 16 cells in phi
	// -----------------------------
	int phi_module = int((phi+8)/16);
	if( phi_module <= 8){
	  phi_module = 8 - phi_module;
	}
	else{
	  phi_module = 24 - phi_module;
	}
	ModuleID = phi_module;
      }
    }
  }

  else if( abs(bec) == 2 || abs(bec) == 3){
    // EMEC 
    // ----
    //
    // x being the ATLAS x direction (pointing towards the center of the collider)
    // we have :             
    //                        SIDE - C                      SIDE - A
    //                        --------                     -----------
    //
    //                        ECC1  ECC2                    ECA2   ECA1    
    //                              
    //                     ECC0         ECC3             ECA3        ECA0
    //                            -------------> X               -----------> X
    //                     ECC7         ECC4             ECA4        ECA7
    //                                                           
    //                        ECC6  ECC5                    ECA5   ECA6    
    //
    //
    if( bec > 0 ){
      // SIDE - A
      if( sampling == 0 || sampling == 1 || 
	  (sampling == 2 && abs(bec) == 3 ) ){
	// -------------------------------
	// 8 Modules of 8 cells in phi
	// -------------------------------
	// Sampling 0, 1 (inner+outer) & 2 region 0 (inner)
	//
	int phi_local = int(phi/8);
	ModuleID = phi_local;
      }
      else if( (sampling == 2 && abs(bec) == 2) || (sampling == 3 && abs(bec) == 2)){
	// ----------------------------------------
	// 8 Modules of 32 cells in phi
	// ----------------------------------------
	// Sampling 2 & 3 ( outer wheel) in [0,255] 
	// 
	int phi_local = int(phi/32);
	ModuleID = phi_local;
      }
    }
    else{
      // SIDE-C
      if( sampling == 0 || sampling == 1 || 
	  (sampling == 2 && abs(bec) == 3 ) ){
	// ------------------------------
	// 8 Modules of 8 cells in phi
	// -------------------------------
	// Sampling 0, 1 & 2( inner wheel) in [0,63] 
	//
	int rev_phi=-99;
	int phi_local = int(phi/8);
	if( phi_local == 0 ){rev_phi = 3;}
	if( phi_local == 1 ){rev_phi = 2;}
	if( phi_local == 2 ){rev_phi = 1;}
	if( phi_local == 3 ){rev_phi = 0;}
	if( phi_local == 4 ){rev_phi = 7;}
	if( phi_local == 5 ){rev_phi = 6;}
	if( phi_local == 6 ){rev_phi = 5;}
	if( phi_local == 7 ){rev_phi = 4;}
	ModuleID = rev_phi;
      }
      else if( (sampling == 2 && abs(bec) == 2) || (sampling == 3 && abs(bec) == 2)){
	// ----------------------------------------
	// 8 Modules of 32 cells in phi
	// ----------------------------------------
	// Sampling 2 & 3 ( outer wheel) in [0,255] 
	//
	int rev_phi=-99; 
	int phi_local = int(phi/32);
	if( phi_local == 0 ){rev_phi = 3;}
	if( phi_local == 1 ){rev_phi = 2;}
	if( phi_local == 2 ){rev_phi = 1;}
	if( phi_local == 3 ){rev_phi = 0;}
	if( phi_local == 4 ){rev_phi = 7;}
	if( phi_local == 5 ){rev_phi = 6;}
	if( phi_local == 6 ){rev_phi = 5;}
	if( phi_local == 7 ){rev_phi = 4;}
	ModuleID = rev_phi;
      }
    }
  }


  else if( abs(bec) == 20 ){
    // HEC Detector
    // ============
    if( bec > 0){
      // WHEEL A
      if( region == 0 ){
	// OUTER WHEEL
	// ----------------------------
	// 32 Modules of 2 cells in phi
	// ----------------------------
	int phi_module=int(phi/2);
	ModuleID = phi_module;
      }
      else if( region == 1){
	// INNER WHEEL
	// ----------------------------
	// 32 Modules of 1 cells in phi
	// ----------------------------
	int phi_module=int(phi);
	ModuleID = phi_module;
      }	
    }
    else if( bec < 0){
      // WHEEL C
      if( region == 0 ){
	// OUTER WHEEL
	// -----------
	int phi_module=int(phi/2);
	if( phi_module <= 15 ){
	  phi_module = 15 - phi_module; 
	}
	else{
	  phi_module = 47 - phi_module;
	}
	ModuleID = phi_module;
      }
      else if( region == 1){
	// INNER WHEEL
	// -----------
	int phi_module=int(phi);
	if( phi_module <= 15 ){
	  phi_module = 15 - phi_module; 
	}
	else{
	  phi_module = 47 - phi_module;
	}
	ModuleID = phi_module;	
      }  
    }
  }
  // ===========================================================================
  // x being the ATLAS x direction (pointing towards the center of the collider)
  // we have :             
  //                        SIDE - C (z<0)               SIDE - A (z>0)
  //                        -------------                --------------
  //                          C04                            A04
  //                      C03    C05                      A05   A03  
  //                     C02        C06                 A06       A02    
  //                    C01          C07               A07          A01
  //                    C00----------C08-->X           A08----------A00---> X
  //                    C15          C09               A09          A15
  //                      C14       C10                 A10        A14
  //                       C13    C11                     A11   A13      
  //                          C12                            A12 
  //============================================================================
  
  else if( abs(bec) == 40 ){
    // FCAL Detector
    // ==============
    // FCAL-Module 1   : EM 
    // FCAL-Module 2,3 : Hadronic
    // -> 16 cells in phi per Module
    // ==============================
    if( sampling == 1){
      ModuleID = int(phi);
    }
    else if( sampling == 2 ){
      ModuleID = int(phi/2);
    }
    else if( sampling == 3){
      ModuleID = int(phi/4);
    }
  }
  else{
    msg(MSG::WARNING) << " -->> Invalid MODULE returned " << endmsg;
  }
  ATH_MSG_DEBUG("Closing getCellModule " );
  return ModuleID;
}



//===================================================================
int LArHVCablingTool::getCellEtaSector( const Identifier &offId )
//===================================================================
// Returns the HV eta sector given an cell ID as an integert
//
// 05-FEB-2007 : AL added FCAL info from Zhaoyu
//
// 11-FEB-2005: modified getHVEtaSector to return an int.
// 25-NOV-2004: AL Added EMEC 
// 09-DEC-2004: AL fixed EMBPS : change from 8 to 4 HV-sectors in Eta (08-DEC-2004)
//                & fixed EMEC   
// 15-DEC-2004: AL added HEC wheels
//===================================================================
{// Start of getHVEtaSector()
  ATH_MSG_VERBOSE("Entering getCellEtaSector ...." );

  std::string l_version = m_emHelper->dictionaryVersion();

  // Returned Value
  int l_bec   =-99;
  int sampling=-99;
  int region  =-99;
  int ieta    =-99;
  // Identify the Helper to be used 
  // =====================================
  if( m_caloHelper->is_em( offId ) ){
    l_bec    = m_emHelper->barrel_ec(offId);
    sampling = m_emHelper->sampling(offId);
    region   = m_emHelper->region(offId);
    ieta     = m_emHelper->eta(offId);
  }
  else if( m_caloHelper->is_hec( offId )  ){
    l_bec    = m_hecHelper->pos_neg(offId)*10; 
    sampling = m_hecHelper->sampling(offId);
    region   = m_hecHelper->region(offId);
    ieta     = m_hecHelper->eta(offId);
  }
  else if( m_caloHelper->is_fcal( offId )  ){
    l_bec    = m_fcalHelper->pos_neg(offId)*20; 
    sampling = m_fcalHelper->module(offId);
  }

  // Output argument
  // =====================================
  int hvEta=-99;

  // Construct heEta
  // =====================================
  if( abs(l_bec) == 1 ){
    // BARREL 
    if( sampling == 1 )
      {// FRONT sampling
	if( region == 0 ){
	  if( ieta >= 0 && ieta < 64 ){
	    hvEta=1;
	  } 
	  else if( ieta >=64 && ieta < 128 ){
	    hvEta=2;
	  }
	  else if( ieta >=128 && ieta < 192 ){
	    hvEta=3;
	  }
	  else if( ieta >=192 && ieta < 256 ){
	    hvEta=4;
	  }
	  else if( ieta >=256 && ieta < 320 ){
	    hvEta=5;
	  }
	  else if( ieta >=320 && ieta < 384 ){
	    hvEta=6;
	  }
	  else if( ieta >=384 && ieta < 448 ){
	    hvEta=7;
	  }
	}
	else if( region == 1){
	  hvEta=7;
	}
	else {
	  msg(MSG::WARNING) << "==> Invalid HV Eta-Sector for sampling=" << sampling << " Region=" << region << " I(eta)="<< ieta << endmsg;
	}
      } 
    else if( sampling == 2 ){
      // MIDDLE Sampling in Barrel
      // ----------------------
      // 
      if( region == 0 ){
	if( ieta >= 0 && ieta < 8 ){
	  hvEta=1;
	} 
	else if( ieta >=8 && ieta < 16 ){
	  hvEta=2;
	}
	else if( ieta >=16 && ieta < 24 ){
	  hvEta=3;
	}
	else if( ieta >=24 && ieta < 32 ){
	  hvEta=4;
	}
	else if( ieta >=32 && ieta < 40 ){
	  hvEta=5;
	}
	else if( ieta >=40 && ieta < 48 ){
	  hvEta=6;
	}
	else if( ieta >=48 && ieta < 57 ){
	  hvEta=7;
	}
      }
      else if( region == 1)
	{
	  hvEta=7; // to be checked with Francesco
	  
	}
      else{
	msg(MSG::WARNING) << "==> Invalid HV Eta-Sector for sampling=" << sampling << " Region=" << region << " I(eta)="<< ieta << endmsg; 
      }
    }
    else if( sampling == 3 ){
      // BACK Sampling in Barrel
      // -----------------------
      //
      if( region == 0 ){
	if( ieta >= 0 && ieta < 4 ){
	  hvEta=1;
	} 
	else if( ieta >=4 && ieta < 8 ){
	  hvEta=2;
	}
	else if( ieta >=8 && ieta < 12 ){
	  hvEta=3;
	}
	else if( ieta >=12 && ieta < 16 ){
	  hvEta=4;
	}
	else if( ieta >=16 && ieta < 20 ){
	  hvEta=5;
	}
	else if( ieta >=20 && ieta < 24 ){
	  hvEta=6;
	}
	else if( ieta >=24 && ieta < 28 ){
	  hvEta=7;
	}
      }
      else if( region == 1){
	msg(MSG::WARNING) << "==> Invalid HV Eta-Sector for sampling=" << sampling << " Region=" << region << " I(eta)="<< ieta << endmsg; 
      }
      else{
	msg(MSG::WARNING) << "==> Invalid HV Eta-Sector for sampling=" << sampling << " Region=" << region << " I(eta)="<< ieta << endmsg; 
      }
    }
    
    else if( sampling == 0 ){ 
      // PreSampler Sampling in Barrel
      // -----------------------------
      if( region == 0 ){
	// OKed by JY Hostachy
	if( ieta <= 60 ){
	  if( ieta >= 0 && ieta < 15 ){
	    hvEta=1;
	  } 
	  else if( ieta >=15 && ieta <30  ){
	    hvEta=2;
	  }
	  else if( ieta >=30 && ieta < 45 ){
	    hvEta=3;
	  }
	  else if( ieta >=45 && ieta < 61 ){
	    hvEta=4;
	  }
	}
	else{
	  msg(MSG::WARNING) << "Not in EMBPS..i(eta)=" << ieta << endmsg;
 	}
      }
    }
    else {
      msg(MSG::WARNING) << "Not in Barrel ..." << endmsg;
    }
  }
  else if( abs(l_bec) == 2 || abs(l_bec) == 3){ 
    // ENDCAP EMEC 
    // -----------
    //
    if( sampling == 3 ){ 
      // Sampling 3 OUTER Wheel
      // -----------------------
      // Validated on NOV-25-2004 with Djama (CPPM)
      // 
      if( ieta >= 0 && ieta <= 1){
	hvEta=1;
      }
      else if(ieta >=2 && ieta <= 5){
	hvEta=2;
      }
      else if(ieta >=6 && ieta <= 9){
	hvEta=3;
      }
      else if(ieta >=10 && ieta <= 11){
	hvEta=4;
      }
      else if(ieta >=12 && ieta <= 15){
	hvEta=5;
      }
      else if(ieta >=16 && ieta <= 19){
	hvEta=6;
      }
    }
    else if( sampling == 2 && abs(l_bec) == 2 ){
      // Sampling 2 OUTER Wheel 
      // ----------------------
      //
      // Validated on NOV-25-2004 with Djama (CPPM)
      // 
      if(region == 0){
	hvEta =0;
      }
      else if( region == 1 ){
	// region 1 Sampling 2
	if( ieta >=0 && ieta <= 2){
	  hvEta=0;
	}
	else if( ieta >=3 && ieta <= 6){
	  hvEta=1;
	}
	else if( ieta >=7 && ieta <= 15){
	  hvEta=2;
	}
	else if( ieta >=16 && ieta <= 23){
	  hvEta=3;
	}
	else if( ieta >=24 && ieta <= 27){
	  hvEta=4;
	}
	else if( ieta >=28 && ieta <= 35){
	  hvEta=5;
	}
	else if( ieta >=36 && ieta <= 42){
	  hvEta=6;
	}
      }
    }

    else if( sampling == 2 && abs(l_bec) == 3 ){
      // Sampling 2 INNER Wheel 
      // -----------------------
      // 
      // Validated on NOV-25-2004 with Djama (CPPM)
      // 
	if( ieta >=0 && ieta <= 2){
	  hvEta=7;
	}
	else if( ieta >=3 && ieta <= 6){
	  hvEta=8;
	}
    }

    else if( sampling == 1 && abs(l_bec) == 3 ){ 
      // Sampling 1 INNER Wheel 
      // -----------------------
      // 
      // Validated on NOV-25-2004 with Djama (CPPM)
      // 
	if( ieta >=0 && ieta <= 2){
	  hvEta=7;
	}
	else if( ieta >=3 && ieta <= 6){
	  hvEta=8;
	}
    }

    else if( sampling == 1 && abs(l_bec) == 2 ){
      // Sampling 1 OUTER Wheel 
      // -----------------------
      // 
      if( region == 0 || region == 1)
	{// 1.375 < eta < 1.5
	  hvEta=0;
	}
      else if( region == 2 ){
	if( ieta >= 0 && ieta <= 31)
	  {
	    // 1.5 < eta < 1.6
	    hvEta = 1;
	  }
	else if( ieta <= 95) {
	  // 1.6 < eta < 1.8
	  hvEta = 2;
	}
      }
      else if(region == 3){
	// 1.8 < eta < 2.0
	hvEta = 3;
      }
      else if(region == 4){
	// 2.0 < eta < 2.4 
	if( ieta >=0 && ieta <= 15 ){
	  // 2.0 < eta < 2.1 
	  hvEta = 4;
	}
	else if( ieta >= 16 && ieta <= 47 ){
	  // 2.1 < eta < 2.3 
	  hvEta = 5;
	}
	else if( ieta >= 48 && ieta <= 63 ){
	  // 2.3 < eta < 2.4
	  hvEta = 6;
	}
      }
      else if( region == 5 ){
	// 2.4 < eta < 2.5 
	hvEta = 6;
      }
    }
  }


  else if( abs(l_bec)== 20 ){
    // HEC Wheels
    //-----------
    if( sampling == 0 ){
      hvEta = 1;
    }
    else if( sampling == 1){
      hvEta = 2;
    }
    else if( sampling == 2){
      hvEta = 3;
    }
    else if( sampling == 3){
      hvEta = 4;
    }
  }
  // ============================
  // FCAL eta bins
  // eta = [0,62] in Module 1
  // eta = [0,31] in Module 2 
  // eta = [0,15] in Module 3
  // ============================
  else if( abs(l_bec)== 40 ){
    // FCAL Wheels
    //-----------
    if( sampling == 1 ){
      hvEta = 1;
    }
    else if( sampling == 2 ){
      hvEta = 2;
    }
    else if( sampling == 3 ){
      hvEta = 3;
    }
    else{
      msg(MSG::WARNING) 
	  << "[getCellEtaSector] invalid FCAL input cell.." << endmsg;
    }
  }
  else{
    msg(MSG::WARNING) << "[getCellModule] Detector unknown ! " << endmsg;
  }
  // Output 
  ATH_MSG_DEBUG("Closing getCellEtaSector ...." );
  return hvEta;
}




//=====================================================================
int LArHVCablingTool::getCellPhiSector( const Identifier &offId )
//=====================================================================
// Returns the phi-sector used number for a given cell 
//
// 23-JAN-2007 AL change of convention for EMB :
//   now returns hv_phi = 0 for FT=-1 (prev. 1) 
//               hv_phi = 1 for FT= 0 (prev. 0)
//
// 09-FEB-2005 AL fixed for H8 (module A00 rotated)
// 09-DEC-2004 AL created from getLArFeefdthrough(int,int,int,int)
//     - Added H8-EMBPS returning FT=0
//     - Corrected for reversed order for C-Wheel 
// 16-DEC-2004 AL added HEC and H6-HEC (which returns 0)
// 14-JAN-2005 AL added H6-EMEC
//     - Added H6-EMEC returning FT=0 
//=====================================================================
{// Start of getCellPhiSector()
  ATH_MSG_VERBOSE("Entering getCellPhiSector ...." );
  //
  // Get relevant offline fields
  //
  int bec      =99;
  int sampling=-99;
  int region  =-99;
  int phi     =-99;

  if( m_caloHelper->is_em( offId ) ){
    bec      = m_emHelper->barrel_ec(offId);
    sampling = m_emHelper->sampling(offId);
    region   = m_emHelper->region(offId);
    phi      = m_emHelper->phi(offId);
  }
  else if( m_caloHelper->is_hec( offId )  ){
    bec      = m_hecHelper->pos_neg(offId)*10; 
  }
  else if( m_caloHelper->is_fcal( offId )  ){
    bec      = m_fcalHelper->pos_neg(offId)*20; // ie: +/-40
  }
  // Output Argument
  int hvPhi=-2;
  //
  //*** Note*** : 
  //
  // For EMB, Feedthrough (FT) takes value -1 and 0
  // Instead, we encode these values as:
  // FT = -1 --> hvPhiSector = 1
  // FT =  0 --> hvPhiSector = 0
  // 
  if( abs(bec) == 1){
    // EMBARREL
    //----------
    if( bec == 1){
      // WHEEL-A
      //--------
      if( sampling == 0 ){
	// ------------------------------
	// EMBPS : phiSector not relevant
	// ------------------------------
	hvPhi = 0;
      }
      if( sampling == 1 && region == 0 ){
	// --------------------------------- 
	// EM1 Region 0: phi-cells in [0,63]
	// --------------------------------- 
	//
	int phi_local = int(phi);
	for(int ift=0; ift<= 15; ift++){
	  if( phi_local == ift*4 || phi_local == 1+ift*4 ){
	    hvPhi = 0;
	  }
	  if( phi_local == 2+ift*4 || phi_local == 3+ift*4 ){
	    hvPhi = 1;
	  }
	}
      }
      else if( sampling == 2 || sampling == 3 || (sampling == 1 && region == 1 ) ){
	// ----------------------------------------------
	// FRONT Region 1 + MIDDLE + BACK Phi in [0,255]
	// ----------------------------------------------
	int phi_local = int(phi);
	for( int ift=0; ift<= 15; ift++){
	  for( int i=0; i<=7; i++){
 	    if( phi_local == i+ift*16 ){
	      hvPhi = 0;
	    }
	  }
	  for( int i=8; i<=15; i++){
	    if( phi_local == i+ift*16 ){
	      hvPhi = 1;
	    }
	  }
	}
      }
    }

    else if( bec == -1){
      // WHEEL-C
      //--------
      if( sampling == 0 ){
	// ------------------------------
	// EMBPS : phiSector not relevant
	// ------------------------------
	hvPhi = 0;
      }
      if( sampling == 1 && region == 0 ){
	// ---------------------------------------
	// FRONT (Region 0): phi-cells in [0,63]
	// --------------------------------------- 
	int phi_local = int(phi);
	for(int ift=0; ift<= 15; ift++){
	  if( phi_local == ift*4 || phi_local == 1+ift*4 ){
	    hvPhi = 1;
	  }
	  if( phi_local == 2+ift*4 || phi_local == 3+ift*4 ){
	    hvPhi = 0;
	  }
	}
      }
      else if( sampling == 2 || sampling == 3 || (sampling == 1 && region == 1 ) ){
	// FRONT Region 1 + MIDDLE + BACK Phi in [0,255]
	int phi_local = int(phi);
	for( int ift=0; ift<= 15; ift++){
	  for( int i=0; i<=7; i++){
	    if( phi_local == i+ift*16 ){
	      hvPhi = 1;
	    }
	  }
	  for( int i=8; i<=15; i++){
	    if( phi_local == i+ift*16 ){
	      hvPhi = 0;
	    }
	  }
	}
      }
    }    
  }
  
  else if( abs(bec) == 2 || abs(bec) == 3 ){
    // EMEC
    if( bec > 0 ){
      // WHEEL-A
      if( sampling == 1 && abs(bec) == 2 ){
	// ====================================
	// Sampling 1 OUTER Wheel 
	// =====================================
	// 64 cells per SIDE
	//  8 Modules per side
	//  --> 8 cells / module
	//  4 FT per module [1,4]
	//  --> 2 cells / FT
	// =====================================
	int imod=int(phi/8);
	int phi_local = phi - imod*8;
	if( phi_local == 0 || phi_local == 1){
	  hvPhi = 1;
	}
	else if( phi_local == 2 || phi_local == 3){
	  hvPhi = 2;
	}
	else if( phi_local == 4 || phi_local == 5){
	  hvPhi = 3;
	}
	else if( phi_local == 6 || phi_local == 7){
	  hvPhi = 4;
	}
      }
      if( sampling == 1 && abs(bec) == 3 ){
	// Sampling 1 INNER Wheel (FT in [1,8])
	// ------------------------------------
	// 
	int imod=int(phi/8);
	int phi_local = phi-imod*8;
	hvPhi = phi_local+1;
      }
      else if( sampling == 2 && abs(bec) == 3 ){
	//
	// Sampling 2 Inner Wheel (FT in [1,8])
	// ------------------------------------
	//
	int imod=int(phi/8);
	int phi_local = phi - imod*8;
	hvPhi = phi_local+1;
      }
      else if( sampling == 2 && abs(bec) == 2 ){
	//
	// Sampling 2 Outer Wheel (FT in [1,4])
	// ------------------------------------
	//
	int imod = (phi/32);
	int phi_local= phi-imod*32;
	if( phi_local >= 0 && phi_local <= 7 ){
	  hvPhi = 1;
	}
	else if( phi_local >= 8 && phi_local <= 15 ){
	  hvPhi = 2;
	}
	else if( phi_local >= 16 && phi_local <= 23 ){
	  hvPhi = 3;
	}
	else if( phi_local >= 24 && phi_local <= 31 ){
	  hvPhi = 4;
	}
      }
      else if( sampling == 3 ){
	//
	// Sampling 3 Outer Wheel (FT in [1,4])
	// ------------------------------------
	// 
	int imod = (phi/32);
	int phi_local= phi-imod*32;
	if( phi_local >= 0 && phi_local <= 7 ){
	  hvPhi = 1;
	}
	else if( phi_local >= 8 && phi_local <= 15 ){
	  hvPhi = 2;
	}
	else if( phi_local >= 16 && phi_local <= 23 ){
	  hvPhi = 3;
	}
	else if( phi_local >= 24 && phi_local <= 31 ){
	  hvPhi = 4;
	}
      }
    }
    else if( bec < 0 ){
      // WHEEL-C
      if( sampling == 1 && abs(bec) == 2 ){
	// Sampling 1 OUTER Wheel (FT in [1,4])
	// ------------------------------------
	int imod=int(phi/8);
	int phi_local = phi-imod*8;
	if( phi_local == 0 || phi_local == 1){
	  hvPhi = 4;
	}
	else if( phi_local == 2 || phi_local == 3){
	  hvPhi = 3;
	}
	else if( phi_local == 4 || phi_local == 5){
	  hvPhi = 2;
	}
	else if( phi_local == 6 || phi_local == 7){
	  hvPhi = 1;
	}
      }
      if( sampling == 1 && abs(bec) == 3 ){
	// Sampling 1 INNER Wheel (FT in [1,8])
	// ------------------------------------
	// 
	int imod=int(phi/8);
	int phi_local = phi - imod*8;
	hvPhi = 8-(phi_local);
      }
      else if( sampling == 2 && abs(bec) == 3 ){
	// Sampling 2 Inner Wheel (FT in [1,8])
	// ------------------------------------
	int imod=int(phi/8);
	int phi_local = phi-imod*8;
	hvPhi = 8-(phi_local);
      }
      else if( sampling == 2 && abs(bec) == 2 ){
	// Sampling 2 Outer Wheel (FT in [1,4])
	// ------------------------------------
	int imod = (phi/32);
	int phi_local= phi-imod*32;
	if( phi_local >= 0 && phi_local <= 7 ){
	  hvPhi = 4;
	}
	else if( phi_local >= 8 && phi_local <= 15 ){
	  hvPhi = 3;
	}
	else if( phi_local >= 16 && phi_local <= 23 ){
	  hvPhi = 2;
	}
	else if( phi_local >= 24 && phi_local <= 31 ){
	  hvPhi = 1;
	}
      }
      else if( sampling == 3 ){
	// Sampling 3 Outer Wheel (FT in [1,4])
	// ------------------------------------
	int imod = (phi/32);
	int phi_local= phi-imod*32;
	if( phi_local >= 0 && phi_local <= 7 ){
	  hvPhi = 4;
	}
	else if( phi_local >= 8 && phi_local <= 15 ){
	  hvPhi = 3;
	}
	else if( phi_local >= 16 && phi_local <= 23 ){
	  hvPhi = 2;
	}
	else if( phi_local >= 24 && phi_local <= 31 ){
	  hvPhi = 1;
	}
      }
    }
  }

  else if( abs(bec)==20){
    // HEC detector
    //------------
    // Note: not used because we use the 32 modules instead
    hvPhi = 0;
  }  
  else if( abs(bec)==40){
    // FCAL detector
    //------------
    // Note: not used 
    hvPhi = 0;
  }
  ATH_MSG_DEBUG("Closing getCellPhiSector ...." );
  return hvPhi;
}





//============================================================================
std::pair<int,int> LArHVCablingTool::getCellElectrodeMinMax( const Identifier &offId )
//============================================================================
// Returns a pair <1st_electrode,last_electrode> of a given calo cell 
//
// 12-FEB-2007 : AL fixed a bug in HEC 
// 02-FEB-2007 : AL added FCAL info *null* (see Zhaoyu)
// 01-FEB-2007 : AL modified HEC info according to TDR 96-41
//               *te be checked by an expert*
// 26-JAN-2007 : AL fixed electrode Min,Max for EMBARREL and EMEC
// 
// 09-FEB-2005 : AL fixed the H8 inversion problem (rotation of Module 00)
// 07-DEC-2004 : AL fixed A-Wheel (phi_local vs phi_atlas)
//                  fixed for Reversed order for Barrel C-Wheel 
// 15-DEC-2004 : AL added HEC detector
//
//============================================================================ 
{// get getCell1stElectrode start  
  ATH_MSG_VERBOSE("[getCellElectrodeMinMax] Entering..." );

  int l_bec=99;
  int l_sampling=-99;
  int l_region=-99;
  //int l_eta=-99;
  int l_phi=-99;
 
  if( m_caloHelper->is_em( offId ) ){
    l_bec      = m_emHelper->barrel_ec(offId);
    l_sampling = m_emHelper->sampling(offId);
    l_region   = m_emHelper->region(offId);
    //l_eta      = m_emHelper->eta(offId);
    l_phi      = m_emHelper->phi(offId);
  }
  else if( m_caloHelper->is_hec( offId )  ){
    l_bec      = m_hecHelper->pos_neg(offId)*10; // +-20
    l_sampling = m_hecHelper->sampling(offId);
    l_region   = m_hecHelper->region(offId);
    //l_eta      = m_hecHelper->eta(offId);
    l_phi      = m_hecHelper->phi(offId);
  }
  else if( m_caloHelper->is_fcal( offId )  ){
    l_bec      = m_fcalHelper->pos_neg(offId)*20; // +-40 
  }
  // Output argument
  // ===============
  int ElecMin=-99;
  int ElecMax=-99;
  if( abs(l_bec) == 1 )
    {// if in BARREL
      if( l_bec == 1 ) {
	// EMBARREL SIDE A 
	if( l_sampling == 0 || ( l_sampling == 1 && l_region == 0) ){
	  // ========================================
	  // - 64 cells-in-phi per SIDE
	  // -  4 cells-in-phi per MODULE
	  // - 64 electrodes-in-phi per MODULE
	  //   --> 4 cells of 16 electrodes per CELL
	  // Examples in *ATLAS* (not H8)
	  // --> Cell #0 ==> MOD A01 FT=-1 ELEC 0-15
	  // --> Cell #1 ==> MOD A01 FT=-1 ELEC 16-31
	  // --> Cell #2 ==> MOD A01 FT=0  ELEC 32-47
	  // --> Cell #3 ==> MOD A01 FT=0  ELEC 48-64
	  // ========================================
	  int phi_module = int((l_phi+2)/4);
	  int phi_local = l_phi+2 - 4*phi_module;
	  ElecMin = phi_local*16;
	  ElecMax = ElecMin + 16-1;
	  ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMB"
			<< " Side A "    << l_bec
			<< " Sampling= " << l_sampling 
			<< " Region= " << l_region
			<< " Module= A" << phi_module
			<< " Phi/Module= "       << phi_local 
			<< " Electrode=[" << ElecMin << "." << ElecMax << "]"
			);
	  if( l_sampling == 0 ){
	    ElecMin = 0;
	    ElecMax = 0;
	  }

	}
	else if( (l_sampling == 1 && l_region == 1) || l_sampling == 2 || l_sampling == 3 ){
	  // ==========================================
	  // - 256 cells-in-phi per SIDE
	  // -  16 cells-in-phi per MODULE
	  // -  64 electrode's planes per MODULE
	  //   --> 16 cells of 4 electrodes per CELL
	  // ==========================================
	  // --> Cell #0 ==> MOD A01 FT=-1  ELEC 00-03
	  // --> Cell #1 ==> MOD A01 FT=-1  ELEC 04-07
	  // --> Cell #2 ==> MOD A01 FT=-1  ELEC 08-11
	  // --> Cell #3 ==> MOD A01 FT=-1  ELEC 12-15
	  // --> Cell #4 ==> MOD A01 FT=-1  ELEC 16-19
	  // --> Cell #5 ==> MOD A01 FT=-1  ELEC 20-23
	  // --> Cell #6 ==> MOD A01 FT=-1  ELEC 24-27
	  // --> Cell #7 ==> MOD A01 FT=-1  ELEC 28-31
	  // --> Cell #8 ==> MOD A01 FT=0   ELEC 32-35
	  // --> Cell #9 ==> MOD A01 FT=0   ELEC 36-39
	  // --> Cell#10 ==> MOD A01 FT=0   ELEC 40-43
	  // --> Cell#11 ==> MOD A01 FT=0   ELEC 44-47
	  // --> Cell#12 ==> MOD A01 FT=0   ELEC 48-51
	  // --> Cell#13 ==> MOD A01 FT=0   ELEC 52-55
	  // --> Cell#14 ==> MOD A01 FT=0   ELEC 56-59
	  // --> Cell#15 ==> MOD A01 FT=0   ELEC 60-63
	  // ==========================================
	  int phi_module = int((l_phi+8)/16);
	  // shift of 8 cells because Module #0 is rotated by half-module 
	  int phi_local  = l_phi+8 - 16*phi_module;
	  ElecMin = phi_local * 4;   
	  ElecMax = ElecMin + 4 - 1;
	  ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMB"
			<< " Side A "    << l_bec
			<< " Sampling= " << l_sampling 
			<< " Region= "   << l_region
			<< " Phi/Module= "       << phi_local 
			<< " Module= A"  << phi_module
			<< " Electrode=[" << ElecMin << "." << ElecMax << "]"
			);
	}
      }

      else if( l_bec == -1){
	//=====================
	//=====================
	//   EMBARREL SIDE C
	//=====================
	//=====================
	// Phi in module is reversed going from left to right
	//     in the MODULE frame
	if( l_sampling == 0 || ( l_sampling == 1 && l_region == 0) ){
	  // ========================================
	  // SAMPLING 0
	  // - 64 cells-in-phi per DETECTOR
	  // -  4 cells-in-phi per MODULE
	  // - 64 electrodes-in-phi per MODULE
	  //   --> 4 cells of 16 electrodes per CELL
	  // ========================================
	  // --> Cell #0 ==> MOD C01 FT=-1 ELEC 0-15
	  // --> Cell #1 ==> MOD C01 FT=-1 ELEC 16-31
	  // --> Cell #2 ==> MOD C01 FT=0  ELEC 32-47
	  // --> Cell #3 ==> MOD C01 FT=0  ELEC 48-64
	  // ========================================
	  int phi_module = int((l_phi+2)/4);
	  int phi_local = (l_phi+2 - 4*phi_module) ;
	  ElecMin = 48-phi_local*16;
	  ElecMax = ElecMin + 16 - 1;
	  ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMB"
			<< " Side C "    << l_bec
			<< " Sampling= " << l_sampling 
			<< " Region= " << l_region
			<< " Module= C" << phi_module
			<< " Phi/Module= "       << phi_local 
			<< " Electrode=[" << ElecMin << "." << ElecMax << "]"
			);
	  if( l_sampling == 0 ){
	    ElecMin = 0;
	    ElecMax = 0;
	  }
	}
	else if( (l_sampling == 1 && l_region == 1) || l_sampling == 2 || l_sampling == 3 ){
	  //===========================================
	  // - 256 cells-in-phi per SIDE
	  // -  16 cells-in-phi per MODULE
	  // -  64 electrodes-in-phi per MODULE
	  //   --> 16 cells of 4 electrodes per CELL
	  // Cell[electrodes]: 0[0,3],1[4,7],2[8,11],3[12,15]..etc...
	  //===========================================
	  // --> Cell #0 ==> MOD C01 FT=-1  ELEC 00-03
	  // --> Cell #1 ==> MOD C01 FT=-1  ELEC 04-07
	  // --> Cell #2 ==> MOD C01 FT=-1  ELEC 08-11
	  // --> Cell #3 ==> MOD C01 FT=-1  ELEC 12-15
	  // --> Cell #4 ==> MOD C01 FT=-1  ELEC 16-19
	  // --> Cell #5 ==> MOD C01 FT=-1  ELEC 20-23
	  // --> Cell #6 ==> MOD C01 FT=-1  ELEC 24-27
	  // --> Cell #7 ==> MOD C01 FT=-1  ELEC 28-31
	  // --> Cell #8 ==> MOD C01 FT=0   ELEC 32-35
	  // --> Cell #9 ==> MOD C01 FT=0   ELEC 36-39
	  // --> Cell#10 ==> MOD C01 FT=0   ELEC 40-43
	  // --> Cell#11 ==> MOD C01 FT=0   ELEC 44-47
	  // --> Cell#12 ==> MOD C01 FT=0   ELEC 48-51
	  // --> Cell#13 ==> MOD C01 FT=0   ELEC 52-55
	  // --> Cell#14 ==> MOD C01 FT=0   ELEC 56-59
	  // --> Cell#15 ==> MOD C01 FT=0   ELEC 60-63
	  //===========================================
	  int phi_module = int((l_phi+8)/16);
	  int phi_local  = (l_phi+8 - 16*phi_module);
	  // Just to get the right MOD id (electrode plane is idem to A-side)
	  // Do not use to compute local Phi
	  if( phi_module <= 8){
	    phi_module = 8 - phi_module;
	  }
	  else{
	    phi_module = 24 - phi_module;
	  }
	  ElecMin = 60-phi_local * 4;   
	  ElecMax = ElecMin + 4 - 1;
	  ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMB"
			<< " Side C "    << l_bec
			<< " Sampling= " << l_sampling 
			<< " Region= " << l_region
			<< " Module= C" << phi_module
			<< " Phi/Module= "       << phi_local 
			<< " Electrode=[" << ElecMin << "." << ElecMax << "]"
			);
	}
      }
    }
  // ======================================
  // ======================================
  //     EM ENDCAP (A+C SIDES)
  // ======================================
  // ======================================
  else if(abs(l_bec) == 2 || abs(l_bec) == 3){
    // EMEC 
    if( l_bec == 2 ){// bec==2
      //======================
      // A-SIDE / OUTER WHEEL
      //======================
      if( l_sampling == 0 || 
	  (l_sampling == 1 && l_region >= 0 && l_region <= 5) ){
	//=============================
	//  64 cells per SIDE
	//   8 Modules per SIDE
	//  -> 8 cells / module
	// 768 electrodes for 64 cells
	//  -> 96 electrodes per module 
	//  -> 12 electrodes per cell
	//=============================
	// --> Cell #0 ==> ELEC 00-11
	// --> Cell #1 ==> ELEC 12-23
	// --> Cell #2 ==> ELEC 24-35
	// --> Cell #3 ==> ELEC 36-47
	// --> Cell #4 ==> ELEC 48-59
	// --> Cell #5 ==> ELEC 60-71
	// --> Cell #6 ==> ELEC 72-83
	// --> Cell #7 ==> ELEC 84-95
	//=============================
	int phi_module = int(l_phi/8);
	int phi_local  = l_phi - 8*phi_module;
	ElecMin = phi_local * 12;
	ElecMax = ElecMin + 12 - 1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side A "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= A" << phi_module
		      << " FT= "       << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );

      }
      else if( l_sampling == 2 || l_sampling == 3){
	//=============================
	// 256 cells per SIDE
	//   8 Modules per SIDE
	// --> 32 cells per module
	// 768 electrodes for 256 cells
	// --> 96 electrodes per module
	// --> 3 electrodes per cell
	//=============================
	// --> Cell #0 ==> ELEC 00-02
	// --> Cell #1 ==> ELEC 03-05
	// --> Cell #2 ==> ELEC 06-08
	// ...
	// --> Cell #31 => ELEC 92-95 
	//=============================
	int phi_module = int(l_phi/32);
	int phi_local  = l_phi - 32*phi_module;
	ElecMin = phi_local*3;
	ElecMax = ElecMin+3-1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side A "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= A" << phi_module
		      << " Phi/Module= "  << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );

      }
    }
    else if( l_bec == 3 ){
      // =================
      // INNER WHEEL
      // =================
      if( l_sampling == 1 || l_sampling == 2 || l_sampling == 0 ){
	//====================================
	//  64 cells per SIDE
	//   8 module per SIDE
	//  -> 8 cells per Module
	// 256 electrodes for 8 modules
	// --> 32 electrodes per module
	// -->  4 electrodes per cell
	//====================================
	// --> Cell #0 ==> ELEC 00-03
	// --> Cell #1 ==> ELEC 04-07
	// --> Cell #2 ==> ELEC 08-11
	// ...
	// --> Cell #7 ==> ELEC 28-31
	//====================================
	int phi_module = int(l_phi/8);
	int phi_local  = l_phi - 8*phi_module;
	ElecMin = phi_local *4;	
	ElecMax = ElecMin + 4 - 1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side A "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= A" << phi_module
		      << " Phi/Module= "       << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );

      }
    }
    else if( l_bec == -2 ){
      //========================
      // C-SIDE OUTER WHEEL 
      //========================
      if( l_sampling == 0 || (l_sampling == 1 && l_region >= 0 && l_region <= 5) ){
	//=============================
	//  64 cells per SIDE
	//   8 Modules per SIDE
	//  -> 8 cells / module
	// 768 electrodes for 64 cells
	//  -> 96 electrodes per module 
	//  -> 12 electrodes per cell
	//=============================
	// --> Cell #0 ==> ELEC 00-11
	// --> Cell #1 ==> ELEC 12-23
	// --> Cell #2 ==> ELEC 24-35
	// --> Cell #3 ==> ELEC 36-47
	// --> Cell #4 ==> ELEC 48-59
	// --> Cell #5 ==> ELEC 60-71
	// --> Cell #6 ==> ELEC 72-83
	// --> Cell #7 ==> ELEC 84-95
	//=============================
	int phi_module = int(l_phi/8);
	int phi_local  = 7 - (l_phi - 8*phi_module);
	ElecMin = phi_local * 12;
	ElecMax = ElecMin + 12 - 1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side C "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= C" << phi_module
		      << " Phi/Module= "       << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );

      }
      else if( l_sampling == 2 || l_sampling == 3){
	//=============================
	// 256 cells per SIDE
	//   8 Modules per SIDE
	// --> 32 cells per module
	// 768 electrodes for 256 cells
	// --> 96 electrodes per module
	// --> 3 electrodes per cell
	//=============================
	// --> Cell #0 ==> ELEC 00-02
	// --> Cell #1 ==> ELEC 03-05
	// --> Cell #2 ==> ELEC 06-08
	// ...
	// --> Cell #31 => ELEC 92-95 
	//=============================
	int phi_module = int(l_phi/32);
	int phi_local  = 31 - (l_phi - 32*phi_module);
	ElecMin = phi_local*3;
	ElecMax = ElecMin + 3 - 1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side C "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= C" << phi_module
		      << " Phi/Module= "       << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );
      }
    }
    else if( l_bec == -3 ){
      // INNER WHEEL C
      if( l_sampling == 1 || l_sampling == 2 || l_sampling == 0 ){
	// 256 electrodes for 64 cells
	// -->  8 cells per module
	// --> 32 electrodes per module
	// -->  4 electrodes per cell
	int phi_module = int(l_phi/8);
	int phi_local  = l_phi - 8*phi_module;
	ElecMin = 28 - phi_local *4;	
	ElecMax = ElecMin + 4 -1;
	ATH_MSG_DEBUG("[getCellElectrodeMinMax] EMEC"
		      << " Side C "    << l_bec
		      << " Sampling= " << l_sampling 
		      << " Region= " << l_region
		      << " Module= C" << phi_module
		      << " Phi/Module= "       << phi_local 
		      << " Electrode=[" << ElecMin << "." << ElecMax << "]"
		      );
      }
    }
  }
  else if( abs(l_bec)==20 ){// HEC
    //==========================
    // HEC wheels
    //==========================
    // Sampling 0 & 1 : 1st wheel
    // Sampling 2 & 3 : 2nd wheel
    // l_region 0       : Outer
    // region 1       : Inner
    //==========================
    if( l_sampling == 0){
      // ======================
      // Sampling 0 , region 0 
      //   * 1st wheel * 
      // 64 cells per SIDE
      // 32 module per SIDE
      // -> 2 cells in phi per Module
      //  8 electrode's planes 
      // -> 8 electrode's planes per cell !
      // ================================
      // Made using Fig. 8-1 in TDR 96-41
      // ================================
      //ElecMin = l_eta+1;
      //ElecMax = ElecMin + 4;
      ElecMin = 1;
      ElecMax = 8;
      // Note : idem for region 0 and region 1
    }
    else if( l_sampling == 1){
      // ====================
      // sampling 1 l_region 0
      //   * 1st wheel * 
      // 64 cells in phi per SIDE 
      // 10 cells in eta per SIDE
      // 32 modules per SIDE
      // -> 2x10 cells per module 
      // 16 electrode's planes per module 
      // ================================
      // Made using Fig. 8-1 in TDR 96-41
      // ================================
      ElecMin = 9;
      ElecMax =24; 
      // Note : idem for region 0 and region 1
    }
    else if( l_sampling == 2){
      // ====================
      // sampling 2 region 0
      //   * 2nd wheel * 
      // 64 cells in phi per SIDE 
      // 10 cells in eta per SIDE
      // 32 modules in phi per SIDE
      // 16 electrode's plane
      // ================================
      // Made using Fig. 8-1 in TDR 96-41
      // ================================
      // Note : idem for region 0 and region 1
      ElecMin = 25;
      ElecMax = 32;
    }
    else if( l_sampling == 3){
      // ==============================
      // index starting at 2 thus the 32-2=20
      // 2 cells in phi per module 
      // 8 electrode per module 
      // ie: 4 electrodes per cell
      // ==============================
      // Note : idem for region 0 and region 1
      ElecMin = 33;
      ElecMax = 40;
    }
  } // HEC
  else if( abs(l_bec)==40 ){// FCAL
    ElecMin = 0;
    ElecMax = 0;
  }
  else{
    msg(MSG::WARNING) 
	<< "[getCellElectrodeMinMax] Detector unknown .." 
	<< endmsg; 
  }
  ATH_MSG_DEBUG("[getCellElectrodeMinMax] Exiting..." );
  std::pair<int, int> electrodeMinMax( ElecMin, ElecMax); 
  return electrodeMinMax;
}



StatusCode LArHVCablingTool::iovCallback(IOVSVC_CALLBACK_ARGS) {
  m_initHVMap=false;
  return StatusCode::SUCCESS;
}
