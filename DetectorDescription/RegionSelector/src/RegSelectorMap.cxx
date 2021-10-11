/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "RegionSelector/RegSelectorMap.h"
#include "PathResolver/PathResolver.h"
#include <cstring>




/// implementation of the IRegSelUT interface - intentionally inlined                                                                               

/// hash id methods                                                                                                                                 

void RegSelectorMap::HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {
  if ( roi.isFullscan() ) regionSelector( TILE, -4.9, 4.9, 0, 2*M_PI, idlist );
  regionSelector( TILE, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist );
}

void RegSelectorMap::HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {;
  if ( roi.isFullscan() ) regionSelector( layer, -4.9, 4.9, 0, 2*M_PI, idlist );
  regionSelector( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist );
}

/// Rob identifier methods                                                                                                                          

void RegSelectorMap::ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {
  if ( roi.isFullscan() ) regionSelectorRobIdUint( TILE, -4.9, 4.9, 0, 2*M_PI, roblist );
  regionSelectorRobIdUint( TILE, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist );
}

void RegSelectorMap::ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {
  if ( roi.isFullscan() ) regionSelectorRobIdUint( layer, -4.9, 4.9, 0, 2*M_PI, roblist );
  regionSelectorRobIdUint( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist );
}


/// the rest of the class starts here

double RegSelectorMap::etaminValue(void) const {
  return m_etaminDet;
}

double RegSelectorMap::etamaxValue(void) const {
  return m_etamaxDet;
}

double RegSelectorMap::phiminValue(void) const {
  return m_phiminDet;
}

double RegSelectorMap::phimaxValue(void) const {
  return m_phimaxDet;
}

const std::vector<IdentifierHash>& RegSelectorMap::hashIdOut(void) const {
  return m_hashId;
}

const std::vector<uint32_t>& RegSelectorMap::robIdOut(void) const {
  return m_robId;
}

const std::vector<int>& RegSelectorMap::barORendOut(void) const {
  return m_barORend;
}

const std::vector<int>& RegSelectorMap::layORdskOut(void) const {
  return m_layORdsk;
}

const std::vector<double>& RegSelectorMap::etaMinOut(void) const {
  return m_etamin;
}

const std::vector<double>& RegSelectorMap::etaMaxOut(void) const {
  return m_etamax;
}

const std::vector<double>& RegSelectorMap::phiMinOut(void) const {
  return m_phimin;
}

const std::vector<double>& RegSelectorMap::phiMaxOut(void) const {
  return m_phimax;
}

void RegSelectorMap::mountDataStruct(void){
  int nLines = m_barORend.size();
  int i;

  for( i = 0; i < nLines; ++i){ // creating list of RegSelectElementUint
    insertDataElement(m_barORend[i], m_layORdsk[i],
		      m_etamin[i], m_etamax[i],
		      m_phimin[i], m_phimax[i],
		      m_hashId[i], m_robId[i] );
  }
  std::list<RegSelectorMapElement>::iterator it;
  // find max and min values of each barrel layer and endcap disk
  for(it = m_barreldataList.begin();it != m_barreldataList.end(); ++it)
    (*it).findMaxMinElem();
  for(it = m_posdataList.begin();it != m_posdataList.end(); ++it)
    (*it).findMaxMinElem();
  for(it = m_negdataList.begin();it != m_negdataList.end(); ++it)
    (*it).findMaxMinElem();
  // find max and min values of a subdetector
  m_etaminDet = 1000; m_etamaxDet = -1000;
  m_phiminDet = 1000; m_phimaxDet = -1000;
  findMaxMinValues(m_barreldataList);
  findMaxMinValues(m_posdataList);
  findMaxMinValues(m_negdataList);
}

void RegSelectorMap::regionSelectorRobIdUint( DETID TYPE, 
					      double etaminIn, double etamaxIn,
					      double phiminIn, double phimaxIn,
					      std::vector<uint32_t>& outList ) const {
  
  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::list<RegSelectorMapElement>::const_iterator it;
  std::set<uint32_t> outset;
  std::set<uint32_t>::const_iterator itset, negbarrelbeg, negbarrelend;
  bool divideEta = false;

  for( it = m_barreldataList.begin(); //barrel list
       it != m_barreldataList.end(); ++it){ // runs trought all RegSelectElementUints
	   (*it).selectionRobIdUint( etaminIn, etamaxIn, phiminIn, phimaxIn, outset );
  }

  switch(TYPE){
  case TILE: divideEta = true; break;
  default: break;
  }

  if( divideEta ){
    if(etaminIn < 0 || etamaxIn < 0){ // negative data list
      for(it = m_negdataList.begin();it != m_negdataList.end(); ++it){ // runs trought all RegSelectElementUints
        (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }

    if( etaminIn >= 0 || etamaxIn >= 0 ){ // positive data list
      for(it = m_posdataList.begin();it != m_posdataList.end(); ++it){ // runs trought all RegSelectElementUints
        (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }
  }
  else{
    for(it = m_negdataList.begin();it != m_negdataList.end(); ++it){ // runs trought all RegSelectElementUints
      (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
    for(it = m_posdataList.begin();it != m_posdataList.end(); ++it){ // runs trought all RegSelectElementUints
      (*it).selectionRobIdUint(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
  }

  if (TYPE==TILE){
	negbarrelbeg = outset.upper_bound(0x51000f);
	negbarrelend = outset.lower_bound(0x530000);
	for(itset = negbarrelbeg; itset != negbarrelend; ++itset){
	  outList.push_back(*itset);
	}
	for(itset = outset.begin(); itset != negbarrelbeg; ++itset){
	  outList.push_back(*itset);
	}
	for(itset = negbarrelend; itset != outset.end(); ++itset){
	  outList.push_back(*itset);
	}
  }else{
  for(itset = outset.begin(); itset != outset.end(); ++itset){
    outList.push_back(*itset);
  }
  }

}

void RegSelectorMap::addLut( const RegionSelectorLUT* detLut ){


  for( size_t i=0; i< detLut->maxHash(); ++i) {
    writeLine(detLut->layerDiskPosition(i),
              detLut->layerDiskNumber(i),
	      detLut->hashId(i),
	      detLut->robId(i),
              detLut->etaMin(i),
              detLut->etaMax(i),
              detLut->phiMin(i),
              detLut->phiMax(i));
    //    std::cout << "RSDEBUG i=" << i << " hashid=" << detLut.hashId(i) << std::endl;
  }

}

void RegSelectorMap::regionSelector( DETID TYPE, 
				     double etaminIn,  double etamaxIn,
				     double phiminIn,  double phimaxIn,
				     std::vector<IdentifierHash>& outList) const {
  
  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::list<RegSelectorMapElement>::const_iterator it;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::const_iterator itset, negbarrelbeg,negbarrelend;
  bool divideEta = false;

  for(it = m_barreldataList.begin(); //barrel list
      it != m_barreldataList.end(); ++it){ // runs trought all RegSelectElementUints
	   (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
  }

  switch(TYPE){
  case TILE: divideEta = true; break;
  default: break;
  }

  if( divideEta ){
    if(etaminIn < 0 || etamaxIn < 0){ // negative data list
      for(it = m_negdataList.begin();it != m_negdataList.end(); ++it){ // runs trought all RegSelectElementUints
        (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }

    if( etaminIn >= 0 || etamaxIn >= 0 ){ // positive data list
      for(it = m_posdataList.begin();it != m_posdataList.end(); ++it){ // runs trought all RegSelectElementUints
        (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
      }
    }
  }
  else{
    for(it = m_negdataList.begin();it != m_negdataList.end(); ++it){ // runs trought all RegSelectElementUints
      (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
    for(it = m_posdataList.begin();it != m_posdataList.end(); ++it){ // runs trought all RegSelectElementUints
      (*it).selection(etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
    }
  }

  if (TYPE==TILE){
	negbarrelbeg = outset.upper_bound(0x3f);
	negbarrelend = outset.lower_bound(0x80);
	for(itset = negbarrelbeg; itset != negbarrelend; ++itset){
	  outList.push_back(*itset);
	}
	for(itset = outset.begin(); itset != negbarrelbeg; ++itset){
	  outList.push_back(*itset);
	}
	for(itset = negbarrelend; itset != outset.end(); ++itset){
	  outList.push_back(*itset);
	}
  } else{
  for(itset = outset.begin(); itset != outset.end(); ++itset){
    outList.push_back(*itset);
  }
  }

}

 void RegSelectorMap::regionSelector( TYPEID typeinID, 
				      double etaminIn, double etamaxIn,
				      double phiminIn, double phimaxIn,
				      std::vector<IdentifierHash>& outList) const {

  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::list<RegSelectorMapElement>::const_iterator it;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::const_iterator itset;
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

  for(itset = outset.begin(); itset != outset.end(); ++itset){
    outList.push_back(*itset);
  }
}



void RegSelectorMap::regionSelector( long layNumber, 
				     double etaminIn, double etamaxIn,
				     double phiminIn, double phimaxIn,
				     std::vector<IdentifierHash>& outList) const {

  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::list<RegSelectorMapElement>::const_iterator it, itEnd;
  std::set<IdentifierHash> outset;
  std::set<IdentifierHash>::const_iterator itset;
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
     ++it;
   }
	(*it).selection( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
	
  for(itset = outset.begin(); itset != outset.end(); ++itset){
    outList.push_back(*itset);
  }
  
}

StatusCode RegSelectorMap::read(const char *filename, DETID type) {

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
  case TILE:
    sc = readTILE(fullFileName.c_str());
    break;
  default:
    /// can't call the msg stream in this class - oh dear leave the code in place for the 
    /// time being until we sort out a solution 
    //    ATH_MSG_ERROR( "Don't EVER use this code for anything except the Tile Calorimeter" );
    sc = StatusCode::FAILURE;
  }
  return sc;
}

void RegSelectorMap::summaryDataFile(std::list<RegSelectorMapElement> &dataList){
  std::list<RegSelectorMapElement>::iterator it;

  for(it = dataList.begin(); it != dataList.end(); ++it){ // runs through entire list
    std::cout << "position: " << (*it).layerDiskPosition() << " number: " << (*it).layerDiskNumber() << std::endl;
    std::cout << "etamin: " << m_etaminDet << "  etamax: " << m_etamaxDet << std::endl;
    std::cout << "phimin: " << m_phiminDet << "  phimax: " << m_phimaxDet << std::endl;
    std::cout << "hashId: ";
    std::vector<IdentifierHash> aux = (*it).hashId();
    for(std::vector<IdentifierHash>::iterator i = aux.begin(); i != aux.end(); ++i)
      std::cout << (*i) << " ";
    std::cout << std::endl;
    std::cin.get();
  }
}



void RegSelectorMap::verifyInputsInternal( double &etaminIn, double &etamaxIn,
					   double &phiminIn, double &phimaxIn ) const {

  while (phiminIn > 2*M_PI) phiminIn -= 2*M_PI;
  while (phiminIn < 0     ) phiminIn += 2*M_PI;
  while (phimaxIn > 2*M_PI) phimaxIn -= 2*M_PI;
  while (phimaxIn < 0     ) phimaxIn += 2*M_PI;

  if( ( (etaminIn < m_etaminDet) && (etamaxIn < m_etaminDet) ) ||
      ( (etaminIn > m_etamaxDet) && (etamaxIn > m_etamaxDet) ) ){
  }
  else{
    if( (etaminIn < m_etaminDet) && (etamaxIn > m_etaminDet) ){
        etaminIn = m_etaminDet;
    }
    if( (etamaxIn > m_etamaxDet) && (etaminIn < m_etamaxDet) ){
        etamaxIn = m_etamaxDet;
    }
  }

}


StatusCode RegSelectorMap::verifyInputs( double &etaminIn, double &etamaxIn,
					 double &phiminIn, double &phimaxIn ) const {
  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );
  return StatusCode::SUCCESS;
}



StatusCode RegSelectorMap::verifyInputsMinusPi( double &etaminIn, double &etamaxIn,
						double &phiminIn, double &phimaxIn) const {

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

void RegSelectorMap::verifyOutput( double etaminIn, double etamaxIn,
				   double phiminIn, double phimaxIn,
				   std::vector<IdentifierHash> outputIdlist) const {

  std::vector<IdentifierHash> outset;
  int i;
  unsigned int j;
  double etamin, etamax, phimin, phimax;
  int vecsize = m_etamin.size();
  for( i= 0; i < vecsize; ++i){
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
  for( j=0; j < outset.size(); ++j)
    std::cout << std::dec << outset[j] << " ";
  std::cout << std::endl;
  std::cout << "obtained output ";
  for( j=0; j < outputIdlist.size(); ++j)
    std::cout << std::dec << outputIdlist[j] << " ";
  std::cout << std::endl;
}


void RegSelectorMap::regionSelectorRobIdUint( double etaminIn, double etamaxIn,
					      double phiminIn, double phimaxIn,
					      std::vector<uint32_t>& outList) const {
  
  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::set<uint32_t> outset;
  int i;
  unsigned int j;
  double etamin, etamax, phimin, phimax;
  std::vector<uint32_t> outvec;

  int vecsize = m_etamin.size();
  for( i= 0; i < vecsize; ++i){
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
  for(it = outset.begin(); it != outset.end(); ++it)
    outvec.push_back(*it);

  if(outvec == outList)
    std::cout << "equal vectors " << std::endl;
  std::cout << "desired output ";
  for( j=0; j < outvec.size(); ++j)
    std::cout << std::dec << outvec[j] << " ";
  std::cout << std::endl;
  std::cout << "obtained output ";
  for( j=0; j < outList.size(); ++j)
    std::cout << std::dec << outList[j] << " ";
  std::cout << std::endl;
}




void RegSelectorMap::regionSelectorRobIdUint( long layNumber,
					      double etaminIn, double etamaxIn,
					      double phiminIn, double phimaxIn,
					      std::vector<uint32_t>& outList) const {
  
  verifyInputsInternal( etaminIn, etamaxIn, phiminIn, phimaxIn );

  std::set<uint32_t> outset;
  std::list<RegSelectorMapElement>::const_iterator it, itEnd;
  std::set<uint32_t>::const_iterator itset;
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
     ++it;
   }
	(*it).selectionRobIdUint( etaminIn, etamaxIn, phiminIn, phimaxIn, outset);
	
  for(itset = outset.begin(); itset != outset.end(); ++itset){
    outList.push_back(*itset);
  }
  
}

void RegSelectorMap::findPosition( TYPEID typeinID, bool posORneg,
				   std::list<RegSelectorMapElement>::const_iterator& it)  const {
  
  std::list<RegSelectorMapElement>::const_iterator itPos, itNeg, itEnd;

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

   /// what does this even do ? just loops over the oiterators and 
   /// then exits with the value of the iterator set ?? 
   /// this is quite dangerous - in principle it can return itEnd 
   /// but will any calling function test for this properly ??

   while(it != itEnd) {
     if( (*it).layerDiskNumber() == typeinID )
       break;
     ++it;
   }

}

void RegSelectorMap::findPosition( long layNumber, bool posORneg,
				   std::list<RegSelectorMapElement>::const_iterator& it) const {
  
  std::list<RegSelectorMapElement>::const_iterator itPos, itNeg, itEnd;

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
     ++it;
   }

}

void RegSelectorMap::findMaxMinValues(std::list<RegSelectorMapElement> &dataList) {

  std::list<RegSelectorMapElement>::iterator it;

  for(it = dataList.begin(); it != dataList.end(); ++it){ // runs through entire list
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
						       IdentifierHash& hashIdIn, uint32_t& robIdIn ) {
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

  for(it = dataList.begin(); it != dataList.end(); ++it){ // runs through entire positive list
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



void RegSelectorMap::getEtaPhi(IdentifierHash hashId, 
			       double *etaMin, double *etaMax,
			       double *phiMin, double *phiMax) const {

	(*etaMin) = m_etamin[hashId];
	(*etaMax) = m_etamax[hashId];
	(*phiMin) = m_phimin[hashId];
	(*phiMax) = m_phimax[hashId];
}
