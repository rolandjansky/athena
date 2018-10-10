/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool describing arrangement of silicon layers wrt vertex region. 
 Allocates layer number to a SpacePoint (needed during pattern recognition).
 --------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATGEOMETRYTOOLS_LAYERNUMBERALLOCATOR_H
# define IPATGEOMETRYTOOLS_LAYERNUMBERALLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ILayerNumberAllocator.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ILayerAllocator;
class SiliconLayer;

class LayerNumberAllocator: public AthAlgTool,
			    virtual public ILayerNumberAllocator
{

public:
    LayerNumberAllocator	(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~LayerNumberAllocator 	(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    int				layerNumber (double r, double z) const;
    void			print (void) const;
    
private:
    typedef	std::vector<double>	       		boundary_vector;
    typedef	boundary_vector::iterator      		boundary_iterator;
    typedef	boundary_vector::const_iterator 	boundary_citerator;
    typedef	std::vector<SiliconLayer*>		layer_vector;
    typedef	layer_vector::const_iterator		layer_citerator;
    typedef	layer_vector::const_reverse_iterator   	layer_criterator;
    typedef	std::vector<boundary_vector*> 		wheel_vector;
    typedef	wheel_vector::iterator			wheel_iterator;
    typedef	wheel_vector::const_iterator   		wheel_citerator;
    typedef	wheel_vector::reverse_iterator 		wheel_riterator;
    
    void				pixel_definition (const layer_vector& barrel,
							  const layer_vector& endcap);
    void				sct_definition   (const layer_vector& barrel,
							  const layer_vector& endcap);

          
    // helpers, managers, tools
    ToolHandle<ILayerAllocator>		m_layerAllocator;
    
    // state
    int					m_pixel_layers;
    wheel_vector			m_pixel_r_boundaries;
    double			       	m_pixel_sct_boundary;
    boundary_vector			m_pixel_z_boundary;
    wheel_vector			m_sct_r_boundaries;
    boundary_vector			m_sct_z_boundary;
    double				m_vertex_spread;

};   

#endif // IPATGEOMETRYTOOLS_LAYERNUMBERALLOCATOR_H
