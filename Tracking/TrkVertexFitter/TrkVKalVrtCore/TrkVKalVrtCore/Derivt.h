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
  

// Base class for any constraint
// Contains derivatines needed for application
//
  
  class VKVertex;
   
  class VKConstraintBase
  {
    public:
      VKConstraintBase(const int, int);
      virtual ~VKConstraintBase();
    public:
      int NCDim;                                // constraint dimension
      int NTrk;                                 // number of tracks
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
        VKMassConstraint(int,double, const std::vector<int>&, VKVertex*); 
        ~VKMassConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKMassConstraint& );
   
      public:
        VKVertex * getOriginVertex() const { return m_originVertex;}
        void setTargetMass(double M) { m_targetMass=M; };
        double getTargetMass() const { return m_targetMass; };
        std::vector<int> getUsedParticles() const { return m_usedParticles; };

      private:
	std::vector<int> m_usedParticles;
        VKVertex * m_originVertex;
        double m_targetMass;
 
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
        VKPointConstraint(int,double[3], VKVertex*, bool ); 
        ~VKPointConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKPointConstraint& );
        VKVertex * getOriginVertex() const { return m_originVertex; };
        bool onlyZ() const {return m_onlyZ; };
        void setTargetVertex(double VRT[3]){ m_targetVertex[0]=VRT[0]; m_targetVertex[1]=VRT[1]; m_targetVertex[2]=VRT[2]; };
        const double * getTargetVertex() const { return m_targetVertex;};

      private:
        bool m_onlyZ; 
        double m_targetVertex[3];   //Target vertex is in global reference system
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
