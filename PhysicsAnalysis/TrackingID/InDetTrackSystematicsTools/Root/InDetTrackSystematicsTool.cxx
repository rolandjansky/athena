/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSystematicsTools/InDetTrackSystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"

using std::string;

namespace InDet {


  InDetTrackSystematicsTool::InDetTrackSystematicsTool( const string& name )
    : asg::AsgTool(name)
  {
  }

  StatusCode InDetTrackSystematicsTool::initialize()
  {
    
    // add the affecting systematics to the registry
    auto& registry = CP::SystematicRegistry::getInstance();
    if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
      ATH_MSG_ERROR( "Unable to register systematics" );
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  bool InDetTrackSystematicsTool::isAffectedBySystematic( const CP::SystematicVariation& systematic ) const
  {
    auto affSysts = affectingSystematics();
    return ( affSysts.find(systematic) != affSysts.end() );
  }
 
  CP::SystematicSet InDetTrackSystematicsTool::recommendedSystematics() const
  {
    // it's probably a good idea to change this behavior to return a more intelligently thought-out list
    return affectingSystematics();
  }
  
  CP::SystematicCode InDetTrackSystematicsTool::applySystematicVariation( const CP::SystematicSet& systematics )
  {

    // first check if we already know this systematic configuration
    // look for it in our filter map
    auto sysMapItr = m_sysFilterMap.find(systematics);

    if (sysMapItr == m_sysFilterMap.end()) {

      // filter the input systematic with the affecting systematics
      const CP::SystematicSet affectingSysts = affectingSystematics();
      CP::SystematicSet filteredSysts;
      if ( CP::SystematicSet::filterForAffectingSystematics
	   (systematics, affectingSysts, filteredSysts) != CP::SystematicCode::Ok ) {
	ATH_MSG_ERROR( "Recieved unsupported systematics: " << systematics.name() );
	return CP::SystematicCode::Unsupported;
      }

      // check in here for incompatible systematics
      auto isIn = [&](InDet::TrackSystematic syst) {
	// helper function to check if a systematic exists in the filtered set
	return filteredSysts.find( InDet::TrackSystematicMap[ syst ] ) != filteredSysts.end();
      };
      if ( isIn( TRK_RES_D0_MEAS) + isIn( TRK_RES_D0_MEAS_UP ) + isIn( TRK_RES_D0_MEAS_DOWN ) > 1 ) {
	// should only have 1 of these options simultantously
	ATH_MSG_ERROR( "You must pick only one choice for the d0 resolution systematic" );
	return CP::SystematicCode::Unsupported;
      }
      if ( isIn(TRK_RES_Z0_MEAS) + isIn( TRK_RES_Z0_MEAS_UP ) + isIn( TRK_RES_Z0_MEAS_DOWN ) > 1 ) {
	ATH_MSG_ERROR( "You must pick only one choice for the z0 resolution systematic" );
	return CP::SystematicCode::Unsupported;
      }
      bool isSetTrkLoose = isIn( TRK_FAKE_RATE_LOOSE )
	|| isIn( TRK_EFF_LOOSE_GLOBAL )
	|| isIn( TRK_EFF_LOOSE_IBL )
	|| isIn( TRK_EFF_LOOSE_PP0 )
	|| isIn( TRK_EFF_LOOSE_PHYSMODEL );
      bool isSetTrkTight = isIn( TRK_FAKE_RATE_TIGHT )
	|| isIn( TRK_EFF_TIGHT_GLOBAL )
	|| isIn( TRK_EFF_TIGHT_IBL )
	|| isIn( TRK_EFF_TIGHT_PP0 )
	|| isIn( TRK_EFF_TIGHT_PHYSMODEL );
      if ( isSetTrkLoose && isSetTrkTight) {
	ATH_MSG_ERROR( "Both Loose and TightPrimary versions of systematics are simultaneously active." );
	return CP::SystematicCode::Unsupported;
      }

      // if the systematics set looks supported, then insert it into the map for future use
      sysMapItr = m_sysFilterMap.insert(std::make_pair(systematics, filteredSysts)).first;
    }

    m_activeSysts = &sysMapItr->second;

    return CP::SystematicCode::Ok;
  }
  
  bool InDetTrackSystematicsTool::isActive( TrackSystematic syst ) const
  {
    if (m_activeSysts == nullptr) {
      ATH_MSG_DEBUG( "applySystematicsVariation() has not been successfully called." );
      return false;
    }
    const auto it_syst = m_activeSysts->find(InDet::TrackSystematicMap[syst]);
    return it_syst != m_activeSysts->end();
  }

  TFile* InDetTrackSystematicsTool::getFile(const string& filename) const
  {
    // now the files are stored in the calibration area
    string filenameWithPath = PathResolverFindCalibFile
      ("InDetTrackSystematicsTools/CalibData_21.2_2017-v12/" + filename);
    TFile* file =  TFile::Open(filenameWithPath.data(), "READ");
    if (file != nullptr) return file;
    ATH_MSG_WARNING( "Could not find file " << filename << " in the calibration database." );
    ATH_MSG_WARNING( "Will now look in InDetTrackSystematicsTools/data/ ." );
    ATH_MSG_WARNING( "You should not see this message unless you are a dev testing a new file." );
#ifdef XAOD_STANDALONE
    filenameWithPath = PathResolverFindCalibFile("InDetTrackSystematicsTools/" + filename);
#else
    filenameWithPath = PathResolverFindDataFile("InDetTrackSystematicsTools/data/" + filename);
#endif
    file = TFile::Open(filenameWithPath.data(), "READ");
    return file;
  }

  
}
