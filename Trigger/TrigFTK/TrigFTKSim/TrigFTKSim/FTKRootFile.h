/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef H_FTKRootFile
#define H_FTKRootFile

/* package FTKRootFile
 *
 * Common interface to open TFile objects by name
 * This offer the possibility to implement different protocols
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 * classes:
 *     FTKRootFile
 *         provides static methods to open root files for reading or
 *         to create new root files for writing
 *     FTKRootFileChain
 *        sequence of several root files which are treated as a single
 *        source
 *
 */

#include <iostream>
#include <map>
#include <vector>
#include <TDirectory.h>
#include "TrigFTKSim/FTKLogging.h"

class FTKRootFile {
public:
   static FTKRootFile *Instance(void);
   virtual TDirectory *OpenRootFileReadonly(char const *path);
   virtual TDirectory *CreateRootFile(char const *path,int compression=1);
   virtual bool IsRootFile(char const *path);
protected:
   FTKRootFile(void);
private:
   static FTKRootFile *sTheFileAccess;
};

class FTKRootFileChain : public FTKLogging {
public:
   explicit inline FTKRootFileChain(int maxOpen=50) :  FTKLogging("FTKRootFileChain"), fMaxOpenFiles(maxOpen) {
   }
   ~FTKRootFileChain();
   void AddFile(char const *fileName);
   inline int GetLength(void) const { return fFileNames.size(); }
   inline std::string const & operator[](int i) { return fFileNames[i]; }
   // returns true if the directory was changed
   bool Open(std::string const &fileName);
   // return the directory selected in the last call to Open
   TDirectory *GetDirectory(void) const;
   //virtual TDirectory *OpenRootFileReadonly(char const *fileName);
   TDirectory *OpenRootFileReadonly(char const *fileName);
protected:
   int fMaxOpenFiles;
   std::vector<std::string> fFileNames;
   std::map<std::string,TDirectory *> fOpenDirectories;
   std::string fLastFileName;
};

#endif
