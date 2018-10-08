/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_CALIBRATIONHITMERGER_H
#define LARG4SD_CALIBRATIONHITMERGER_H 1

// STL includes
#include <set>
#include <string>
#include <vector>

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

namespace LArG4 {

  /** @class CalibrationHitMerger

      @brief Athena Algorithm that merges a given set of CaloCalibrationHitContainers into a single CaloCalibrationHitContainers - based on CollectionMerger by Elmar Ritsch.

     */
  class CalibrationHitMerger final : public AthAlgorithm {

  public:
    /** Constructor */
    CalibrationHitMerger( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~CalibrationHitMerger();

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  execute()    override final;
    virtual StatusCode  finalize()   override final;

  private:
    class LessHit {
    public:
      bool operator() ( CaloCalibrationHit* const& p, CaloCalibrationHit* const& q ) const
      {
        return p->Less(q);
      }
    };
    typedef std::set< CaloCalibrationHit*, LessHit >  m_calibrationHits_t;

    /** Initialize the given VarHandleKey */
    StatusCode initializeVarHandleKey( SG::VarHandleKey& varHandleKey ) const;

    /** Commonly used member and function parameter types */
    typedef std::vector<std::string>  SGKeyVector_t;
    typedef std::vector<SG::ReadHandleKey<CaloCalibrationHitContainer>> ReadHandleKeyVector_t;


    /** Setup a vector of ReadHandleKeys for the given vector of string StoreGate Keys */
    StatusCode setupReadHandleKeyVector( const SGKeyVector_t&      sgKeyVec,
                                         ReadHandleKeyVector_t& readHandleVec ) const;

    /** Input collection StoreGate keys */
    SGKeyVector_t m_inputHitsSGKeys;

    /** Input collection ReadHandleKeys */
    ReadHandleKeyVector_t m_inputHits;

    /** Output collection WriteHandleKeys */
    SG::WriteHandleKey<CaloCalibrationHitContainer> m_outputHits;
  };

}

#endif //> !LARG4SD_CALIBRATIONHITMERGER_H
