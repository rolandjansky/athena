/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  enum class VKContraintType { Mass, Phi, Theta, Point, Plane  };
  class VKConstraintBase
  {
    public:
      VKConstraintBase(const int, int, VKContraintType, VKVertex*);
      virtual ~VKConstraintBase();
    public:
      const VKVertex * getOriginVertex() const { return m_originVertex;}
      VKContraintType getType() const { return m_type; }
      virtual VKConstraintBase* clone() const = 0;
      virtual void applyConstraint() = 0;
      int NCDim;                                // constraint dimension
      int NTrk;                                 // number of tracks
      std::vector<double> aa;                     // Constraint values
      std::vector< std::vector< Vect3DF > > f0t;  // Constraint momentum derivatives 
      std::vector< Vect3DF > h0t;	          // Constraint space derivatives
    protected:
      VKVertex * m_originVertex;
      const VKContraintType m_type;
   };
//
//  Mass constraint
//
   class VKMassConstraint : public VKConstraintBase
   {
      public:
        VKMassConstraint(int,double, VKVertex*); 
        VKMassConstraint(int,double, std::vector<int>, VKVertex*); 
        ~VKMassConstraint(); 
        friend std::ostream& operator<<( std::ostream& out, const VKMassConstraint& );
        virtual VKConstraintBase* clone() const override;
      public:
        void setTargetMass(double M) { m_targetMass=M; };
        virtual void applyConstraint() override;
        double getTargetMass() const { return m_targetMass; };
        const std::vector<int>& getUsedParticles() const { return m_usedParticles; };

      private:
	std::vector<int> m_usedParticles;
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
        virtual VKConstraintBase* clone() const override;
        virtual void applyConstraint() override;
        friend std::ostream& operator<<( std::ostream& out, const VKPhiConstraint& );
   };
   class VKThetaConstraint : public VKConstraintBase
   {
      public:
        VKThetaConstraint(int,VKVertex*); 
        ~VKThetaConstraint(); 
        virtual VKConstraintBase* clone() const override;
        virtual void applyConstraint() override;
        friend std::ostream& operator<<( std::ostream& out, const VKThetaConstraint& );
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
        bool onlyZ() const {return m_onlyZ; };
        void setTargetVertex(double VRT[3]){ m_targetVertex[0]=VRT[0]; m_targetVertex[1]=VRT[1]; m_targetVertex[2]=VRT[2]; };
        const double * getTargetVertex() const { return m_targetVertex;};
        virtual VKConstraintBase* clone() const override;
        virtual void applyConstraint() override;
      private:
        bool m_onlyZ; 
        double m_targetVertex[3]{};   //Target vertex is in global reference system
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
        double getA() const { return m_A;}
        double getB() const { return m_B;}
        double getC() const { return m_C;}
        double getD() const { return m_D;}
        virtual void applyConstraint() override;
        virtual VKConstraintBase* clone() const override;
      private:
        double m_A,m_B,m_C,m_D;
   };

} /* End namespace */
#endif
