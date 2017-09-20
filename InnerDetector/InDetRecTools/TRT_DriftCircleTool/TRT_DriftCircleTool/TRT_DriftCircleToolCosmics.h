/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  TRT_DriftCircleToolCosmics
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for TRT RDI collection production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/02/2003 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#ifndef TRT_DriftCircleToolCosmics_H
#define TRT_DriftCircleToolCosmics_H

#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "CommissionEvent/ComTime.h"
#include "StoreGate/ReadHandleKey.h"

class ITRT_StrawSummarySvc;
class IInDetConditionsSvc;
class ITRT_DriftFunctionTool;
class TRT_ID;
class TRT_RDORawData;
class TRT_DriftCircleCollection;
class TRT_DriftCircle;
namespace InDetDD {
 class TRT_DetectorManager;
}


namespace InDet {
/** @class TRT_DriftCircleToolCosmics
Class for converting a RDO collection to a DriftCircle collection
Performs trigger phase corrections for in-cavern cosmics
*/
class TRT_DriftCircleToolCosmics: public AthAlgTool, virtual public ITRT_DriftCircleTool
 
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

public:
  /** constructor */
  TRT_DriftCircleToolCosmics(const std::string&,const std::string&,const IInterface*);
  /** constructor */
  virtual ~TRT_DriftCircleToolCosmics ();
  /** initialize needed services */
  virtual StatusCode initialize();
  /** finalize */
  virtual StatusCode finalize  ();
  /** make the conversion from RDOs to DriftCircles */
  virtual InDet::TRT_DriftCircleCollection*  
  convert(int,const InDetRawDataCollection<TRT_RDORawData>*, const bool  m_CTBBadChannels );  
  /** test validity gate for corrected drift times */
  virtual bool passValidityGate(unsigned int word, float lowGate, float highGate, float t0) const;


  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
  
 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
  SG::ReadHandleKey<ComTime> m_comTimeName {this,"ComTimeName","ComTime","RHK to retrieve ComTime"}; //!< RHK for ComTime

  ToolHandle< ITRT_DriftFunctionTool > m_driftFunctionTool;  //!< DriftFunctionTool
//  ServiceHandle< IInDetConditionsSvc> m_ConditionsSummary; //!< The ConditionsSummaryTool
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_ConditionsSummary; //!< The ConditionsSummaryTool

  bool                                 m_useConditionsStatus;     //!< SHall the ConditionsSummaryTool be used?
  bool                                 m_useConditionsHTStatus;     //!< Shall the ConditionsSummaryTool be used for HT to find argon straws? 
  std::string                          m_trt_mgr_location ;  //!< Manager name
  //std::string                          m_comTimeName      ;  //!< Phase handler name
  const InDetDD::TRT_DetectorManager * m_trt_mgr          ;  //!< Manager handle
  const TRT_ID                       * m_trtid            ;  //!< TRT id helper handle
  unsigned int                         m_coll_pll         ;  //!< phase offset (in clock bins)
  double                               m_global_offset    ;  //!< Global offset to TRT Phase to make TRT Calibration happy ...
  bool				       m_useToTCorrection;  //!< Shall the Time over Threshold correction be used?
  bool                                 m_useHTCorrection;  //!< Shall the High Threshold correction be used?

  bool                                 m_reject_if_first_bit; //!< If true, reject this DC if first bit high
  bool                                 m_reject_if_first_bit_argon; //!< If true, reject this DC if first bit high
  float                                m_min_trailing_edge; //!< Min raw trailing edge position to not reject this DC
  float                                m_min_trailing_edge_argon; //!< Min raw trailing edge position to not reject this DC
  float                                m_max_drift_time; //!< Max allowed raw drift time to not reject this DC
  float                                m_max_drift_time_argon; //!< Max allowed raw drift time to not reject this DC
  bool                                 m_out_of_time_supression; //!< Turn this on to turn on the OOT options!
  bool                                 m_out_of_time_supression_argon; //!< Turn this on to turn on the OOT options!
  bool                                 m_validity_gate_suppression; //! Turn on suppression using validity gates
  bool                                 m_validity_gate_suppression_argon; //! Turn on suppression using validity gates
  float                                m_low_gate; //! Low value for gate 
  float                                m_low_gate_argon; //! Low value for gate 
  float                                m_high_gate; //! High value for gate
  float                                m_high_gate_argon; //! High value for gate
  bool                                 m_mask_first_HT_bit; // mask off ht bit in first (earliest) 25ns time bin
  bool                                 m_mask_first_HT_bit_argon; // mask off ht bit in first (earliest) 25ns time bin
  bool                                 m_mask_middle_HT_bit; // mask off ht bit in middle 25 ns time bin
  bool                                 m_mask_middle_HT_bit_argon; // mask off ht bit in middle 25 ns time bin
  bool                                 m_mask_last_HT_bit; // mask off ht bit in last (latest) 25 ns time bin
  bool                                 m_mask_last_HT_bit_argon; // mask off ht bit in last (latest) 25 ns time bin
};

} // end of namespace

#endif // TRT_DriftCircleToolCosmics_H
