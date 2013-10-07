/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TRT_DriftCircleValidationNtupleWriter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDET_TRT_DRIFTCIRCLEVALIDATIONNTUPLEWRITER_H
#define INDET_TRT_DRIFTCIRCLEVALIDATIONNTUPLEWRITER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

#include "InDetPrepRawData/TRT_DriftCircleContainer.h" // typedef

//class AtlasDetectorID;
class TTree;

namespace InDet {

/** @class TRT_DriftCircleValidationNtupleWriter
 
   Validation Algorithm to retrieve a TRT_DriftCircleContainer, loop over the entries and fill
   the validation ntuple with RIO specific entries.
   
   @author Sebastian.Fleischmann@cern.ch
*/

class TRT_DriftCircleValidationNtupleWriter : public AthAlgorithm {
public:
    /** Standard Athena-Algorithm Constructor */
    TRT_DriftCircleValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~TRT_DriftCircleValidationNtupleWriter();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:

    const InDet::TRT_DriftCircleContainer*  m_riocontainer; //!< container of RIOs
    std::string mjo_riocontainername; //!< jobOption: name of container with RIOs
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file name
    std::string m_ntupleDirName;        //!< jobOption: Ntuple directory name
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name

    // pointer to the NTuple tree
    TTree* m_nt;

    int                 m_eventNumber; //!< event number
    int                 m_nRIOs;       //!< number of driftcircles in current event. This variable set the size of the vectors with driftcircle information.

    std::vector<float>* m_rioLoc1;     //!< measured drift radius UNIT:mm
    std::vector<float>* m_rioSurfaceX; //!< global X coordinate of straw centre position
    std::vector<float>* m_rioSurfaceY; //!< global Y coordinate of straw centre position
    std::vector<float>* m_rioSurfaceZ; //!< global Z coordinate of straw centre position


};

} // close of namespace

#endif // INDET_TRT_DRIFTCIRCLEVALIDATIONNTUPLEWRITER_H

