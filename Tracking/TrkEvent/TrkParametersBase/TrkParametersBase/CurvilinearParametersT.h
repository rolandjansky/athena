/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CurvilinearParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_CURVILINEARPARAMETERS_T_H
#define TRKPARAMETERSBASE_CURVILINEARPARAMETERS_T_H

// STL
#include <memory>

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
// Tracking includes
#include "TrkParametersBase/ParametersT.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"

class MsgStream;

namespace Trk
{
  /**
   @class CurvilinearParametersT

   The specialised class for charged and neutral track parameters in the curvilinear frame.

   The method parameters() returns a vector of the track parameters, defined as:
   \f$\left(\begin{array}{c}
   locX\\locY\\\phi\\\theta\\q/p
   \end{array}\right)\f$

   The following is true per definition:
    - the local position is per definition (0.,0.)
    - the surface is per definition a plane surface normal to the track (UVT frame)

   @tparam DIM number of track parameters (usually 5)
   @tparam T   charge of track (either <tt>Trk::Charged</tt> or <tt>Trk::Neutral</tt>)
   
   @author andreas.salzburger@cern.ch
  */

  template<int DIM,class T, class S>
  class CurvilinearParametersT : public ParametersT<DIM,T,S>
  {
  public:
    /** default constructor only for POOL */
    CurvilinearParametersT() = default;

    /** Create CurvilinearParametersT from DIM+2 parameters
        - these are: global position, momentum, charge, extension */
    CurvilinearParametersT(const AmgVector(DIM+2)& parameters,
			   AmgSymMatrix(DIM)* covariance = 0,
			   unsigned int cIdenfier = 0);
    
    /**Create CurvilinearParametersT from mixed parameters: pos, local parameters*/
    CurvilinearParametersT(const Amg::Vector3D& pos, 
			   double phi, 
			   double theta, 
			   double qOverP,
			   AmgSymMatrix(DIM)* covariance = 0,
			   unsigned int cIdenfier = 0);
       
    /** Create CurvilinearParametersT from global parameters.
       -- it will throw a GaudiException if the position is not on surface  */     
    CurvilinearParametersT(const Amg::Vector3D& pos, 
			   const Amg::Vector3D& mom,
			   double charge,
			   AmgSymMatrix(DIM)* covariance = 0,
			   unsigned int cIdenfier = 0);
        
    /** Copy Constructor */
    CurvilinearParametersT(const CurvilinearParametersT<DIM,T,S>&); 
   
    /** Move Constructor */
    CurvilinearParametersT(CurvilinearParametersT<DIM,T,S>&&); 
   
    /** Destructor */
    virtual ~CurvilinearParametersT()=default;
                 
    /** Assignment operator*/
    CurvilinearParametersT<DIM,T,S> &operator=(const CurvilinearParametersT<DIM,T,S>&);

    /** Move assignment operator*/
    CurvilinearParametersT<DIM,T,S> &operator=(CurvilinearParametersT<DIM,T,S>&&);

    /** equality operator */
    virtual bool operator==(const ParametersBase<DIM,T>& rhs) const override final;

    /** Pseudo constructor */             
    virtual CurvilinearParametersT<DIM,T,S>* clone() const override final 
    {return new CurvilinearParametersT<DIM,T,S>(*this);}
                 
    /** Return the ParametersType enum */
    virtual ParametersType type() const override
    {return Trk::Curvilinear;}
    
    /** Return the measurementFrame of the parameters */
    virtual const Amg::RotationMatrix3D measurementFrame() const override final;

    /**Dumps relevant information about the track parameters into the ostream.*/
    virtual MsgStream& dump(MsgStream& out) const override;
    virtual std::ostream& dump(std::ostream& out) const override;

    /** the curvilinear parameters identifier */
    unsigned int cIdentifier() const {return m_cIdentifier;}

  protected:
    friend class MaterialEffectsEngine;
    virtual void updateParameters(const AmgVector(DIM)& updatedParameters,AmgSymMatrix(DIM)* updatedCovariance = 0) override; 

  private:
    /** return the curvilinear frame */
    const CurvilinearUVT curvilinearFrame() const;
    
    /** the curvilinear parameters identifier */
    unsigned int m_cIdentifier=0;
  };
}//end of namespace Trk

#include "TrkParametersBase/CurvilinearParametersT.icc"
#endif
