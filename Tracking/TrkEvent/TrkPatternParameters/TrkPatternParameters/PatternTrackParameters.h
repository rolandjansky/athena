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
#include "TrkPatternParameters/NoiseOnSurface.h"

class MsgStream;

namespace Trk {

  class PlaneSurface       ;  
  class StraightLineSurface;
  class DiscSurface        ;
  class CylinderSurface    ;
  class PerigeeSurface     ;
  class ConeSurface        ;

  class PatternTrackParameters
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

      const double*    par               ()     const {return  &m_parameters[0];}
      const double*    cov               ()     const {return  &m_covariance[0];}
      const Surface*   associatedSurface ()     const {return   m_surface      ;}
      const bool&      iscovariance      ()     const {return   m_iscovariance ;}
      Amg::Vector2D    localPosition     ()     const;
      Amg::Vector3D    momentum          ()     const; 
      Amg::Vector3D    position          ()     const; 
      AmgVector(5)     parameters        ()     const;
      AmgSymMatrix(5)  covariance        ()     const;
      double           charge            ()     const;	
      double           sinPhi            ()     const;
      double           cosPhi            ()     const;
      double           sinTheta          ()     const;
      double           cosTheta          ()     const;
      double           cotTheta          ()     const;
      double           pT                ()     const;
      double           eta               ()     const;   
      void             changeDirection   ()          ;

      ///////////////////////////////////////////////////////////////////
      // Methods set
      ///////////////////////////////////////////////////////////////////
      
      void setParameters              (const Surface*,const double*              );
      void setCovariance              (                             const double*);
      void setParametersWithCovariance(const Surface*,const double*,const double*);

      ///////////////////////////////////////////////////////////////////
      // Convertors
      ///////////////////////////////////////////////////////////////////

      const ParametersBase<5, Trk::Charged>* convert(bool) const;
      bool  production(const ParametersBase<5, Trk::Charged>*);

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
      
      void newCovarianceMatrix
	(PatternTrackParameters&,double*);

      ///////////////////////////////////////////////////////////////////
      // Print
      ///////////////////////////////////////////////////////////////////

      std::ostream& dump(std::ostream&) const;
      MsgStream&    dump(MsgStream&   ) const;	

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data
      ///////////////////////////////////////////////////////////////////

      const  Surface* m_surface       ;
      double          m_parameters[ 5];
      double          m_covariance[15];
      bool            m_iscovariance  ;

      ///////////////////////////////////////////////////////////////////
      // Comments
      // m_surface is pointer to associated surface
      // m_parameters[ 0] - 1 local coordinate
      // m_parameters[ 1] - 2 local coordinate
      // m_parameters[ 2] - Azimuthal angle
      // m_parameters[ 3] - Polar     angle
      // m_parameters[ 4] - charge/Momentum
      // m_covariance is  elements of the lower triangle of covariance matrix
      //   0    
      //   1  2
      //   3  4  5
      //   6  7  8  9
      //  10 11 12 13 14
      // m_iscovariance is true if m_covariance contains correct information
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
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Overload operator
  /////////////////////////////////////////////////////////////////////////////////

  std::ostream& operator << (std::ostream&,const PatternTrackParameters&); 
  MsgStream& operator    << (MsgStream&, const PatternTrackParameters& );
 
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////


  inline PatternTrackParameters::PatternTrackParameters()
    {
      m_surface      =     nullptr;
      m_iscovariance =     false;
      std::fill (std::begin(m_parameters), std::end(m_parameters), 0);
      std::fill (std::begin(m_covariance), std::end(m_covariance), 0);
    }

  inline PatternTrackParameters::PatternTrackParameters(const PatternTrackParameters& P):
    m_surface(P.m_surface),m_parameters{},m_covariance{},m_iscovariance(P.m_iscovariance)
    {
      *this = P;
    }
  
  inline PatternTrackParameters& PatternTrackParameters::operator = 
    (const PatternTrackParameters& P) 
    {
      if (&P != this){
				m_surface        = P.m_surface       ;
				m_parameters[ 0] = P.m_parameters[ 0];
				m_parameters[ 1] = P.m_parameters[ 1];
				m_parameters[ 2] = P.m_parameters[ 2];
				m_parameters[ 3] = P.m_parameters[ 3];
				m_parameters[ 4] = P.m_parameters[ 4];
				m_covariance[ 0] = P.m_covariance[ 0];
				m_covariance[ 1] = P.m_covariance[ 1];
				m_covariance[ 2] = P.m_covariance[ 2];
				m_covariance[ 3] = P.m_covariance[ 3];
				m_covariance[ 4] = P.m_covariance[ 4];
				m_covariance[ 5] = P.m_covariance[ 5];
				m_covariance[ 6] = P.m_covariance[ 6];
				m_covariance[ 7] = P.m_covariance[ 7];
				m_covariance[ 8] = P.m_covariance[ 8];
				m_covariance[ 9] = P.m_covariance[ 9];
				m_covariance[10] = P.m_covariance[10];
				m_covariance[11] = P.m_covariance[11];
				m_covariance[12] = P.m_covariance[12];
				m_covariance[13] = P.m_covariance[13];
				m_covariance[14] = P.m_covariance[14];
				m_iscovariance   = P.m_iscovariance  ;
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
      m_surface        =     s;  
      m_parameters[ 0] = p[ 0];
      m_parameters[ 1] = p[ 1];
      m_parameters[ 2] = p[ 2];
      m_parameters[ 3] = p[ 3];
      m_parameters[ 4] = p[ 4];
      m_iscovariance   = false;
    }

  ///////////////////////////////////////////////////////////////////
  // Set covariance
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::setCovariance
    (const double* c)
    {
      m_covariance[ 0] = c[ 0];
      m_covariance[ 1] = c[ 1];
      m_covariance[ 2] = c[ 2];
      m_covariance[ 3] = c[ 3];
      m_covariance[ 4] = c[ 4];
      m_covariance[ 5] = c[ 5];
      m_covariance[ 6] = c[ 6];
      m_covariance[ 7] = c[ 7];
      m_covariance[ 8] = c[ 8];
      m_covariance[ 9] = c[ 9];
      m_covariance[10] = c[10];
      m_covariance[11] = c[11];
      m_covariance[12] = c[12];
      m_covariance[13] = c[13];
      m_covariance[14] = c[14];
      m_iscovariance  = true;
    }

  ///////////////////////////////////////////////////////////////////
  // Set parameters with covariance
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::setParametersWithCovariance
    (const Surface* s,const double* p,const double* c)
    {
      setParameters(s,p);
      setCovariance(c  );
    }
  
  ///////////////////////////////////////////////////////////////////
  // Diagonal symetric matrix production
  ///////////////////////////////////////////////////////////////////

  inline void PatternTrackParameters::diagonalization(double D)
    {
      double* c = &m_covariance[0];

      c[ 0]*=D ;
      c[ 1] =0.;  c[ 2]*=D ;
      c[ 3] =0.;  c[ 4] =0.; c[ 5]*=D ;
      c[ 6] =0.;  c[ 7] =0.; c[ 8] =0.; c[ 9]*=D ;
      c[10] =0.;  c[11] =0.; c[12] =0.; c[13] =0.; c[14]*=D;
    }

  ///////////////////////////////////////////////////////////////////
  // Add noise to track parameters
  ///////////////////////////////////////////////////////////////////
  
  inline void PatternTrackParameters::addNoise
    (const NoiseOnSurface& N,PropDirection D) 
    {
      m_covariance[ 5]+=N.covarianceAzim();
      m_covariance[ 9]+=N.covariancePola();
      m_covariance[14]+=N.covarianceIMom();

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
      m_covariance[ 5]-=N.covarianceAzim();
      m_covariance[ 9]-=N.covariancePola();
      m_covariance[14]-=N.covarianceIMom();

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

  inline Amg::Vector2D PatternTrackParameters::localPosition () const
    {
      Amg::Vector2D lp(m_parameters[0],m_parameters[1]);  
      return lp;
    }

  inline double         PatternTrackParameters::charge        () const
    {
      if(m_parameters[4] > 0.) { return 1.; } return -1.;

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

  inline double         PatternTrackParameters::pT            () const
    {
      if(m_parameters[4]!=0.) { return fabs(sin(m_parameters[3])/m_parameters[4]);
}
      return 10e9;
    }

  inline double         PatternTrackParameters::eta           () const
    {
      return -log(tan(.5*m_parameters[3]));
    }   
  
  inline AmgVector(5) PatternTrackParameters::parameters    () const
    {
      AmgVector(5) p;

      p[0] = m_parameters[0];
      p[1] = m_parameters[1];
      p[2] = m_parameters[2];
      p[3] = m_parameters[3];
      p[4] = m_parameters[4];
      return p;
    }          
  
  inline Amg::Vector3D PatternTrackParameters::momentum      () const
    {
      double p   = m_parameters[4]!=0. ?  1./fabs(m_parameters[4]) :  10e9;

      double Sf;
      double Cf; sincos(m_parameters[2],&Sf,&Cf);
      double Se;
      double Ce; sincos(m_parameters[3],&Se,&Ce);
      double psn = p*Se;
      Amg::Vector3D GM(psn*Cf,psn*Sf,p*Ce);
      return GM;
    } 

  inline AmgSymMatrix(5) PatternTrackParameters::covariance  () const
    {
      AmgSymMatrix(5) C;
      const double* c = &m_covariance[0];
      C<<
	c[ 0],c[ 1],c[ 3],c[ 6],c[10],
	c[ 1],c[ 2],c[ 4],c[ 7],c[11],
	c[ 3],c[ 4],c[ 5],c[ 8],c[12],
	c[ 6],c[ 7],c[ 8],c[ 9],c[13],
	c[10],c[11],c[12],c[13],c[14];
      return C;
    }

} // end of name space

#endif // PatternTrackParameters
