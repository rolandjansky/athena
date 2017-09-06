/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page UserAnalysis_page The User Analysis Package

 - This class is an analysis skeleton - The user can implement his analysis here

 - This class is also used for the demonstration of the distributed analysis
 
 - Some electron histograms are used for the distributed case. The user may
remove the histograms and the electron stuff if not needed.

 - Note: the single algorithm structure as an analysis code does not scale. For detailed 
analysis examples, look in CVS: PhysicsAnalysis/AnalysisCommon/AnalysisExamples/

 - pathena: a glue script to submit user-defined jobs to distributed analysis systems

 - FakeAppMgr.py: a fake application manager to collect job configuration without DLL loading

 - ConfigExtractor.py: This jobO extracts job configuration

 - buildJob: a generic transformation to compile user-defined sources on a remote site

 - runAthena: a generic transformation to run athena on a remote site

 - getPFN: this is used to get a physical file name (PFN) for a logical file name (LFN)

 - Client.py,FileSpec.py,JobSpec.py: they are used by pathena. Users don't have to modify them
 
 - For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov

*/
