//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** This include file contains inline functions describing LAr Parameters properties used in
    LArReadParamsFromFile and LArParams2Ntuple.
    To use them you must use namespace operator 'LArParamsProperties::' 
    or put the line 'using LArParamsProperties'
   * @author M. Fanti
   * 27.02.2006
*/
#ifndef LARCALIBTOOLS_LARPARAMSPROPERTIES_H
#define LARCALIBTOOLS_LARPARAMSPROPERTIES_H

#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArRinjComplete.h"
#include "LArRawConditions/LArTshaperComplete.h"
#include "LArRawConditions/LArEMEC_CphiComplete.h"
#include "LArRawConditions/LArEMEC_HValphaComplete.h"
#include "LArRawConditions/LArEMEC_HVbetaComplete.h"
#include "LArRawConditions/LArCableLengthComplete.h"
#include "LArRawConditions/LArCableAttenuationComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"

namespace LArParamsProperties {

  ////////////////////////////////////////////////////////////////////////////////
  //
  // To add a new LAr data class (e.g. LArNewClass) containing payload objects
  // (e.g. LArPayload) you must:
  //
  // (0) overload the 'inline bool isValid()' function with the reference
  //     to the payload object (e.g. isValid(const LArPayload&) {return(...);};
  //     giving  a validity criterion
  //
  // (1) add its sequential number to the enum, before END_OF_LIST
  //     (call it with the class name without 'LAr', e.g. 'NewClass')
  //
  // (2) overload the 'inline unsigned getClassIndex()' with the pointer 
  //     to the new class (e.g. getClassIndex(LArNewClass*) {return NewClass;};
  //
  // (3) add the properties of the new class to the functions:
  //     ClassNames(), numberOfParams(unsigned), isGainDependent(unsigned) 
  //
  // all the rest should be self-consistent and does not need any update.
  //
  ////////////////////////////////////////////////////////////////////////////////

  // validity of payload data ( invalid = empty )
  //----------------------------------------------
  bool isValid(const LArCaliPulseParamsP & p);
  bool isValid(const LArDetCellParamsP & p);
  bool isValid(const LArPhysCaliTdiffP & p);
  bool isValid(const LArTdriftP & p);
#ifdef LARRAWCONDITIONS_LARSINGLEFLOATP
  bool isValid(const LArSingleFloatP & p);
#endif
#ifdef LARRAWCONDITIONS_LARMPHYSOVERMCALP1
  bool isValid(const LArMphysOverMcalP1 & p);
#endif
  bool isValid(const LArRinjP & p);
  bool isValid(const LArTshaperP & p);
  bool isValid(const LArEMEC_CphiP & p);
  bool isValid(const LArEMEC_HValphaP & p);
  bool isValid(const LArEMEC_HVbetaP & p);
  bool isValid(const LArCableLengthP & p);
  bool isValid(const LArCableAttenuationP & p);
  bool isValid(const LArOFCBinP & p);
  bool isValid(const LArPhysCaliTdiffP & p);

  // sequential number associated to foreseen data classes
  //-------------------------------------------------------
  enum ClassNumbering { 
    CaliPulseParamsComplete = 0 ,
    DetCellParamsComplete ,
    PhysCaliTdiffComplete ,
    TdriftComplete ,
    MphysOverMcalComplete ,
    RinjComplete ,
    TshaperComplete ,
    EMEC_CphiComplete ,
    EMEC_HValphaComplete ,
    EMEC_HVbetaComplete ,
    CableLengthComplete ,
    CableAttenuationComplete ,
    //CaliPulseParamsVsCalib ,
    OFCBinComplete ,
    END_OF_LIST          // used to count allowed data classes
  } ;

  // conversion: data class pointer --> class index
  //------------------------------------------------
  unsigned getClassIndex(const LArCaliPulseParamsComplete*);
  unsigned getClassIndex(const LArDetCellParamsComplete*);
  unsigned getClassIndex(const LArPhysCaliTdiffComplete*);
  unsigned getClassIndex(const LArTdriftComplete*);
  unsigned getClassIndex(const LArMphysOverMcalComplete*);
  unsigned getClassIndex(const LArRinjComplete*);
  unsigned getClassIndex(const LArTshaperComplete*);
  unsigned getClassIndex(const LArEMEC_CphiComplete*);
  unsigned getClassIndex(const LArEMEC_HValphaComplete*);
  unsigned getClassIndex(const LArEMEC_HVbetaComplete*);
  unsigned getClassIndex(const LArCableLengthComplete*);
  unsigned getClassIndex(const LArCableAttenuationComplete*);
  //unsigned getClassIndex(const LArCaliPulseParamsVsCalib*);
  unsigned getClassIndex(const LArOFCBinComplete*);
  unsigned getClassIndex(const LArPhysCaliTdiffComplete*);

  // vector of class names
  //-----------------------
  const std::vector< std::string > & ClassNames();

  // number of parameters hosted in each data class
  //------------------------------------------------
  unsigned numberOfParams(unsigned index);

  // gain dependency of data class parameters
  //------------------------------------------
  bool isGainDependent(unsigned index);

  /////////////////////////////////////////
  //  don't modify anything from now on!
  /////////////////////////////////////////

  // getClassName : from class index and class pointer
  //---------------------------------------------------
  const std::string & getClassName(unsigned index);
  template <class T>
  const std::string & getClassName(const T* p) { return getClassName(getClassIndex(p)) ; }

  // get class index from class name :
  //-----------------------------------
  unsigned getClassIndex(std::string className);

  // number of parameters contained in each data class
  //---------------------------------------------------
  unsigned numberOfParams(std::string className);

  template <class T>
  unsigned numberOfParams(const T* p) { return numberOfParams(getClassIndex(p)) ; } 

  // gain dependance of data class
  //-------------------------------  
  bool isGainDependent(std::string className);

  template <class T>
  bool isGainDependent(const T* p) { return isGainDependent(getClassIndex(p)) ; } 

  // keyword for detector store
  //----------------------------
  std::string keyword(const std::string& classname);
  std::string keyword(unsigned i);

  template < class T > 
  std::string keyword(const T* p){ return keyword(getClassName(p)) ; } 

} 

#endif 
