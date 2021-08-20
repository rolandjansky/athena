/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArCabling/src/LArHVIdMappingReader.cxx
 * @author scott snyder <snyder@bnl.gov>, from code originally in LArHVCablingTool.
 * @date Jun, 2021
 * @brief Helper to read HV ID mapping data from a file or conditions.
 */


#include "LArCabling/LArHVIdMappingReader.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "PathResolver/PathResolver.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaKernel/getMessageSvc.h"
#include <sstream>
#include <fstream>


LArHVIdMappingReader::LArHVIdMappingReader()
  : AthMessaging (Athena::getMessageSvc(), "LArHVIdMappingReader")
{
}


StatusCode LArHVIdMappingReader::read (const AthenaAttributeList* attrList,
                                       const LArHVLineID& hvlineHelper,
                                       const LArElectrodeID& electrodeHelper,
                                       hvlineMap_t& hvlineMap,
                                       electrodeMap_t& electrodeMap) const
{
  hvlineMap.clear();
  electrodeMap.clear();

  std::string content;
  if (attrList) {
    content = fromAttrList (*attrList);
  }
  else {
    content = fromFile();
  }

  if (content.empty()) {
    return StatusCode::FAILURE;
  }

  return fillHV (content,
                 hvlineHelper,
                 electrodeHelper,
                 hvlineMap,
                 electrodeMap);
}


std::string
LArHVIdMappingReader::fromAttrList (const AthenaAttributeList& attrList) const
{
  std::ostringstream attrStr1;
  coral::AttributeList::const_iterator itr = attrList.begin();
  itr->toOutputStream(attrStr1);
  std::string::size_type pos = attrStr1.str().find(":");
  if (pos == std::string::npos)
    pos = 0;
    else
      pos++;
  std::string Line = attrStr1.str().substr(pos);
  ATH_MSG_DEBUG(" Line " << Line );
  return Line;
}


std::string LArHVIdMappingReader::fromFile() const
{
  ATH_MSG_WARNING( " Cannot find /LAR/IdentifierOfl/HVLineToElectrodeMap from database, Use ASCII file indeed !!!");
  std::string tablename = PathResolver::find_file ("HVLineToElectrode.data", "DATAPATH");
  if (tablename == "") {
    ATH_MSG_FATAL( "[fillHVMap] Could not locate HVLineToElectrode.data file as well");
    return "";
  }
  else{
    ATH_MSG_DEBUG("[fillHVMap] have located HVLineToElectrode.data OK !");
  }
  const char * tablefile = tablename.c_str() ;
  std::ifstream myfile(tablefile);
  // get pointer to associated buffer object
  std::filebuf* pbuf = myfile.rdbuf();
  int size = pbuf->pubseekoff (0,std::ios::end,std::ios::in);
  pbuf->pubseekpos (0,std::ios::in);
  // allocate memory to contain file data
  std::string buffer;
  buffer.resize (size, ' ');
  // get file data  
  pbuf->sgetn (buffer.data(),size);
  myfile.close();
  return buffer;
}


StatusCode
LArHVIdMappingReader::fillHV (const std::string& content,
                              const LArHVLineID& hvlineHelper,
                              const LArElectrodeID& electrodeHelper,
                              hvlineMap_t& hvlineMap,
                              electrodeMap_t& electrodeMap) const
{
  ATH_MSG_DEBUG("[fillHVMap] entering fillHVMap()..." );

  hvlineMap.clear();
  electrodeMap.clear();

  std::istringstream infile;
  infile.str (content);
  
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
  while (infile 
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
    if (max > 1000) // Crude check against reading nonsense.
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
    HWIdentifier hvlineId = hvlineHelper.HVLineId(hvpartition,hvcanline,hvcannode,hvline);

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
          electrodeHelper.ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,ielec);
        electrodeIdvec.push_back(electrodeId);

        // Check if electrode is already in map ?
        hvlineMap.emplace (electrodeId, hvlineId);
      }
    }
    else{
      HWIdentifier electrodeId = electrodeHelper.ElectrodeId(detect,side,mod,hvphi,hveta,hvgap,min);
      electrodeIdvec.push_back(electrodeId);

      // Check if electrode is already in map ?
      hvlineMap.emplace (electrodeId, hvlineId);
    }

    std::vector<HWIdentifier>& l = electrodeMap[hvlineId];
    l.insert (l.end(), electrodeIdvec.begin(), electrodeIdvec.end());
  }

  ATH_MSG_DEBUG(" end reading infile " );
  ATH_MSG_DEBUG("[fillHV] Mapping electrode-to-hvline : number of Ids= "<<hvlineMap.size());
  ATH_MSG_DEBUG("[fillHV] Mapping hvline-to-electrode : number of Ids= "<<electrodeMap.size());
  ATH_MSG_DEBUG("[fillHV] exiting fillHVMap ... OK" );
  return StatusCode::SUCCESS;
}

