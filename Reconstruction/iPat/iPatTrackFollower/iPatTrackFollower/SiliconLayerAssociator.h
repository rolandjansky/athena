/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiliconLayerAssociator.h, (c) ATLAS iPatRec
//
// associate clusters to predictions given in a SiliconLayer
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFOLLOWER_SILICONLAYERASSOCIATOR_H
# define IPATTRACKFOLLOWER_SILICONLAYERASSOCIATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "iPatInterfaces/ISiliconLayerAssociator.h"
#include "iPatTrackFollower/LayerAssociation.h"
#include "iPatTrackFollower/SiliconClusterMap.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class SiliconLayerAssociator: public AthAlgTool,
	                      virtual public ISiliconLayerAssociator
{

public:
    SiliconLayerAssociator(const std::string& type,
			   const std::string& name,
			   const IInterface* parent);
    ~SiliconLayerAssociator(void);

    StatusCode	initialize();
    StatusCode	finalize();

    /**ILayerAllocator interface:
       clear internal state */
    void   	clear (void);
    
    /**ILayerAllocator interface:
       associate clusters from the SiliconClusterMap given predictions within a SiliconLayer */
    void   	associateClusters (const SiliconClusterMap*			clusterMap,
				   const Amg::Vector3D				position,
				   const Amg::Vector3D  		     	transverseDirection,
				   double					rPhiWidth,
				   double					zWidth,
				   std::list<LayerPrediction*>::const_iterator	begin,
				   std::list<LayerPrediction*>::const_iterator	end);
    
private:
    typedef	std::vector<LayerAssociation*>			association_vector;
    typedef	association_vector::iterator       		association_iterator;
    typedef	SiliconClusterMap::clusterIterator		cluster_iterator;
    typedef	std::list<LayerPrediction*>::const_iterator	prediction_iterator;
    
    void		associate (bool						isStereo,
				   std::list<LayerPrediction*>::const_iterator	begin,
				   std::list<LayerPrediction*>::const_iterator	end);
    LayerAssociation*	choose (LayerAssociation* reference_hit);
    void	       	makeBarrelAxialAssociations (LayerPrediction*	prediction);
    void	       	makeBarrelPixelAssociations (LayerPrediction*	prediction);
    void	       	makeBarrelStereoAssociations (LayerPrediction*	prediction);
    void	       	makeEndcapAxialAssociations (LayerPrediction*	prediction);
    void	       	makeEndcapPixelAssociations (LayerPrediction*	prediction);
    void	       	makeEndcapStereoAssociations (LayerPrediction*	prediction);

    ToolHandle<IInDetConditionsTool> m_pixelConditions{this, "PixelConditionsTool", "PixelConditionsSummaryTool/InDetPixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};
    ToolHandle<IInDetConditionsTool> m_sctConditions{this, "SCT_ConditionsTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions Summary"};
    association_vector			m_associations;
    const SiliconClusterMap*		m_clusterMap;
    double				m_cosPhi;
    double				m_cosStereo;
    Amg::Vector3D			m_position;
    double				m_rPhiWidth;
    double				m_rStereoWidth;
    double				m_rZWidth;
    LayerAssociation*			m_referenceAssociation;
    double				m_sinPhi;
    double				m_sinStereo;
    Amg::Vector3D			m_transverseDirection;
    
};

#endif // IPATTRACKFOLLOWER_SILICONLAYERASSOCIATOR_H

