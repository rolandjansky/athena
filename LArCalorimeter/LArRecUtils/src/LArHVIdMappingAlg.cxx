/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVIdMappingAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"

#include "PathResolver/PathResolver.h"

#include <fstream>

StatusCode LArHVIdMappingAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode LArHVIdMappingAlg::execute() {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArHVIdMapping> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_readKey};
  const AthenaAttributeList* attr{*readHandle};
  writeHandle.addDependency(readHandle);
  // Reading of input conditions data, copied from LArHVCablingTool
  if (attr==nullptr) {
    ATH_MSG_WARNING( " Cannot find /LAR/IdentifierOfl/HVLineToElectrodeMap from database, Use ASCII file indeed !!!");
    std::string tablename=PathResolver::find_file ("HVLineToElectrode.data", "DATAPATH");
    if (tablename == "") {
      ATH_MSG_FATAL( "[fillHVMap] Could not locate HVLineToElectrode.data file as well");
      return StatusCode::FAILURE;
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
    m_infile.str(buffer);
    delete [] buffer; 
  }
  else {
    if (!readHandle.isValid()) {
      ATH_MSG_ERROR( "Failed to retrieve AttributeList with key " << m_readKey.fullKey() << " from store" );
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( " Found AttrList from " << m_readKey.fullKey() );
    std::ostringstream attrStr1;
    coral::AttributeList::const_iterator itr=attr->begin();
    itr->toOutputStream(attrStr1);
    std::string::size_type pos = attrStr1.str().find(':');
    if (pos == std::string::npos)
      pos = 0;
    else
      pos++;
    std::string Line=attrStr1.str().substr(pos);
    ATH_MSG_DEBUG(" Line " << Line );
    m_infile.str(Line);
  }


  const CaloCell_ID* calocellID;
  ATH_CHECK(detStore()->retrieve(calocellID,"CaloCell_ID"));

  const LArHVLineID* hvlineID;
  ATH_CHECK(detStore()->retrieve(hvlineID,"LArHVLineID"));
  
  const LArElectrodeID* electrodeID;
  ATH_CHECK(detStore()->retrieve(electrodeID,"LArElectrodeID"));

  std::unique_ptr<LArHVIdMapping> HVIdMap = std::make_unique<LArHVIdMapping>(calocellID, hvlineID, electrodeID);
  std::map<HWIdentifier,HWIdentifier> &elecLineMap = HVIdMap->m_electrode_hvline_map; //basically a shorthand
  std::map<HWIdentifier,std::vector<HWIdentifier> > &lineElecMap = HVIdMap->m_hvline_electrode_map;

  fillHVMap(elecLineMap, lineElecMap, hvlineID, electrodeID);


  if(writeHandle.record(std::move(HVIdMap)).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCalibLineMapping object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

 
  return StatusCode::SUCCESS;
}

// This function is taken from LArHVCablingTool
//=================================================================================
void LArHVIdMappingAlg::fillHVMap(std::map<HWIdentifier,HWIdentifier> & elmap, std::map<HWIdentifier,std::vector<HWIdentifier> > & lemap, const LArHVLineID* hvlineHelper, const LArElectrodeID* electrodeHelper ) 
//=================================================================================
{// Start of fillHVMap()  
  ATH_MSG_DEBUG("[fillHVMap] entering fillHVMap()..." );

  elmap.clear();
  lemap.clear();
  
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
  while ( m_infile 
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
      HWIdentifier hvlineId = hvlineHelper->HVLineId(hvpartition,hvcanline,hvcannode,hvline);

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
	    electrodeHelper->ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,ielec);
	  //ATH_MSG_DEBUG("[fillHVMap] have defined electrodeId..." );
	  electrodeIdvec.push_back(electrodeId);
	  //ATH_MSG_DEBUG("[fillHVMap] push_back in electrodeId vector..." );

         // Check if electrode is already in map ?
	  elmap.insert(std::pair<HWIdentifier,HWIdentifier>(electrodeId,hvlineId));
	}
      }
      else{
	//msg(MSG::INFO) << "[fillHVMap] max < min STOP !!!" << endmsg;
	HWIdentifier electrodeId = electrodeHelper->ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,min);
	electrodeIdvec.push_back(electrodeId);

         // Check if electrode is already in map ?
        elmap.insert(std::pair<HWIdentifier,HWIdentifier>(electrodeId,hvlineId));
      }

      std::map<HWIdentifier,std::vector<HWIdentifier> >::iterator elecIt=lemap.find(hvlineId);
      if( elecIt != lemap.end()) {
           std::vector<HWIdentifier>* list1 = &((elecIt)->second);
           for (unsigned int ii=0;ii<electrodeIdvec.size();ii++) {
               list1->push_back(electrodeIdvec[ii]);
           }
      }
      else {
	  lemap.insert(std::pair<HWIdentifier,std::vector<HWIdentifier> >(hvlineId,electrodeIdvec));
      }
    }  
  ATH_MSG_DEBUG(" end reading infile " );
  ATH_MSG_DEBUG("[fillHVMap] Mapping electrode-to-hvline : number of Ids= "<<elmap.size());
  ATH_MSG_DEBUG("[fillHVMap] Mapping hvline-to-electrode : number of Ids= "<<lemap.size());
  //GU infile.close();
  ATH_MSG_DEBUG("[fillHVMap] exiting fillHVMap ... OK" );
  return;  
} 
