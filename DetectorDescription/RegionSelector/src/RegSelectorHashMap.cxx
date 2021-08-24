/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RegionSelector/RegSelectorHashMap.h"
#include "PathResolver/PathResolver.h"
#include "RegionSelector/RegionSelectorLUT.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <cstring>





/// implementation of the IRegSelUT interface                                                                                                                                        

/// hash id methods

void RegSelectorHashMap::HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {
  if ( roi.isFullscan() ) regionSelector( -4.8, 4.8, -M_PI, M_PI, idlist );
  regionSelector( roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist);
}

void RegSelectorHashMap::HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const {
  if ( roi.isFullscan() ) regionSelector( layer, -4.8, 4.8, -M_PI, M_PI, idlist );
  regionSelector( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), idlist);
}

/// rob methods

void RegSelectorHashMap::ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {
  if ( roi.isFullscan() ) regionSelectorRobIdUint( -4.8, 4.8, -M_PI, M_PI, roblist );
  regionSelectorRobIdUint( roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist);
}

void RegSelectorHashMap::ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const {
  if ( roi.isFullscan() ) regionSelectorRobIdUint( layer, -4.8, 4.8, -M_PI, M_PI, roblist );
  regionSelectorRobIdUint( layer, roi.etaMinus(), roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), roblist);
}



/// rest of the class implementation

double RegSelectorHashMap::etaminValue() const {
  return m_etaminDet;
}

double RegSelectorHashMap::etamaxValue() const {
  return m_etamaxDet;
}

double RegSelectorHashMap::phiminValue() const {
  return m_phiminDet;
}

double RegSelectorHashMap::phimaxValue() const {
  return m_phimaxDet;
}

const std::vector<IdentifierHash>& RegSelectorHashMap::hashIdOut(void) const {
  return m_hashId;
}

const std::vector<std::vector<uint32_t> >& RegSelectorHashMap::robIdOut(void) const {
  return m_robId;
}

const std::vector<int>& RegSelectorHashMap::sampleOut(void) const {
  return m_sample;
}

const std::vector<int>& RegSelectorHashMap::layerOut(void) const {
  return m_layer;
}

const std::vector<double>& RegSelectorHashMap::etaMinOut(void) const {
  return m_etamin;
}

const std::vector<double>& RegSelectorHashMap::etaMaxOut(void) const {
  return m_etamax;
}

const std::vector<double>& RegSelectorHashMap::phiMinOut(void) const {
  return m_phimin;
}

const std::vector<double>& RegSelectorHashMap::phiMaxOut(void) const {
  return m_phimax;
}

void RegSelectorHashMap::mountDataStruct(void){
  int iNumRanges, iRepeatOverX, iRepeatOverY, xEta, yPhi, k;
  int iPage, iPosition, iXeta, iYphi; // , iNumSamples;
  double dEtaMin=-999, dEtaMax=-999, dPhiMin= -999, dPhiMax = -999;

  m_iLines = std::floor((m_phimaxDet-m_phiminDet)/m_stepMinPhi);
  m_iColumns = std::floor((m_etamaxDet-m_etaminDet)/m_stepMinEta);

  //Number of elements from the vectors (number of different ranges)
  iNumRanges = m_sample.size();

  //Creates the m_NumSamples matrix"es"
  // m_NumSamples*iColumns*iLines ->defines the matrix M page
  // Yphi*iColumns+Xeta -> define the element M[X,Y]

  // initialize matrix
  initMatrix();

  //Populates the matrix
  for(k=0;k<iNumRanges;++k){
    //Get the range values
    dEtaMin = std::min(m_etamin[k],m_etamax[k]);
    dEtaMax = std::max(m_etamin[k],m_etamax[k]);
    dPhiMin = std::min(m_phimin[k],m_phimax[k]);
    dPhiMax = std::max(m_phimin[k],m_phimax[k]);
    //Guess how many times the hashID must be repeated on the X axis
    iRepeatOverX = MyRound((dEtaMax-dEtaMin)/m_stepMinEta);
    iRepeatOverX = std::abs(iRepeatOverX);
    //Guess how many times the hashID must be repeated on the Y axis
    iRepeatOverY = MyRound((dPhiMax-dPhiMin)/m_stepMinPhi);
    iRepeatOverY = std::abs(iRepeatOverY);

    iPage = m_sample[k] * (m_iColumns * m_iLines);

    // initial position
    iYphi = std::floor((dPhiMin - m_phiminDet+2e-6)/m_stepMinPhi);
    for(yPhi=0;yPhi<iRepeatOverY;++yPhi){
     iXeta = std::floor((dEtaMin - m_etaminDet)/m_stepMinEta);
     for(xEta=0;xEta<iRepeatOverX;++xEta){
        iPosition = iPage + (iYphi* m_iColumns +iXeta);
        m_hashIDMap[iPosition] = m_hashId[k];
        m_robIDMap[iPosition][0]=m_robId[k][0];
	for(unsigned int i=1; i< m_robId[k].size();++i )
           m_robIDMap[iPosition].push_back(m_robId[k][i]);
        ++iXeta;
      }
      ++iYphi;
    }
  }
#ifdef REGSELDEBUG 	 
   for(k=0;k<iNumRanges;++k){ 	 
         std::cout << "REGSELECTOR HASH TABLE " << 	 
         m_phimin[k] << " " << m_phimax[k] << " " << 	 
         m_etamin[k] << " " << m_etamax[k] << " " << 	 
         m_sample[k] << " " << m_layer[k]  << " " << 	 
         m_hashId[k] << " 0x"; 	 
	 std::cout << std::hex;
         for(unsigned int i=0;i<m_robId[k].size();++i) 	 
                 std::cout << m_robId[k][i]  << " "; 	 
	 std::cout << std::dec;
         std::cout << std::endl; 	 
   } 	 
   //Populates the matrix 	 
   for(k=0;k<iNumRanges;++k){ 	 
     //Get the range values 	 
     dEtaMin = std::min(m_etamin[k],m_etamax[k]); 	 
     dEtaMax = std::max(m_etamin[k],m_etamax[k]); 	 
     dPhiMin = std::min(m_phimin[k],m_phimax[k]); 	 
     dPhiMax = std::max(m_phimin[k],m_phimax[k]); 	 
     iRepeatOverX = MyRound((dEtaMax-dEtaMin)/m_stepMinEta); 	 
     iRepeatOverX = std::abs(iRepeatOverX); 	 
     iRepeatOverY = MyRound((dPhiMax-dPhiMin)/m_stepMinPhi); 	 
     iRepeatOverY = std::abs(iRepeatOverY); 	 
     iPage = m_sample[k] * (m_iColumns * m_iLines); 	 
     // initial position 	 
     iYphi = std::floor((dPhiMin - m_phiminDet+2e-6)/m_stepMinPhi); 	 
     for(yPhi=0;yPhi<iRepeatOverY;++yPhi){ 	 
      iXeta = std::floor((dEtaMin - m_etaminDet)/m_stepMinEta); 	 
      for(xEta=0;xEta<iRepeatOverX;++xEta){ 	 
         iPosition = iPage + (iYphi* m_iColumns +iXeta); 	 
         std::cout << "REGSELECTOR MAPS " << 	 
         iXeta << " " << iYphi << " " << iPosition << " " << 	 
         m_hashIDMap[iPosition] << " "; 	 
	 std::cout << std::hex;
         for(unsigned int i=0; i<m_robIDMap[iPosition].size();++i) 	 
                 std::cout << "0x" << m_robIDMap[iPosition][i]  << " "; 	 
	 std::cout << std::dec;
         std::cout << std::endl; 	 
         ++iXeta; 	 
       } 	 
       ++iYphi; 	 
     } 	 
    } 	 
#endif

}

void RegSelectorHashMap::regionSelectorRobIdUint(double etaminIn, double etamaxIn,
					       double phiminIn, double phimaxIn,
					       std::vector<uint32_t>& outList) const {
  regionSelectorINROB(-1, etaminIn, etamaxIn, phiminIn, phimaxIn,&outList);
}

void RegSelectorHashMap::regionSelectorRobIdUint(int sampling, double etaminIn, double etamaxIn,
					       double phiminIn, double phimaxIn,
					       std::vector<uint32_t>& outList) const {
  regionSelectorINROB(sampling, etaminIn, etamaxIn, phiminIn, phimaxIn, &outList);
}

void RegSelectorHashMap::populateMatrix(int iPage,IdentifierHash value) {
  int xEta,yPhi, iPosition;
  int max_iPosition(0);
  std::vector<uint32_t> tocreate(1);
  IdentifierHash tocreateIH(value);

  for(xEta= 0; xEta < m_iColumns; ++xEta){
    for(yPhi= 0; yPhi < m_iLines; ++yPhi){
      iPosition = iPage + (yPhi* m_iColumns + xEta);
      if(iPosition>max_iPosition) max_iPosition=iPosition;
    }
  }
  m_hashIDMap.resize(max_iPosition+1,tocreateIH);
  m_robIDMap.resize(max_iPosition+1,tocreate);
  for(xEta= 0; xEta < m_iColumns; ++xEta){
    for(yPhi= 0; yPhi < m_iLines; ++yPhi){
      iPosition = iPage + (yPhi* m_iColumns + xEta);
      m_robIDMap[iPosition].clear();
      m_robIDMap[iPosition].push_back(value);
    }
  }

}

void RegSelectorHashMap::populateMatrixRobId(int iPage,uint32_t value)  {
  int xEta,yPhi, iPosition;

  for(xEta= 0; xEta < m_iColumns; ++xEta){
    for(yPhi= 0; yPhi < m_iLines; ++yPhi){
      iPosition = iPage + (yPhi* m_iColumns + xEta);
      m_robIDMap[iPosition].push_back(value);
    }
  }

}

void RegSelectorHashMap::initMatrix(void){
  int k, iPage, iNumRanges;

  iNumRanges =  m_NumSamples;

  for(k=0;k<=iNumRanges;++k){
    iPage = k * (m_iColumns * m_iLines);
      populateMatrix(iPage, INITHASH);
  }

}

void RegSelectorHashMap::writeLine(const int& layer, const IdentifierHash& hashId, 
			const std::vector<uint32_t>& robId, const double& emin,
			const double& emax, const double& pmin,
			const double& pmax, const int& samp){

  m_hashId.push_back(hashId);
  m_robId.push_back(robId);
  m_etamin.push_back(emin);
  m_etamax.push_back(emax);
  m_phimin.push_back(pmin);
  m_phimax.push_back(pmax);
  m_layer.push_back(layer);
  m_sample.push_back(samp);

}

int RegSelectorHashMap::MyRound(double pdValue){
  double dFloor, dAux;
  int iReturn;

  dFloor = std::floor(pdValue);
  dAux = std::fabs(pdValue-dFloor);
  if(dAux >= 0.5f)
    iReturn = (int) (dFloor + 1);
  else
    iReturn = (int) dFloor;

  return iReturn;
}

void RegSelectorHashMap::regionSelectorIN(const int& sampling,
					  const double& etaminIn, const double& etamaxIn, 
					  const double& phiminIn, const double& phimaxIn,
					  std::vector<IdentifierHash>* outListIH) const {
  int iYBeg, iYEnd, iXBeg, iXEnd, k, iPage;
  int iXTemp;
  std::vector<IdentifierHash>& auxsetIH=(*outListIH);

  /// why are we making this 
  iYBeg = std::floor((phiminIn - m_phiminDet+7e-3)/m_stepMinPhi);
  iYEnd = std::ceil((phimaxIn - m_phiminDet-7e-3)/m_stepMinPhi);


  /// need to check whether spans the pi boundary - using iYBeg > iYEnd isn't 
  /// good enough since it it is very nearly 2pi, then iYBeg and iYEnd will 
  /// be equal 
  bool pi_boundary = false;
  if ( phiminIn>phimaxIn ) pi_boundary = true;

  /// wrap phi
   if(iYBeg < 0)        iYBeg += m_iLines;
   if(iYBeg > m_iLines) iYBeg -= m_iLines; 	 
   if(iYEnd < 0)        iYEnd += m_iLines ; 	 
   if(iYEnd > m_iLines) iYEnd -= m_iLines;


   /// why reduce the RoI size?
   iXBeg = std::floor((etaminIn - m_etaminDet+2e-5)/m_stepMinEta);
   iXEnd = std::ceil((etamaxIn - m_etaminDet-2e-5)/m_stepMinEta);

    if(iXBeg < 0) iXBeg = 0;
    if(iXBeg > m_iColumns) iXBeg = m_iColumns;
    if(iXEnd < 0) iXEnd = 0;
    if(iXEnd > m_iColumns) iXEnd = m_iColumns;


    /// This is WRONG!! this forces *all* rois to increase as the 
    /// radius increases - oh dear
    if(iXBeg > iXEnd){
      iXTemp = iXBeg;
      iXBeg = iXEnd;
      iXEnd = iXTemp;
    }
    if(sampling == -1){ // Consider all samplings (0...3)
      if( pi_boundary ){
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifier(auxsetIH, iXBeg, iXEnd, iYBeg, m_iLines, iPage);
        }
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifier(auxsetIH, iXBeg, iXEnd, 0, iYEnd, iPage);
        }
      }
      else{
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifier(auxsetIH,iXBeg, iXEnd, iYBeg, iYEnd, iPage);
        }
      }
    }
    else{ // Consider only the input sampling value
      k= sampling;
      iPage = k * (m_iColumns * m_iLines);
      if( pi_boundary ){
        findIdentifier(auxsetIH,iXBeg, iXEnd, iYBeg, m_iLines, iPage);
        findIdentifier(auxsetIH,iXBeg, iXEnd, 0, iYEnd, iPage);
      }
      else{
        findIdentifier(auxsetIH,iXBeg, iXEnd, iYBeg, iYEnd, iPage);
      }
    }
}




void RegSelectorHashMap::regionSelectorINROB(const int& sampling,
					     const double& etaminIn, const double& etamaxIn, const double& phiminIn,
					     const double& phimaxIn,
					     std::vector<uint32_t>* outList) const {
  int iYBeg{}, iYEnd{}, iXBeg{}, iXEnd{}, k{}, iPage{};
  int iXTemp{};
  std::vector<uint32_t>& auxset=(*outList);

  /// why do we reduce the size of the RoI here? if we must mess with it, 
  /// shouldn't we make it larger?
  if (m_stepMinPhi !=0){
    //
    iYBeg = std::floor((phiminIn - m_phiminDet+7e-3)/m_stepMinPhi);
    iYEnd = std::ceil((phimaxIn - m_phiminDet-7e-3)/m_stepMinPhi);
  }

  /// need to check whether spans the pi boundary - using (iYBeg > iYEnd) 
  /// isn't good enough since it it is very nearly 2pi, then iYBeg and iYEnd 
  /// will be equal. 
  bool pi_boundary = false;
  if ( phiminIn>phimaxIn ) pi_boundary = true;

  /// wrap phi
   if(iYBeg < 0)        iYBeg += m_iLines;
   if(iYBeg > m_iLines) iYBeg -= m_iLines; 	 
   if(iYEnd < 0)        iYEnd += m_iLines ; 	 
   if(iYEnd > m_iLines) iYEnd -= m_iLines;
   /// why reduce the size of the RoI? 
   if (m_stepMinEta!=0) {
     iXBeg = std::floor((etaminIn - m_etaminDet+2e-5)/m_stepMinEta);
     iXEnd = std::ceil((etamaxIn - m_etaminDet-2e-5)/m_stepMinEta);
   }
    if(iXBeg < 0) iXBeg = 0;
    if(iXBeg > m_iColumns) iXBeg = m_iColumns;
    if(iXEnd < 0) iXEnd = 0;
    if(iXEnd > m_iColumns) iXEnd = m_iColumns;
  

    /// This is WRONG!! this forces *all* rois to increase as the 
    /// radius increases - oh dear
    if(iXBeg > iXEnd){
      iXTemp = iXBeg;
      iXBeg = iXEnd;
      iXEnd = iXTemp;
    }

    if(sampling == -1){ // Consider all samplings (0...3)

      if( pi_boundary ){
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifierROB(auxset, iXBeg, iXEnd, iYBeg, m_iLines,
		iPage);
        }
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifierROB(auxset, iXBeg, iXEnd, 0, iYEnd, iPage);
        }
      }
      else{
        for(k=0; k<=m_NumSamples; ++k){
          iPage = k * (m_iColumns * m_iLines);
          findIdentifierROB(auxset,iXBeg, iXEnd, iYBeg, iYEnd, iPage);
        }
      }
    }
    else{ // Consider only the input sampling value

      k= sampling;
      iPage = k * (m_iColumns * m_iLines);
      if( pi_boundary ){
        findIdentifierROB(auxset,iXBeg, iXEnd, iYBeg, m_iLines, iPage);
        findIdentifierROB(auxset,iXBeg, iXEnd, 0, iYEnd, iPage);
      }
      else{
        findIdentifierROB(auxset,iXBeg, iXEnd, iYBeg, iYEnd, iPage);
      }
    }
}

void RegSelectorHashMap::phiCondition(const std::vector<IdentifierHash>& identifier, int i,
				      double etaminIn, double etamaxIn,
				      double phiminIn, double phimaxIn, 
				      double etamin, double etamax,
				      std::set<IdentifierHash>* outset) const {
  double phimin, phimax;
  if(m_phimin[i] > m_phimax[i]){
    phimin = m_phimin[i];
    phimax = m_phimaxDet;

    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i]);
      }
    }
    phimin = m_phiminDet;
    phimax = m_phimax[i];
    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i]);
      }
    }
  }
  else{
    phimin = m_phimin[i]; phimax = m_phimax[i];
    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i]);
      }
    }
  }
  
}

void RegSelectorHashMap::phiConditionRobId(const std::vector<std::vector<uint32_t> >& identifier, int i,
					   double etaminIn, double etamaxIn,
					   double phiminIn, double phimaxIn, 
					   double etamin, double etamax,
					   std::set<uint32_t>* outset) const {
  double phimin, phimax;
  if(m_phimin[i] > m_phimax[i]){
    phimin = m_phimin[i];
    phimax = m_phimaxDet;

    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i][0]);
      }
    }
    phimin = m_phiminDet;
    phimax = m_phimax[i];
    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i][0]);
      }
    }
  }
  else{
    phimin = m_phimin[i]; phimax = m_phimax[i];
    if( (etaminIn <= etamax) && (etamaxIn >= etamin) ){
      if( (phiminIn <= phimax) && (phimaxIn >= phimin) ){
        (*outset).insert(identifier[i][0]);
      }
    }
  }
  
}

void RegSelectorHashMap::regionSelector(double etaminIn, double etamaxIn,
					double phiminIn, double phimaxIn,std::vector<IdentifierHash>& outList) const {
  verifyInputs(etaminIn, etamaxIn, phiminIn, phimaxIn);
  regionSelectorIN((int)-1, (double)etaminIn, (double)etamaxIn,
		   (double)phiminIn, (double)phimaxIn,&outList);
  
}

void RegSelectorHashMap::regionSelector(int sampling, double etaminIn, double etamaxIn,
					double phiminIn, double phimaxIn, std::vector<IdentifierHash>& outList) const {
  verifyInputs(etaminIn, etamaxIn, phiminIn, phimaxIn);
  regionSelectorIN((int)sampling, (double)etaminIn, (double)etamaxIn,
		   (double)phiminIn, (double)phimaxIn,&outList);
  
}

void RegSelectorHashMap::initvar(void){
  m_NumSamples = -1;
  m_stepMinPhi = 99;
  m_stepMinEta = 99;
  m_etaminDet = m_phiminDet = 99;
  m_etamaxDet = m_phimaxDet = -99;
}

StatusCode RegSelectorHashMap::read(const char *filename){
  StatusCode sc = StatusCode::SUCCESS;
  char  buffer_[256]; // only needs to be 128 long, but safety first!
  char* buffer = buffer_;
  int samp, layer;
  unsigned int hashId;
  double emin, emax, pmin, pmax;
  double stepPhi, stepEta;
  char robIdStr[12], robIdStr2[12];
  char *pch; 
  std::vector<uint32_t> robId;
  robId.push_back(0);

// use path resolver to find full path to file
  std::string unresolvedFileName(filename);
  std::string fullFileName = PathResolver::find_file (unresolvedFileName, "DATAPATH");
  if (fullFileName == "") {
    std::cerr << "LArTTRegionSelectorIdentifierHash: FATAL: Could not find input file in DATAPATH" <<  unresolvedFileName<< std::endl;
    return StatusCode::FAILURE;
  }

  std::ifstream fin(fullFileName.c_str());
  fin.getline(buffer,128,'\n');
   if(fin.bad()){  //Test if the file failed:
     sc = StatusCode::FAILURE;
     return sc;
   }
  if (strncmp(buffer," TT-ID",6)==0){
    fin.getline(buffer, 128, '\n');
    do{
      robId.clear();
      pch = strchr(buffer,' ');
      int test = sscanf(pch, " %u %d %d %lf %lf %lf %lf %s %s",  &hashId, &layer, &samp, &emin, &emax, &pmin, &pmax, robIdStr, robIdStr2);
      robId.push_back(strtol(robIdStr,nullptr,16));
      if ( test == 9 ) // this means that there are 2 ROBs in 1 TT
        robId.push_back(strtol(robIdStr2,nullptr,16));
      pch=strchr(buffer,' ');
      stepPhi = std::fabs(pmax-pmin);// initial value for phi and eta step
      stepEta = std::fabs(emin-emax);
      m_stepMinPhi = std::min(m_stepMinPhi,stepPhi);
      m_stepMinEta = std::min(m_stepMinEta,stepEta);
      m_etaminDet = std::min(std::min(m_etaminDet,emin),emax);
      m_phiminDet = std::min(std::min(m_phiminDet,pmin),pmax);
      m_etamaxDet = std::max(std::max(m_etamaxDet,emax),emin);
      m_phimaxDet = std::max(std::max(m_phimaxDet,pmax),pmin);
      m_NumSamples = std::max(m_NumSamples,samp);
      writeLine(layer, (IdentifierHash) hashId, robId, emin, emax, pmin, pmax, samp);
      // ++nlines;
    }while((fin.getline(buffer, 128, '\n')));
  }
  // m_readFromFile = !( nlines == 0 );
  fin.close();

  return sc;
}

void RegSelectorHashMap::addLut(const RegionSelectorLUT *detLut){
  // unsigned int j;

  initvar();
  std::vector<uint32_t> robId_cpy;

  // If not readFromFile detLut->maxHash() != 0
  m_readFromFile = (detLut->maxHash() == 0);
  // Assuming that we can have up to 2 ROBs for a given TT
  double etamin(-999.0), etamax(-999.0), phimin(-999.0), phimax(-999.0); 
  int samp(999), layer(999);
  int hash(999);
  int tablesize = detLut->maxHash();
  m_hashId.reserve(tablesize);
  m_robId.reserve(tablesize);
  m_etamin.reserve(tablesize);
  m_etamax.reserve(tablesize);
  m_phimin.reserve(tablesize);
  m_phimax.reserve(tablesize);
  m_layer.reserve(tablesize);
  m_sample.reserve(tablesize);
  
  for( int j=0; j<tablesize; ++j){
    if ( detLut->hashId(j)==hash && detLut->sampling(j)==samp &&
	  detLut->etaMin(j)==etamin && detLut->etaMax(j) == etamax &&
          detLut->phiMin(j)==phimin && detLut->phiMax(j) == phimax &&
	  detLut->layerDiskNumber(j) == layer ){
    	robId_cpy.push_back(detLut->robId(j));
	writeLine(detLut->layerDiskNumber(j),
              detLut->hashId(j),
	      robId_cpy,
              detLut->etaMin(j),
              detLut->etaMax(j),
              detLut->phiMin(j),
              detLut->phiMax(j),
              detLut->sampling(j));
	      ++j;
     } else { // Write previous line
	if ( j!= 0 )
	writeLine(layer,hash,robId_cpy,etamin,etamax,phimin,phimax,samp);
     }
     robId_cpy.clear();
     robId_cpy.push_back(detLut->robId(j));
     hash=detLut->hashId(j); samp=detLut->sampling(j);
     etamin=detLut->etaMin(j); etamax=detLut->etaMax(j);
     phimin=detLut->phiMin(j); phimax=detLut->phiMax(j);
     layer=detLut->layerDiskNumber(j);
     // After prossessing need to update global vars
     double stepPhi = std::fabs(phimax-phimin);// initial value for phi and eta step
     double stepEta = std::fabs(etamin-etamax);
     m_stepMinPhi = std::min(m_stepMinPhi,stepPhi);
     m_stepMinEta = std::min(m_stepMinEta,stepEta);
     m_etaminDet = std::min(std::min(m_etaminDet,etamin),etamax);
     m_phiminDet = std::min(std::min(m_phiminDet,phimin),phimax);
     m_etamaxDet = std::max(std::max(m_etamaxDet,etamax),etamin);
     m_phimaxDet = std::max(std::max(m_phimaxDet,phimax),phimin);
     m_NumSamples = std::max(m_NumSamples,samp);
  }
  // Shall not forget to save last one
  // Assuming last line is NOT a 2 ROB/TT case
  writeLine(layer,hash,robId_cpy,etamin,etamax,phimin,phimax,samp);

}

void RegSelectorHashMap::summaryDataFile(std::list<RegSelectorMapElement> &dataList){
  for(std::list<RegSelectorMapElement>::iterator it = dataList.begin();
      it != dataList.end(); ++it){ // runs through entire list
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

void RegSelectorHashMap::verifyInputs(double &etaminIn, double &etamaxIn,
				      double &phiminIn, double &phimaxIn) const {
  
  if ( m_readFromFile ) { // Files still 0 <-> 2pi
    while (phiminIn > 2*M_PI ) phiminIn -= 2*M_PI;
    while (phiminIn < 0      ) phiminIn += 2*M_PI;
    while (phimaxIn > 2*M_PI ) phimaxIn -= 2*M_PI;
    while (phimaxIn < 0      ) phimaxIn += 2*M_PI;
  } else { // SG maps -pi <-> pi
    while (phiminIn > M_PI  ) phiminIn -= 2*M_PI;
    while (phiminIn < -M_PI ) phiminIn += 2*M_PI;
    while (phimaxIn > M_PI  ) phimaxIn -= 2*M_PI;
    while (phimaxIn < -M_PI ) phimaxIn += 2*M_PI;
  }
  
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
}

void RegSelectorHashMap::verifyROBIDOutput(double etaminIn, double etamaxIn,
				 double phiminIn, double phimaxIn,
				 std::vector<uint32_t>& outputIdlist) const {
  int i;
  std::set<uint32_t> myset;
  std::vector<uint32_t> outset;
  std::vector<std::vector<uint32_t> > identifier;
  double etamin, etamax, etatmp, phimintmp, phimaxtmp;
  int vecsize = m_sample.size();

  identifier = m_robId;

  if(etaminIn > etamaxIn){
    etatmp = etamaxIn;
    etamaxIn = etaminIn;
    etaminIn = etatmp;
  }

  phimintmp = phiminIn;
  phimaxtmp = phimaxIn;

  for(i= 0; i < vecsize; ++i){
    if(m_etamin[i] > m_etamax[i]){
      etamin = m_etamax[i]; etamax = m_etamin[i];
    }
    else{
      etamin = m_etamin[i]; etamax = m_etamax[i];
    }
    if(phiminIn > phimaxIn ){
      phimaxIn = m_phimaxDet;		
      phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
      phiminIn = m_phiminDet;
      phimaxIn = phimaxtmp;
      phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
    }
    else{
      phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
    }
    phiminIn = phimintmp; phimaxIn = phimaxtmp;
  }

  std::set<uint32_t>::iterator it;
  for(it = myset.begin(); it!= myset.end(); ++it){
    outset.push_back(*it);
  }

  if( (outset == outputIdlist) || (outputIdlist.size() >= outset.size()) ){
    std::cout << "equal vectors " << std::endl;
  }
  else {
    std::cout << "nonequal vectors " << std::endl;
    std::cout << "eta(" << etaminIn << "," << etamaxIn << ")   phi(" << phiminIn << "," << phimaxIn << ")" << std::endl;
    std::cout << "desired output ";
    for(unsigned int i=0; i < outset.size(); ++i)
      std::cout << std::dec << outset[i] << " ";
    std::cout << std::endl;
    std::cout << "obtained output ";
    for(unsigned int i=0; i < outputIdlist.size(); ++i)
      std::cout << std::dec << outputIdlist[i] << " ";
    std::cout << std::endl;
    std::cin.get();
  }
}

void RegSelectorHashMap::verifyROBIDOutput(int sampling,
						double etaminIn, double etamaxIn,
						double phiminIn, double phimaxIn,
						std::vector<uint32_t>& outputIdlist) const {

  int i;
  std::set<uint32_t> myset;
  std::vector<std::vector<uint32_t> > identifier;
  std::vector<uint32_t> outset;
  double etamin, etamax, etatmp, phimintmp, phimaxtmp;
  int vecsize = m_sample.size();

  identifier = m_robId;

  if(etaminIn > etamaxIn){
    etatmp = etamaxIn;
    etamaxIn = etaminIn;
    etaminIn = etatmp;
  }

  phimintmp = phiminIn;
  phimaxtmp = phimaxIn;

  for(i= 0; i < vecsize; ++i){
    if( sampling == m_sample[i]){
      if(m_etamin[i] > m_etamax[i]){
        etamin = m_etamax[i]; etamax = m_etamin[i];
      }
      else{
        etamin = m_etamin[i]; etamax = m_etamax[i];
      }
      if(phiminIn > phimaxIn ){
        phimaxIn = m_phimaxDet;
        phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
        phiminIn = m_phiminDet;
        phimaxIn = phimaxtmp;
        phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
      }
      else{
        phiConditionRobId(identifier, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
      }
    }
    phiminIn = phimintmp; phimaxIn = phimaxtmp;
  }

  std::set<uint32_t>::iterator it;
  for(it = myset.begin(); it!= myset.end(); ++it){
    outset.push_back(*it);
  }

  if(outset == outputIdlist){
    std::cout << "equal vectors " << std::endl;
  }
  else {
    std::cout << "sampling " << sampling << "eta(" << etaminIn << "," << etamaxIn << ")   phi(" << phiminIn << "," << phimaxIn << ")" << std::endl;
    std::cout << "desired output ";
    for(unsigned int i=0; i < outset.size(); ++i)
      std::cout << std::dec << outset[i] << " ";
    std::cout << std::endl;
    std::cout << "obtained output ";
    for(unsigned int i=0; i < outputIdlist.size(); ++i)
      std::cout << std::dec << outputIdlist[i] << " ";
    std::cout << std::endl;
    std::cin.get();
  }
}

void RegSelectorHashMap::verifyOutput(double etaminIn, double etamaxIn,
				      double phiminIn, double phimaxIn,
				      std::vector<IdentifierHash>* outputIdlist) const {
  int i;
  std::set<IdentifierHash> myset;
  std::vector<IdentifierHash> outset;
  double etamin, etamax, etatmp, phimintmp, phimaxtmp;
  int vecsize = m_sample.size();

  if(etaminIn > etamaxIn){
    etatmp = etamaxIn;
    etamaxIn = etaminIn;
    etaminIn = etatmp;
  }

  phimintmp = phiminIn;
  phimaxtmp = phimaxIn;

  for(i= 0; i < vecsize; ++i){
    if(m_etamin[i] > m_etamax[i]){
      etamin = m_etamax[i]; etamax = m_etamin[i];
    }
    else{
      etamin = m_etamin[i]; etamax = m_etamax[i];
    }
    if(phiminIn > phimaxIn ){
      phimaxIn = m_phimaxDet;															
      phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
      phiminIn = m_phiminDet;
      phimaxIn = phimaxtmp;
      phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
    }
    else{
      phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
    }
    phiminIn = phimintmp; phimaxIn = phimaxtmp;
  }

  std::set<IdentifierHash>::iterator it;
  for(it = myset.begin(); it!= myset.end(); ++it){
    outset.push_back(*it);
  }

  if(outset == (*outputIdlist) ){
    std::cout << "equal vectors " << std::endl;
  }
  else {
    std::cout << "eta(" << etaminIn << "," << etamaxIn << ")   phi(" << phiminIn << "," << phimaxIn << ")" << std::endl;
    std::cout << "desired output ";
    for(unsigned int i=0; i < outset.size(); ++i)
      std::cout << std::dec << outset[i] << " ";
    std::cout << std::endl;
    std::cout << "obtained output ";
    for(unsigned int i=0; i < (*outputIdlist).size(); ++i)
      std::cout << std::dec << (*outputIdlist)[i] << " ";
    std::cout << std::endl;
  }
}

void RegSelectorHashMap::verifyOutput(int sampling,double etaminIn, double etamaxIn,
				      double phiminIn, double phimaxIn,
				      std::vector<IdentifierHash>* outputIdlist) const {
  int i;
  std::set<IdentifierHash> myset;
  std::vector<IdentifierHash> outset;
  double etamin, etamax, etatmp, phimintmp, phimaxtmp;
  int vecsize = m_sample.size();

  if(etaminIn > etamaxIn){
    etatmp = etamaxIn;
    etamaxIn = etaminIn;
    etaminIn = etatmp;
  }

  phimintmp = phiminIn;
  phimaxtmp = phimaxIn;

  for(i= 0; i < vecsize; ++i){
    if( sampling == m_sample[i]){
      if(m_etamin[i] > m_etamax[i]){
        etamin = m_etamax[i]; etamax = m_etamin[i];
      }
      else{
        etamin = m_etamin[i]; etamax = m_etamax[i];
      }
      if(phiminIn > phimaxIn ){
        phimaxIn = m_phimaxDet;
        phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
        phiminIn = m_phiminDet;
        phimaxIn = phimaxtmp;
        phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax, &myset);
      }
      else{
        phiCondition(m_hashId, i, etaminIn, etamaxIn, phiminIn, phimaxIn, etamin, etamax,&myset);
      }
    }
    phiminIn = phimintmp; phimaxIn = phimaxtmp;
  }

  std::set<IdentifierHash>::iterator it;
  for(it = myset.begin(); it!= myset.end(); ++it){
    outset.push_back(*it);
  }

  if(outset == (*outputIdlist) ){
    std::cout << "equal vectors " << std::endl;
  }
  else {
    std::cout << "sampling " << sampling << "eta(" << etaminIn << "," << etamaxIn << ")   phi(" << phiminIn << "," << phimaxIn << ")" << std::endl;
    std::cout << "desired output ";
    for(unsigned int i=0; i < outset.size(); ++i)
      std::cout << std::dec << outset[i] << " ";
    std::cout << std::endl;
    std::cout << "obtained output ";
    for(unsigned int i=0; i < (*outputIdlist).size(); ++i)
      std::cout << std::dec << (*outputIdlist)[i] << " ";
    std::cout << std::endl;
    std::cin.get();
  }
}

void RegSelectorHashMap::findIdentifier(std::vector<IdentifierHash> &auxsetIH,
					const int iXBeg, const int iXEnd, 
					const int iYBeg, const int iYEnd,
					const int iPage) const  {
 int xEta, yPhi, iPosition;

  for(xEta= iXBeg; xEta < iXEnd; ++xEta){
    for(yPhi=iYBeg; yPhi < iYEnd; ++yPhi){
      iPosition = iPage + (yPhi* m_iColumns + xEta);
      if(m_hashIDMap[iPosition] != INITHASH){
          auxsetIH.push_back(m_hashIDMap[iPosition]);
      }
    }
  }
  
  if(auxsetIH.size()!=0){
    sort(auxsetIH.begin(),auxsetIH.end());
    auxsetIH.erase(unique(auxsetIH.begin(),auxsetIH.end()),auxsetIH.end());
  }
  
}

void RegSelectorHashMap::findIdentifierROB( std::vector<uint32_t> &auxset, 
					    const int  iXBeg, const int  iXEnd, 
					    const int  iYBeg, const int  iYEnd, 
					    const int iPage ) const {
  int xEta, yPhi, iPosition;
  unsigned int i,size_rod;
  
  for(xEta= iXBeg; xEta < iXEnd; ++xEta){
    for(yPhi=iYBeg; yPhi < iYEnd; ++yPhi){
      iPosition = iPage + (yPhi* m_iColumns + xEta);
          size_rod=m_robIDMap[iPosition].size();
          for(i=0; i < size_rod; ++i)
             if ( m_robIDMap[iPosition][i] != INITHASH )
             auxset.push_back(m_robIDMap[iPosition][i]);
      }
    }

  if(auxset.size()!=0){
    sort(auxset.begin(),auxset.end());
    auxset.erase(unique(auxset.begin(),auxset.end()),auxset.end());
  }
  
}



void RegSelectorHashMap::getEtaPhi(IdentifierHash hashId, 
				   double *etaMin, double *etaMax,
				   double *phiMin, double *phiMax) const {
  
  (*etaMin) = m_etamin[hashId];
  (*etaMax) = m_etamax[hashId];
  (*phiMin) = m_phimin[hashId];
  (*phiMax) = m_phimax[hashId];
}

void RegSelectorHashMap::DisableMissingROBs(const std::vector<uint32_t>& vec){
	for(size_t i=0;i<vec.size();++i){
		for(size_t j=0;j<m_robIDMap.size();++j){
			for(size_t k=0;k<m_robIDMap[j].size();++k){
			   if ( vec[i] == m_robIDMap[j][k] )
				m_robIDMap[j][k] = INITHASH;
			}
		}
	}
}
