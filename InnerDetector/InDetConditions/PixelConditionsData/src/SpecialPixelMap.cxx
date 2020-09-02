/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/SpecialPixelMap.h"

// Athena/Gaudi
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "CoralBase/Blob.h"
#include "Identifier/IdentifierHash.h"

// standard library
#include<fstream>
#include<sstream>
#include<string>
#include<iomanip>


//***** DetectorSpecialPixelMap *****//
const int nmtype(5);
static const unsigned int columnsPerFEIX[5]={18,80,132,80,132}; // number of columns per FEI3, 4, 50, 51, 52 
//static unsigned int rowsPerFEIX[5]={164, 336, 672, 339, 678}; // number of rows per FEI3, 4, 50, 51, 52
//static unsigned int rowsRdoPerFEIX[5]={160, 336, 672, 336, 672}; // number of rows readout per FEI3, 4, 50, 51, 52


DetectorSpecialPixelMap::DetectorSpecialPixelMap(){}

DetectorSpecialPixelMap::DetectorSpecialPixelMap(const DetectorSpecialPixelMap& spm) :
  PixelCoralClientUtils::DetectorSpecialPixelMap(){

  for(DetectorSpecialPixelMap::const_iterator module = spm.begin(); module != spm.end(); ++module){
    
    push_back(new ModuleSpecialPixelMap( dynamic_cast<ModuleSpecialPixelMap&>(**module)));
    
  }
}

DetectorSpecialPixelMap::DetectorSpecialPixelMap(const std::vector<ModuleSpecialPixelMap*>& modules){

  for(std::vector<ModuleSpecialPixelMap*>::const_iterator module = modules.begin(); module != modules.end(); ++module){
    
    push_back(new ModuleSpecialPixelMap(**module));

  }
}

DetectorSpecialPixelMap::~DetectorSpecialPixelMap(){}


const ModuleSpecialPixelMap* DetectorSpecialPixelMap::module(IdentifierHash moduleHash) const{
  return dynamic_cast<const ModuleSpecialPixelMap*>(PixelCoralClientUtils::DetectorSpecialPixelMap::module(moduleHash));
}


DetectorSpecialPixelMap& DetectorSpecialPixelMap::operator=(const DetectorSpecialPixelMap& spm){
  if(this !=&spm){ 
    *this =  dynamic_cast<DetectorSpecialPixelMap&>(PixelCoralClientUtils::DetectorSpecialPixelMap::operator=(spm));
  }
  return (*this);
}

DetectorSpecialPixelMap& DetectorSpecialPixelMap::operator+=(const DetectorSpecialPixelMap& spm){
  return dynamic_cast<DetectorSpecialPixelMap&>(PixelCoralClientUtils::DetectorSpecialPixelMap::operator+=(spm));
}



//***** ModuleSpecialPixelMap *****//

ModuleSpecialPixelMap::ModuleSpecialPixelMap(){}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const ModuleSpecialPixelMap& mspm) :
  PixelCoralClientUtils::ModuleSpecialPixelMap(mspm){}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const char* filename, unsigned int mchips) :
  PixelCoralClientUtils::ModuleSpecialPixelMap(filename, mchips){}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const std::string& clob, unsigned int mchips) :
  PixelCoralClientUtils::ModuleSpecialPixelMap(clob, mchips){}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const coral::Blob& blob, unsigned int mchips) :
  PixelCoralClientUtils::ModuleSpecialPixelMap(blob, mchips){}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const std::map<unsigned int, unsigned int>& pixels, 
					     unsigned int module_status,
					     std::vector<unsigned int> chip_status,
					     std::vector<std::vector<unsigned int> > column_pair_status,
					     unsigned int mchips) :
  PixelCoralClientUtils::ModuleSpecialPixelMap(pixels, module_status, chip_status, column_pair_status, mchips){}

ModuleSpecialPixelMap::~ModuleSpecialPixelMap(){}


ModuleSpecialPixelMap& ModuleSpecialPixelMap::operator=(const ModuleSpecialPixelMap& mspm){
  if(this !=&mspm){
    *this =dynamic_cast<ModuleSpecialPixelMap&>(PixelCoralClientUtils::ModuleSpecialPixelMap::operator=(mspm));
  }
  return (*this);
}

ModuleSpecialPixelMap& ModuleSpecialPixelMap::operator+=(ModuleSpecialPixelMap mspm){
  return dynamic_cast<ModuleSpecialPixelMap&>(PixelCoralClientUtils::ModuleSpecialPixelMap::operator+=(mspm));
}

void ModuleSpecialPixelMap::print(int component,
				  unsigned int layer,
				  unsigned int phi,
				  int eta,
				  bool verbosePixelID,
				  bool binaryPixelStatus) const{
  
  MsgStream log(Athena::getMessageSvc(), "ModuleSpecialPixelMap");
  
  unsigned int system, subSystem;
  system = 2;
  subSystem = 1;

  int itype = (int)m_chipsPerModule%10;
  if(itype>(nmtype-1))itype=nmtype-1;
  int mch = (int)m_chipsPerModule/10;
  int mcolumns = columnsPerFEIX[itype]; // set protection here

  int moduleID = itype==0 ? ( ((component + 2) / 2) << 25 ) + ( layer << 23 ) + ( phi << 17 ) + ( (eta + 6) << 13 ) : -1;

  //print header

  log << MSG::INFO << " " << endmsg;
  if(moduleID>-1)log << MSG::INFO << "ModuleSpecialPixelMap for module " << moduleID << ":" << endmsg;
  log << MSG::INFO << "Offline ID [system,subsystem,endcap/barrel,layer/disk,phi,eta] : ["
      << system << ',' << subSystem << ',' << component << ',' << layer << ',' << phi << ',' << eta << ']' << endmsg;
  log << MSG::INFO << "Module status: ";
  log.width(10);
  log << m_module_status << endmsg;
  log << MSG::INFO << " " << endmsg;

  bool print_chip_status = false;
  bool print_column_pair_status = false;
  
  for(int i = 0; i < mch; i++){
    if(m_chip_status.size() != 0 && m_chip_status[i] != 0){
      print_chip_status = true;
    }
    for(int j = 0; j < mcolumns/2; j++){
      if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0 
	 && m_column_pair_status[i][j] != 0){
	print_chip_status = true;
	print_column_pair_status = true;
      }
    }
  }
  if(mch>2){
    if(print_chip_status){
      log <<MSG::INFO << "Chip number:       ";
      for(int i = mch-1; i>mch/2-1; i--)log<<i<<"         "; 
      log<< endmsg;

      log << MSG::INFO << "Chip status:  ";
      for(int i = mch-1; i > mch/2-1; i--){
	log.width(10);
	if(m_chip_status.size() != 0){
	  log << m_chip_status[i] << " ";
	}
	else{
	  log << 0 << " ";
	}
      }
      log << endmsg;
      log << MSG::INFO << " " << endmsg;
      
      if(print_column_pair_status){
	log << MSG::INFO << "Column pairs: ";
	for(int i = mch-1; i > mch/2-1; i--){
	  log.width(10);
	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	    log << m_column_pair_status[i][0] << " ";
	  }
	  else{
	    log << 0 << " ";
	  }
	}
	log << endmsg;
	log << MSG::INFO << "pairs 0 to "<<mch/2<<"  ";
	for(int i = mch-1; i > mch/2-1; i--){
	  log.width(10);
	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	    log << m_column_pair_status[i][1] << " ";
	  }
	  else{
	    log << 0 << " ";
	  }
	}
	log << endmsg;
	for(int j = 2; j < mcolumns/2; j++){
	  log << MSG::INFO << "              ";
	  for(int i = mch-1; i > mch/2-1; i--){
	    log.width(10);
	    if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	      log << m_column_pair_status[i][j] << " ";
	    }
	    else{
	      log << 0 << " ";
	    }
	  }
	  log << endmsg;
	}
	log << MSG::INFO << " " << endmsg;
	
	
	log << MSG::INFO << "Column pairs: ";
	for(int i = 0; i < mch/2; i++){
	  log.width(10);
	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	    log << m_column_pair_status[i][0] << " ";
	  }
	  else{
	    log << 0 << " ";
	  }
	}
	log << endmsg;
	log << MSG::INFO << "pairs 0 to "<<mch/2<<"  ";
	for(int i = 0; i < mch/2; i++){
	  log.width(10);
	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	    log << m_column_pair_status[i][1] << " ";
	  }
	  else{
	    log << 0 << " ";
	  }
	}
	log << endmsg;
	for(int j = 2; j < mcolumns/2; j++){
	  log << MSG::INFO << "              ";
	  for(int i = 0; i < mch/2; i++){
	    log.width(10);
	    if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	      log << m_column_pair_status[i][j] << " ";
	    }
	    else{
	      log << 0 << " ";
	    }
	  }
	  log << endmsg;
	}
	log << MSG::INFO << " " << endmsg;
      }
      else{
	log << MSG::INFO << "No special column pairs on this module" << endmsg;
	log << MSG::INFO << " " << endmsg;
      }
      
      log << MSG::INFO << "Chip status:  ";
      for(int i = 0; i < mch/2; i++){
	log.width(10);
	if(m_chip_status.size() != 0){
	  log << m_chip_status[i] << " ";
	}
	else{
	  log << 0 << " ";
	}
      }
      log << endmsg;
      log << MSG::INFO << "Chip number:           ";
      for(int i = 0; i<mch/2; ++i)log<<i<<"          ";
      log<< endmsg;
      log << MSG::INFO << " " << endmsg;
      log << MSG::INFO << " " << endmsg;
    }
    else{
      log << MSG::INFO << "No special chips and/or column pairs on this module" << endmsg;
      log << MSG::INFO << " " << endmsg;
    }
    
    //print map
    
    for(const_iterator pixel = begin(); pixel != end(); ++pixel){
      if(verbosePixelID){
	unsigned int pixelID = pixel->first;
	unsigned int chip = pixelID % 16;
	unsigned int column = (itype==0) ? ((pixelID / 16) % 32) : ((pixelID / 16) % 256);
	unsigned int row = (itype==0) ? (pixelID / 512) : (pixelID / 4096);
	log << "Pixel (chip/column/row) ";
	log << ' ';
	log << chip << "/";
	log << ' ';
	
	log << column << "/";
	
	log << ' ';
	log << row;
      }
      else{
	log << MSG::INFO << "Pixel " << pixel->first;
      }
      if(binaryPixelStatus){
	unsigned int pixelStatus = pixel->second;
	log << " status: ";
	for(int i = 31; i >= 0; i--){
	  bool statusBit = pixelStatus & (1 << i);
	  log << statusBit;
	  if(!(i%4)) log << ' ';
	}
	log << endmsg;
      }
      else{
	log << " status: " << pixel->second << endmsg;
      }
    }
  }
  else{// IBL FEI4 module
    //
    if(print_chip_status){
      log<<MSG::INFO << "Chip number:          ";
      for(int i =0; i<mch; ++i)log<<i<<"          "; 
      log<<endmsg;
      log <<MSG::INFO << "Chip status:  ";
      for(int i = 0; i < (int)mch; i++){
	log.width(10);
	if(m_chip_status.size() != 0){
	  log << m_chip_status[i] << " ";
	}
	else{
	  log << 0 << " ";
	}
      }
      log << endmsg;
      log <<MSG::INFO << " " << endmsg;

      if(print_column_pair_status){
	log <<MSG::INFO <<"Column pairs 0 to  "<<mcolumns/2<<" ";
	log << endmsg;
	for(int j = 0; j<mcolumns/2; ++j){
	  for(int i = 0; i < (int)mch; i++){
	    log.width(10);
	    if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	      log << m_column_pair_status[i][j] << " ";
	    }
	    else{
	      log << 0 << " ";
	    }
	  }
	  log << endmsg;
	}

	log << " " << endmsg;

      }
      else{
	log << "No special column pairs on this module" << endmsg;
	log << " " << endmsg;
      }
    }
    else{
      log << "No special chips and/or column pairs on this module" << endmsg;
      log << " " << endmsg;
    }
    //print map

    for(const_iterator pixel = begin(); pixel != end(); ++pixel){
      if(verbosePixelID){
	unsigned int pixelID = pixel->first;
	unsigned int chip = pixelID % 16;
	unsigned int column = (itype==0) ? ((pixelID / 16) % 32) : ((pixelID / 16) % 256);
	unsigned int row = (itype==0) ? (pixelID / 512) : (pixelID / 4096);

	log <<  "Pixel (chip/column/row) ";
	log << ' ';
	log << chip << "/";
        log <<' ';
	log << column << "/";
        log << ' ';
	log << row;
      }
      else{
	log << MSG::INFO <<"Pixel " << pixel->first;
      }
      if(binaryPixelStatus){
	unsigned int pixelStatus = pixel->second;
	log << " status: ";
	for(int i = 31; i >= 0; i--){
	  bool statusBit = pixelStatus & (1 << i);
	  log << statusBit;
	  if(!(i%4)) log << ' ';
	}
	log << endmsg;
      }
      else{
	log << " status: " << pixel->second << endmsg;
      }
    }
    //
  }
     
  return;
}

StatusCode ModuleSpecialPixelMap::fill_from_string(const std::string& clob){
  if( PixelCoralClientUtils::ModuleSpecialPixelMap::fill_from_string(clob) ){
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ModuleSpecialPixelMap::fill_from_blob(const coral::Blob& blob){
  if( PixelCoralClientUtils::ModuleSpecialPixelMap::fill_from_blob(blob) ){
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
