/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDITIONS_CALOHADDMCOEFF2_H
#define CALOCONDITIONS_CALOHADDMCOEFF2_H

/**
 * @class CaloHadDMCoeff2
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 21-November-2007
 * @brief Dead material correction coefficients
 *
 * Holds dead material correction coefficients for using in Local Hadronic 
 * Correction Procedure (DeadMaterialCorrrection tool) */


#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>
#include <string>
#include <fstream>

class CaloHadDMCoeff2 {
 public:
   enum key_dim_type { kDIM_FLAT, kDIM_HAND };
   enum key_area_type { kAREA_PROF, kAREA_LOOKUP, kAREA_SMPW};
   struct HadDMDimension {
     std::string m_title;
     int m_type;
     int m_nbins;
     float m_vMin;
     float m_vMax;
     float m_bin_size;
     int xloc;
     std::vector<float > m_vBins;

     HadDMDimension() : m_type(0), m_nbins(0), m_vMin(0.0), m_vMax(0.0), m_bin_size(0.0), xloc(0) {};
   };
   struct HadDMArea {
     std::string m_title;
     int m_indx;
     int m_is_on;
     int m_type;
     int m_nPars;
     int m_offset;
     int m_length;
     HadDMDimension m_dimFrac;
     HadDMDimension m_dimEner;
     HadDMDimension m_dimLambda;
     HadDMDimension m_dimEta;

     HadDMArea() : m_indx(0), m_is_on(0), m_type(0), m_nPars(0), m_offset(0), m_length(0) {};
   };
   typedef std::vector<float> HadDMCoeff;

   // Default Constructor
   CaloHadDMCoeff2();
   CaloHadDMCoeff2(const CaloHadDMCoeff2 &other);
   CaloHadDMCoeff2& operator= (const CaloHadDMCoeff2& other);

   // methods to deal with DM areas
   inline int getSizeDMAreaSet() const { return (int)m_DMAreaSet.size(); }
   void addHadDMArea(const HadDMArea & theArea);
   void setHadDMArea(const int n_area, const HadDMArea & theArea);
   const HadDMArea * getHadDMArea(int n_area) const;

   // methods to deal with DM coefficients
   inline int getSizeDMCoeffSet() const { return (int)m_DMCoeffSet.size(); }
   void addHadDMCoeff(const HadDMCoeff & theData);
   void setHadDMCoeff(const int iBin, const HadDMCoeff & theData);
   const HadDMCoeff * getHadDMCoeff(const int & iBin) const;
   const HadDMCoeff * getHadDMCoeff(const int & n_area, const float & cls_emfrac, const float & cls_ener, const float & cls_lambda, const float & cls_eta) const;

   // methods to get DM coefficients bin number
   int getBinFrac(const int dm_area, const float & cls_frac) const;
   int getBinEner(const int dm_area, const float & cls_ener) const;
   int getBinLambda(const int dm_area, const float & cls_lambda) const;
   int getBinEta(const int dm_area, const float & cls_eta) const;
   int getBin(const int n_area, const float &cls_em_frac, const float &cls_energy, const float &cls_lambda, const float &cls_eta) const;

   // Accessories
   void PrintData(std::ostream &ff) const;
   int bin2indexes(const int iBin, std::vector<int> &v_dim_indx) const;

 private:
   std::vector<HadDMCoeff> m_DMCoeffSet;
   std::vector<HadDMArea> m_DMAreaSet;
   std::vector<int > m_area_offsets;
};
//Class Identifier obtained by
//'clid CaloHadDMCoeff2'
CLASS_DEF(CaloHadDMCoeff2, 249025083,1)

#endif
