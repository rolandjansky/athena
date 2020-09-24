/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloHadDMCoeff2.h"

#include "boost/io/ios_state.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

/* ***************************************************************************
c-tor
*************************************************************************** */
CaloHadDMCoeff2::CaloHadDMCoeff2()
{

}


CaloHadDMCoeff2::CaloHadDMCoeff2(const CaloHadDMCoeff2 &other)
  : m_DMCoeffSet (other.m_DMCoeffSet),
    m_DMAreaSet (other.m_DMAreaSet),
    m_area_offsets (other.m_area_offsets)
{
}


CaloHadDMCoeff2& CaloHadDMCoeff2::operator= (const CaloHadDMCoeff2& other)
{
  if (this != &other) {
    m_DMCoeffSet = other.m_DMCoeffSet;
    m_DMAreaSet = other.m_DMAreaSet;
    m_area_offsets = other.m_area_offsets;
  }
  return *this;
}


/* ***************************************************************************
methods to deal with DM areas
*************************************************************************** */
void CaloHadDMCoeff2::addHadDMArea(const HadDMArea & theArea)
{
  m_DMAreaSet.push_back(theArea);
  m_area_offsets.push_back(theArea.m_offset);
}

void CaloHadDMCoeff2::setHadDMArea(const int n_area, const HadDMArea & theArea)
{
  m_DMAreaSet[n_area] = theArea;
}

const CaloHadDMCoeff2::HadDMArea * CaloHadDMCoeff2::getHadDMArea(int n_area) const 
{
  if(n_area >= 0 && n_area<(int)m_DMAreaSet.size() ) {
    return &(m_DMAreaSet[n_area]);
  } else {
    return nullptr;
  }
}


/* ***************************************************************************
methods to deal with DM coefficients
*************************************************************************** */
void CaloHadDMCoeff2::addHadDMCoeff(const HadDMCoeff & theData)
{
  m_DMCoeffSet.push_back(theData);
}

void CaloHadDMCoeff2::setHadDMCoeff(const int iBin, const HadDMCoeff & theData)
{
  m_DMCoeffSet[iBin] = theData;
}

const CaloHadDMCoeff2::HadDMCoeff * CaloHadDMCoeff2::getHadDMCoeff(const int & iBin) const
{
  if ( iBin > -1 && iBin < (int)m_DMCoeffSet.size() ) {
    return (& m_DMCoeffSet[iBin]);
  }else{
    return nullptr;
  }
}


/* ***************************************************************************
methods to get DM coefficients bin number
*************************************************************************** */

// get EM fraction bin number
int CaloHadDMCoeff2::getBinFrac(const int n_area, const float & cls_emfrac) const
{
  const HadDMArea *dmArea = &m_DMAreaSet[n_area];
  if(cls_emfrac < dmArea->m_dimFrac.m_vMin || cls_emfrac >= dmArea->m_dimFrac.m_vMax) return (-1);
  return (int)((cls_emfrac - dmArea->m_dimFrac.m_vMin)/dmArea->m_dimFrac.m_bin_size);
}

// get energy bin number
int CaloHadDMCoeff2::getBinEner(const int n_area, const float & cls_ener) const
{
  const HadDMArea *dmArea = &m_DMAreaSet[n_area];
  if( cls_ener < dmArea->m_dimEner.m_vMin || cls_ener >= dmArea->m_dimEner.m_vMax ) return (-1);
  int iener;
  for (iener=0; iener<dmArea->m_dimEner.m_nbins; iener++) {
    if( cls_ener < dmArea->m_dimEner.m_vBins[iener] ) break;
    if( cls_ener >= dmArea->m_dimEner.m_vBins[iener] && (iener == dmArea->m_dimEner.m_nbins-1 || cls_ener < dmArea->m_dimEner.m_vBins[iener+1]) ) break;
  }
  return iener;
}

// get lambda bin number
int CaloHadDMCoeff2::getBinLambda(const int n_area, const float & cls_lambda) const
{
  const HadDMArea *dmArea = &m_DMAreaSet[n_area];
  if(cls_lambda < dmArea->m_dimLambda.m_vMin || cls_lambda >= dmArea->m_dimLambda.m_vMax) return (-1);
  return (int)((cls_lambda - dmArea->m_dimLambda.m_vMin)/dmArea->m_dimLambda.m_bin_size);
}

// get eta bin number
int CaloHadDMCoeff2::getBinEta(const int n_area, const float & cls_eta) const
{
  const HadDMArea *dmArea = &m_DMAreaSet[n_area];
  float cls_abseta = fabs(cls_eta);
  if(cls_abseta < dmArea->m_dimEta.m_vMin || cls_abseta >= dmArea->m_dimEta.m_vMax) return (-1);
  return (int)((cls_abseta - dmArea->m_dimEta.m_vMin)/dmArea->m_dimEta.m_bin_size);
}

// get general bin number
int CaloHadDMCoeff2::getBin(const int n_dm_area, const float &cls_emfrac, const float &cls_ener, const float &cls_lambda, const float &cls_eta) const
{
  if(n_dm_area < 0 || n_dm_area >= (int)m_DMAreaSet.size()) {
    std::cout << " WARNING CaloHadDMCoeff::getBin -> wrong n_area: " << n_dm_area << std::endl;
    return (-1);
  }
  int iBin = -1;
  const HadDMArea *dmArea = &m_DMAreaSet[n_dm_area];
  int ifrac = -1, iener = -1, ilambda = -1, ieta = -1;
  ifrac = getBinFrac(n_dm_area, cls_emfrac);
  if( ifrac != -1 ) {
    iener = getBinEner(n_dm_area, cls_ener);
    if( iener != -1 ) {
      ilambda = getBinLambda(n_dm_area, cls_lambda);
      if( ilambda != -1 ) {
        ieta = getBinEta(n_dm_area, cls_eta);
        if ( ieta != -1 ) {
          iBin = ieta + ilambda*dmArea->m_dimEta.m_nbins 
              + iener*dmArea->m_dimEta.m_nbins*dmArea->m_dimLambda.m_nbins 
              + ifrac*dmArea->m_dimEta.m_nbins*dmArea->m_dimLambda.m_nbins*dmArea->m_dimEner.m_nbins 
              + dmArea->m_offset;
        } // ieta
      }// ilambda
    } // iener
  } // ifrac
  return iBin;
}


const CaloHadDMCoeff2::HadDMCoeff * CaloHadDMCoeff2::getHadDMCoeff(const int & n_area, const float & cls_emfrac, const float & cls_ener, const float & cls_lambda, const float & cls_eta) const
{
   int iBin = getBin(n_area, cls_emfrac, cls_ener, cls_lambda, cls_eta);
   return getHadDMCoeff(iBin);
}



/* ***************************************************************************

Accessories

*************************************************************************** */
int CaloHadDMCoeff2::bin2indexes(const int iBin, std::vector<int> &v_dim_indx) const
{
  int i_area = 0;
  int narea = (int)m_area_offsets.size();
  for(i_area=0; i_area<narea; i_area++) {
    if( iBin < m_area_offsets[i_area] ) break;
    if( iBin >= m_area_offsets[i_area] && (i_area == narea-1 || iBin < m_area_offsets[i_area+1]) ) break;
  }
  const HadDMArea *theArea = &m_DMAreaSet[i_area];
  std::vector<HadDMDimension > v_dims;
  v_dims.resize(4);
  v_dims[0] = theArea->m_dimFrac;
  v_dims[1] = theArea->m_dimEner;
  v_dims[2] = theArea->m_dimLambda;
  v_dims[3] = theArea->m_dimEta;
  int ndim = (int) v_dims.size();
  v_dim_indx.resize(ndim,0);
  for(int i_dim=0; i_dim<ndim; i_dim++){
    int x0 = 0;
    for(int j=0; j<i_dim; j++){
      x0 += v_dim_indx[j]*v_dims[j].xloc;
    }
    v_dim_indx[i_dim] = (iBin-theArea->m_offset-x0)/v_dims[i_dim].xloc;
  }
  return 0;
}


/* ***************************************************************************
for printing DM correction coefficients into text file
*************************************************************************** */
void CaloHadDMCoeff2::PrintData(std::ostream &fout) const
{
  const char *comments = 
  {
    "# Coeffs for Dead Material corrections in DeadMaterialCorrectionTool2 tool.\n\n"
  };
  fout << comments << std::endl;
  char line[1024];

  // loop over DM areas
  for(int i_zone=0; i_zone < (int)m_DMAreaSet.size(); i_zone++){
    const HadDMArea *area = &m_DMAreaSet[i_zone];
    fout << "zone  " << area->m_indx << "  " << area->m_is_on << "  " << area->m_title << std::endl;
    std::vector<CaloHadDMCoeff2::HadDMDimension > v_dims;
    v_dims.resize(4);
    v_dims[0] = area->m_dimFrac;
    v_dims[1] = area->m_dimEner;
    v_dims[2] = area->m_dimLambda;
    v_dims[3] = area->m_dimEta;
    for(unsigned int i_dim=0; i_dim<v_dims.size(); i_dim++){
      CaloHadDMCoeff2::HadDMDimension dim = v_dims[i_dim];
      sprintf(line,"%-6s %2d %6.3f %12.3f ",dim.m_title.c_str(), dim.m_nbins, dim.m_vMin, dim.m_vMax);
      std::string sline(line);
      if(dim.m_type == CaloHadDMCoeff2::kDIM_FLAT) {
        sline += "flat";
      }else if(dim.m_type == CaloHadDMCoeff2::kDIM_HAND) {
        sline += "hand";
      } else {
        std::cout << "CaloHadDMCoeff2::PrintData2() -> Error! p1." << std::endl;
      }
      fout << sline;
      if(dim.m_type == CaloHadDMCoeff2::kDIM_HAND) {
        for(unsigned int i=0; i<dim.m_vBins.size(); i++){
          fout << " " << dim.m_vBins[i];
        }
      }
      fout << std::endl;
    }
    fout << "npars " << area->m_nPars << std::endl;
    // now printing the data
    for(int i_data=0; i_data<area->m_length; i_data++) {
      int indx = area->m_offset + i_data;
      const HadDMCoeff *pars = getHadDMCoeff(indx);
      fout << std::setw(5) << indx << "   ";
      std::vector<int > v_dim_indexes;
      bin2indexes(indx, v_dim_indexes);
      for(unsigned int i_dim=0; i_dim<v_dim_indexes.size(); i_dim++){
        fout << std::setw(4) << v_dim_indexes[i_dim]  << " ";
      }
      fout << "     ";
      for(int i_par=0; i_par<area->m_nPars; i_par++){
        boost::io::ios_base_all_saver foutsave (fout);
        fout << std::fixed << std::setprecision(5) << std::setw(12) << (*pars)[i_par] << " ";
      }
      fout << std::endl;
    }
    // end of DM area
    fout << std::endl;
  }
  // printing title strin
}
