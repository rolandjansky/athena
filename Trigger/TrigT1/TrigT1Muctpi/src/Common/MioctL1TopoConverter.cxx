/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MioctL1TopoConverter.h"

namespace LVL1MUCTPI {

  MioctL1TopoConverter::MioctL1TopoConverter( ): m_logger( "MioctL1TopoConverter" ){

  }

  MioctL1TopoConverter::~MioctL1TopoConverter() {

  }

  
  
  void MioctL1TopoConverter::setupParser(const std::string & inputfile) {
    m_xmlParser.readConfiguration(inputfile);
    m_xmlParser.parseConfiguration();
  }

  LVL1::MuCTPIL1TopoCandidate MioctL1TopoConverter::convertToL1Topo(const MioctID& mioctModuleID, const Sector& sector, bool isFirstCandidate) {
    LVL1::MuCTPIL1TopoCandidate l1topoCand;

    //check variable to make sure a match was found, otherwise issue warning
    bool foundRoiMatch = false;
    
    // check varialble to make sure the TopoCell info is found for the RoI
    bool foundTopoCellMatch = false;


    // get access to the MuCTPI geometry and Pt encoding from Parser
    MuCTPiGeometry muctpiGeo = m_xmlParser.getMuCTPiGeometry(); 
    L1MuonPtEncoding  ptEnc = muctpiGeo.ptEncoding();

    //Note in MioctModule terms we have: neg hemisphere / mod ID 0-7 and pos hemisphere / mod ID 0-7
    //While from the parser we have mod ID 0-15, with 0-7 in neg hemisphere, and 8-15 in pos hemisphere
    unsigned int mioctModNumber=0;

    if (mioctModuleID.getRapidityRegion() == NEGATIVE){
      mioctModNumber = mioctModuleID.getNumber();
    } else if (mioctModuleID.getRapidityRegion() == POSITIVE){
      mioctModNumber = mioctModuleID.getNumber() + 8;
    } else {
      REPORT_ERROR_MSG("Unknown Hemisphere description");
    }

    //Loop over Miocts to get the right one
    std::vector<MioctGeometry> mioctVec = muctpiGeo.octants();
    MioctGeometry thisMioctGeo;

    for ( std::vector<MioctGeometry>::iterator it = mioctVec.begin(); it != mioctVec.end(); ++it){
      if (it->mioctId() == mioctModNumber){
	thisMioctGeo=(*it);
	break;
      }
    }
    
    // Loop over the sectors in this Mioct to get the right one
    std::vector<MioctSectorGeometry> mioctSecVec = thisMioctGeo.sectors();
    std::vector<MioctTopoCellGeometry> topCellVec = thisMioctGeo.topoCells();
    MioctSectorGeometry thisMioctSecGeo;

    for (std::vector<MioctSectorGeometry>::iterator it = mioctSecVec.begin(); it != mioctSecVec.end(); ++it){
      if (it->name() == sector.getIDString()) {
	thisMioctSecGeo = (*it);
	break;
      }
    }

    // pick candidate one or two from input sector 
    unsigned int inputPt;
    unsigned int inputRoi;
    if (isFirstCandidate) {
      inputPt  = sector.getPtCand1();
      inputRoi = sector.getROI1();
    } else {
      inputPt  = sector.getPtCand2();
      inputRoi = sector.getROI2();
    }
    // Find the right ROI in the Geometry
    std::vector<MioctROIGeometry> mioctRoiGeo = thisMioctSecGeo.ROIs();
    MioctROIGeometry thisRoi;
    // Loop over the RoIs in this sector to find the right one and get the equivalent eta/phi etc
    for (  std::vector<MioctROIGeometry>::iterator it = mioctRoiGeo.begin(); it != mioctRoiGeo.end(); ++it) {
      if (it->roiid() == inputRoi){
	foundRoiMatch =true;
	thisRoi = (*it);
	break;
      }
    }

    int thisieta = 0;
    int thisiphi = 0;

    if (!foundRoiMatch) {
      REPORT_MSG(LVL1MUCTPI::WARNING,"No RoI match found for Sector: " << sector.getIDString() << "  RoI: " << inputRoi <<  " in MioctL1TopoConverter, returning 0 for eta/phi" << std::endl)
	}
    else {
      // now loop over the topocell info and find the ieta and iphi values for this RoI
      
      for (  std::vector<MioctTopoCellGeometry>::iterator it = topCellVec.begin(); it != topCellVec.end(); ++it) {
	if (it->etacode() == thisRoi.etacode() && it->phicode() == thisRoi.phicode()  ){
	  thisieta = it->ieta();
	  thisiphi = it->iphi();
	  foundTopoCellMatch = true;
	  break;
	}
      }
    }
    
    if (!foundTopoCellMatch) {
      REPORT_MSG(LVL1MUCTPI::WARNING,"No TopoCell match found for Sector: " << sector.getIDString() << "  RoI: " << inputRoi <<  " in MioctL1TopoConverter, returning 0 for ieta/iphi" << std::endl)
	}


    // get the Pt encoding
    unsigned int ptCode = 0;
    unsigned int thresholdValue = 0;
    ptEnc.getCodingInfo( inputPt-1, ptCode, thresholdValue );

    // Now fill all the information into the output object
    l1topoCand.setCandidateData(sector.getIDString(), inputRoi, sector.getBCID(), inputPt, ptCode, thresholdValue,
				thisRoi.eta(), thisRoi.phi(), thisRoi.etacode(), thisRoi.phicode(),
				thisRoi.etamin(), thisRoi.etamax(), thisRoi.phimin(), thisRoi.phimax(), mioctModNumber, 
				thisieta, thisiphi );
				
    return l1topoCand;
  }
  

} // namespace LVL1MUCTPI
