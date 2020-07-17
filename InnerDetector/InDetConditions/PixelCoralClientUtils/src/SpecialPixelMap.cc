/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// SpecialPixelMap.cxx
// 
// Classes to hold data about special pixels in the detector store and make it
// accessible to athena components.
//
// author Christoph Ruwiedel <ruwiedel@physik.uni-bonn.de>
// updated to be comptible with IBL and phase II: -- Wei-Ming Yao <wmyao@lbl.gov>


#include "PixelCoralClientUtils/SpecialPixelMap.hh"

#include<fstream>
#include<sstream>
#include<string>
#include<iomanip>
#include<stdint.h>
#include<cmath>


namespace PixelCoralClientUtils {

//***** DetectorSpecialPixelMap *****//

//===== Constructors, Destructor =====//

DetectorSpecialPixelMap::DetectorSpecialPixelMap(){}

DetectorSpecialPixelMap::DetectorSpecialPixelMap(const DetectorSpecialPixelMap& spm) : 
  std::vector<ModuleSpecialPixelMap*>(){

  for(DetectorSpecialPixelMap::const_iterator module = spm.begin(); module != spm.end(); ++module){

    push_back(new ModuleSpecialPixelMap(**module));
    
  }
}

DetectorSpecialPixelMap::DetectorSpecialPixelMap(const std::vector<ModuleSpecialPixelMap*>& modules){

  for(std::vector<ModuleSpecialPixelMap*>::const_iterator module = modules.begin(); module != modules.end(); ++module){

    push_back(new ModuleSpecialPixelMap(**module));

  }
}

void DetectorSpecialPixelMap::clear(){

  for(const_iterator module = begin(); module != end(); ++module){
    delete *module;
  }
  std::vector<ModuleSpecialPixelMap*>::clear();
}

DetectorSpecialPixelMap::~DetectorSpecialPixelMap(){
  for(const_iterator module = begin(); module != end(); ++module){
    delete *module;
  }
}


//===== member functions =====//

const ModuleSpecialPixelMap* DetectorSpecialPixelMap::module(unsigned int moduleHash) const{
  return (*this)[moduleHash];
}


DetectorSpecialPixelMap& DetectorSpecialPixelMap::operator=(const DetectorSpecialPixelMap& spm){
  if(&spm != this){
    for(const_iterator module = begin(); module != end(); ++module){
      delete *module;
    }
    
    resize(spm.size());
    
    for(DetectorSpecialPixelMap::const_iterator module = spm.begin(); module != spm.end(); ++module){
      (*this)[module - spm.begin()] = new ModuleSpecialPixelMap(**module);
    }
  }
  return *this;
}

DetectorSpecialPixelMap& DetectorSpecialPixelMap::operator+=(const DetectorSpecialPixelMap& spm){
  if(&spm != this){

    if(size() != spm.size()){ 
      std::cout << "Unable to merge special pixel maps: sizes do not match" << std::endl;

      return *this;
    }
    
    for(DetectorSpecialPixelMap::const_iterator module = spm.begin(); module != spm.end(); ++module){
      if( (*this)[module - spm.begin()] == 0 && (*module) != 0 ){
	(*this)[module - spm.begin()] = new ModuleSpecialPixelMap( **module );
      }
      else if( (*module) != 0 ){
	(*this)[module - spm.begin()]->merge(*module);
      }
    }
  }
  return *this;
}

void DetectorSpecialPixelMap::merge(const DetectorSpecialPixelMap* spm){
  (*this) += (*spm);
  return;
}

void DetectorSpecialPixelMap::markSpecialRegions(){
  for(DetectorSpecialPixelMap::iterator module = begin(); module != end(); ++module){
    (*module)->markSpecialRegions();
  }
  return;
}

void DetectorSpecialPixelMap::setNeighbourFlags(){
  for(DetectorSpecialPixelMap::iterator module = begin(); module != end(); ++module){
    (*module)->setNeighbourFlags();
  }
  return;
}



//***** ModuleSpecialPixelMap *****//

//===== Constructors, Destructor =====//

ModuleSpecialPixelMap::ModuleSpecialPixelMap() :
  m_chipsPerModule(0), 
  m_module_status(0)
{}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const ModuleSpecialPixelMap& mspm) :
  std::map<unsigned int, unsigned int>(mspm),
  m_chipsPerModule(mspm.m_chipsPerModule),
  m_module_status(mspm.m_module_status),
  m_chip_status(mspm.m_chip_status),
  m_column_pair_status(mspm.m_column_pair_status)
{}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const char* filename, unsigned int mchips) :
  m_chipsPerModule(mchips),
  m_module_status(0)
{
  std::ifstream in(filename, std::ios_base::in);
  //std::string ssink;
  //std::getline(in, ssink);

  while(in.good()){
    unsigned int chip, column, row, status;
    in >> chip >> column >> row >> status;

    if(in.good()){
      if(validPixelID(chip, column, row)){
	//do not record pixels that are listed as "unknown dead" and nothing else
	if((status & ~((15 << 25) + (1 << 16))) != 0){
	  //mark pixels as "unknown dead" if they are dead but we don't know why
	  if((status % 2 == 1) && ((status & (255 << 8)) == 0)){
	    setPixelStatus(encodePixelID(chip, column, row, m_chipsPerModule), status |= (1 << 16));
	  }
	  else{
	    setPixelStatus(encodePixelID(chip, column, row, m_chipsPerModule), status);
	  }
	}
      }
      else{
	std::cout << "In ModuleSpecialPixelMap::ModuleSpecialPixelMap(const char*) :" 
		  << " Invalid pixel ID read from text file" << std::endl;
      }
    }
  }

  setNeighbourFlags();
}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const std::string& clob, unsigned int mchips) :
  m_chipsPerModule(mchips),
  m_module_status(0)
{
  bool statuscode = fill_from_string(clob);
  if( statuscode ){
    std::cout << "In ModuleSpecialPixelMap::ModuleSpecialPixelMap(const std::string&) :" 
	<< " Construction from clob failed" << std::endl;
  }
}


ModuleSpecialPixelMap::ModuleSpecialPixelMap(const coral::Blob& blob, unsigned int mchips) :
  m_chipsPerModule(mchips),
  m_module_status(0)
{
  bool statuscode = fill_from_blob(blob);
  if(statuscode){
    std::cout << "In ModuleSpecialPixelMap::ModuleSpecialPixelMap(const coral::Blob&) :" 
	<< " Construction from blob failed" << std::endl;
  }
}

ModuleSpecialPixelMap::ModuleSpecialPixelMap(const std::map<unsigned int, unsigned int>& pixels, 
					     unsigned int module_status,
					     std::vector<unsigned int> chip_status,
					     std::vector<std::vector<unsigned int> > column_pair_status,
					     unsigned int mchips) :
  std::map<unsigned int, unsigned int>(pixels),
  m_chipsPerModule(mchips),
  m_module_status(module_status),
  m_chip_status(chip_status),
  m_column_pair_status(column_pair_status)
{
}

ModuleSpecialPixelMap::~ModuleSpecialPixelMap(){}


//===== public member functions =====//


ModuleSpecialPixelMap& ModuleSpecialPixelMap::operator=(const ModuleSpecialPixelMap& mspm){
  if(&mspm != this){
    std::map<unsigned int, unsigned int>::operator=(mspm);
    m_module_status = mspm.m_module_status;
    m_chip_status = mspm.m_chip_status;
    m_column_pair_status = mspm.m_column_pair_status;
    m_chipsPerModule = mspm.m_chipsPerModule;
  }
  return *this;
}

ModuleSpecialPixelMap& ModuleSpecialPixelMap::operator+=(ModuleSpecialPixelMap mspm){
  
  if(mspm.moduleStatus() != 0 || mspm.hasSpecialChips() || mspm.hasSpecialColumnPairs() || !mspm.empty()){
    
    mspm.resetSpecialRegions();
    resetSpecialRegions();
    
    for(ModuleSpecialPixelMap::const_iterator pixel = mspm.begin(); pixel != mspm.end(); pixel++){
      (*this)[(*pixel).first] |= (*pixel).second;
    }
    
    markSpecialRegions();
    setNeighbourFlags();
  }
  
  return *this;
}

void ModuleSpecialPixelMap::merge(const ModuleSpecialPixelMap* mspm){
  (*this) += (*mspm);
  return;
}

unsigned int ModuleSpecialPixelMap::pixelStatus(unsigned int pixelID) const{
  int i = (int)chipType();
  int mch = (int)chipsPerModule();
  int mcolumns =(int)columnsPerChip();
  int mrowsrdo = (int)rowsrdoPerChip();
  
  int chip = pixelID % 16;
  int column = (i==0) ? ((pixelID / 16) % 32) : ((pixelID / 16) % 256);
  int row = (i==0) ? (pixelID / 512) : (pixelID / 4096);
						
  if(mch>2){

    if(m_module_status != 0){
      unsigned int status = m_module_status;
      if(column != 0 || (chip != 0 && chip != mch/2)){     // left
	status |= (1 << 27);
      }
      if(column != (mcolumns -1) || (chip != (mch/2 - 1) && chip != (mch - 1))){   // right
	status |= (1 << 28);
      }
      status |= (1 << 26);                             // top
      if(row != 0){                                    // bottom
	status |= (1 << 25);
      }
      return status;
    }
    else if(m_chip_status.size() != 0 && m_chip_status[chip] != 0){

      unsigned int status = m_chip_status[chip];
      ModuleSpecialPixelMap::const_iterator pixel;

      if(column != 0){                                 // left
	status |= (1 << 27);
      }
      else if(column == 0 && chip != 0 && chip != mch/2){
	pixel = find(encodePixelID(chip - 1, (mcolumns-1), row, m_chipsPerModule));
	if(m_chip_status[chip-1] != 0){
	  status |= (1 << 27);
	}
	else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip - 1].size() != 0
		&& m_column_pair_status[chip - 1][mcolumns/2 - 1] != 0){
	  status |= (1 << 27);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 27);
	  }
	}	
      }

      if(column != (mcolumns -1)){                                // right
	status |= (1 << 28);
      }
      else if(column == (mcolumns-1) && chip != (mch/2 - 1) && chip != (mch - 1)){
	pixel = find(encodePixelID(chip + 1, 0, row, m_chipsPerModule));
	if(m_chip_status[chip + 1] != 0){
	  status |= (1 << 28);
	}
	else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip + 1].size() != 0
		&& m_column_pair_status[chip + 1][0] != 0){
	  status |= (1 << 28);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}  
      }

      if(row != (mrowsrdo - 1)){                                  // top
	status |= (1 << 26);
      }
      else if(row == (mrowsrdo - 1)){
	pixel = find(encodePixelID(mch-1- chip, mcolumns-1- column, row, m_chipsPerModule));
	if(m_chip_status[mch-1 - chip] != 0){
	  status |= (1 << 26);
	}
	else if(m_column_pair_status.size() != 0 && m_column_pair_status[mch - 1 - chip].size() != 0
		&& m_column_pair_status[mch - 1 - chip][(mcolumns - 1 - column) / 2] != 0){
	  status |= (1 << 26);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 26);
	  }
	}
      }

      if(row != 0){                                    // bottom
	status |= (1 << 25);
      }
      return status;
    }
    else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip].size() != 0 
	    && m_column_pair_status[chip][column / 2] != 0){
      unsigned int status = m_column_pair_status[chip][column / 2];
      ModuleSpecialPixelMap::const_iterator pixel;
    
      if(column % 2 != 0){                             // left
	status |= (1 << 27);
      }
      else if(column % 2 == 0 && column != 0){
	pixel = find(encodePixelID(chip, column - 1, row, m_chipsPerModule));
	if(m_column_pair_status[chip][column / 2 - 1] != 0){
	  status |= (1 << 27);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 27);
	  }
	}
      }
      else if(column == 0 && chip != 0 && chip != mch/2){
	pixel = find(encodePixelID(chip - 1, mcolumns - 1, row, m_chipsPerModule));
	if(m_chip_status.size() != 0 && m_chip_status[chip - 1] != 0){
	  status |= (1 << 27);
	}
	else if(m_column_pair_status[chip - 1].size() != 0 
		&& m_column_pair_status[chip - 1][mcolumns/2 - 1] != 0){
	  status |= (1 << 27);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 27);
	  }
	}
      }

      if(column % 2 == 0){                             // right
	status |= (1 << 28);
      }
      else if(column % 2 != 0 && column != (mcolumns -1)){
	pixel = find(encodePixelID(chip, column + 1, row, m_chipsPerModule));
	if(m_column_pair_status[chip][(column / 2) + 1] != 0){
	  status |= (1 << 28);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}
      }
      else if(column == (mcolumns-1) && chip != (mch/2-1) && chip != (mch-1)){
	pixel = find(encodePixelID(chip + 1, 0, row, m_chipsPerModule));
	if(m_chip_status.size() != 0 && m_chip_status[chip + 1] != 0){
	  status |= (1 << 28);
	}
	else if(m_column_pair_status[chip + 1].size() != 0
		&&m_column_pair_status[chip + 1][0] != 0){
	  status |= (1 << 28);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}
      }
      
      if(row != (mrowsrdo - 1)){                                  // top
	status |= (1 << 26);
      }
      else if(row == (mrowsrdo-1)){
	pixel = find(encodePixelID(mch-1 - chip, mcolumns-1 - column, mrowsrdo-1, m_chipsPerModule));
	if(m_chip_status.size() != 0 && m_chip_status[mch-1 - chip] != 0){
	  status |= (1 << 26);
	}
	else if(m_column_pair_status[mch-1 - chip].size() != 0 
		&& m_column_pair_status[mch-1 - chip][(mcolumns-1 - column) / 2] != 0){
	  status |= (1 << 26);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 26);
	  }
	}
      }
      
      if(row != 0){                                    // bottom
	status |= (1 << 25);
      }
      return status;
    }
    else if(empty()){
      return 0;
    }
    else{
      const_iterator pixel = find(pixelID);
      if(pixel == end()){
	return 0;
      }
      else{
	return (*pixel).second;
      }
    }
  }
  else { // IBL modules 

    if(m_module_status != 0){
      unsigned int status = m_module_status;
      if(column != 0 || (chip != 0)){     // left
        status |= (1 << 27);
      }
      if(column != (mcolumns-1) || (chip != 1&&mch==2)){   // right
        status |= (1 << 28);
      }
      if(row != (mrowsrdo-1))status |= (1 << 26);                             // top
      if(row != 0){                                    // bottom
        status |= (1 << 25);
      }
      return status;
    }
    else if(m_chip_status.size() != 0 && m_chip_status[chip] != 0){

      unsigned int status = m_chip_status[chip];
      ModuleSpecialPixelMap::const_iterator pixel;

      if(column != 0){                                 // left
        status |= (1 << 27);
      }
      else if(column == 0 && chip != 0&&mch==2){
        pixel = find(encodePixelID(chip - 1, mcolumns-1, row, m_chipsPerModule));
        if(m_chip_status[chip-1] != 0){
          status |= (1 << 27);
        }
        else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip - 1].size() != 0
                && m_column_pair_status[chip - 1][mcolumns/2 - 1] != 0){
          status |= (1 << 27);
        }
        else if(pixel != end()){
          if(((*pixel).second & ~(15 << 25)) != 0){
            status |= (1 << 27);
          }
        }
      }

      if(column != (mcolumns - 1) ){                                // right
        status |= (1 << 28);
      }
      else if(column == (mcolumns - 1) && chip != 1 &&mch==2){
        pixel = find(encodePixelID(chip + 1, 0, row,m_chipsPerModule));
        if(m_chip_status[chip + 1] != 0){
          status |= (1 << 28);
        }
        else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip + 1].size() != 0
                && m_column_pair_status[chip + 1][0] != 0){
          status |= (1 << 28);
        }
        else if(pixel != end()){
          if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}  
      }

      if(row != (mrowsrdo-1)){                                  // top
	status |= (1 << 26);
      }

      if(row != 0){                                    // bottom
	status |= (1 << 25);
      }
      return status;
    }
    else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip].size() != 0 
	    && m_column_pair_status[chip][column / 2] != 0){
      unsigned int status = m_column_pair_status[chip][column / 2];
      ModuleSpecialPixelMap::const_iterator pixel;
    
      if(column % 2 != 0){                             // left
	status |= (1 << 27);
      }
      else if(column % 2 == 0 && column != 0){
	pixel = find(encodePixelID(chip, column - 1, row, m_chipsPerModule));
	if(m_column_pair_status[chip][(column / 2) - 1] != 0){
	  status |= (1 << 27);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 27);
	  }
	}
      }
      else if(column == 0 && chip != 0 && mch==2){
	pixel = find(encodePixelID(chip - 1, (mcolumns-1), row, m_chipsPerModule));
	if(m_chip_status.size() != 0 && m_chip_status[chip - 1] != 0){
	  status |= (1 << 27);
	}
	else if(m_column_pair_status[chip - 1].size() != 0 
		&& m_column_pair_status[chip - 1][mcolumns/2-1] != 0){
	  status |= (1 << 27);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 27);
	  }
	}
      }

      if(column % 2 == 0){                             // right
	status |= (1 << 28);
      }
      else if(column % 2 != 0 && column != (mcolumns-1)){
	pixel = find(encodePixelID(chip, column + 1, row, m_chipsPerModule));
	if(m_column_pair_status[chip][(column / 2) + 1] != 0){
	  status |= (1 << 28);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}
      }
      else if(column == (mcolumns-1) && chip != 1 && mch==2){
	pixel = find(encodePixelID(chip + 1, 0, row, m_chipsPerModule));
	if(m_chip_status.size() != 0 && m_chip_status[chip + 1] != 0){
	  status |= (1 << 28);
	}
	else if(m_column_pair_status[chip + 1].size() != 0
		&&m_column_pair_status[chip + 1][0] != 0){
	  status |= (1 << 28);
	}
	else if(pixel != end()){
	  if(((*pixel).second & ~(15 << 25)) != 0){
	    status |= (1 << 28);
	  }
	}
      }
      
      if(row != (mrowsrdo-1)){                                  // top
	status |= (1 << 26);
      }      
      if(row != 0){                                    // bottom
	status |= (1 << 25);
      }
      return status;
    }
    else if(empty()){
      return 0;
    }
    else{
      const_iterator pixel = find(pixelID);
      if(pixel == end()){
	return 0;
      }
      else{
	return (*pixel).second;
      }
    }
  }
}

bool ModuleSpecialPixelMap::pixelStatusBit(unsigned int pixelID, unsigned int bit) const{
  if(bit < 32){
    unsigned int status = pixelStatus(pixelID); 
    return  static_cast<bool>(status & (1 << bit));
  }   
  else {
    std::cout << "Requested bit " << bit << " out of range ([0,31] allowed)" << std::endl;
  }
  return false;
}

std::string ModuleSpecialPixelMap::to_string() const{

  std::ostringstream clob(std::ios::binary);
  
  clob << m_module_status << ' ';
  
  unsigned int specialRegionsStatus = 0;
  int mch =(int)chipsPerModule();
  int mcolumns = (int)columnsPerChip()/2;  

  for(int i = 0; i < mch; i++){
    if(m_chip_status.size() != 0 && m_chip_status[i] != 0){
      specialRegionsStatus |= (1 << i);
    }
    for(int j = 0; j < mcolumns; j++){
      if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0
	 && m_column_pair_status[i][j] != 0){
	specialRegionsStatus |= (1 << (i+mch));
      }
    }
  }
  
  clob << specialRegionsStatus << ' ';
  
  for(int i = 0; i < mch; i++){ 
    if((specialRegionsStatus & (1 << i)) != 0){
      clob << m_chip_status[i] << ' ';
    }
    if((specialRegionsStatus & (1 << (i+mch))) != 0){
      for(int j = 0; j < mcolumns; j++){
	clob << m_column_pair_status[i][j] << ' ';
      }
    }
  }
  
  for(const_iterator mi = begin(); mi != end(); ++mi){
    clob << mi->first << ' ' << mi->second << ' ';
  }
  return clob.str();
}


coral::Blob* ModuleSpecialPixelMap::to_blob() const{

  unsigned int specialRegionsStatus = 0;
  unsigned int specialRegionsSize = 0;
  int mch =(int)chipsPerModule();
  int mcolumns = (int)columnsPerChip()/2; 
  
  for(int i = 0; i < mch; i++){
    if(m_chip_status.size() != 0 && m_chip_status[i] != 0){
      specialRegionsStatus |= (1 << i);
      ++specialRegionsSize;
    }
    for(int j = 0; j < mcolumns; j++){
      if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0
	 && m_column_pair_status[i][j] != 0){
	
	if( (specialRegionsStatus & (1 << (i+mch)) ) == 0){
	  specialRegionsSize += mcolumns;
	}

	specialRegionsStatus |= (1 << (i+mch));
      }
    }
  }

  coral::Blob* blob = new coral::Blob( (2 + specialRegionsSize + 2 * size()) * sizeof(uint32_t) );
  
  uint32_t* p = static_cast<uint32_t*>(blob->startingAddress());

  *p = m_module_status;
  ++p;
  *p = specialRegionsStatus;

  for(int i = 0; i < mch; i++){
    if((specialRegionsStatus & (1 << i)) != 0){
      ++p;
      *p = m_chip_status[i];
    }
    if((specialRegionsStatus & (1 << (i+mch))) != 0){
      for(int j = 0; j < mcolumns; j++){
	++p;
	*p = m_column_pair_status[i][j];
      }
    }
  }
  
  for(const_iterator mi = begin(); mi != end(); ++mi){
    ++p;
    *p = mi->first;
    ++p;
    *p = mi->second;
  }

  return blob;
}

void ModuleSpecialPixelMap::print(int component,
				  unsigned int layer,
				  unsigned int phi,
				  int eta,
				  bool verbosePixelID,
				  bool binaryPixelStatus) const{
  
  unsigned int system, subSystem;
  system = 2;
  subSystem = 1;

  int itype = (int)chipType();
  int mch = (int)chipsPerModule();
  int mcolumns =(int)columnsPerChip();
  
  // check what to do with moduleID ? 
  int moduleID = itype==0 ? ( ( ((component + 2) / 2) << 25 ) + 
					       ( layer << 23 ) + ( phi << 17 ) + ( (eta + 6) << 13 )): -1;

  //print header

  std::cout << " " << std::endl;
  if(moduleID>-1)std::cout << "ModuleSpecialPixelMap for module " << moduleID << ":" << std::endl;
  std::cout << "Offline ID [system,subsystem,endcap/barrel,layer/disk,phi,eta] : [" 
	    << system << ',' << subSystem << ',' << component << ',' << layer << ',' << phi << ',' << eta << ']' << std::endl;
  std::cout << "Module status: ";
  std::cout.width(10);
  std::cout << m_module_status << std::endl;
  std::cout << " " << std::endl;

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
     std::cout << "Chip number:       ";
     for(int i = mch-1; i>mch/2-1; i--)std::cout<<i<<"         "; 
     std::cout<< std::endl;
     std::cout << "Chip status:  ";
     for(int i = mch-1; i > mch/2-1; i--){
       std::cout.width(10);
       if(m_chip_status.size() != 0){
 	std::cout << m_chip_status[i] << " ";
       }
       else{
	 std::cout << 0 << " ";
       }
     }
     std::cout << std::endl;
     std::cout << " " << std::endl;
    
     if(print_column_pair_status){
       std::cout << "Column pairs: ";
       for(int i = mch-1; i > mch/2-1; i--){
 	std::cout.width(10);
 	if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	  std::cout << m_column_pair_status[i][0] << " ";
 	}
 	else{
 	  std::cout << 0 << " ";
 	}
       }
       std::cout << std::endl;
       std::cout << "pairs 0 to "<<mch/2<<"  ";
       for(int i = mch-1; i > mch/2-1; i--){
 	std::cout.width(10);
 	if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	  std::cout << m_column_pair_status[i][1] << " ";
 	}
 	else{
 	  std::cout << 0 << " ";
 	}
       }
       std::cout << std::endl;
       for(int j = 2; j < mcolumns/2; j++){
 	std::cout << "              ";
 	for(int i = mch-1; i > mch/2-1; i--){
 	  std::cout.width(10);
 	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	    std::cout << m_column_pair_status[i][j] << " ";
 	  }
 	  else{
 	    std::cout << 0 << " ";
 	  }
 	}
 	std::cout << std::endl;
       }
       std::cout << " " << std::endl;
       
       
       std::cout << "Column pairs: ";
       for(int i = 0; i < mch/2; i++){
 	std::cout.width(10);
  	if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	  std::cout << m_column_pair_status[i][0] << " ";
 	}
 	else{
 	  std::cout << 0 << " ";
 	}
       }
       std::cout << std::endl;
       std::cout << "pairs 0 to "<<mch/2<<"  ";
       for(int i = 0; i < mch/2; i++){
 	std::cout.width(10);
 	if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	  std::cout << m_column_pair_status[i][1] << " ";
 	}
 	else{
 	  std::cout << 0 << " ";
 	}
       }
       std::cout << std::endl;
       for(int j = 2; j < mcolumns/2; j++){
 	std::cout << "              ";
 	for(int i = 0; i < mch/2; i++){
 	  std::cout.width(10);
 	  if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
 	    std::cout << m_column_pair_status[i][j] << " ";
 	  }
 	  else{
 	    std::cout << 0 << " ";
 	  }
 	}
 	std::cout << std::endl;
       }
       std::cout << " " << std::endl;
     }
     else{
       std::cout << "No special column pairs on this module" << std::endl;
       std::cout << " " << std::endl;
     } 
 
     std::cout << "Chip status:  ";
     for(int i = 0; i < mch/2; i++){
       std::cout.width(10);
       if(m_chip_status.size() != 0){
 	std::cout << m_chip_status[i] << " ";
       }
       else{
 	std::cout << 0 << " ";
       }
     }
     std::cout << std::endl;
     std::cout << "Chip number:           ";
     for(int i = 0; i<mch/2; ++i)std::cout<<i<<"          ";
     std::cout<< std::endl;
     std::cout << " " << std::endl;
     std::cout << " " << std::endl;
   }
   else{
     std::cout << "No special chips and/or column pairs on this module" << std::endl;
     std::cout << " " << std::endl;
   }

   //print map

   for(const_iterator pixel = begin(); pixel != end(); ++pixel){
     if(verbosePixelID){
       unsigned int pixelID = pixel->first;
       unsigned int chip = pixelID % 16;
       unsigned int column = (itype==0) ? ((pixelID / 16) % 32) : ((pixelID / 16) % 256);
       unsigned int row = (itype==0) ? (pixelID / 512) : (pixelID / 4096);
       std::cout << "Pixel (chip/column/row) ";
       std::cout << ' ';
       std::cout << chip << "/";       
       std::cout << ' ';
       std::cout << column << "/";
       std::cout << ' ';
       std::cout << row;
     }
     else{
       std::cout << "Pixel " << pixel->first;
     }
     if(binaryPixelStatus){
       unsigned int pixelStatus = pixel->second;
       std::cout << " status: ";
       for(int i = 31; i >= 0; i--){
 	bool statusBit = pixelStatus & (1 << i);
 	std::cout << statusBit;
 	if(!(i%4)) std::cout << ' ';
       }
       std::cout << std::endl;
     }
     else{
       std::cout << " status: " << pixel->second << std::endl;
     }
   }
  }
  else{ // IBL FEI4 module 
    //
   if(print_chip_status){
     std::cout << "Chip number:          ";
     for(int i =0; i<mch; ++i)std::cout<<i<<"          "; 
     std::cout<< std::endl;
     
     std::cout << "Chip status:  ";
     for(int i = 0; i < (int)mch; i++){
       std::cout.width(10);
       if(m_chip_status.size() != 0){
	 std::cout << m_chip_status[i] << " ";
       }
       else{
	 std::cout << 0 << " ";
       }
     }
     std::cout << std::endl;
     std::cout << " " << std::endl;
    
     if(print_column_pair_status){
       std::cout << "Column pairs 0 to  "<<mcolumns/2<<" ";
       std::cout << std::endl;
       for(int j = 0; j<mcolumns/2; ++j){
	 for(int i = 0; i < mch; i++){
	   std::cout.width(10);
	   if(m_column_pair_status.size() != 0 && m_column_pair_status[i].size() != 0){
	     std::cout << m_column_pair_status[i][j] << " ";
	   }
	   else{
	     std::cout << 0 << " ";
	   }
	 }
	 std::cout << std::endl;
       }

       std::cout << " " << std::endl;              

     }
     else{
       std::cout << "No special column pairs on this module" << std::endl;
       std::cout << " " << std::endl;
     } 
   }
   else{
     std::cout << "No special chips and/or column pairs on this module" << std::endl;
     std::cout << " " << std::endl;
   }

   //print map

   for(const_iterator pixel = begin(); pixel != end(); ++pixel){
     if(verbosePixelID){
       unsigned int pixelID = pixel->first;
       unsigned int chip = pixelID % 16;
       unsigned int column = (itype==0) ? ((pixelID / 16) % 32) : ((pixelID / 16) % 256);
       unsigned int row = (itype==0) ? (pixelID / 512) : (pixelID / 4096);

       std::cout << "Pixel (chip/column/row) ";
       std::cout <<' ';
       std::cout << chip << "/";
       std::cout <<' ';
       std::cout << column << "/";  
       std::cout <<' ';
       std::cout << row;
     }
     else{
       std::cout << "Pixel " << pixel->first;
     }
     if(binaryPixelStatus){
       unsigned int pixelStatus = pixel->second;
       std::cout << " status: ";
       for(int i = 31; i >= 0; i--){
 	bool statusBit = pixelStatus & (1 << i);
 	std::cout << statusBit;
 	if(!(i%4)) std::cout << ' ';
       }
       std::cout << std::endl;
     }
     else{
       std::cout << " status: " << pixel->second << std::endl;
     }
   }
    //
  }
  return;
}
 
void ModuleSpecialPixelMap::setPixelStatus(unsigned int pixelID, unsigned int status){
  if(validPixelID(pixelID)){
    condResetSpecialRegions(pixelID, status);

    unsigned int chip = pixelID % 16;
    unsigned int column = chipType()==0 ? ((pixelID / 16) % 32): ((pixelID / 16) % 256);
    
    if(status == 0){
      erase(pixelID);
    }
    else{
      if(m_module_status == 0 && (m_chip_status.size() == 0 || m_chip_status[chip] == 0)
	 && (m_column_pair_status.size() == 0 || m_column_pair_status[chip].size() == 0
	     || m_column_pair_status[chip][column/ 2] == 0)){
      (*this)[pixelID] = status;
      }
    }
  }
  else{
    std::cout << "Invalid pixel ID requested";
  }
  return;
}

void ModuleSpecialPixelMap::setPixelStatusBit(unsigned int pixelID, unsigned int bit){
  unsigned int chip = pixelID % 16;
  unsigned int column = (chipType())==0 ? ((pixelID / 16) % 32): ((pixelID / 16) % 256);

  if(validPixelID(pixelID)){
    if((pixelStatus(pixelID) & (1 << bit)) == 0){
      condResetSpecialRegions(pixelID, pixelStatus(pixelID) | (1 << bit));
    }
    if(bit < 32){
      if(m_module_status == 0 && (m_chip_status.size() == 0 || m_chip_status[chip] == 0)
	 && (m_column_pair_status.size() == 0 || m_column_pair_status[chip].size() == 0
	     || m_column_pair_status[chip][column / 2] == 0)){
	(*this)[pixelID] |= (1 << bit);
      }
    } 
    else {
      std::cout << "Requested bit " << bit << " out of range ([0,31] allowed)" << std::endl;
    }
  }
  else{
    std::cout << "Invalid pixel ID requested" << std::endl;
  }
  return;
}

void ModuleSpecialPixelMap::clearPixelStatusBit(unsigned int pixelID, unsigned int bit){
  unsigned int chip = pixelID % 16;
  unsigned int column = chipType()==0 ? ((pixelID / 16) % 32): ((pixelID /16) % 256);

  if(validPixelID(pixelID)){
    if((pixelStatus(pixelID) & (1 << bit)) != 0){
      condResetSpecialRegions(pixelID, pixelStatus(pixelID) & ~(1 << bit));
    }
    if(bit < 32){
      if(!empty()){
	if(find(pixelID) != end()){
	  if(m_module_status == 0 && (m_chip_status.size() == 0 || m_chip_status[chip] == 0)
	     && (m_column_pair_status.size() == 0 || m_column_pair_status[chip].size() == 0
		 || m_column_pair_status[chip][column / 2] == 0)){
	    (*this)[pixelID] &= ~(1 << bit);
	    if(((*this)[pixelID] & ~(15 << 25)) == 0){
	      erase(pixelID);
	    }
	  }
	}
      }
    }
    else{
      std::cout << "Requested bit " << bit << " out of range ([0,31] allowed)" << std::endl;
    }
  } 
  else {
    std::cout << "Invalid pixel ID requested" << std::endl;
  }
  return;
}

void ModuleSpecialPixelMap::condResetSpecialRegions(unsigned int pixelID, unsigned status){

  // reset module/chip/column pair status if necessary
  unsigned int ix = chipType();
  unsigned int mcolumns = columnsPerChip()/2;
  unsigned int mrows = rowsrdoPerChip();
  int mch = (int)chipsPerModule();
  unsigned int chip = pixelID % 16;
  unsigned int column = (ix==0) ? ((pixelID / 16) % 32): ((pixelID /16) % 256);

  if(m_module_status != 0 && m_module_status != (status & ~(15 << 25))){
    m_chip_status.resize(mch);
    for(int i = 0; i < mch; i++){
      m_chip_status[i] = m_module_status;
    }
    m_module_status = 0;
  }

  if(m_chip_status.size() != 0 && m_chip_status[chip] != 0 
     && m_chip_status[chip] != (status & ~(15 << 25))){
    m_column_pair_status.resize(mch);
    m_column_pair_status[chip].resize(mcolumns);
    for(int j = 0; j < (int)mcolumns; j++){
      m_column_pair_status[chip][j] = m_chip_status[chip];
    }
    m_chip_status[chip] = 0;
  } 

  if(m_column_pair_status.size() != 0 && m_column_pair_status[chip].size() != 0 
     && m_column_pair_status[chip][column / 2] != 0
     && m_column_pair_status[chip][column / 2] != (status & ~(15 << 25))){
    for(int i = 0; i < 2; i++){
      for(int row = 0; row < (int)mrows; row++){
	(*this)[encodePixelID(chip, (column / 2) * 2 + i, row, m_chipsPerModule)]
	  =  m_column_pair_status[chip][column / 2];
      }
    }
    m_column_pair_status[chip][column / 2] = 0;
    setNeighbourFlags();
  }
  
  return;
}

void ModuleSpecialPixelMap::resetSpecialRegions(){
  unsigned int mcolumns = columnsPerChip()/2;
  unsigned int mrows = rowsrdoPerChip();
  int mch = (int)chipsPerModule();

  // reset module/chip/column pair status

  if(m_module_status != 0){
    m_chip_status.resize(mch);
    for(int chip = 0; chip < mch; chip++){
      m_chip_status[chip] = m_module_status;
    }
    m_module_status = 0;
  }

  if(m_chip_status.size() != 0){
    for(int chip = 0; chip < mch; chip++){ 
      if(m_chip_status[chip] != 0){
	m_column_pair_status.resize(mch);
	m_column_pair_status[chip].resize(mcolumns);
	for(int column_pair = 0; column_pair < (int)mcolumns; column_pair++){
	  m_column_pair_status[chip][column_pair] = m_chip_status[chip];
	}
      }
    }
    m_chip_status.resize(0);
  } 

  if(m_column_pair_status.size() != 0){
    for(int chip = 0; chip < mch; chip++){
      if(m_column_pair_status[chip].size() != 0){
	for(int column_pair = 0; column_pair < (int)mcolumns; column_pair++){
	  if(m_column_pair_status[chip][column_pair] != 0){
	    for(int i = 0; i < 2; i++){
	      for(int row = 0; row < (int)mrows; row++){
		(*this)[encodePixelID(chip, 2*column_pair + i, row, m_chipsPerModule)]
		  =  m_column_pair_status[chip][column_pair];
	      }
	    }
	  }
	}
      }
    }
    m_column_pair_status.resize(0);
    setNeighbourFlags();
  }
  return;
}

bool ModuleSpecialPixelMap::fill_from_string(const std::string& clob){
  unsigned int mcolumns = columnsPerChip()/2;
  int mch = (int)chipsPerModule();
  int mkey = (int)pow(2, mch) -1;

   unsigned int pixelID, status;
   std::istringstream input_string(clob, std::ios::binary);
   input_string >> m_module_status;

   unsigned int specialRegionsStatus = 0;

   input_string >> specialRegionsStatus;
   
   if((specialRegionsStatus & mkey) != 0){
     m_chip_status.resize(mch);
   }
   if((specialRegionsStatus & (mkey << mch)) != 0){
     m_column_pair_status.resize(mch);
   }

   for(int i = 0; i < mch; i++){
     if((specialRegionsStatus & (1 << i)) != 0){
       input_string >> m_chip_status[i];
     }
     if((specialRegionsStatus & (1 << (i+mch))) != 0){
       m_column_pair_status[i].resize(mcolumns);
       for(int j = 0; j < (int)mcolumns; j++){
	 input_string >> m_column_pair_status[i][j];
       }
     }
   }

   input_string >> pixelID >> status;
   while(input_string.good()){
      setPixelStatus(pixelID, status);
      input_string >> pixelID >> status;
   }
   return false;
}


bool ModuleSpecialPixelMap::fill_from_blob(const coral::Blob& blob){
  unsigned int mcolumns = columnsPerChip()/2;
  int mch = (int)chipsPerModule();
  int mkey = (int)pow(2, mch) -1;

  unsigned int pixelID, status;

  const uint32_t* p = static_cast<const uint32_t*>(blob.startingAddress());

  m_module_status = *p;

  ++p;
  unsigned int specialRegionsStatus = *p;
  unsigned int integersRead = 2;
    
  if((specialRegionsStatus & mkey) != 0){
    m_chip_status.resize(mch);
  }
  if((specialRegionsStatus & (mkey << mch)) != 0){
    m_column_pair_status.resize(mch);
  }
  
  for(int i = 0; i < mch; i++){
    if((specialRegionsStatus & (1 << i)) != 0){
      ++p;
      ++integersRead;
      m_chip_status[i] = *p;
    }
    if((specialRegionsStatus & (1 << (i+mch))) != 0){
      m_column_pair_status[i].resize(mcolumns);
      for(int j = 0; j < (int)mcolumns; j++){
	++p;
	++integersRead;
	m_column_pair_status[i][j] = *p;
      }
    }
  }

  while( static_cast<int>(integersRead * sizeof(unsigned int)) < blob.size() ){
    ++p;
    ++integersRead;
    pixelID = *p;
    ++p;
    ++integersRead;
    status = *p;
    setPixelStatus(pixelID, status);
  }

  return false; 
}

unsigned int ModuleSpecialPixelMap::pixelType(unsigned int column, unsigned int row, unsigned int mchips){
  unsigned int i = std::min(mchips%10, nmtype-1);
  int mch = (int)mchips/10;
  unsigned int mcolumns = columnsPerFEIX[i];
  unsigned int mrowsrdo = rowsRdoPerFEIX[i];
  unsigned int mrows = rowsPerFEIX[i];
  int ng = (int)(mrows - mrowsrdo);

  if(mch>2){
    if(column > 0 && column < mcolumns-1 && row < mrowsrdo){
      if(ng>0){ 
	for(int k = 0; k<ng; ++k){ 
	  if(row==(mrowsrdo-2-2*k))return 5; // inter-ganged
	  if(row==(mrowsrdo-1-2*k)) return 2; // ganged 
	}
      }
      return 0; // normal
    }
    else if((column == 0 || column == mcolumns-1) && row < mrowsrdo){
      if(ng>0){ 
	for(int k = 0; k<ng; ++k){ 
	  if(row==(mrowsrdo-2-2*k))return 6; // long inter-ganged
	  if(row==(mrowsrdo-1-2*k)) return 3; // long ganged 
	}
      }
      return 1; //long 
    }
    else return 8; //invalid row/column pair
  }
  else { // IBL FEI4
    if( row<mrows&&column<mcolumns ){ 
      if( mch==2 && (column ==0 || column==(mcolumns-1)) ){
	return 1; //long
      }
      else{
	return 0; // normal
      }
    }
    else return 8; //invalid row/column pair 
  }	
}

/*
unsigned int ModuleSpecialPixelMap::pixelType(unsigned int column, unsigned int row){
  unsigned int mchips = 16;
  return pixelType(column, row, mchips);
}
*/


std::vector<unsigned int> ModuleSpecialPixelMap::decodePixelID(unsigned int pixelID, unsigned int typeFE){
  
  unsigned int chip = (pixelID & 0xF);
  pixelID >>=4;
  unsigned int column = (pixelID & 0x1F);
  (typeFE == 0) ? pixelID >>=5 : pixelID >>=8; //typeFE = 0 for FEI3 and 1 for FEI4
  unsigned int row = pixelID;

  std::vector<unsigned int> ChipColRow;
  ChipColRow.push_back(chip);
  ChipColRow.push_back(column);
  ChipColRow.push_back(row);
  
  return ChipColRow;

}


unsigned int ModuleSpecialPixelMap::encodePixelID(unsigned int chip, unsigned int column, unsigned int row, unsigned int mchips){
  int i = mchips%10; // type of module 
  unsigned int pixelID = row;
  pixelID = (i==0) ? pixelID << 5 : pixelID << 8;
  pixelID += column;
  pixelID = pixelID << 4;
  pixelID += chip;
  return pixelID;
}


unsigned int ModuleSpecialPixelMap::encodePixelID(unsigned int chip, unsigned int column, unsigned int row ){

  unsigned int mchips = 16*10;
  return encodePixelID(chip, column, row, mchips);
}


unsigned int ModuleSpecialPixelMap::encodePixelID(int component, unsigned int module_phi_index, 
						  unsigned int pixel_eta_index, unsigned int pixel_phi_index, unsigned int mchips){
  unsigned int i = std::min(mchips%10, nmtype-1);
  int mch = (int)mchips/10;
  unsigned int mcolumns = columnsPerFEIX[i];
  unsigned int mrowsrdo = rowsRdoPerFEIX[i];
  unsigned int mrows = rowsPerFEIX[i];
  int ng = (int)(mrows - mrowsrdo);

  unsigned int chip = 0, column = 0, row = 0;
  //
  if(std::abs(component)!=2) {   //not endcap
    if(mch>2){
      if(pixel_phi_index < mrows){
        chip = pixel_eta_index / mcolumns + mch/2;
        column = pixel_eta_index % mcolumns;
        if(pixel_phi_index < mrowsrdo)row = pixel_phi_index;
        else if(ng>0){
          for(int k=0; k<ng; ++k){ 
            if(pixel_phi_index ==(mrowsrdo+k))row = mrowsrdo+1 -2*(ng-k);
          }
        }
      }
      else if(pixel_phi_index >= mrows && pixel_phi_index < 2*mrows){
        chip = mch/2 - (1 + pixel_eta_index / mcolumns);
        column = mcolumns - (1+pixel_eta_index % mcolumns);
        if(pixel_phi_index > (mrows+ng-1))row = 2*mrows-1 - pixel_phi_index;
        else if(ng>0){
          for(int k = 0; k<ng; ++k){
            if(pixel_phi_index ==(mrows+k))row =mrowsrdo-1-2*k;
          }
        }
      }
    }
    else{ 
      row = (mrows-1) - pixel_phi_index; 
      column = mcolumns-(1 + pixel_eta_index%mcolumns);
      chip = mch==1 ? 0: 1-pixel_eta_index/mcolumns; 
    }
  }
  else{                    //disks
    if(module_phi_index % 2 == 0){                                //even modules
      if(pixel_phi_index < mrows){
        chip = mch/2 - (1+pixel_eta_index / mcolumns);
        column = mcolumns - (1+pixel_eta_index % mcolumns);
        if(pixel_phi_index < mrowsrdo)row = pixel_phi_index;
        else if(ng>0){
          for(int k=0; k<ng; ++k){ 
            if(pixel_phi_index ==(mrowsrdo+k)) row = mrowsrdo+1 -2*(ng-k);
          }
        }
      }
      else if(pixel_phi_index > (mrows-1) && pixel_phi_index < 2*mrows){
        chip = pixel_eta_index / mcolumns + mch/2;
        column = pixel_eta_index % mcolumns;
        if(pixel_phi_index > (mrows+ng-1))row = 2*mrows - 1 - pixel_phi_index;
        else if(ng>0){
          for(int k = 0; k<ng; ++k){
            if(pixel_phi_index ==(mrows+k))row =mrowsrdo-1-2*k;
          }
        }
      }
    }
    else if(module_phi_index % 2 == 1){                          //odd modules
      if(pixel_phi_index > (mrows+ng-1) && pixel_phi_index < 2*mrows){
        chip = mch/2 -(1 + pixel_eta_index / mcolumns);
        column = mcolumns - (1 + pixel_eta_index % mcolumns);
        if(pixel_phi_index > (mrows+ng-1))row = 2*mrows - 1 - pixel_phi_index;
        else if(ng>0){
          for(int k = 0; k<ng; ++k){
            if(pixel_phi_index ==(mrows+k))row =mrowsrdo-1-2*k;
          }
        }
      }
      else if(pixel_phi_index < mrows){
        chip = pixel_eta_index / mcolumns + mch/2;
        column = pixel_eta_index % mcolumns;
        if(pixel_phi_index < mrowsrdo)row = pixel_phi_index;
        else if(ng>0){ 
          for(int k=0; k<ng; ++k){ 
            if(pixel_phi_index ==(mrowsrdo+k)) row = mrowsrdo+1 -2*(ng-k);
          }
        }
      }
    }
  }
  unsigned int pixelID = row;
  pixelID = (i==0) ? pixelID << 5 : pixelID << 8;
  pixelID += column;
  pixelID = pixelID << 4;
  pixelID += chip;

  return pixelID;
}

/*
unsigned int ModuleSpecialPixelMap::encodePixelID(int component, unsigned int module_phi_index,
                                                  unsigned int pixel_eta_index, unsigned int pixel_phi_index){
  unsigned int mchips = 16;
  return encodePixelID(component, module_phi_index,
					      pixel_eta_index, pixel_phi_index,mchips);
}

*/

void ModuleSpecialPixelMap::markSpecialRegions(){

  // start with column pairs
  unsigned int mcolumns = columnsPerChip()/2;
  unsigned int mrows = rowsrdoPerChip();
  
  for(int chip = 0; chip < (int)chipsPerModule(); chip++){
    for(int column_pair = 0; column_pair < (int)mcolumns; column_pair++){
      int i = 0, row = 0;
      unsigned int column_pair_status = (pixelStatus(encodePixelID(chip, 2*column_pair, row,m_chipsPerModule)) & ~(15 << 25));
      
      if(column_pair_status != 0){
	while(i < 2 && (pixelStatus(encodePixelID(chip, 2*column_pair + i, row, m_chipsPerModule)) & ~(15 << 25)) == column_pair_status){
	  row++;
	  if(row == (int)mrows){
	    i++;
	    row = 0;
	  }
	}
	if(i == 2){
	  setColumnPairStatus(chip, column_pair, column_pair_status);
	}
      }
    }
  }

  // then mark special chips
  
  if(m_column_pair_status.size() != 0){
    for(int chip = 0; chip < (int)chipsPerModule(); chip++){
      if(m_column_pair_status[chip].size() != 0){
	unsigned int chip_status = (m_column_pair_status[chip][0] & ~(15 << 25));
	int i = 1;
	while(i < (int)mcolumns && ((m_column_pair_status[chip][i] & ~(15 << 25)) == chip_status)){
	  i++;
	}
	if(i == (int)mcolumns){
	  setChipStatus(chip, chip_status);
	  m_column_pair_status[chip].resize(0);
	}
	else{
	  setChipStatus(chip, 0);
	}
      }      
    }
    int j = 0;
    while(j < (int)chipsPerModule() && m_column_pair_status[j].size() == 0){
      j++;
    }
    if(j == (int)chipsPerModule()){
      m_column_pair_status.resize(0);
    }
  }    

  // finally see whether the whole module is special
  
  if(m_chip_status.size() != 0){
    unsigned int module_status = (m_chip_status[0] & ~(15 << 25));
    int i = 1;
    while(i < (int)chipsPerModule() && (m_chip_status[i] & ~(15 << 25)) == module_status){
      i++;
    }
    if(i == (int)chipsPerModule()){
      setModuleStatus(module_status);
      m_chip_status.resize(0);
      m_column_pair_status.resize(0);
    }
    else{
      setModuleStatus(0);
    }
  }
  return;
}

void ModuleSpecialPixelMap::setModuleStatus(unsigned int module_status){
  if(module_status != 0){
    clear();
  }
  m_module_status = (module_status & ~(15 << 25));
  return;
}

void ModuleSpecialPixelMap::setChipStatus(unsigned int chip, unsigned int chip_status){
  unsigned int mcolumns = columnsPerChip();
  unsigned int mrows = rowsrdoPerChip();

  if(chip_status != 0){
    for(int column = 0; column < (int)mcolumns; column++){
      for(int row = 0; row < (int)mrows; row ++){
	erase(encodePixelID(chip, column, row, m_chipsPerModule));
      }
    }
    m_chip_status.resize(chipsPerModule());
    m_chip_status[chip] = chip_status;
  }
  else if(m_chip_status.size() != 0){
    m_chip_status[chip] = (chip_status & ~(15 << 25));
  }

  return;
}

void ModuleSpecialPixelMap::setColumnPairStatus(unsigned int chip, unsigned int column_pair, unsigned int column_pair_status){
  unsigned int mcolumns = columnsPerChip()/2; 
  unsigned int mrows = rowsrdoPerChip();

  if(column_pair_status != 0){
    for(int i = 0; i < 2; i++){
      for(int row = 0; row < (int)mrows; row++){
	erase(encodePixelID(chip, 2*column_pair + i, row, m_chipsPerModule));
      }
    }
    m_column_pair_status.resize(chipsPerModule());
    m_column_pair_status[chip].resize(mcolumns);
    m_column_pair_status[chip][column_pair] = (column_pair_status & ~(15 << 25));
  }
  else if(m_column_pair_status.size() != 0 && m_column_pair_status[chip].size() != 0){
    m_column_pair_status[chip][column_pair] = column_pair_status;
  }

  return;
}

void ModuleSpecialPixelMap::setNeighbourFlags(){
  if(empty()){
    return;
  }
  else{
    //loop
    int mch = (int)chipsPerModule(); 
    int mco =mch>2? mch/2:mch; 
    for(int chip = 0; chip < mch; chip++){
      for(int column = 0; column < (int)columnsPerChip(); column++){
	for(int row = 0; row < (int)rowsrdoPerChip(); row++){
	  
	  //if this pixel is special...
	  const_iterator pixel = find(encodePixelID(chip, column, row, m_chipsPerModule));
	  if(pixel != end()){
	    if(((*pixel).second & ~(15 << 25)) != 0){
	      
	      //see if left neighbour is also special
	      if(column != 0 &&
		 (pixelStatus(encodePixelID(chip, column-1, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 27);
	      }
	      else if(mch>2&&column == 0 && chip != 0 && chip != mco &&
		      (pixelStatus(encodePixelID(chip-1, columnsPerChip()-1, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 27);
	      }
	      else if(mch==2&&column == 0 && chip != 0 &&
		      (pixelStatus(encodePixelID(chip-1, columnsPerChip()-1, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 27);
	      }
	      else{
		clearPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 27);
	      }

		//see if right neighbour is special
	      if(column != (int)(columnsPerChip()-1) &&
		   (pixelStatus(encodePixelID(chip, column+1, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		  setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 28);
		}
	      else if(mch>2&&column == (int)(columnsPerChip()-1) && chip != (mco-1) && chip != (mch-1) &&
			(pixelStatus(encodePixelID(chip+1, 0, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		  setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 28);
		}
	      else if(mch==2&&column == (int)(columnsPerChip()-1) && chip != 1 &&
			(pixelStatus(encodePixelID(chip+1, 0, row, m_chipsPerModule)) & ~(15 << 25)) != 0){
		  setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 28);
	      }
	      else{
		clearPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 28);
	      }

		//see if top neighbour is special
	      if(row != (int)(rowsrdoPerChip()-1) &&
		   (pixelStatus(encodePixelID(chip, column, row+1, m_chipsPerModule)) & ~(15 << 25)) != 0){
		  setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 26);
	      }
	      else if(mch>2&&row == (int)(rowsrdoPerChip()-1) &&
		      (pixelStatus(encodePixelID((mch-1-chip), (columnsPerChip()-1-column),(rowsrdoPerChip()-1), m_chipsPerModule)) & ~(15 << 25)) != 0){
		  setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 26);
	      }
	      else{
		clearPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 26);
	      }

	      //see if bottom neighbour is special
	      if(row != 0 && 
		 (pixelStatus(encodePixelID(chip, column, row-1, m_chipsPerModule)) & ~(15 << 25)) != 0){
		setPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 25);
	      }
	      else{
		clearPixelStatusBit(encodePixelID(chip, column, row, m_chipsPerModule), 25);
	      }
	    }
	  }
	}
      }
    }
    return;
  }
}


ModuleSpecialPixelMap::size_type ModuleSpecialPixelMap::size() const{
  return std::map<unsigned int, unsigned int>::size();
}

bool ModuleSpecialPixelMap::empty() const{
  return std::map<unsigned int, unsigned int>::empty();
}

ModuleSpecialPixelMap::iterator ModuleSpecialPixelMap::begin(){
  return std::map<unsigned int, unsigned int>::begin();
}

ModuleSpecialPixelMap::const_iterator ModuleSpecialPixelMap::begin() const{
  return std::map<unsigned int, unsigned int>::begin();
}

ModuleSpecialPixelMap::iterator ModuleSpecialPixelMap::end(){
  return std::map<unsigned int, unsigned int>::end();
}

ModuleSpecialPixelMap::const_iterator ModuleSpecialPixelMap::end() const{
  return std::map<unsigned int, unsigned int>::end();
}

bool ModuleSpecialPixelMap::hasSpecialChips() const{
  if(m_chip_status.size() == 0){
    return false;
  }
  else{
    for(std::vector<unsigned int>::const_iterator chip = m_chip_status.begin(); chip != m_chip_status.end(); ++chip){
      if(*chip != 0){
	return true;
      }
    }
  }

  return false;
}

bool ModuleSpecialPixelMap::hasSpecialColumnPairs() const{
  if(m_column_pair_status.size() == 0){
    return false;
  }
  else{
    
    for(std::vector<std::vector<unsigned int> >::const_iterator chip = m_column_pair_status.begin(); 
	chip != m_column_pair_status.end(); ++chip){
      
      for(std::vector<unsigned int>::const_iterator column_pair =
	    m_column_pair_status[chip - m_column_pair_status.begin()].begin(); 
	  column_pair != m_column_pair_status[chip - m_column_pair_status.begin()].end();
	  ++column_pair){
	
	if((*column_pair) != 0){
	  return true;
	}
      }
    }
  }

  return false;

}
  
bool ModuleSpecialPixelMap::hasSpecialColumnPairs(unsigned int chip) const{
  
  if(m_column_pair_status.size() == 0){
    return false;
  }
  else if(chip >= chipsPerModule()){
    std::cout << "Chip index " << chip << " out of range" << std::endl;
    return false;
  }
  else{
    for(std::vector<unsigned int>::const_iterator column_pair = m_column_pair_status[chip].begin(); 
	column_pair != m_column_pair_status[chip].end(); ++column_pair){
      
      if((*column_pair) != 0){
	return true;
      }
    }
  }

  return false;
}

} // namespace PixelCoralClientUtils
