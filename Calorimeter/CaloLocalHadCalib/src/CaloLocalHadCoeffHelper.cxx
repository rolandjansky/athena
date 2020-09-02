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
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include "boost/io/ios_state.hpp"


#define MAX_BUFFER_LEN 1024


CaloLocalHadCoeffHelper::CaloLocalHadCoeffHelper()
{

}


CaloLocalHadCoeffHelper::~CaloLocalHadCoeffHelper()
{

}


// get HadDMArea from area name
const CaloLocalHadCoeff::LocalHadArea * CaloLocalHadCoeffHelper::getAreaFromName(const CaloLocalHadCoeff * coeff, const std::string& sname, int &indx) const
{
  for(int i_area=0; i_area<coeff->getSizeAreaSet(); i_area++) {
    if(sname == coeff->getArea(i_area)->getTitle()) {
      indx = i_area;
      return coeff->getArea(i_area);
    }
  }
  std::cout << "CaloLocalHadCoeffHelper::getAreaFromName() -> Error! No such area '" << sname << "'" << std::endl;
  return nullptr;
}



/* ****************************************************************************
To read set of local hadronic coefficients from text file
**************************************************************************** */
CaloLocalHadCoeff *CaloLocalHadCoeffHelper::InitDataFromFile(const char *filename)
{
  CaloLocalHadCoeff *data = new CaloLocalHadCoeff();

  char cLine[MAX_BUFFER_LEN];

  // Find the full path to filename
  std::cout << "CaloLocalHadCoeffHelper::InitDataFromFile - Reading file '" << filename << "'." << std::endl;

  std::ifstream fin(filename);
  if ( !fin ) {
    std::cout << "CaloLocalHadCoeffHelper::InitDataFromFile - Can't open file '" << filename << "'." << std::endl;
    delete data; return nullptr;
  }

  std::string sLine;
  std::istringstream ist;
  while(fin.getline(cLine,sizeof(cLine)-1)) {
    if( strlen(cLine)==0 ||  cLine[0] == '#' || cLine[0] == '\n') continue;

    // parsing area line
    sLine = cLine;
    ist.clear(); ist.str(sLine);
    std::string sdummy, area_title;
    int area_indx(0), area_type(0), area_npars(0);
    if( !(ist >> sdummy >> area_indx >> area_title >> area_type >> area_npars) ) {
      std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() -> Error! Could not parse line '" << cLine << "' at p1." << std::endl;
      delete data; return nullptr;
    }

    CaloLocalHadCoeff::LocalHadArea theArea(area_title.c_str(), area_type, area_npars);

    // loop over defined dimensions
    while(fin.getline(cLine,sizeof(cLine)-1)){
      if( cLine[0] == '#') continue;
      sLine = cLine;
      if(sLine.find("break") != std::string::npos) {
        break;
      }
      CaloLocalHadCoeff::LocalHadDimension *dim = parse_dim(sLine);
      if( !dim ) {
        std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << sLine << "' at p2a." << std::endl;
        delete data; return nullptr;
      }
      theArea.addDimension(*dim);
      delete dim;
    }

    data->addArea(theArea);

    // now reading parameters
    for(int i_len=0; i_len<theArea.getLength(); i_len++){
      if(!fin.getline(cLine,sizeof(cLine)-1)) {
        std::cout << "panic " << std::endl;
        delete data; return nullptr;
      }
      sLine = cLine;
      ist.clear(); ist.str(sLine);
      int idummy;
      if( !(ist >> idummy) ) {
        std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() -> Warning! Area " << theArea.getTitle() << " doesn't have parameters." << std::endl;
        break;
      }
      if(idummy != theArea.getOffset()+i_len){
        std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p3." << std::endl;
        delete data; return nullptr;
      }
      for(int j=0; j<theArea.getNdim(); j++) {
        if(!(ist >> idummy)) {
          std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() -> panic!" << std::endl;
          delete data; return nullptr;
        }
      }
      CaloLocalHadCoeff::LocalHadCoeff pars;
      pars.resize(theArea.getNpars(),0.0);
      for(int j=0; j<theArea.getNpars(); j++) {
        if( !(ist >> pars[j]) ) {
          std::cout << "CaloLocalHadCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p4." << std::endl;
          std::cout << " dmArea.m_title" << theArea.getTitle() << std::endl;
          delete data; return nullptr;
        }
      }
      data->setCoeff(theArea.getOffset()+i_len, pars);
    }
  }
  fin.close();

  return data;
}



/* ****************************************************************************

**************************************************************************** */
void CaloLocalHadCoeffHelper::PrintData(const CaloLocalHadCoeff *data, const char *fname)
{
  std::ofstream fout;
  fout.open(fname);
  PrintData(data, fout);
  fout.close();
}



/* ****************************************************************************

**************************************************************************** */
void CaloLocalHadCoeffHelper::PrintData(const CaloLocalHadCoeff *data, std::ostream &fout)
{
  const char *comments = 
  {
    "# Coefficients for local hadronic calibration .\n\n"
  };
  fout << comments << std::endl;
  char line[1024];

  // loop over areas
  for(int i_area=0; i_area < data->getSizeAreaSet(); i_area++){
    const CaloLocalHadCoeff::LocalHadArea *area = data->getArea(i_area);
    fout << "area  " << i_area << "  " << area->getTitle() << "  " << area->getType() << " " << area->getNpars() << std::endl;
    for(int i_dim=0; i_dim<area->getNdim(); i_dim++){
      const CaloLocalHadCoeff::LocalHadDimension *dim = area->getDimension(i_dim);
      sprintf(line,"%-6s %2d %6.3f %12.3f ",dim->getTitle().c_str(), dim->getNbins(), dim->getXmin(), dim->getXmax() );
       std::string sline(line);
       sline += "flat";
       fout << sline;
//       if( !dim.m_xbins.size() ) {
//         sline += "flat";
//         fout << sline;
//       }else {
//         sline += "hand";
//         fout << sline;
//         for(unsigned int i=0; i<dim.m_xbins.size(); i++){
//           fout << " " << dim.m_xbins[i];
//         }
//       }
      fout << std::endl;
    }
    fout << "break" << std::endl; // i.e. no more dimensions

    // now printing the data
    for(int i_data=0; i_data<area->getLength(); i_data++) {
      int indx = area->getOffset() + i_data;
      const CaloLocalHadCoeff::LocalHadCoeff *pars = data->getCoeff(indx);
      if( !pars ) {
        std::cout << "CaloLocalHadCoeffHelper::PrintData() -> Error! Wrong bin number" << std::endl;
        return;
      }
      boost::io::ios_base_all_saver foutsave (fout);
      fout << std::setw(5) << indx << "   ";
      std::vector<int > v_dim_indexes;
      data->bin2indexes(indx, v_dim_indexes);
      for(unsigned int i_dim=0; i_dim<v_dim_indexes.size(); i_dim++){
        fout << std::setw(4) << v_dim_indexes[i_dim]  << " ";
      }
      fout << "     ";
      for(unsigned int i_par=0; i_par<(*pars).size(); i_par++) {
        fout << std::fixed << std::setprecision(6) << std::setw(12) << (*pars)[i_par] << " ";
      }
      fout << std::endl;
    }

    // end of DM area
    fout << std::endl;
  }
  // printing title strin
}



/* **************************************************************************
parsing dimension string  of type 'ener   8   3.1    6.3'
*************************************************************************** */
CaloLocalHadCoeff::LocalHadDimension *CaloLocalHadCoeffHelper::parse_dim(std::string &sLine)
{
  CaloLocalHadCoeff::LocalHadDimension *dim = nullptr;
  std::istringstream ist(sLine.c_str());

  std::string dim_title;
  std::string stype;
  int dim_nbins(0), dim_type(0);
  float dim_xmax(0), dim_xmin(0);

  if( !(ist >> dim_title >> dim_nbins >> dim_xmin >> dim_xmax >> stype) ||
      dim_nbins <= 0 ||
      dim_nbins > 1000)
  {
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at p1." << std::endl;
    return nullptr;
  }

  if(stype.find("flat") != std::string::npos) {
    // equidistant binning
    dim = new CaloLocalHadCoeff::LocalHadDimension(dim_title.c_str(), dim_type, dim_nbins, dim_xmin, dim_xmax);
  }else if(stype.find("hand") != std::string::npos) {
    // user defined binning
    std::vector<float> x_bins;
    float e;
    for(int i=0; i<dim_nbins+1; i++) {
      if( !(ist >> e) ) {
        std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at p2." << std::endl;
        return nullptr;
      }else{
        x_bins.push_back(e);
      }
    }
    dim = new CaloLocalHadCoeff::LocalHadDimension(dim_title.c_str(), dim_type, x_bins);
  }else{
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at p3." << std::endl;
    return nullptr;
  }
  return dim;
}
