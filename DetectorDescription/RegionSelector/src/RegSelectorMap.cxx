/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "RegionSelector/RegSelectorMap.h"
#include "PathResolver/PathResolver.h"
#include <cstring>
// using namespace std;

double RegSelectorMap::etaminValue(void){
  return m_etaminDet;
}

double RegSelectorMap::etamaxValue(void){
  return m_etamaxDet;
}

double RegSelectorMap::phiminValue(void){
  return m_phiminDet;
}

double RegSelectorMap::phimaxValue(void){
  return m_phimaxDet;
}

std::vector<IdentifierHash> RegSelectorMap::hashIdOut(void){
  return m_hashId;
}
std::vector<uint32_t> RegSelectorMap::robIdOut(void){
  return m_robId;
}

std::vector<int> RegSelectorMap::barORendOut(void){
  return m_barORend;
}

std::vector<int> RegSelectorMap::layORdskOut(void){
  return m_layORdsk;
}

std::vector<double> RegSelectorMap::etaMinOut(void){
  return m_etamin;
}
std::vector<double> RegSelectorMap::etaMaxOut(void){
  return m_etamax;
}
std::vector<double> RegSelectorMap::phiMinOut(void){
  return m_phimin;
}
std::vector<double> RegSelectorMap::phiMaxOut(void){
  return m_phimax;
}

void RegSelectorMap::mountDataStruct(void){
  int nLines = m_barORend.size();
  int i;

  for( i = 0; i < nLines; i++){ // creating list of RegSelectElementUint
    insertDataElement(m_barORend[i], m_layORdsk[i],
		      m_etamin[i], m_etamax[i],
		      m_phimin[i], m_phimax[i],
		      m_hashId[i], m_robId[i] );
  }
  std::list<RegSelectorMapElement>::iterator it;
  // find max and min values of each barrel layer and endcap disk
  for(it = m_barreldataList.begin();it != m_barreldataList.end(); it++)
    (*it).findMaxMinElem();
  for(it = m_posdataList.begin();it != m_posdataList.end(); it++)
    (*it).findMaxMinElem();
  for(it = m_negdataList.begin();it != m_negdataList.end(); it++)
    (*it).findMaxMinElem();
  // find max and min values of a subdetector
  m_etaminDet = 1000; m_etamaxDet = -1000;
  m_phiminDet = 1000; m_phimaxDet = -1000;
  findMaxMinValues(m_barreldataList);
  findMaxMinValues(m_posdataList);
  findMaxMinValues(m_negdataList);
}

void RegSelectorMap::regionSelectorRobIdUint(DETID TYPE, double& etaminIn, double& etamaxIn,
					    double& phiminIn, double& phimaxIn,std::vector<uint32_t>& outList){

  std::list<RegSelectorMapElement>::iterator it;
  std::set<uint32_t> outset;
  std::set<uint32_t>::iterator itset, negbarrelbeg, negbarrelend;
  bool divideEta = false;

  for(it = m_barreldataList.begin(); //barrel list
      it != m_barreldataList.end(); it++){ // runs trought all RegSelectElementUints
	   (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
  }

  switch(TYPE){
  case PIXEL: divideEta = true; break;
  case SCT: divideEta = true; break;
  case TRT: divideEta = false; break;
  case LAR: break;
  case TTEM: break;
  case TTHEC: break;
  case FCALEM: break;
  case FCALHAD: break;
  case TILE: divideEta = true; break;
  case MDT: divideEta = true; break;
  case RPC: divideEta = true; break;
  case TGC: divideEta = true; break;
  case CSC: divideEta = true; break;
  default: break;
  }

  if( divideEta ){
    if(etaminIn < 0 || etamaxIn < 0){ // negative data list
      for(it = m_negdataList.begin();it != m_negdataList.end(); it++){ // runs trought all RegSelectElementUints
        (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }

    if( etaminIn >= 0 || etamaxIn >= 0 ){ // positive data list
      for(it = m_posdataList.begin();it != m_posdataList.end(); it++){ // runs trought all RegSelectElementUints
        (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }
  }
  else{
    for(it = m_negdataList.begin();it != m_negdataList.end(); it++){ // runs trought all RegSelectElementUints
      (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
    for(it = m_posdataList.begin();it != m_posdataList.end(); it++){ // runs trought all RegSelectElementUints
      (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
  }

  if (TYPE==TILE){
	negbarrelbeg = outset.upper_bound(0x51000f);
	negbarrelend = outset.lower_bound(0x530000);
	for(itset = negbarrelbeg; itset != negbarrelend; itset++){
	  outList.push_back(*itset);
	}
	for(itset = outset.begin(); itset != negbarrelbeg; itset++){
	  outList.push_back(*itset);
	}
	for(itset = negbarrelend; itset != outset.end(); itset++){
	  outList.push_back(*itset);
	}
  }else{
  for(itset = outset.begin(); itset != outset.end(); itset++){
    outList.push_back(*itset);
  }
  }

}

void RegSelectorMap::addLut(const RegionSelectorLUT &detLut){
  unsigned int i;

  for( i=0; i< detLut.maxHash(); i++){
    writeLine(detLut.layerDiskPosition(i),
              detLut.layerDiskNumber(i),
	      detLut.hashId(i),
	      detLut.robId(i),
              detLut.etaMin(i),
              detLut.etaMax(i),
              detLut.phiMin(i),
              detLut.phiMax(i));
    //    std::cout << "RSDEBUG i=" << i << " hashid=" << detLut.hashId(i) << std::endl;
  }

}

void RegSelectorMap::regionSelector(DETID TYPE, double& etaminIn, double& etamaxIn,
					    double& phiminIn, double& phimaxIn,std::vector<IdentifierHash>& outList){

  std::list<RegSelectorMapElement>::iterator it;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::iterator itset, negbarrelbeg,negbarrelend;
  bool divideEta = false;

  for(it = m_barreldataList.begin(); //barrel list
      it != m_barreldataList.end(); it++){ // runs trought all RegSelectElementUints
	   (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
  }

  switch(TYPE){
  case PIXEL: divideEta = true; break;
  case SCT: divideEta = true; break;
  case TRT: divideEta = false; break;
  case LAR: break;
  case TTEM: break;
  case TTHEC: break;
  case FCALEM: break;
  case FCALHAD: break;
  case TILE: divideEta = true; break;
  case MDT: divideEta = true; break;
  case RPC: divideEta = true; break;
  case TGC: divideEta = true; break;
  case CSC: divideEta = true; break;
  default: break;
  }

  if( divideEta ){
    if(etaminIn < 0 || etamaxIn < 0){ // negative data list
      for(it = m_negdataList.begin();it != m_negdataList.end(); it++){ // runs trought all RegSelectElementUints
        (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }

    if( etaminIn >= 0 || etamaxIn >= 0 ){ // positive data list
      for(it = m_posdataList.begin();it != m_posdataList.end(); it++){ // runs trought all RegSelectElementUints
        (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }
  }
  else{
    for(it = m_negdataList.begin();it != m_negdataList.end(); it++){ // runs trought all RegSelectElementUints
      (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
    for(it = m_posdataList.begin();it != m_posdataList.end(); it++){ // runs trought all RegSelectElementUints
      (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
  }

  if (TYPE==TILE){
	negbarrelbeg = outset.upper_bound(0x3f);
	negbarrelend = outset.lower_bound(0x80);
	for(itset = negbarrelbeg; itset != negbarrelend; itset++){
	  outList.push_back(*itset);
	}
	for(itset = outset.begin(); itset != negbarrelbeg; itset++){
	  outList.push_back(*itset);
	}
	for(itset = negbarrelend; itset != outset.end(); itset++){
	  outList.push_back(*itset);
	}
  } else{
  for(itset = outset.begin(); itset != outset.end(); itset++){
    outList.push_back(*itset);
  }
  }

}

void RegSelectorMap::regionSelector(TYPEID typeinID, double& etaminIn, double& etamaxIn, double& phiminIn,
                      double& phimaxIn, std::vector<IdentifierHash>& outList){
  std::list<RegSelectorMapElement>::iterator it;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::iterator itset;
  bool posORneg;


 if( etaminIn > 0 && etamaxIn > 0 ){ //search positive list to find TYPEID
   posORneg = false; // it's POSITIVE
   findPosition(typeinID,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }
 else if( etaminIn < 0 && etamaxIn < 0 ){ //search negative list to find TYPEID
   posORneg = true; // it's NEGATIVE
   findPosition(typeinID,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }
 else { //search both list to find TYPEID
   posORneg = false;
   findPosition(typeinID,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
   posORneg = true;
   findPosition(typeinID,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }

  for(itset = outset.begin(); itset != outset.end(); itset++){
    outList.push_back(*itset);
  }
}

void RegSelectorMap::regionSelector(long layNumber, double& etaminIn, double& etamaxIn, double& phiminIn,
                      double& phimaxIn, std::vector<IdentifierHash>& outList){
  std::list<RegSelectorMapElement>::iterator it, itEnd;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::iterator itset;
  bool posORneg;


 if( etaminIn > 0 && etamaxIn > 0 ){ //search positive list to find layNumber
   posORneg = false; // it's POSITIVE
   findPosition(layNumber,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }
 else if( etaminIn < 0 && etamaxIn < 0 ){ //search negative list to find layNumber
   posORneg = true; // it's NEGATIVE
   findPosition(layNumber,posORneg, it);
   (*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }

  // barrel
  it = m_barreldataList.begin(); 
  itEnd = m_barreldataList.end();

   while(it != itEnd) {
     if( (*it).layerDiskNumber() == layNumber )
       break;
     it++;
   }
	(*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
	
  for(itset = outset.begin(); itset != outset.end(); itset++){
    outList.push_back(*itset);
  }
  
}

StatusCode RegSelectorMap::read(const char *filename, DETID type){

  StatusCode sc = StatusCode::SUCCESS;

  // use path resolver to find full path to file
  std::string unresolvedFileName(filename);
  std::string fullFileName = PathResolver::find_file (unresolvedFileName, "DATAPATH");
  //log << MSG::DEBUG << "PathResolver found " << fullFileName << endmsg;
  if (fullFileName == "") {
    //log << MSG::FATAL << "Could not find input file in DATAPATH" <<  unresolvedFileName<< endmsg;
    std::cerr << "RegSelectorMap: FATAL: Could not find input file in DATAPATH" <<  unresolvedFileName<< std::endl;
    return StatusCode::FAILURE;
  }
  //std::cout << "RegSelectDataIdentifierHash::read fullFileName=" << fullFileName << std::endl;

  switch(type){
  case PIXEL:
    break;
  case SCT:
    break;
  case TRT:
    break;
 case TILE:
    sc = readTILE(fullFileName.c_str());
    break;
  case MDT:
    sc = readMuon(MDET_MDT,fullFileName.c_str());
    break;
  case RPC:
    sc = readMuon(MDET_RPC,fullFileName.c_str());
    break;
  case TGC:
    sc = readMuon(MDET_TGC,fullFileName.c_str());
    break;
  case CSC:
    sc = readMuon(MDET_CSC,fullFileName.c_str());
    break;
  default:
    sc = StatusCode::SUCCESS;
  }
  return sc;
}

void RegSelectorMap::summaryDataFile(std::list<RegSelectorMapElement> &dataList){
  std::list<RegSelectorMapElement>::iterator it;

  for(it = dataList.begin(); it != dataList.end(); it++){ // runs through entire list
    std::cout << "position: " << (*it).layerDiskPosition() << " number: " << (*it).layerDiskNumber() << std::endl;
    std::cout << "etamin: " << m_etaminDet << "  etamax: " << m_etamaxDet << std::endl;
    std::cout << "phimin: " << m_phiminDet << "  phimax: " << m_phimaxDet << std::endl;
    std::cout << "hashId: ";
    std::vector<IdentifierHash> aux = (*it).hashId();
    for(std::vector<IdentifierHash>::iterator i = aux.begin(); i != aux.end(); i++)
      std::cout << (*i) << " ";
    std::cout << std::endl;
    std::cin.get();
  }
}

StatusCode RegSelectorMap::verifyInputs(double &etaminIn, double &etamaxIn,
				 double &phiminIn, double &phimaxIn){

  StatusCode sc = StatusCode::SUCCESS;

  while (phiminIn > 2*M_PI) phiminIn -= 2*M_PI;
  while (phiminIn < 0     ) phiminIn += 2*M_PI;
  while (phimaxIn > 2*M_PI) phimaxIn -= 2*M_PI;
  while (phimaxIn < 0     ) phimaxIn += 2*M_PI;

  if( ( (etaminIn < m_etaminDet) && (etamaxIn < m_etaminDet) ) ||
      ( (etaminIn > m_etamaxDet) && (etamaxIn > m_etamaxDet) ) ){
    //sc = StatusCode::FAILURE;
  }
  else{
    if( (etaminIn < m_etaminDet) && (etamaxIn > m_etaminDet) ){
        etaminIn = m_etaminDet;
    }
    if( (etamaxIn > m_etamaxDet) && (etaminIn < m_etamaxDet) ){
        etamaxIn = m_etamaxDet;
    }
  }

  return sc;
}

StatusCode RegSelectorMap::verifyInputsMinusPi(double &etaminIn, double &etamaxIn,
				 double &phiminIn, double &phimaxIn){

  StatusCode sc = StatusCode::SUCCESS;

  while (phiminIn > M_PI)   phiminIn -= 2*M_PI;
  while (phiminIn < -M_PI)  phiminIn += 2*M_PI;
  while (phimaxIn > M_PI)   phimaxIn -= 2*M_PI;
  while (phimaxIn < -M_PI)  phimaxIn += 2*M_PI;

  if( ( (etaminIn < m_etaminDet) && (etamaxIn < m_etaminDet) ) ||
      ( (etaminIn > m_etamaxDet) && (etamaxIn > m_etamaxDet) ) ){
    //sc = StatusCode::FAILURE;
  }
  else{
    if( (etaminIn < m_etaminDet) && (etamaxIn > m_etaminDet) ){
        etaminIn = m_etaminDet;
    }
    if( (etamaxIn > m_etamaxDet) && (etaminIn < m_etamaxDet) ){
        etamaxIn = m_etamaxDet;
    }
  }

  return sc;
}

void RegSelectorMap::verifyOutput(double etaminIn, double etamaxIn,
				 double phiminIn, double phimaxIn,
				 std::vector<IdentifierHash> outputIdlist){
  std::vector<IdentifierHash> outset;
  int i;
  unsigned int j;
  double etamin, etamax, phimin, phimax;
  int vecsize = m_etamin.size();
  for( i= 0; i < vecsize; i++){
    if(m_etamin[i] > m_etamax[i]){
      etamin = m_etamax[i]; etamax = m_etamin[i];
    }
    else{
      etamin = m_etamin[i]; etamax = m_etamax[i];
    }
    if(m_phimin[i] > m_phimax[i]){
      phimin = m_phimin[i];
      phimax = phimaxValue();
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.push_back(m_hashId[i]);
	}
      }
      phimin = phiminValue();
      phimax = m_phimax[i];
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.push_back(m_hashId[i]);
	}
      }
    }
    else{
      phimin = m_phimin[i]; phimax = m_phimax[i];
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.push_back(m_hashId[i]);
	}
      }
    }
  }
  if(outset == outputIdlist)
    std::cout << "equal vectors " << std::endl;
  std::cout << "desired output ";
  for( j=0; j < outset.size(); j++)
    std::cout << std::dec << outset[j] << " ";
  std::cout << std::endl;
  std::cout << "obtained output ";
  for( j=0; j < outputIdlist.size(); j++)
    std::cout << std::dec << outputIdlist[j] << " ";
  std::cout << std::endl;
}

void RegSelectorMap::regionSelectorRobIdUint(double& etaminIn, double& etamaxIn, double& phiminIn,
  			       double& phimaxIn, std::vector<uint32_t>& outList){
  std::set<uint32_t> outset;
  int i;
  unsigned int j;
  double etamin, etamax, phimin, phimax;
  std::vector<uint32_t> outvec;

  int vecsize = m_etamin.size();
  for( i= 0; i < vecsize; i++){
    if(m_etamin[i] > m_etamax[i]){
      etamin = m_etamax[i]; etamax = m_etamin[i];
    }
    else{
      etamin = m_etamin[i]; etamax = m_etamax[i];
    }
    if(m_phimin[i] > m_phimax[i]){
      phimin = m_phimin[i];
      phimax = phimaxValue();
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.insert(m_robId[i]);
	}
      }
      phimin = phiminValue();
      phimax = m_phimax[i];
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.insert(m_robId[i]);
	}
      }
    }
    else{
      phimin = m_phimin[i]; phimax = m_phimax[i];
      if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
	if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
	  outset.insert(m_robId[i]);
	}
      }
    }
  }

  std::set<uint32_t>::iterator it;
  for(it = outset.begin(); it != outset.end(); it++)
    outvec.push_back(*it);

  if(outvec == outList)
    std::cout << "equal vectors " << std::endl;
  std::cout << "desired output ";
  for( j=0; j < outvec.size(); j++)
    std::cout << std::dec << outvec[j] << " ";
  std::cout << std::endl;
  std::cout << "obtained output ";
  for( j=0; j < outList.size(); j++)
    std::cout << std::dec << outList[j] << " ";
  std::cout << std::endl;
}

void RegSelectorMap::regionSelectorRobIdUint(long layNumber,double& etaminIn, double& etamaxIn, 
					double& phiminIn, double& phimaxIn, std::vector<uint32_t>& outList){
  std::set<uint32_t> outset;
  std::list<RegSelectorMapElement>::iterator it, itEnd;
  std::set<uint32_t>::iterator itset;
  bool posORneg;


 if( etaminIn > 0 && etamaxIn > 0 ){ //search positive list to find layNumber
   posORneg = false; // it's POSITIVE
   findPosition(layNumber,posORneg, it);
   (*it).selectionRobIdUint( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }
 else if( etaminIn < 0 && etamaxIn < 0 ){ //search negative list to find layNumber
   posORneg = true; // it's NEGATIVE
   findPosition(layNumber,posORneg, it);
   (*it).selectionRobIdUint( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
 }

  // barrel
  it = m_barreldataList.begin(); 
  itEnd = m_barreldataList.end();

   while(it != itEnd) {
     if( (*it).layerDiskNumber() == layNumber )
       break;
     it++;
   }
	(*it).selectionRobIdUint( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
	
  for(itset = outset.begin(); itset != outset.end(); itset++){
    outList.push_back(*itset);
  }
  
}

void RegSelectorMap::findPosition(TYPEID& typeinID, bool& posORneg,
				  std::list<RegSelectorMapElement>::iterator& it){
  
  std::list<RegSelectorMapElement>::iterator itPos, itNeg, itEnd;

  itPos = m_posdataList.begin(); // positioning iterator at the begining of POSITIVE eta value list
				 // each element of this list is a EtaPhiMap of a MDT chamber
  itNeg = m_negdataList.begin(); // NEGATIVE eta value list of MDT chambers

   if(posORneg == false){ // it's positive
     it = itPos;
     itEnd = m_posdataList.end();
   }
   else{
     it = itNeg;
     itEnd = m_negdataList.end();
   }

   while(it != itEnd) {
     if( (*it).layerDiskNumber() == typeinID )
       break;
     it++;
   }

}

void RegSelectorMap::findPosition(long& layNumber, bool& posORneg,
				  std::list<RegSelectorMapElement>::iterator& it){
  
  std::list<RegSelectorMapElement>::iterator itPos, itNeg, itEnd;

  itPos = m_posdataList.begin(); // positioning iterator at the begining of POSITIVE eta value list
  itNeg = m_negdataList.begin(); // NEGATIVE eta value list 

   if(posORneg == false){ // it's positive
     it = itPos;
     itEnd = m_posdataList.end();
   }
   else{
     it = itNeg;
     itEnd = m_negdataList.end();
   }

   while(it != itEnd) {
     if( (*it).layerDiskNumber() == layNumber )
       break;
     it++;
   }

}

void RegSelectorMap::findMaxMinValues(std::list<RegSelectorMapElement> &dataList){
  std::list<RegSelectorMapElement>::iterator it;

  for(it = dataList.begin(); it != dataList.end(); it++){ // runs through entire list
    if( (*it).etaminElem() < m_etaminDet )
      m_etaminDet = (*it).etaminElem();
    if( (*it).etamaxElem() > m_etamaxDet )
      m_etamaxDet = (*it).etamaxElem();
    if( (*it).phiminElem() < m_phiminDet )
      m_phiminDet = (*it).phiminElem();
    if( (*it).phimaxElem() > m_phimaxDet )
      m_phimaxDet = (*it).phimaxElem();
  }

}

RegSelectorMapElement RegSelectorMap::creatingElement( int&    positionIn, int&    numberIn, //????
						       double& etaminIn,   double& etamaxIn,
						       double& phiminIn,   double& phimaxIn,
						       IdentifierHash& hashIdIn, uint32_t& robIdIn ){
  RegSelectorMapElement newElement;

  newElement.additem( hashIdIn, 
		      etaminIn, etamaxIn,
	      	      phiminIn, phimaxIn, 
		      numberIn, positionIn, robIdIn );

  return newElement;
}

void RegSelectorMap::insertList(std::list<RegSelectorMapElement> &dataList, int positionIn,
					     int numberIn, double etaminIn, double etamaxIn,
			                     double phiminIn, double phimaxIn,
			                     IdentifierHash hashIdIn, uint32_t& robIdIn){
  bool flag = false;
  RegSelectorMapElement newElement;
  std::list<RegSelectorMapElement>::iterator it;

  for(it = dataList.begin(); it != dataList.end(); it++){ // runs through entire positive list
    if( positionIn == (*it).layerDiskPosition() ){ // position already exist
      flag = false;
      if( numberIn == (*it).layerDiskNumber() ) {// number already exist
	(*it).additem( hashIdIn, etaminIn, etamaxIn,
	      	       phiminIn, phimaxIn, numberIn, positionIn, robIdIn );
        break;
      }
      else
	flag = true;
    }
    else // position & number doesn't exist
      flag = true;
  } // end for

  if(flag == true || dataList.size() == 0){ // inserting new element
    newElement = creatingElement(positionIn, numberIn,
				 etaminIn,etamaxIn,
				 phiminIn, phimaxIn,
				 hashIdIn, robIdIn);
    dataList.push_back( newElement );
  }
}

void RegSelectorMap::insertDataElement(int& positionIn, int& numberIn, double& etaminIn, double& etamaxIn,
				      double& phiminIn, double& phimaxIn, IdentifierHash& hashIdIn, uint32_t& robIdIn){
  if(positionIn > 0 ){ // list of positive values
    insertList(m_posdataList, positionIn, numberIn,
	       etaminIn, etamaxIn, phiminIn, phimaxIn, hashIdIn, robIdIn);
  }
  else if(positionIn < 0){ // list of negative values
    insertList(m_negdataList, positionIn, numberIn,
	       etaminIn, etamaxIn, phiminIn, phimaxIn, hashIdIn, robIdIn);
  }
  else if(positionIn == 0){
    insertList(m_barreldataList, positionIn, numberIn,
	       etaminIn, etamaxIn, phiminIn, phimaxIn, hashIdIn, robIdIn);
  }
}

void RegSelectorMap::writeLine(int barORend, int layORdsk, IdentifierHash hashId, uint32_t robId, 
		double emin, double emax,
		 double pmin, double pmax){

  m_barORend.push_back(barORend);
  m_layORdsk.push_back(layORdsk);
  m_hashId.push_back(hashId);
  m_robId.push_back(robId);
  m_etamin.push_back(emin);
  m_etamax.push_back(emax);
  m_phimin.push_back(pmin);
  m_phimax.push_back(pmax);
}

StatusCode RegSelectorMap::readTILE(const char *filename){
  StatusCode sc = StatusCode::SUCCESS;
  char  buffer_[256];
  char *buffer = buffer_;
  int barORend =0, layORwhe =0;
  IdentifierHash  hashId;
  double emin, emax, pmin, pmax;
  uint32_t robid, collid;

  std::ifstream fin(filename);
  if(fin.bad()){  //Test if the file failed:
    sc = StatusCode::FAILURE;
    return sc;
  }
  fin.getline(buffer,128,'\n');
  while (fin.getline(buffer, 128, '\n')){
    if (strncmp(buffer,"#",1)!=0){
      sscanf(buffer, "%x %x %x %lf %lf %lf %lf", &robid, &collid, (unsigned int*)&hashId, &emin, &emax, &pmin, &pmax);
      writeLine(barORend, layORwhe, hashId, robid, emin, emax, pmin, pmax);
    }
  }
  fin.close();
  
  return sc;
}

StatusCode RegSelectorMap::readMuon(MDET mtype, const char *filename){
  StatusCode sc = StatusCode::SUCCESS;
  char  buffer_[256];
  char *buffer = buffer_;
  std::ifstream fin(filename);
   if(fin.bad()){  //Test if the file failed:
     sc = StatusCode::FAILURE;
     return sc;
   }
  fin.getline(buffer,128,'\n');
  switch(mtype){
  case MDET_MDT:
    readMDTData(fin);
    break;
  case MDET_RPC:
    readRPCData(fin);
    break;
  case MDET_TGC:
    readMDTData(fin);
    break;
  case MDET_CSC:
    readMDTData(fin);
    break;
  default: 
    break;
  }
  fin.close();

  return sc;
}

void RegSelectorMap::readMDTData(std::ifstream &fin){
 int aux[5];
 int barORend, layORdsk;
 IdentifierHash hashId;
 double emin, emax, pmin, pmax;
 char  buffer_[256];
 char *buffer = buffer_;
 while (fin.getline(buffer, 128, '\n')){
   sscanf(buffer, "%d/%d/%d/%d/%d %u %lf %lf %lf %lf", &aux[0], &aux[1], &aux[2], &aux[3],&aux[4],(unsigned int*)&hashId, &emin, &emax, &pmin, &pmax);
   aux[1] += 1;
   if(aux[2] > 0) barORend = 1*aux[1]; // positive eta values
   else barORend = -1*aux[1]; // negative eta values
   layORdsk = aux[1];
   writeLine(barORend, layORdsk, hashId, 0, emin, emax, pmin, pmax);
  }
}

void RegSelectorMap::readRPCData(std::ifstream &fin){
  int aux[6];
  int barORend, layORdsk;
  IdentifierHash hashId;
  double emin, emax, pmin, pmax;
  char buffer_[256];
  char *buffer = buffer_;

  while (fin.getline(buffer, 128, '\n')){
    sscanf(buffer, "%d/%d/%d/%d/%d/%d %u %lf %lf %lf %lf", &aux[0], &aux[1], &aux[2],&aux[3],&aux[4],
	   &aux[5], (unsigned int*)&hashId, &emin, &emax, &pmin, &pmax);
    if (aux[1] == 0) aux[1] = 11;
    if(aux[2] > 0) barORend = 1*aux[1];
    else barORend = -1*aux[1];
    layORdsk = aux[1];
    writeLine(barORend, layORdsk, hashId, 0, emin, emax, pmin, pmax);
  }
}

void RegSelectorMap::getEtaPhi(IdentifierHash hashId, 
		double *etaMin, double *etaMax,
		double *phiMin, double *phiMax){

	(*etaMin) = m_etamin[hashId];
	(*etaMax) = m_etamax[hashId];
	(*phiMin) = m_phimin[hashId];
	(*phiMax) = m_phimax[hashId];
}
