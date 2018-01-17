/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRKPREPRAWDATAGENPARTICLEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRKPREPRAWDATAGENPARTICLEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "HepMC/GenParticle.h"
#include "Identifier/Identifier.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "AthenaKernel/BaseInfo.h"
#include <map>

SG_BASE(InDet::PixelCluster, Trk::PrepRawData);

namespace D3PD {

class TrkPrepRawDataGenParticleAssociationTool 
  : public MultiAssociationTool<Trk::PrepRawData, HepMC::GenParticle>
{
public:
  typedef MultiAssociationTool<Trk::PrepRawData, HepMC::GenParticle> Base;
  typedef std::multimap<Identifier, HepMcParticleLink>::const_iterator PRDTruthLink;

  TrkPrepRawDataGenParticleAssociationTool (const std::string& type,
					  const std::string& name,
					  const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode reset (const Trk::PrepRawData& p); 
  virtual const HepMC::GenParticle* next();

private:

  /* Iterators **/
  PRDTruthLink m_itr;
  PRDTruthLink m_end;

  /* StoreGate keys **/
  std::string m_mapKey;

  /* Flags **/
  bool m_storeBarcode;

  /* NTuple variables **/
  std::vector<int> *m_barcode;

  /* Need this in case we match to a null particle before reaching the end **/
  HepMC::GenParticle m_dummy;

}; // class TrkPrepRawDataGenParticleAssociationTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKPREPRAWDATAGENPARTICLEASSOCIATIONTOOL_H
