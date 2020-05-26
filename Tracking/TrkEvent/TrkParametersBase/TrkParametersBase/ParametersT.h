/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERS_T_H
#define TRKPARAMETERSBASE_PARAMETERS_T_H

// Trk includes
#include "TrkParametersBase/ParametersBase.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParametersBase/SurfaceUniquePtrT.h"

/*
 * Needed for persistency 
 * friends
 */
template<typename T>
class TrackParametersCovarianceCnv;
class TrackParametersCnv_p2;
class MeasuredPerigeeCnv_p1;
template< class SURFACE_CNV, class ATA_SURFACE >
class AtaSurfaceCnv_p1;


namespace Trk
{
  class MaterialEffectsEngine;

  /**
     @class ParametersT

     The class for charged and neutral track representations containing
    
     - the parameters vector
     - charge
     - position
     - momentum

     Which can be returned by the parameters() method as a Amg::Vector(DIM):
     \f$
     \left(\begin{array}{c}
     x\\y\\z\\p_{x}\\p_{y}\\p_{z}\end{array}\right)
     \f$

     @tparam DIM number of track parameters (usually 5)
     @tparam T charge of track (either <tt>Trk::Charged</tt> or <tt>Trk::Neutral</tt>)
     @tparam S type of surface
     
     @author edward.moyse@cern.ch, andreas.salzburger@cern.ch
     @author Christos Anastopoulos (Athena MT modifications)
  */
  template<int DIM,class T,class S>
  class ParametersT : public ParametersBase<DIM,T>
  {
  public:
    /** 
     * default constructor ONLY for POOL and derived classes 
     */
    ParametersT();
    
    /** Constructor with local arguments - uses global <-> local for parameters */
    ParametersT(double loc1,
		double loc2,
		double phi,
		double theta,
		double qop, 
		const S& surface,
		AmgSymMatrix(DIM)* covariance = nullptr);
  
    /** Constructor with parameters - extract position and momentum */
    ParametersT(const AmgVector(DIM)& parameters,
		const S& surface,
		AmgSymMatrix(DIM)* covariance = 0);
  
    /** Constructor with global arguments - uses global <-> local for parameters */
    ParametersT(const Amg::Vector3D& position,
		const Amg::Vector3D& momentum,
		double charge,
		const S& surface,
		AmgSymMatrix(DIM)* covariance = nullptr);

    /** Constructor with mixed arguments 1 - uses global <-> local for parameters */
    ParametersT (const Amg::Vector3D& position,
		 double phi, double theta, double qop,
		 const S& surface,
		 AmgSymMatrix(DIM)* covariance = nullptr);
    
    /** Copy constructor */
    ParametersT(const ParametersT<DIM,T,S>& rhs);
      
    /** Move constructor */
    ParametersT(ParametersT<DIM,T,S>&& rhs);
    
    /** Assignment operator */
    ParametersT<DIM,T,S>& operator=(const ParametersT<DIM,T,S>& rhs);

    /** Move assignment operator */
    ParametersT<DIM,T,S>& operator=(ParametersT<DIM,T,S>&& rhs);
      
    //** Destructor */
    virtual ~ParametersT()=default;
 
    /** Test to see if there's a surface there. */
    virtual bool hasSurface() const override final { return m_surface != nullptr; }
 
    /** Access to the Surface method */
    virtual const S& associatedSurface() const override final {return *m_surface;}    
      
    /** equality operator */
    virtual bool operator==(const ParametersBase<DIM,T>& rhs) const override;

    /** Virtual clone */
    virtual ParametersT<DIM,T,S>* clone() const override {return new ParametersT<DIM,T,S>(*this);}

    /** Return the ParametersType enum */
    virtual ParametersType type() const override {return Trk::AtaSurface;}

    /** Return the measurementFrame of the parameters */
    virtual Amg::RotationMatrix3D measurementFrame() const override;
 
     
  private :
    /* Helper to factor in update of parameters*/
    virtual void updateParametersHelper(const AmgVector(DIM)&) override;

  protected:
   
    /*
     * Add dependent names into scope
     */
    using ParametersBase<DIM,T>::m_parameters;
    using ParametersBase<DIM,T>::m_covariance;
    using ParametersBase<DIM,T>::m_position;
    using ParametersBase<DIM,T>::m_momentum;
    using ParametersBase<DIM,T>::m_chargeDef;
    SurfaceUniquePtrT<const S> m_surface; //!< surface template
   
  /** 
     * @brief Constructor used by curvilinear 
     * Parameters
     */
    ParametersT(const Amg::Vector3D& pos,
                const Amg::Vector3D& mom,
                AmgSymMatrix(DIM) * covariance = 0);

    /** 
     * @brief Constructor for persistency
     */
    ParametersT (const AmgVector(DIM)& parameters,
                 const S* surface,
                 AmgSymMatrix(DIM)* covariance = 0);


    /*
     * friends needed for Persistency
     */
    template<typename pars>
    friend class ::TrackParametersCovarianceCnv;
    friend class ::TrackParametersCnv_p2;
    friend class ::MeasuredPerigeeCnv_p1;
    template <class SURFACE_CNV, class ATA_SURFACE>
    friend class ::AtaSurfaceCnv_p1;
 
    /** DESIGN TO BE REVISITED */
    friend class MaterialEffectsEngine;
  };
} //end of namespace Trk

#include "TrkParametersBase/ParametersT.icc"
#endif // TRKTRACKPARAMETERS_H

