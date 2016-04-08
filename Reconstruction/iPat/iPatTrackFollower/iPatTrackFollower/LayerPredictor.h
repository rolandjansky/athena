/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 predict ordered list<LayerPrediction*> for intersected SiliconDetector's
 within a given SiliconLayer
 ***************************************************************************/

#ifndef IPATTRACKFOLLOWER_LAYERPREDICTOR_H
# define IPATTRACKFOLLOWER_LAYERPREDICTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include <utility>
#include "GaudiKernel/ToolHandle.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ISiClusterProperties;
class LayerPrediction;

class LayerPredictor
{

public:
    typedef     std::list<LayerPrediction*>::const_iterator           prediction_const_iterator;

    // constructor
    LayerPredictor(hit_list*		hits,
		   const SiliconLayer&	layer,
		   const Amg::Vector3D&	position,
		   const Amg::Vector3D&	transverseDirection,
		   double		rPhiWidth,
		   double		rZWidth);

    // destructor
    ~LayerPredictor(void);

    // forbidden copy constructor
    // forbidden assignment operator

    // prediction iterators
    prediction_const_iterator	begin (void) const;
    prediction_const_iterator	end (void) const;
    
    // instantiate corresponding HitOnTrack's and insert in list (return number inserted)
    int				insertHits (const ToolHandle<ISiClusterProperties>& properties);

    // add layer material as a scattering centre (another HitOnTrack in list)
    void			insertScatterer (void);

    // number of holes found in layer
    int				numberOfHoles (void) const;
    
    // for debugging
    void			print (void) const;
    
private:
    typedef     std::list<LayerPrediction*>::iterator		prediction_iterator;
    typedef     std::list<LayerPrediction*>::reverse_iterator	prediction_reverse_iterator;

    // make a prediction for each intersected detector
    void		predict (SiliconLayer::ring_const_iterator	begin,
				 SiliconLayer::ring_const_iterator	end);

    static const double			s_boundaryTolerance;

    HitOnTrack*				m_finalHit;
    LayerPrediction*			m_finalPrediction;
    hit_list* 				m_hits;
    HitOnTrack*				m_initialHit;
    LayerPrediction*			m_initialPrediction;
    bool				m_isActive;
    bool				m_isBarrel;
    std::pair<double,double>		m_materialProperties;
    int					m_numberOfHits;
    int					m_numberOfHoles;
    Amg::Vector3D			m_position;
    std::list<LayerPrediction*>		m_predictions;
    double				m_rPhiWidth;
    double				m_rZWidth;
    Amg::Vector3D			m_transverseDirection;
    
    // copy, assignment: no semantics, no implementation
    LayerPredictor (const LayerPredictor&);
    LayerPredictor &operator= (const LayerPredictor&);
    
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline LayerPredictor::prediction_const_iterator
LayerPredictor::begin(void) const
{ return m_predictions.begin(); }

inline LayerPredictor::prediction_const_iterator
LayerPredictor::end(void) const
{ return m_predictions.end(); }

inline int
LayerPredictor::numberOfHoles(void) const
{ return m_numberOfHoles; }

#endif // IPATTRACKFOLLOWER_LAYERPREDICTOR_H

