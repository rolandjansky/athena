/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetTagger.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#ifndef TRIGBJETHYPO_TRIGBJETTAGGER
#define TRIGBJETHYPO_TRIGBJETTAGGER

#include <map>
#include <string>
#include <vector>
#include <cmath>

#ifndef VALIDATION_TOOL
#include "TrigBjetHypo/TrigBjetFex.h"
//#else
//#include "Root/TrigBjetFexStandalone.h"
#endif

#ifndef VALIDATION_TOOL
class MsgStream;
class TrigBjetFex;
#else
class TrigBjetFexStandalone;
#include "Root/MsgStream.h"
#endif

class TaggerHelper;
class TuningLikelihood;

class TrigBjetTrackInfo;
class TrigBjetPrmVtxInfo;
class TrigBjetSecVtxInfo;
class TrigBjetJetInfo;



/**
 * @brief Base class for likelihood taggers.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base class for the computation of the different likelihood taggers. 
 */


class TrigBjetTagger {

 public:
  
  /** @brief Constructor. */
#ifndef VALIDATION_TOOL
  TrigBjetTagger(TrigBjetFex*, MsgStream&, unsigned int);
#else
  TrigBjetTagger(TrigBjetFexStandalone*, MsgStream&, unsigned int, bool);
#endif

  /** @brief Destructor. */
  ~TrigBjetTagger();

  /** @brief To retrieve particular element of private member m_taggersXMap. */ 
  float taggersXMap(std::string tagger) {return m_taggersXMap[tagger];};

  /** @brief To retrieve IP1D tagger distribution (for monitoring). */
  float getXIP1D() const {return m_taggersXMap.find("IP1D")->second;};
  /** @brief To retrieve IP2D tagger distribution (for monitoring). */
  float getXIP2D() const {return m_taggersXMap.find("IP2D")->second;};
  /** @brief To retrieve IP3D tagger distribution (for monitoring). */
  float getXIP3D() const {return m_taggersXMap.find("IP3D")->second;};
  /** @brief To retrieve CHI2 tagger distribution (for monitoring). */
  float getXCHI2() const {return m_taggersXMap.find("CHI2")->second;};
  /** @brief To retrieve SVTX tagger distribution (for monitoring). */
  float getXSVTX() const {return m_taggersXMap.find("SVTX")->second;};
  /** @brief To retrieve COMB tagger distribution (for monitoring). */
  float getXCOMB() const {return m_taggersXMap.find("COMB")->second;};
  /** @brief To retrieve EVTX tagger distribution (for monitoring). */
  float getXEVTX() const {return m_taggersXMap.find("EVTX")->second;};
  /** @brief To retrieve NVTX tagger distribution (for monitoring). */
  float getXNVTX() const {return m_taggersXMap.find("NVTX")->second;};
  /** @brief To retrieve MVTX tagger distribution (for monitoring). */
  float getXMVTX() const {return m_taggersXMap.find("MVTX")->second;};

  /** @brief To fill maps of likelihood taggers using calibrations stored in python files. */
  void fillLikelihoodMap(const std::string tagger, TuningLikelihood* pointerToTuningLikelihood) 
    {m_likelihoodMap[tagger] = pointerToTuningLikelihood;};

  void getWeights();
  void getWeights(std::vector<TrigBjetTrackInfo>*&, TrigBjetPrmVtxInfo*&, TrigBjetSecVtxInfo*&, TrigBjetJetInfo*&);
  void getBestWeight();

#ifdef VALIDATION_TOOL
  std::vector<float> m_vectorIP1D;
  std::vector<float> m_vectorIP2D;
  std::vector<float> m_vectorIP1D_lowSiHits;
  std::vector<float> m_vectorIP2D_lowSiHits;
#endif

 private:

#ifdef VALIDATION_TOOL
public:
#endif

  /** @brief Pointer to TrigBjetFex class. */
#ifndef VALIDATION_TOOL
  TrigBjetFex* m_trigBjetFex;
#else
  TrigBjetFexStandalone* m_trigBjetFex;
#endif

  /** @brief To parametrize errors on track longitudinal impact parameter at LVL2. */
  float getIP1DErr(float, float);
  /** @brief To parametrize errors on track transverse impact parameter at LVL2. */
  float getIP2DErr(float, float);

  /** @brief To get the likelihood ratio of a given track. */
  float getW(const std::string, float);
  /** @brief To get the likelihood ratio of a given track for the 2D combination of significances likelihood taggers. */
  float getW(const std::string, float, float);
  /** @brief To get the likelihood ratio of the three-dimensioanl tagger based on all secondary vertex taggers. */
  float getW(const std::string, float, float, float);

  /** @brief To scale the final likelihood weight using X=-std::log10(W). */
  float getX(double);
  /** @brief To scale the final likelihood weight using X=W/(W+1). */
  float getOldX(double); 
  /** @brief To scale the final combined likelihood weight to be more uniform. */  
  float getXComb(double);

  float f_ip(float, int);

  /** @brief To use properly message service. */
  MsgStream m_log;
  /** @brief To use properly message service. */
  unsigned int m_logLvl;  

  /** @brief Pointer to TaggerHelper class. */ 
  TaggerHelper* m_taggerHelper;

  /** @brief Association between the name of the tagger and its W variable. */
  std::map<std::string, float> m_taggersWMap;
  /** @brief Association between the name of the tagger and its X variable (used to perform the selection in TrigBjetHypo). */
  std::map<std::string, float> m_taggersXMap;
  /** @brief Association between the likelihood tagger name and the relative TuningLikelihood object. */
  std::map<std::string, TuningLikelihood*> m_likelihoodMap;

};
#endif
