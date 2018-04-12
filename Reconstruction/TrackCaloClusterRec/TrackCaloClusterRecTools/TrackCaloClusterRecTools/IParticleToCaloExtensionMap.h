/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_IPARTICLETOCALEXTENSIONMAP_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_IPARTICLETOCALEXTENSIONMAP_H

#include "xAODBase/IParticle.h"
#include "TrkParameters/TrackParameters.h"

class IParticleToCaloExtensionMap {
 public:
  IParticleToCaloExtensionMap();
  ~IParticleToCaloExtensionMap();
  
  void addEntry(const xAOD::IParticle& particle, const Trk::CurvilinearParameters& extension);
  const Trk::CurvilinearParameters*  readCaloEntry(const xAOD::IParticle* particle) const;
  unsigned int size() const;

 private :
  std::map<const xAOD::IParticle*, const Trk::CurvilinearParameters > m_caloExtensionMap;

};

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( IParticleToCaloExtensionMap , 221362399 , 2 )

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_IPARTICLETOCALEXTENSIONMAP_H