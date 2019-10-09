/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP3
#define __TRK_PARTICLE_TRUTH_COLL_CNTNR_CNV_TLP3

/*
  @brief T/P conversion for TrackParticleTruthCollectionContainer used in the trigger
  @author Jiri.Masik@cern.ch
 */

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_tlp3.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p3.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p3.h"

///Tau details container converter
class TrackParticleTruthCollectionContainerCnv_tlp3:
    public AthenaPoolTopLevelTPConverter<TrackParticleTruthCollectionContainerCnv_p3, 
					 TrackParticleTruthCollectionContainer_tlp3>
{
public:
  TrackParticleTruthCollectionContainerCnv_tlp3();

  virtual ~TrackParticleTruthCollectionContainerCnv_tlp3() {}

  virtual void setPStorage( TrackParticleTruthCollectionContainer_tlp3 *storage );

protected:
  TrackParticleTruthCollectionCnv_p3 m_trackparttruthcollCnv;

};

// ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
template <>
class T_TPCnv<TrackParticleTruthCollectionContainer,
	      TrackParticleTruthCollectionContainer_tlp3 >
  : public TrackParticleTruthCollectionContainerCnv_tlp3
{
public:
  ///Constructor
  T_TPCnv();
    
  ///Destructor
  virtual ~T_TPCnv();
  
  virtual void persToTrans (const TrackParticleTruthCollectionContainer_tlp3* pers,
			    TrackParticleTruthCollectionContainer* trans,
			    MsgStream& msg);

  virtual void transToPers (const TrackParticleTruthCollectionContainer* trans,
			    TrackParticleTruthCollectionContainer_tlp3* pers,
			    MsgStream& msg);
};




#endif
