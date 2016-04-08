/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOCALIBRATION_EGAMMATRANSITIONREGIONS
#define TRIGT2CALOCALIBRATION_EGAMMATRANSITIONREGIONS

#include "TrigT2CaloCalibration/IEgammaCalibration.h"
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
class EgammaTransitionRegions  : virtual public IEgammaCalibration,
	virtual public AthAlgTool, public CaloRec::ToolWithConstantsMixin {
	public:
	/** Constructor */
	EgammaTransitionRegions (const std::string & type, const std::string & name,
		const IInterface* parent) : AthAlgTool(type,name,parent),
	CaloRec::ToolWithConstantsMixin() {
    declareInterface<IEgammaCalibration>(this);
    declareConstant("correction",      m_correction);
    declareConstant("etamin_TR00",     m_etamin_TR00);
    declareConstant("etamax_TR00",     m_etamax_TR00);
    declareConstant("etamin_TR08",     m_etamin_TR08);
    declareConstant("etamax_TR08",     m_etamax_TR08);
    declareConstant("use_raw_eta",     m_use_raw_eta);
    finish_ctor();
	}
	virtual ~EgammaTransitionRegions() { }

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
	CaloRec::Array<2>  m_correction;// on CalibHits m_correction' size is 3
	float m_etamin_TR00;
	float m_etamax_TR00;
	float m_etamin_TR08;
	float m_etamax_TR08;
	bool m_use_raw_eta;

};

#endif
