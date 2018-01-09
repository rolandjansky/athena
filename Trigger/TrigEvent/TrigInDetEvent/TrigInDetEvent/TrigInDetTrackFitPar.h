/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETTRACKFITPAR_H
#define TRIGINDETTRACKFITPAR_H
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include <vector>

/** @class TrigInDetTrackFitPar
    encapsulates LVL2 track parameters and covariance matrix
    The vector of track parameters consists of 

    \f$ a_0 \f$  , \f$ \phi_0 \f$, \f$ z_0 \f$, \f$ \eta \f$, \f$ p_T \f$

    The first three parameters are defined as follows 
    
    if m_surfaceType is PERIGEE - the default

    \f$ a_0 \f$     : transverse impact parameter, a.k.a., \f$ d_0 \f$, the distance of the closest approach 
                      of a helix to the z-axis.  
 
    \f$ \phi_0 \f$  : momentum angle phi at the point of the closest approach 

    \f$ z_0 \f$     : longitudinal impact parameter, the z value at the point of the closest approach.

    The convention for the sign of \f$ a_0 \f$ is the following. Let \f$ \Phi \f$ be the azimuthal angle to the 
    perigee point. The sign of \f$ a_0 \f$ is then defined as positive if \f$ \Phi = \phi_0 + \frac{\pi}{2} \f$. 
    Note that this convention is independent from particle charge sign. 

    if m_surfaceType is BARREL, radius is given by m_surfaceCoordinate

    \f$ a_0 \f$    : Phi - azimuthal angle of the track position on the surface

    \f$ \phi_0 \f$  : momentum angle phi in the global reference frame

    \f$ z_0 \f$    : z-position (along the barrel axis)

   if m_surfaceType is ENDCAP,  surface is a disk orthogonal to the global z-axis
                                z-coordinate of the disk is given by m_surfaceCoordinate

    \f$ a_0 \f$    : Phi - azimuthal angle of the track position on the surface

    \f$ \phi_0 \f$  : momentum angle phi in the global reference frame

    \f$ z_0 \f$    : Rho - radial coordinate of the track position on the disk

    Covariance matrix is stored in a std::vector<double>: 

    cov   : vector of 15 numbers corresponding to the upper half
            of the covariance matrix, packed as follows:
            cov(a0,a0) ->  cov(a0,phi0) -> cov(a0,z0) ->  cov(a0,eta) ->  cov(a0,pT) ->
                        cov(phi0,phi0) -> cov(phi0,z0) -> cov(phi0,eta) -> cov(phi0,pT) ->
                                           cov(z0,z0) ->  cov(z0,eta) ->  cov(z0,pT) ->
                                                         cov(eta,eta) -> cov(eta,pT) ->
                                                                            cov(pT,pT) 


   m_ea0, m_ephi0, m_ez0, m_eeta, m_epT - square roots out of corresponding diagonal elements 
   of the covariance matrix

   Note that pT is actually pT \f$ \times \f$ sign of particle charge
   
*/

class TrigInDetTrackFitPar {
  
 public:

  enum TrigSurfaceType{PERIGEE=0, BARREL=1, ENDCAP=2, UNDEFINED=3};

  /** Constructor for POOL only */
  TrigInDetTrackFitPar () : //!< Default constructor: needed by POOL 
    m_a0(0.0), m_phi0(0.0), m_z0(0.0),m_eta(0.0), m_pT(0.0), 
    m_ea0(0.0), m_ephi0(0.0), m_ez0(0.0), m_eeta(0.0), m_epT(0.0), 
    m_cov(NULL), m_surfaceType(PERIGEE), m_surfaceCoordinate(0.0)
    {};

  /** Constructor for parameters on PERIGEE surface */
  TrigInDetTrackFitPar (const double a0,
			const double phi0,
			const double z0,
			const double eta,
			const double pT,
			const double ea0,
			const double ephi0,
			const double ez0,
			const double eeta,
			const double epT,
			const std::vector<double>* cov=0) :
    m_a0(a0), m_phi0(phi0), m_z0(z0),
    m_eta(eta), m_pT(pT), m_ea0(ea0), m_ephi0(ephi0), m_ez0(ez0),
    m_eeta(eeta), m_epT(epT), m_cov(cov),m_surfaceType(PERIGEE), 
    m_surfaceCoordinate(0.0)
    {};

  
  /** Constructor for parameters on non-PERIGEE surface */
  TrigInDetTrackFitPar (const double a0,
			const double phi0,
			const double z0,
			const double eta,
			const double pT,
			const double ea0,
			const double ephi0,
			const double ez0,
			const double eeta,
			const double epT,
			const TrigSurfaceType t,
			const double c,
			const std::vector<double>* cov=0) :
    m_a0(a0), m_phi0(phi0), m_z0(z0),
    m_eta(eta), m_pT(pT), m_ea0(ea0), m_ephi0(ephi0), m_ez0(ez0),
    m_eeta(eeta), m_epT(epT), m_cov(cov), m_surfaceType(t), m_surfaceCoordinate(c)
    {};

  /** Constructor for PERIGEE parameters without errors or covariance */
  TrigInDetTrackFitPar (const double a0,
                        const double phi0,
                        const double z0,
                        const double eta,
                        const double pT,
                        const std::vector<double>* cov=0) :
    m_a0(a0), m_phi0(phi0), m_z0(z0),
    m_eta(eta), m_pT(pT),
    m_ea0(0.0), m_ephi0(0.0), m_ez0(0.0), m_eeta(0.0), 
    m_epT(0.0),   m_cov(cov), m_surfaceType(PERIGEE), 
    m_surfaceCoordinate(0.0){ 
      if(cov!=NULL) { 
	if((*cov)[0]>=0.0) m_ea0 = sqrt((*cov)[0]); 
	if((*cov)[5]>=0.0) m_ephi0 = sqrt((*cov)[5]); 
	if((*cov)[9]>=0.0) m_ez0 = sqrt((*cov)[9]); 
	if((*cov)[12]>=0.0) m_eeta = sqrt((*cov)[12]);  
	if((*cov)[14]>=0.0) m_epT = sqrt((*cov)[14]);  
      } 
    } 

  /** Constructor for non-PERIGEE parameters without errors or covariance */
  TrigInDetTrackFitPar (const double a0,
                        const double phi0,
                        const double z0,
                        const double eta,
                        const double pT,
			const TrigSurfaceType t,
			const double c,
                        const std::vector<double>* cov=0) :
    m_a0(a0), m_phi0(phi0), m_z0(z0),
    m_eta(eta), m_pT(pT), 
    m_ea0(0.0), m_ephi0(0.0), m_ez0(0.0), m_eeta(0.0), m_epT(0.0), m_cov(cov), m_surfaceType(t), 
    m_surfaceCoordinate(c)
    {
      if(cov!=NULL) { 
	if((*cov)[0]>=0.0) m_ea0 = sqrt((*cov)[0]); 
	if((*cov)[5]>=0.0) m_ephi0 = sqrt((*cov)[5]); 
	if((*cov)[9]>=0.0) m_ez0 = sqrt((*cov)[9]); 
	if((*cov)[12]>=0.0) m_eeta = sqrt((*cov)[12]);  
	if((*cov)[14]>=0.0) m_epT = sqrt((*cov)[14]);  
      } 
    }

  /** Copy constructor. */
  TrigInDetTrackFitPar (const TrigInDetTrackFitPar& rhs)
    : m_a0 (rhs.m_a0),
      m_phi0 (rhs.m_phi0),
      m_z0 (rhs.m_z0),
      m_eta (rhs.m_eta),
      m_pT (rhs.m_pT),
      m_ea0 (rhs.m_ea0),
      m_ephi0 (rhs.m_ephi0),
      m_ez0 (rhs.m_ez0),
      m_eeta (rhs.m_eeta),
      m_epT (rhs.m_epT),
      m_surfaceType (rhs.m_surfaceType),
      m_surfaceCoordinate (rhs.m_surfaceCoordinate)
  {
    if (rhs.m_cov)
      m_cov = new std::vector<double> (*rhs.m_cov);
    else
      m_cov = nullptr;
  }

  /** Move assignment. */
  TrigInDetTrackFitPar& operator= (TrigInDetTrackFitPar&& rhs)
  {
    if (this != &rhs) {
      m_a0 = rhs.m_a0;
      m_phi0 = rhs.m_phi0;
      m_z0 = rhs.m_z0;
      m_eta = rhs.m_eta;
      m_pT = rhs.m_pT;
      m_ea0 = rhs.m_ea0;
      m_ephi0 = rhs.m_ephi0;
      m_ez0 = rhs.m_ez0;
      m_eeta = rhs.m_eeta;
      m_epT = rhs.m_epT;
      m_surfaceType = rhs.m_surfaceType;
      m_surfaceCoordinate = rhs.m_surfaceCoordinate;

      delete m_cov;
      m_cov = rhs.m_cov;
      rhs.m_cov = nullptr;
    }
    return *this;
  }
  
  /** Destructor */
  ~TrigInDetTrackFitPar() {if(m_cov) delete m_cov;}


  // Methods to set data members
  /** Setter: transverse impact parameter */
  void a0  ( const double a0   )         { m_a0    = a0;    }
  /** Setter: longitudinal impact parameter */
  void z0  ( const double z0   )         { m_z0     = z0;   }
  /** Setter: azimuthal angle of the momentum */
  void phi0( const double phi0 )         { m_phi0   = phi0; }
  /** Setter: pseudorapidity */
  void eta ( const double eta  )         { m_eta = eta;     }
  /** Setter: transverse momentum */
  void pT  ( const double pT   )         { m_pT   = pT;     }
  /** Setter: covariance matrix of track parameters */
  void cov ( const std::vector<double>* cov )   { m_cov = cov; }
  /** Setter: surface type PERIGEE=0, BARREL=1, ENDCAP=2 */
  void surfaceType(TrigSurfaceType s) { m_surfaceType=s; }
  /** Setter: surface reference coordinate for non-perigee surfaces */
  void surfaceCoordinate(double c) { m_surfaceCoordinate=c; }
  
  // Methods to retrieve data members 
  /** transverse impact parameter */
  double a0()     const                  { return m_a0;     }
  /** longitudinal impact parameter */
  double z0()     const                  { return m_z0;     }
  /** azimuthal angle of the momentum */
  double phi0()   const                  { return m_phi0;   }
  /** pseudorapidity */
  double eta()    const                  { return m_eta;    }
  /** transverse momentum */
  double pT()     const                  { return m_pT;     }
  /** variance of transverse impact parameter */
  double ea0()    const                  { return m_ea0;    }
  /** variance of longitudinal impact parameter */
  double ez0()    const                  { return m_ez0;    }
  /** variance of azimuthal angle of the momentum */
  double ephi0()  const                  { return m_ephi0;  }
  /** variance of pseudorapidity */
  double eeta()   const                  { return m_eeta;   }
  /** variance of transverse momentum */
  double epT()    const                  { return m_epT;    }
  /** covariance (packed) of track parameters */
  const std::vector<double>* cov() const { return m_cov;    }
  /** surface type */
  TrigSurfaceType surfaceType() const { return m_surfaceType; }
  /** surface reference coordinate (radius or z-position) for non-perigee parameters */
  double surfaceCoordinate() const { return m_surfaceCoordinate; }
  
 private:
  double m_a0;//!< see detailed description below
  double m_phi0;//!< see detailed description below
  double m_z0;//!< see detailed description below
  double m_eta;//!< pseudorapidity
  double m_pT;//!< transverse momentum
  double m_ea0;//!<\f$ \sqrt{cov(a_0,a_0)}\f$
  double m_ephi0;//!<\f$ \sqrt{cov(\phi_0,\phi_0)}\f$
  double m_ez0;//!<\f$ \sqrt{cov(z_0,z_0)}\f$
  double m_eeta;//!<\f$ \sqrt{cov(\eta,\eta)}\f$
  double m_epT;//!<\f$ \sqrt{cov(p_T,p_T)}\f$
  const std::vector<double>* m_cov;//!< covariance matrix packed as described below
  TrigSurfaceType m_surfaceType;//!< type of track parameters - perigee, barrel, or endcap
  double m_surfaceCoordinate;//!<barrel radius or z of endcap disk
};

CLASS_DEF( TrigInDetTrackFitPar , 159201573 , 1 )
CLASS_DEF( DataVector<TrigInDetTrackFitPar> , 99002908 , 1 )

#endif 






