/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloConditions/CaloHadDMCoeff.h"


CaloHadDMCoeff::CaloHadDMCoeff()
{

}


CaloHadDMCoeff::CaloHadDMCoeff(const CaloHadDMCoeff &other)
  : m_DMCoeffSet (other.m_DMCoeffSet),
    m_DMAreaSet (other.m_DMAreaSet)
{
}


CaloHadDMCoeff& CaloHadDMCoeff::operator= (const CaloHadDMCoeff& other)
{
  if (this != &other) {
    m_DMCoeffSet = other.m_DMCoeffSet;
    m_DMAreaSet = other.m_DMAreaSet;
  }
  return *this;
}


const CaloHadDMCoeff::HadDMCoeff * CaloHadDMCoeff::getHadDMCoeff(const int & n_area, const float & cls_emfrac, const float & cls_ener, const float & cls_eta) const
{
   int iBin = getBin(n_area, cls_emfrac, cls_ener, cls_eta);
   return getHadDMCoeff(iBin);
}
 

const CaloHadDMCoeff::HadDMArea * CaloHadDMCoeff::getHadDMArea(int n_area) const 
{
   if(n_area >= 0 && n_area<(int)m_DMAreaSet.size() ) {
      return &(m_DMAreaSet[n_area]);
   } else {
      return nullptr;
   }
}


void CaloHadDMCoeff::addHadDMCoeff(const HadDMCoeff & theData)
{
   m_DMCoeffSet.push_back(theData);
}


void CaloHadDMCoeff::setHadDMCoeff(const int iBin, const HadDMCoeff & theData)
{
   m_DMCoeffSet[iBin] = theData;
}


void CaloHadDMCoeff::addHadDMArea(const HadDMArea & theArea)
{
   m_DMAreaSet.push_back(theArea);
}


void CaloHadDMCoeff::setHadDMArea(const int n_area, const HadDMArea & theArea)
{
   m_DMAreaSet[n_area] = theArea;
}


/* *****************************************************************************
get EM fraction bin
***************************************************************************** */
int CaloHadDMCoeff::getBinFrac(const int n_area, const float & cls_emfrac) const
{
   const HadDMArea *dmArea = &m_DMAreaSet[n_area];
   if(cls_emfrac < dmArea->m_MinFrac || cls_emfrac >= dmArea->m_MaxFrac) return -1;
   int ifrac;
   int nfrac = dmArea->m_FracBins.size();
   for (ifrac=0; ifrac<nfrac; ifrac++) {
      if( cls_emfrac < dmArea->m_FracBins[ifrac] ) break;
      if( cls_emfrac >= dmArea->m_FracBins[ifrac] && (ifrac == nfrac-1 || cls_emfrac < dmArea->m_FracBins[ifrac+1]) ) break;
   }
   return ifrac;
}


/* *****************************************************************************
To get energy bin
***************************************************************************** */
int CaloHadDMCoeff::getBinEner(const int n_area, const float & cls_ener) const
{
   const HadDMArea *dmArea = &m_DMAreaSet[n_area];
   if(cls_ener < dmArea->m_MinEner || cls_ener >= dmArea->m_MaxEner) return (-1);
   int iener;
   int nener = (int)dmArea->m_EnerBins.size();
   for (iener=0; iener<nener; iener++) {
      if( cls_ener < dmArea->m_EnerBins[iener] ) break;
      if( cls_ener >= dmArea->m_EnerBins[iener] && (iener == nener-1 || cls_ener < dmArea->m_EnerBins[iener+1]) ) break;
   }
   return iener;
}


/* *****************************************************************************
get eta bin
***************************************************************************** */
int CaloHadDMCoeff::getBinEta(const int n_area, const float & cls_eta) const
{
   const HadDMArea *dmArea = &m_DMAreaSet[n_area];
   if(cls_eta < dmArea->m_MinEta || cls_eta >= dmArea->m_MaxEta) return (-1);
   int neta = (int)dmArea->m_EtaBins.size();
   int ieta = (int)((cls_eta - dmArea->m_MinEta)/(dmArea->m_MaxEta - dmArea->m_MinEta)*neta);
   return ieta;
}


/* *****************************************************************************
get common bin
***************************************************************************** */
int CaloHadDMCoeff::getBin(const int & n_dm_area, const float & cls_emfrac, const float & cls_ener, const float & cls_eta) const
{
   if(n_dm_area < 0 || n_dm_area >= (int)m_DMAreaSet.size()) {
      std::cout << " WARNING CaloHadDMCoeff::getBin -> wrong n_area: " << n_dm_area << std::endl;
      return (-1);
   }
   int ifrac = getBinFrac(n_dm_area, cls_emfrac);
   int iener = getBinEner(n_dm_area, cls_ener);
   int ieta = getBinEta(n_dm_area, cls_eta);
   int iBin = getBin(n_dm_area, ifrac, iener, ieta);
   return iBin;
}


int CaloHadDMCoeff::getBin(const int & n_dm_area, const int & ifrac, const int & iener, const int & ieta) const
{
   const HadDMArea *dmArea = &m_DMAreaSet[n_dm_area];
   int iBin = -1;
   if(ifrac!=-1 && iener !=-1 && ieta!=-1) { 
      iBin = ieta + iener*(int)dmArea->m_EtaBins.size() + ifrac*(int)dmArea->m_EnerBins.size()*(int)dmArea->m_EtaBins.size() + dmArea->m_offset;
   }
   return iBin;
}


void CaloHadDMCoeff::PrintData(FILE *fout) const
{

   const char *comments={
   "# Coeffs for Dead Material corrections in CaloClusterLocalCalib algorithm.\n"
   "# Author: Guennadi Pospelov <pospelov@mppmu.mpg.de>\n"
   "#\n"
   "#\n"
   "# dm_id is_ON dm_title\n"
   "# nFrac FracMin FracMax\n"
   "# nEner EnerMin EnerMax\n"
   "# nEta  EtaMin  EtaMax\n"
   "# Npars\n"
   "# indx   ifrac iener ieta   frac ener eta   pars[]\n"
   "#\n\n"};
   fprintf(fout,"%s",comments);
   int indx = 0;
   for(int i_zone=0; i_zone < (int)m_DMAreaSet.size(); i_zone++){
      const HadDMArea *area = &m_DMAreaSet[i_zone];
      fprintf(fout," %d %d %s\n",area->m_indx, area->m_is_on, area->m_title.data());
      fprintf(fout,"%2d %6.3f %6.3f\n", (int)area->m_FracBins.size(), area->m_MinFrac, area->m_MaxFrac);
      fprintf(fout,"%2d %6.3g %6.3g\n", (int)area->m_EnerBins.size(), area->m_MinEner, area->m_MaxEner);
      fprintf(fout,"%2d %6.3f %6.3f\n", (int)area->m_EtaBins.size(), area->m_MinEta, area->m_MaxEta);
      fprintf(fout,"%2d\n", area->m_nPars);
      for(int i_frac=0; i_frac<(int)area->m_FracBins.size(); i_frac++){
         for(int i_ener=0; i_ener<(int)area->m_EnerBins.size(); i_ener++){
            for(int i_eta=0; i_eta<(int)area->m_EtaBins.size(); i_eta++){
               float cls_emfrac = area->m_FracBins[i_frac];
               float cls_ener = area->m_EnerBins[i_ener];
               float cls_eta = area->m_EtaBins[i_eta]+0.01;
               const HadDMCoeff *vv = getHadDMCoeff(i_zone, cls_emfrac, cls_ener, cls_eta);
               fprintf(fout,"%4d   %2d %2d %2d   %6.3f %6.3g %6.3f    ",
               indx, i_frac,i_ener, i_eta, area->m_FracBins[i_frac], area->m_EnerBins[i_ener], area->m_EtaBins[i_eta]);
               for(int i_par=0; i_par<area->m_nPars; i_par++){
                  if(area->m_indx!=10) {
                     fprintf(fout,"%10.4f ",(*vv)[i_par]);
                  } else{
                     fprintf(fout,"%10.7f ",(*vv)[i_par]);                  
                  }
               } // i_par
               indx++;
               fprintf(fout,"\n");
            } // i_eta
         } // i_ener
      } // i_frac
      fprintf(fout,"\n");
   }
}
