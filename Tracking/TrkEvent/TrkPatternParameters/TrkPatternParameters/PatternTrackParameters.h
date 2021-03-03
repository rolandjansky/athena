/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class PatternTrackParameters
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for pattern track parameters
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 09/08/2006 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef PatternTrackParameters_H
#define PatternTrackParameters_H

#include "TrkParametersBase/ParametersBase.h"
#include "TrkParametersBase/Charged.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkSurfaces/Surface.h"
#include "TrkPatternParameters/NoiseOnSurface.h"
#include "CxxUtils/CachedValue.h"

class MsgStream;

namespace Trk {

  class PlaneSurface       ;  
  class StraightLineSurface;
  class DiscSurface        ;
  class CylinderSurface    ;
  class PerigeeSurface     ;
  class ConeSurface        ;

  class PatternTrackParameters : public ParametersBase<5, Trk::Charged>
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      PatternTrackParameters();
      PatternTrackParameters(const PatternTrackParameters&);
      ~PatternTrackParameters();
      PatternTrackParameters& operator  = (const PatternTrackParameters&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      virtual
      const Surface&   associatedSurface ()     const override {return   *m_surface;}
      bool             iscovariance      ()     const {return   m_covariance != nullptr ;}
      double           sinPhi            ()     const;
      double           cosPhi            ()     const;
      double           sinTheta          ()     const;
      double           cosTheta          ()     const;
      double           cotTheta          ()     const;
      void             changeDirection   ()          ;

      virtual Amg::Vector3D position() const override final;
      virtual Amg::Vector3D momentum() const override final;
      virtual double charge() const override final;
      virtual bool hasSurface() const override final;
      virtual Amg::RotationMatrix3D measurementFrame() const override final;
      virtual PatternTrackParameters * clone() const override final;
      virtual ParametersType type() const override final;
      virtual int surfaceType() const override final;
      virtual void updateParametersHelper(const AmgVector(5) &) override final;

      ///////////////////////////////////////////////////////////////////
      // Methods set
      ///////////////////////////////////////////////////////////////////
      
      void setParameters              (const Surface*,const double*              );
      void setCovariance              (                             const double*);
      void setParametersWithCovariance(const Surface*,const double*,const double*);
      void setParametersWithCovariance(const Surface*,const double*,const AmgSymMatrix(5)&);

      ///////////////////////////////////////////////////////////////////
      // Convertors
      ///////////////////////////////////////////////////////////////////

      std::unique_ptr<ParametersBase<5, Trk::Charged>> convert(bool) const;
      bool production(const ParametersBase<5, Trk::Charged>*);

      ///////////////////////////////////////////////////////////////////
      // Initiation
      ///////////////////////////////////////////////////////////////////

      void diagonalization (double);
      bool initiate
	(PatternTrackParameters&, const Amg::Vector2D&,const Amg::MatrixX&);

      ///////////////////////////////////////////////////////////////////
      // Add or remove noise
      ///////////////////////////////////////////////////////////////////

      void addNoise   (const NoiseOnSurface&,PropDirection); 
      void removeNoise(const NoiseOnSurface&,PropDirection); 

      ///////////////////////////////////////////////////////////////////
      // Covariance matrix production using jacobian CovNEW = J*CovOLD*Jt
      ///////////////////////////////////////////////////////////////////
      
      static AmgSymMatrix(5) newCovarianceMatrix(const AmgSymMatrix(5) &, const double *);

      ///////////////////////////////////////////////////////////////////
      // Print
      ///////////////////////////////////////////////////////////////////

      virtual std::ostream& dump(std::ostream&) const override;
      virtual MsgStream&    dump(MsgStream&   ) const override;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data
      ///////////////////////////////////////////////////////////////////

      SurfaceUniquePtrT<const Surface> m_surface;

      ///////////////////////////////////////////////////////////////////
      // Comments
      // m_surface is pointer to associated surface
      // m_parameters[ 0] - 1 local coordinate
      // m_parameters[ 1] - 2 local coordinate
      // m_parameters[ 2] - Azimuthal angle
      // m_parameters[ 3] - Polar     angle
      // m_parameters[ 4] - charge/Momentum
      // m_covariance is the covariance matrix
      ///////////////////////////////////////////////////////////////////


      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      
      Amg::Vector3D localToGlobal(const PlaneSurface       *) const;
      Amg::Vector3D localToGlobal(const StraightLineSurface*) const;
      Amg::Vector3D localToGlobal(const DiscSurface        *) const;
      Amg::Vector3D localToGlobal(const CylinderSurface    *) const;
      Amg::Vector3D localToGlobal(const PerigeeSurface     *) const;
      Amg::Vector3D localToGlobal(const ConeSurface        *) const;

      Amg::Vector3D calculatePosition(void) const;
      Amg::Vector3D calculateMomentum(void) const;
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Overload operator
  /////////////////////////////////////////////////////////////////////////////////

  std::ostream& operator << (std::ostream&,const PatternTrackParameters&); 
  MsgStream& operator    << (MsgStream&, const PatternTrackParameters& );
 
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////


  inline PatternTrackParameters::PatternTrackParameters():
    ParametersBase<5, Trk::Charged>()
    {
      m_parameters.setZero();
    }

  inline PatternTrackParameters::PatternTrackParameters(const PatternTrackParameters& P):
    PatternTrackParameters()
    {
      *this = P;
    }
  
  inline PatternTrackParameters& PatternTrackParameters::operator = 
    (const PatternTrackParameters& P) 
    {
      if (&P != this){
        if (P.m_surface != nullptr) {
          m_surface.reset(P.m_surface->isFree() ? P.m_surface->clone() : P.m_surface.get());
        } else {
          m_surface.reset(nullptr);
        }

        m_parameters     = P.m_parameters    ;

        if (P.m_covariance != nullptr) {
          if (m_covariance == nullptr) {
            m_covariance = std::make_unique<AmgSymMatrix(5)>(*P.m_covariance);
          } else {
            *m_covariance = *P.m_covariance;
          }
        }
      }

      return (*this);
    }

  inline PatternTrackParameters::~PatternTrackParameters() {}

  ///////////////////////////////////////////////////////////////////
  // Set parameters
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::setParameters
    (const Surface* s,const double* p)
    {
      m_surface.reset(s && s->isFree() ? s->clone() : s);
      m_parameters[ 0] = p[ 0];
      m_parameters[ 1] = p[ 1];
      m_parameters[ 2] = p[ 2];
      m_parameters[ 3] = p[ 3];
      m_parameters[ 4] = p[ 4];
      m_covariance.reset(nullptr);
    }

  ///////////////////////////////////////////////////////////////////
  // Set covariance
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::setCovariance
    (const double* c)
    {
      if (m_covariance == nullptr) {
        m_covariance = std::make_unique<AmgSymMatrix(5)>();
      }

      m_covariance->fillSymmetric(0, 0, c[ 0]);
      m_covariance->fillSymmetric(0, 1, c[ 1]);
      m_covariance->fillSymmetric(1, 1, c[ 2]);
      m_covariance->fillSymmetric(0, 2, c[ 3]);
      m_covariance->fillSymmetric(1, 2, c[ 4]);
      m_covariance->fillSymmetric(2, 2, c[ 5]);
      m_covariance->fillSymmetric(0, 3, c[ 6]);
      m_covariance->fillSymmetric(1, 3, c[ 7]);
      m_covariance->fillSymmetric(2, 3, c[ 8]);
      m_covariance->fillSymmetric(3, 3, c[ 9]);
      m_covariance->fillSymmetric(0, 4, c[10]);
      m_covariance->fillSymmetric(1, 4, c[11]);
      m_covariance->fillSymmetric(2, 4, c[12]);
      m_covariance->fillSymmetric(3, 4, c[13]);
      m_covariance->fillSymmetric(4, 4, c[14]);
    }

  ///////////////////////////////////////////////////////////////////
  // Set parameters with covariance
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::setParametersWithCovariance
    (const Surface* s,const double* p,const double* c)
    {
      m_surface.reset(s && s->isFree() ? s->clone() : s);
      m_parameters[ 0] = p[ 0];
      m_parameters[ 1] = p[ 1];
      m_parameters[ 2] = p[ 2];
      m_parameters[ 3] = p[ 3];
      m_parameters[ 4] = p[ 4];
      setCovariance(c  );
    }

  inline void PatternTrackParameters::setParametersWithCovariance
    (const Surface* s,const double* p,const AmgSymMatrix(5)& c)
    {
      double C[15] = {
        c(0, 0),
        c(1, 0), c(1, 1),
        c(2, 0), c(2, 1), c(2, 2),
        c(3, 0), c(3, 1), c(3, 2), c(3, 3),
        c(4, 0), c(4, 1), c(4, 2), c(4, 3), c(4, 4)
      };
      setParametersWithCovariance(s, p, C);
    }
  
  ///////////////////////////////////////////////////////////////////
  // Diagonal symetric matrix production
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::diagonalization(double D)
    {
      if (m_covariance == nullptr) {
        return;
      }

      m_covariance->fillSymmetric(0, 1, 0);
      m_covariance->fillSymmetric(0, 2, 0);
      m_covariance->fillSymmetric(1, 2, 0);
      m_covariance->fillSymmetric(0, 3, 0);
      m_covariance->fillSymmetric(1, 3, 0);
      m_covariance->fillSymmetric(2, 3, 0);
      m_covariance->fillSymmetric(0, 4, 0);
      m_covariance->fillSymmetric(1, 4, 0);
      m_covariance->fillSymmetric(2, 4, 0);
      m_covariance->fillSymmetric(3, 4, 0);

      (*m_covariance)(0, 0) *= D;
      (*m_covariance)(1, 1) *= D;
      (*m_covariance)(2, 2) *= D;
      (*m_covariance)(3, 3) *= D;
      (*m_covariance)(4, 4) *= D;
    }

  ///////////////////////////////////////////////////////////////////
  // Add noise to track parameters
  ///////////////////////////////////////////////////////////////////
  
  inline void PatternTrackParameters::addNoise
    (const NoiseOnSurface& N,PropDirection D) 
    {
      if (m_covariance != nullptr) {
        (*m_covariance)(2, 2)+=N.covarianceAzim();
        (*m_covariance)(3, 3)+=N.covariancePola();
        (*m_covariance)(4, 4)+=N.covarianceIMom();
      }

      if( D > 0 ) {
	N.correctionIMom() > 1. ? 
	  m_parameters[ 4]*=N.correctionIMom() : m_parameters[ 4]/=N.correctionIMom();
      }
      else {
	N.correctionIMom() > 1. ? 
	  m_parameters[ 4]/=N.correctionIMom() : m_parameters[ 4]*=N.correctionIMom();
      }
    }

  ///////////////////////////////////////////////////////////////////
  // Remove noise from track parameters
  ///////////////////////////////////////////////////////////////////
  
  inline void PatternTrackParameters::removeNoise
    (const NoiseOnSurface& N,PropDirection D) 
    {
      if (m_covariance != nullptr) {
        (*m_covariance)(2, 2)-=N.covarianceAzim();
        (*m_covariance)(3, 3)-=N.covariancePola();
        (*m_covariance)(4, 4)-=N.covarianceIMom();
      }

      if( D > 0 ) {
	N.correctionIMom() > 1. ? 
	  m_parameters[ 4]/=N.correctionIMom() : m_parameters[ 4]*=N.correctionIMom();
      }
      else {
	N.correctionIMom() > 1. ? 
	  m_parameters[ 4]*=N.correctionIMom() : m_parameters[ 4]/=N.correctionIMom();
      }
    }

  ///////////////////////////////////////////////////////////////////
  // Different  track parameters
  ///////////////////////////////////////////////////////////////////

  inline double         PatternTrackParameters::charge        () const
    {
      if (m_parameters[4] > 0.0) {
        return 1.0;
      } else {
        return -1.0;
      }
    }	

  inline double         PatternTrackParameters::sinPhi        () const
    {
      return sin(m_parameters[2]);
    }

  inline double         PatternTrackParameters::cosPhi        () const
    {
      return cos(m_parameters[2]);
    }
  
  inline double         PatternTrackParameters::sinTheta      () const
    {
      return sin(m_parameters[3]);
    }

  inline double         PatternTrackParameters::cosTheta      () const
    {
      return cos(m_parameters[3]);
    }

  inline double         PatternTrackParameters::cotTheta      () const
    {
      return (1./tan(m_parameters[3]));
    }

  inline Amg::Vector3D PatternTrackParameters::momentum      () const
    {
      return calculateMomentum();
    }
} // end of name space

#endif // PatternTrackParameters
