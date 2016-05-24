/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_Derivt_H
#define _TrkVKalVrtCore_Derivt_H


#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalUtils.h"
#include <vector>
#include <iostream>

namespace Trk {
  
  struct DerivT{
    long int ncnst, ndummy;
    double aa[8];
    double f0t[3*NTrkM*8];	/* was [3][300][8] */
    double h0t[24];	        /* was [3][8] */
  };



// Base class for any constraint
// Contains derivatines needed for application
//
  
  class VKVertex;
   
  class VKConstraintBase
  {
    public:
      VKConstraintBase( int, int);
      virtual ~VKConstraintBase();
    public:
      int NCDim;                                  // constraint dimension
      std::vector<double> aa;                     // Constraint values
      std::vector< std::vector< Vect3DF > > f0t;  // Constraint momentum derivatives 
      std::vector< Vect3DF > h0t;	          // Constraint space derivatives
   };
//
//  Mass constraint
//
   class VKMassConstraint : public VKConstraintBase
   {
      public:
        VKMassConstraint(int,double, VKVertex*); 
        VKMassConstraint(int,double,std::vector<int>, VKVertex*); 
        ~VKMassConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKMassConstraint& );
   
      public:
        double targetMass;
	std::vector<int> usedParticles;

     private:
       VKVertex * m_originVertex;
 
     public:
       VKVertex * getOriginVertex() const { return m_originVertex;}
   };
//
//  Angular constraints
//
   class VKPhiConstraint : public VKConstraintBase
   {
      public:
        VKPhiConstraint(int,VKVertex*); 
        ~VKPhiConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKPhiConstraint& );
        VKVertex * getOriginVertex() const { return m_originVertex;}
   
      private:
        VKVertex * m_originVertex;
   };
   class VKThetaConstraint : public VKConstraintBase
   {
      public:
        VKThetaConstraint(int,VKVertex*); 
        ~VKThetaConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKThetaConstraint& );
        VKVertex * getOriginVertex() const { return m_originVertex;}
  
      private:
        VKVertex * m_originVertex;
   };

//
//  Pointing constraints
//
   class VKPointConstraint : public VKConstraintBase
   {
      public:
        VKPointConstraint(int,double [], VKVertex*); 
        ~VKPointConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKPointConstraint& );
        VKVertex * getOriginVertex() const { return m_originVertex;}

      public:
         double targetVertex[3];   //Target vertex is in global reference system
         bool onlyZ; 

      private:
        VKVertex * m_originVertex;
   };
//
//  Vertex in plane constraint
//
   class VKPlaneConstraint : public VKConstraintBase
   {
      public:
        VKPlaneConstraint(int, double, double, double, double, VKVertex*); 
        ~VKPlaneConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKPlaneConstraint& );
        VKVertex * getOriginVertex() const { return m_originVertex;}
        double getA() const { return m_A;}
        double getB() const { return m_B;}
        double getC() const { return m_C;}
        double getD() const { return m_D;}
   
      private:
        double m_A,m_B,m_C,m_D;
        VKVertex * m_originVertex;
   };

} /* End namespace */
#endif
