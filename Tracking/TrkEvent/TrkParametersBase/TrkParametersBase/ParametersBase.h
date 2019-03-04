/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERSBASE_H
#define TRKPARAMETERSBASE_PARAMETERSBASE_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

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
    Curvilinear  = 1,
    MultiPars    = 2
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
    /** default ctor , copy ctors, and assignment operators*/
    ParametersBase()=default; 
    ParametersBase(const ParametersBase&)=default;
    ParametersBase& operator=(const ParametersBase&)=default;
    ParametersBase(ParametersBase&&)=default;
    ParametersBase& operator=(ParametersBase&&)=default;  

    //** equality operator */
    virtual bool operator==(const ParametersBase<DIM,T>&) const = 0;
    
    //** inequality operator */
    bool operator!=(const ParametersBase<DIM,T>& rhs) const
    {
      return !(*this == rhs);
    }
    
    /** Access method for the parameters */
    virtual const AmgVector(DIM)& parameters() const = 0;             
      
    /** Access method for the covariance matrix - returns 0 if no covariance matrix is given */
    virtual const AmgSymMatrix(DIM)* covariance() const = 0;
      
    /** Access to the Surface method */
    virtual const Surface& associatedSurface() const = 0;
      
    /** Return the measurement frame - this is needed for alignment, in particular for StraightLine and Perigee Surface
	- the default implementation is the the RotationMatrix3D of the transform */
    virtual const Amg::RotationMatrix3D measurementFrame() const = 0;
      
    /** Access method for the position */
    virtual const Amg::Vector3D& position() const = 0;
      
    /** Access method for the momentum */
    virtual const Amg::Vector3D& momentum() const = 0;
      
    /** Access method for the local coordinates, \f$(loc1,loc2)\f$ 
	local parameter definitions differ for each surface type. */
    Amg::Vector2D localPosition() const;
      
    /** Access method for transverse momentum */
    double pT() const {return momentum().perp();}
      
    /** Access method for pseudorapidity - from momentum */
    double eta() const {return momentum().eta();}
      
    /** Dumps relevant information about the track parameters into the ostream */
    virtual MsgStream&    dump(MsgStream& out) const; 
    virtual std::ostream& dump(std::ostream& out) const;
      
    /** Pseudo constructor - avoids excessive type-casting.
	@return new object copied from the concrete type of this object. 
	Ownership is passed (i.e. you must take care of deletion of resulting object) */
    virtual ParametersBase<DIM,T>* clone() const = 0;

    /** Return the ParametersType enum */
    virtual ParametersType type() const = 0;
    
    /** Test to see if there's a surface there. */
    virtual bool hasSurface() const = 0 ;


    /** Returns charge of concrete type (i.e. must be implemented in inheriting classes) */
    virtual double charge() const = 0;

    /** Update parameters of these TrackParameters - be careful when using if you don't own the object! */
    virtual void updateParameters(const AmgVector(DIM)&,AmgSymMatrix(DIM)* = 0) {};

    /** DESIGN TO BE REVISITED */
  public:
    friend class MaterialEffectsEngine;

  };

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/
  template<int DIM,class T>
  MsgStream& operator<<(MsgStream& sl,const Trk::ParametersBase<DIM,T>& tp);

  template<int DIM,class T>
  std::ostream& operator<<(std::ostream& sl,const Trk::ParametersBase<DIM,T>& tp);

} // end of namespace Trk


#include "TrkParametersBase/ParametersBase.icc"

#endif
