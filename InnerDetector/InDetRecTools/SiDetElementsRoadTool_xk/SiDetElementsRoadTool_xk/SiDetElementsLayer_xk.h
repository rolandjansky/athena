/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsLayer_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for detector elements layer
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsLayer_xk_H
#define SiDetElementsLayer_xk_H

#include <utility>
#include <vector>
#include <list>
#include "SiDetElementsRoadTool_xk/SiDetElementLink_xk.h"

namespace InDet{


  class SiDetElementsLayer_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiDetElementsLayer_xk();
      SiDetElementsLayer_xk(double,double,double,double,double);

      SiDetElementsLayer_xk(const SiDetElementsLayer_xk&) = default;
      SiDetElementsLayer_xk(SiDetElementsLayer_xk&&) = default;
      ~SiDetElementsLayer_xk() = default;
      SiDetElementsLayer_xk& operator  = (const SiDetElementsLayer_xk&) = default;
      SiDetElementsLayer_xk& operator  = (SiDetElementsLayer_xk&&) = default;

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const float                    & r       () const {return m_r       ;}
      const float                    & dr      () const {return m_dr      ;}
      const float                    & z       () const {return m_z       ;}
      const float                    & dz      () const {return m_dz      ;}
      const float                    & dfe     () const {return m_dfe     ;}
      std::vector<SiDetElementLink_xk>& elements()      {return m_elements;}
      const std::vector<SiDetElementLink_xk>& elements() const {return m_elements;}

      void  set(double,double,double,double,double);
      void  add(const SiDetElementLink_xk&);
      int   nElements() const;
      void getBarrelDetElements
        (float*,
         float*,
         std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
         std::vector<InDet::SiDetElementLink_xk::UsedFlag>   &used) const;
      void getEndcapDetElements
	(float*,
         float*,
         std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
         std::vector<InDet::SiDetElementLink_xk::UsedFlag>   &used) const;

      void sortDetectorElements();

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      float  m_z   ;                               // Z coordinate
      float  m_dz  ;                               // Z half width
      float  m_r   ;                               // Radius
      float  m_dr  ;                               // Radius half width
      float  m_dfe ;                               // max azimuthal width of de
      std::vector<SiDetElementLink_xk> m_elements; // detector elements
 
      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////
      void getDetElements(float*,
                          float*,
                          float,
                          float,
                          std::vector<InDet::SiDetElementLink_xk::ElementWay> &lDE,
                          std::vector<InDet::SiDetElementLink_xk::UsedFlag>   &used) const;
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiDetElementsLayer_xk::SiDetElementsLayer_xk()
  {
    m_z   = 0.;
    m_dz  = 0.;
    m_r   = 0.;
    m_dr  = 0.;
    m_dfe = 0.;
  } 

  inline SiDetElementsLayer_xk::SiDetElementsLayer_xk
    (double r,double dr,double z,double dz,double df)
    {
      m_r  = float(r );
      m_dr = float(dr);
      m_z  = float(z );
      m_dz = float(dz);
      m_dfe= float(df);
    } 

  inline void SiDetElementsLayer_xk::set
    (double r,double dr,double z,double dz,double df)
    {
      m_r  = float(r );
      m_dr = float(dr);
      m_z  = float(z );
      m_dz = float(dz);
      m_dfe= float(df);
    } 

  inline void  SiDetElementsLayer_xk::add(const SiDetElementLink_xk& link)
    {
      m_elements.push_back(link);
    }

  inline int SiDetElementsLayer_xk::nElements() const
    {
      return m_elements.size();
    }  


} // end of name space

#endif // SiDetElementsLayer_xk_H

