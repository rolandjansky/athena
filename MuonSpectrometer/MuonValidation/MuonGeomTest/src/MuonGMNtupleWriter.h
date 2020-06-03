/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGM_MUONGMNTUPLEWRITER_H
#define MUONGM_MUONGMNTUPLEWRITER_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkValidationUtils/SurfaceNtupleBranch.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class TTree;

namespace MuonGM {
  
  /** @class MuonGMNtupleWriter
       
      BAsic MuonGM Ntuple dumper
        
      @author Niels van Eldik
  */
      
  class MuonGMNtupleWriter : public AthAlgorithm  {

  public:

    /** Standard Athena-Algorithm Constructor */
    MuonGMNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /** Default Destructor */
    virtual ~MuonGMNtupleWriter();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();

    /** standard Athena-Algorithm method */
    StatusCode          execute();
       
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
       
  protected:
    void fillNtuple();

    std::string m_ntupleTreeName;       /** jobOption: Ntuple tree name*/
    std::string m_ntupleFileName;       /** jobOption: Ntuple file name*/    
    std::string m_ntupleDirName;        /** jobOption: Ntuple directory name*/
    TTree* m_tree;
    int m_nevents;

    Trk::SurfaceNtupleBranch m_mdtSurfaceBranch;
    
    // MuonDetectorManager from the conditions store
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
	"MuonDetectorManager", 
	"Key of input MuonDetectorManager condition data"};    

    bool m_outputToTextFile;
  };
   
   
}

#endif 

