/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PattMergeRootAlgo_h
#define PattMergeRootAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigFTKSim/FTKLogging.h"

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSSMap.h"

#include <vector>
#include <string>
/////////////////////////////////////////////////////////////////////////////

class FTKSSMap;

class PattMergeRootAlgo: public AthAlgorithm , public FTKLogger {
 public:
  PattMergeRootAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PattMergeRootAlgo();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode RunMerging();

protected:
   virtual void PostMessage(void); // FTKLogger write out to Athena


protected:
   std::string m_TextImportRootFile;
   std::string m_RootOutFile;
   std::string m_TextOutFile;
   bool m_DeleteAfterMerging;   
   int m_NSub;
   int m_MinCov;
   int m_Compression;
   std::string m_WhereToRunMerging;
   std::vector< std::string > m_InputFiles;

   FTKSSMap *m_ssmap;
   int m_hwmodeid;

   std::string m_ssmap_path,m_pmap_path,m_rmap_path,m_modulelut_path;
   int m_curreg;
 
};

#endif // PattMergeRootAlgo_h
