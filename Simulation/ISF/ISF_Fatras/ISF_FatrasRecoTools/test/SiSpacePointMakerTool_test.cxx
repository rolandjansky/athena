#undef NDEBUG

#include "ISF_FatrasRecoTools/ISF_SiSpacePointMakerTool.h"
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "ISF_FatrasEvent/PlanarClusterCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


const int NELTS = 10;

struct IDHelpers
{
  PixelID* m_pix_id;
  SCT_ID* m_sct_id;
  Identifier m_pixel_ids;
  std::unique_ptr<iFatras::PlanarDetElement> m_pixel_elts;
  Identifier m_sct_ids;
  std::unique_ptr<iFatras::PlanarDetElement> m_sct_elts;
};

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
  return std::make_unique<iFatras::PlanarDetElement>
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
  auto helpers = std::make_unique<IDHelpers>();
  auto pix_id = std::make_unique<PixelID>();
  auto sct_id = std::make_unique<SCT_ID>();
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

  auto pixel_map = std::make_unique<iFatras::IdHashDetElementCollection>();
  {
    Identifier id = helpers->m_pix_id->wafer_id (0,
                                                 1,
                                                 3,
                                                 4);
    IdentifierHash hash = helpers->m_pix_id->wafer_hash (id);
    std::unique_ptr<iFatras::PlanarDetElement> elt = makeEle(id, hash,
                                                             true, 0);
    pixel_map->emplace (hash, elt.get());
    helpers->m_pixel_ids = id;
    helpers->m_pixel_elts = std::move(elt);
  }
  
  auto sct_map = std::make_unique<iFatras::IdHashDetElementCollection>();
  {
    Identifier id = helpers->m_sct_id->wafer_id (0,
                                                 1,
                                                 3,
                                                 4,
                                                 0);
    IdentifierHash hash = helpers->m_sct_id->wafer_hash (id);
    std::unique_ptr<iFatras::PlanarDetElement> elt = makeEle (id, hash,
                                                              false, 5);
    sct_map->emplace (hash, elt.get());
    helpers->m_sct_ids = id;
    helpers->m_sct_elts = std::move(elt);
  }
  
  return helpers;
}


void test (iFatras::ISF_SiSpacePointMakerTool& tool, const IDHelpers& helpers)
{
  std::cout << "test: "  << tool.name() << "\n";
  // Creation of PlanarClusterCollection is from PlanarClusterContainerCnv test (by Scott)
  iFatras::PlanarClusterContainer planarCont (3000);
  {
    Identifier wafer_id = helpers.m_pixel_ids;
    IdentifierHash wafer_hash = helpers.m_pix_id->wafer_hash (wafer_id);
    auto coll = std::make_unique<iFatras::PlanarClusterCollection> (wafer_hash);
    for (int i=0; i < NELTS; i++) {
      int o = i*10;

      Amg::Vector2D locpos (1.5+o, 2.5+o);
      std::vector<Identifier> rdoList {
        helpers.m_pix_id->pixel_id (wafer_id,
                                    21+o,
                                    32+o),
        helpers.m_pix_id->pixel_id (wafer_id,
                                    21+o,
                                    33+o),
        helpers.m_pix_id->pixel_id (wafer_id,
                                    21+o,
                                    34+o) };
      InDet::SiWidth width (Amg::Vector2D (1+o, 2+o),
                            Amg::Vector2D (3.5+o, 4.5+o));

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + o;

      auto clus = std::make_unique<iFatras::PlanarCluster>
        (helpers.m_pix_id->pixel_id (wafer_id,
                                     21+o,
                                     31+o),
         locpos,
         rdoList,
         width,
         helpers.m_pixel_elts.get(),
         new Amg::MatrixX(cov));

      coll->push_back (std::move (clus));
    }
    planarCont.addCollection (coll.release(), wafer_hash).ignore();
  }

  {
    Identifier wafer_id = helpers.m_sct_ids;
    IdentifierHash wafer_hash = helpers.m_sct_id->wafer_hash (wafer_id);
    auto coll = std::make_unique<iFatras::PlanarClusterCollection> (wafer_hash);
    for (int i=0; i < NELTS; i++) {
      int o = i*10+5;

      Amg::Vector2D locpos (1.5+o, 2.5+o);
      std::vector<Identifier> rdoList {
        helpers.m_sct_id->strip_id (wafer_id, 42+o),
        helpers.m_sct_id->strip_id (wafer_id, 43+o),
        helpers.m_sct_id->strip_id (wafer_id, 44+o) };
      InDet::SiWidth width (Amg::Vector2D (1+o, 2+o),
                            Amg::Vector2D (3.5+o, 4.5+o));

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + o;

      auto clus = std::make_unique<iFatras::PlanarCluster>
        (helpers.m_sct_id->strip_id (wafer_id, 41+i),
         locpos,
         rdoList,
         width,
         helpers.m_sct_elts.get(),
         new Amg::MatrixX(cov));

      coll->push_back (std::move (clus));
    }
    planarCont.addCollection (coll.release(), wafer_hash).ignore();
  }

  iFatras::PlanarClusterContainer::const_iterator it1 = planarCont.begin();
  const iFatras::PlanarClusterCollection& coll1 = **it1;
  std::cout << "Collection size: " << coll1.size() << "\n";
  
  IdentifierHash idHash = it1->identifyHash(); 
  Identifier elementID = it1->identify(); 
  auto spacepointCollection = std::make_unique< SpacePointCollection >(idHash);
  spacepointCollection->setIdentifier(elementID); 
  tool.fillPixelSpacePointCollection(*it1, spacepointCollection.get());
  std::cout << "coll size  : " << (*it1)->size()  << "\n";
  std::cout << "space size  : " << spacepointCollection->size()  << "\n";

  assert( (*it1)->size() == spacepointCollection->size() );
  
}


int main()
{
  std::cout << "ISF_FatrasRecoTools/ISF_SiSpacePointMakerTool_test.cxx \n";


  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("ISF_FatrasRecoTools_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  std::unique_ptr<IDHelpers> helpers = make_idhelpers (pSvcLoc);

  ToolHandle<iFatras::ISF_SiSpacePointMakerTool> tool ("iFatras::ISF_SiSpacePointMakerTool");
  assert( tool.retrieve().isSuccess() );
  
  test (*tool, *helpers);

  return 0;
}
