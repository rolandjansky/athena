/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/JetROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1CaloEvent/JEMRoI.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  JetROIRetriever::JetROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("JetROI")
  {

    declareInterface<IDataRetriever>(this);
    declareProperty("readJEM", m_readJEM=false,"If 'true' reads low-level ROI data from L1Calo hardware. False by default");
    // this could be used to mask the lower 4 of the possible 8 multiplicities
    // to be implemented if necessary
    declareProperty("maskLowerThresholds", m_maskLowerThresholds=false);
  }

  //--------------------------------------------------------------------------

  StatusCode JetROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " retrieving" <<
    ( m_readJEM ? " from JEMRoIs (ACR) " : " JetROI (LVL1_ROI) ") << endmsg;

  DataVect phi;
  DataVect eta;
  DataVect energy;
  DataVect roiWord;
  DataVect thrPattern;

  if (!m_readJEM){ // 'normal' mode: Use Offline EmTauROI object

    const LVL1_ROI * roi;

    // L1JetObject -not- available
    m_sgKey = "LVL1_ROI";
    if ( evtStore()->retrieve(roi,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No LVL1_ROI found in SG for Level-1 JetROI" << endmsg;
      return StatusCode::SUCCESS;
    } 

    // Access as in: PhysicsAnalysis/AnalysisCommon/AnalysisTest/ReadTrigg
    // Tested with: AODTriggerRead_topOptions.py

    LVL1_ROI::jets_type::const_iterator itJET  = (roi->getJetROIs()).begin();
    LVL1_ROI::jets_type::const_iterator itJETe = (roi->getJetROIs()).end();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "JetROIs from LVL1_ROI retrieved from StoreGate with size: "<< (roi->getJetROIs()).size() <<endmsg;

    for (; itJET != itJETe; ++itJET)
        {
        phi.push_back(DataType( itJET->getPhi()));
	eta.push_back(DataType( itJET->getEta()));

// info from Alan Watson: pT is getET8x8();
//        energy.push_back(DataType(  itJET->pt()/1000. ));
//// other options: getET4x4(), getET6x6()
        energy.push_back(DataType( itJET->getET4x4()/1000. )); 

        roiWord.push_back(DataType( itJET->getROIWord()));
	thrPattern.push_back(DataType( itJET->getThrPattern()));
      }

  }else{ // readJEM mode: decode bits from JEM into eta,phi, for ACR data from SFI  

    const DataVector<LVL1::JEMRoI>* jemRoICollection = 0;
    LVL1::JEPRoIDecoder decoder;
    double m_roiPhi=0.;
    // L1JetObject -not- available
    m_sgKey = "JEMRoIs";
    if ( evtStore()->retrieve(jemRoICollection, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No DataVector<LVL1::JEMRoI> found with key JEMRoIs" << endmsg;
      return StatusCode::SUCCESS;
    } 
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "DataVector<LVL1::JEMRoI> retrieved from StoreGate with size: "<< jemRoICollection->size() <<endmsg;

      DataVector<LVL1::JEMRoI>::const_iterator roi_it = jemRoICollection->begin();
      DataVector<LVL1::JEMRoI>::const_iterator roi_end = jemRoICollection->end();
	
      for(;roi_it!=roi_end;++roi_it) {
        const LVL1::JEMRoI* roi = (*roi_it);

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

	 roiWord.push_back(DataType( roi->roiWord() )); 
	 thrPattern.push_back(DataType( roi->hits() )); 
        }
   }//end if readJEM

    DataMap myDataMap;
    myDataMap["energy"] = energy;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrPattern"] = thrPattern;
  
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() 
           << " from: " << m_sgKey << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
