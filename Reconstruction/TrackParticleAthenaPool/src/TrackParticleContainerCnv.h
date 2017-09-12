/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleContainerCnv.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLECONTAINER_CNV_H
#define TRACKPARTICLECONTAINER_CNV_H

// Hack so we can access the private data. EJWM
#define protected public
#include "GaudiKernel/MsgStream.h"
#undef protected

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
  
  TrackParticleContainerCnv( ISvcLocator *svcloc );
  ~TrackParticleContainerCnv();
  virtual StatusCode initialize();
  
  virtual TrackParticleContainer_PERS *createPersistent( Rec::TrackParticleContainer *transCont);
  virtual Rec::TrackParticleContainer *createTransient();

  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter_tlp3; }

 
 private:
  
  IMessageSvc          *m_msgSvc;
  MsgStream             m_log;
  TrackParticleContainerCnv_tlp1	m_TPConverter_tlp1;
  TrackParticleContainerCnv_tlp2	m_TPConverter_tlp2;
  TrackParticleContainerCnv_tlp3	m_TPConverter_tlp3;

};//end of class definitions

#endif // TRACKPARTICLECONTAINER_CNV_H
