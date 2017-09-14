/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     IAlgToolCalo.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   M.P. Casado
 
 PURPOSE:  Serve as a basis for the TrigT2Calo Tools
	   providing some commom framework basis, like
	   data access via appropriated pointers and
	   ByteStream converter objects. Also, time
	   measurement items.
 KNOWTOINHERIT : TrigT2CaloEgamma/EgammaSamp2Fex,
	   EgammaSamp1Fex, EgammaEmEnFex, EgammaHadEnFex
	   TrigT2CaloTau/TauSamp2Fex,TauSamp1Fex,
	   TauEmEnFex,TauHadEnFex
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_IALGTOOLCALO_H 
#define TRIGT2CALOCOMMON_IALGTOOLCALO_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "Identifier/IdentifierHash.h"
#include "LArRawUtils/LArTT_Selector.h"

#include "LArRecEvent/LArCellCollection.h"

#include "TileEvent/TileCell.h"

#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/T2Calibration.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "TrigSteeringEvent/Enums.h"

//class TrigEMCluster;
class TrigTauCluster;
class T2CaloConfig;

static const InterfaceID IID_IAlgToolCalo("IAlgToolCalo",1,0);

/** Base Class for Tools used for Egamma and Tau Feature
	Extraction Algorithms */
class IAlgToolCalo: public virtual IAlgTool, 
                       public AthAlgTool {
  public:
    /** Constructor */
    IAlgToolCalo(const std::string & type, const std::string & name,
                 const IInterface* parent) : AthAlgTool(type,name,parent),
		 m_timersvc("TrigTimerSvc","IAlgToolCalo"),
		 m_geometryTool("T2GeometryTool/T2GeometryTool", this ),
		 m_data("TrigDataAccess/TrigDataAccess"),
                 m_caloDDE(0), m_cellkeepthr(1e5) {
	 declareInterface<IAlgToolCalo>(this);
         declareProperty("SaveCellsInContainer",m_saveCells=false,"Enables saving of the RoI Calorimeter Cells in StoreGate");
         declareProperty("TrigTimerSvc",m_timersvc,"Trigger Timer Service for benchmarking algorithms");
         declareProperty("T2GeometryTool",m_geometryTool,
		"Tool to check that a cells are contained in a given cluster - for different cluster sizes");
         declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");
         declareProperty("ThresholdKeepCells",m_cellkeepthr,"Threshold to keep cells into container");
    }
    /** Destructor */
    virtual ~IAlgToolCalo() { }
    /** Interface ID for a virtual class */
    static const InterfaceID& interfaceID();


    /** The same function should initialize all Tools.
    It will also provide stardard timers for all tools */
    StatusCode initialize();
    /** Finalize method */
    StatusCode finalize();
    /** @brief This is the execute method for Egamma Algorithms. These
    * interface must be common to allow all tools to be called
    * within T2CaloEgamma::hltExecute as a vector of IAlgToolCalos. 
    * @param[out] ptrigEMCluster : cluster to be filled with FEX results.
    * @param[in] eta/phi-min/max : limits of RoI.
    */

    virtual StatusCode execute(xAOD::TrigEMCluster& /*ptrigEMCluster*/,
			       const IRoiDescriptor& /*roi*/ ) {return StatusCode::SUCCESS;}

    /// obsolete 
    virtual StatusCode execute(xAOD::TrigEMCluster& /*ptrigEMCluster*/
		, double /*etamin*/, double /*etamax*/, double /*phimin*/
		, double /*phimax*/){return StatusCode::SUCCESS;}
    /** @brief This is the execute method for Tau Algorithms. These
    * interface must be common to allow all tools to be called
    * within T2CaloTau::hltExecute as a vector of IAlgToolCalos. 
    * @param[out] ptrigTauCluster : cluster to be filled with FEX results.
    * @param[in] eta/phi-min/max : limits of RoI.
    */
    virtual HLT::ErrorCode execute(TrigTauCluster& /*ptrigTauCluster*/,
				   const IRoiDescriptor& /*roi*/ ) {return HLT::OK;} 

    /// obsolete
    virtual HLT::ErrorCode execute(TrigTauCluster& /*ptrigTauCluster*/
		,double /*etamin*/, double /*etamax*/, double /*phimin*/
		,double /*phimax*/, double /*RoIeta*/, double /*RoIphi*/) 
		{return HLT::OK;} 
    /** Expose error for a given IAlgToolCalo */
    uint32_t report_error() const { return m_error; };

    /** enumerate tau-specific errors */
    enum TAUCLUSTERROR{
      FAILPRESEED=31,
      FAILSEED=30,
      HADS1E0=11,
      HADS2E0=10,
      HADS3E0=9,
      EMS0E0=15,
      EMS1E0=14,
      EMS2E0=13,
      EMS3E0=12
    };

    /** Method to get the caloDDE for some important cell in cluster */
    const CaloDetDescrElement* getCaloDetDescrElement() const { return m_caloDDE; }

    /** Sets the pointer of the cell container */
    void setCellContainerPointer(CaloCellContainer** p )
	{ m_CaloCellContPoint = p; }

    /** Method to set the caloDDE for some important cell in cluster */
    void setCaloDetDescrElement(const CaloDetDescrElement *caloDDE) {
      m_caloDDE = caloDDE;    
    }
  protected:

    /** methods to manage error bit inside the tools: general and tau-specific */
    void SetClusterError(unsigned int bit) { m_error |= (0x1<<bit); }
    void ClearClusterError(unsigned int bit) { m_error &= ~(0x1<<bit); }
    void SetClusterError(TAUCLUSTERROR biterr) { SetClusterError((unsigned int)biterr); }
    void ClearClusterError(TAUCLUSTERROR biterr) { ClearClusterError((unsigned int)biterr); }
  

#ifndef NDEBUG
    /** Very useful function to check about the Cluster structure.
    It prints out a grid of cells eta and phi positions and energies.
    Only exist in DEBUG versions of the code. */
    void PrintCluster(const double energyFromAlg, const int nCaloType,const int nCaloSamp,
		const CaloSampling::CaloSample, const CaloSampling::CaloSample );
#endif
         /** Store All Calorimeter Cells in RoI in an StoreGate Container. */
         void storeCells(void);

	/** iterators to LArCells. To be used when online
	LArCellCont (fast ByteStream Decoder) are to be used.
	Used as input for TrigDataAccess::LoadCollections */
	LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
	LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
	LArTT_Selector<LArCellCont>::const_iterator m_it;
	/** iterators to TileCells. To be used when on/offline
	TileCellCont/IDC (fast/slow ByteStream Decoder) are to be used.
	Used as input for TrigDataAccess::LoadCollections */
	TileCellCollection::const_iterator m_itBegin;
	TileCellCollection::const_iterator m_itEnd;
	TileCellCollection::const_iterator m_itt;

	/** DataHandle to TrigTimerSvc */
	ServiceHandle<ITrigTimerSvc> m_timersvc;
	/** Timing measure items in all IAlgToolCalos. 
	m_timer[0] is the complete exec timer. m_timer[1] is the
	Region Selector timer, m_timer[2] is the LoadCollection
	timer (Data Preparation), m_timer[3] is the real algorithmic
	timer and m_timer[4] is the saving data (to TrigCaloEvent
	object) timer. */
	TrigTimer* m_timer[5];

	/** Objects will need T2Calibration. So far, a given tool will
	not need more than one of such objects. */
	T2Calibration *m_calib;
	/** Input objects to calibration. This will be changed in such
	a way the the calibration tool will have to know alone about
	such properties. */
	std::vector<float> m_limit;
	std::vector<int> m_dimension;
	std::vector<float> m_correction;

	/** Objects will need T2GeometryTool (maybe two instances 
	for EmEn and three for HadEn - to be studied) */
	ToolHandle<IT2GeometryTool> m_geometryTool;
	/** Object  that provides data access in a Region of
	Interest. See TrigDataAccess for more details. */
	ToolHandle<ITrigDataAccess> m_data;
	/** Calorimeter Id Manager for calorimeter part
	determination (Barrel versus EndCap) */
	const DataHandle<CaloIdManager>        m_larMgr;
	/** (don't by default) Save Calorimeter Cells in StoreGate
	for a posteriori analysis */
        bool m_saveCells;
        /** Pointer of the cell container */
        CaloCellContainer** m_CaloCellContPoint;
        // Objects that the Tools should not worry about
  protected:
	bool m_lardecoded, m_tiledecoded;
	/** error control */
        uint32_t m_error;
        /** caloDDE (of the cluster 2nd layer hottest cell) */
        const CaloDetDescrElement* m_caloDDE;      
        /** Threshold to keep cells  in RoI */
	float m_cellkeepthr;
  private:
};

inline const InterfaceID& IAlgToolCalo::interfaceID()
{
        return IID_IAlgToolCalo;
}

#endif
