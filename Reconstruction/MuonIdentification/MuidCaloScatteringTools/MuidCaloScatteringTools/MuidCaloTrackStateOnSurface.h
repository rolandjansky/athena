/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuidCaloTrackStateOnSurface
  AlgTool to allocate 3 scattering centres as TrackStateOnSurface objects
  to represent the Coulomb scattering and energy deposit between the InDet
  and MuonSpectrometer entrance.

  This tool retrieves the appropriate parametrized scattering centres and
  hybrid (measured + parametrized) energy deposit evaluation
  using the IMuidCaloMaterialParam and IMuidCaloEnergy interfaces
  
   @author Alan.Poppleton@cern.ch
  (c) ATLAS Combined Muon software
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H
#define MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class IPropagator;
    class MagneticFieldProperties;
}

namespace Rec
{

class IMuidCaloEnergy;   
class IMuidCaloMaterialParam;

class MuidCaloTrackStateOnSurface: public AthAlgTool,
				   virtual public IMuidCaloTrackStateOnSurface
{

public:
    MuidCaloTrackStateOnSurface (const std::string&	type, 
				 const std::string&	name,
				 const IInterface*	parent);
    ~MuidCaloTrackStateOnSurface (void); // destructor
  
    StatusCode				initialize();
    StatusCode				finalize();

    /**IMuidCaloTrackStateOnSurface interface:
       to get the 3 scattering and energy deposit TSOS'es representing the calorimeter.
       The input TrackParameters may be anywhere along the track. */
    std::vector<const Trk::TrackStateOnSurface*>* caloTSOS(
	const Trk::TrackParameters& parameters) const;
    
    /**IMuidCaloTrackStateOnSurface interface:
       to get individually the scattering TSOS'es representing the calorimeter.
       The input TrackParameters may be anywhere along the track. */
    const Trk::TrackStateOnSurface*	innerTSOS (const Trk::TrackParameters& parameters) const;
    const Trk::TrackStateOnSurface*	outerTSOS (const Trk::TrackParameters& parameters) const;
    
    /**IMuidCaloTrackStateOnSurface interface:
       to get the energy deposit TSOS representing the calorimeter.
       The input TrackParameters may be anywhere along the track, but when the inner
       or outer parameters are provided they must be expressed at the appropriate surface.
       The return TSOS surface is at the material midpoint. */
    const Trk::TrackStateOnSurface*	middleTSOS(const Trk::TrackParameters& middleParameters,
						   const Trk::TrackParameters* innerParameters,
						   const Trk::TrackParameters* outerParameters) const;
    
private:
    // private methods
    const Trk::TrackParameters*		innerParameters  (const Trk::TrackParameters& parameters) const;
    const Trk::TrackParameters*		middleParameters (const Trk::TrackParameters& parameters) const;
    const Trk::TrackParameters*		outerParameters  (const Trk::TrackParameters& parameters) const;
    bool				useEtaPhiFromDirection (
	const Trk::TrackParameters& parameters) const;
    
    // helpers, managers, tools
    ToolHandle<Rec::IMuidCaloEnergy>		m_caloEnergyDeposit;
    ToolHandle<Rec::IMuidCaloEnergy>		m_caloEnergyParam;
    ToolHandle<Rec::IMuidCaloMaterialParam>	m_caloMaterialParam;
    Trk::MagneticFieldProperties*		m_magFieldProperties;
    ServiceHandle<MagField::IMagFieldSvc>       m_magFieldSvcHandle;
    MagField::IMagFieldSvc*                     m_magFieldSvc;
    ToolHandle<Trk::IPropagator>		m_propagator;

    // configurable cut-off values
    double					m_minCaloRadius;
    double					m_minRemainingEnergy;
    double					m_paramPtCut;
    
    // counters (for finalize)
    mutable int					m_count;
    mutable int					m_countArbitrarySolution;
    mutable int					m_countCompleteFailure;
    mutable int					m_countInnerFailure;
    mutable int					m_countOuterFailure;
};

}	// end of namespace

#endif	// MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H


