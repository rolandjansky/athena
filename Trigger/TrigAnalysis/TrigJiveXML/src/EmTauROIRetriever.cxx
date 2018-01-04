/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/EmTauROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  EmTauROIRetriever::EmTauROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("EmTauROI")
  {

    declareInterface<IDataRetriever>(this);
    declareProperty("readCPM", m_readCPM=false,"If 'true' reads low-level ROI data from L1Calo hardware. False by default");
    // this could be used to mask the lower 4 of the possible 8 multiplicities
    // to be implemented if necessary
    declareProperty("maskLowerThresholds", m_maskLowerThresholds=false);
  }

  //--------------------------------------------------------------------------

  StatusCode EmTauROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " retrieving" <<
     ( m_readCPM ? " from CPMRoIs (ACR) " : " EmTauROI (LVL1_ROI) ") << endmsg;

  DataVect phi;
  DataVect eta; 
  DataVect energy; 
  DataVect energyEM;
  DataVect energyTAU;
  DataVect roiWord; 
  DataVect thrPattern; 

  if (!m_readCPM){ // 'normal' mode: Use Offline EmTauROI object

    const LVL1_ROI * roi;

    // L1JetObject -not- available
    m_sgKey = "LVL1_ROI"; 
    if ( evtStore()->retrieve(roi,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No LVL1_ROI found in SG for Level-1 JetROI" << endmsg;
      return StatusCode::SUCCESS;
    } 
 
    // Access as in: PhysicsAnalysis/AnalysisCommon/AnalysisTest/ReadTrigger
    // Tested with: AODTriggerRead_topOptions.py
    // class read:
    //    PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent/EmTau_ROI.h
    LVL1_ROI::emtaus_type::const_iterator itEM  = (roi->getEmTauROIs()).begin();
    LVL1_ROI::emtaus_type::const_iterator itEMe = (roi->getEmTauROIs()).end();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "JetROIs from LVL1_ROI retrieved from StoreGate with size: "<< (roi->getJetROIs()).size() <<endmsg;

    for (; itEM != itEMe; ++itEM)
        {
        phi.push_back(DataType( itEM->getPhi()));
	eta.push_back(DataType( itEM->getEta()));
	//// Info from Alan Watson:
        //// pt() is highest threshold passed ! Not ROI energy.
        //// getEMClus() returns the EM cluster ET
	//// getTauClus() returns the Tau cluster ET 
        //// use TAU for 'energy', which includes EM layers:
        energy.push_back(DataType( itEM->getTauClus() /CLHEP::GeV ));
        // Seperate EM and TAU, useful to experts:
	energyEM.push_back(DataType( itEM->getEMClus() / CLHEP::GeV ));
	energyTAU.push_back(DataType( itEM->getTauClus() /CLHEP::GeV ));
        roiWord.push_back(DataType( itEM->getROIWord()));
	thrPattern.push_back(DataType( itEM->getThrPattern()));
      }

  }else{ // readCPM mode: decode bits from CPM into eta,phi, for ACR data from SFI  

    const DataVector<LVL1::CPMRoI>* cpmRoICollection = 0;
    LVL1::CPRoIDecoder decoder;
    double m_roiPhi=0.;
    // L1JetObject -not- available
    m_sgKey = "CPMRoIs";
    if ( evtStore()->retrieve(cpmRoICollection, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No DataVector<LVL1::CPMRoI> found with key CPMRoIs" << endmsg;
      return StatusCode::SUCCESS;
    } 
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "DataVector<LVL1::CPMRoI> retrieved from StoreGate with size: "<< cpmRoICollection->size() <<endmsg;

      DataVector<LVL1::CPMRoI>::const_iterator roi_it = cpmRoICollection->begin();
      DataVector<LVL1::CPMRoI>::const_iterator roi_end = cpmRoICollection->end();
	
      for(;roi_it!=roi_end;++roi_it) {
        const LVL1::CPMRoI* roi = (*roi_it);

  	const LVL1::CoordinateRange coord(decoder.coordinate(roi->roiWord()));
	const double roiEta = coord.eta();
	const double roiPhi = coord.phi();
        m_roiPhi = roiPhi;  

        // different coordinates in phi, correct for this:
        if (m_roiPhi > M_PI ){ m_roiPhi = m_roiPhi-2*M_PI; }

	phi.push_back(DataType( m_roiPhi ));
	eta.push_back(DataType( roiEta ));

         // info from Alan Watson: energy would need to be decoded
	 // with jet elements. This will only be implemented if necessary
	 //    jpt 19Mar09
	 energy.push_back(DataType( 1 ) ); //placeholder
	 energyEM.push_back(DataType( 1 ) ); //placeholder
	 energyTAU.push_back(DataType( 1 ) ); //placeholder

	 roiWord.push_back(DataType( roi->roiWord() )); 
	 thrPattern.push_back(DataType( roi->hits() )); 
        }
   }//end if readCPM

    DataMap myDataMap;
    myDataMap["energy"] = energy;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["energyEM"] = energyEM;
    myDataMap["energyTAU"] = energyTAU;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrPattern"] = thrPattern;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size()
					    << " from: " << m_sgKey << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
