/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERS_T_H
#define TRKPARAMETERSBASE_PARAMETERS_T_H

// Trk includes
#include "TrkParametersBase/ParametersBase.h"
#include "TrkParametersBase/SurfaceUniquePtrT.h"
#include "TrkEventPrimitives/DefinedParameter.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include <assert.h>
#include <cmath>

class MsgStream;

class TrackParametersCnv_p2;

namespace Trk {
  /**
  @class ParametersT

  The templated base class for charged and neutral representations,
  template argumetns are the type and the surface
    
    - the parameters vector
	- charge
	- position
	- momentum
	- local position (ponter to)

	Which can be returned by the parameters() method as a Amg::Vector(DIM):
	\left(\begin{array}{c}
	x\\y\\z\\p_{x}\\p_{y}\\p_{z}\end{array}\right)
	\f$

	The standard dimension for both charged and neutral parameters
	is of dimension 5, extended parameters will have dimension 6.
    
    The friend list is motivated by the most common manipulation needs

	@author edward.moyse@cern.ch, andreas.salzburger@cern.ch
  */
    
  template <int DIM, class T, class S> class ParametersT : public ParametersBase<DIM, T> {
  
    public:
         
      friend class MaterialEffectsEngine;
      friend class ::TrackParametersCnv_p2;
          
        
  	  /** default constructor */
      ParametersT ();
  
      /** Constructor with local arguments - uses global <-> local for parameters */
      ParametersT (double loc1, double loc2, double phi, double theta, double qop, 
                   const S& surface,
  				   AmgSymMatrix(DIM)* covariance = 0);
  
      /** Constructor with local arguments - uses global <-> local for parameters */
      ParametersT (const AmgVector(DIM)& parameters,
                   const S& surface,
                   AmgSymMatrix(DIM)* covariance = 0);
  
      /** Constructor with local arguments - uses global <-> local for parameters.
          Takes ownership of the surface.
          Meant for use by P->T conversion. */
      ParametersT (const AmgVector(DIM)& parameters,
                   SurfaceUniquePtrT<const S> surface,
                   AmgSymMatrix(DIM)* covariance = 0);
  
      /** Constructor with mixed arguments 1 - uses global <-> local for parameters */
      ParametersT (const Amg::Vector3D& position,
                   double phi, double theta, double qop,
                   const S& surface,
                   AmgSymMatrix(DIM)* covariance = 0);
      
      /** Constructor with mixed arguments 2 - uses global <-> local for parameters */
      ParametersT (double loc1, double loc2,
                   const Amg::Vector3D& momentum,
                   double charge,
                   const S& surface,
                   AmgSymMatrix(DIM)* covariance = 0);
          
      /** Constructor with global arguments - uses global <-> local for parameters */
      ParametersT (const Amg::Vector3D& position,
                   const Amg::Vector3D& momentum,
  				   double charge,
  				   const S& surface,
  				   AmgSymMatrix(DIM)* covariance = 0);
  
  	  /** Copy constructor */
      ParametersT (const ParametersT& rhs);
      
      /** Assignment operator */
      ParametersT& operator= (const ParametersT& rhs);

      /** Move assignment operator */
      ParametersT& operator= (ParametersT&& rhs);
      
      //**Destructor*/
      virtual ~ParametersT ();
      
      /** Virtual constructor. Allows copying of vector of ParametersT*/
      virtual ParametersT* clone() const override { return new ParametersT<DIM,T,S>(*this); }
  
      /** Access to the Surface method */
      virtual const S& associatedSurface() const override { return (*m_surface); }    
  
      /** Return the measurementFrame of the parameters */
      const Amg::RotationMatrix3D measurementFrame() const override;
  
      /** Return the ParametersType enum */
      virtual ParametersType type() const override { return AtaSurface; }

    private:
	  /** --- PRIVATE CONSTRUCTOR : for persistency purpose only */
      ParametersT (const AmgVector(DIM)& parameters,
                   const S* surface,
                   AmgSymMatrix(DIM)* covariance = 0);

      /** --- PRIVATE METHOD: access is controlled via friend list 
          Update method for single defined parameters parameters,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      void updateParameter(DefinedParameter updatedParameter,
                           AmgSymMatrix(DIM)* updatedCovariance = 0) const override;
                                   
      /** ---- PRIVATE METHOD: access is controlled via friend list 
          Update method for measurement parameter update and material effects update,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      void updateParameters(const AmgVector(DIM)& updatedParameters,
                            AmgSymMatrix(DIM)* updatedCovariance = 0) const override; 
  
      mutable const S*                m_surface;          //!< surface template
      
          
  };

} //end of namespace Trk

#include "TrkParametersBase/ParametersT.icc"
#endif // TRKTRACKPARAMETERS_H

