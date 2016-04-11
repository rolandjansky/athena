/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ISF_FatrasEventTPCnv/test/PlanarClusterCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarClusterCnv_p2.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include <cassert>
#include <iostream>


struct IDHelpers
{
  PixelID* m_pix_id;
  SCT_ID* m_sct_id;
  Identifier m_pixel_ids[1];
  std::unique_ptr<iFatras::PlanarDetElement> m_pixel_elts[1];
  Identifier m_sct_ids[1];
  std::unique_ptr<iFatras::PlanarDetElement> m_sct_elts[1];
};


void compare (const InDet::SiWidth& p1,
              const InDet::SiWidth& p2)
{
  assert (p1.colRow() == p2.colRow());
  assert (p1.widthPhiRZ() == p2.widthPhiRZ());
}


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.localPosition() == p2.localPosition());
  assert (p1.rdoList() == p2.rdoList());
  assert (p1.localCovariance() == p2.localCovariance());
}


void compare (const iFatras::PlanarCluster& p1,
              const iFatras::PlanarCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  //assert (p1.globalPosition() == p2.globalPosition());
  //assert (p1.detectorElement() == p2.detectorElement());
  compare (p1.width(), p2.width());
}


void testit (const IDHelpers& helpers,
             const iFatras::PlanarCluster& trans1,
             const Identifier& wafer_id)
{
  MsgStream log (0, "test");
  PlanarClusterCnv_p2 cnv (helpers.m_pix_id, helpers.m_sct_id);
  iFatras::PlanarCluster_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  iFatras::PlanarCluster trans2 (wafer_id,
                                 Amg::Vector2D(),
                                 std::vector<Identifier>(),
                                 InDet::SiWidth(),
                                 nullptr,
                                 static_cast<const Amg::MatrixX*>(nullptr));
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 (const IDHelpers& helpers)
{
  std::cout << "test1\n";

  Identifier wafer_id = helpers.m_pixel_ids[0];

  Amg::Vector2D locpos (1.5, 2.5);
  std::vector<Identifier> rdoList {
      helpers.m_pix_id->pixel_id (wafer_id,
                                  21,
                                  32),
      helpers.m_pix_id->pixel_id (wafer_id,
                                  21,
                                  33),
      helpers.m_pix_id->pixel_id (wafer_id,
                                  21,
                                  34) };
  InDet::SiWidth width (Amg::Vector2D (1, 2),
                        Amg::Vector2D (3.5, 4.5));

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  iFatras::PlanarCluster trans1 (helpers.m_pix_id->pixel_id (wafer_id,
                                                             21,
                                                             31),
                                 locpos,
                                 rdoList,
                                 width,
                                 helpers.m_pixel_elts[0].get(),
                                 new Amg::MatrixX(cov));
  testit (helpers, trans1, wafer_id);
}


void test2 (const IDHelpers& helpers)
{
  std::cout << "test2\n";

  Identifier wafer_id = helpers.m_sct_ids[0];

  Amg::Vector2D locpos (1.5, 2.5);
  std::vector<Identifier> rdoList {
      helpers.m_sct_id->strip_id (wafer_id, 42),
      helpers.m_sct_id->strip_id (wafer_id, 43),
      helpers.m_sct_id->strip_id (wafer_id, 44) };
  InDet::SiWidth width (Amg::Vector2D (11, 12),
                        Amg::Vector2D (13.5, 14.5));

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1) + 1.5;

  iFatras::PlanarCluster trans2 (helpers.m_sct_id->strip_id (wafer_id, 41),
                                 locpos,
                                 rdoList,
                                 width,
                                 helpers.m_sct_elts[0].get(),
                                 new Amg::MatrixX(cov));
  testit (helpers, trans2, wafer_id);
}


std::unique_ptr<iFatras::PlanarDetElement> makeEle(const Identifier& id,
                                                   const IdentifierHash& idhash,
                                                   bool isPixel,
                                                   int o)
{
  Amg::Transform3D xform;
  if (isPixel)
    xform = Amg::getRotateX3D (0.5+o/100);
  else
    xform = Amg::getRotateY3D (0.5+o/100);
  return CxxUtils::make_unique<iFatras::PlanarDetElement>
    (id,
     idhash,
     Amg::Vector3D (10.5+o, 11.5+o, 12.5+o),
     xform,
     Trk::HomogeneousLayerMaterial(),
     13.5+o, // thickness
     14.5+o, // lengthY
     15.5+o, // lengthXmin
     16.5+o, // lengthXmax
     17.5+o, // rMin
     18.5+o, // rMax
     19.5+o, // pitchX
     20.5+o, // pitchY
     21.5+o, // stereo
     isPixel,
     o&1,
     !(o&1),
     o&1);
}


std::unique_ptr<IDHelpers> make_idhelpers (ISvcLocator* svcLoc)
{
  auto helpers = CxxUtils::make_unique<IDHelpers>();
  auto pix_id = CxxUtils::make_unique<PixelID>();
  auto sct_id = CxxUtils::make_unique<SCT_ID>();
  helpers->m_pix_id = pix_id.get();
  helpers->m_sct_id = sct_id.get();

  IdDictParser parser;
  parser.register_external_entity ("InnerDetector",
                                   "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  pix_id->initialize_from_dictionary (idd);
  sct_id->initialize_from_dictionary (idd);

  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("DetectorStore", sg).isSuccess() );
  assert ( sg->record (std::move (pix_id), "PixelID") );
  assert ( sg->record (std::move (sct_id), "SCT_ID") );

  {
    Identifier id = helpers->m_pix_id->wafer_id (0,
                                                 1,
                                                 3,
                                                 4);
    IdentifierHash hash = helpers->m_pix_id->wafer_hash (id);
    std::unique_ptr<iFatras::PlanarDetElement> elt = makeEle(id, hash, true, 0);
    helpers->m_pixel_ids[0] = id;
    helpers->m_pixel_elts[0] = std::move(elt);
  }
  
  {
    Identifier id = helpers->m_sct_id->wafer_id (0,
                                                 1,
                                                 3,
                                                 4,
                                                 0);
    IdentifierHash hash = helpers->m_sct_id->wafer_hash (id);
    std::unique_ptr<iFatras::PlanarDetElement> elt = makeEle (id, hash,
                                                              false, 50);
    helpers->m_sct_ids[0] = id;
    helpers->m_sct_elts[0] = std::move(elt);
  }
  
  return helpers;
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("ISF_FatrasEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  std::unique_ptr<IDHelpers> helpers = make_idhelpers (pSvcLoc);
  test1(*helpers);
  test2(*helpers);
  return 0;
}
