/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloHadDMCoeffHelper.cxx,v 1.2 2009-03-06 14:43:23 pospelov Exp $
//
// Description: see CaloHadDMCoeffHelper.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------
#include "CaloLocalHadCalib/CaloHadDMCoeffHelper.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#define MAX_BUFFER_LEN 1024


CaloHadDMCoeffHelper::CaloHadDMCoeffHelper()
{

}

CaloHadDMCoeffHelper::~CaloHadDMCoeffHelper()
{

}


// get HadDMArea from bin number
const CaloHadDMCoeff2::HadDMArea * CaloHadDMCoeffHelper::getHadDMAreaFromBin(CaloHadDMCoeff2 * coeff, int iBin) const 
{
  int i_area = 0;
  int narea = coeff->getSizeDMAreaSet();
  for(i_area=0; i_area<narea; i_area++) {
    if( iBin < coeff->getHadDMArea(i_area)->m_offset ) break;
    if( iBin >= coeff->getHadDMArea(i_area)->m_offset 
        && (i_area == narea-1 || iBin < coeff->getHadDMArea(i_area+1)->m_offset) ) break;
  }
  return coeff->getHadDMArea(i_area);
}

// get HadDMArea from area name
const CaloHadDMCoeff2::HadDMArea * CaloHadDMCoeffHelper::getHadDMAreaFromName(CaloHadDMCoeff2 * coeff, const std::string& sname) const 
{
  for(int i_area=0; i_area<coeff->getSizeDMAreaSet(); i_area++) {
    if(sname == coeff->getHadDMArea(i_area)->m_title) {
      return coeff->getHadDMArea(i_area);
    }
  }
  std::cout << "CaloHadDMCoeffHelper::getHadDMAreaFromName() -> Error! No such dmArea '" << sname << "'" << std::endl;
  return nullptr;
}


int CaloHadDMCoeffHelper::getBin(CaloHadDMCoeff2 * coeff, const int n_area, int ifrac, int iener, int ilambda, int ieta) const
{
  const CaloHadDMCoeff2::HadDMArea *dmArea = coeff->getHadDMArea(n_area);
  int iBin = ieta + ilambda*dmArea->m_dimEta.m_nbins 
      + iener*dmArea->m_dimEta.m_nbins*dmArea->m_dimLambda.m_nbins 
      + ifrac*dmArea->m_dimEta.m_nbins*dmArea->m_dimLambda.m_nbins*dmArea->m_dimEner.m_nbins 
      + dmArea->m_offset;
  return iBin;
}


/* ****************************************************************************

**************************************************************************** */
CaloHadDMCoeff2 *CaloHadDMCoeffHelper::InitDataFromFile(std::string &filename)
{
  CaloHadDMCoeff2 *data = new CaloHadDMCoeff2();

  char cLine[MAX_BUFFER_LEN];

  // Find the full path to filename
  std::cout << "CaloHadDMCoeffHelper::InitDataFromFile() - Reading file '" << filename << "'." << std::endl;

  std::ifstream fin(filename.c_str());
  if ( !fin ) {
    std::cout << "CaloHadDMCoeffHelper::InitDataFromFile() - Can't open file '" << filename << "'." << std::endl;
    delete data; return nullptr;
  }

  int offset = 0;
  int indx = 0;

  std::string sLine;
  std::string sdummy;
  std::istringstream ist;
  while(fin.getline(cLine,sizeof(cLine)-1)) {
    if( strlen(cLine)==0 ||  cLine[0] == '#' || cLine[0] == '\n') continue;

    // creating new dead material area
    CaloHadDMCoeff2::HadDMArea dmArea;
    // parsing zone line
    sLine = cLine;
    ist.clear(); ist.str(sLine);
    if( !(ist >> sdummy >> dmArea.m_indx >> dmArea.m_is_on >> dmArea.m_title) && sdummy.find("zone")==std::string::npos ) {
      std::cout << "CaloHadDMCoeffHelper::initDataFromFile() -> Error! Could not parse line '" << cLine << "' at p1." << std::endl;
      delete data; return nullptr;
    }

    dmArea.m_type = kAREA_PROF;
    if(dmArea.m_title.find("DEAD_FCAL") != std::string::npos) {
      dmArea.m_type = kAREA_SMPW;
    }else if(dmArea.m_title.find("DEAD_LEAKAGE") != std::string::npos) {
      dmArea.m_type = kAREA_LOOKUP;
    }else if(dmArea.m_title.find("DEAD_UNCLASS") != std::string::npos) {
      dmArea.m_type = kAREA_LOOKUP;
    }

    // loop over dimensions of this zones
    std::vector<CaloHadDMCoeff2::HadDMDimension > v_dims;
    while(fin.getline(cLine,sizeof(cLine)-1)){
      if( cLine[0] == '#') continue;
      sLine = cLine;
      // reading number of parameters
      if(sLine.find("npars") != std::string::npos){
        ist.clear(); ist.str(sLine);
        if( !(ist >> sdummy >> dmArea.m_nPars) ||
            dmArea.m_nPars < 0 ||
            dmArea.m_nPars > 1000)
        {
          std::cout << "CaloHadDMCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p2." << std::endl;
          delete data; return nullptr;
        }
        // this is the end of zone
        break;
      }
      CaloHadDMCoeff2::HadDMDimension dim;
      if( !parse_dim(sLine, dim) ) {
        std::cout << "CaloHadDMCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << sLine << "' at p2a." << std::endl;
        delete data; return nullptr;
      }
      v_dims.push_back(dim);
    }
    // calculation offset for this area
    int ndim = (int) v_dims.size();
    int length =  1;
    for(int i=0; i<ndim; i++){
      length = length*v_dims[i].m_vBins.size();
    }
    dmArea.m_offset = offset;
    dmArea.m_length = length;
    // calculation of dimension locator coefficient
    for(int i_dim=0; i_dim<ndim; i_dim++){
      int xloc = 1;
      for(int j=i_dim+1; j<ndim; j++){
        xloc = xloc*v_dims[j].m_vBins.size();
      }
      v_dims[i_dim].xloc = xloc;
    }
    if( v_dims.size() != 4 ){
      std::cout << "CaloHadDMCoeffHelper::initDataFromFile() ->Error! Wrong number of dimensions for area'" << dmArea.m_title << "' at p3." << std::endl;
      delete data; return nullptr;
    }
    dmArea.m_dimFrac = v_dims[0];
    dmArea.m_dimEner = v_dims[1];
    dmArea.m_dimLambda = v_dims[2];
    dmArea.m_dimEta = v_dims[3];

    data->addHadDMArea(dmArea);
    offset += length; // ofset for next DM area
    indx++;
    // putting zero parameters
    CaloHadDMCoeff2::HadDMCoeff pars;
    pars.resize(dmArea.m_nPars, 0.0);
    for(int i_len=0; i_len<dmArea.m_length; i_len++) data->addHadDMCoeff(pars);
    // now reading parameters
    for(int i_len=0; i_len<dmArea.m_length; i_len++){
      if(!fin.getline(cLine,sizeof(cLine)-1)) {
        std::cout << "panic " << std::endl;
        delete data; return nullptr;
      }
      sLine = cLine;
      ist.clear(); ist.str(sLine);
      int idummy;
      if( !(ist >> idummy) ) {
        std::cout << "CaloHadDMCoeffHelper::initDataFromFile() -> Warning! Area " << dmArea.m_title << " doesn't have parameters." << std::endl;
        break;
      }
      if(idummy != dmArea.m_offset+i_len){
        std::cout << "CaloHadDMCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p3." << std::endl;
        delete data; return nullptr;
      }
      for(int j=0; j<(int)v_dims.size(); j++) {
        if(!(ist >> idummy)) {
          std::cout << "CaloHadDMCoeffHelper::initDataFromFile() -> panic!" << std::endl;
          delete data; return nullptr;
        }
      }
      CaloHadDMCoeff2::HadDMCoeff pars;
      pars.resize(dmArea.m_nPars,0.0);
      for(int j=0; j<dmArea.m_nPars; j++) {
        if( !(ist >> pars[j]) ) {
          std::cout << "CaloHadDMCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p4." << std::endl;
          std::cout << " dmArea.m_title" << dmArea.m_title << std::endl;
          delete data; return nullptr;
        }
      }
      data->setHadDMCoeff(dmArea.m_offset+i_len,pars);
    }
  }
  fin.close();

  return data;
}



/* **************************************************************************
parsing dimension string
*************************************************************************** */
int CaloHadDMCoeffHelper::parse_dim(std::string &sLine, CaloHadDMCoeff2::HadDMDimension &dim)
{
  std::string dummy;
  std::string stype;
  std::istringstream ist(sLine.c_str());

  if( !(ist >> dim.m_title >> dim.m_nbins >> dim.m_vMin >> dim.m_vMax >> stype)||
      dim.m_nbins < 0 ||
      dim.m_nbins > 1000)
  {
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp1." << std::endl;
    return 0;
  }
  // bins according to flat scale
  if(stype.find("flat") != std::string::npos) {
    dim.m_type = CaloHadDMCoeff2::kDIM_FLAT;
    dim.m_bin_size = (dim.m_vMax - dim.m_vMin)/float(dim.m_nbins);
    for(int i=0; i<dim.m_nbins; i++) dim.m_vBins.push_back(dim.m_vMin + i*(dim.m_vMax - dim.m_vMin)/float(dim.m_nbins));
  // bins are setted in the string
  }else if(stype.find("hand") != std::string::npos) {
    dim.m_type = CaloHadDMCoeff2::kDIM_HAND;
    float e;
    for(int i=0; i<dim.m_nbins; i++) if( ist >> e ) dim.m_vBins.push_back(e);
  }else{
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp3." << std::endl;
    return 0;
  }
  if((int)dim.m_vBins.size() != dim.m_nbins){
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at pp4." << std::endl;
    return 0;
  }
  return (int) dim.m_vBins.size();
}

