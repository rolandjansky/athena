/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOCALIBRATION_EGAMMAHITSCALIBRATION
#define TRIGT2CALOCALIBRATION_EGAMMAHITSCALIBRATION

#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "TrigT2CaloCalibration/EgammaHitsShowerDepth.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

// Specific for this calibration
#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloConditions/Array.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODTrigCalo/TrigEMCluster.h"

// Just for the pointer definition
//class TrigEMCluster;


/** General Interface for calibrations at the LVL2 Egamma Calo Fex algo */
class EgammaHitsCalibration  : virtual public IEgammaCalibration,
	virtual public AthAlgTool, public CaloRec::ToolWithConstantsMixin {
	public:
	/** Constructor */
	EgammaHitsCalibration (const std::string & type, const std::string & name,
		const IInterface* parent) : AthAlgTool(type,name,parent),
	CaloRec::ToolWithConstantsMixin(),
  m_showerDepth(0){
    declareInterface<IEgammaCalibration>(this);
    declareConstant("correction",      m_correction);
    declareConstant("sampling_depth",  m_sampling_depth);
    declareConstant("eta_start_crack", m_eta_start_crack);
    declareConstant("eta_end_crack",   m_eta_end_crack);
    declareConstant("etamax",          m_etamax);
    declareConstant("use_raw_eta",     m_use_raw_eta);
    declareConstant("preserve_offset", m_preserve_offset);
    declareConstant("fix_v6_pathologies", m_fix_v6_pathologies, true);
    finish_ctor();
	}
	virtual ~EgammaHitsCalibration () { }

	/** Initialization of the tool */
	StatusCode initialize();

	/** Finalization of the tool */
	StatusCode finalize();

	/** Set Property necessary */
        using AthAlgTool::setProperty;
	StatusCode setProperty(const std::string&,
		const std::string&);
	StatusCode setProperty(const Property&);

	/** method to perform the correction. The correction
		type is defined by the tool which also uses
		this interface. In some cases, the tool needs
		more than the cluster to perform the calibration.
		This can be passed via the void pointer */
	virtual void makeCorrection(xAOD::TrigEMCluster*, const void* v=NULL);

	private:
	MsgStream* m_log;
	// Correction Variables
	CaloRec::Array<3>  m_correction;// on CalibHits m_correction' size is 3
  CaloRec::Array<2>  m_sampling_depth; 
	CaloRec::WritableArrayData<1>* m_interp_barriers;
	float m_eta_start_crack;
	float m_eta_end_crack;
	float m_etamax;
	bool m_use_raw_eta;
	bool m_preserve_offset;
	bool m_fix_v6_pathologies;

  EgammaHitsShowerDepth *m_showerDepth;

};

#endif
