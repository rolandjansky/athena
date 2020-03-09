/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleContainerCnv.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLECONTAINER_CNV_H
#define TRACKPARTICLECONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "Particle/TrackParticleContainer.h"

#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp1.h"
#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp2.h"
#include "TrackParticleTPCnv/TrackParticleContainerCnv_tlp3.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Rec::TrackParticleContainer_tlp3	TrackParticleContainer_PERS;

typedef T_AthenaPoolCustomCnv<Rec::TrackParticleContainer, TrackParticleContainer_PERS> TrackParticleContainerCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class TrackParticleContainerCnv : public TrackParticleContainerCnvBase
{
 
 friend class CnvFactory<TrackParticleContainerCnv>;
 
 protected:
  
public:
  TrackParticleContainerCnv( ISvcLocator *svcloc );
protected:
  virtual StatusCode initialize() override;
  
  virtual TrackParticleContainer_PERS *createPersistent( Rec::TrackParticleContainer *transCont) override;
  virtual Rec::TrackParticleContainer *createTransient() override;

  AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv()
  { return &m_TPConverter_tlp3; }

 
 private:
  IMessageSvc          *m_msgSvc;
  TrackParticleContainerCnv_tlp1	m_TPConverter_tlp1;
  TrackParticleContainerCnv_tlp2	m_TPConverter_tlp2;
  TrackParticleContainerCnv_tlp3	m_TPConverter_tlp3;

};//end of class definitions

#endif // TRACKPARTICLECONTAINER_CNV_H
