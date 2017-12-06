/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDITIONS_CALOHADDMCOEFF_H
#define CALOCONDITIONS_CALOHADDMCOEFF_H

/*!
\file  CaloHadDMCoeff.h
\author   Guennadi.Pospelov@cern.ch
\date     March 2006

\class DeadMaterialCorrectionTool
\brief Local cluster correction tool for Dead Material correction 
       in hadronic clusters.
*/

#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>
#include <string>
#include <cstdio>

class CaloHadDMCoeff {
 public:
   struct HadDMArea {
      std::string m_title;
      int m_indx;
      int m_is_on;
      float m_MinFrac;
      float m_MaxFrac;
      std::vector<float> m_FracBins;
      float m_MinEner;
      float m_MaxEner;
      std::vector<float> m_EnerBins;
      float m_MinEta;
      float m_MaxEta;
      std::vector<float> m_EtaBins;
      int m_nPars;
      int m_offset;
      HadDMArea() : m_indx(0), m_is_on(0), m_MinFrac(0.0), m_MaxFrac(0.0), 
                    m_MinEner(0.0), m_MaxEner(0.0), m_MinEta(0.0), m_MaxEta(0.0), 
                    m_nPars(0), m_offset(0) {};
   };
   typedef std::vector<float> HadDMCoeff;

   /// Default Constructor
   CaloHadDMCoeff();
   CaloHadDMCoeff(const CaloHadDMCoeff &other);
   CaloHadDMCoeff& operator= (const CaloHadDMCoeff& other);

   const HadDMCoeff * getHadDMCoeff(const int & dm_area, const float & cls_emfrac, const float & cls_ener, const float & cls_eta) const;
   inline const HadDMCoeff * getHadDMCoeff(const int & iBin) const
   {
      if ( iBin > -1 && iBin < (int)m_DMCoeffSet.size() ) {
         return (& m_DMCoeffSet[iBin]);
      }else{
         return 0;
      }
   }
   
   const HadDMArea * getHadDMArea(int n_area) const;
   void addHadDMCoeff(const HadDMCoeff & theData);
   void setHadDMCoeff(const int iBin, const HadDMCoeff & theData);
   void addHadDMArea(const HadDMArea & theArea);
   void setHadDMArea(const int n_area, const HadDMArea & theArea);
   void PrintData(FILE *fout = stdout) const;

   inline int getSizeDMAreaSet() const {return (int)m_DMAreaSet.size();}
   inline int getSizeDMCoeffSet() const {return (int)m_DMCoeffSet.size();}
   int getBinFrac(const int dm_area, const float & cls_frac) const;
   int getBinEner(const int dm_area, const float & cls_ener) const;
   int getBinEta(const int dm_area, const float & cls_eta) const;
   int getBin(const int & n_dm_area, const float & cls_frac, const float & cls_ener, const float & cls_eta) const;
   int getBin(const int & n_dm_area, const int & ifrac, const int & iener, const int & ieta) const;

 private:
   std::vector<HadDMCoeff> m_DMCoeffSet;
   std::vector<HadDMArea> m_DMAreaSet;
};
//Class Identifier obtained by
//'clid CaloHadDMCoeff'
CLASS_DEF(CaloHadDMCoeff,227697804,1)

#endif
