/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronEfficiencyCorrection/ElectronEfficiencyHelpers.h"
#include "AsgTools/AsgMessaging.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get type and charge of the truth electron
// made them static so they can be used by analyses to do truth matches
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CP::CorrectionCode ElectronEfficiencyHelpers::getEleTruthCharge( const xAOD::Electron& ele,  int& truthcharge )
{

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronEfficiencyHelpers");

  if ( !(ele.isAvailable<int>("firstEgMotherPdgId")) ) {
    msg.msg(MSG::ERROR) << "Link not available for firstEgMotherPdgId...BAD!!!" << endmsg;
    msg.msg(MSG::ERROR) << "Need to have present: ( !(ele.isAvailable<int>('firstEgMotherPdgId')) )" << endmsg;
    return CP::CorrectionCode::OutOfValidityRange;
  }

  truthcharge = (-1)*ele.auxdata<int>("firstEgMotherPdgId");
  // Make truthcharge -1, 0, +1
  truthcharge = (0 < truthcharge) - (truthcharge < 0);

  return CP::CorrectionCode::Ok;

}


CP::CorrectionCode ElectronEfficiencyHelpers::isGoodEle( const xAOD::Electron& ele, bool& goodEle)
{

  // good ele => (firstEgMotherPdgId) == 11 
  // this is the case for type 2,3 and 4 electrons, so is not a real truth match!!!
  // but we believe that it doesn't hurt to apply scale factors for all these cases
  goodEle = false;
  int firstEgPdgId = -9999;

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronEfficiencyHelpers");

  if ( !(ele.isAvailable<int>("firstEgMotherPdgId")) ) {
    msg.msg(MSG::ERROR) << "firstEgMotherPdgId IS NOT AVAILABLE!!" << endmsg;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  else {

    firstEgPdgId = ele.auxdata<int>("firstEgMotherPdgId");

    if ( std::abs(firstEgPdgId) != 11) {

      goodEle = false;
      return CP::CorrectionCode::Ok;

    }
    else {

      goodEle = true;
      return CP::CorrectionCode::Ok;

    }
  }

  return CP::CorrectionCode::OutOfValidityRange;
}


CP::CorrectionCode ElectronEfficiencyHelpers::isPromptEle( const xAOD::Electron& ele, bool& promptEle)
{

  // loose e/y truth match
  // avoid using information from origin to be more general 
  // reproduces e/y loose truth match on a PB Zee sample but not tested widely, yet

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronEfficiencyHelpers");
  

  // first, check if the firstEgMotherType == 2 and return true if it is
  int firstEgType = -9999;

  if ( !(ele.isAvailable<int>("firstEgMotherTruthType")) ) {
    msg.msg(MSG::ERROR) << "firstEgMotherTruthType IS NOT AVAILABLE!!" << endmsg;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  else {

    firstEgType = ele.auxdata<int>("firstEgMotherTruthType");
    
    if ( firstEgType == 2 ) {
      promptEle = true;
      return CP::CorrectionCode::Ok;
    }

  }

  // second, check for the FSR case and return false if it's neither FSR nor type 2
  // ( firstEgMotherType == 5 && firstEgMotherOrigin == 40 )

  if ( !(ele.isAvailable<int>("firstEgMotherTruthOrigin")) ) {
    msg.msg(MSG::ERROR) << "firstEgMotherTruthOrigin IS NOT AVAILABLE!!" << endmsg;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  else {
    
    int firstEgOrigin = ele.auxdata<int>("firstEgMotherTruthOrigin");
    
    if ( firstEgType != 2 && ( firstEgType != 15 || firstEgOrigin != 40 ) ) {
      
      promptEle = false;
      return CP::CorrectionCode::Ok;

    }
    else {
	  
      promptEle = true;
      return CP::CorrectionCode::Ok;
      
    }
  }
  
  return CP::CorrectionCode::OutOfValidityRange;

}
