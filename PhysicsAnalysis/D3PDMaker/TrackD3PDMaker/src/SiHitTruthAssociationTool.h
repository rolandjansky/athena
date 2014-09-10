/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SIHITTRUTHASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_SIHITTRUTHASSOCIATIONTOOL_H
 
#include "D3PDMakerUtils/SingleAssociationTool.h"
 
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"
 #include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"

 namespace HepMC {
 class GenParticle;
 }
 
 namespace Trk {
 class Track;
 }
 
 namespace D3PD {
 
 class SiHitTruthAssociationTool
   : public SingleAssociationTool<SiHit, HepMC::GenParticle>
 {
 public:
  typedef SingleAssociationTool<SiHit, HepMC::GenParticle> Base;
 
  SiHitTruthAssociationTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent);
 
  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual const HepMC::GenParticle* get (const SiHit& sihit);

 private:
 
   /* Pointer to storegate **/
  // StoreGateSvc* m_storeGate;
 
  /* StoreGate keys **/
  //std::string m_TruthMap;
  //std::string m_TrkContainer;

  /* NTuple variables **/
  //float* m_trueProb;
  // int* m_trueBarcode;
 
 }; // class SiHitTruthAssociationTool
 
} // namespace D3PD
 
 #endif // not TRACKD3PDMAKER_SIHITTRUTHASSOCIATIONTOOL_H
 
