/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ComparisonFunction, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_COMPARISONFUNCTION_H
#define TRKEVENTUTILS_COMPARISONFUNCTION_H

//STL
#include <ext/algorithm>
#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

  /** 
    @class ComparisonFunction 
    
    this functor inherits from std::binary_function to find the closest objects:
    
    - a point
    - a line
    - a cylinder of radius R
    
    @author Andreas.Salzburger@cern.ch
     
    */

   template <class T>  class ComparisonFunction : 
           public std::binary_function<const T*, const T*, bool> {
     public:        
        /** Default Constructor */
        ComparisonFunction():
          m_point(0),
          m_line(0),
          m_radius(0.)
        {}
        
        /** Constructor for pointlike Search */        
        ComparisonFunction(const Amg::Vector3D& sp):
          m_point(new Amg::Vector3D(sp)),
          m_line(0),
          m_radius(0.)
        {}  

        /** Constructor for search from line */
        ComparisonFunction(const Amg::Vector3D& sp, const Amg::Vector3D& dir):
          m_point(new Amg::Vector3D(sp)),
          m_line(new Amg::Vector3D(dir)),
          m_radius(0.)
        {} 
        
        ComparisonFunction(double cradius):
          m_point(0),
          m_line(0),
          m_radius(fabs(cradius))
        {}

        ComparisonFunction(const ComparisonFunction& cpf):
          m_point(cpf.m_point ? new Amg::Vector3D(*cpf.m_point) : 0),
          m_line(cpf.m_line  ? new Amg::Vector3D(*cpf.m_line) : 0),
          m_radius(cpf.m_radius)
        {}

        virtual ~ComparisonFunction(){
          delete m_point;
          delete m_line;
        }

        ComparisonFunction &operator=(const ComparisonFunction& cpf) {
          if (this != &cpf) {
            delete m_point;
            m_point=(cpf.m_point ? new Amg::Vector3D(*cpf.m_point) : 0);
            delete m_line;
            m_line=(cpf.m_line  ? new Amg::Vector3D(*cpf.m_line) : 0);
            m_radius=(cpf.m_radius);
          }
          return *this;
        }
                       
        bool operator() (const T* one, const T* two) const {                 
                if (!m_point && !m_line){
                  return ( fabs(one->position().perp() - m_radius) < fabs(two->position().perp() - m_radius) );
                }
                if (!m_line){
                 return ( (one->position() - (*m_point)).mag() < (two->position() - (*m_point)).mag());
                }
                Amg::Vector3D distPosOne((one->position())-(*m_point)); 
		double lmag2 = m_line->mag(); lmag2 *= lmag2;
		double dp1_mag2 = distPosOne.mag(); dp1_mag2 *= dp1_mag2;
		double pl1_2 = m_line->dot(distPosOne); pl1_2 *= pl1_2;
                double distOne = (lmag2*dp1_mag2 - pl1_2)/dp1_mag2; 

                Amg::Vector3D distPosTwo((two->position())-(*m_point)); 
		double dp2_mag2 = distPosTwo.mag(); dp1_mag2 *= dp1_mag2;
		double pl2_2 = m_line->dot(distPosTwo); pl1_2 *= pl1_2;
                double distTwo = (lmag2*dp2_mag2 - pl2_2)/dp2_mag2; 
                
                return ( fabs(distOne) < fabs(distTwo) );
         }
         
      private:
        Amg::Vector3D* m_point;
        Amg::Vector3D* m_line;
        double         m_radius;
                    
  };
   
} // end of namespace

#endif

