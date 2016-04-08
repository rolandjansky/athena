/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSelectorTools/Helpers.h"

#include "TSystem.h"

#include <math.h>

namespace JCT
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

bool fileExists(const std::string& fileName)
{
    return fileExists(TString(fileName.c_str()));
}

} // End utils namespace
} // End JCT namespace

