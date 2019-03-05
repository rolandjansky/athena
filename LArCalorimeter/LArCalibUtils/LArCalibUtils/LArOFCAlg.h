//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArOFCAlgorithm_H
#define LArOFCAlgorithm_H
 
#include <vector>
#include <string>
 
#include "LArRawConditions/LArWaveCumul.h"

#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArAutoCorrDecoderTool.h"

//#include "LArElecCalib/ILArPedestal.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArPhysWaveContainer.h"

#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "LArRawConditions/LArShapeComplete.h"

#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include <Eigen/Dense>

#include "tbb/blocked_range.h"

class LArOnlineID; 
class CaloDetDescrManager; 
class LArDSPConfig;

#include "GaudiKernel/ToolHandle.h"

class LArOFCAlg:public AthAlgorithm {
 
public:
 
  LArOFCAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  //  Eigen::VectorXd  getAmpCoef();
  //Eigen::VectorXd  getATauCoef();

private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  struct perChannelData_t {
    //Input:
    const LArWaveCumul* inputWave;
    HWIdentifier chid;
    unsigned gain;

    //Output:
    std::vector<std::vector<float> > ofc_a;
    std::vector<std::vector<float> > ofc_b;

    std::vector<std::vector<float> > ofcV2_a;
    std::vector<std::vector<float> > ofcV2_b;

    std::vector<std::vector<float> >shape;
    std::vector<std::vector<float> >shapeDer;
    
    float tstart;
    float timeBinWidthOFC;
    unsigned phasewMaxAt3;
    bool faultyOFC;
    bool shortWave;


    perChannelData_t(const LArWaveCumul* wave, const HWIdentifier hi, const unsigned g) : 
      inputWave(wave), chid(hi), gain(g),tstart(0), timeBinWidthOFC(25./24), phasewMaxAt3(0), faultyOFC(false), shortWave(false) {};

  };


  std::vector<perChannelData_t> m_allChannelData;

  void           optFilt(const std::vector<float> &gWave_in, const std::vector<float>  &gDerivWave_in, const Eigen::MatrixXd& autoCorrInv, //input variables
			 std::vector<float>& OFCa, std::vector<float>& OFCb // Output variables;
			 ) const; 

  void           optFiltDelta(const std::vector<float> &gWave_in, const std::vector<float>  &gDerivWave_in, const Eigen::MatrixXd& autoCorrInv, 
			      const Eigen::VectorXd& delta, //input variables
			      std::vector<float>& vecOFCa, std::vector<float>& vecOFCb // Output variables;
			      ) const; 

  void process(perChannelData_t&, const LArOnOffIdMapping* cabling) const;


  bool verify(const HWIdentifier chid, const std::vector<float>& OFCa, const std::vector<float>& OFCb, 
	      const std::vector<float>& Shape, const char* ofcversion, const unsigned phase) const;

  void printOFCVec(const std::vector<float>& vec, MsgStream& mLog) const;

  
  StatusCode     initPhysWaveContainer(const LArOnOffIdMapping* cabling);
  StatusCode     initCaliWaveContainer();

    
  std::string              m_dumpOFCfile ;
  std::vector<std::string> m_keylist;
  bool                     m_verify; 
  bool                     m_normalize;
  bool                     m_timeShift;
  int                      m_timeShiftByIndex ;


  unsigned int             m_nSamples;
  unsigned int             m_nPhases;
  unsigned int             m_dPhases; // number of samples between two neighboring phases (OFC sets)
  unsigned int             m_nDelays ;
  unsigned int             m_nPoints;
  float                    m_addOffset;

  ToolHandle<ILArAutoCorrDecoderTool> m_AutoCorrDecoder;
  ToolHandle<ILArAutoCorrDecoderTool> m_AutoCorrDecoderV2;

  const LArOnlineID*       m_onlineID; 
  const CaloDetDescrManager* m_calo_dd_man;
  const LArOFCBinComplete* m_larPhysWaveBin;

  double m_errAmpl;
  double m_errTime;

  bool                     m_readCaliWave ;
  bool                     m_fillShape ;
  std::string              m_ofcKey; 
  std::string              m_ofcKeyV2; 
  std::string              m_shapeKey; 
  bool                     m_storeMaxPhase;
  std::string              m_ofcBinKey;

  // Grouping type
  std::string              m_groupingType;
  std::string              m_larPhysWaveBinKey;

  int                      m_useDelta;
  int                      m_useDeltaV2;
  bool                     m_computeV2;
  bool                     m_runThreaded;

  bool                     m_readDSPConfig;
  std::string              m_DSPConfigFolder;
  LArDSPConfig*            m_DSPConfig;

  bool                     m_forceShift;

  Eigen::VectorXd getDelta(std::vector<float>& samples, const HWIdentifier chid, unsigned nSamples) const;
 


  bool useDelta(const HWIdentifier chid, const int jobOFlag, const LArOnOffIdMapping* cabling) const;

  static const float m_fcal3Delta[5];
  static const float m_fcal2Delta[5];
  static const float m_fcal1Delta[5];


  //Functor for processing with TBB
  class Looper {
  public:
    Looper(std::vector<perChannelData_t>* p, const LArOnOffIdMapping* cabling, const LArOFCAlg* a) : m_perChanData(p), m_cabling(cabling), m_ofcAlg(a) {};
    void operator() (tbb::blocked_range<size_t>& r) const {
      //std::cout << "TBB grainsize " << r.end() - r.begin() << std::endl;
      for (size_t i=r.begin();i!=r.end();++i) {
	m_ofcAlg->process(m_perChanData->at(i),m_cabling);
      }
    }
  private:
    std::vector<perChannelData_t>* m_perChanData;
    const LArOnOffIdMapping* m_cabling;
    const LArOFCAlg* m_ofcAlg;
  };

};


#endif

