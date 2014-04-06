/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP1
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP1

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_tlp1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p1.h"

///Tau details container converter
class TrackParticleTruthCollectionContainerCnv_tlp1:
    public AthenaPoolTopLevelTPConverter<TrackParticleTruthCollectionContainerCnv_p1, 
					 TrackParticleTruthCollectionContainer_tlp1>
{
public:
  TrackParticleTruthCollectionContainerCnv_tlp1();

  virtual ~TrackParticleTruthCollectionContainerCnv_tlp1() {}

  virtual void setPStorage( TrackParticleTruthCollectionContainer_tlp1 *storage );

protected:
  TrackParticleTruthCollectionCnv_p1 m_trackparttruthcollCnv;

};

// ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
template <>
class T_TPCnv<TrackParticleTruthCollectionContainer,
	      TrackParticleTruthCollectionContainer_tlp1 >
  : public TrackParticleTruthCollectionContainerCnv_tlp1
{
public:
  ///Constructor
  T_TPCnv();
    
  ///Destructor
  virtual ~T_TPCnv();
  
  virtual void persToTrans (const TrackParticleTruthCollectionContainer_tlp1* pers,
			    TrackParticleTruthCollectionContainer* trans,
			    MsgStream& msg);

  virtual void transToPers (const TrackParticleTruthCollectionContainer* trans,
			    TrackParticleTruthCollectionContainer_tlp1* pers,
			    MsgStream& msg);
};




#endif
