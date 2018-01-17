/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArCalibCopyAlg.h
 * @author Walter Lampl <walter.lampl @cern.ch>
 * @date Feb 2008
 * @brief Algorithm to copy LAr Electronic Calibration constants to a new container with a different grouping
 */


#ifndef LARCALIBCOPYALG_H
#define LARCALIBCOPYALG_H
 
#include <vector>
#include <string>
 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArTdriftComplete.h"
#include "LArRawConditions/LArPedestalComplete.h"


/** 
  * @class LArCalibValidationAlg
  * @brief Algorithm to copy LAr elec. calibration constants to a different conatiner (different grouping)
  *
  */


template<class CONDITIONSCONTAINER>
class LArCalibCopyAlg:public AthAlgorithm {
 
public:
  /**
   * @brief regular Algorithm constructor
   */
  LArCalibCopyAlg (const std::string& name, ISvcLocator* pSvcLocator);

  /**
   * @brief Destructor
   */
  ~LArCalibCopyAlg();

  /**
   * @brief Initialize method.
   * @return Gaudi status code.
   * Analyzes and verifies the jobOption settings
   */
  StatusCode initialize();

  /**
   * @brief Empty Execute method
   */
  StatusCode execute() {return StatusCode::SUCCESS;}

   /**
   * @brief stop(() method.
   * @return Gaudi status code.
   * All the job is done here
   */
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:

  typedef typename CONDITIONSCONTAINER::ConstConditionsMapIterator CONTIT;
  typedef typename CONDITIONSCONTAINER::LArCondObj LArCondObj;
  typedef typename CONDITIONSCONTAINER::ConstCorrectionIt CORRIT;
  
  
  std::string m_inputName;
  std::string m_outputName;
  std::string  m_groupingType;
  bool m_useCorrChannel;

};

#include "LArCalibCopyAlg.icc"

#endif

