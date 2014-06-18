/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKMergeRoot_h
#define FTKMergeRoot_h

/* package FTKMergeRoot
 *
 * Class for merging of patterns stored in root format
 * For instruction, see FTKMergeRoot.cxx
 *
 */

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <TDirectory.h>
#include "TrigFTKSim/FTKLogging.h"

class FTKMergeRoot : FTKLogging {
public:
   FTKMergeRoot(std::string outfilename);
   ~FTKMergeRoot();
   void DoTextImport();
   void DoTextExport(TString TextOutFilename, int MinCoverage=1);
   int DoMerge(int MinCoverage=1);
   void AddFile(std::string filename);
   void AddFiles(std::vector<std::string> filenames);

   void SetOutputFile(std::string filename);
   std::string SetOutputFile() { return m_OutFile;}
   void SetTempRootFileName(std::string filename);
   std::string GetTempRootFileName() const { return m_TempRootFile.Data();}
   int GetNInput() const { return m_InputFiles.size();}
   std::string GetInput(unsigned int i) const { return m_InputFiles[i];}
   void SetNSubregions(int nsubregions) { m_NSubregions = nsubregions;}
   int GetNSubregions() const { return m_NSubregions;}
   void SetDeleteFilesAfterMerging(bool DoRemove=true) { m_DeleteAfterMerging = DoRemove;}
   bool GetDeleteFilesAfterMerging() { return m_DeleteAfterMerging;}
   
protected:
   

private:


protected:
   std::string m_OutFile;
   TString m_TempRootFile;
   std::vector<std::string > m_InputFiles;
   int m_NSubregions;
   bool m_DeleteAfterMerging;
private:


};


#endif
