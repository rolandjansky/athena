/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class CaloNoiseTool
    @brief Tool to provide electronic and pile up noise in MeV
    @author David Rousseau
    @author Mathieu Lechowski
*/

#ifndef CALOTOOLS_CALONOISETOOL_H
#define CALOTOOLS_CALONOISETOOL_H

class Identifier; 
class StoreGateSvc; 

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArID.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "CaloIdentifier/LArID_Exception.h"
 
#include "CaloIdentifier/CaloGain.h"

#include "StoreGate/DataHandle.h"
#include "LArElecCalib/ILArAutoCorr.h" 
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArMinBias.h"

#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h" 

#include "TileConditions/TileInfo.h"
#include "TileConditions/ITileCellNoiseTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondIdTransforms.h"

#include "CaloEvent/CaloCell.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "GaudiKernel/IIncidentListener.h"


typedef std::vector< std::vector<float> > VectorContainer;
typedef std::vector< float >              SingleContainer;

enum VALUE_ENUM{BADVALUE=-999,BADVALUE_TO_RETURN=-1};
enum DATABASE_ENUM{iADC2MEV,iSIGMANOISE,iAUTOCORR,iOFC,iSHAPE,
		   iMINBIASRMS,iFSAMPL,nDATABASE};

class CaloNoiseTool: public AthAlgTool,
	             virtual public ICaloNoiseTool,
		     public IIncidentListener 
{
private: 
  typedef float (CaloNoiseTool::*GETNOISE_CDDE)(const CaloDetDescrElement*,
						const CaloGain::CaloGain,
						const float,
						const int);
  typedef float (CaloNoiseTool::*GETNOISE_CELL)(const CaloCell*,
						const CaloGain::CaloGain,
						const float,
						const int);
  
  float elecNoiseRMS(const CaloCell* caloCell,
		     const CaloGain::CaloGain gain,
		     const float Nminbias,
		     const int step);
  float pileupNoiseRMS(const CaloCell* caloCell,
		       const CaloGain::CaloGain gain,
		       const float Nminbias,
		       const int step);
  float totalNoiseRMS(const CaloCell* caloCell,
		      const CaloGain::CaloGain gain,
		      const float Nminbias,
		      const int step);
  float pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
		       const CaloGain::CaloGain gain,
		       const float Nminbias,
		       const int step);
  float totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
		      const CaloGain::CaloGain gain,
		      const float Nminbias,
		      const int step);
  GETNOISE_CDDE m_CachedGetNoiseCDDE;
  GETNOISE_CELL m_CachedGetNoiseCELL;
  
  std::string   m_ReturnNoiseName;
  int m_gain_from_joboption;


//Constants
  static const int m_nCalos=4;// number of calorimeters

  float m_LowGainThresh[m_nCalos];
  float m_HighGainThresh[m_nCalos];  
  CaloGain::CaloGain  m_highestGain[m_nCalos];

//Switches
  bool m_WithOF;

//Identifiers
  const AtlasDetectorID* m_atlas_id; 

  const CaloIdManager* m_calo_id_man;

  //  const LArDetDescrManager* m_lar_dd_man; 
  const LArEM_ID*    m_lar_em_id; 
  const LArHEC_ID*   m_lar_hec_id;
  const LArFCAL_ID*  m_lar_fcal_id; 
   
  //const TileDetDescrManager* m_tile_dd_man; 
  const TileID*      m_tile_id;

  const CaloDetDescrManager* m_calo_dd_man; 
  const CaloCell_ID* m_calocell_id;

  IdentifierHash m_LArHashMax;
  IdentifierHash m_TileHashMax;
  IdentifierHash m_CaloHashMax;
  IdentifierHash m_CaloHashMin;

  ToolHandle<LArCablingLegacyService> m_cablingService;

  std::string     m_tileInfoName;
  const TileInfo* m_tileInfo;

  ToolHandle<ITileCellNoiseTool> m_tileCellNoise{this,
      "TileCellNoiseTool", "TileCellNoiseTool", "Tile Cell noise tool"};

  ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
      "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

  ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
      "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

  ToolHandle<TileCondIdTransforms> m_tileIdTransforms{this,
      "TileCondIdTransforms", "TileCondIdTransforms",
      "Tile tool to tranlate hardware identifiers to the drawerIdx, channel, and adc"};

//Database  

  bool m_retrieve[15];

  float  m_Adc2MeVFactor;
  double m_AdcPerMev;
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;

  const  DataHandle<ILArNoise> m_dd_noise; 
  float  m_SigmaNoise;
  double m_CNoise;
 
  bool m_useRMSpedestal;
  const DataHandle<ILArPedestal> m_dd_pedestal;
  float m_RMSpedestal;

  const DataHandle<ILArAutoCorr> m_dd_acorr;
  ILArAutoCorr::AutoCorrRef_t m_AutoCorr;
  float m_c[32][32];

  ToolHandle<ILArOFCTool> m_OFCTool;
  ILArOFCTool::OFCRef_t m_OFC;
   
  const DataHandle<ILArShape> m_dd_shape;
  ILArShape::ShapeRef_t m_Shape; 
  int m_nsamples;

  const DataHandle<ILArfSampl> m_dd_fsampl;
  float m_fSampl;

  const DataHandle<ILArMinBias> m_dd_minbias; 
  float m_MinBiasRMS;
  float m_Nminbias;
  float m_Nminbias_usedForCache;
// 
  int m_WorkMode;
  bool m_NormalizeAutoCorr;
  //bool m_Geant3;
  bool m_UseLAr;
  bool m_UseTile;
  bool m_UseSymmetry;
  bool m_DumpDatabase[3];
  bool m_DumpDatabaseHG,m_DumpDatabaseMG,m_DumpDatabaseLG;
  bool m_isMC;
  bool m_loadAtBegin; // load constant at BeginRun time.
 
  std::string m_keyNoise, m_keyPedestal, m_keyAutoCorr;
  std::string m_keyShape, m_keyfSampl, m_keyMinBias;

  unsigned int m_Nmessages_forTilePileUp;

//DIAGNOSTIC (for TESTBEAM, since limited to 5000 cells)
  bool m_noiseOK;
  bool m_DiagnosticHG,m_DiagnosticMG,m_DiagnosticLG;
  bool m_diagnostic[3];
  int m_nCellsWithProblem[3];
  int m_nReason[5000][3];
  int m_itReason[10][3];
  int m_idHash[5000][3];
  int m_reason[5000][10][3];
  std::string m_reasonName[10];

  // state of the Cache
  bool m_cacheValid ;

  int m_deltaBunch;
  unsigned int m_firstSample;

//Containers    
  std::vector<IdentifierHash>  m_indexContainer;
    // = vector indexed with all the hashids, 
    //containing which index (of container) should be used with a hashid
  std::vector<IdentifierHash>  m_idSymmCaloHashContainer;
    //used only to initialize m_indexOfContainer 

  // experimental
  // typedef std::map<IdentifierHash, unsigned int> CaloHashIdMap;
  // CaloHashIdMap m_idSymmCaloHashMap ;

  VectorContainer              m_elecNoiseRAWContainer;
  VectorContainer              m_elecNoiseCELLContainer;
  SingleContainer              m_pileupNoiseContainer;
  //SingleContainer              m_scaleContainer;
  VectorContainer              m_adc2mevContainer;


//Functions
  StatusCode initialize();
  StatusCode retrieveDatabase();
  StatusCode initContainers();
  StatusCode initData();
  StatusCode initAdc2MeV();
  StatusCode initElecNoise();
  StatusCode initPileUpNoise();

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  bool checkIfConnected(const Identifier &id);
  void commonCalculations(float & OFC_AC_OFC,float & OFC_OFC, int icase, unsigned int firstSample=0);
  StatusCode retrieveCellDatabase(const IdentifierHash & idCaloHash,
				  const Identifier & id,int igain,
				  const float &Nminbias,
				  std::string function_name);
  StatusCode checkCellDatabase(const Identifier & id, int igain,
			       std::string function_name);
  void updateDiagnostic(int reason,std::string reason_name,int igain);

  std::vector<float> 
    calculateElecNoiseForLAR(const IdentifierHash &idCaloHash,
			     const CaloCell_ID::SUBCALO iCalo,
			     const float &Nminbias);
  std::vector<float> 
    calculateElecNoiseForTILE(const IdentifierHash &idCaloHash);

  float
    calculatePileUpNoise(const IdentifierHash &idCaloHash,
			 const float &Nminbias);

  void initIndex();
  int  index(const IdentifierHash &idCaloHash);  
  CaloCell_ID::SUBCALO caloNum(const IdentifierHash idCaloHash);  
  bool isBadValue(float tested_value); 

  CaloGain::CaloGain estimatedLArGain(const CaloCell_ID::SUBCALO &iCalo,
				      const CaloDetDescrElement* caloDDE,
				      const float &energy,
				      const int &step);
  CaloGain::CaloGain estimatedTileGain(const CaloCell* caloCell,
				       const CaloDetDescrElement* caloDDE,
				       const int &step);


public:    
  
  CaloNoiseTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  //virtual ~CaloNoiseTool();  


//-------------- user interfaces ------------------------------------------

// Note on NMinBias : if you use the interface without it or if you take -1, 
//                    the returned data will be the one calculated at the 
//                    initialization with the default value (0) or the 
//                    specified one (with the property m_Nminbias).
//                    In all cases, the NMinBias used to calculate the OFCs 
//                    is NOT known by CaloNoiseTool (CaloNoiseTool always 
//                    recomputes OFCs for its own needs)


  
//== just one function for the interface to the CaloNoiseTool ==
  // which noise to return is defined via jobOptions or via the
  // optional parameter CalorimeterNoiseType
  
  float getNoise(const CaloCell* caloCell,
		 CalorimeterNoiseType type=JOBOPTION );
  float getNoise(const CaloDetDescrElement* caloDDE,
		 CalorimeterNoiseType type=JOBOPTION );

  bool isEOverNSigma(const CaloCell* caloCell, float sigmaCut , 
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );

  float getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type=JOBOPTION);

  float getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                     CalorimeterNoiseType type=JOBOPTION );


  

//== ELECTRONIC NOISE ==  

  //''''''''' functions to use currently

  float elecNoiseRMS(const CaloCell* caloCell, 
		     const int step=ICaloNoiseToolStep::CELLS);
    //Returns the sigma of the electronic noise, finding itself the right gain 
    //from the energy of the given caloCell.
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  float elecNoiseRMS(const CaloCell* caloCell, 
		     const float Nminbias,
		     const int step=ICaloNoiseToolStep::CELLS);
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  float elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				const int step=ICaloNoiseToolStep::CELLS);
    //Returns the sigma of the electronic noise for the caloDDE of the given 
    //caloCELL and for the highest gain of the considered calorimeter 
    //(eg: HG for LAr, MG for HEC, ...).
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  float elecNoiseRMSHighestGain(const CaloCell* caloCell, 
				const float Nminbias,
				const int step=ICaloNoiseToolStep::CELLS);
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  //''''''''' functions more specialized

  float elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				const int step=ICaloNoiseToolStep::CELLS);
    //Returns the sigma of the electronic noise for the given caloDDE and 
    //for the highest gain of the considered calorimeter (eg: MG for HEC).
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  float elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				const float Nminbias,
				const int step=ICaloNoiseToolStep::CELLS);
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  float elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
		     const CaloGain::CaloGain gain,
		     const float Nminbias,
		     const int step=ICaloNoiseToolStep::CELLS);
    //Returns the sigma of the electronic noise for the given caloDDE, gain and
    //the number of minimum bias events per bunch crossing. 
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
		       const int step=ICaloNoiseToolStep::CELLS);
    //Returns a vector containing the sigma of the electronic noise for the 
    //given caloDDE, for each gain of the considered calorimeter 
    //(eg: vector of 3 sigma3 gains for LAr)
    //Can also return the sigma without the scale applied in LArG3Escale 
    //(meaning at the level of the RawChannels) specifying the step (use ENUM)

  std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
		       const float Nminbias, 
		       const int step=ICaloNoiseToolStep::CELLS);
    //same as above, but must specify the number of minimum bias events 
    //per bunch crossing 
    // (slower since do not use stored data -> calculate again)

  VectorContainer* elecNoiseRMSContainer(const int &iCalo);
    //TO BE USED ONLY FOR SPECIAL CASES
    //Returns a vector containing, for each Hash Identifier of the calorimeter 
    //iCalo, a vector  containing the sigma of the electronic noise for all 
    //gains. 
    //So it's a vector (indexed by hashID) of vectors (sigma for each gain)
    //(see typedef VectorContainer).


//== PILEUP NOISE ==  

  float pileupNoiseRMS(const CaloCell* caloCell, 
		       const float Nminbias=-1); 
  float pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
		       const float Nminbias=-1);
  //Nminbias is the number of minimum-bias events per bunch-crossing
  // (default is 0 (set in the constructor) ) 

//== TOTAL NOISE ==   (only at the "Cells" step)

  float totalNoiseRMS(const CaloCell* caloCell, 
		      const float Nminbias=-1); 
  float totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
		      const CaloGain::CaloGain gain,
		      const float Nminbias=-1);
  float totalNoiseRMSHighestGain(const CaloCell* caloCell, 
				 const float Nminbias=-1);
  float totalNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
				 const float Nminbias=-1);



//== GAIN == 
  CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
				   const int &step);
  CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
				   const CaloDetDescrElement* caloDDE,
				   const int &step);
  CaloGain::CaloGain estimatedGain(const CaloDetDescrElement* caloDDE,
				   const float &energy,
				   const int &step);

//== ADDITIONNAL STUFF ==
  // these functions will be replaced soon by dedicated tools,
  // so please AVOID TO USING THEM

  //float eScale(const CaloDetDescrElement* caloDDE);
    //Returns the scale between RawChannels and Cells, i.e 
    //what returns LArG3Escale

  float adc2mev(const CaloDetDescrElement* caloDDE,
		const CaloGain::CaloGain gain);
    //Returns adc2mev factor for the given caloDDE and gain

  float adc2mev(const Identifier& id,const CaloGain::CaloGain gain);
    //Returns adc2mev factor for the given ID and gain
    //SHOULD NOT BE USED ANYMORE, prefer the one above with caloDDE


  /** 
    implement IIncidentListener interface 
  */
  virtual void handle(const Incident&);


};

//------------------------------------------------------------

inline bool 
CaloNoiseTool::isBadValue(float tested_value) 
{
  if(tested_value<BADVALUE+1) return true;
  return false;
}

//////////////////////////////////////////////////

inline CaloCell_ID::SUBCALO  
CaloNoiseTool::caloNum(const IdentifierHash idCaloHash)
{
  return 
    static_cast<CaloCell_ID::SUBCALO>(m_calocell_id->sub_calo(idCaloHash));
}

#endif

