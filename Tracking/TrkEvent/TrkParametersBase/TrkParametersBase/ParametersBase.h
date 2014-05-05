/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERSBASE_H
#define TRKPARAMETERSBASE_PARAMETERSBASE_H

// Trk
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/DefinedParameter.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/GaudiException.h"

#ifndef TRKPARAMSDIM
#define TRKPARAMSDIM 5
#endif

#ifndef TRKEXTENDEDPARAMSDIM
#define TRKEXTENDEDPARAMSDIM 6
#endif

class MsgStream;

namespace Trk {

  class Surface;

  /**
  @class ParametersBase

  The Base class for Neutral and Charged Track parameters.
  It represents the free state of a trajectory, represented by

  The position and the momentum are both given in the tracking
  reference frame.

  @author Andreas.Salzburger@cern.ch
  */

  template <int DIM, class T> class ParametersBase {
    public:
      /** Default constructor */
      ParametersBase(AmgSymMatrix(DIM)* covariance = 0);
      
      /** Constructor with global position */
      ParametersBase(const Amg::Vector3D& pos,
                     AmgSymMatrix(DIM)* covariance = 0);
      
      /** Constructor with glboal arguments */
      ParametersBase(const Amg::Vector3D& pos,
                     const Amg::Vector3D& mom,
                     AmgSymMatrix(DIM)* covariance = 0);
      
      /** Copy constructor */
      ParametersBase(const ParametersBase& pbase);
      
      /** Desctructor */
      virtual ~ParametersBase();
      
      /** Assignmnet operator */
      ParametersBase& operator=(const ParametersBase& pbase);
      
      /** Access method for the parameters */
      const AmgVector(DIM)& parameters() const { return m_parameters; }             
      
      /** Access method for the covariance matrix - 0 if no covariance matrix is given */
      const AmgSymMatrix(DIM)* covariance() const { return m_covariance; }
      
      /** Access to the Surface method */
      virtual const Surface& associatedSurface() const = 0;
      
      /** Access method for the position */
      const Amg::Vector3D& position() const  { return m_position; }
      
      /** Access method for the momentum */
      const Amg::Vector3D& momentum() const { return m_momentum; } 
      
      /** Access method for the local coordinates, \f$(loc1,loc2)\f$ 
          local parameter definitions differ for each surface type. */
      const Amg::Vector2D localPosition() const 
        { return Amg::Vector2D(m_parameters[loc1],m_parameters[loc2]); }
      
      /* Access method for transverse momentum */
      double pT() const { return m_momentum.perp(); }
      
      /** Access method for pseudorapidity - from momentum */
      double eta() const { return m_momentum.eta(); }
      
      /** Return number of parameters currently created - EDM monitoring */
      static unsigned int numberOfInstantiations() { return s_numberOfInstantiations; }
      
      /**Dumps relevant information about the track parameters into the ostream*/
      MsgStream&    dump( MsgStream& out ) const; 
      std::ostream& dump( std::ostream& out ) const ;
      
      /** Pseudo constructor - avoids excessive type-casting.
      @return new object copied from the concrete type of this object. 
      Ownership is passed (i.e. you must take care of deletion of resulting object)*/
      virtual ParametersBase* clone() const = 0;
      
      /** Returns charge of concrete type (i.e. must be implemented in inheriting classes)
         defined 0. if 1/pt=0*/
      double charge () const { return (double)m_chargeDefinition; };

    protected:
      /** --- PROTECTED METHOD: access is controlled via fiend list 
          Checkout method for covariance matrix,
          this sets the pointer to 0, but does not delete it, ownership is passed back */
      AmgSymMatrix(DIM)* checkoutCovariance() const;

      /** --- PROTECTED METHOD: access is controlled via friend list 
          Update method for single defined parameters parameters,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      virtual void updateParameter(DefinedParameter updatedParameter,
                                   AmgSymMatrix(DIM)* updatedCovariance = 0) const = 0;
                                   
      /** ---- PROTECTED METHOD: access is controlled via friend list 
          Update method for measurement parameter update and material effects update,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      virtual void updateParameters(const AmgVector(DIM)& updatedParameters,
                                    AmgSymMatrix(DIM)* updatedCovariance = 0) const = 0;
       
      mutable AmgVector(DIM)        m_parameters;       //!< contains the n parameters
      mutable AmgSymMatrix(DIM)*    m_covariance;       //!< contains the n x n covariance matrix
      mutable T                     m_chargeDefinition; //!< charge definition template
        
      mutable Amg::Vector3D         m_position;         //!< point on track
      mutable Amg::Vector3D         m_momentum;         //!< momentum at this point on track 

      /** number of objects of this type in memory */
      static unsigned int            s_numberOfInstantiations;
  };

      /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
      template <int DIM, class T> MsgStream& operator << ( MsgStream& sl, const ParametersBase<DIM,T>& pbas);
      template <int DIM, class T> std::ostream& operator << ( std::ostream& sl, const ParametersBase<DIM,T>& pbas); 
} // end of namespace

#include "TrkParametersBase/ParametersBase.icc"

#endif
