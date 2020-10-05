/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPHitsTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1CPHITSTOOLS_H
#define LVL1L1CPHITSTOOLS_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthContainers/DataVector.h"
#include "TrigT1CaloToolInterfaces/IL1CPHitsTools.h"

namespace LVL1 
{

class CMMCPHits;
class CPAlgorithm;
class CPMHits;
class CPMRoI;
class EmTauROI;

  /** @class L1CPHitsTools

      This is a tool to reconstruct the L1 CPM and CMM-CP hits
      from RoIs.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1CPHitsTools->formCPMRoI()    // convert to CPMRoI from other
                                             // RoI classes
              L1CPHitsTools->formCPMHits()   // form CPM hits from RoIs
              L1CPHitsTools->formCMMCPHits() // form CMM-CP hits from CPM hits
      
      @author  Alan Watson / Peter Faulkner
  */  

class L1CPHitsTools : virtual public IL1CPHitsTools, public AthAlgTool
{
  public:
     
    L1CPHitsTools(const std::string& type, const std::string& name,
                                           const IInterface* parent);

     /** default destructor */
    virtual ~L1CPHitsTools ();
      
     /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
     /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
      
     /** CPAlgorithm to CPMRoI conversion */
    virtual void formCPMRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                  DataVector<CPMRoI>*      cpmRoiVec) const;
     /** EmTauROI to CPMRoI conversion */
    virtual void formCPMRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                  DataVector<CPMRoI>*   cpmRoiVec) const;
     /** form CPM hits from RoIs - single slice */
    virtual void formCPMHits(const DataVector<CPMRoI>*  cpmRoiVec,
                                   DataVector<CPMHits>* cpmHitsVec) const;
     /** form CPM hits from RoIs - multiple slices */
    virtual void formCPMHits(
                 const std::vector<const DataVector<CPMRoI>*>& cpmRoiColls,
                 DataVector<CPMHits>* cpmHitsVec, int peak) const;
     /** form complete CMM-CP hits from CPM hits */
    virtual void formCMMCPHits(const DataVector<CPMHits>*   cpmHitsVec,
                                     DataVector<CMMCPHits>* cmmHitsVec) const;
     /** form partial CMM-CP hits (module) from CPM hits */
    virtual void formCMMCPHitsModule(const DataVector<CPMHits>* cpmHitsVec,
                                     DataVector<CMMCPHits>* cmmHitsMod) const;
     /** form partial CMM-CP hits (crate) from module CMM-CP hits */
    virtual void formCMMCPHitsCrate(const DataVector<CMMCPHits>* cmmHitsMod,
                                    DataVector<CMMCPHits>* cmmHitsCrate) const;
     /** form partial CMM-CP hits (system) from crate CMM-CP hits */
    virtual void formCMMCPHitsSystem(const DataVector<CMMCPHits>* cmmHitsCrate,
                                     DataVector<CMMCPHits>* cmmHitsSys) const;
      
  private:
      
    typedef std::vector<unsigned int>   HitsVector;
    typedef std::vector<int>            ErrorVector;

    /** Add hits from second vector to first */
    void addCMMCPHits(HitsVector& vec1, const HitsVector& vec2) const;
    /** Increment CPM/CMM hit word */
    unsigned int addHits(unsigned int hitMult, unsigned int hitVec,
                                  int multBits, int vecBits) const;
    /** Merge CMM-CP hits vectors */
    void mergeCMMCPHits(DataVector<CMMCPHits>* cmmHitsVec1,
                        DataVector<CMMCPHits>* cmmHitsVec2) const;
    /** Save non-zero CMM-CP hits */
    void saveCMMCPHits(DataVector<CMMCPHits>* cmmHitsVec,
                       const HitsVector& hits0, const HitsVector& hits1,
		       int crate, int dataId, int peak) const;

    bool m_debug;
     
};

} // end of namespace

#endif 
