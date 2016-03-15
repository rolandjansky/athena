///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRegionsTool.cxx 
// Implementation file for class METRegionsTool 
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <vector>

// METReconstruction includes
#include "METReconstruction/METRegionsTool.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METRegionsTool::METRegionsTool(const std::string& name) :
    AsgTool(name),
    METRefinerTool(name),
    m_region_values(0),
    m_region_names(0),
    m_region_eta_values(0),
    m_debug_counter(0)
  {
    declareProperty( "InputMETContainer" , m_base_met_containerKey = ""  );
    declareProperty( "InputMETMap"       , m_base_met_mapKey       = ""  );
    declareProperty( "InputMETKey"       , m_base_met_inputKey     = ""  ); 
    declareProperty( "RegionValues"      , m_region_values               );
    declareProperty( "PrintEvents"       , m_print_events          = 10  );
  }

  // Destructor
  ///////////////
  METRegionsTool::~METRegionsTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRegionsTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_base_met_containerKey.size()==0 || m_base_met_inputKey.size()==0 ) {
      ATH_MSG_FATAL("Both InputMETContainer or InputMETKey must be provided.");
      return StatusCode::FAILURE;
    }

    if( m_region_values.size() == 0) {
      ATH_MSG_WARNING("Setting up default regions");
      m_region_values.push_back( 1.5);
      m_region_values.push_back( 3.2);
      m_region_values.push_back(10.0);
    } else {
      std::sort(m_region_values.begin(), m_region_values.end()); // Default should be good enough
    } 

    // Set the names and eta ranges
    float eta_min = 0., eta_max = 0.;
    for(unsigned int index=0; index<m_region_values.size(); ++index) {
      if(index == 0) { 
        eta_min = 0.; 
        eta_max = m_region_values.at(index);
      } else {
        eta_min = m_region_values.at(index-1);
        eta_max = m_region_values.at(index);
      }

      // A few nice formatting
      std::string lowerName = std::to_string(eta_min), higherName = std::to_string(eta_max);
      lowerName.erase(lowerName.find_last_not_of('0') + 1, std::string::npos);
      higherName.erase(higherName.find_last_not_of('0') + 1, std::string::npos);
      if(lowerName[lowerName.size()-1] == '.') lowerName.append("0");
      if(higherName[higherName.size()-1] == '.') higherName.append("0");
      std::replace(lowerName.begin(),lowerName.end(),'.','p'); 
      std::replace(higherName.begin(),higherName.end(),'.','p'); 

      // Names
      m_region_names.push_back(lowerName + "_" + higherName);

      // Regions
      std::pair<float, float> currentPair(eta_min,eta_max);
      m_region_eta_values.push_back(currentPair);
    }

    // Only to debug nightly problem - switch to DEBUG : ASM 14/3/2016
    for(unsigned int index=0; index < m_region_eta_values.size(); index++) {
      ATH_MSG_INFO("METRegion " << m_region_names.at(index)  << 
                   " is set w/ eta [" << m_region_eta_values.at(index).first << 
                   "," << m_region_eta_values.at(index).second << "]");
    } 
    //

    return StatusCode::SUCCESS;
  }

  StatusCode METRegionsTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode METRegionsTool::executeTool(xAOD::MissingET* metTerm_central, xAOD::MissingETComponentMap* metMap) 
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    // First retrieve the BaseMET
    const MissingETContainer* base_met_container = 0;
    if( evtStore()->retrieve( base_met_container, m_base_met_containerKey ).isFailure() ) {
      ATH_MSG_WARNING("Could not retrieve base MET container!");
      return StatusCode::SUCCESS;
    }

    // First retrieve the BaseMET
    const MissingETComponentMap* base_met_map = 0;
    if( evtStore()->retrieve( base_met_map, m_base_met_mapKey ).isFailure() ) {
      ATH_MSG_WARNING("Could not retrieve base MET map!");
      return StatusCode::SUCCESS;
    }

    // Add to the Container 
    MissingETContainer* metCont = static_cast<MissingETContainer*>( metTerm_central->container() );
    if(!metCont) {
      ATH_MSG_DEBUG("METRegionsTool expecting a MissingETContainer given an inconsistent type.");
      return StatusCode::SUCCESS;
    }

    // Get the components of the base MET
    MissingETContainer::const_iterator iterBaseCont = base_met_container->find( m_base_met_inputKey );
    MissingETComponentMap::const_iterator iterBaseConstit = MissingETComposition::find( base_met_map, (*iterBaseCont) );

    if( iterBaseCont == base_met_container->end() ) {
      ATH_MSG_WARNING("Could not find base MET object " << m_base_met_inputKey << " in MET container!");
      return StatusCode::SUCCESS;
    }

    if( iterBaseConstit == metMap->end() ) {
      ATH_MSG_WARNING("Could not find base METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }

    // Create MET term, push to container and maps
    for(unsigned int index=0; index<m_region_values.size(); ++index) {
      MissingET* currentMetTerm;
      if(index == 0) { 
        currentMetTerm = metTerm_central; 
        currentMetTerm->setName( m_base_met_inputKey + "_" + m_region_names.at(index) );
      } else { 
        currentMetTerm = new MissingET(0.,0.,0.);
        currentMetTerm->setName( m_base_met_inputKey + "_" + m_region_names.at(index) );
        ATH_MSG_DEBUG("Adding MET Term " << currentMetTerm->name() << " to MET container" );
        metCont->push_back( currentMetTerm ); 
        ATH_MSG_DEBUG("Adding MET Term " << currentMetTerm->name() << " to MET map" );
        MissingETComposition::add( metMap, metCont->back() );
      }
      // Only to debug nightly problem - switch to DEBUG : ASM 14/3/2016
      if(m_debug_counter < m_print_events) {
        ATH_MSG_INFO("METRegion :: Adding MET term" << currentMetTerm->name() << 
                     " for region [" << m_region_eta_values.at(index).first << "," << m_region_eta_values.at(index).second << "]" ); 
      }
      //
      m_mapRangeToMET.insert(std::pair<std::pair<float,float>,MissingET*>(m_region_eta_values.at(index),metCont->back()));
    }
    m_debug_counter++; // Only to debug nightly problem - switch to DEBUG : ASM 14/3/2016 

    // Fill the MET terms and maps
    if(!(*iterBaseConstit)->empty()) {
      vector<const IParticle*> objectList = (*iterBaseConstit)->objects();
      vector<const IParticle*> dummyList;

      for( vector<const IParticle*>::const_iterator iObj = objectList.begin(); iObj!=objectList.end(); ++iObj ) {
	    MissingETBase::Types::weight_t objWeight = (*iterBaseConstit)->weight(*iObj);
        for(std::map< std::pair<float,float>, MissingET* >::iterator it=m_mapRangeToMET.begin();
            it!=m_mapRangeToMET.end(); ++it) {
            if( fabs((*iObj)->eta()) > it->first.first && fabs((*iObj)->eta()) < it->first.second ) {
              it->second->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
                              (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
                              (*iObj)->pt()*objWeight.wet());
	          MissingETComposition::insert( metMap, it->second, *iObj, dummyList, objWeight );
            }
        } // end of loop over met terms
      } // end of loop over constituents
    }

    // Clean map for the next event
    m_mapRangeToMET.clear();

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
