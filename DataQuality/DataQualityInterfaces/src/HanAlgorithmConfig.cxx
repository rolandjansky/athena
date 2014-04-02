/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanAlgorithmConfig.cxx,v 1.3 2009-05-07 14:45:54 ponyisi Exp $
// **********************************************************************

#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1.h>

#include "DataQualityInterfaces/HanAlgorithmConfig.h"
#include "DataQualityInterfaces/HanConfigAlgLimit.h"
#include "DataQualityInterfaces/HanConfigAlgPar.h"
#include "DataQualityInterfaces/HanConfigAssessor.h"
#include "DataQualityInterfaces/HanUtils.h"


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanAlgorithmConfig::
HanAlgorithmConfig( const HanConfigAssessor& hca, TFile* config )
  : file(config)
  , ref(0)
{
  CopyAlgConfig( hca );
}

HanAlgorithmConfig::
HanAlgorithmConfig( TObject* reference, 
		    const std::map< std::string, double >& parameters,
		    const std::map< std::string, double >& greenThresholds,
		    const std::map< std::string, double >& redThresholds,
		    const HanConfigAssessor* hca)
    : file(0)
    , ref(reference)
    , pars(parameters)
    , grthr(greenThresholds)
    , rdthr(redThresholds)
    , m_hca(hca)
{
}

HanAlgorithmConfig::
~HanAlgorithmConfig()
{
  delete ref;
}


TObject*
HanAlgorithmConfig::
getReference() const
throw (dqm_core::BadConfig)
{
  if (ref) { 
    TObject* rv = ref->Clone();
    if (rv) {
      return rv;
    } else {
      const char* parname = m_hca ? m_hca->GetName() : "???";
      throw dqm_core::BadConfig( ERS_HERE, parname,
				 "Unable to clone reference" );
    }
  }
  const char* parname = m_hca ? m_hca->GetName() : "???";
  throw dqm_core::BadConfig( ERS_HERE, parname, 
			     "No reference histogram provided" );
}


const std::map< std::string, double >&
HanAlgorithmConfig::
getParameters() const
{
  return pars;
}


const std::map< std::string, double >&
HanAlgorithmConfig::
getGreenThresholds() const
{
  return grthr;
}


const std::map< std::string, double >&
HanAlgorithmConfig::
getRedThresholds() const
{
  return rdthr;
}


// *********************************************************************
// Protected Methods
// *********************************************************************

void
HanAlgorithmConfig::
CopyAlgConfig( const HanConfigAssessor& hca )
{
  typedef std::map< std::string, double >::value_type ParsVal_t;
  typedef std::map< std::string, double >::value_type ThrVal_t;
  
  m_hca = &hca;

  std::string parName;
  TIter nextPar( hca.GetAllAlgPars() );
  HanConfigAlgPar* par;
  while( (par = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
    parName = std::string( par->GetName() );
    ParsVal_t parMapVal( parName, par->GetValue() );
    pars.insert( parMapVal );
  }
  
  std::string limName;
  TIter nextLim( hca.GetAllAlgLimits() );
  HanConfigAlgLimit* lim;
  while( (lim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
    limName = std::string( lim->GetName() );
    ThrVal_t greenMapVal( limName, lim->GetGreen() );
    grthr.insert( greenMapVal );
    ThrVal_t redMapVal( limName, lim->GetRed() );
    rdthr.insert( redMapVal );
  }
  
  std::string refName( hca.GetAlgRefName() );
  if( refName != "" ) {
    TKey* key = dqi::getObjKey( file, refName );
    if (key == NULL) {
      std::cout << "ERROR: can't find reference " << refName << std::endl;
    } else {
      ref = key->ReadObj();
      //ref = file->Get(refName.c_str());
    }
  }
}


// *********************************************************************
// Private Methods
// *********************************************************************

} // namespace dqi
