/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEGEOMETRYREADER_H
#define STOREGATEGEOMETRYREADER_H

#include "StoreGate/StoreGate.h"
#include <string>
#include <vector>
#include <QColor>

//#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

class CaloDetDescrManager;
class CaloDetDescriptor;
class Identifier;
class TileDetDescrManager;
class LArDetectorManager;

namespace InDetDD
{
  class SiDetectorManager;
  class TRT_DetectorManager;
}

namespace MuonGM
{
  class MuonDetectorManager;
  class MuonStation;
}

class Detector
{
 public:
  enum detectorType
    {
      ABox, ATrd, ATBx,             //<--- Muon geometry
      AGeometryConstant,            //      ^
      AGeneralPath,                 //      |
      ADisc, ARectangle,            //      |
      ABarrelCalorimeter,           //      |
      AEndcapCalorimeter,           //      |
      AGapCalorimeter,              // Other Geometry
      ABarrelSiliconDetector,       //      |
      AEndcapSiliconDetector,       //      |
      ABarrelTRTDetector,           //      |
      AEndcapTRTDetector,           //      v
      AEndcapCryostat,              // I haven't seen this part anywhere outside the GeomeryJiveXML code ???
      ERROR_TYPE                    // Set this when tag is missing/error
    };

  Detector():name(""), type(ERROR_TYPE), zInner(0), zOuter(0), rInner(0), rOuter(0), eta(0), dphi(0),sh(0), RPCi(0), RPCo(0), w(0), zInnerS(0), zOuterS(0), wS(0), oR(0),
    wInner(0), wOuter(0), alpha(0), agc_value(0), projection(""), numPoints(0), xReflect(true), yReflect(true), nInner(256), nOuter(256), xMin(0), xMax(0), yMin(0), yMax(0),
    sampling(0), region(0), eta0(0), deta(0), neta(0), meta(0), nphi(0), phi0(0), rMin(0), rMax(0), zMin(0), zMax(0), layer(0), length(0), width(0), thickness(0), nz(0),
    tilt(0), r0(0), phiIn(0), phiOut(0)
    {
    }

  //elements common to ALL detectors
  std::string name;
  detectorType type;
  QColor color;

  //elements common to both ABox and ATrd
  double zInner;
  double zOuter;
  double rInner;   //also possessed by ADisc
  double rOuter;   //also possessed by ADisc
  int eta;
  std::vector<int>  phi;
  double dphi;
  double sh;

  //elements only in ABox+ATBx
  double RPCi;
  double RPCo;
  double w;

  //elements only in ATBx
  double zInnerS;
  double zOuterS;
  double wS;
  double oR;

  //elements only in ATrd
  double wInner;
  double wOuter;
  double alpha;

  //elements in AGeometryConstant
  double agc_value;

  //element common to AGeneralPath, ADisc, ARectangle
  std::string projection;

  //element only in AGeneralPath
  int numPoints;

  //element in AGeneralPath, ARectangle
  bool xReflect;
  bool yReflect;

  //elements in ADisc
  int nInner;
  int  nOuter;

  //elements in ARectangle
  double xMin;
  double xMax;
  double yMin;
  double yMax;

  //elements in A*Calorimeter
  int sampling;
  int region;
  double eta0;
  double deta; //no (d-m)eta (except "eta") in AGapCalorimeter
  int neta;
  int meta;

  //elements in A*Calorimeter, A*SiliconDetector, A*TRTDetector
  int nphi;
  double phi0; //no phi0 in ABarrelTRTDetector
  double rMin; //no rMin or rMax in ABarrelSiliconDetector
  double rMax;
  double zMin;
  double zMax;

  //elements in A*SiliconDetector, A*TRTDetector
  int layer;

  //elements in A*SiliconDetector
  double length;
  double width;
  double thickness;
  int nz;

  //elements in ABarrelSiliconDetector
  double tilt;
  double r0;

  //elements in ABarrelTRTDetector
  double phiIn;
  double phiOut;

};

class StoreGateGeometryReader
{
 public:
  StoreGateGeometryReader();
  void setDetStore(StoreGateSvc *detStore) {m_detStore = detStore;}
  std::vector<Detector> mDetectors;
  bool readGeometry();
  bool readPixelGeometry();
  bool readSCTGeometry();
  bool readTRTGeometry();
  bool readSolenoidGeometry();
  bool readLArGeometry();
  bool readMBTSGeometry();
  bool readTILEGeometry();

  bool readToDiscElement();
  bool readToRectElement();
  bool readToRectanglishElement(); //wtf?

  bool readMuonGeometry();

 private:
  Detector *newDetector;

  /** Pixel detector manager. */
  const InDetDD::SiDetectorManager* m_pixel_manager;

  /** Silicon detector manager. */
  const InDetDD::SiDetectorManager* m_silicon_manager;

  /** TRT detector manager. */
  const InDetDD::TRT_DetectorManager* m_trt_manager;

  /** Calorimeter detector manager. */
  const CaloDetDescrManager* m_calo_manager;
  const TileDetDescrManager* m_tile_manager;
  const LArDetectorManager *m_lar_manager;

  /** The number of calorimeter sampling types. */
  static const int m_numCaloTypes = 24;

  std::vector<double> m_larMin;
  std::vector<double> m_larMax;

  //helper functions for Muon parsing
  Amg::Vector3D getPosition(const MuonGM::MuonStation *station, int maxPhi) const;

  double getDeltaPhi(const Amg::Vector3D &pos, int maxPhi) const;
  double getShift(const Amg::Vector3D &pos, double dphi) const;
  double getAlpha(const Amg::Transform3D &trans) const;
  std::vector<int> getPhiVectorFromString(std::string phiString);

  const MuonGM::MuonDetectorManager *m_muon_manager;
  static const double m_smallAngle, m_smallDistance;

  // Detector Store
  StoreGateSvc *m_detStore;

};

#endif
