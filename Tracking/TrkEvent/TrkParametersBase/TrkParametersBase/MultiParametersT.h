/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultiParameters.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_MULTIPARAMETERST_H
#define TRKPARAMETERSBASE_MULTIPARAMETERST_H

// STD
#include <set>
#include <functional>
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/ParametersBase.h"

namespace Trk
{
  /** typedef for track parameters with their associated weight */
  template<int DIM>
  using WeightedTrackParameters = std::pair<double,ParametersBase<DIM,Trk::Charged>*>;

  /** typedef for set of weighted track parameters sorted according decreasing weight */
  template<int DIM>
  using TrackParameterSet = std::multiset<WeightedTrackParameters<DIM>, std::greater<WeightedTrackParameters<DIM> > >;
  
  /**
     @class MultiParametersT
     
     class describing multiple sets of track parameters, each one associcated with a weight

     @tparam DIM number of track parameters

     @author <christian.gumpert@cern.ch>
   */
  template<int DIM>
  class MultiParametersT : public ParametersBase<DIM,Trk::Charged>
  {  
  public:
    typedef typename TrackParameterSet<DIM>::iterator        iterator;
    typedef typename TrackParameterSet<DIM>::const_iterator  const_iterator;
    typedef typename TrackParameterSet<DIM>::size_type       size_type;
    
    /** default constructor */
    MultiParametersT(ParametersBase<DIM,Trk::Charged>& tp,double weight = 1);

    /** copy constructor */
    MultiParametersT(const MultiParametersT<DIM>&) = default;

    /** move constructor */
    MultiParametersT(MultiParametersT<DIM>&&) = default;
    
    /** destructor */
    virtual ~MultiParametersT() = default;

    /** assignment operator */
    MultiParametersT<DIM>& operator=(const MultiParametersT<DIM>&) = default;

    /** move assignment operator */
    MultiParametersT<DIM>& operator=(MultiParametersT<DIM>&&) = default;

    //* equality operator */
    virtual bool operator==(const ParametersBase<DIM,Trk::Charged>& rhs) const override;
    
    /** Return the ParametersType enum */
    virtual ParametersType type() const override {return Trk::MultiPars;}

    /** Virtual constructor */
    virtual MultiParametersT<DIM>* clone() const override {return new MultiParametersT<DIM>(*this);}
    
    /** Returns charge of concrete type (i.e. must be implemented in inheriting classes) */
    virtual double charge() const override;

    /** Access method for the parameters */
    virtual const AmgVector(DIM)& parameters() const override;             
      
    /** Access method for the covariance matrix - 0 if no covariance matrix is given */
    virtual const AmgSymMatrix(DIM)* covariance() const override;
      
    /** Access to the Surface method */
    virtual const Surface& associatedSurface() const override;
      
    /** Return the measurement frame - this is needed for alignment, in particular for StraightLine and Perigee Surface
	- the default implementation is the the RotationMatrix3D of the transform */
    virtual const Amg::RotationMatrix3D measurementFrame() const override;
      
    /** Access method for the position */
    virtual const Amg::Vector3D& position() const override;
      
    /** Access method for the momentum */
    virtual const Amg::Vector3D& momentum() const override;

    /** Access method for tp parameter list */
    const TrackParameterSet<DIM>& trackParameterList() const {return m_TPList;}
    
    /** Access method for tp parameter list */
    TrackParameterSet<DIM>& trackParameterList() {return m_TPList;}

    /** Access to beginning of parameter list */
    iterator begin() {return m_TPList.begin();}
    
    /** Access to beginning of parameter list (const) */
    const_iterator begin() const {return m_TPList.begin();}

    /** Access to end of parameter list */
    iterator end() {return m_TPList.end();}
    
    /** Access to end of parameter list (const) */
    const_iterator end() const {return m_TPList.end();}

    /** Insert new track parameter into the set */
    iterator insert(ParametersBase<DIM,Trk::Charged>& tp,double weight = 1);
    
    /** Number of track parameters stored */
    size_type size() const {return m_TPList.size();}
	
  private:
    TrackParameterSet<DIM> m_TPList;  //!< list of contained track parameters with their weights
  };
} // end of namespace Trk

#include "MultiParametersT.icc"

#endif // TRKPARAMETERSBASE_MULTIPARAMETERS_H
