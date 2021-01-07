/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTAUXDYN_IO_H
#define ROOTAUXDYN_IO_H

#include <string>
#include <mutex>

class TBranch;
class IRootAuxDynReader;


namespace RootAuxDynIO {
   /// Common post-fix for the names of auxiliary containers in StoreGate
   static const std::string AUX_POSTFIX = "Aux.";
   static const std::string AUXDYN_POSTFIX = "Dyn.";


  /**
   * @brief Check is a branch holds AuxStore objects
   * @param branch TBranch to check
   */
   bool isAuxDynBranch(TBranch *branch);


 /**
   * @brief Crate RootAuxDynReader for a given TBranch
   * @param branch TBranch in which AuxStore objects are

   Will return null in case of problems
   */ 
   std::unique_ptr<IRootAuxDynReader>  getReaderForBranch(TBranch *branch);
  
 /**
   * @brief Construct branch name for a given dynamic attribute
   * @param attr_name the name of the attribute
   * @param baseBranchName branch name for the main AuxStore object
   */
   std::string auxBranchName(const std::string& attr_name, const std::string& baseBranchName);


}
 

class IRootAuxDynReader
{
public :

 /**
   * @brief Attach specialized AuxStore for reading dynamic attributes
   * @param object object instance to which the store will be attached to - has to be an instance of the type the reader was created for
   * @param ttree_row

   Use this method to instrument an AuxStore object AFTER it was read (every time it is read)
   This will attach its dynamic attributes with read-on-demand capability
   */
  virtual void addReaderToObject(void* object, size_t ttree_row, std::recursive_mutex* iomtx = nullptr) = 0;

  virtual size_t getBytesRead() = 0;

  virtual void resetBytesRead() = 0; 

  
  virtual ~IRootAuxDynReader() {}
};


#endif

