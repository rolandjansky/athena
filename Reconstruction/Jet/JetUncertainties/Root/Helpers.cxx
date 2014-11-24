/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/Helpers.h"

#include "PathResolver/PathResolver.h"

#include "TSystem.h"

#include <math.h>

namespace jet
{
namespace utils
{
template <>
bool GetTypeObjFromString<std::string>(const std::string& str, std::string& obj)
{
    obj = str;
    return true;
}

template <>
bool GetTypeObjFromString<TString>(const std::string& str, TString& obj)
{
    obj = str.c_str();
    return true;
}

template <>
bool GetTypeObjFromString<bool>(const std::string& str, bool& obj)
{
    bool toReturn = false;
    if (str == "true" || str == "True" || str == "TRUE")
    {
        obj = true;
        toReturn = true;
    }
    else if (str == "false" || str == "False" || str == "FALSE")
    {
        obj = false;
        toReturn = true;
    }
    else
    {
        int value;
        toReturn = GetTypeObjFromString<int>(str,value);
        if (toReturn)
            obj = static_cast<bool>(value);
    }
    return toReturn;
}

template <>
bool GetTypeObjFromString<std::string>(const TString& str, std::string& obj)
{
    obj = str.Data();
    return true;
}

template <>
bool GetTypeObjFromString<TString>(const TString& str, TString& obj)
{
    obj = str;
    return true;
}

template <>
bool GetTypeObjFromString<bool>(const TString& str, bool& obj)
{
    bool toReturn = false;
    if (str.EqualTo("true",TString::kIgnoreCase))
    {
        obj = true;
        toReturn = true;
    }
    else if (str.EqualTo("false",TString::kIgnoreCase))
    {
        obj = false;
        toReturn = true;
    }
    else
    {
        int value;
        toReturn = GetTypeObjFromString<int>(str,value);
        if (toReturn)
            obj = static_cast<bool>(value);
    }
    return toReturn;
}

bool FileExists(const TString& fileName)
{
    return gSystem->AccessPathName(fileName) == false;
}

TString FindFilePath(const TString& fileName, const TString& path)
{
    TString pathToGet = "";

    // First, try the raw filename plus path (user-specified), then raw filename (local)
    if (FileExists(fileName))
        pathToGet = fileName;
    else if (FileExists(path+(path.EndsWith("/")?"":"/")+fileName))
        pathToGet = path+(path.EndsWith("/")?"":"/")+fileName;

    // Next, try PathResolver
    if (pathToGet == "")
        pathToGet = TString(PathResolverFindCalibFile(Form("JetUncertainties/%s",fileName.Data())).c_str());

    
    // Try backup locations now
    if (pathToGet == "")
    {
        // Try ROOTCOREBIN
        if (TString(gSystem->Getenv("ROOTCOREBIN")) != "" && FileExists(TString(gSystem->Getenv("ROOTCOREBIN"))+"/data/JetUncertainties/"+fileName))
            pathToGet = TString(gSystem->Getenv("ROOTCOREBIN"))+"/data/JetUncertainties/"+fileName;
        // Try ROOTCOREDIR
        else if (TString(gSystem->Getenv("ROOTCOREDIR")) != "" && FileExists(TString(gSystem->Getenv("ROOTCOREDIR"))+"/data/JetUncertainties/"+fileName))
            pathToGet = TString(gSystem->Getenv("ROOTCOREDIR"))+"/data/JetUncertainties/"+fileName;
        // Try standard athena location
        else if (TString(gSystem->Getenv("TestArea")) != "" && FileExists(TString(gSystem->Getenv("TestArea"))+"/Reconstruction/Jet/JetUncertainties/share/"+fileName))
            pathToGet = TString(gSystem->Getenv("TestArea"))+"/Reconstruction/Jet/JetUncertainties/share/"+fileName;
    }

    // Give up, hopefully sucessfully
    // User must check if return is "" (failed) or an actual path (success)
    return pathToGet;
}

TFile* ReadRootFile(const TString& fileName, const TString& path)
{
    TFile* rootFile = NULL;
    
    TString pathToGet = FindFilePath(fileName,path);

    if (pathToGet != "")
        rootFile = new TFile(pathToGet,"READ");
    return rootFile;
}


std::vector<double> GetLogBins(const size_t numBins, const double minVal, const double maxVal)
{
    std::vector<double> bins;
    bins.resize(numBins+1);
    
    const double dx = (log(maxVal)-log(minVal))/numBins;
    for (size_t iBin = 0; iBin <= numBins; ++iBin)
        bins[iBin] = exp(log(minVal)+iBin*dx);

    return bins;
}

std::vector<double> GetUniformBins(const size_t numBins, const double minVal, const double maxVal)
{
    std::vector<double> bins;
    bins.resize(numBins+1);

    const double dx = (maxVal-minVal)/numBins;
    for (size_t iBin = 0; iBin <= numBins; ++iBin)
        bins[iBin] = minVal+iBin*dx;

    return bins;
}


} // End utils namespace
} // End jet namespace

