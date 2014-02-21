/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FastSimulation/EndcapFastSimDedicatedSD.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArG4Code/EnergySpot.h"
#include "LArG4SD/LArG4SD.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVHitMerger.h"
#include "GeoSpecialShapes/LArWheelCalculator.h"


using HepGeom::Point3D;
using HepGeom::Transform3D;


class EndcapFastSimDedicatedSD::Clockwork {

public:

  const EMECDetectorManager * emecManager;
  const LArWheelCalculator  * innerWheelCalculatorPos;
  const LArWheelCalculator  * innerWheelCalculatorNeg;
  const LArWheelCalculator  * outerWheelCalculatorPos;
  const LArWheelCalculator  * outerWheelCalculatorNeg;

};



// Constructor:
EndcapFastSimDedicatedSD::EndcapFastSimDedicatedSD():
  IFastSimDedicatedSD("EndcapFastSimDedicated"), c(new Clockwork())
{

  c->emecManager=NULL;
  c->innerWheelCalculatorPos=NULL;
  c->innerWheelCalculatorNeg=NULL;
  c->outerWheelCalculatorPos=NULL;
  c->outerWheelCalculatorNeg=NULL;

}

// Destructor:
EndcapFastSimDedicatedSD::~EndcapFastSimDedicatedSD() {
  delete c;
}


// ProcessHitsMethod
void EndcapFastSimDedicatedSD::ProcessSpot(const EnergySpot  & spot){
  if (!c->emecManager) {
    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
    if (detStore->retrieve(c->emecManager)!= StatusCode::SUCCESS) {
      throw std::runtime_error("Error, cannot get a Endcap Manager in EndcapFastSimDedicatedSD");
    }
  }
  
  if (!c->emecManager) {
    throw std::runtime_error("Error, cannot get a Endcap Manager in EndcapFastSimDedicatedSD");
  }
  
  if (!c->innerWheelCalculatorPos) c->innerWheelCalculatorPos = new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel,+1); 
  if (!c->innerWheelCalculatorNeg) c->innerWheelCalculatorNeg = new LArWheelCalculator(LArWheelCalculator::InnerAbsorberWheel,-1); 
  if (!c->outerWheelCalculatorPos) c->outerWheelCalculatorPos = new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel,+1); 
  if (!c->outerWheelCalculatorNeg) c->outerWheelCalculatorNeg = new LArWheelCalculator(LArWheelCalculator::OuterAbsorberWheel,-1); 
  
  
  
  // Fill the identifier.
  Point3D<double> globalPosition=spot.GetPosition();
  
  static Transform3D xfPos = c->emecManager->getDetectorRegion(1,1,0,0)->getAbsoluteTransform().inverse();
  static Transform3D xfNeg = c->emecManager->getDetectorRegion(0,1,0,0)->getAbsoluteTransform().inverse();
  
  
  Point3D<double> localPosition        = globalPosition.z()<0 ? xfNeg*globalPosition : xfPos*globalPosition;
  Point3D<double> shiftedLocalPosition = localPosition + Point3D<double>(0.0,0.0,c->emecManager->getFocalToRef()+c->emecManager->getRefToActive());
  int zIndex = globalPosition.z()<0 ? 0:1;
  double eta = shiftedLocalPosition.getEta();
  double phi = localPosition.getPhi();
  double z   = localPosition.z();

  EMECDetectorManager::DetectorRegionConstIterator e;
  for (e=c->emecManager->beginDetectorRegion();e!=c->emecManager->endDetectorRegion();  e++) {
    const EMECDetectorRegion *region = *e;
    if (region->getEndcapIndex()!=zIndex) continue;
    const EMECDetDescr       *regionDescriptor=region->getDescriptor();
    const CellBinning       & etaBinning=regionDescriptor->getEtaBinning();
    if (eta>etaBinning.getStart() && eta<etaBinning.getEnd()) {
     
      unsigned int etaIndex = int((eta - etaBinning.getStart())/etaBinning.getDelta()) + etaBinning.getFirstDivisionNumber();
      unsigned int phiIndex = 0;
      EMECCellConstLink cellPtr = region->getEMECCell(etaIndex,phiIndex);
      double zmax = cellPtr->getZLocal(EMECCell::BACK);
      double zmin = cellPtr->getZLocal(EMECCell::FRONT);
      if (z>zmin && z<zmax) {
	const CellBinning       & phiBinning=regionDescriptor->getPhiBinning();
	double minPhi=std::min(phiBinning.getStart(), phiBinning.getEnd());
	double maxPhi=std::max(phiBinning.getStart(), phiBinning.getEnd());
 	while (phi<minPhi)   phi += 2*M_PI;
	while (phi>maxPhi)   phi -= 2*M_PI;

	unsigned int regionIndex = region->getRegionIndex();
	unsigned int samplingIndex = region->getSamplingIndex();
	//unsigned int phiIndex      =  int((phi - phiBinning.getStart())/phiBinning.getDelta()) + phiBinning.getFirstDivisionNumber();
	int wheel  = region->getRadialIndex()==0 ? 2:3;
	int sWheel = zIndex==0 ? -wheel: wheel;

	// But now we are more sophisticated...:

	LArWheelCalculator *wheelCalc=NULL;
	int nGaps= region->getRadialIndex()==0 ? 768:256;
	int nBins= phiBinning.getNumDivisions();
	int gapsPerBin = nGaps/nBins;
	

	if      (zIndex==0 && region->getRadialIndex()==0) {
	  wheelCalc=const_cast<LArWheelCalculator *> (c->outerWheelCalculatorNeg);
	}
	else if (zIndex==0 && region->getRadialIndex()==1){
	  wheelCalc=const_cast<LArWheelCalculator *> (c->innerWheelCalculatorPos);
	}
	else if (zIndex==1 && region->getRadialIndex()==0){
	  wheelCalc=const_cast<LArWheelCalculator *>(c->outerWheelCalculatorNeg);
	 }
	else if (zIndex==1 && region->getRadialIndex()==1){
	  wheelCalc=const_cast<LArWheelCalculator *> (c->innerWheelCalculatorPos);
	}
	else {
	  throw std::runtime_error("Error, unknown wheel in EndcapFastSimDedicatedSD");
	}

	int phiBin = wheelCalc->GetPhiGap(localPosition)/gapsPerBin;

	if (zIndex==0) {
	   phiBin = (nBins-2)/2 - phiBin;
	   if(phiBin < 0) {
	     phiBin += nBins;
	   }
	}

	//std::cout << didIt <<  "Compare phi gap: " << nGaps << ' ' << nBins << ' ' << gapsPerBin << ':' << phiBin <<  '/' << phiIndex << std::endl;

	static LArG4Identifier id;
	id.clear();
	id             << 4          // LArCalorimeter
		       << 1          // LArEM
		       << sWheel
		       << samplingIndex
		       << regionIndex
		       << etaIndex
		       << phiBin;   
	// call process with dummy first argument
	m_hitMerger->process(0,id, spot.GetTime(), spot.GetEnergy());
	return;
      }
    }
  }
 
  
}


