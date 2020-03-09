/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEPHitsTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JEPHITSTOOLS_H
#define LVL1L1JEPHITSTOOLS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/DataVector.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloToolInterfaces/IL1JEPHitsTools.h"


namespace LVL1 
{

class CMMJetHits;
class JEMHits;
class JEMRoI;
class JetAlgorithm;
class JetROI;

  /** @class L1JEPHitsTools

      This is a tool to reconstruct the L1 JEM and CMM-Jet hits
      from RoIs.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1JEPHitsTools->formJEMRoI()     // convert to JEMRoI from
                                               // other RoI classes
              L1JEPHitsTools->formJEMHits()    // form JEM hits from RoIs
              L1JEPHitsTools->formCMMJetHits() // form CMM-Jet hits from
	                                       // JEM hits
      
      @author  Alan Watson / Peter Faulkner
  */  

class L1JEPHitsTools : virtual public IL1JEPHitsTools, public AthAlgTool
{
  public:
     
    L1JEPHitsTools(const std::string& type, const std::string& name,
                                            const IInterface* parent);

     /** default destructor */
    virtual ~L1JEPHitsTools ();
      
     /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
     /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
     /** JetAlgorithm to JEMRoI conversion */
    virtual void formJEMRoI(const DataVector<JetAlgorithm>* jetAlgorithmVec,
                                  DataVector<JEMRoI>*       jemRoiVec) const;
     /** JetROI to JEMRoI conversion */
    virtual void formJEMRoI(const DataVector<JetROI>*   jetRoiVec,
                                  DataVector<JEMRoI>*   jemRoiVec) const;
     /** form JEM hits from RoIs - single slice */
    virtual void formJEMHits(const DataVector<JEMRoI>*  jemRoiVec,
                                   DataVector<JEMHits>* jemHitsVec) const;
     /** form JEM hits from RoIs - multiple slices */
    virtual void formJEMHits(
                 const std::vector<const DataVector<JEMRoI>*>& jemRoiColls,
                 DataVector<JEMHits>* jemHitsVec, int peak) const;
     /** form complete CMM-Jet hits from JEM hits */
    virtual void formCMMJetHits(const DataVector<JEMHits>*    jemHitsVec,
                                      DataVector<CMMJetHits>* cmmHitsVec) const;
     /** form partial CMM-Jet hits (module) from JEM hits */
    virtual void formCMMJetHitsModule(const DataVector<JEMHits>* jemHitsVec,
                                      DataVector<CMMJetHits>* cmmHitsMod) const;
     /** form partial CMM-Jet hits (crate) from module CMM-Jet hits */
    virtual void formCMMJetHitsCrate(const DataVector<CMMJetHits>* cmmHitsMod,
                                    DataVector<CMMJetHits>* cmmHitsCrate) const;
     /** form partial CMM-Jet hits (system) from crate CMM-Jet hits */
    virtual void formCMMJetHitsSystem(
                                     const DataVector<CMMJetHits>* cmmHitsCrate,
                                     DataVector<CMMJetHits>* cmmHitsSys) const;
     /** form partial CMM-Jet hits (et map) from system CMM-Jet hits */
    virtual void formCMMJetHitsEtMap(const DataVector<CMMJetHits>* cmmHitsSys,
                                     DataVector<CMMJetHits>* cmmHitsMap) const;
      
  private:

    enum    HitsType { MAIN_HITS, FORWARD_HITS };
      
    typedef std::vector<unsigned int>   HitsVector;
    typedef std::vector<int>            ErrorVector;

    /** Add hits from second vector to first */
    void addCMMJetHits(HitsVector& vec1, const HitsVector& vec2,
                                               HitsType type) const;
    /** Increment JEM/CMM hit word */
    unsigned int addHits(unsigned int hitMult, unsigned int hitVec,
                         int multBits, int vecBits, int nthresh) const;
    /** Extract Forward hit counts from Forward+Main and split left/right */
    HitsVector forward(int jem, const HitsVector& vec) const;
    /** Extract Main hit counts from Forward+Main and expand to 3 bits */
    HitsVector main(const HitsVector& vec) const;
    /** Merge CMM-Jet hits vectors */
    void mergeCMMJetHits(DataVector<CMMJetHits>* cmmHitsVec1,
                         DataVector<CMMJetHits>* cmmHitsVec2) const;
    /** Save non-zero CMM-Jet hits */
    void saveCMMJetHits(DataVector<CMMJetHits>* cmmHitsVec,
                        const HitsVector& hits,
		        int crate, int dataId, int peak) const;

    /** trigger configuration service */
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

    bool m_debug;
     
};

} // end of namespace

#endif 
