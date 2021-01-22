/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitIdHelper.h"

// Default consdtructor needed by athenaroot
//
SiHit::SiHit( ) :
  m_stX(0.),
  m_stY(0.),
  m_stZ(0.),
  m_enX(0.),
  m_enY(0.),
  m_enZ(0.),
  m_energyLoss(0.),
  m_meanTime(0.),
  m_partLink(),
  m_ID(0xffff)
{

}

SiHit::~SiHit() {}


// Constructor
SiHit::SiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const int trackNumber,
             const unsigned int id) :
  //  m_localStartPosition(localStartPosition),
  //  m_localEndPosition(localEndPosition),
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(trackNumber),
  m_ID(id)
{
}

// Constructor
SiHit::SiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const int trackNumber,
             const int Part, const int BrlECap, const int LayerDisk,
             const int etaM, const int phiM, const int side) :
  //  m_localStartPosition(localStartPosition),
  //  m_localEndPosition(localEndPosition),
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(trackNumber),
  m_ID(0)
{
  // Compress the location info into the integer:
  m_ID =  SiHitIdHelper::GetHelper()->buildHitId(Part,BrlECap,LayerDisk,etaM,phiM,side);
}

// Constructor
SiHit::SiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const HepMcParticleLink &track,
             const unsigned int id) :
  //  m_localStartPosition(localStartPosition),
  //  m_localEndPosition(localEndPosition),
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(track),
  m_ID(id)
{
}

// Constructor
SiHit::SiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const HepMcParticleLink &track,
             const int Part, const int BrlECap, const int LayerDisk,
             const int etaM, const int phiM, const int side) :
  //  m_localStartPosition(localStartPosition),
  //  m_localEndPosition(localEndPosition),
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(track),
  m_ID(0)
{
  // Compress the location info into the integer:
  m_ID =  SiHitIdHelper::GetHelper()->buildHitId(Part,BrlECap,LayerDisk,etaM,phiM,side);
}

void SiHit::ScaleLength(double sfactor) {
  //  m_localStartPosition *= sfactor;
  //  m_localEndPosition   *= sfactor;
  m_stX *=  (float) sfactor;
  m_stY *=  (float) sfactor;
  m_stZ *=  (float) sfactor;
  m_enX *=  (float) sfactor;
  m_enY *=  (float) sfactor;
  m_enZ *=  (float) sfactor;
}

bool SiHit::isPixel() const {
  return  SiHitIdHelper::GetHelper()->isPixel(m_ID);
}

bool SiHit::isSCT() const {
  return  SiHitIdHelper::GetHelper()->isSCT(m_ID);
}

bool SiHit::isHGTD() const {
  return  SiHitIdHelper::GetHelper()->isHGTD(m_ID);
}

HepGeom::Point3D<double> SiHit::localStartPosition() const
{
  //  return m_localEndPosition;
  return HepGeom::Point3D<double>((double) m_stX, (double) m_stY, (double) m_stZ);
}

HepGeom::Point3D<double> SiHit::localEndPosition() const
{
  //  return m_localEndPosition;
  return HepGeom::Point3D<double>((double) m_enX, (double) m_enY, (double) m_enZ);
}


int SiHit::getBarrelEndcap() const {
  return  SiHitIdHelper::GetHelper()->getBarrelEndcap(m_ID);
}

int SiHit::getLayerDisk() const {
  return  SiHitIdHelper::GetHelper()->getLayerDisk(m_ID);
}

int SiHit::getEtaModule() const {
  return  SiHitIdHelper::GetHelper()->getEtaModule(m_ID);
}

int SiHit::getPhiModule() const {
  return  SiHitIdHelper::GetHelper()->getPhiModule(m_ID);
}

int SiHit::getSide() const {
  return  SiHitIdHelper::GetHelper()->getSide(m_ID);
}

void SiHit::print() const {
  if (isPixel() == true) {
    std::cout << "*** Pixel Hit " << std::endl;
  } else {
    std::cout << "*** SCT Hit " << std::endl;
  }
  std::cout << "          Barrel/ EndCap Number " << getBarrelEndcap() << std::endl;
  std::cout << "          Layer/Disk Number     " << getLayerDisk() << std::endl;
  std::cout << "          Eta Number            " << getEtaModule() << std::endl;
  std::cout << "          Phi Number            " << getPhiModule() << std::endl;
  std::cout << "          Side Number            " << getSide() << std::endl;
}



int SiHit::trackNumber() const
{
  return m_partLink.barcode();
}
