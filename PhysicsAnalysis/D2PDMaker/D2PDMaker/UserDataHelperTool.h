/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_USERDATAHELPERTOOL_H
#define D2PDMAKER_USERDATAHELPERTOOL_H

//============================================================================
// Name: UserDataHelperTool.h
//
/**
   @class UserDataHelperTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Define all needed fitler methods here

*/
//=============================================================================


// STL includes
#include <string>

// DataModel
#include "SGTools/CLASS_DEF.h"

// Athena Tool
#include "AthenaBaseComps/AthAlgTool.h"


// forwar declarations
class IAthenaBarCode;


static const InterfaceID IID_UserDataHelperTool("UserDataHelperTool", 1, 0);


class UserDataHelperTool : public AthAlgTool
{


public:

  /** Default constructor */
  UserDataHelperTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent );

  /** Default destructor */
  virtual ~UserDataHelperTool() ;

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_UserDataHelperTool; };

  
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();


public:

  /**  Default method that tries to retrieve the UserData of userDataLabel for a 
       given object abd. It first looks into memory and only afterwards in the
       input file. */
  StatusCode passElementUserDataCut( const IAthenaBarCode* inav,
                                     std::string &userDataLabel,
                                     double userDataMinCut,
                                     double userDataMaxCut,
                                     bool &isPassed );


}; // End class UserDataHelperTool


#endif
