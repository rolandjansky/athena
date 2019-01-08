/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LVL1L1JETCMXTOOLS_H
#define LVL1L1JETCMXTOOLS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
//#include "TrigConfigSvc/ITrigConfigSvc.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigT1CaloToolInterfaces/IL1JetCMXTools.h"

namespace LVL1 {

/** @class L1JetCMXTools

    This is a tool to reconstruct the L1 JEM and CMX-Jet hits
    from RoIs.
    Used for offline monitoring and trigger reconstruction.

    Useage: L1JetCMXTools->formJEMTobRoI()  // convert to JEMTobRoI from
                                            // other RoI classes
            L1JetCMXTools->formCMXJetTob()  // form CMX-Jet TOBs from
                                            // JEMTobRoIs
            L1JetCMXTools->formCMXJetHits() // form CMX-Jet hits from
                                            // CMXJetTobs

    @author  Peter Faulkner
*/

class L1JetCMXTools : virtual public IL1JetCMXTools, public AthAlgTool {
 public:
  L1JetCMXTools(const std::string& type, const std::string& name,
                const IInterface* parent);

  /** default destructor */
  virtual ~L1JetCMXTools();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize();
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize();

  /** form CMX-Jet TOBs from RoIs - single slice */
  virtual void formCMXJetTob(const xAOD::JEMTobRoIContainer* jemRoiVec,
                             xAOD::CMXJetTobContainer* cmxTobVec) const;

  /** form CMX-Jet TOBs from RoIs - multiple slices */
  virtual void formCMXJetTob(
      const std::vector<const xAOD::JEMTobRoIContainer*>& jemRoiColls,
      xAOD::CMXJetTobContainer* cmxTobVec, int peak) const;

  /** form complete CMX-Jet hits from CMX-Jet TOBs */
  virtual void formCMXJetHits(const xAOD::CMXJetTobContainer* cmxTobVec,
                              xAOD::CMXJetHitsContainer* cmxHitsVec) const;

  /** form partial CMX-Jet hits (crate) from CMX-Jet TOBs */
  virtual void formCMXJetHitsCrate(
      const xAOD::CMXJetTobContainer* cmxTobVec,
      xAOD::CMXJetHitsContainer* cmxHitsCrate) const;

  /** form partial CMX-Jet hits (system) from crate CMX-Jet hits */
  virtual void formCMXJetHitsSystem(
      const xAOD::CMXJetHitsContainer* cmxHitsCrate,
      xAOD::CMXJetHitsContainer* cmxHitsSys) const;
  /** form partial CMX-Jet hits (topo) from system CMX-Jet TOBs */
  virtual void formCMXJetHitsTopo(const xAOD::CMXJetTobContainer* cmxTobVec,
                                  xAOD::CMXJetHitsContainer* cmxHitsTopo) const;

 private:
  enum HitsType { MAIN_HITS, FORWARD_HITS };

  typedef std::vector<uint32_t> HitsVector;
  typedef std::vector<uint32_t> ErrorVector;

  /** Get hit map */
  void getHits(const xAOD::CMXJetTob* tob, HitsVector& hit10, HitsVector& hit11,
               HitsVector& hit20, HitsVector& hit21) const;

  /** Add overflow bit */
  void addOverflow(ErrorVector& hitErr, const ErrorVector& tobErr) const;
  /** Add hits from second vector to first */
  void addCMXJetHits(HitsVector& vec1, const HitsVector& vec2,
                     HitsType type) const;
  /** Increment JEM/CMX hit word */
  unsigned int addHits(unsigned int hitMult, unsigned int hitVec, int multBits,
                       int vecBits, int nthresh) const;

  /** Merge CMX-Jet hits vectors */
  void mergeCMXJetHits(xAOD::CMXJetHitsContainer* cmxHitsVec1,
                       xAOD::CMXJetHitsContainer* cmxHitsVec2) const;

  void saveCMXJetHits(xAOD::CMXJetHitsContainer* cmxHitsVec,
                      const HitsVector& hits0, const HitsVector& hits1,
                      const ErrorVector& err0, const ErrorVector& err1,
                      int crate, int source, int peak) const;

  /** Trigger configuration service */
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
  /** Number of crates */
  int m_crates;
  /** Number of JEM modules per crate */
  int m_modules;
  /** Maximum number of TOBs per module to CMXs */
  int m_maxTobs;
  /** System crate number */
  int m_sysCrate;
  /** Debug flag */
  bool m_debug;
};

}  // end of namespace

#endif
