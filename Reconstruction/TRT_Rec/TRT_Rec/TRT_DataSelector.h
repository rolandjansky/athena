/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool selecting RDO's in vicinity of a Track prolongation into the TRT
 ***************************************************************************/

#ifndef TRTREC_TRTDATASELECTOR_H
# define TRTREC_TRTDATASELECTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "iPatInterfaces/ITRT_DataSelector.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class MsgStream;
class TRT_ID;
class TRT_Prediction;
class TRT_RoadData;
namespace InDetDD
{
    class TRT_DetectorManager;
}

class TRT_DataSelector: public AthAlgTool,
			virtual public ITRT_DataSelector
{

public:
    TRT_DataSelector			(const std::string& type, 
					 const std::string& name,
					 const IInterface* parent);
    ~TRT_DataSelector 			(void); 	// destructor
    
    StatusCode				initialize();
    StatusCode				finalize();

    const std::vector<TRT_RoadData*>&	roadData(
	const std::vector<TRT_Prediction*>& predictions, double	rRoadVertex);
    int					selectedLayers(void) const;
    int					selectedStraws(void) const;
    int					selectedTrLayers(void) const;
    void				setDataContainer(
	const InDet::TRT_DriftCircleContainer* container);
  
private:

    bool					m_debug;
    const InDet::TRT_DriftCircleContainer*	m_driftCircleContainer;
    MsgStream* 					m_log;
    std::vector<TRT_RoadData*>			m_roadDataList;
    int						m_selectedLayers;
    int						m_selectedStraws;
    int						m_selectedTrLayers;
    const TRT_ID*				m_trtHelper;
    bool					m_verbose;
    
};   

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline int
TRT_DataSelector::selectedLayers (void) const
{ return m_selectedLayers; }

inline int
TRT_DataSelector::selectedStraws (void) const
{ return m_selectedStraws; }

inline int
TRT_DataSelector::selectedTrLayers (void) const
{ return m_selectedTrLayers; }

#endif // TRTREC_TRTDATASELECTOR_H
