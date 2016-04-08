/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDEXAMPLE_READALIGNTRANS_H
#define DETDESCRCONDEXAMPLE_READALIGNTRANS_H
// ReadAlignTrans - simple algorithm to demonstrate reading of
// AlignableTransforms for SCT and Pixel
// Richard Hawkings, started 19/1/04

#include <vector>
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/IOVSvcDefs.h"

class IGeoModelTool;
class IGeoModelSvc;
namespace InDetDD {
  class SiDetectorManager;
}

class ReadAlignTrans: public AthAlgorithm
{
 public:
  ReadAlignTrans(const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadAlignTrans(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

  StatusCode testCallBack( IOVSVC_CALLBACK_ARGS );
  StatusCode readSiPositions(std::vector<Identifier>&, 
			     std::vector<Amg::Vector3D >&);

 private:
  IGeoModelSvc* p_geomodelsvc;
  const InDetDD::SiDetectorManager* p_sidetman[2];
  std::vector<Identifier> m_identcache;
  std::vector<Amg::Vector3D> m_poscache;
};

#endif // DETDESCRCONDEXAMPLE_READALIGNTRANS_H
