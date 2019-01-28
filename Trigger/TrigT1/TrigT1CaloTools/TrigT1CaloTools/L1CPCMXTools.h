/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LVL1L1CPCMXTOOLS_H
#define LVL1L1CPCMXTOOLS_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/DataVector.h"
#include "TrigT1CaloToolInterfaces/IL1CPCMXTools.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"

namespace TrigConf {
class ITrigConfigSvc;
}

namespace LVL1 {
class CPAlgorithm;
class CPMTobRoI;
class EmTauROI;

/** @class L1CPCMXTools

    This is a tool to reconstruct the L1 CMX-CP TOBs and hits
    from RoIs.
    Used for offline monitoring and trigger reconstruction.

    Useage: L1CPCMXTools->formCPMTobRoI() // convert to CPMTobRoI from other
                                           // RoI classes
            L1CPCMXTools->formCMXCPTob()  // form CMX-CP TOBs from RoIs
            L1CPCMXTools->formCMXCPHits() // form CMX-CP hits from TOBs

    @author  Peter Faulkner
*/

class L1CPCMXTools : virtual public IL1CPCMXTools, public AthAlgTool {
public:
  L1CPCMXTools(const std::string &type, const std::string &name,
               const IInterface *parent);

  /** default destructor */
  virtual ~L1CPCMXTools();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize();
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize();

  /** CPAlgorithm to CPMTobRoI conversion */ // function deprecated
  virtual void formCPMTobRoI(const DataVector<CPAlgorithm> *cpAlgorithmVec,
                             DataVector<CPMTobRoI> *cpmRoiVec) const;
  /** EmTauROI to CPMTobRoI conversion */ // function deprecated
  virtual void formCPMTobRoI(const DataVector<EmTauROI> *emTauRoiVec,
                             DataVector<CPMTobRoI> *cpmRoiVec) const;
  /** form CMX-CP TOBs from RoIs - single slice */
  virtual void formCMXCPTob(const xAOD::CPMTobRoIContainer *cpmRoiVec,
                            xAOD::CMXCPTobContainer *cmxTobVec) const;
  /** form CMX-CP TOBs from RoIs - multiple slices */
  virtual void
  formCMXCPTob(const std::vector<const xAOD::CPMTobRoIContainer *> &cpmRoiColls,
               xAOD::CMXCPTobContainer *cmxTobVec, uint8_t peak) const;
  /** form complete CMX-CP hits from CMX-CP TOBs */
  virtual void formCMXCPHits(const xAOD::CMXCPTobContainer *cmxTobVec,
                             xAOD::CMXCPHitsContainer *cmxHitsVec) const;
  /** form partial CMX-CP hits (crate) from CMX-CP TOBs */
  virtual void formCMXCPHitsCrate(const xAOD::CMXCPTobContainer *cmxTobVec,
                                  xAOD::CMXCPHitsContainer *cmxHitsCrate) const;
  /** form partial CMX-CP hits (system) from crate CMX-CP hits */
  virtual void formCMXCPHitsSystem(const xAOD::CMXCPHitsContainer *cmxHitsCrate,
                                   xAOD::CMXCPHitsContainer *cmxHitsSys) const;
  /** form partial CMX-CP hits (topo) from CMX-CP TOBs */
  virtual void formCMXCPHitsTopo(const xAOD::CMXCPTobContainer *cmxTobVec,
                                 xAOD::CMXCPHitsContainer *cmxHitsTopo) const;

private:
  typedef std::vector<uint32_t> HitsVector;
  typedef std::vector<uint32_t> ErrorVector;

  /** Temporary for testing until CPAlgorithm and EmTauROI are updated */
  std::pair<uint32_t, uint32_t> roiWord(const EmTauROI *roi) const;
  int isolationEm(unsigned int clusterEnergy, unsigned int emIsol,
                  unsigned int hadIsol, unsigned int hadVeto) const;
  int isolationTau(unsigned int clusterEnergy, unsigned int emIsol,
                   unsigned int hadIsol) const;
  void unpackEmIsol(int energy, int isol, unsigned int &emIsol,
                    unsigned int &hadIsol, unsigned int &hadVeto) const;
  void unpackTauIsol(int energy, int isol, unsigned int &emIsol,
                     unsigned int &hadIsol) const;
  // void getHits(const xAOD::CMXCPTob* tob,
  //		 HitsVector& hit0, HitsVector& hit1) const;
  void getHits(const xAOD::CMXCPTob *tob, HitsVector &hits0,
               HitsVector &hits1) const;

  void addOverflow(ErrorVector &hitErr, const ErrorVector &tobErr) const;

  /** Add hits from second vector to first */
  void addCMXCPHits(HitsVector &vec1, const HitsVector &vec2) const;
  /** Increment CPM/CMX hit word */
  unsigned int addHits(unsigned int hitMult, unsigned int hitVec, int multBits,
                       int vecBits) const;
  /** Merge CMX-CP hits vectors */
  void mergeCMXCPHits(xAOD::CMXCPHitsContainer *cmxHitsVec1,
                      xAOD::CMXCPHitsContainer *cmxHitsVec2) const;
  /** Save non-zero CMX-CP hits */
  void saveCMXCPHits(xAOD::CMXCPHitsContainer *cmxHitsVec,
                     const HitsVector &hits0, const HitsVector &hits1,
                     const ErrorVector &err0, const ErrorVector &err1,
                     uint8_t crate, uint8_t cmx, uint8_t source,
                     uint8_t peak) const;

  /** Trigger configuration service */
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

  /** Number of CP crates */
  int m_crates;
  /** Number of CPMs per crate */
  int m_modules;
  /** Maximum number of TOBs per CPM */
  int m_maxTobs;
  /** System crate */
  int m_sysCrate;
  /** Debug printout flag */
  bool m_debug;
};

} // end of namespace

#endif
