/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file LArRampAdHocPatchingAlg.h
 * @author Marco Delmastro <Marco.Delmastro@cern.ch>
 * @date Mar 2010
 * @brief Algorithm implementing ad-hoc Ramp patches
 */

#ifndef LARRAMPADHOCPATCHINGALG_H
#define LARRAMPADHOCPATCHINGALG_H
 
#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/ToolHandle.h"
#include "LArRawConditions/LArRampComplete.h"

#include <vector>
#include <string>

class LArRampAdHocPatchingAlg : public Algorithm 
{ 
 public:
  /**
   * @brief regular Algorithm constructor
   */
  LArRampAdHocPatchingAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /**
   * @brief Destructor
   */
  ~LArRampAdHocPatchingAlg();

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
   * @brief Finalize method.
   * @return Gaudi status code.
   * All the job is done here
   */
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:

  typedef LArRampComplete::LArCondObj LArRampObj;

  StoreGateSvc* m_detStore;    
  MsgStream* m_log;

  const LArRampComplete* m_contIn; 
  LArRampComplete* m_contOut;  

  std::string    m_containerKey;
  std::string    m_newContainerKey;

  /*
  std::vector<HWIdentifier> m_channelsToBePatchedHG;
  std::vector<HWIdentifier> m_channelsToBePatchedMG;
  std::vector<HWIdentifier> m_channelsToBePatchedLG;
  */

  std::vector<unsigned> m_channelsToBePatchedHG;
  std::vector<unsigned> m_channelsToBePatchedMG;
  std::vector<unsigned> m_channelsToBePatchedLG;

  std::vector<unsigned> m_patchesToBeAppliedHG;
  std::vector<unsigned> m_patchesToBeAppliedMG;
  std::vector<unsigned> m_patchesToBeAppliedLG;

  enum patchMethod{
    ZeroIntercept
  };

  
  bool ZeroTheIntercept(HWIdentifier chid, unsigned gain);
  StatusCode ApplyAdHocPatches( std::vector<unsigned>& m_channelsToBePatched,  std::vector<unsigned>& m_patchesTobeApplied, unsigned gain );

  bool m_useCorrChannel;

};

#endif

