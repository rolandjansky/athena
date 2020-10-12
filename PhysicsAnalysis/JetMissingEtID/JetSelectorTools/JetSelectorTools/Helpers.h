/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSELECTORTOOLS_HELPERS_H
#define JETSELECTORTOOLS_HELPERS_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TFile.h"

#include "AsgMessaging/MsgStreamMacros.h"


namespace JCT
{

namespace utils
{
    // Check variable types from strings
    template<typename T>
    bool isTypeObjFromString(const std::string& str);
    template<typename T>
    bool isTypeObjFromString(const TString& str);

    // Get variables from strings
    template<typename T>
    bool getTypeObjFromString(const std::string& str, T& obj);
    template<typename T>
    T getTypeObjFromString(const std::string& str);
    template<typename T>
    bool getTypeObjFromString(const TString& str, T& obj);
    template<typename T>
    T getTypeObjFromString(const TString& str);

    // Specializations of getting variables from strings
    template <>
    bool getTypeObjFromString<std::string>(const std::string& str, std::string& obj);
    template <>
    bool getTypeObjFromString<TString>(const std::string& str, TString& obj);
    template <>
    bool getTypeObjFromString<bool>(const std::string& str, bool& obj);
    template <>
    bool getTypeObjFromString<std::string>(const TString& str, std::string& obj);
    template <>
    bool getTypeObjFromString<TString>(const TString& str, TString& obj);
    template <>
    bool getTypeObjFromString<bool>(const TString& str, bool& obj);
    
    // Convert strings to vectors of objects
    template <typename T>
    bool vectorize(const TString& str, const TString& sep, std::vector<T>& result);
    template <typename T>
    std::vector<T> vectorize(const TString& str, const TString& sep);

    // Check if a file exists
    bool fileExists(const TString& fileName);
    bool fileExists(const std::string& fileName);
}

template <typename T>
bool utils::isTypeObjFromString(const std::string& str)
{
    std::istringstream iss(str);
    T obj;
    return !(iss >> obj).fail();
}

template <typename T>
bool utils::isTypeObjFromString(const TString& str)
{
    std::string stdstr = str.Data();
    return isTypeObjFromString<T>(stdstr);
}

template <typename T>
bool utils::getTypeObjFromString(const std::string& str, T& obj)
{
    std::istringstream iss(str);
    return !(iss >> obj).fail();
}
template <typename T>
T utils::getTypeObjFromString(const std::string& str)
{
    T toReturn;
    if (!getTypeObjFromString(str,toReturn))
        printf("Failed to convert object: %s\n",str.c_str());

    return toReturn;
}
template <typename T>
bool utils::getTypeObjFromString(const TString& str, T& obj)
{
    std::string stdstr = str.Data();
    return getTypeObjFromString(stdstr,obj);
}
template <typename T>
T utils::getTypeObjFromString(const TString& str)
{
    T toReturn;
    if (!getTypeObjFromString(str,toReturn))
        printf("ERROR: Failed to convert object: %s\n",str.Data());

    return toReturn;
}

template <typename T>
bool utils::vectorize(const TString& str, const TString& sep, std::vector<T>& result)
{
    bool success = true;
    result.clear();

    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);
    while(TObjString* os=(TObjString*)istr())
    {
        T obj;
        if (!getTypeObjFromString(os->GetString(),obj))
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
std::vector<T> utils::vectorize(const TString& str, const TString& sep)
{
    std::vector<T> result;
    TObjArray* tokens = str.Tokenize(sep);
    TIter istr(tokens);

    while(TObjString* os=(TObjString*)istr())
    {
        T obj;
        if (!getTypeObjFromString(os->GetString(),obj))
            printf("ERROR: String \"%s\" is not the requested type\n",os->GetString().Data());
        result.push_back(obj);
    }
    delete tokens;
    
    return result;
}

} // end JCT namespace

#endif


