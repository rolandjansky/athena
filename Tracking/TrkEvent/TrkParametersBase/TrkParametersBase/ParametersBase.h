/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERSBASE_H
#define TRKPARAMETERSBASE_PARAMETERSBASE_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <memory>

class MsgStream;

namespace Trk
{
  class Surface;
  class MaterialEffectsEngine;

  /**
     @enum ParametersType
     
     Enum to avoid dynamic cast for different parameter types.
     
     From AtaSurface one can go directly to the Surface::type()
     enumeration.
  */
  enum ParametersType {
    AtaSurface   = 0,
    Curvilinear  = 1
  };

  /**
     @class ParametersBase

     The base class for neutral and charged Track parameters.
     It represents the free state of a trajectory, represented by
     the track parameters.
     The position and the momentum are both given in the tracking
     reference frame.

     @tparam DIM number of track parameters (usually 5)
     @tparam T   charge of track (either <tt>Trk::Charged</tt> or <tt>Trk::Neutral</tt>)
     
     @author Andreas.Salzburger@cern.ch
  */

  template <int DIM,class T>
  class ParametersBase
  {
  public:
    static constexpr int dim = DIM;
    /*
     * Rule of 5 
     */
    /** virtual Destructor */
    virtual ~ParametersBase()=default;
    //** equality operator */
    virtual bool operator==(const ParametersBase<DIM,T>&) const = 0;
    
    /** Access method for the parameters */
    const AmgVector(DIM)& parameters() const;             
      
    /** Access method for the covariance matrix - returns nullptr if no covariance matrix is given */
    const AmgSymMatrix(DIM)* covariance() const;
 
    /** Access method for the position */
    const Amg::Vector3D& position() const;
 
    /** Access method for the momentum */
    const Amg::Vector3D& momentum() const;
 
    /** Access method for transverse momentum */
    double pT() const;
      
    /** Access method for pseudorapidity - from momentum */
    double eta() const;
 
    /** Returns charge of concrete type (i.e. must be implemented in inheriting classes) */
    double charge() const;
 
    /** Access method for the local coordinates, \f$(loc1,loc2)\f$ 
	local parameter definitions differ for each surface type. */
    Amg::Vector2D localPosition() const;
  
    /** Access to the Surface method */
    virtual const Surface& associatedSurface() const = 0;
      
    /** Return the measurement frame - this is needed for alignment, in particular for StraightLine and Perigee Surface
	- the default implementation is the the RotationMatrix3D of the transform */
    virtual Amg::RotationMatrix3D measurementFrame() const = 0;
     
   /** Pseudo constructor - avoids excessive type-casting.
        @return new object copied from the concrete type of this object.*/
    virtual ParametersBase<DIM,T>* clone() const = 0;

    /** Return the ParametersType enum */
    virtual ParametersType type() const = 0;
    
    /** Test to see if there's a surface there. */
    virtual bool hasSurface() const = 0 ;

    /** Update parameters and covariance */
    virtual void updateParameters(const AmgVector(DIM)&, AmgSymMatrix(DIM)* = nullptr) = 0;

    /** Update parameters  and covariance , passing covariance by ref. A covariance
     * is created if one does not exist.  Otherwise in place update occurs*/
    virtual void updateParameters(const AmgVector(DIM)&, const AmgSymMatrix(DIM)&) = 0;

    /** Dumps relevant information about the track parameters into the ostream */
    virtual MsgStream&    dump(MsgStream& out) const; 
    virtual std::ostream& dump(std::ostream& out) const;
 
    /** DESIGN TO BE REVISITED */
    friend class MaterialEffectsEngine;

  protected :
    /*
     * This has pure virtual functions 
     * so it is abstract class and we can not instanticate objects directly.
     * In the other hand derived classed can use ctors
     */
    ParametersBase()=default; 
    ParametersBase(const AmgVector(DIM) parameters,
                   AmgSymMatrix(DIM)* covariance,
                   const Amg::Vector3D&  position,
                   const Amg::Vector3D&  momentum,
                   const T  chargeDef);
    ParametersBase(ParametersBase&&)=default;
    ParametersBase& operator=(ParametersBase&&)=default;  



    AmgVector(DIM)                      m_parameters;       //!< contains the n parameters
    std::unique_ptr<AmgSymMatrix(DIM)>  m_covariance;       //!< contains the n x n covariance matrix
    Amg::Vector3D                       m_position;         //!< point on track
    Amg::Vector3D                       m_momentum;         //!< momentum at this point on track
    T                                   m_chargeDef;        //!< charge definition for this track
  };

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/
  template<int DIM,class T>
  MsgStream& operator<<(MsgStream& sl,const Trk::ParametersBase<DIM,T>& tp);

  template<int DIM,class T>
  std::ostream& operator<<(std::ostream& sl,const Trk::ParametersBase<DIM,T>& tp);

} // end of namespace Trk


#include "TrkParametersBase/ParametersBase.icc"

#endif
