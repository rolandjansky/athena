/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// //-----------------------------------------------------------------------
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
#include "CaloUtils/CaloLCCoeffHelper.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <cmath>
#include "boost/io/ios_state.hpp"

#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloUtils/CaloWeightInterpolator.h"


CaloLCCoeffHelper::CaloLCCoeffHelper()
{

}


CaloLCCoeffHelper::~CaloLCCoeffHelper()
{

}


// get HadDMArea from area name
const CaloLocalHadCoeff::LocalHadArea * CaloLCCoeffHelper::getAreaFromName(const CaloLocalHadCoeff * coeff, const std::string& sname, int &indx) const
{
  for(int i_area=0; i_area<coeff->getSizeAreaSet(); i_area++) {
    if(sname == coeff->getArea(i_area)->getTitle()) {
      indx = i_area;
      return coeff->getArea(i_area);
    }
  }
  std::cout << "CaloLCCoeffHelper::getAreaFromName() -> Error! No such area '" << sname << "'" << std::endl;
  return nullptr;
}



/* ****************************************************************************
To read set of local hadronic coefficients from text file
**************************************************************************** */
CaloLocalHadCoeff *CaloLCCoeffHelper::InitDataFromFile(const char *filename)
{
  CaloLocalHadCoeff *data = new CaloLocalHadCoeff();

  char cLine[1024];

  // Find the full path to filename
  std::cout << "CaloLCCoeffHelper::InitDataFromFile - Reading file '" << filename << "'." << std::endl;

  std::ifstream fin(filename);
  if ( !fin ) {
    std::cout << "CaloLCCoeffHelper::InitDataFromFile - Can't open file '" << filename << "'." << std::endl;
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
      std::cout << "CaloLCCoeffHelper::initDataFromFile() -> Error! Could not parse line '" << cLine << "' at p1." << std::endl;
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
        std::cout << "CaloLCCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << sLine << "' at p2a." << std::endl;
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
        std::cout << "CaloLCCoeffHelper::initDataFromFile() -> Warning! Area " << theArea.getTitle() << " doesn't have parameters." << std::endl;
        break;
      }
      if(idummy != theArea.getOffset()+i_len){
        std::cout << "CaloLCCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p3." << std::endl;
        delete data; return nullptr;
      }
      for(int j=0; j<theArea.getNdim(); j++) {
        if(!(ist >> idummy)) {
          std::cout << "CaloLCCoeffHelper::initDataFromFile() -> panic!" << std::endl;
          delete data; return nullptr;
        }
      }
      CaloLocalHadCoeff::LocalHadCoeff pars;
      pars.resize(theArea.getNpars(),0.0);
      for(int j=0; j<theArea.getNpars(); j++) {
        if( !(ist >> pars[j]) ) {
          std::cout << "CaloLCCoeffHelper::initDataFromFile() ->Error! Could not parse line '" << cLine << "' at p4." << std::endl;
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
void CaloLCCoeffHelper::PrintData(CaloLocalHadCoeff *data, const char *fname)
{
  std::ofstream fout;
  fout.open(fname);
  PrintData(data, fout);
  fout.close();
}



/* ****************************************************************************

**************************************************************************** */
void CaloLCCoeffHelper::PrintData(CaloLocalHadCoeff *data, std::ostream &fout)
{
  boost::io::ios_base_all_saver ifs (fout);
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
      snprintf(line,sizeof(line),
               "%-6s %2d %6.3f %12.3f ",dim->getTitle().c_str(), dim->getNbins(), dim->getXmin(), dim->getXmax() );
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
        std::cout << "CaloLCCoeffHelper::PrintData() -> Error! Wrong bin number" << std::endl;
        return;
      }
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
   Interpolate data, area  in phase space point x, over dimensions dim (all if 0)
   for speed reason, no check on dim content !!!!!!!!
*************************************************************************** */
bool CaloLCCoeffHelper::Interpolate(const CaloLocalHadCoeff *data, const unsigned int n_area, 
                    std::vector<float> &x, CaloLocalHadCoeff::LocalHadCoeff &pars,  
                    const std::vector<int> &dim, double xfit) 
{
  // Sanity check
  if( n_area >= (unsigned int)data->getSizeAreaSet()) return false;
  // We are not extrapolator
  if(!data->isFilled(data->getBin(n_area,x))) {
    return false;
  }

  const CaloLocalHadCoeff::LocalHadArea *area = data->getArea(n_area);
  unsigned int ndim;
  if(dim.empty()) {
    std::cout << "CaloLCCoeffHelper::Interpolate() -> Error! Empty vector of dimensions to interpolate trough." << std::endl;
    return false;
  } else if ( (int)dim.size() > area->getNdim()){  
    std::cout << "CaloLCCoeffHelper::Interpolate() -> Error! Vector of dimensions to interpolate exceed defined in area." << std::endl;
    return false;
  } else ndim=dim.size();
  unsigned int ncorners = (1<<ndim);

  std::vector<double > vXadj(ndim);
  std::vector<unsigned int > vgbins(ncorners);
  std::vector<double > vWeights(ncorners);
  std::vector<int> ebindx;
  std::vector<int> bx;
  unsigned int nip;

  switch (area->getType()) {
    case CaloLocalHadDefs::AREA_STD: case CaloLocalHadDefs::AREA_DMLOOKUP : case CaloLocalHadDefs::AREA_DMSMPW: case CaloLocalHadDefs::AREA_DMFIT: {
      nip = data->getCoeff(area->getOffset()+1)->size(); 
      if(area->getType() == CaloLocalHadDefs::AREA_DMFIT) {
        nip = 1;
      } else {
        nip = data->getCoeff(area->getOffset()+1)->size();
      }
      pars.resize(nip);
      for(unsigned int ip=0; ip<nip; ++ip) {
        bool isa =   data->getInterpArrays(n_area,dim,x,vXadj,vgbins);
        if(! isa) {
          std::cout<<"No arrays for x: ";
          for(unsigned int l=0; l<x.size(); ++l) std::cout<<x[l]<<" ";
          std::cout<<std::endl;
          return false;
        }
        for(unsigned int i_len=0; i_len<ncorners; ++i_len){
          // Check for empty bin here TODO !!!!!
          if(data->isFilled(vgbins[i_len])) {
            const CaloLocalHadCoeff::LocalHadCoeff * lpars =
              data->getCoeff(vgbins[i_len]);
            if(area->getType() == CaloLocalHadDefs::AREA_DMFIT) {
              // std::cout << "xxx " << xfit << " " << (*pars)[0] << " " << (*pars)[1] << " " << (*pars)[2] << std::endl;
              vWeights[i_len] = (*lpars)[0] + (*lpars)[1]*pow(xfit,(*lpars)[2]);
              } else {
                vWeights[i_len] = (*lpars)[ip];
              }
              //   std::cout<<vWeights[i_len]<<" ";
          } else {
            // std::cout<<x[0]<<" "<<x[1]<<"  :  "<<vXadj[0]<<" "<<vXadj[1]<<" | ";
            // if empty, put inside average over all neighbours in our list
            vWeights[i_len] = 0.;
            unsigned int icount = 0;
            data->bin2indexes(vgbins[i_len],ebindx);
            // for(unsigned int blen=0; blen<ebindx.size(); ++blen) { std::cout<<ebindx[blen]<<" "; }
            //std::cout<<" | ";
            for(unsigned int blen=0; blen<ncorners; ++blen){
              if(blen == i_len) continue;
              if(!data->isFilled(vgbins[blen])){
                data->bin2indexes(vgbins[i_len],bx);
                //for(unsigned int l=0; l<bx.size(); ++l) { std::cout<<bx[l]<<" "; }
                //std::cout<<" nf| ";
                continue;
              }
              if(data->isNeighbour(vgbins[blen], ebindx)) {
                const CaloLocalHadCoeff::LocalHadCoeff * lpars =
                  data->getCoeff(vgbins[blen]);
                if(area->getType() == CaloLocalHadDefs::AREA_DMFIT) {
                  vWeights[i_len] = (*lpars)[0] + (*lpars)[1]*pow(xfit,(*lpars)[2]);
                } else {
                  vWeights[i_len] += (*lpars)[ip];
                }
                ++icount;
              } 
            }
            if(icount) {
              //std::cout<<"i"; 
              vWeights[i_len] /= icount;
            }
            //std::cout<<vWeights[i_len]<<"a ";
            //std::cout<<std::endl;
          }
        }
        pars[ip] = CaloWeightInterpolator::getWeight(vWeights,vXadj);
      }
      return true;
    }
    default: {
      std::cout<<"Not implemented yet for this Area type !!!"<<std::endl;
      return false;
    }
  }
  // Should not get here, but syntax needs this
  return false;
}



/* **************************************************************************
parsing dimension string  of type 'ener   8   3.1    6.3'
*************************************************************************** */
CaloLocalHadCoeff::LocalHadDimension *CaloLCCoeffHelper::parse_dim(std::string &sLine)
{
  CaloLocalHadCoeff::LocalHadDimension *dim = nullptr;
  std::istringstream ist(sLine.c_str());

  std::string dim_title;
  std::string stype;
  int dim_nbins(0), dim_type(0);
  float dim_xmax(0), dim_xmin(0);

  if( !(ist >> dim_title >> dim_nbins >> dim_xmin >> dim_xmax >> stype) ){
    std::cout << "CaloHadDMCoeffHelper::parse_dim() -> Error! Could not parse line '" << sLine << "' at p1." << std::endl;
    return nullptr;
  }

  // Check nbins for reasonableness --- prevents a coverity warning.
  if (dim_nbins < 1)
    dim_nbins = 1;
  else if (dim_nbins > 10000)
    dim_nbins = 10000;

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

