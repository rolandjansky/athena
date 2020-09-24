/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloConditions/src/CaloLocalHadCoeff.cxx
 * @brief Hold binned correction data for local hadronic calibration procedure
 */
//#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>



/* ***************************************************************************
LocalHadDimension ->
*************************************************************************** */


// return bin number for cluster variable
int CaloLocalHadCoeff::LocalHadDimension::getBin(float &x) const
{
  if( x<m_xmin ) {
    return -1;
  }
  if( x>=m_xmax ) {
    return -100;
  }
  int i_bin = -1;
  if( m_xbins.empty() ) { // flat binning
    i_bin = (int)((x - m_xmin)/m_dx);
  }else{ // user binning
    for(i_bin=0; i_bin<(int)m_xbins.size(); i_bin++){
      if( x < m_xbins[i_bin] ) break;
    }
  }
  return i_bin;
}

// return two bins
int CaloLocalHadCoeff::LocalHadDimension::getBinAdjusted(float &x, float &xadj) const
{
  if (x < m_xmin) {
    xadj=0.;
    return 0;
  }

//  int i_bin = (int)((x - m_xmin)/m_dx);
  int i_bin = getBin(x);

  if(i_bin<-10) {
    i_bin=m_nbins-1; 
    xadj=1.0;
  } else if(i_bin<0) {
    i_bin=0; 
    xadj=0.0;
  }else{
    xadj = fmod( (x-m_xmin)/m_dx, 1);
    if(xadj <= 0.5) {
      if(i_bin > 0 ){
        --i_bin;
        xadj += 0.5;
      } else {
        xadj=0.;
      }
    }else{
//       if(i_bin > 0){
      xadj -= 0.5;
//       }
    }
  }
  return i_bin;
}

// return true, if this general bin is neighbour of the bin given by indexes vector
bool CaloLocalHadCoeff::isNeighbour(const int iBinx, std::vector<int> &v_indx) const {
  // Sanity check
  if(iBinx < 0 || iBinx >= (int) m_CoeffSet.size()) return false;
  if((int)v_indx.size() != getAreaFromBin(iBinx)->getNdim()) {
     std::cout<<"CaloLocalHadCoeff::LocalHadDimension::getBinAdjusted() -> Error! Wrong indexes vector size in isNeighbour: "<< v_indx.size()<<std::endl;
     return false;
  }
  std::vector<int> xidx;
  bin2indexes(iBinx, xidx);
  for(unsigned int i=0; i<xidx.size(); ++i) {
     if(abs(v_indx[i]-xidx[i]) == 1) return true;
  }
  return false;
}


// return true, if bin was filled with data, depending on type of area
bool CaloLocalHadCoeff::isFilled(const int bin) const
{
  if( bin<0 || bin >= (int)m_CoeffSet.size() ) {
    return false;
  }
  switch(getAreaFromBin(bin)->getType()) {
    case CaloLocalHadDefs::AREA_DMLOOKUP: case CaloLocalHadDefs::AREA_STD: {
      int ient =  CaloLocalHadDefs::BIN_ENTRIES;
      return m_CoeffSet[bin][ient] != 0;
    }
    case CaloLocalHadDefs::AREA_DMFIT:{
      return !(m_CoeffSet[bin][CaloLocalHadDefs::BIN_P0] == 0 && 

         m_CoeffSet[bin][CaloLocalHadDefs::BIN_P1] == 0 &&

         m_CoeffSet[bin][CaloLocalHadDefs::BIN_P2] == 0);
    }
    case CaloLocalHadDefs::AREA_DMSMPW:{
      bool bf=false;
      for(unsigned int i=0; i<m_CoeffSet[bin].size(); ++i) if(m_CoeffSet[bin][i] != 0) bf=true;
      return bf;
    }
    default:{
      std::cout<<"CaloLocalHadCoeff::isFilled() -> Error! Unknown area typ: "<<getAreaFromBin(bin)->getType()<<" in isFilled !!"<<std::endl;
      return false;
    }
  }
  // should not come here, but syntax need it:
  return false;
}



/* ***************************************************************************
LocalHadArea ->
*************************************************************************** */

// add new dimension to the area
void CaloLocalHadCoeff::LocalHadArea::addDimension(CaloLocalHadCoeff::LocalHadDimension &dim)
{
  m_dims.push_back(dim);
  // recalculation of length of the area (product of bins numbers of each dimension)
  m_length =  1;
  int ndim = (int)m_dims.size();
  for(int i=0; i<ndim; i++){
    m_length = m_length*m_dims[i].getNbins();
  }
  // calculation of dimension locator coefficient for fast calculation of global bin number
  m_dims_loc.resize(ndim, 0);
  for(int i_dim=0; i_dim<ndim; i_dim++){
    int xloc = 1;
    for(int j=i_dim+1; j<ndim; j++){
      xloc = xloc*m_dims[j].getNbins();
    }
    m_dims_loc[i_dim] = xloc;
  }
}



/* ***************************************************************************
CaloLocalHadCoeff
*************************************************************************** */

// default constructor
CaloLocalHadCoeff::CaloLocalHadCoeff()
{
   static_assert(std::is_nothrow_move_constructible<CaloLocalHadCoeff>::value);
   static_assert(std::is_nothrow_move_constructible<LocalHadArea>::value);
   static_assert(std::is_nothrow_move_constructible<LocalHadDimension>::value);
}



/* ***************************************************************************
CaloLocalHadCoeff: area's methods
*************************************************************************** */

// to add new area definition
void CaloLocalHadCoeff::addArea(LocalHadArea & theArea)
{
  // calculating offset for coefficients of this area using previous areas defined
  int offset = 0;
  for(unsigned int i_area=0; i_area<m_AreaSet.size(); i_area++){
    offset += m_AreaSet[i_area].getLength();
  }
  theArea.setOffset(offset);
  // adding zero coefficients
  CaloLocalHadCoeff::LocalHadCoeff pars;
  pars.resize(theArea.getNpars(), 0.0);
  for(int i_size=0; i_size<theArea.getLength(); i_size++){
    m_CoeffSet.push_back(pars);
  }
  // adding area
  m_AreaSet.push_back(theArea);
}

// to set new area
void CaloLocalHadCoeff::setArea(const int n_area, const LocalHadArea & theArea)
{
  m_AreaSet[n_area] = theArea;
}

// return area
const CaloLocalHadCoeff::LocalHadArea * CaloLocalHadCoeff::getArea(int n_area) const
{
  if(n_area >= 0 && n_area<(int)m_AreaSet.size() ) {
    return &(m_AreaSet[n_area]);
  } else {
    return nullptr;
  }
}

// return area for given global bin number
const CaloLocalHadCoeff::LocalHadArea *CaloLocalHadCoeff::getAreaFromBin(int iBin) const
{
  int i_area = 0;
  int narea = m_AreaSet.size();
  for(i_area=0; i_area<narea; i_area++) {
    if( iBin < m_AreaSet[i_area].getOffset() ) break;
    if( iBin >= m_AreaSet[i_area].getOffset()
        && (i_area == narea-1 || iBin < m_AreaSet[i_area+1].getOffset()) ) break;
  }
  return &(m_AreaSet[i_area]);
}

// return area for given global bin number
const CaloLocalHadCoeff::LocalHadArea *CaloLocalHadCoeff::getAreaFromBin(int iBin, int &i_area) const
{
  i_area = 0;
  int narea = m_AreaSet.size();
  for(i_area=0; i_area<narea; i_area++) {
    if( iBin < m_AreaSet[i_area].getOffset() ) break;
    if( iBin >= m_AreaSet[i_area].getOffset()
        && (i_area == narea-1 || iBin < m_AreaSet[i_area+1].getOffset()) ) break;
  }
  return &(m_AreaSet[i_area]);
}



/* ***************************************************************************
CaloLocalHadCoeff: coefficients methods
*************************************************************************** */

// set coefficient
void CaloLocalHadCoeff::setCoeff(const int iBin, const LocalHadCoeff & theCoeff)
{
  m_CoeffSet[iBin] = theCoeff;
}

// return coefficient for given global bin number
const CaloLocalHadCoeff::LocalHadCoeff * CaloLocalHadCoeff::getCoeff(const int & iBin) const
{
  if ( iBin >= 0 && iBin < (int)m_CoeffSet.size() ) {
    return (& m_CoeffSet[iBin]);
  }else{
    return nullptr;
  }
}

// return coefficient for given area number and list of cluster variables
const CaloLocalHadCoeff::LocalHadCoeff * CaloLocalHadCoeff::getCoeff(const int &n_area, std::vector<float> &vars) const
{
  int iBin = getBin(n_area, vars);
  return getCoeff(iBin);
}



/* ***************************************************************************
CaloLocalHadCoeff: bin's methods
*************************************************************************** */

// return global bin number for given area and list of cluster variables
// size of input vector must be equal to the number of dimensions defined for this area
int CaloLocalHadCoeff::getBin(const int n_area, std::vector<float> &vars) const
{
  const LocalHadArea *area = &m_AreaSet[n_area];
  // loop over dimensions
  int iBin = area->getOffset();
  for(int i_dim=0; i_dim<area->getNdim(); i_dim++){
    int i_bin_dim = area->getDimension(i_dim)->getBin( vars[i_dim] );
    if(i_bin_dim < 0) return -1;
    iBin += i_bin_dim*area->getDimLoc(i_dim);
  }
  return iBin;
}

// returns general iBin for given area and vector of bin numbers for each dimension
// size of input vector must be equal to the number of dimensions defined for this area
int CaloLocalHadCoeff::getBin(const int n_area, std::vector<int> &v_indexes) const
{
  const LocalHadArea *area = &m_AreaSet[n_area];
  // loop over dimensions
  int iBin = area->getOffset();
  for(int i_dim=0; i_dim<area->getNdim(); i_dim++){
    if(v_indexes[i_dim] < 0) return -1;
    iBin += v_indexes[i_dim]*area->getDimLoc(i_dim);
  }
  return iBin;
}

// convert general iBin to vector of bins for each dimension
int CaloLocalHadCoeff::bin2indexes(const int iBin, std::vector<int> &v_dim_indx) const
{
  const LocalHadArea *theArea = getAreaFromBin(iBin);
  v_dim_indx.resize(theArea->getNdim(),0);
  for(int i_dim=0; i_dim<theArea->getNdim(); i_dim++){
    int x0 = 0;
    for(int j=0; j<i_dim; j++){
      x0 += v_dim_indx[j]*theArea->getDimLoc(j);
    }
    v_dim_indx[i_dim] = (iBin-theArea->getOffset()-x0)/theArea->getDimLoc(i_dim);
  }
  return 0;
}

// return and arrays needed for interpolation for a given area (n_area), given list of dimensions (dim) and given phase space point (x)
// for speed reason, no check on dimensions !!!!!!!!
bool CaloLocalHadCoeff::getInterpArrays(const int n_area, const std::vector<int> &dim, std::vector<float> &x, 
                                        std::vector<double> &xadj, std::vector<unsigned int> &gbin) const {
  // sanity check
  if(n_area<0 || n_area > (int)m_AreaSet.size()) return false;
  const LocalHadArea *area=&(m_AreaSet[n_area]);
  if(dim.empty()) {
    std::cout << "CaloLocalHadCoeff::getInterpArrays() -> Error! Empty dimension list" << std::endl;
    return false;
  }else if( (int)dim.size() > area->getNdim()) {
    std::cout << "CaloLocalHadCoeff::getInterpArrays() ->  Error! Vector of dimensions to interpolate exceed defined in area." << std::endl;
    return false;
  }
  int ndim = dim.size();

  if((int)x.size()!= area->getNdim()) {
    std::cout<<"CaloLocalHadCoeff::getInterpArrays() -> Error! Wrong size of phase space point !!!"<<std::endl;
    return false;
  }

  xadj.resize(ndim);
  unsigned int ncorners = (1<<ndim);
  gbin.resize(ncorners);

  float xa;
  int ibin, i;
  unsigned int i_len;
  std::vector<int> vTmp;
  std::vector<int> vIdx(area->getNdim());
  std::vector<std::vector<int> > vpW;
  for(i=0; i<ndim; i++){
    ibin = (area->getDimension(dim[i]))->getBinAdjusted(x[dim[i]],xa);
    //if(xadj<0) printf("%d x:%6.3f  xadj:%6.3f ibin:%d ibin+1:%d  \n",i, x[i], xadj, ibin, ibin+1);
    xadj[i] = xa;
    vTmp.clear();
    vTmp.push_back(ibin);
    if(ibin< (area->getDimension(dim[i]))->getNbins()-1) vTmp.push_back(ibin+1);
    else vTmp.push_back(ibin);
    vpW.push_back(vTmp);
  }
  // fill the vIdx for dimensions we are not interpolating over
  for(i=0; i<area->getNdim(); ++i){
    if(std::find(dim.begin(),dim.end(),i) != dim.end()) continue;
    vIdx[i] = area->getDimension(i)->getBin(x[i]);
  }

  // now find the global bin for all corners
  //vTmp.clear();
  //vTmp.resize(ndim);
  for(i_len=0; i_len<ncorners; ++i_len){
    for( i=0; i<ndim; i++){
      //std::cout<<"Sec. idx: "<<int(i_len/int(pow(2,i)))%int(pow(2,i))<<std::endl;
      if(i==0) vIdx[dim[i]] = vpW[i][i_len%2];
      else vIdx[dim[i]] = vpW[i][int(i_len/int(pow(2,i)))%int(pow(2,i))];
      //if(i==2) std::cout<< vTmp[i] <<" ";  
      //std::cout<< vTmp[i] <<" ";  
    }
    // std::cout<<std::endl;
    ibin = getBin(n_area,vIdx);
    gbin[i_len] = ibin;
  }
  return true;
}

