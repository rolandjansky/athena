/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
ccuenca, 24Nov2009
updateHists.C
This macro recursively copies the contents of a given directory (inStem)
from the input root file to a given directory in an output file.

If the stem is a histogram, then it only copies that histogram
to outStem, if provided.

If the output histogram exists, the contents are updated. The macro
will ask before overwriting histograms.

It is intended for the histogram files used as references for
offline (tier0) monitoring, where only a subset of the histograms
from a processed root file are needed,
but have to be updated or expanded regularly
*/

#ifndef __CINT__
#include "TFile.h"
#include "TKey.h"
#endif // __CINT__

#include <iostream>
#include <ostream>
#include <iomanip>

TFile *source = 0;
TFile *target = 0;
int padding = 0;

// function prototypes

int updateHists(std::string inFileName, std::string inStem, std::string outFileName = "", std::string outStem = "");
bool makeDirectories(std::string dirName);
bool makeDir(std::string dirName);
void Copy(std::string inDir, std::string outDir, std::string inHist = "", std::string outHist = "");
void CopyHist(std::string inDir, std::string outDir, std::string inHist, std::string outHist);

//----------------------------------------
// main macro
//----------------------------------------
int updateHists(std::string inFileName, std::string inStem, std::string outFileName, std::string outStem)
{
    //open original file
    source = TFile::Open(inFileName.c_str());
    if (!source) {
        std::cout << "Couldn't open input file, " << inFileName << std::endl;
        return 1;
    } else std::cout << "opening input file, " << inFileName << std::endl;

    //find out whether inStem is a histogram or a directory
    bool isDir = true;
    std::string path = inStem;
    std::string hist;

    if (inStem[inStem.size() - 1] != '/') {
        std::string::size_type lastSlash = inStem.find_last_of('/');
        hist = inStem.substr(lastSlash + 1, inStem.size() - lastSlash - 1);
        path =  inStem.substr(0, lastSlash + 1);
        isDir = (source->FindObjectAny(hist.c_str()))->InheritsFrom("TDirectory");
        if (isDir) {
            path = inStem;
            hist.clear();
        }
    }

    if (path[path.size() - 1] == '/')
        path.resize(path.size() - 1);
    if (!source->GetDirectory(path.c_str())) {
        std::cout << "path " << path << " directory doesn't exist in input file" << std::endl;
        return 1;
    }

    //open target file
    std::cout.fill(' ');
    if (outFileName.empty()) {
        outFileName = std::string(inFileName, 0, inFileName.find(".root"));
        outFileName += "_trimmed.root";
    }
    target = new TFile(outFileName.c_str(), "update");
    if (!target) {
        std::cout << "coundn't open output file " << outFileName << std::endl;
        return 1;
    } else std::cout << "opening output file " << outFileName << std::endl;

    if (outStem.empty()) outStem = inStem;
    std::string targetPath = outStem;
    std::string targetHist;
    if (!isDir) {
        std::string::size_type lastSlash = outStem.find_last_of('/');
        targetPath = outStem.substr(0, lastSlash + 1);
        targetHist = outStem.substr(lastSlash + 1, outStem.size() - lastSlash - 1);
    }

    if (targetPath[targetPath.size() - 1]  == '/')
        targetPath.resize(targetPath.size() - 1);

    target->cd();
    if (!makeDirectories(targetPath)) {
        std::cout << "couldn't create outStem directories in output" << std::endl;
        return 1;
    }

    //copy relevant objects
    Copy(path, targetPath, hist, targetHist);

    delete target;
    delete source;
    return 0;
}

bool makeDirectories(std::string dirName)
{
    padding += 3;
    std::cout << std::setw(padding) << " ";
    std::cout << "makeDirectories, dirName=" << dirName << std::endl;
    bool success = true;

    if (!dirName.empty()) {

        std::string::size_type firstSlash = dirName.find('/');
        if (firstSlash == std::string::npos) {
            success = makeDir(dirName);
        } else {
            std::string subdir(dirName, 0, firstSlash);
            if (!subdir.empty())
                success = makeDir(subdir);

            std::string newSubdir(dirName, firstSlash + 1, dirName.size() - firstSlash);
            success = makeDirectories(newSubdir);
        }
    }
    padding -= 3;
    return success;
}

bool makeDir(std::string dirName)
{
    padding += 3;
    std::cout << std::setw(padding) << " ";

    if (!gDirectory->FindKey(dirName.c_str())) {
        gDirectory->mkdir(dirName.c_str());
        std::cout << "makeDir=" << dirName << std::endl;
    } else
        std::cout << "object " << dirName << " already exists in directory " << gDirectory->GetName() << std::endl;

    padding -= 3;
    return gDirectory->cd(dirName.c_str());
}

void Copy(std::string inDir, std::string outDir, std::string inHist, std::string outHist)
{
    padding += 3;

    if (!inHist.empty()) {
        CopyHist(inDir, outDir, inHist, outHist);

    } else {
        TDirectory *sourceDir = source->GetDirectory(inDir.c_str());
        TDirectory *targetDir = target->GetDirectory(outDir.c_str());

        TIter nextKey(sourceDir->GetListOfKeys());

        TKey *key;
        while ((key = (TKey*)nextKey())) {
            std::string keyName  = key->GetName();
            std::string className(key->GetClassName());

            if (className == "TDirectoryFile") {
                std::string newInDir = inDir;
                newInDir += '/';
                newInDir += keyName;

                std::string newOutDir = outDir;
                newOutDir += '/';
                newOutDir += keyName;

                if (! targetDir->FindKey(keyName.c_str())) {
                    std::cout << std::setw(padding) << " ";
                    std::cout << "creating Dir " << newOutDir << std::endl;
                    targetDir->mkdir(keyName.c_str());
                }

                std::cout << std::setw(padding) << " ";
                std::cout << "moving to " << newInDir << ", a " << className << std::endl;
                Copy(newInDir, newOutDir);

            } else {
                CopyHist(inDir, outDir, keyName, keyName);
            }
        }
    }

    padding -= 3;
}

void CopyHist(std::string inDir, std::string outDir, std::string inHist, std::string outHist)
{
    TDirectory *sourceDir = source->GetDirectory(inDir.c_str());
    TDirectory *targetDir = target->GetDirectory(outDir.c_str());

    targetDir->cd();
    TKey *key = sourceDir->FindKey(inHist.c_str());
    TObject *object = key->ReadObj();
    bool permission = true;

    if (targetDir->FindKey(outHist.c_str())) {
        std::cout << "WARNING: object "
                  << outHist << " exists in" << std::endl << "         "
                  << targetDir->GetFile()->GetName() << ":" << outDir << std::endl
                  << "         Do you want to overwrite it? [y]/n ";
        std::string userIn;
        std::getline(std::cin, userIn);
        if (userIn.empty()) userIn = "y";
        if (userIn != "y") permission = false;
    }

    if (permission) {
        object->Write(outHist.c_str(), TObject::kOverwrite);
        std::cout << std::setw(padding) << " ";
        std::cout << "copying " << outHist << ", a " << key->GetClassName() << std::endl;
    }
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " inFileName inStem outFileName outStem" << std::endl;
        return 1;
    } else {
        return updateHists(argv[1], argv[2], argv[3], argv[4]);
    }
}
#endif // __CINT__
