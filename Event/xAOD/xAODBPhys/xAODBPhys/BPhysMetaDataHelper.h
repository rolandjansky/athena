/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
/** 
 *  @file   BPhysMetaDataHelper.h
 *  @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 *
 *  @brief  B-physcis FileMetaData helpers.
 *
 *  This class provides an interface to B-physics augmemtation of
 *  the FileMetaData objects.
 *
 *  @note
 *
 *  Usage example:
 *
 *  @code
 *    void myFunction(FileMetaData* metaObj) {
 *      // Let "metaObj" be some FileMetaData object either 
 *      // being retrieved from the input file or
 *      // created by the b-physics software
 *      // We gain access to augmentations through the helper class:
 *      xAOD::BPhysMetaDataHelper metaData(metaObj);
 *
 *
 *    }
 *  @endcode
 *
 */  

#ifndef XAOD_BPHYSMETADATAHELPER_H
#define XAOD_BPHYSMETADATAHELPER_H

// System include(s):
#include <assert.h>
#include <map>
#include <iosfwd>
#include <vector>

// ROOT include(s):
#include "TString.h"

// EDM include(s):
#include "xAODMetaData/FileMetaData.h"

/** Class for the B-physcis FileMetaData helpers */
namespace xAOD {

  ///
  /// Helper class providing easy access to FileMetaData object
  /// holding file-level metadata about an xAOD file specific to
  /// B-physics derivations.
  ///
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  class  ATLAS_NOT_THREAD_SAFE  BPhysMetaDataHelper {

  public:
    
    ///
    /// @brief Main constructor
    ///
    /// The main contructor.
    /// BPhysMetaDataHelper does NOT take ownership of the class "metaObj".
    ///
    /// @param[in] fm Pointer to the xAOD::FileMetaData.
    ///
    BPhysMetaDataHelper(const xAOD::FileMetaData* fm);

    ///
    /// Getter method for the cached xAOD::FileMetaData object.
    ///
    /// @returns cached xAOD::FileMetaData.
    ///
    const xAOD::FileMetaData* metaObj() const;

    /// Set prefix for variable names
    void setPrefix(std::string prefix);

    /// Map of metadata names and types
    std::map<std::string, const std::type_info*> varTypes() const;

    /// List of metadata names and types as string
    ///
    /// @param[in] header optional header string for table
    /// @return TString with list of variable types used by metadata
    ///
    TString varTypesToString(TString header="") const;

    ///
    /// Complete metadata contents as TString
    ///
    /// @param[in] header optional header string for table
    /// @return TString with metadata contents as table
    ///
    TString metaDataToString(TString header="") const;
  
    ///
    /// @name Getter methods for different metadata types
    /// @{
    ///
    /// @param[in]  name name of metadata variable (without prefix)
    /// @param[out] val  value of metadata variable
    /// @returns    true if named variable exists
    ///
    /// @note applies prefix to each variable name
    ///
    bool value(const std::string& name, int&                      val) const;
    bool value(const std::string& name, float&                    val) const;
    bool value(const std::string& name, double&                   val) const;
    bool value(const std::string& name, bool&                     val) const;
    bool value(const std::string& name, std::string&              val) const;
    bool value(const std::string& name, std::vector<int>&         val) const;
    bool value(const std::string& name, std::vector<float>&       val) const;
    bool value(const std::string& name, std::vector<double>&      val) const;
    bool value(const std::string& name, std::vector<bool>&        val) const;
    bool value(const std::string& name, std::vector<std::string>& val) const;
    /// @}
    
    ///
    /// @name Getter methods for maps of different metadata types
    /// @{
    ///
    std::map<std::string, int>                       valuesI()  const;
    std::map<std::string, float>                     valuesF()  const;
    std::map<std::string, double>                    valuesD()  const;
    std::map<std::string, bool>                      valuesB()  const;
    std::map<std::string, std::string>               valuesS()  const;
    std::map<std::string, std::vector<int> >         valuesVI() const;
    std::map<std::string, std::vector<float> >       valuesVF() const;
    std::map<std::string, std::vector<double> >      valuesVD() const;
    std::map<std::string, std::vector<bool> >        valuesVB() const;
    std::map<std::string, std::vector<std::string> > valuesVS() const;
    /// @}
    
  protected:
    /// Updating internal variable-to-variable-type cache
    void cacheVarTypes() const;

  protected:
    /// FileMetaData object pointer
    const xAOD::FileMetaData* m_fm;

    /// Prefix for variable names
    std::string m_prefix;

    /// @name Internal caches and corresponding switches
    /// @{

    /// map of metadata names and variable types
    mutable std::map<std::string, const std::type_info*> m_tmap;
    /// flag indicating an up-to-date cache
    mutable bool m_tmapOk;

    /// @}
    
  }; // BPhysMetaDataHelper
    
} // namespace xAOD

#endif // XAOD_BPHYSMETADATAHELPER_H
