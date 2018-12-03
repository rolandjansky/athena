/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 least squared fit to track hit data => PerigeeParameters with covariance
 and fit quality
 ------------------------------------------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATREC_TRACKFITTER_H
# define IPATREC_TRACKFITTER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "iPatInterfaces/IiPatFitter.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class ITrackingVolumesSvc;
    class FitMeasurement;
    class FitParameters;
    class FitProcedure;
    class FitProcedureQuality;
    class IIntersector;
    class IPropagator;
    class IMaterialAllocator;
    class Volume;
}

class TrackFitter: public AthAlgTool,
		   virtual public IiPatFitter
{

public:

    TrackFitter			(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~TrackFitter		(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    // updates the track PerigeeParameters and FitQuality
    void			fit(TrackStatus		status,
				    FitQuality*&	fitQuality,
				    PerigeeParameters&	perigee,
				    hit_const_iterator	begin,
				    hit_const_iterator	end);
      
    // updates the track PerigeeParameters, FitQuality and residuals. May modify hits
    void			fitWithRejection(TrackStatus		status,
						 FitQuality*&        	fitQuality,
						 int			maxReject,
						 PerigeeParameters&	perigee,
						 parameter_vector*& 	scattererParameters,
						 hit_iterator 		begin,
						 hit_iterator 		end,
						 hit_const_iterator 	original_begin,
						 hit_const_iterator 	original_end);

    // as fit above - but with residual set in HitOnTrack
    void			fitWithResiduals(TrackStatus		status,
						 FitQuality*&		fitQuality,
						 PerigeeParameters&	perigee,
						 parameter_vector*&	scattererParameters,
						 hit_const_iterator	begin,
						 hit_const_iterator	end);

    // uses transverse vertex as a measurement
    void			fitWithVertex(TrackStatus		status,
					      FitQuality*&		fitQuality,
					      PerigeeParameters&	perigee,
					      const Trk::RecVertex&	vertex,
					      hit_const_iterator	begin,
					      hit_const_iterator	end);
    
protected:
    // clean up memory
    void			clearMeasurements (void);

    // create internal measurements from HitOnTrack or RIO_OnTrack
    void	       		createMeasurements (hit_const_iterator begin,
						    hit_const_iterator end);
    
    // set and invoke fortran fit procedure
    void			fitProcedure (TrackStatus		status,
					      const PerigeeParameters&	perigee,
					      hit_const_iterator      	begin,
					      hit_const_iterator      	end);
    
    // debug print-out
    void			print (TrackStatus		status,
				       const PerigeeParameters&	perigee,
				       const FitQuality&	fitQuality) const;
    
    // get methods to extract fit results
    void			createPerigee (PerigeeParameters&) const;
    
    void			createQuality (FitQuality*&) const;
    parameter_vector*		createScatterers (void) const;
    void			setResiduals (void) const;

    // temporary 3-point treatment
    void			threePoint (const PerigeeParameters&	perigee,
					    hit_const_iterator      	begin,
					    hit_const_iterator      	end);
  
    int					m_driftHits;
    bool				m_fieldOff;
    int					m_fitCode;
    Trk::FitProcedure*			m_fitProcedure;
    Trk::FitProcedureQuality*		m_fitProcedureQuality;
    int					m_hit_pt3;
    ToolHandle<Trk::IMaterialAllocator>	m_materialAllocator;
    std::vector<Trk::FitMeasurement*>	m_measurements;
    Trk::FitParameters*			m_parameters;
    int					m_planarHits;
    ToolHandle<Trk::IIntersector>	m_rungeKuttaIntersector;
    double				m_scatteringAngle;
    double				m_second_sigma_pt1;
    double				m_sigma_pt1;
    double				m_sigma_pt3;
    ToolHandle<Trk::IIntersector>	m_solenoidalIntersector;
    ToolHandle<Trk::IIntersector>	m_straightLineIntersector;
    ToolHandle< Trk::IPropagator >      m_stepPropagator;
    ServiceHandle<Trk::ITrackingVolumesSvc>  m_trackingVolumesSvc;
    Trk::FitMeasurement*		m_vertexMeasurement;
    Trk::Volume*                        m_indetVolume;

};

#endif // IPATREC_TRACKFITTER_H
