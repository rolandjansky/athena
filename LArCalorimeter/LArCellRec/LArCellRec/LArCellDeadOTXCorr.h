/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  @file  LArCellDeadOTXCorr.h
 *  @brief  CaloCell energy correction tool for missing FEBs
 *
 *
 *  @author  Deepak Kar
 *
 *  @date    08/01/09
 *
 *  @internal
 *     Created :  08/01/09
 * Last update :  07/21/2010 10:49:12 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */


#ifndef LARCELLDEADOTXCORR_H
#define LARCELLDEADOTXCORR_H 

// CaloCell Energy Correction Tool for Missing FEBs
// by Deepak Kar with help from Guillaume Unal and Damien Prieur
// Version 1, Date Aug 1, 2009


// Headerfile

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/Identifier.h"
#include "CaloConditions/Array.h"
#include "CaloRec/ToolWithConstantsMixin.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include <string>
#include <vector>
#include <map>

class LArCablingService;
class StoreGateSvc;
class CaloCell_ID;
class LArOnlineID;
class CaloLVL1_ID;
class ILArBadChanTool;
class CaloTriggerTowerService;
// class L1CaloTTIdTools;
class CaloIdManager;
class L1CaloCondSvc;


/**
 * @class LArCellDeadOTXCorr
 * @brief Tool used to correct energy in missing FEBs
 *
 * This tool uses the trigger energy in order to fill cell energies in missing FEBs.\n
 * Trigger energy is estimated with a Landau-Landau fit on ADC samples. Corrections depending on eta and on the energy are then applied.\n
 * Known cell energies in the trigger tower are removed to this trigger energy, and the result is uniformly divided into the missing layer.
 */
class LArCellDeadOTXCorr : public AthAlgTool,
	public CaloRec::ToolWithConstantsMixin,
	virtual public ICaloCellMakerTool 
{

	public:

		LArCellDeadOTXCorr(const std::string& type, 
				const std::string& name, 
				const IInterface* parent) ;

		virtual ~LArCellDeadOTXCorr();

		virtual StatusCode initialize() ; 
		virtual StatusCode process( CaloCellContainer * CellCont) ;
		virtual StatusCode finalize();

		// Callback added to handle Data-driven GeoModel initialisation

		virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

		using AthAlgTool::setProperty;

		virtual StatusCode setProperty (const std::string& propname,
				const std::string& value);
		virtual StatusCode setProperty (const Property& p);


	private:


		// get a handle to the tool helper 

		ToolHandle<ILArBadChanTool> m_badChannelTool;
		ToolHandle<LArCablingService> m_cablingService;

                //std::string m_TTLocation;
                SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_TTLocation;
		std::vector<double> m_triggerNoiseCut;
		bool m_useL1CaloDB;

		CaloRec::Array<1> m_etaCalibrationSizes;
		CaloRec::Array<1> m_energyCalibrationTypes;
		CaloRec::Array<1> m_etaCalibrations;
		CaloRec::Array<1> m_energyCalibrations;


		static const std::map<int, int> m_typeSizeMapping;

		//CaloRec::Array<1> m_ignoredTTs;
		std::vector<unsigned int> m_ignoredTTs;


		std::map<Identifier, Identifier> m_cellTTMapping;
		std::map<Identifier, std::pair<unsigned int, int> > m_idIndexMapping;

		const CaloIdManager* m_caloMgr;
		const CaloLVL1_ID* m_lvl1Helper;
		const CaloCell_ID* m_calo_id;
		const LArOnlineID* m_onlineID;
		const CaloLVL1_ID* m_TT_ID;


		L1CaloCondSvc* m_l1CondSvc;
		CaloTriggerTowerService*      m_ttSvc;
		// L1CaloTTIdTools* m_l1CaloTTIdTools;

		double TTID_etaWidth(double eta) const;
		double TTID_phiWidth(double eta) const;
		int TTID_pos_neg_z(double eta) const;
		int TTID_regionIndex(double eta) const;
		int TTID_etaIndex(double eta) const;
		int TTID_phiIndex(double eta, double phi) const;
		static const double BASEDETA;
		static const double BASEDPHI;
		static const double FCALDETA;
		static const double ETAMAXREGION0;
		static const double ETAMAXREGION1;
		static const double ETAMAXREGION2;
		static const double ETAMAXREGION3;
		static const double ETAMIN;
		static const double PHIMIN;


		// functions used to calculate trigger energy from parabola fit
		double getA(double x1, double y1,
				double x2, double y2,
				double x3, double y3);

		double getB(double a,
				double x1, double y1,
				double x2, double y2);

		double getC(double a, double b,
				double x1, double y1);


		/**
		 * @brief  Compute L1 energy with 5 samples around the maximum of the pulse.
		 * @param  ADCsamples : ADC samples of the trigger tower
		 * @param pedestal : pedestal of the tower
		 * @param eta : eta of the tower
		 * @param type : em=0, had=1
		 * @return L1 energy
		 *
		 *
		 * Energy is first estimated with a Landau-Landau fit; then a calibration factor depending on eta and the energy is applied.
		 */
		double getL1Energy(const std::vector<uint_least16_t> & ADCsamples, int pedestal, double eta, int type);

		void getInitialFitParameters(const std::vector<uint_least16_t> & ADCsamples, double & max, double& maxPos, unsigned int& TTADCMaxIndex);

		double getEtaCalibration(double eta, int type);
		double getEnergyCalibration(double eta, int type, double energy);
		double getMaxOverSumRatio(const std::vector<uint_least16_t>& ADCsamples, int pedestal);

		static const std::map<int,int> defineSizeType();

};

#endif // 
