/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_HELPERS_H
#define JETUNCERTAINTIES_HELPERS_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TFile.h"

#include "AsgTools/MsgStreamMacros.h"

#define JESUNC_ERROR_CODE -1234
#define JESUNC_NO_DEFAULT_CONSTRUCTOR ATH_MSG_FATAL("Default constructor is not supported");
#define JESUNC_SAFE_DELETE(T) { if(T) { delete T; T = NULL; } }

namespace jet
{

namespace utils
{
    // Get variables from strings
    template<typename T>
    bool GetTypeObjFromString(const std::string& str, T& obj);
    template<typename T>
    T GetTypeObjFromString(const std::string& str);
    template<typename T>
    bool GetTypeObjFromString(const TString& str, T& obj);
    template<typename T>
    T GetTypeObjFromString(const TString& str);

    // Specializations of getting variables from strings
    template <>
    bool GetTypeObjFromString<std::string>(const std::string& str, std::string& obj);
    template <>
    bool GetTypeObjFromString<TString>(const std::string& str, TString& obj);
    template <>
    bool GetTypeObjFromString<bool>(const std::string& str, bool& obj);
    template <>
    bool GetTypeObjFromString<std::string>(const TString& str, std::string& obj);
    template <>
    bool GetTypeObjFromString<TString>(const TString& str, TString& obj);
    template <>
    bool GetTypeObjFromString<bool>(const TString& str, bool& obj);
    
    // Convert strings to vectors of objects
    template <typename T>
    bool Vectorize(const TString& str, const TString& sep, std::vector<T>& result);
    template <typename T>
    std::vector<T> Vectorize(const TString& str, const TString& sep);

    // Check if a file exists
    bool FileExists(const TString& fileName);

    // Find a valid file path
    TString FindFilePath(const TString& fileName, const TString& path = "");
    
    // Open a root file
    TFile* ReadRootFile(const TString& fileName, const TString& path = "");

    // Make bins easily
    std::vector<double> GetLogBins(const size_t numBins, const double minVal, const double maxVal);
    std::vector<double> GetUniformBins(const size_t numBins, const double minVal, const double maxVal);
}
    
template <typename T>
bool utils::GetTypeObjFromString(const std::string& str, T& obj)
{
    std::istringstream iss(str);
    return !(iss >> obj).fail();
}
template <typename T>
T utils::GetTypeObjFromString(const std::string& str)
{
    T toReturn;
    if (!GetTypeObjFromString(str,toReturn))
        printf("Failed to convert object: %s\n",str.c_str());

    return toReturn;
}
template <typename T>
bool utils::GetTypeObjFromString(const TString& str, T& obj)
{
    std::string stdstr = str.Data();
    return GetTypeObjFromString(stdstr,obj);
}
template <typename T>
T utils::GetTypeObjFromString(const TString& str)
{
    T toReturn;
    if (!GetTypeObjFromString(str,toReturn))
        printf("ERROR: Failed to convert object: %s\n",str.Data());

    return toReturn;
}

template <typename T>
bool utils::Vectorize(const TString& str, const TString& sep, std::vector<T>& result)
{
    bool success = true;
    result.clear();

    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while(TObjString* os=(TObjString*)istr())
    {
        T obj;
        if (!GetTypeObjFromString(os->GetString(),obj))
        {
            success = false;
            break;
        }
        else
            result.push_back(obj);
    }
    delete tokens;

    return success;
}

template <typename T>
std::vector<T> utils::Vectorize(const TString& str, const TString& sep)
{
    std::vector<T> result;
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);

    while(TObjString* os=(TObjString*)istr())
    {
        T obj;
        if (!GetTypeObjFromString(os->GetString(),obj))
            printf("ERROR: String \"%s\" is not the requested type\n",os->GetString().Data());
        result.push_back(obj);
    }
    delete tokens;
    
    return result;
}

} // end jet namespace

#endif


