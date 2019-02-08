/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArParamsProperties.h"

// validity of payload data ( invalid = empty )
//----------------------------------------------

bool LArParamsProperties::isValid(const LArCaliPulseParamsP & p)  { return ( p.m_Tcal   != ILArCaliPulseParams::ERRORCODE ) ; } 
bool LArParamsProperties::isValid(const LArDetCellParamsP & p)    { return ( p.m_Omega0 != ILArDetCellParams::ERRORCODE )   ; } 
bool LArParamsProperties::isValid(const LArPhysCaliTdiffP & p)    { return ( p.m_Tdiff  != ILArPhysCaliTdiff::ERRORCODE )   ; } 
bool LArParamsProperties::isValid(const LArTdriftP & p)           { return ( p.m_Tdrift != ILArTdrift::ERRORCODE )          ; } 
#ifdef LARRAWCONDITIONS_LARSINGLEFLOATP
bool LArParamsProperties::isValid(const LArSingleFloatP & p)      { return ( p.isEmpty());}
#endif
#ifdef LARRAWCONDITIONS_LARMPHYSOVERMCALP1
bool LArParamsProperties::isValid(const LArMphysOverMcalP1 & p)   { return ( p.m_MphysOverMcal >= 0 ) ; } 
#endif
bool LArParamsProperties::isValid(const LArRinjP & p)             { return ( p.m_Rinj != ILArRinj::ERRORCODE ) ; } 
bool LArParamsProperties::isValid(const LArTshaperP & p)          { return ( p.m_Tshaper != ILArTshaper::ERRORCODE ) ; } 
bool LArParamsProperties::isValid(const LArEMEC_CphiP & p)        { return ( p.m_EMEC_Cphi != ILArEMEC_Cphi::ERRORCODE ) ; } 
bool LArParamsProperties::isValid(const LArEMEC_HValphaP & p)     { return ( p.m_EMEC_HValpha != ILArEMEC_HValpha::ERRORCODE ) ; }
bool LArParamsProperties::isValid(const LArEMEC_HVbetaP & p)      { return ( p.m_EMEC_HVbeta != ILArEMEC_HVbeta::ERRORCODE ) ; }
bool LArParamsProperties::isValid(const LArCableLengthP & p)      { return ( p.m_CableLength != ILArCableLength::ERRORCODE ) ; }
bool LArParamsProperties::isValid(const LArCableAttenuationP & p) { return ( p.m_CableAttenuation != ILArCableAttenuation::ERRORCODE ) ; }
bool LArParamsProperties::isValid(const LArOFCBinP & p) { return ( p.m_bin != LArOFCBinP::ERRORCODE ) ; }

// conversion: data class pointer --> class index
//------------------------------------------------
unsigned LArParamsProperties::getClassIndex(const LArCaliPulseParamsComplete*)   { return CaliPulseParamsComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArDetCellParamsComplete*)     { return DetCellParamsComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArPhysCaliTdiffComplete*)     { return PhysCaliTdiffComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArTdriftComplete*)            { return TdriftComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArMphysOverMcalComplete*)     { return MphysOverMcalComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArRinjComplete*)              { return RinjComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArTshaperComplete*)           { return TshaperComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArEMEC_CphiComplete*)         { return EMEC_CphiComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArEMEC_HValphaComplete*)      { return EMEC_HValphaComplete ; }
unsigned LArParamsProperties::getClassIndex(const LArEMEC_HVbetaComplete*)       { return EMEC_HVbetaComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArCableLengthComplete*)       { return CableLengthComplete ; } 
unsigned LArParamsProperties::getClassIndex(const LArCableAttenuationComplete*)  { return CableAttenuationComplete ; } 
//unsigned LArParamsProperties::getClassIndex(const LArCaliPulseParamsVsCalib*)    { return CaliPulseParamsVsCalib ; } 
unsigned LArParamsProperties::getClassIndex(const LArOFCBinComplete*)    { return OFCBinComplete ; } 

// vector of class names
//-----------------------
const std::vector< std::string > & LArParamsProperties::ClassNames() 
{
  static std::vector<std::string> names ;
  if ( names.size() == 0 ) {
    names.resize(END_OF_LIST) ;
    names[CaliPulseParamsComplete]    =  "LArCaliPulseParamsComplete" ;
    names[DetCellParamsComplete]      =  "LArDetCellParamsComplete" ;
    names[PhysCaliTdiffComplete]      =  "LArPhysCaliTdiffComplete" ;
    names[TdriftComplete]             =  "LArTdriftComplete" ;
    names[MphysOverMcalComplete]      =  "LArMphysOverMcalComplete" ;
    names[RinjComplete]               =  "LArRinjComplete" ;
    names[TshaperComplete]            =  "LArTshaperComplete" ;
    names[EMEC_CphiComplete]          =  "LArEMEC_CphiComplete" ;
    names[EMEC_HValphaComplete]       =  "LArEMEC_HValphaComplete" ;
    names[EMEC_HVbetaComplete]        =  "LArEMEC_HVbetaComplete" ;
    names[CableLengthComplete]        =  "LArCableLengthComplete" ;
    names[CableAttenuationComplete]   =  "LArCableAttenuationComplete" ;
    //names[CaliPulseParamsVsCalib]     =  "LArCaliPulseParamsVsCalib" ;
    names[OFCBinComplete]             =  "LArOFCBinComplete" ;
    names[PhysCaliTdiffComplete]      =  "LArPhysCaliTdiffComplete" ;
  }
  return names ;
} 

// number of parameters hosted in each data class
//------------------------------------------------
unsigned LArParamsProperties::numberOfParams(unsigned index) 
{
  static std::vector<unsigned> nOfPars ;
  if ( nOfPars.size() == 0 ) {
    nOfPars.resize(END_OF_LIST) ;
    nOfPars[CaliPulseParamsComplete]   = 5 ; 
    nOfPars[DetCellParamsComplete]     = 2 ;
    nOfPars[PhysCaliTdiffComplete]     = 1 ;
    nOfPars[TdriftComplete]            = 1 ;
    nOfPars[MphysOverMcalComplete]     = 1 ;
    nOfPars[RinjComplete]              = 1 ;
    nOfPars[TshaperComplete]           = 1 ;
    nOfPars[EMEC_CphiComplete]         = 1 ;
    nOfPars[EMEC_HValphaComplete]      = 1 ;
    nOfPars[EMEC_HVbetaComplete]       = 1 ;
    nOfPars[CableLengthComplete]       = 1 ;
    nOfPars[CableAttenuationComplete]  = 1 ;
    //nOfPars[CaliPulseParamsVsCalib]    = 5 ; 
    nOfPars[OFCBinComplete]            = 1 ; 
    nOfPars[PhysCaliTdiffComplete]     = 1 ; 
  }
  if ( index < END_OF_LIST ) return nOfPars[index] ;
  return 0 ;
}

// gain dependency of data class parameters
//------------------------------------------
bool LArParamsProperties::isGainDependent(unsigned index) 
{
  static std::vector<bool> useGain ;
  if ( useGain.size() == 0 ) {
    useGain.resize(END_OF_LIST) ;
    useGain[CaliPulseParamsComplete]   = true ; 
    useGain[DetCellParamsComplete]     = true ;
    useGain[PhysCaliTdiffComplete]     = true ;
    useGain[TdriftComplete]            = false ;
    useGain[MphysOverMcalComplete]     = true ;
    useGain[RinjComplete]              = false ;
    useGain[TshaperComplete]           = false ;
    useGain[EMEC_CphiComplete]         = false ;
    useGain[EMEC_HValphaComplete]      = false ;
    useGain[EMEC_HVbetaComplete]       = false ;
    useGain[CableLengthComplete]       = false ;
    useGain[CableAttenuationComplete]  = false ;
    //useGain[CaliPulseParamsVsCalib]    = false ; 
    useGain[OFCBinComplete]            = true ; 
    useGain[PhysCaliTdiffComplete]     = true ; 
  }
  if ( index < END_OF_LIST ) return useGain[index] ;
  return false ;
}

// getClassName : from class index and class pointer
//---------------------------------------------------
const std::string & LArParamsProperties::getClassName(unsigned index) 
{
  static std::string empty("");
  const std::vector<std::string> & names = ClassNames() ;
  if ( index < names.size() )  return names[index] ;
  return empty ;
}

// get class index from class name :
//----------------------------------
unsigned LArParamsProperties::getClassIndex(std::string className) 
{ 
  const std::vector<std::string> & names = ClassNames() ;
  unsigned N =  names.size() ;
  for ( unsigned i=0 ; i<N ; i++ ) {
    if ( className == names[i] ) return i ;
  }
  return N ;
} 

// number of parameters contained in each data class
//---------------------------------------------------
unsigned LArParamsProperties::numberOfParams(std::string className) { return numberOfParams(getClassIndex(className)) ; } 


// gain dependance of data class
//-------------------------------  
bool LArParamsProperties::isGainDependent(std::string className) { return isGainDependent(getClassIndex(className)) ; } 


// keyword for detector store
//----------------------------
std::string LArParamsProperties::keyword(const std::string& classname) 
{
  unsigned len = classname.length() ;
  if ( classname.substr(len-8,8) == std::string("Complete") )  return classname.substr(0,len-8) ;
  if ( classname.substr(len-7,7) == std::string("VsCalib") )   return classname.substr(0,len-7) ;
  return classname ;
} 

std::string LArParamsProperties::keyword(unsigned i) { return keyword(getClassName(i)) ; } 
