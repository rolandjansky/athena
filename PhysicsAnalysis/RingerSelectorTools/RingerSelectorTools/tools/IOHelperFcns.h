/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOHelperFcns.h 679254 2015-06-30 12:54:50Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_IOHELPERFCNS_H
#define RINGERSELECTORTOOLS_TOOLS_IOHELPERFCNS_H

// STL includes:
#include <stdexcept>
#include <string>
#include <memory>

// ROOT includes:
#include <TFile.h>
#include <TList.h>
#include <THashList.h>

#ifndef INCLUDE_HEADER_ONLY // Avoid circular includes
#define INCLUDE_HEADER_ONLY
#endif
#include "RingerSelectorTools/RingerSelectorToolsDefs.h"
#undef INCLUDE_HEADER_ONLY

namespace Ringer {

namespace IOHelperFcns {

/**
 * @brief Set varDest to varName key pointer inside TDirectory dir
 **/
template <typename ReadingType, typename WrittenType = ReadingType >
void readVar(TDirectory* dir, const char* varName, ReadingType& varDest);

/**
 * @brief Write var with name varName at directory dir, raises otherwise.
 *
 * It cannot write non-scalar C objects, such as float[] and so on. Use
 * std::vector<> otherwise. It also needs the root dictionary available 
 * for the type being writen. Some std::vector types available are:
 *
 * std::vector<float>
 * std::vector<double>
 * std::vector<unsigned int>
 *
 * But their long counterparts seem not possible to be written. 
 *
 * For enumeration types, convert them into integers and when reading, convert
 * them back, i.e.:
 *
 * --- Writing ---
 * EtaDependency etaDependency = EtaIndependent;
 * writeVar< EtaDependecy, unsigned int>(dir, "etaDep", etaDependency);
 *
 * --- Reading ---
 * EtaDependency etaDependency;
 * readVar< EtaDependency, unsigned int>(dir, "etaDep", etaDependency);
 **/
template <typename InputType, typename WrittenType = InputType >
void writeVar(TDirectory* dir, const char* varName, InputType& var);

/**
 * @brief Make index string from index
 *
 * If vector contains indexes [1], for instance, then returning string will
 * be "@(1)".
 *
 **/
std::string makeIdxStr(unsigned int idxV);

/**
 * @brief Make index string from index vector
 *
 * If vector contains indexes [1,2], for instance, then returning string will
 * be "@(1,2)".
 *
 **/
std::string makeIdxStr(std::vector<unsigned int> &idxV);

/**
 * @brief Fill vector with indexes contained in string made with makeIdxStr.
 *
 * Throws runtime_error if string does not contains string formated as created
 * by makeIdxStr or if it cannot retrieve n-th index.
 **/
void getIdxVecFromStr(const char* cstr, std::vector<unsigned int> &n);

/**
 * @brief Get n-th index from string containing makeIdxStr.
 *
 * Throws runtime_error if string does not contains string formated as created
 * by makeIdxStr or if it cannot retrieve n-th index.
 **/
unsigned int getIdxFromStr(const char* cstr, unsigned int n);

/**
 * @brief Returns package version as unsigned
 *
 * Remove package name and '-' from string, keep only package version
 * as RingerSelectorTools-10-11-12 would lead into version 101112.
 *
 * NOTE: This means that a package cannot have any version number higher
 * than 99.
 **/
unsigned version();

/**
 * @brief Check if cStr starts with string cStrStart.
 **/
bool startsWith(const char *cStrStart, const char *cStr);

/**
 * @brief Check if TFile is correctly opened.
 **/
void checkFile(const TFile& file);

/**
 * @brief Check if TFile is exists.
 **/
bool fileExist(const char* fileName);

/**
 * @brief Delete file
 **/
int deleteFile(const char* fileName);

/**
 * @brief Check if directory is not pointing into void.
 *
 * Throws runtime_error otherwise error occurred
 **/
void checkDir(TDirectory* baseDir);

/**
 * @brief Write package version to directory
 **/
void writeVersion(TDirectory* dir);

/**
 * @brief Creates directory in baseDir with given name.
 *
 * Throws runtime_error if error occurred. It also adds to created directory 
 * the package version in which it was created. This can be written using the
 * getWrittenVersion function.
 **/
TDirectory *makeDir(TDirectory* baseDir, const char *name);

/**
 * @brief Retrieve TList with directories on directory dir.
 **/
std::shared_ptr<THashList> getDirList(TDirectory* dir);

/**
 * @brief Gets package written version
 *
 * It returns the package version in which this directory was written,
 * otherwise it throws runtime_error.
 *
 **/
unsigned getWrittenVersion(TDirectory *configDir);

} // namespace IOHelperFcns

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_IOHELPERFCNS_H

#ifndef INCLUDE_HEADER_ONLY // Use to avoid circular includes
#include "IOHelperFcns.icc"
#endif // INCLUDE_HEADER_ONLY

