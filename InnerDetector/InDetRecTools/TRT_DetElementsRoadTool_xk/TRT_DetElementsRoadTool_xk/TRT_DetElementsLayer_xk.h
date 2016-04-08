/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DetElementsLayer_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for detector elements layer
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DetElementsLayer_xk_H
#define TRT_DetElementsLayer_xk_H

#include <utility>
#include <vector>
#include <list>

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementLink_xk.h"

namespace InDet{


  class TRT_DetElementsLayer_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      TRT_DetElementsLayer_xk();
      TRT_DetElementsLayer_xk(double,double,double,double,double);
      TRT_DetElementsLayer_xk(const TRT_DetElementsLayer_xk&);
      ~TRT_DetElementsLayer_xk();
      TRT_DetElementsLayer_xk& operator  = (const TRT_DetElementsLayer_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const float& r  () const {return this->m_r  ;}
      const float& dr () const {return this->m_dr ;}
      const float& z  () const {return this->m_z  ;}
      const float& dz () const {return this->m_dz ;}
      const float& dfe() const {return this->m_dfe;}

      void  set(double,double,double,double,double,double,double);
      void  add(const TRT_DetElementLink_xk&);
      int   nElements() const;
      void getBarrelDetElementsATL
	(float*,float*,std::list<InDet::TRT_DetElementLink_xk*>&);
      void getBarrelDetElementsCTB
	(float*,float*,std::list<InDet::TRT_DetElementLink_xk*>&);
      void getEndcapDetElements
	(float*,float*,std::list<InDet::TRT_DetElementLink_xk*>&);

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      float  m_z   ;                                // Z coordinate
      float  m_dz  ;                                // Z half width
      float  m_r   ;                                // Radius
      float  m_dr  ;                                // Radius half width
      float  m_dfe ;                                // max azimuthal width 
      float  m_f0  ;                                // 
      float  m_sfi ;                                //
      float  m_wf  ;                                // Width in phi direction
      float  m_wz  ;                                // Width in r,z direction
      std::vector<TRT_DetElementLink_xk> m_elements; // detector elements
 
      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_DetElementsLayer_xk::TRT_DetElementsLayer_xk()
  {
    m_z   = 0.;
    m_dz  = 0.;
    m_r   = 0.;
    m_dr  = 0.;
    m_dfe = 0.;
    m_f0  = 0.;
    m_sfi = 0.;
    m_wf  = 0.;
    m_wz  = 0.;
  } 

  inline TRT_DetElementsLayer_xk::TRT_DetElementsLayer_xk
    (double r,double dr,double z,double dz,double df)
    {
      m_r   = float(r );
      m_dr  = float(dr);
      m_z   = float(z );
      m_dz  = float(dz);
      m_dfe = float(df);
      m_wf  = 0.       ;
      m_wz  = 0.       ;
    } 


  inline TRT_DetElementsLayer_xk::TRT_DetElementsLayer_xk(const TRT_DetElementsLayer_xk& L)
    {
      *this = L; 
    }
  
  inline TRT_DetElementsLayer_xk& TRT_DetElementsLayer_xk::operator = 
    (const TRT_DetElementsLayer_xk& L) 
    {
      if(&L!=this) {
	m_z         = L.m_z       ;
	m_dz        = L.m_dz      ;
	m_r         = L.m_r       ;
	m_dr        = L.m_dr      ;
	m_dfe       = L.m_dfe     ;
	m_f0        = L.m_f0      ;
	m_sfi       = L.m_sfi     ;
	m_wf        = L.m_wf      ;
	m_wz        = L.m_wz      ;
	m_elements  = L.m_elements;
      }
      return(*this);
    }

  inline TRT_DetElementsLayer_xk::~TRT_DetElementsLayer_xk() {}

  inline void  TRT_DetElementsLayer_xk::add(const TRT_DetElementLink_xk& link)
    {
      m_elements.push_back(link);
    }

  inline int TRT_DetElementsLayer_xk::nElements() const
    {
      return m_elements.size();
    }  


} // end of name space

#endif // TRT_DetElementsLayer_xk_H

