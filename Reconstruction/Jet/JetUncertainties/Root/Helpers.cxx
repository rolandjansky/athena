/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/Helpers.h"

#include "PathResolver/PathResolver.h"

#include "TSystem.h"
#include "TH1.h"

#include <math.h>

namespace jet
{
namespace utils
{
template <>
bool getTypeObjFromString<std::string>(const std::string& str, std::string& obj)
{
    obj = str;
    return true;
}

template <>
bool getTypeObjFromString<TString>(const std::string& str, TString& obj)
{
    obj = str.c_str();
    return true;
}

template <>
bool getTypeObjFromString<bool>(const std::string& str, bool& obj)
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
        toReturn = getTypeObjFromString<int>(str,value);
        if (toReturn)
            obj = static_cast<bool>(value);
    }
    return toReturn;
}

template <>
bool getTypeObjFromString<std::string>(const TString& str, std::string& obj)
{
    obj = str.Data();
    return true;
}

template <>
bool getTypeObjFromString<TString>(const TString& str, TString& obj)
{
    obj = str;
    return true;
}

template <>
bool getTypeObjFromString<bool>(const TString& str, bool& obj)
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
        toReturn = getTypeObjFromString<int>(str,value);
        if (toReturn)
            obj = static_cast<bool>(value);
    }
    return toReturn;
}

bool fileExists(const TString& fileName)
{
    return gSystem->AccessPathName(fileName) == false;
}

TString findFilePath(const TString& fileName, const TString& path)
{
    TString pathToGet = "";

    // First, try the raw filename plus path (user-specified), then raw filename (local)
    if (fileExists(fileName))
        pathToGet = fileName;
    else if (fileExists(path+(path.EndsWith("/")?"":"/")+fileName))
        pathToGet = path+(path.EndsWith("/")?"":"/")+fileName;
    
    // Next, try PathResolver with the default path (most users should be in this case)
    if (pathToGet == "")
        pathToGet = TString(PathResolverFindCalibFile(Form("JetUncertainties/%s",fileName.Data())).c_str());

    // Third, try PathResolver, but without specifying the path. This is to allow the tool to read files residing in other packages
    if (pathToGet == "")
        pathToGet = TString(PathResolverFindCalibFile(fileName.Data()).c_str());    
    
    // Try backup locations now
    if (pathToGet == "")
    {
        // Try ROOTCOREBIN
        if (TString(gSystem->Getenv("ROOTCOREBIN")) != "" && fileExists(TString(gSystem->Getenv("ROOTCOREBIN"))+"/data/JetUncertainties/"+fileName))
            pathToGet = TString(gSystem->Getenv("ROOTCOREBIN"))+"/data/JetUncertainties/"+fileName;
        // Try ROOTCOREDIR
        else if (TString(gSystem->Getenv("ROOTCOREDIR")) != "" && fileExists(TString(gSystem->Getenv("ROOTCOREDIR"))+"/data/JetUncertainties/"+fileName))
            pathToGet = TString(gSystem->Getenv("ROOTCOREDIR"))+"/data/JetUncertainties/"+fileName;
        // Try standard athena location
        else if (TString(gSystem->Getenv("TestArea")) != "" && fileExists(TString(gSystem->Getenv("TestArea"))+"/Reconstruction/Jet/JetUncertainties/share/"+fileName))
            pathToGet = TString(gSystem->Getenv("TestArea"))+"/Reconstruction/Jet/JetUncertainties/share/"+fileName;
    }

    // Give up, hopefully sucessfully
    // User must check if return is "" (failed) or an actual path (success)
    return pathToGet;
}

TFile* readRootFile(const TString& fileName, const TString& path)
{
    TFile* rootFile = NULL;
    
    TString pathToGet = findFilePath(fileName,path);
    std::cout << "Looking for file " << fileName << " in path " << path << std::endl;

    if (pathToGet != "")
        rootFile = new TFile(pathToGet,"READ");
    return rootFile;
}


std::vector<double> getLogBins(const size_t numBins, const double minVal, const double maxVal)
{
    std::vector<double> bins;
    bins.resize(numBins+1);
    
    const double dx = (log(maxVal)-log(minVal))/numBins;
    for (size_t iBin = 0; iBin <= numBins; ++iBin)
        bins[iBin] = exp(log(minVal)+iBin*dx);

    return bins;
}

std::vector<double> getUniformBins(const size_t numBins, const double minVal, const double maxVal)
{
    std::vector<double> bins;
    bins.resize(numBins+1);

    const double dx = (maxVal-minVal)/numBins;
    for (size_t iBin = 0; iBin <= numBins; ++iBin)
        bins[iBin] = minVal+iBin*dx;

    return bins;
}


void scaleHistoAxes(TH1* toScale, const double factorX, const double factorY, const double factorZ)
{
    const int dim = toScale->GetDimension();

    // Check for simple copies and sanity checks
    bool returnDuplicate = false;
    if      ( dim == 1 && (factorX < 0 || fabs(factorX-1) < 1.e-4) )
        returnDuplicate = true;
    else if ( dim == 2 && (factorX < 0 || fabs(factorX-1) < 1.e-4)
                       && (factorY < 0 || fabs(factorY-1) < 1.e-4) )
        returnDuplicate = true;
    else if ( dim == 3 && (factorX < 0 || fabs(factorX-1) < 1.e-4)
                       && (factorY < 0 || fabs(factorY-1) < 1.e-4)
                       && (factorZ < 0 || fabs(factorZ-1) < 1.e-4) )
        returnDuplicate = true;
    if (returnDuplicate)
    {
        printf("ScaleHistoAxes: Doing nothing, as all scale factors require no changes\n");
        return;
    }
    
    // Let negative numbers mean no change
    const double facX = factorX < 0 ? 1 : factorX;
    const double facY = factorY < 0 ? 1 : factorY;
    const double facZ = factorZ < 0 ? 1 : factorZ;

    // Scale the x axis if necessary
    if (fabs(facX-1) > 1.e-4)
    {
        std::vector<double> binsX;
        for (int iBin = 1; iBin <= toScale->GetNbinsX()+1; ++iBin)
            binsX.push_back(toScale->GetXaxis()->GetBinLowEdge(iBin)*facX);
        toScale->GetXaxis()->Set(binsX.size()-1,&binsX[0]);
    }
    // Scale the y axis if necessary
    if (dim > 1 && fabs(facY-1) > 1.e-4)
    {
        std::vector<double> binsY;
        for (int iBin = 1; iBin <= toScale->GetNbinsY()+1; ++iBin)
            binsY.push_back(toScale->GetYaxis()->GetBinLowEdge(iBin)*facY);
        toScale->GetYaxis()->Set(binsY.size()-1,&binsY[0]);
    }
    // Scale the z axis if necessary
    if (dim > 2 && fabs(facZ-1) > 1.e-4)
    {
        std::vector<double> binsZ;
        for (int iBin = 1; iBin <= toScale->GetNbinsZ()+1; ++iBin)
            binsZ.push_back(toScale->GetZaxis()->GetBinLowEdge(iBin)*facZ);
        toScale->GetZaxis()->Set(binsZ.size()-1,&binsZ[0]);
    }

}


} // End utils namespace
} // End jet namespace

