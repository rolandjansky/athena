/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class CaloNoiseToolDB
    @brief Tool to provide electronic and pile up noise in MeV from reading values stored in DB
    @brief  Implementing ICaloNoiseTool interface
    @author  G.Unal
*/

#ifndef CALOTOOLS_CaloNoiseToolDB_H
#define CALOTOOLS_CaloNoiseToolDB_H


//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloNoiseTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h"
#include <string>
#include <boost/multi_array.hpp>
#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArCellHVCorrTool.h"
#include <mutex>
#include <atomic>

#define sqrt2 1.4142135623730950
#define invsqrt2 0.707106781186547524


class CaloCondBlobFlt;


class CaloNoiseToolDB: public AthAlgTool,
	             virtual public ICaloNoiseTool
{
public:    

  typedef std::vector< std::vector<float> > VectorContainer;
  
  CaloNoiseToolDB(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  enum SYSTEM{EMECZNEG = 0,
              EMBZNEG  = 1,
              EMBZPOS  = 2,
              EMECZPOS = 3,
              HEC      =16,
              FCAL     =32,
              TILE     =48};



//-------------- user interfaces ------------------------------------------

// Note on NMinBias : if you use the interface without it or if you take -1, 
//                    the returned data will be the one calculated at the 
//                    initialization with the default value (0) or the 
//                    specified one (with the property m_Nminbias).
// FIXME : to be updated using the proper luminosity information from lumi block
//   (and then should also register a call back on the lumi block for cached data)

  
//== just one function for the interface to the CaloNoiseToolDB ==
  // which noise to return is defined via jobOptions or via the
  // optional parameter CalorimeterNoiseType
  
  virtual float getNoise(const CaloCell* caloCell,
                         CalorimeterNoiseType type=JOBOPTION ) override;
  virtual float getNoise(const CaloDetDescrElement* caloDDE,
                         CalorimeterNoiseType type=JOBOPTION ) override;
  float getNoise(const CaloCell* caloCell, float energy,
                 CalorimeterNoiseType type=JOBOPTION );

  virtual bool isEOverNSigma(const CaloCell* caloCell, float sigmaCut , 
                             CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                             CalorimeterNoiseType type=JOBOPTION ) override;
  float calcSig(double e, double sigma1, double ratio, double sigma2);

  virtual float getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type=JOBOPTION) override;

  virtual float getEffectiveSigma(const CaloCell* caloCell,
                                  CalorimeterNoiseSymmetryHandling symmetryHandling=ONLYRIGHTSIDEINTEGRATION,
                                  CalorimeterNoiseType type=JOBOPTION ) override;



//== ELECTRONIC NOISE ==  

  //''''''''' functions to use currently

  virtual float elecNoiseRMS(const CaloCell* caloCell, 
                             const int step=ICaloNoiseToolStep::CELLS) override;
    //Returns the sigma of the electronic noise, finding itself the right gain 
    //from the energy of the given caloCell.

  virtual float elecNoiseRMS(const CaloCell* caloCell, 
                             const float Nminbias,
                             const int step=ICaloNoiseToolStep::CELLS) override;
  // returns the same as above (Nminbias ignored in this implementation from database)

  virtual float elecNoiseRMSHighestGain(const CaloCell* caloCell, 
                                        const int step=ICaloNoiseToolStep::CELLS) override;
    //Returns the sigma of the electronic noise for the caloDDE of the given 
    //caloCELL and for the highest gain of the considered calorimeter 
    //(eg: HG for LAr, MG for HEC, ...).

  virtual float elecNoiseRMSHighestGain(const CaloCell* caloCell, 
                                        const float Nminbias,
                                        const int step=ICaloNoiseToolStep::CELLS) override;
   // same as above

  //''''''''' functions more specialized

  virtual float elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
                                        const int step=ICaloNoiseToolStep::CELLS) override;
    //Returns the sigma of the electronic noise for the given caloDDE and 
    //for the highest gain of the considered calorimeter (eg: MG for HEC).

  virtual float elecNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
                                        const float Nminbias,
                                        const int step=ICaloNoiseToolStep::CELLS) override;
    //same as above

  virtual float elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
                             const CaloGain::CaloGain gain,
                             const float Nminbias,
                             const int step=ICaloNoiseToolStep::CELLS) override;
    //Returns the sigma of the electronic noise for the given caloDDE, gain 
    // Nminbias is ignored

  float elecNoiseRMS(const CaloDetDescrElement* caloDDE, 
		     const CaloGain::CaloGain gain,
		     const float Nminbias,
		     const float energy,
		     const int step=ICaloNoiseToolStep::CELLS);
    //Returns the sigma of the electronic noise for the given caloDDE in 
    //case of 2-gaussian noise, gain 
    // Nminbias is ignored

  virtual
  std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
                       const int step=ICaloNoiseToolStep::CELLS) override;
    //Returns a vector containing the sigma of the electronic noise for the 
    //given caloDDE, for each gain of the considered calorimeter 
    //(eg: vector of 3 sigma3 gains for LAr)

  virtual
  std::vector<float> 
    elecNoiseRMS3gains(const CaloDetDescrElement* caloDDE,
		       const float Nminbias, 
		       const int step=ICaloNoiseToolStep::CELLS) override;
    //same as above, Nminbias ignored

  virtual
  VectorContainer* elecNoiseRMSContainer(const int &iCalo) override;
    //TO BE USED ONLY FOR SPECIAL CASES
    //Returns a vector containing, for each Hash Identifier of the calorimeter 
    //iCalo, a vector  containing the sigma of the electronic noise for all 
    //gains. 
    //So it's a vector (indexed by hashID) of vectors (sigma for each gain)
    //(see typedef VectorContainer).
    // empty for the time being


//== PILEUP NOISE ==  

  virtual float pileupNoiseRMS(const CaloCell* caloCell, 
                               const float Nminbias=-1) override;
  virtual float pileupNoiseRMS(const CaloDetDescrElement* caloDDE, 
                               const float Nminbias=-1) override;
  //Nminbias is the number of minimum-bias events per bunch-crossing
  // (default is 0 (set in the constructor) ) 

//== TOTAL NOISE ==   (only at the "Cells" step)

  virtual float totalNoiseRMS(const CaloCell* caloCell, 
                              const float Nminbias=-1) override; 
  virtual float totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
                              const CaloGain::CaloGain gain,
                              const float Nminbias=-1) override;
  virtual float totalNoiseRMSHighestGain(const CaloCell* caloCell, 
                                         const float Nminbias=-1) override;
  virtual float totalNoiseRMSHighestGain(const CaloDetDescrElement* caloDDE, 
                                         const float Nminbias=-1) override;



//== GAIN ==   Only there to fulfill the interface, returns INVALID values...
  virtual CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
                                           const int &step) override;
  virtual CaloGain::CaloGain estimatedGain(const CaloCell* caloCell,
                                           const CaloDetDescrElement* caloDDE,
                                           const int &step) override;
  virtual CaloGain::CaloGain estimatedGain(const CaloDetDescrElement* caloDDE,
                                           const float &energy,
                                           const int &step) override;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) override;

private: 
  

  ICalorimeterNoiseTool::CalorimeterNoiseType m_cached;

  // cached noise per cell
  boost::multi_array<float, 2> m_noise;   // float[MaxGains][MaxCells]

  typedef float (CaloNoiseToolDB::*GETNOISE_CDDE)(const CaloDetDescrElement*,
                                                const CaloGain::CaloGain,
                                                const float,
                                                const int);
  typedef float (CaloNoiseToolDB::*GETNOISE_CELL)(const CaloCell*,
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
  float totalNoiseRMS(const CaloDetDescrElement* caloDDE, 
                      const CaloGain::CaloGain gain,
                      const float Nminbias,
                      const float energy,
                      const int step);
  GETNOISE_CDDE m_CachedGetNoiseCDDE;
  GETNOISE_CELL m_CachedGetNoiseCELL;
  
  int m_gain_from_joboption;
  bool m_speedTwoGauss;

//Constants
  static const int m_nCalos=CaloCell_Base_ID::NSUBCALO;// number of calorimeters
  int m_ncell;   // total number of calo cells

  CaloGain::CaloGain  m_highestGain[m_nCalos];

  float m_Nminbias;
  float m_lumi0;

//Functions
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  
// private methods to access database
  float getA(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getB(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getC(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getD(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getE(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getDBNoise(SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain, float lumi, float HVCorr=1.0) const;

  float getA(unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getB(unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getC(unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getD(unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getE(unsigned int cellHash, CaloGain::CaloGain caloGain) const ;
  float getDBNoise(unsigned int cellHash, CaloGain::CaloGain caloGain, float lumi, float HVCorr=1.0) const;

  int checkObjLength(unsigned int cellHash) const;

  SYSTEM caloSystem(unsigned int cellHash, unsigned int& subHash) const;

    //=== callback function to update noise map
    virtual StatusCode updateMap(IOVSVC_CALLBACK_ARGS);
    void updateCache();

    //=== callback function for luminosity storate
    virtual StatusCode updateLumi(IOVSVC_CALLBACK_ARGS);

    //=== callback function for HV changes
    virtual StatusCode clearCache(IOVSVC_CALLBACK_ARGS);

    //=== blob storage
    std::vector<DataHandle<CondAttrListCollection> > m_noiseAttrListColl;
    std::map<SYSTEM, const CaloCondBlobFlt*> m_noiseBlobMap;

    std::atomic<bool> m_cacheValid;

   std::string   m_ReturnNoiseName;

  const CaloIdManager*     m_caloIdMgr = nullptr;
  const CaloCell_ID*       m_calo_id;

  // number of EM systems in SYSTEM enum
  enum { NUM_EM_SYS = 4 };

  // First IdHash for each system in EM Barrel and EndCap (indexed by SYSTEM, only first NUM_EM_SYS SYSTEMs)
  IdentifierHash m_firstSysHash[NUM_EM_SYS];

  std::vector<std::string> m_folderNames;//jobOption
  std::string m_lumiFolderName;


  ToolHandle<ILArCellHVCorrTool> m_larHVCellCorrTool;
  bool m_rescaleForHV;

  unsigned m_cacheUpdateCounter;

  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutex_t m_mutex;
};

#endif
