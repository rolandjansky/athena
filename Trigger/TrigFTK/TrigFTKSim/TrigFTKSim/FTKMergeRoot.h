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

class FTKSSMap;

class FTKMergeRoot : FTKLogging {
public:
   FTKMergeRoot(std::string outfilename);
   ~FTKMergeRoot();
   void DoTextImport();
   void DoTextExport(std::string const &TextOutFilename, int MinCoverage=1);
   int DoMerge(int MinCoverage=1,int compression=1,
               FTKSSMap *checkMap=0,int checkTower=-1,int checkHWmodeid=-1);
   void AddFile(std::string const &filename);
   void AddFiles(std::vector<std::string> const &filenames);

   //std::string SetOutputFile() { return m_OutFile;}
   void SetTempRootFileName(std::string const &filename);
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
   std::string m_OutFile_basename;
   TString m_OutFile_rootname;
   TString m_TempRootFile;
   std::vector<std::string > m_InputFiles;
   int m_NSubregions;
   bool m_DeleteAfterMerging;
private:


};


#endif
