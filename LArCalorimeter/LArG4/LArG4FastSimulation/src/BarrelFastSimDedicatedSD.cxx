/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBAccordionDetails.h"
#include "LArReadoutGeometry/GeoStraightAccSection.h"
#include "LArG4Code/EnergySpot.h"
#include "StoreGate/StoreGateSvc.h"

using HepGeom::Point3D;
using HepGeom::Transform3D;
using CLHEP::Hep3Vector;

// Constructor:
BarrelFastSimDedicatedSD::BarrelFastSimDedicatedSD(StoreGateSvc* detStore)
  : IFastSimDedicatedSD("BarrelFastSimDedicatedSD", detStore)
  , m_embManager(nullptr)
  , m_accordionDetails(nullptr)
  , m_absorberSections(nullptr)
{
  G4cout << GetName() << "::initialize()" << G4endl;
  if ( detStore->retrieve( m_embManager ).isFailure()  ){
    throw std::runtime_error("Could not retrieve EMB manager");
  }
  m_accordionDetails=m_embManager->getAccordionDetails();
  m_absorberSections=m_accordionDetails->getAbsorberSections();
}

// ProcessHitsMethod
void BarrelFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){

  // Fill the identifier.
  Point3D<double> globalPosition=spot.GetPosition();

  static Transform3D xfNeg = m_embManager->getDetectorRegion(0,0,0)->getAbsoluteTransform().inverse();
  static Transform3D xfPos = m_embManager->getDetectorRegion(1,0,0)->getAbsoluteTransform().inverse();

  Point3D<double> localPosition = globalPosition.z()<0 ? xfNeg*globalPosition : xfPos*globalPosition;
  int zIndex = globalPosition.z() <0 ? 0:1;
  double eta = localPosition.getEta();
  double phi = localPosition.getPhi();
  double r   = localPosition.perp();

  bool implementWaves=true;

  EMBDetectorManager::DetectorRegionConstIterator e;
  for (e=m_embManager->beginDetectorRegion();e!=m_embManager->endDetectorRegion();  e++) {
    const EMBDetectorRegion *region = *e;
    if (region->getEndcapIndex()!=zIndex) continue;
    const EMBDetDescr       *regionDescriptor=region->getDescriptor();
    const CellBinning       & etaBinning=regionDescriptor->getEtaBinning();
    if (eta>etaBinning.getStart() && eta<etaBinning.getEnd()) {
      unsigned int etaIndex = int((eta - etaBinning.getStart())/etaBinning.getDelta()) + etaBinning.getFirstDivisionNumber();
      unsigned int phiIndex = 0;
      EMBCellConstLink cellPtr = region->getEMBCell(etaIndex,phiIndex);
      double rmax = cellPtr->getRLocal(EMBCell::BACK);
      double rmin = cellPtr->getRLocal(EMBCell::FRONT);
      if (r>rmin && r<rmax) {
        const CellBinning       & phiBinning=regionDescriptor->getPhiBinning();
        double minPhi=std::min(phiBinning.getStart(), phiBinning.getEnd());
        double maxPhi=std::max(phiBinning.getStart(), phiBinning.getEnd());

        while (phi<minPhi)   phi += 2*M_PI;
        while (phi>maxPhi)   phi -= 2*M_PI;
        unsigned int regionIndex = region->getRegionIndex();
        unsigned int samplingIndex = region->getSamplingIndex();
        unsigned int phiIndex      =  int((phi - phiBinning.getStart())/phiBinning.getDelta()) + phiBinning.getFirstDivisionNumber();

        // Now get the nominal cell and see if the indices need to be corrected for the accordion waves:
        if (samplingIndex!=0) {
          EMBCellConstLink cellPtr=region->getEMBCell(etaIndex,phiIndex);

          double delta=-2.0*M_PI/1024/2;

          double phiLocalUpper=cellPtr->getPhiLocalUpper()-delta;
          double phiLocalLower=cellPtr->getPhiLocalLower()-delta;

          while (phiLocalLower<0)      phiLocalLower += 2.0*M_PI;
          while (phiLocalUpper<0)      phiLocalUpper += 2.0*M_PI;
          while (phiLocalLower>2*M_PI) phiLocalLower -= 2.0*M_PI;
          while (phiLocalUpper>2*M_PI) phiLocalUpper -= 2.0*M_PI;

          int accordionIndexUpper=int(phiLocalUpper/(2*M_PI)*1024+0.25); if (accordionIndexUpper==1024) accordionIndexUpper=0;
          int accordionIndexLower=int(phiLocalLower/(2*M_PI)*1024+0.25); if (accordionIndexLower==1024) accordionIndexLower=0;

          Point3D<double> A0     =
            ( Point3D<double>(m_absorberSections->XCent(accordionIndexLower,1),m_absorberSections->YCent(accordionIndexLower,1),0)+
              Point3D<double>(m_absorberSections->XCent(accordionIndexUpper,1),m_absorberSections->YCent(accordionIndexUpper,1),0))/2.0;

          Point3D<double> A1=
            (Point3D<double>(m_absorberSections->XCent(accordionIndexLower,12),m_absorberSections->YCent(accordionIndexLower,12),0)+
             Point3D<double>(m_absorberSections->XCent(accordionIndexUpper,12),m_absorberSections->YCent(accordionIndexUpper,12),0))/2.0-A0;

          Point3D<double> P0=Point3D<double>(localPosition.x(), localPosition.y(),0)-A0;
          int stackIndex=int(A1.dot(P0)/A1.mag2()*22.0 + 3.0)/2 ;

          if (stackIndex<0 || stackIndex>13) {
            G4cout << "Warning, bad stack index " << stackIndex << ' ' << rmin << ' ' << r << ' ' << A0.perp() << ' ' << A0 <<  ' ' << A1 << ' ' << P0 << G4endl;
            if (implementWaves) return;
          } else {
            double xcent      = m_absorberSections->XCent     (accordionIndexLower,stackIndex);
            double ycent      = m_absorberSections->YCent     (accordionIndexLower,stackIndex);
            double cosU       = m_absorberSections->Cosu      (accordionIndexLower,stackIndex);
            double sinU       = m_absorberSections->Sinu      (accordionIndexLower,stackIndex);
            double halfLength = m_absorberSections->HalfLength(accordionIndexLower,stackIndex);

            Point3D<double> u(xcent-halfLength*cosU, ycent-halfLength*sinU,0);
            Point3D<double> v(xcent+halfLength*cosU, ycent+halfLength*sinU,0);
            Point3D<double> x=v-u,y=Hep3Vector(localPosition.x(),localPosition.y(),0)-u;
            if ((zIndex==0 && x.cross(y).z()>0) || (zIndex==1&& x.cross(y).z()<0)) {
              if (implementWaves) {
                if (phiIndex==0) phiIndex=phiBinning.getNumDivisions()-1;
                else phiIndex--;
              }
            } else {
              double xcent      = m_absorberSections->XCent     (accordionIndexUpper,stackIndex);
              double ycent      = m_absorberSections->YCent     (accordionIndexUpper,stackIndex);
              double cosU       = m_absorberSections->Cosu      (accordionIndexUpper,stackIndex);
              double sinU       = m_absorberSections->Sinu      (accordionIndexUpper,stackIndex);
              double halfLength = m_absorberSections->HalfLength(accordionIndexUpper,stackIndex);

              Point3D<double> u(xcent-halfLength*cosU, ycent-halfLength*sinU,0);
              Point3D<double> v(xcent+halfLength*cosU, ycent+halfLength*sinU,0);
              Point3D<double> x=v-u,y=Hep3Vector(localPosition.x(),localPosition.y(),0)-u;
              if ((zIndex==0 && x.cross(y).z()<0) || (zIndex==1 && x.cross(y).z()>0)) {
                if (implementWaves) {
                  if (phiIndex==phiBinning.getNumDivisions()-1) phiIndex=0;
                  else phiIndex++;
                }
              }
            }
          }
        }

        //static LArG4Identifier id;
        m_larID.clear();
        m_larID << 4          // LArCalorimeter
                << 1          // LArEM
                << ((zIndex==0) ? -1:1)
                << samplingIndex
                << regionIndex
                << etaIndex
                << phiIndex;
        // call process to add this to the collection 
        SimpleHit(m_larID, spot.GetTime(), spot.GetEnergy());

        return;
      }
    }
  }

}
