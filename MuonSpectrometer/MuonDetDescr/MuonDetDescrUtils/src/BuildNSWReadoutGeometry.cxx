/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDetDescrUtils/BuildNSWReadoutGeometry.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorStore.h"

#include <TString.h> // for Form
#include <fstream>

using namespace MuonGM;

BuildNSWReadoutGeometry::BuildNSWReadoutGeometry()
{}

bool BuildNSWReadoutGeometry::BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr/*, std::map<GeoFullPhysVol*, std::string>* vec*/) const
{
  bool geoBuilt = true;  

  detectorList& dList=AGDDDetectorStore::GetDetectorStore()->GetDetectorList();
  detectorList::const_iterator it;
  for (it=dList.begin(); it!=dList.end(); ++it) {

    std::vector<AGDDDetectorPositioner*>& dPos=((*it).second)->GetDetectorPositioners();
    for (unsigned int i=0;i<dPos.size();i++) {

      std::string chTag = dPos[i]->ID.detectorAddress;
      GeoFullPhysVol* vol = dPos[i]->theVolume;
      
      std::string stName = chTag.substr(0,4);
      
      int etaIndex = 999;
      int phiIndex = 999;
      int mLayer   = 999;
      int iSide    = 0;
      int iLS      = atoi((chTag.substr(3,1)).c_str()); //sTG3 and sMD3 are small chambers for small sectors 
      if (iLS==3) iLS = 1; // small 
      else iLS = 0; // large 
      if (chTag.substr(13,1)=="A") iSide=1;
      else if (chTag.substr(13,1)=="C") iSide=-1;
      etaIndex = iSide*atoi((chTag.substr(5,1)).c_str());
      phiIndex = atoi((chTag.substr(12,1)).c_str());
      mLayer = atoi((chTag.substr(7,1)).c_str());
      
      if (chTag.substr(0,3)=="sMD") {

	MMReadoutElement* re = new MMReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);
	std::string myVolName = (chTag.substr(0,8)).c_str();
	re->initDesign(-999., -999., -999., -999., -999.);
	re->fillCache();
	mgr->addMMReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	re->setDelta(mgr);
      }
      else if (chTag.substr(0,3)=="sTG") {
	
	sTgcReadoutElement* re = new sTgcReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);	  
	std::string myVolName = (chTag.substr(0,8)).c_str();
	re->initDesign(-999., -999., -999., 3.2, -999., 2.7, -999., 2.6);
	re->fillCache();
	mgr->addsTgcReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	re->setDelta(mgr);
      }
    }
  }
  return geoBuilt;

}
