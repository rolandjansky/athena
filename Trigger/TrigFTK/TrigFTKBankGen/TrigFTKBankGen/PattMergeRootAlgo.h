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
#include <vector>
#include <string>
/////////////////////////////////////////////////////////////////////////////


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
 
};

#endif // PattMergeRootAlgo_h
