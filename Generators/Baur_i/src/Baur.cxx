/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AuthorList:
//     Georgios Stavropoulos:  Initial Code October 2002
//     Brian Cole, Mikhail Leltchouk, Andrzej Olszewski: 2006, 2007
// Modified:
//     2007-Feb Alden Stradling:  Added vertex shifting and made options settable in jO.
//     2007-Sep Aaron Angerami: keepSpectators option.
//     2007-Nov Brian Cole, Mikhail Leltchouk: each vertex is added to the event before particles are added to the vertex
//          - this fixed bug #30991 for release 13.1.0 and for HepMC 2.3.0 where the 'set's comparison operates
//            on the barcode rather than on the pointer.

#include "Baur_i/Baur.h"
#include "TruthUtils/GeneratorName.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Point3D.h"
#include <cstdlib>


extern "C" {
  void baurzgammamain_();
}


Baur::Baur(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator)
{
  m_storeGate = 0;
  m_efrm = 0.;
  m_iap = 0;
  m_iat = 0;
  m_izp = 0;
  m_izt = 0;
  m_bmin = 0.;
  m_bmax = 0.;
  m_x = 0.;
  m_y = 0.;
  m_z = 0.;
  m_sel = false;
  m_spec = false;
  m_rand = false;
  m_wide = false;
  m_partonStoreMinPt = 0.;
  m_vertexOffsetCut = 0.;
  m_randomseed = 0;
  m_events = 0;
}


StatusCode Baur::genInitialize() {
  ATH_MSG_INFO("===> Nov.11,2007 Baur_i  INITIALISING.  \n");
  return StatusCode::SUCCESS;
}


StatusCode Baur::callGenerator() {
  ATH_MSG_INFO(" Baur generating.+++++++++  \n");
  baurzgammamain_();
  return StatusCode::SUCCESS;
}


StatusCode Baur::genFinalize() {
  ATH_MSG_INFO(" Baur Ending.  \n");
  return StatusCode::SUCCESS;
}


StatusCode Baur::fillEvt(HepMC::GenEvent*) {
  return StatusCode::SUCCESS;
}
