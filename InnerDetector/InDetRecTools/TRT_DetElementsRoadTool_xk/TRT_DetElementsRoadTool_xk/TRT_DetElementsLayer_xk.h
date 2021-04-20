/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
      TRT_DetElementsLayer_xk(const TRT_DetElementsLayer_xk&) = default;
      ~TRT_DetElementsLayer_xk();
      TRT_DetElementsLayer_xk& operator  = (const TRT_DetElementsLayer_xk&) = default;
      TRT_DetElementsLayer_xk& operator  = (TRT_DetElementsLayer_xk&&) = default;

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const float& r  () const {return this->m_r  ;}
      const float& dr () const {return this->m_dr ;}
      const float& z  () const {return this->m_z  ;}
      const float& dz () const {return this->m_dz ;}
      const float& dfe() const {return this->m_dfe;}

      void  set(double,double,double,double,double,double,double);
      void  add(TRT_DetElementLink_xk&&);
      /** Get number of links to detector elements.
        */
      int   nElements() const;
       /**
        * @param ?
        * @param ?
        * @param will be filled with  pairs of detector elements and way(?)
        * @param array with size matching number of elements, which tells which elements are already used.
        */
      void getBarrelDetElementsATL
	(const float*,
         const float*,
         std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> >&,
         std::vector<InDet::TRT_DetElementLink_xk::Used_t> &used) const;
       /**
        * @param ?
        * @param ?
        * @param will be filled with  pairs of detector elements and way(?)
        * @param array with size matching number of elements, which tells which elements are already used.
        */
      void getBarrelDetElementsCTB
        (const float*,
         const float*,
         std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> >&,
         std::vector<InDet::TRT_DetElementLink_xk::Used_t> &used) const;
       /**
        * @param ?
        * @param ?
        * @param will be filled with  pairs of detector elements and way(?)
        * @param array with size matching number of elements, which tells which elements are already used.
        */
      void getEndcapDetElements
	(const float*,
         const float*,
         std::vector<std::pair<const InDet::TRT_DetElementLink_xk*,float> >&,
         std::vector<InDet::TRT_DetElementLink_xk::Used_t> &used) const;


      void reserve(std::size_t n_elements) {
         m_elements.reserve(n_elements);
      }
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

  inline TRT_DetElementsLayer_xk::~TRT_DetElementsLayer_xk() {}

  inline void  TRT_DetElementsLayer_xk::add(TRT_DetElementLink_xk&& link)
    {
       m_elements.push_back(std::move(link));
    }

  inline int TRT_DetElementsLayer_xk::nElements() const
    {
      return m_elements.size();
    }  


} // end of name space

#endif // TRT_DetElementsLayer_xk_H

