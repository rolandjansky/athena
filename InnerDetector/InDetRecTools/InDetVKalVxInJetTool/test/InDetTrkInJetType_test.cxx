/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file InDetVKalVxInJetTool/test/InDetTrkInJetType_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for InDetTrkInJetType.
 */


#undef NDEBUG
#include "InDetVKalVxInJetTool/InDetTrkInJetType.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


#include "CLHEP/Vector/LorentzVector.h"

using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


std::unique_ptr<AmgSymMatrix(5)> cov5()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setIdentity();
  return m;
}


// Copied from TrackParticleCreatorTool.
void setDefiningParameters( xAOD::TrackParticle& tp,
                            const Trk::Perigee& perigee )
{
  tp.setDefiningParameters(perigee.parameters()[Trk::d0],
    perigee.parameters()[Trk::z0],
    perigee.parameters()[Trk::phi0],
    perigee.parameters()[Trk::theta],
    perigee.parameters()[Trk::qOverP]);
  const AmgSymMatrix(5)* covMatrix = perigee.covariance();
  // see https://its.cern.ch/jira/browse/ATLASRECTS-645 for justification to comment out the following line 
  // assert(covMatrix && covMatrix->rows()==5&& covMatrix->cols()==5); 
  std::vector<float> covMatrixVec;
  if( !covMatrix ) ;//ATH_MSG_WARNING("Setting Defining parameters without error matrix");
  else Amg::compress(*covMatrix,covMatrixVec);
  tp.setDefiningParametersCovMatrixVec(covMatrixVec);
  const Amg::Vector3D& surfaceCenter = perigee.associatedSurface().center(); 
  tp.setParametersOrigin(surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z() );
}


xAOD::TrackParticle makeTP()
{
  Amg::Vector3D pos0 { 0, 0, 0 };
  Amg::Vector3D pos { 2*mm, 0*mm, -10*mm };
  Amg::Vector3D mom { 50*GeV, 3*GeV, 2*GeV };

  Trk::Perigee p (pos, mom, 1, pos0, cov5().release());

  xAOD::TrackParticle tp;
  tp.makePrivateStore();
  setDefiningParameters (tp, p);
  tp.setFitQuality (7, 3);
  uint8_t pixelHits = 3;
  tp.setSummaryValue (pixelHits, xAOD::numberOfPixelHits);
  uint8_t sctHits = 7;
  tp.setSummaryValue (sctHits, xAOD::numberOfSCTHits);

  return tp;
}


void test1 (InDet::IInDetTrkInJetType& tool)
{
  std::cout << "test1\n";

  xAOD::TrackParticle tp = makeTP();

  xAOD::Vertex pv;
  pv.makePrivateStore();
  pv.setPosition ({0, 0, -9*mm});

  TLorentzVector j (70*GeV, 1*GeV, 10*GeV, 100*GeV);

  std::vector<float> out = tool.trkTypeWgts (&tp, pv, j);
  assert( out.size() == 3 );
  assert( Athena_test::isEqual (out[0], 0.0946472, 1e-5) );
  assert( Athena_test::isEqual (out[1], 0.479743, 1e-5) );
  assert( Athena_test::isEqual (out[2], 0.42561, 1e-5) );
}


int main()
{
  std::cout << "InDetVKalVxInJetTool/InDetTrkInJetType_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("InDetVKalVxInJetTool/InDetVKalVxInJetTool_tests.txt", svcloc);

  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  ToolHandle<InDet::IInDetTrkInJetType> tool ("InDet::InDetTrkInJetType");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
