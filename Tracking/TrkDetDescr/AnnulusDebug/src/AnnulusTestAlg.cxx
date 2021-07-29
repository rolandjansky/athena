/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AnnulusDebug/AnnulusTestAlg.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrkSurfaces/AnnulusBounds.h"
#include "TrkSurfaces/AnnulusBoundsPC.h"
#include "TrkSurfaces/BoundaryCheck.h"

#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"

#include <sstream>
#include <fstream>
#include <random>

// preprocessor macros so that stringify (#) can be used
// PRINT_VECTOR and DUMP allow for a flexible output to any ATH_MSG stream - see lines 99-109
// ATH_MSG streams are always used in the end


#define CHECK_TRUE(x, msg) do { \
  if ((x) != true) { \
    ATH_MSG_ERROR("Assertion " << #x << " failed (l" << __LINE__ << "): " << msg); \
    return StatusCode::FAILURE; \
  } \
} while(0)

#define CHECK_APPROX(a, b, tol) do { \
  std::stringstream msg; \
  msg << "Failure: " << #a << " (=" << a << ") != " << #b << " (=" << b << ") within " << #tol; \
  msg << " : "; \
  CHECK_TRUE((std::abs(a-b) < tol), msg.str()); \
  } while (0)

#define PRINT_VECTOR2D(v) #v << "(l0=" << v.x() << ", l1=" << v.y() << ")"
#define PRINT_VECTOR3D(v) #v << "(x=" << v.x() << ", y=" << v.y() << ", z=" << v.z() << ")"
#define DUMP(v) #v << " = " << v

AnnulusTestAlg::AnnulusTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm (name, pSvcLocator)
{

}

AnnulusTestAlg::~AnnulusTestAlg(void)
{}

StatusCode
AnnulusTestAlg::initialize()
{
  ATH_MSG_INFO( "AnnulusTestAlg::initialize(). " );
  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::execute()
{
  ATH_MSG_VERBOSE ("AnnulusTestAlg::execute()");

  ATH_CHECK( testInsideDistances() );
  ATH_CHECK( visualInsideTolerance() );
  ATH_CHECK( visualInsideCovariance() );
  ATH_CHECK( testConsistencyCartesian() );
  ATH_CHECK( testBoundsOnSurface() );

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::testInsideDistances()
{
  ATH_MSG_INFO(name() << "::" << __FUNCTION__);

#include "AnnulusBoundsPCTestData.h"

  if (asymT1TestPoints.size() != asymT1ClosestPoints.size()
      || asymT1TestPoints.size() != asymT1ActDistances.size()
      || asymT1TestPoints.size() != asymT1ActInsides.size()) {
    ATH_MSG_ERROR("Test data invalid");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE(asymT1Ab);

  for (size_t i=0;i<asymT1TestPoints.size();i++) {
    const Amg::Vector2D& testPointXY = asymT1TestPoints.at(i);
    Amg::Vector2D testPointPC(testPointXY.perp(), testPointXY.phi());
    //const Amg::Vector2D& closestPointXY = asymT1ClosestPoints.at(i);
    double expDistance = asymT1ActDistances.at(i);
    bool expInside = asymT1ActInsides.at(i);

    bool actInside = asymT1Ab.inside(testPointPC);

    double actDistance = asymT1Ab.minDistance(testPointPC);

    // MsgStream  log(msgSvc(),name());
    auto dumpContext = [&,this](auto level) {

      this->msg(level) << "--- Begin test context----" << endmsg;
      this->msg(level) << "idx = " << i << endmsg;
      this->msg(level) << PRINT_VECTOR2D(testPointPC) << endmsg;
      this->msg(level) << DUMP(expInside) << endmsg;
      this->msg(level) << DUMP(actInside) << endmsg;
      this->msg(level) << DUMP(expDistance) << endmsg;
      this->msg(level) << DUMP(actDistance) << endmsg;
      this->msg(level) << "--- End test context----" << endmsg;
    };

    dumpContext(MSG::VERBOSE);

    CHECK_TRUE(actInside == expInside, "Inside check mismatch at idx " << i);
    double distDelta = std::abs(actDistance-expDistance);
    double failThresh = 0.01;

    if (distDelta > failThresh) {
      dumpContext(MSG::ERROR);
    }

    CHECK_TRUE(distDelta < failThresh, "idx " << i);
    bool warnThresh = 0.001;
    if (std::abs(actDistance - expDistance) > warnThresh) {
      ATH_MSG_WARNING("Distances are more than " << warnThresh << " apart, idx: " << i);
      ATH_MSG_WARNING("expDistance = " << expDistance << " actDistance = " << actDistance
          << " delta = " << std::abs(actDistance - expDistance));
    }


  }

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::visualInsideTolerance()
{
  std::mt19937 gen(42);
  std::uniform_real_distribution<double> x(0.1, 5);
  std::uniform_real_distribution<double> y(-5, 5);
  std::ofstream vis_tol12("vis_tol12.csv");

  vis_tol12 << "x;y;insideAbs;insideTol;insideLoc1Abs;"
            << "insideLoc1Tol;insideLoc2Abs;insideLoc2Tol"
            << "\n";


  double tol1 = 0.2, tol2 = 0.3;
  Trk::BoundaryCheck bcLoc12Abs(true, true, 0, 0);
  Trk::BoundaryCheck bcLoc12Tol(true, true, tol1, tol2);
  Trk::BoundaryCheck bcLoc1Abs(true, false, 0, 0);
  Trk::BoundaryCheck bcLoc1Tol(true, false, tol1, 0);
  Trk::BoundaryCheck bcLoc2Abs(false, true, 0, 0);
  Trk::BoundaryCheck bcLoc2Tol(false, true, 0, tol2);

#include "AnnulusBoundsPCTestData.h"

  size_t n = 1e6;
  for (size_t i=0;i<n;i++) {
    Amg::Vector2D testPointXY(x(gen), y(gen));
    Amg::Vector2D testPointPC(testPointXY.perp(), testPointXY.phi());
    vis_tol12 << testPointXY.x() << ";" << testPointXY.y() << ";";

    bool insideAbs = asymT1Ab.inside(testPointPC, 0, 0);
    bool insideTol = asymT1Ab.inside(testPointPC, tol1, tol2);

    bool insideLoc1Abs = asymT1Ab.insideLoc1(testPointPC, 0);
    bool insideLoc1Tol = asymT1Ab.insideLoc1(testPointPC, tol1);

    bool insideLoc2Abs = asymT1Ab.insideLoc2(testPointPC, 0);
    bool insideLoc2Tol = asymT1Ab.insideLoc2(testPointPC, tol2);

    // internal consistency
    CHECK_TRUE(insideAbs == asymT1Ab.inside(testPointPC, bcLoc12Abs), "idx " << i);
    CHECK_TRUE(insideTol == asymT1Ab.inside(testPointPC, bcLoc12Tol), "idx " << i);
    CHECK_TRUE(insideLoc1Abs == asymT1Ab.inside(testPointPC, bcLoc1Abs), "idx " << i);
    CHECK_TRUE(insideLoc1Tol == asymT1Ab.inside(testPointPC, bcLoc1Tol), "idx " << i);
    CHECK_TRUE(insideLoc2Abs == asymT1Ab.inside(testPointPC, bcLoc2Abs), "idx " << i);
    CHECK_TRUE(insideLoc2Tol == asymT1Ab.inside(testPointPC, bcLoc2Tol), "idx " << i);

    vis_tol12 << insideAbs << ";" << insideTol << ";"
              << insideLoc1Abs << ";" << insideLoc1Tol << ";"
              << insideLoc2Abs << ";" << insideLoc2Tol;


    vis_tol12 << "\n";
  }

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::visualInsideCovariance()
{

  std::mt19937 gen(43);
  std::uniform_real_distribution<double> x(0.1, 5);
  std::uniform_real_distribution<double> y(-5, 5);

  std::ofstream vis_cov("vis_cov.csv");
  vis_cov << "x;y;inside;insideCov;side" << "\n";
  Eigen::Matrix<double, 2, 2> cov;
  cov << .125, 0,
         0, .125;
  int nSigmas = 1;

  Trk::BoundaryCheck bcCov(cov, nSigmas);

#include "AnnulusBoundsPCTestData.h"

  size_t n = 1e6;
  for (size_t i=0;i<n;i++) {
    Amg::Vector2D testPointXY(x(gen), y(gen));
    Amg::Vector2D testPointPC(testPointXY.perp(), testPointXY.phi());

    bool insideAbs = asymT1Ab.inside(testPointPC, 0, 0);

    bool insideCov = asymT1Ab.inside(testPointPC, bcCov);

    vis_cov << testPointXY.x() << ";" << testPointXY.y() << ";";
    vis_cov << insideAbs << ";"
            << insideCov << ";" << 0 ;

    vis_cov << "\n";

  }
  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::testConsistencyCartesian()
{

  // COMPARE TO CURRENT IMPLEMENTATION
  std::ofstream vis_cmp("vis_cmp.csv");
  vis_cmp << "x;y;insideRef;inside" << std::endl;

  Trk::AnnulusBounds refAnnulus(5, 10, 2, 0.5, 0.1);
  auto res = Trk::AnnulusBoundsPC::fromCartesian(refAnnulus);
  Trk::AnnulusBoundsPC& annulusPC = res.first;
  double phiShift = res.second;
  // extract shift parameter.
  // note: origin was rotated into internal coordinate system, need to reverse
  // rotation here
  Eigen::Transform<double, 2, Eigen::Affine> trfModToStrip;
  trfModToStrip.setIdentity();
  Eigen::Translation<double, 2> transl(annulusPC.moduleOrigin());
  
  Eigen::Rotation2D<double> rot(-phiShift);

  trfModToStrip = transl * rot;
  std::mt19937 gen(44);
  std::uniform_real_distribution<double> xCmp(-15, 15);
  std::uniform_real_distribution<double> yCmp(-15, 15);

  size_t n = 1e6;
  for(size_t i=0;i<n;i++) {
    Amg::Vector2D testPointXYMod(xCmp(gen), yCmp(gen));
    Amg::Vector2D testPointXYStrip = trfModToStrip * testPointXYMod;

    Amg::Vector2D testPointPCStrip(testPointXYStrip.perp(), testPointXYStrip.phi());

    bool refInside = refAnnulus.inside(testPointXYMod);
    bool pcInside = annulusPC.inside(testPointPCStrip);

    CHECK_TRUE(refInside == pcInside, "idx = " << i);

    vis_cmp << testPointXYMod.x() << ";"
            << testPointXYMod.y() << ";"
            << refInside << ";"
            << pcInside << "\n";
  }

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::testBoundsOnSurface()
{
  std::mt19937 gen(45);
  std::uniform_real_distribution<double> rotDist(0, 2*M_PI);
  std::uniform_real_distribution<double> transDist(-5, 5);
  std::uniform_real_distribution<double> longDist(-50, 50);

  std::uniform_real_distribution<double> rminDist(2, 20);
  std::uniform_real_distribution<double> drDist(1, 10);
  std::uniform_real_distribution<double> phiDist(0, M_PI/2);
  std::uniform_real_distribution<double> phiSDist(0, 0.3);


  std::ofstream vis_cmp_srf("vis_cmp_srf.csv");
  vis_cmp_srf << "nr;locx;locy;globx;globy;globz;locInsideRef;locInsideAct;onSurfaceRef;onSurfaceAct;phiS;Ox;Oy;ref_corners;act_corners" << std::endl;

  size_t nr = 100;
  for(size_t j=0;j<nr;j++) {

    double rMin = rminDist(gen);
    double rMax = rMin + drDist(gen);
    double phi = phiDist(gen);
    double phiS = phiSDist(gen);

    Trk::AnnulusBounds* refAnnulus = new Trk::AnnulusBounds(rMin, rMax, (rMin+rMax)/2., phi, phiS);

    // determine search box based on radii
    std::uniform_real_distribution<double> searchDist(-rMax-1, rMax+1);

    Amg::Transform3D* planeTrf;
    planeTrf = new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0, 0, 10)));
    Amg::Transform3D rotX(Amg::AngleAxis3D(rotDist(gen), Amg::Vector3D::UnitX()));
    Amg::Transform3D rotY(Amg::AngleAxis3D(rotDist(gen), Amg::Vector3D::UnitY()));
    Amg::Transform3D rotZ(Amg::AngleAxis3D(rotDist(gen), Amg::Vector3D::UnitZ()));
    Amg::Transform3D transX(Amg::Translation3D(transDist(gen) * Amg::Vector3D::UnitX()));
    Amg::Transform3D transY(Amg::Translation3D(transDist(gen) * Amg::Vector3D::UnitY()));
    Amg::Transform3D transZ(Amg::Translation3D(longDist(gen) * Amg::Vector3D::UnitZ()));
    *planeTrf = (*planeTrf) * transX * transY * transZ * rotX * rotY * rotZ;

    Trk::PlaneSurface planeSrf(planeTrf, refAnnulus);

    std::stringstream ref_corners_ss;
    bool first = true;
    for(const auto& corner : refAnnulus->corners()) {
      if(!first) ref_corners_ss << ",";
      Amg::Vector2D corner_loc(corner.first, corner.second);
      //Amg::Vector3D corner_glob = planeSrf.transform() * corner_loc;
      //ref_corners_ss << corner.first << "," << corner.second;
      Amg::Vector3D corner_glob;
      planeSrf.localToGlobal(corner_loc, Amg::Vector3D(), corner_glob);
      //ref_corners_ss << corner_glob.x() << "," << corner_glob.y() << "," << corner_glob.z();
      ref_corners_ss << corner_loc[0] << "," << corner_loc[1] << ",0";
      first = false;
    }



    // now construct DiscSurface from same transform and bounds
    // The constructor will handle the conversion into AnnulusBoundsPC
    Trk::DiscSurface discSrf(new Amg::Transform3D(*planeTrf), refAnnulus->clone());
    const Trk::AnnulusBoundsPC* annbopc = dynamic_cast<const Trk::AnnulusBoundsPC*>(&discSrf.bounds());
  
    auto res = Trk::AnnulusBoundsPC::fromCartesian(*refAnnulus);
    Trk::AnnulusBoundsPC& annbopc_loc = res.first;
    double phiShift_loc = res.second;
    Eigen::Transform<double, 2, Eigen::Affine> trfModToStrip;
    trfModToStrip.setIdentity();
    Eigen::Translation<double, 2> transl(annbopc_loc.moduleOrigin());
    Eigen::Rotation2D<double> rot(-phiShift_loc);
    //trfModToStrip = transl * rot;
    trfModToStrip = Eigen::Transform<double, 2, Eigen::Affine>::Identity();
    
    std::stringstream act_corners_ss;
    first = true;
    for(const auto& corner : annbopc->corners()) {
      if(!first) act_corners_ss << ",";
      double x = corner.first * std::cos(corner.second);
      double y = corner.first * std::sin(corner.second);
      Amg::Vector3D corner_glob;
      Amg::Vector2D corner_loc(corner.first, corner.second);
      discSrf.localToGlobal(corner_loc, Amg::Vector3D(), corner_glob);
      //act_corners_ss << corner_glob.x() << "," << corner_glob.y() << "," << corner_glob.z();
      act_corners_ss << x << "," << y << ",0";
      first = false;
    }

    // throw points on the surface
    size_t n = 1e4;
    for(size_t i=0;i<n;i++) {
      Amg::Vector3D testPointXYLocal(searchDist(gen), searchDist(gen), 0);
      Amg::Vector3D testPointXYGlobal = (*planeTrf) * testPointXYLocal;

      Amg::Vector2D testPointXYMod(testPointXYLocal.x(), testPointXYLocal.y());
      Amg::Vector2D testPointXYStrip 
        = trfModToStrip * testPointXYMod;
      Amg::Vector2D testPointPCStrip(testPointXYStrip.perp(), testPointXYStrip.phi());

      vis_cmp_srf << j << ";";
      vis_cmp_srf << testPointXYLocal.x() << ";"
                  << testPointXYLocal.y() << ";";
      vis_cmp_srf << testPointXYGlobal.x() << ";"
                  << testPointXYGlobal.y() << ";"
                  << testPointXYGlobal.z() << ";";

      vis_cmp_srf << refAnnulus->inside(testPointXYMod) << ";"
                  << annbopc_loc.inside(testPointPCStrip) << ";";

      bool onSurfaceRef = planeSrf.isOnSurface(testPointXYGlobal);
      bool onSurfaceAct = discSrf.isOnSurface(testPointXYGlobal);

      CHECK_TRUE(onSurfaceRef == onSurfaceAct, "idx = " << i);

      vis_cmp_srf << onSurfaceRef << ";" << onSurfaceAct;

      Amg::Vector2D mod_orig = annbopc->moduleOrigin();
      vis_cmp_srf << ";" << phiS << ";" << mod_orig.x() << ";" << mod_orig.y();
      vis_cmp_srf << ";" << ref_corners_ss.str();
      vis_cmp_srf << ";" << act_corners_ss.str();
      vis_cmp_srf << "\n";
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
AnnulusTestAlg::finalize()
{
  ATH_MSG_VERBOSE ("AnnulusTestAlg::finalize()");
  return StatusCode::SUCCESS;
}

