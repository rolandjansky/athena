
 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1CPCMXTOOLS_H
#define LVL1L1CPCMXTOOLS_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DataModel/DataVector.h"
#include "TrigT1CaloToolInterfaces/IL1CPCMXTools.h"

namespace TrigConf
{
class ITrigConfigSvc;
}

namespace LVL1 
{

class CMXCPHits;
class CPAlgorithm;
class CMXCPTob;
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

class L1CPCMXTools : virtual public IL1CPCMXTools, public AthAlgTool
{
  public:
     
    L1CPCMXTools(const std::string& type, const std::string& name,
                                          const IInterface* parent);

     /** default destructor */
    virtual ~L1CPCMXTools ();
      
     /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
     /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
     /** CPAlgorithm to CPMTobRoI conversion */
    virtual void formCPMTobRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                  DataVector<CPMTobRoI>*      cpmRoiVec) const;
     /** EmTauROI to CPMTobRoI conversion */
    virtual void formCPMTobRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                  DataVector<CPMTobRoI>*   cpmRoiVec) const;
     /** form CMX-CP TOBs from RoIs - single slice */
    virtual void formCMXCPTob(const DataVector<CPMTobRoI>*   cpmRoiVec,
                                    DataVector<CMXCPTob>* cmxTobVec) const;
     /** form CMX-CP TOBs from RoIs - multiple slices */
    virtual void formCMXCPTob(
                 const std::vector<const DataVector<CPMTobRoI>*>& cpmRoiColls,
                 DataVector<CMXCPTob>* cmxTobVec, int peak) const;
     /** form complete CMX-CP hits from CMX-CP TOBs */
    virtual void formCMXCPHits(const DataVector<CMXCPTob>*  cmxTobVec,
                                     DataVector<CMXCPHits>* cmxHitsVec) const;
     /** form partial CMX-CP hits (crate) from CMX-CP TOBs */
    virtual void formCMXCPHitsCrate(const DataVector<CMXCPTob>* cmxTobVec,
                                    DataVector<CMXCPHits>* cmxHitsCrate) const;
     /** form partial CMX-CP hits (system) from crate CMX-CP hits */
    virtual void formCMXCPHitsSystem(const DataVector<CMXCPHits>* cmxHitsCrate,
                                     DataVector<CMXCPHits>* cmxHitsSys) const;
     /** form partial CMX-CP hits (topo) from CMX-CP TOBs */
    virtual void formCMXCPHitsTopo(const DataVector<CMXCPTob>* cmxTobVec,
                                   DataVector<CMXCPHits>* cmxHitsTopo) const;
      
  private:
      
    typedef std::vector<unsigned int>   HitsVector;
    typedef std::vector<int>            ErrorVector;

    /** Temporary for testing until CPAlgorithm and EmTauROI are updated */
    std::pair<uint32_t,uint32_t> roiWord(const EmTauROI* roi) const;
    int isolationEm(unsigned int clusterEnergy, unsigned int emIsol,
                    unsigned int hadIsol, unsigned int hadVeto) const;
    int isolationTau(unsigned int clusterEnergy, unsigned int emIsol,
                     unsigned int hadIsol) const;
    void unpackEmIsol(int energy, int isol, unsigned int& emIsol,
                      unsigned int& hadIsol, unsigned int& hadVeto) const;
    void unpackTauIsol(int energy, int isol, unsigned int& emIsol,
                       unsigned int& hadIsol) const;
    void getHits(const CMXCPTob* tob,
		 HitsVector& hit0, HitsVector& hit1) const;
    void addOverflow(ErrorVector& hitErr, const ErrorVector& tobErr) const;

    /** Add hits from second vector to first */
    void addCMXCPHits(HitsVector& vec1, const HitsVector& vec2) const;
    /** Increment CPM/CMX hit word */
    unsigned int addHits(unsigned int hitMult, unsigned int hitVec,
                                  int multBits, int vecBits) const;
    /** Merge CMX-CP hits vectors */
    void mergeCMXCPHits(DataVector<CMXCPHits>* cmxHitsVec1,
                        DataVector<CMXCPHits>* cmxHitsVec2) const;
    /** Save non-zero CMX-CP hits */
    void saveCMXCPHits(DataVector<CMXCPHits>* cmxHitsVec,
                       const HitsVector& hits0, const HitsVector& hits1,
                       const ErrorVector& err0, const ErrorVector& err1,
		       int crate, int cmx, int source, int peak) const;

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
