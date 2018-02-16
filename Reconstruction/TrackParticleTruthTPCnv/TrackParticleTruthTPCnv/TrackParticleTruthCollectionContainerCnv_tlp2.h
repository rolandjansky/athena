/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP2
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP2

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_tlp2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p2.h"

///Tau details container converter
class TrackParticleTruthCollectionContainerCnv_tlp2:
    public AthenaPoolTopLevelTPConverter<TrackParticleTruthCollectionContainerCnv_p2, 
					 TrackParticleTruthCollectionContainer_tlp2>
{
public:
  TrackParticleTruthCollectionContainerCnv_tlp2();

  virtual ~TrackParticleTruthCollectionContainerCnv_tlp2() {}

  virtual void setPStorage( TrackParticleTruthCollectionContainer_tlp2 *storage );

protected:
  TrackParticleTruthCollectionCnv_p2 m_trackparttruthcollCnv;

};

// ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
template <>
class T_TPCnv<TrackParticleTruthCollectionContainer,
	      TrackParticleTruthCollectionContainer_tlp2 >
  : public TrackParticleTruthCollectionContainerCnv_tlp2
{
public:
  ///Constructor
  T_TPCnv();
    
  ///Destructor
  virtual ~T_TPCnv();
  
  virtual void persToTrans (const TrackParticleTruthCollectionContainer_tlp2* pers,
			    TrackParticleTruthCollectionContainer* trans,
			    MsgStream& msg);

  virtual void transToPers (const TrackParticleTruthCollectionContainer* trans,
			    TrackParticleTruthCollectionContainer_tlp2* pers,
			    MsgStream& msg);
};




#endif
