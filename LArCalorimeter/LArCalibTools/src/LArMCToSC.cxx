/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   
 */

#include "LArCalibTools/LArMCToSC.h"

#include "LArElecCalib/ILArADC2MeVTool.h"

#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "LArRawConditions/LArPedestalMC.h"
#include "LArCOOLConditions/LArPedestalSC.h"

#include "LArRawConditions/LArOFCMC.h"

#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LAruA2MeVMC.h"

#include "LArCOOLConditions/LAruA2MeVSC.h"
#include "LArCOOLConditions/LArDAC2uASC.h"

#include "LArRawConditions/LArfSamplMC.h"
#include "LArCOOLConditions/LArfSamplSC.h"

#include "LArRawConditions/LArMinBiasAverageComplete.h"
#include "LArCOOLConditions/LArMinBiasAverageSC.h"

#include "LArRawConditions/LArShapeMC.h"
#include "LArCOOLConditions/LArShapeSC.h"

#include "LArRawConditions/LArRampMC.h"
#include "LArCOOLConditions/LArRampSC.h"

#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArCOOLConditions/LArAutoCorrSC.h"

#include "LArRawConditions/LArNoiseMC.h"
#include "LArCOOLConditions/LArNoiseSC.h"

#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArCOOLConditions/LArMinBiasSC.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

#include "CaloDetDescr/CaloDetDescrElement.h"

#include "LArElecCalib/LArCalibErrorCode.h"
#include <iostream>
#include <sstream>
#include <fstream> 
#include <TMath.h>

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArMCToSC::LArMCToSC( const std::string& name, 
                      ISvcLocator* pSvcLocator) : 
	::AthAlgorithm( name, pSvcLocator ),
	m_hashMax(0),
	m_scid(0),  
	m_cid(0),
	m_sem_mgr(0),  
	m_em_mgr(0),
	m_scidtool ("CaloSuperCellIDTool"),
	m_scCablingTool("LArSuperCellCablingTool"),
	m_adc2mevtool ("LArADC2MeVTool")
{
	declareProperty("uA2MeVInput",m_uA2MeVInput="LAruA2MeV");
	declareProperty("DAC2uAVInput",m_DAC2uAInput="LArDAC2uA");
	declareProperty("RampInput",m_RampInput="LArRamp");
	declareProperty("MinBiasInput",m_MinBiasInput="LArMinBias");
	declareProperty("MinBiasAverageInput",m_MinBiasAverageInput="LArMinBiasAverage");
	declareProperty("fSamplInput",m_fSamplInput="LArfSampl");
	declareProperty("autoCorrInput",m_autoCorrInput="LArAutoCorr");
	declareProperty("noiseInput",m_noiseInput="LArNoise");
	declareProperty("pedestalInput",m_pedestalInput="LArPedestal");
	declareProperty ("SCIDTool", m_scidtool,
	                 "Offline / supercell mapping tool.");
	declareProperty("LArSuperCellCablingTool",m_scCablingTool);
	declareProperty ("ADC2MeVTool", m_adc2mevtool);
	declareProperty ("fixedLSB", m_fixedLSB=false);
	declareProperty ("quantEt", m_quantEt=true);
	declareProperty ("mQuant", m_mQuant=125.0);
	declareProperty ("emQuant", m_emQuant=32.0);
	declareProperty ("forwardQuant", m_forwardQuant=250.0);
}

// Destructor
///////////////
LArMCToSC::~LArMCToSC()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LArMCToSC::initialize()
{
	m_xlsIsFCAL.clear();
	m_xlsIsHEC.clear();
	m_xlsIsEM.clear(); 
	m_xlsSampl.clear();
	m_xlsEta.clear(); 
	m_xlsLSB.clear(); 
	m_xlsNoise.clear();

	m_fcalSampl.clear();
	m_fcalEtaI.clear();
	m_fcalNoise.clear();
	m_fcalLSB.clear();

	m_scADC2MeV.clear();

	m_minBiasInput.clear();
	m_offlIDMinBias.clear();

	//CHECK(m_cablingSvc.retrieve());
	return StatusCode::SUCCESS;
}

void LArMCToSC::FillNoise(){ 
	// Read in information from noise spreadsheet
  
	// Get info for barrel, endcap, and hec
	std::ifstream xlsNoiseFile ( "corrNoise_v5.txt" );
 
	std::vector <std::string> lineEl;

	std::string line;
	std::string tempEl;
	int idx = 0;
	//msg(MSG::INFO) << "Here comes the XLS sheet" << std::endl;
	while (xlsNoiseFile.good())
		{
			std::getline (xlsNoiseFile, line);
			std::istringstream lineSS (line);
			//std::cout << "TEST: " << line << std::endl;
			int elIdx = 0, tempInt = 0;
			float tempFloat = 0;
			while(!lineSS.eof()){
				tempInt = tempFloat = 0;
				std::getline(lineSS, tempEl, ' ');
				std::stringstream elSS (tempEl);
				//std::cout << "TEST ETA: " << tempEl << std::endl;
				if(elIdx<4){
					elSS >> tempInt;
					// First element tells if this if for the FCAL
					if(elIdx == 0)
						m_xlsIsFCAL.push_back(tempInt);
					// Second element tells if this if for the HEC
					else if(elIdx == 1)
						m_xlsIsHEC.push_back(tempInt);
					// Third element tells if this if for the EM
					else if(elIdx == 2)
						m_xlsIsEM.push_back(tempInt);
					// And now the samp
					else 
						m_xlsSampl.push_back(tempInt);
				}
				else{
					elSS >> tempFloat;
					//std::cout << "TEST ETA: " << tempFloat << std::endl;
					if(elIdx==4)
						m_xlsEta.push_back(tempFloat);
					else if(elIdx==5)
						m_xlsLSB.push_back(tempFloat);
					else
						m_xlsNoise.push_back(tempFloat);
				}
				elIdx++;
			}
			idx++;
		}


	// The FCAL is special, first the noise
	std::ifstream fcalNoiseLSBFile ( "FCAL_Noise_LSB_fixed.txt" );
 
	idx = 0;
	while (fcalNoiseLSBFile.good())
		{
			std::getline (fcalNoiseLSBFile, line);
			std::istringstream lineSS (line);
			int elIdx = 0, tempInt = 0;
			float tempFloat = 0;
			std::vector <float> tempNoiseVec;
			std::vector <float> tempLSBVec;
			while(!lineSS.eof()){
				tempInt = tempFloat = 0;
				std::getline(lineSS, tempEl, ' ');
				std::stringstream elSS (tempEl);
				if(elIdx==0){
					elSS >> tempInt;
					m_fcalSampl.push_back(tempInt);
				}
				else if(elIdx == 1){
					elSS >> tempInt;
					m_fcalEtaI.push_back(tempInt);
				}
				else if(elIdx<18){
					elSS >> tempFloat;
					tempNoiseVec.push_back(tempFloat);
				}
				else{
					elSS >> tempFloat;
					tempLSBVec.push_back(tempFloat);
				}
				elIdx++;
			}
			m_fcalNoise.push_back(tempNoiseVec);
			m_fcalLSB.push_back(tempLSBVec);
			idx++;
		}
}

void LArMCToSC::FillLArMinBias(){ 
	// Read in information from noise spreadsheet
  
	// Get info for barrel, endcap, and hec
	std::ifstream minBiasFile ( "embCellsSamp3.txt" );
 
	std::vector <std::string> lineEl;

	std::string line;
	std::string tempEl;
	int lI = 0;
	while (minBiasFile.good() && lI<10)
		{
			std::getline (minBiasFile, line);
			std::istringstream lineSS (line);

			uint tempInt = 0;
			int elIdx = 0;
			float tempFloat = 0;
			while(!lineSS.eof()){
				tempInt = tempFloat = 0;
				std::getline(lineSS, tempEl, ' ');
				std::stringstream elSS (tempEl);
				// First element tells if this if for the FCAL
				if(elIdx == 0){
					elSS >> tempInt;
					m_offlIDLArMinBias.push_back(tempInt);
				}
				// // Second element tells if this if for the HEC
				// else if(elIdx == 1){
				//   elSS >> tempFloat;
				//   minBiasLArInput.push_back(tempFloat);
				// }
				elIdx++;
			}
			lI++;

		}
}

void LArMCToSC::FillMinBias(){ 
	// Read in information from noise spreadsheet
  
	// Get info for barrel, endcap, and hec
	std::ifstream minBiasFile ( "sc_rms.txt" );
 
	std::vector <std::string> lineEl;

	std::string line;
	std::string tempEl;
	while (minBiasFile.good())
		{
			std::getline (minBiasFile, line);
			std::istringstream lineSS (line);

			uint tempInt = 0;
			int elIdx = 0;
			float tempFloat = 0;
			while(!lineSS.eof()){
				tempInt = tempFloat = 0;
				std::getline(lineSS, tempEl, ' ');
				std::stringstream elSS (tempEl);
				// First element tells if this if for the FCAL
				if(elIdx == 0){
					elSS >> tempInt;
					m_offlIDMinBias.push_back(tempInt);
				}
				// Second element tells if this if for the HEC
				else if(elIdx == 1){
					elSS >> tempFloat;
					m_minBiasInput.push_back(tempFloat);
				}
				elIdx++;
			}
		}
}

CondAttrListCollection* LArMCToSC::noiseSC(const ILArNoise* input, const std::string& outputName) {
	// Noise
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("Noise", "blob");
	spec->extend<unsigned>("version");
	CondAttrListCollection* coll=new CondAttrListCollection(true);
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	coral::Blob& blobNoise=(*attrList)["Noise"].data<coral::Blob>();
	blobNoise.resize(m_hashMax*sizeof(float));
	float* pblobNoise=static_cast<float*>(blobNoise.startingAddress());
	const std::vector<float> * adc2mev;
	std::vector<Identifier> cells;

	(*attrList)["version"].setValue(0U);
  
	float sumSq, eta, /*phi,*/ sinTheta, totalNoise = -999., lsb = -1., avgADC2MeV = 0.;
	int gain = 0; // high gain = 0
	msg(MSG::INFO) << "hashmax test =  " << m_hashMax << std::endl;
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		sumSq = 0; avgADC2MeV = 0;
		const Identifier scid=m_scid->cell_id(hs);
		cells =  m_scidtool->superCellToOfflineID (scid);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		int nCells = 0;
		for(unsigned cI=0; cI<cells.size(); cI++){
			adc2mev = &(m_adc2mevtool->ADC2MEV(cells[cI], gain));
			sumSq+=pow(input->noise(cells[cI], gain)*(*adc2mev)[1],2); 
			nCells++;
			avgADC2MeV += (*adc2mev)[1];
			//if(m_scid->is_em_barrel(scid))
			//std::cout << "offlineIdCell " << cells[cI].get_identifier32().get_compact() << std::endl;
		}
		m_scADC2MeV.push_back(avgADC2MeV);
    
		if(//m_scid->phi(scid) == 0 && 
		   //m_scid->pos_neg(scid) > 0 && 
		   //!m_scid->is_fcal(scid) &&
		   //!m_scid->is_hec(scid)
		   1
		   ){
			const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
			if (!dde) {
				msg(MSG::WARNING) << "No DDE found for cell " << scid << std::endl;
				continue;
			}
      
			eta = dde->eta_raw();
			//phi = dde->phi_raw();

			if(m_quantEt)
				sinTheta = sin(atan(exp(-1*eta))*2);
			else 
				sinTheta = 1.0;

			totalNoise = -999.; lsb = -1.;
			if(m_scid->is_fcal(scid) == 0){
				for(uint xlsI = 0; xlsI < m_xlsIsEM.size(); xlsI++)
					{ 
						//if(xlsEta[xlsI]-0.05<0.001)
						//  msg(MSG::INFO) << "TEST, eta, noise: " << xlsSampl[xlsI] << std::endl;
	      
						if(//m_scid->is_fcal(scid) == 0 && 
						   m_xlsIsFCAL[xlsI] == m_scid->is_fcal(scid) &&
						   m_xlsIsHEC[xlsI] == m_scid->is_hec(scid) &&
						   m_xlsIsEM[xlsI] == m_scid->is_em_endcap(scid) && 
						   m_xlsSampl[xlsI] == m_scid->sampling(scid) &&
						   fabs(m_xlsEta[xlsI]-fabs(eta))<0.005){
	    
							if(m_scid->is_hec(scid)){
								totalNoise = m_xlsNoise[xlsI]*1000;
							}
							else {
								totalNoise = sqrt(pow(m_xlsNoise[xlsI]*1000,2)+sumSq);
								//totalNoise = sqrt(sumSq);
							}
	    
							if(m_fixedLSB){
								if(!m_scid->is_hec(scid)){
									if(m_scid->sampling(scid) == 2){
										lsb = m_mQuant/sinTheta;
									}
									else{
										lsb = m_emQuant/sinTheta;
									}
								}
								else{
									lsb = m_forwardQuant/sinTheta;
								}
							}
							else{
								lsb = m_xlsLSB[xlsI]*1000;
							}
							break;
						} 
					}
			} else {
				for(uint fcalI = 0; fcalI < m_fcalSampl.size(); fcalI++){
					if(//m_scid->is_fcal(scid) &&
					   m_fcalSampl[fcalI] == m_scid->sampling(scid) &&
					   m_fcalEtaI[fcalI] == m_scid->eta(scid)){
						totalNoise = m_fcalNoise[fcalI][m_scid->phi(scid)]*1000.;
						if(m_fixedLSB){
							lsb = m_forwardQuant/sinTheta;
						}
						else{

							lsb = m_fcalLSB[fcalI][m_scid->phi(scid)]*1000.; 
						}
						break;
						//msg(MSG::INFO) << "FCAL sampling, eta, phi: " << m_scid->sampling(scid) << ", " << m_scid->eta(scid) << ", " << m_scid->phi(scid) << ", " << totalNoise << ", " << lsb << std::endl; 
					}
				}
			}
			// if(m_scid->is_em_barrel(scid) && phi<1.815 && phi>1.81)
			// 	msg(MSG::INFO) << "offline id, sampl, eta, phi, noise, lsb: " << scid << ", " << m_scid->sampling(scid) << ", " << eta << ", " << phi << ", " << totalNoise*sinTheta << ", " << lsb << std::endl;
      
			pblobNoise[scOnlHash]=totalNoise/lsb;
		}
	}
	coll->add(0,*attrList);
	//delete attrList;//???
	StatusCode sc=detStore()->record(coll,outputName);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
		return NULL;
	}
	return coll;
}

CondAttrListCollection* LArMCToSC::rampSC(const ILArRamp* input, const std::string& outputName) {
 
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("RampVec", "blob");
	spec->extend<unsigned>("nPoints");
	spec->extend<unsigned>("version");
	CondAttrListCollection* coll=new CondAttrListCollection(true);
	const unsigned nPoints=2;

	float eta = -999, lsb = 0, avgRamp = 0, sinTheta = -999;
	uint rampSize = 0;
  
	int gain = 0;
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	(*attrList)["version"].setValue(0U);
	coral::Blob& blobRamp=(*attrList)["RampVec"].data<coral::Blob>();
	(*attrList)["nPoints"].setValue(nPoints);
	blobRamp.resize(m_hashMax*sizeof(float)*nPoints);
	float* pblobRamp=static_cast<float*>(blobRamp.startingAddress());
    
	std::vector<Identifier> cells;
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		//std::cout << "Offl id: " << scid.get_identifier32().get_compact() << std::endl;
		avgRamp = 0;
    
		cells =  m_scidtool->superCellToOfflineID (scid);
		for(unsigned cI=0; cI<cells.size(); cI++){
			const ILArRamp::RampRef_t ramp=input->ADC2DAC(cells[cI],gain);
	
			if (ramp.size()>=nPoints) {
				avgRamp+=ramp[1];
				rampSize = ramp.size();
			}
			else{
				rampSize=0;
				break;
			}
		}
		const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
		if (!dde) {
			msg(MSG::WARNING) << "No DDE found for cell " << scid << std::endl;
			continue;
		}
 
		eta = dde->eta_raw();
		if(m_quantEt)
			sinTheta = dde->sinTh();//sin(atan(exp(-1*eta))*2);
		else
			sinTheta = 1.0;
		
		lsb = -1;
		if(m_fixedLSB){
			if(!m_scid->is_hec(scid) && !m_scid->is_fcal(scid)){
				if(m_scid->sampling(scid) == 2){
					lsb = m_mQuant/sinTheta;
				}
				else {
					lsb = m_emQuant/sinTheta;
				}
			}else{
				lsb = m_forwardQuant/sinTheta;
			}
		}
		else{
			for(uint xlsI = 0; xlsI < m_xlsIsEM.size(); xlsI++)
				{ 
					if(m_scid->is_fcal(scid) == 0 && 
					   m_xlsIsFCAL[xlsI] == m_scid->is_fcal(scid) &&
					   m_xlsIsHEC[xlsI] == m_scid->is_hec(scid) &&
					   m_xlsIsEM[xlsI] == m_scid->is_em_endcap(scid) && 
					   m_xlsSampl[xlsI] == m_scid->sampling(scid) &&
					   fabs(m_xlsEta[xlsI]-fabs(eta))<0.005){
	    
					
						lsb = m_xlsLSB[xlsI]*1000;
					 
						break;
					} 
				}
		}
		if(m_fixedLSB){
			lsb = m_forwardQuant/sinTheta;
		}
		else{
			for(uint fcalI = 0; fcalI < m_fcalSampl.size(); fcalI++){
				if(m_scid->is_fcal(scid) 
				   && m_fcalSampl[fcalI] == m_scid->sampling(scid)
				   && m_fcalEtaI[fcalI] == m_scid->eta(scid)){
		
					lsb = m_fcalLSB[fcalI][m_scid->phi(scid)]*1000.; 		
					break;
				}
			}
		}
    
		if (rampSize>=nPoints) {
			for (size_t i=0;i<nPoints;++i) {
				if (i==1 && gain == 0){
					pblobRamp[nPoints*scOnlHash+i]=avgRamp*(lsb/m_scADC2MeV[hs]);
				}
				else{
					pblobRamp[nPoints*scOnlHash+i]=0;
				}
			}
		}
		else {
			msg(MSG::ERROR) << "Found ramp polynom with degree " << rampSize << ". Need at least " 
			                << nPoints << ". Filling with with ERRORCODE" << std::endl;
			for (size_t i=0;i<nPoints;++i) {
				pblobRamp[nPoints*scOnlHash+i]=LArElecCalib::ERRORCODE;
			}
		}
	}//end loop over hash ids
	coll->add(gain,*attrList);
    
	StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibMCSC/Ramp");
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection for ramp with key " << outputName << std::endl;
		return NULL;
	}
	return coll;
}


CondAttrListCollection* LArMCToSC::minBiasSC(const std::string& outputName) {
	// MinBias
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("MinBias", "blob");
	spec->extend<unsigned>("version");
	CondAttrListCollection* coll=new CondAttrListCollection(true);
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	coral::Blob& blobMinBias=(*attrList)["MinBias"].data<coral::Blob>();
	blobMinBias.resize(m_hashMax*sizeof(float));
	float* pblobMinBias=static_cast<float*>(blobMinBias.startingAddress());

	(*attrList)["version"].setValue(0U);
  
	float minBias = -999;
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
   
		const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
		if (!dde) {
			msg(MSG::WARNING) << "No DDE found for cell " << scid << std::endl;
			continue;
		}
		for(uint inI = 0; inI < m_minBiasInput.size(); inI++)
			{
				if(m_offlIDMinBias[inI] == scid.get_identifier32().get_compact()){
					minBias = m_minBiasInput[inI];
					//if(m_scid->is_em_barrel(scid))
					//msg(MSG::INFO) << "Min bias for " << offlIDMinBias[inI] << " is: " << minBiasInput[inI] << std::endl;
				}
			}
		pblobMinBias[scOnlHash]=minBias;    
	}
	coll->add(0,*attrList);
	//delete attrList;//???
	StatusCode sc=detStore()->record(coll,outputName);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
		return NULL;
	}
	return coll;
}
CondAttrListCollection* LArMCToSC::autoCorrSC(const ILArAutoCorr* input, const std::string& outputName) {

	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("AutoCorrVec", "blob");
	spec->extend<unsigned>("nPoints");
	spec->extend<unsigned>("version");
	CondAttrListCollection* coll=new CondAttrListCollection(true);
	const unsigned nPoints=4;
  
	int gain = 0;
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	(*attrList)["version"].setValue(0U);
	coral::Blob& blobAutoCorr=(*attrList)["AutoCorrVec"].data<coral::Blob>();
	(*attrList)["nPoints"].setValue(nPoints);
	blobAutoCorr.resize(m_hashMax*sizeof(float)*nPoints);
	float* pblobAutoCorr=static_cast<float*>(blobAutoCorr.startingAddress());
    
	std::vector<Identifier> cells;
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		std::vector<float> avgAutoCorr(nPoints, 0.0);
		//if(!m_scid->is_fcal(scid) && !m_scid->is_hec(scid)){
		cells =  m_scidtool->superCellToOfflineID (scid);

		for(unsigned cI=0; cI<cells.size(); cI++){
			const ILArAutoCorr::AutoCorrRef_t corr=input->autoCorr(cells[cI],gain);
			if (corr.size()==nPoints) {
				for (size_t i=0;i<nPoints;++i) {
					avgAutoCorr[i] += corr[i]/cells.size();
				}
			}
			else {
	  
				msg(MSG::ERROR) << "Found autoCorr with array length " << corr.size() << " and gain of " << gain << ". Need at least " 
				                << nPoints << ". Filling with with ERRORCODE" << std::endl;
				for (size_t i=0;i<nPoints;++i) {
					avgAutoCorr[i] = LArElecCalib::ERRORCODE;
				}
				break;
			}
		} // End of loop over calo cells within a super cell
		//}
		for (size_t i=0;i<nPoints;++i) {
			pblobAutoCorr[nPoints*scOnlHash+i]=avgAutoCorr[i];
		}
	}//end loop over hash ids
	coll->add(gain,*attrList);
     
	StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibMCSC/AutoCorr");
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection for ramp with key " << outputName << std::endl;
		return NULL;
	}
	return coll;

}

CondAttrListCollection* LArMCToSC::fSamplSC(const ILArfSampl* input, const std::string& outputName) {
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("fSampl", "blob");
	spec->extend<unsigned>("version");

	CondAttrListCollection* coll=new CondAttrListCollection(true);
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	coral::Blob& blob=(*attrList)["fSampl"].data<coral::Blob>();
	blob.resize(m_hashMax*sizeof(float));
	float* pblob=static_cast<float*>(blob.startingAddress());
	(*attrList)["version"].setValue(0U);

	std::vector<Identifier> cells;
	float avgFSAMPL = 0;

	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		avgFSAMPL = 0;
		cells =  m_scidtool->superCellToOfflineID (scid);

		for(unsigned cI=0; cI<cells.size(); cI++){
			avgFSAMPL+=input->FSAMPL(cells[cI])/cells.size();
		}
		pblob[scOnlHash]=avgFSAMPL;
	}
  
	coll->add(0,*attrList);
	//delete attrList;//???
	StatusCode sc=detStore()->record(coll,outputName);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
		return NULL;
	}
	return coll;
}

CondAttrListCollection* LArMCToSC::larMinBiasAverageSC(const ILArMinBiasAverage* input, const std::string& outputName) {
        coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
        spec->extend("MinBiasAverage", "blob");
        spec->extend<unsigned>("version");

        CondAttrListCollection* coll=new CondAttrListCollection(true);
        coral::AttributeList* attrList = new coral::AttributeList(*spec);
        coral::Blob& blob=(*attrList)["MinBiasAverage"].data<coral::Blob>();
        blob.resize(m_hashMax*sizeof(float));
        float* pblob=static_cast<float*>(blob.startingAddress());
        (*attrList)["version"].setValue(0U);

        std::vector<Identifier> cells;
        float avgLArMinBiasAverage = 0;

        for (unsigned hs=0;hs<m_hashMax;++hs) {
                const Identifier scid=m_scid->cell_id(hs);
                const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
                const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
                avgLArMinBiasAverage = 0;
                cells =  m_scidtool->superCellToOfflineID (scid);

                for(unsigned cI=0; cI<cells.size(); cI++){
                        avgLArMinBiasAverage+=input->minBiasAverage(cells[cI]);
                }
                pblob[scOnlHash]=avgLArMinBiasAverage;
        }

        coll->add(0,*attrList);
        StatusCode sc=detStore()->record(coll,outputName);
        if (sc.isFailure()) {
                msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
                return NULL;
        }
        return coll;
}

CondAttrListCollection* LArMCToSC::DAC2uASC(const ILArDAC2uA* input, const std::string& outputName) {
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("DAC2uA", "blob");
	spec->extend<unsigned>("version");

	CondAttrListCollection* coll=new CondAttrListCollection(true);
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	coral::Blob& blob=(*attrList)["DAC2uA"].data<coral::Blob>();
	blob.resize(m_hashMax*sizeof(float));
	float* pblob=static_cast<float*>(blob.startingAddress());
	(*attrList)["version"].setValue(0U);

	std::vector<Identifier> cells;
	float avgDAC2UA = 0;

	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		avgDAC2UA = 0;
		cells =  m_scidtool->superCellToOfflineID (scid);
		for(unsigned cI=0; cI<cells.size(); cI++){
			avgDAC2UA+=input->DAC2UA(cells[cI])/cells.size();	
		}
		pblob[scOnlHash]=avgDAC2UA;
	}
  
	coll->add(0,*attrList);
	//delete attrList;//???
	StatusCode sc=detStore()->record(coll,outputName);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
		return NULL;
	}
	return coll;
}

CondAttrListCollection* LArMCToSC::uA2MeVSC(const ILAruA2MeV* input, const std::string& outputName) {

	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("uA2MeV", "blob");  
	spec->extend<unsigned>("version");
	CondAttrListCollection* coll=new CondAttrListCollection(true);
	coral::AttributeList* attrList = new coral::AttributeList(*spec);
	coral::Blob& blob=(*attrList)["uA2MeV"].data<coral::Blob>();
	blob.resize(m_hashMax*sizeof(float));
	float* pblob=static_cast<float*>(blob.startingAddress());
	(*attrList)["version"].setValue(0U);

	std::vector<Identifier> cells;
	float avgUA2MeV = 0;

	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		cells =  m_scidtool->superCellToOfflineID (scid);
		avgUA2MeV = 0;
		for(unsigned cI=0; cI<cells.size(); cI++){
			avgUA2MeV+=input->UA2MEV(cells[cI])/cells.size();
		}	
		pblob[scOnlHash]=avgUA2MeV;
    
	}
	coll->add(0,*attrList);
	//delete attrList;//???
	StatusCode sc=detStore()->record(coll,outputName);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << std::endl;
		return NULL;
	}
	return coll;
}


CondAttrListCollection* LArMCToSC::pedestalSC(const std::string& outputName) {    
	coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
	spec->extend("Pedestal", "blob");
	spec->extend("PedestalRMS", "blob");
	spec->extend<unsigned>("version");

	CondAttrListCollection* collPed=new CondAttrListCollection(true);

	unsigned int gain = 0;
	coral::AttributeList* attrList = new coral::AttributeList(*spec);               
	(*attrList)["version"].setValue(0U);                               
	coral::Blob& blobPed=(*attrList)["Pedestal"].data<coral::Blob>();
	coral::Blob& blobRMS=(*attrList)["PedestalRMS"].data<coral::Blob>();
	blobPed.resize(m_hashMax*sizeof(float));
	blobRMS.resize(m_hashMax*sizeof(float));
	float* pblobPed=static_cast<float*>(blobPed.startingAddress());
	float* pblobRMS=static_cast<float*>(blobRMS.startingAddress());
	std::vector<Identifier> cells;
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);
		const IdentifierHash scOnlHash=m_onlSCID->channel_Hash(scOnlId);
		cells =  m_scidtool->superCellToOfflineID (scid);
		pblobPed[scOnlHash]=1000;
		pblobRMS[scOnlHash]=0;
	}//end loop over hash ids
	collPed->add(gain,*attrList);
   
	StatusCode sc=detStore()->record(collPed,outputName);//"/LAR/ElecCalibFlat/Pedestal");
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to record CondAttrListCollection for pedestal with key " << outputName << std::endl;
		return NULL;
	}
	return collPed;
}

void LArMCToSC::minBiasCheck(const ILArMinBias* /*input*/) {    
	std::vector<Identifier> cells;
	//double inMinBias = 0;
	unsigned hMax=m_cid->calo_cell_hash_max();
	const TileID *tileID = m_cid->tile_idHelper();
	hMax-=tileID->cell_hash_max();
        /*
	for (unsigned hs=0;hs<hMax;++hs) 
		{
			const Identifier cid=m_cid->cell_id(hs);
			const CaloDetDescrElement* dde = m_em_mgr->get_element(cid);

			 if(m_cid->is_em_barrel(cid) && m_cid->sampling(cid)==2 && m_cid->eta(cid)==0 && m_cid->pos_neg(cid) > 0)
			 	{
			 	  inMinBias=input->minBiasRMS(cid);
			 	}
		}
        */
}

void LArMCToSC::writeCellInfo() {    
	unsigned hMax=m_cid->calo_cell_hash_max();
	const TileID *tileID = m_cid->tile_idHelper();
	hMax-=tileID->cell_hash_max();
	std::ofstream myfile;
	myfile.open ("cellsEMBSamp2.txt");
	std::ofstream rawEtaFile;
	rawEtaFile.open ("cellsRawEtaEMBSamp2.txt");
	for (unsigned hs=0;hs<hMax;++hs) {
		const Identifier cid=m_cid->cell_id(hs);
		const CaloDetDescrElement* dde = m_em_mgr->get_element(cid);
    
		//if(m_cid->is_em_barrel(cid) && m_cid->sampling(cid)==2 && m_cid->eta(cid)==0 && m_cid->pos_neg(cid) > 0 && m_cid->region(cid)==0){
		if(m_cid->is_fcal(cid)){
			myfile << "id,region,samp,eta:" << cid.get_identifier32().get_compact() << " " << m_cid->pos_neg(cid) << " " << m_cid->region(cid) << " " << m_cid->sampling(cid) << " " << m_cid->eta(cid) << "\n";
			rawEtaFile << "id,region,samp,eta:" << cid.get_identifier32().get_compact() << " " << m_cid->pos_neg(cid) << " " << m_cid->region(cid) << " " << m_cid->sampling(cid) << " " << dde->eta_raw() << "\n";
		}
	}
	myfile.close();
}

void LArMCToSC::writeSCellInfo() {    
  
	std::ofstream myfile;
	myfile.open ("scInfo.txt");
	for (unsigned hs=0;hs<m_hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
		const HWIdentifier scOnlId=m_scCablingTool->createSignalChannelID(scid);

		if(m_scid->is_hec(scid)){
			myfile << scOnlId.get_identifier32().get_compact() << " " << scid.get_identifier32().get_compact() << " " << m_scid->pos_neg(scid) << " " << m_scid->region(scid) << " " << m_scid->sampling(scid) << " " << dde->eta_raw() << " " << dde->eta() << " " << dde->phi_raw() << " " << sin(atan(exp(-1*dde->eta_raw()))*2) << " " << dde->sinTh() << "\n";
		}
	}
	myfile.close();
}


StatusCode LArMCToSC::stop() {

	CHECK( m_scidtool.retrieve() );
	CHECK(m_scCablingTool.retrieve());
	StatusCode sc=detStore()->retrieve(m_scid);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get SC Offline ID" << std::endl;
		return sc;
	}
	sc=detStore()->retrieve(m_onlSCID);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArOnline_SCID" << std::endl;
		return sc;
	}
	sc = detStore()->retrieve(m_sem_mgr);                
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to get CaloDetDescrManager" << std::endl;
		return StatusCode::FAILURE;                                 } 

	sc = detStore()->retrieve(m_em_mgr);                
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Unable to get CaloDetDescrManager" << std::endl;
		return StatusCode::FAILURE;     
	} 
	CHECK( m_adc2mevtool.retrieve() );
  
	m_hashMax=m_scid->calo_cell_hash_max();
	const Tile_SuperCell_ID *tileID = m_scid->tile_idHelper();
	m_hashMax-=tileID->cell_hash_max();

	msg(MSG::INFO) << "hashmax =  " << m_hashMax << std::endl;
	msg(MSG::INFO) << "hashmax tile=  " << tileID->cell_hash_max() << std::endl;

	sc=detStore()->retrieve(m_cid);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get CaloCell Offline ID" << std::endl;
		return sc;
	}

	// Get the noise from the xls noise table
	FillNoise();
	FillMinBias();
	FillLArMinBias();

	//CondAttrListCollection* coll;
  
	const ILAruA2MeV* uA2MeVMC;
	sc=detStore()->retrieve(uA2MeVMC,m_uA2MeVInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LAruA2MeV with key " << m_uA2MeVInput << std::endl;
		return sc;
	}
	/*coll=*/uA2MeVSC(uA2MeVMC, "/LAR/ElecCalibMCSC/uA2MeV");

	const ILArDAC2uA* DAC2uAMC;
	sc=detStore()->retrieve(DAC2uAMC,m_DAC2uAInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArDAC2uA with key " << m_DAC2uAInput << std::endl;
		return sc;
	}
	/*coll=*/DAC2uASC(DAC2uAMC, "/LAR/ElecCalibMCSC/DAC2uA");
 
	const ILArfSampl* fSamplMC;
	sc=detStore()->retrieve(fSamplMC,m_fSamplInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArfSampl with key " << m_fSamplInput << std::endl;
		return sc;
	}
	/*coll=*/fSamplSC(fSamplMC, "/LAR/ElecCalibMCSC/fSampl");
 
	const ILArNoise* noiseMC;
	sc=detStore()->retrieve(noiseMC,m_noiseInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArNoise with key " << m_noiseInput << std::endl;
		return sc;
	}
	/*coll=*/noiseSC(noiseMC, "/LAR/ElecCalibMCSC/Noise");

	const ILArRamp* rampMC;
	sc=detStore()->retrieve(rampMC,m_RampInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArRamp with key " << m_RampInput << std::endl;
		return sc;
	}
	/*coll=*/rampSC(rampMC, "/LAR/ElecCalibMCSC/Ramp");

	minBiasSC("/LAR/ElecCalibMCSC/MinBias");

	// const ILArPedestal* pedestalMC;
	// sc=detStore()->retrieve(pedestalMC,m_pedestalInput);
	// if (sc.isFailure()) {
	//   msg(MSG::ERROR) << "Failed to get LArPedestal with key " << m_pedestalInput << std::endl;
	//   return sc;
	// }
	/*coll=*/pedestalSC("/LAR/ElecCalibMCSC/Pedestal");
 
	const ILArMinBiasAverage* larMinBiasAverageMC;
	sc=detStore()->retrieve(larMinBiasAverageMC,m_MinBiasAverageInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get MinBiasAverage with key " << m_MinBiasAverageInput << std::endl;
		return sc;
	}
	larMinBiasAverageSC(larMinBiasAverageMC, "/LAR/ElecCalibMCSC/MinBiasAverage");

	const ILArAutoCorr* autoCorrMC;
	sc=detStore()->retrieve(autoCorrMC,m_autoCorrInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArAutoCorr with key " << m_autoCorrInput << std::endl;
		return sc;
	}
	/*coll=*/autoCorrSC(autoCorrMC, "/LAR/ElecCalibMCSC/AutoCorr");

	const ILArMinBias* minBiasMC;
	sc=detStore()->retrieve(minBiasMC,m_MinBiasInput);
	if (sc.isFailure()) {
		msg(MSG::ERROR) << "Failed to get LArMinBias with key " << m_MinBiasInput << std::endl;
		return sc;
	}
	minBiasCheck(minBiasMC);
	writeCellInfo();
	writeSCellInfo();
  
	return StatusCode::SUCCESS; 
}

