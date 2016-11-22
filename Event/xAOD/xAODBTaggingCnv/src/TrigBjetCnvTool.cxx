/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBjetCnvTool.cxx 785653 2016-11-22 06:07:48Z ssnyder $

// EDM include(s):
#include "TrigParticle/TrigEFBjetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"

// Local include(s):
#include "TrigBjetCnvTool.h"

namespace xAODMaker {

  TrigBjetCnvTool::TrigBjetCnvTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< ITrigBjetCnvTool >( this );
  }

  StatusCode TrigBjetCnvTool::initialize() {

    // Greet the user:
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  /**
   * This is the important function of the tool. It takes the TrigEFBjet, and fills an xAOD::TrigBjetContainer with them.
   *
   * @param aod The AOD object to take it from
   * @param xaod The xAOD container to fill it
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode TrigBjetCnvTool::convert( const TrigEFBjetContainer* aod,
                                       xAOD::BTaggingContainer* xaod ) const {

    // A small sanity check. The output container should really be empty...
    if( xaod->size() ) {
      ATH_MSG_WARNING( "The output xAOD container is not empty (size=="
                       << xaod->size() << ")" );
    }

    // Loop over the AOD objects:
    TrigEFBjetContainer::const_iterator aod_itr = aod->begin();
    TrigEFBjetContainer::const_iterator aod_end = aod->end();

    float xTemp=-1;
    float wTemp=-1;
	
    for( ; aod_itr != aod_end; ++aod_itr ) {
	
      //       // Add a new muon RoI object to the output:
      xAOD::BTagging* roi = new xAOD::BTagging();
      xaod->push_back( roi );
	
      // 	         // Now fill it with data:
      //	         roi->setRoiWord((*aod_itr)->roiId());

             
      xTemp=(*aod_itr)->xSV();
      wTemp=pow(10.,xTemp);
      if(xTemp<-5)wTemp=-5;
      if(wTemp>100.) wTemp=-10;
      roi->setSV1_pb(wTemp);
      roi->setSV1_pu(1.);
      //

      xTemp=(*aod_itr)->xIP2D();
      wTemp=pow(10.,xTemp);
      if(xTemp<-5)wTemp=-5;
      if(wTemp>100.) wTemp=-10;
      roi->setIP2D_pb(wTemp);
      roi->setIP2D_pu(1.);

      xTemp=(*aod_itr)->xIP3D();
      wTemp=pow(10.,xTemp);
      if(xTemp<-5)wTemp=-5;
      if(wTemp>100.) wTemp=-10;
      roi->setIP3D_pb(wTemp);
      roi->setIP3D_pu(1.);

      roi->setJetFitter_pb((*aod_itr)->xCHI2());
      roi->setJetFitter_pu(1.);

             
             
      // Here we put W-value instead of X-value; see https://indico.cern.ch/event/346066/contribution/4/material/slides/0.pdf
      // x=-1.0*TMath::Log10(1-(w/(1+w))) =  TMath::Log10(1+w)  
  
      float xComb_temp=(*aod_itr)->xComb();
      float wComb_temp=-1;
      wComb_temp=pow(10., xComb_temp)-1;
      if(xComb_temp<-5)wComb_temp=-5;
      if(wComb_temp>100.) wComb_temp=-10;
      roi->setMV1_discriminant(wComb_temp);



    }


    // Return gracefully:
    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker
