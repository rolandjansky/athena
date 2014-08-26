/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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


// Preprocessor magic for debugging
//#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This classes header
#include "D2PDMaker/UserDataHelperTool.h"

// STL includes
#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
#include "boost/any.hpp"

// DataModel
#include "GaudiKernel/ToolFactory.h"
#include "SGTools/CLASS_DEF.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"


//=============================================================================
// Constructor
//=============================================================================
UserDataHelperTool::UserDataHelperTool( const std::string& type, 
                                        const std::string& name,
                                        const IInterface* parent ) :
  AthAlgTool( type, name, parent )
{
  // declare interface
  declareInterface<UserDataHelperTool>( this );

  // Properties go here
  //declareProperty("filterTool", m_outCollKey="MyCompositeCollection" );

}



//=============================================================================
// Destructor
//=============================================================================
UserDataHelperTool::~UserDataHelperTool()
{
}




//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode UserDataHelperTool::initialize()
{
  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode UserDataHelperTool::finalize()
{
  return StatusCode::SUCCESS;
}





//=============================================================================
// Default method that tries to retrieve the UserData of userDataLabel for a 
// given object abd. It first looks into memory and only afterwards in the 
// input file.
//=============================================================================
StatusCode UserDataHelperTool::passElementUserDataCut( const IAthenaBarCode* abc,
                                                       std::string &userDataLabel,
                                                       double userDataMinCut,
                                                       double userDataMaxCut,
                                                       bool &isPassed )
{
  //-----------------------------------------
  // Default debug message printed to the log file.
  // To let you know that you are in this method.
  //-----------------------------------------
  ATH_MSG_DEBUG ( "Now, trying to retrieve UserData for this object..." );

  //-----------------------------------------
  // The default value for the isPassed return
  //-----------------------------------------
  isPassed = true;


  // Figure out if the requested UserData exists
  if ( userStore()->contains( *abc, userDataLabel ) )
    {
      const boost::any* payload=0;
      ATH_CHECK ( userStore()->retrieveAny( *abc, userDataLabel, payload ) );
      ATH_MSG_VERBOSE ( "Could retrieve UserData with name " << userDataLabel );

      if ( payload )
        {
          // Convert the payload type to double, if possible
          bool foundType(true);
          double value(0.0);
          const std::type_info& payloadType = payload->type();
          if ( payloadType == typeid(bool) ){ value = (double) boost::any_cast<bool>(*payload); }
          else if ( payloadType == typeid(double) ){ value = boost::any_cast<double>(*payload); }
          else if ( payloadType == typeid(float) ){ value = (double) boost::any_cast<float>(*payload); }
          else if ( payloadType == typeid(uint32_t) ){ value = (double) boost::any_cast<uint32_t>(*payload); }
          else if ( payloadType == typeid(int32_t) ){ value = (double) boost::any_cast<int32_t>(*payload); }
          else { foundType = false; }

          if ( foundType )
            {
              isPassed = isPassed && ( userDataMinCut <= value && value <= userDataMaxCut );
            }
          else
            {
              ATH_MSG_WARNING ( "Could not find type of UserData with name/label " << userDataLabel
                                << " ...passing this object!" );
              isPassed = isPassed && true;
            }
        }
    }
  else
    {
      ATH_MSG_WARNING ( "Could not find UserData with name/label " << userDataLabel
                        << " ...passing this object!" );
      isPassed = isPassed && true;
    }


  return StatusCode::SUCCESS;
}

