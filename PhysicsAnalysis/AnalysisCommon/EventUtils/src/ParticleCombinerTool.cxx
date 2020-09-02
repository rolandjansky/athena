/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: ParticleCombinerTool.cxx
//
/**
   @class ParticleCombinerTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date July 2014

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and only the
   ones passing the used selections will be written to StoreGate.
*/
//=============================================================================

// This classes header
#include "ParticleCombinerTool.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>
#include <algorithm>

// The composite particle
#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleAuxContainer.h"

// Other EDM includes
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODParticleEvent/Particle.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/TrackParticle.h"
// #include "xAODTruth/TruthParticle.h"
#include "xAODPFlow/PFO.h"
#include "xAODJet/Jet.h"

// include other stuff
#include "CxxUtils/fpcompare.h"
#include "PATCore/CombinatoricsOdometer.h"



//=============================================================================
// Constructor
//=============================================================================
ParticleCombinerTool::ParticleCombinerTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_inCollKeyList(),
  m_metName(""),
  m_outCollKey(""),
  m_writeSplitAux(false),
  m_pdgId(0),
  m_nEventsProcessed(0),
  m_anIPartLinkList(),
  m_inputLinkContainerList(),
  m_inputLinkContainerListToDelete(),
  m_inputLinkContainerNames(),
  m_alreadyUsedInputContainers(),
  m_containerLabels(),
  m_containerMaxN(),
  m_alreadyUsedContainers()
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("InputContainerList", m_inCollKeyList,   "List of input collection keys" );
  declareProperty("MissingETObject",    m_metName="Final", "The name of the xAOD::MissingET object (default: 'Final'" );

  declareProperty("OutputContainer",    m_outCollKey="DefaultCompositeParticleContainer",
                  "The name of the output container (default: 'DefaultCompositeParticleContainer')" );
  declareProperty("WriteSplitOutputContainer", m_writeSplitAux=false,
                  "Decide if we want to write a fully-split AuxContainer such that we can remove any variables" );

  declareProperty("SetPdgId",           m_pdgId=0,         "PDG ID of the new output xAOD::CompositeParticle" );
  declareProperty("SortConstituents",   m_sortConstit=false, "If true: sort the constituents in decending pt order" );

}







//=============================================================================
// Destructor
//=============================================================================
ParticleCombinerTool::~ParticleCombinerTool()
{
}






//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode ParticleCombinerTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_inCollKeyList );
  ATH_MSG_DEBUG ( " using = " << m_metName );
  ATH_MSG_DEBUG ( " using = " << m_outCollKey );
  ATH_MSG_DEBUG ( " using = " << m_writeSplitAux );
  ATH_MSG_DEBUG ( " using = " << m_pdgId );
  ATH_MSG_DEBUG ( " using = " << m_sortConstit );

  // ATH_MSG_DEBUG ( " using mcTruthRequireSameMotherPdgID   = " << m_mcTruthRequireSameMotherPdgID );
  // ATH_MSG_DEBUG ( " using mcTruthRequireSameMotherBarcode = " << m_mcTruthRequireSameMotherBarcode );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;

  return StatusCode::SUCCESS;
}



//=============================================================================
// Run once per event
//=============================================================================
StatusCode ParticleCombinerTool::addBranches() const
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> addBranches " << name() << " on " << m_nEventsProcessed << ". event..." );


  //-----------------------------------------
  // Retrieve and store the input containers
  //-----------------------------------------

  // Create some objects that are needed later on
  bool allInputContainersAreFull = true;
  const xAOD::MissingET* metObject(0);
  int nMet(0);

  // Clear the vector of LinkContainers and a map of their names.
  //  use a map to determine if a given input container
  //  has already been seen, and simply point to the
  //  link container corresponding to  that already-used
  //  input container
  m_inputLinkContainerListToDelete.clear();
  m_inputLinkContainerList.clear();
  m_inputLinkContainerNames.clear();
  m_alreadyUsedInputContainers.clear();

  // Loop over the list of input containers
  for ( const std::string& anInputCollName  :  m_inCollKeyList.value() ) {
    // Check if the current input container name is a LinkContainer
    if ( evtStore()->contains< xAOD::IParticleLinkContainer >( anInputCollName ) ) {
      // Actually retrieve the LinkContainer from StoreGate
      const xAOD::IParticleLinkContainer* aLinkContainer(0);
      ATH_CHECK( evtStore()->retrieve( aLinkContainer , anInputCollName ) );
      ATH_MSG_DEBUG ( "Input link collection = '" << anInputCollName
                      << "' retrieved from StoreGate which has " << aLinkContainer->size() << " entries." );

      // If any input container has zero size, we won't be able to build a
      // CompositeParticle. Thus, we can stop here.
      if ( aLinkContainer->size() == 0 ) {
        allInputContainersAreFull &= false;
        ATH_MSG_VERBOSE("Found an empty input link container");
        break;
      }

      if ( m_inputLinkContainerNames.find(aLinkContainer)  == m_inputLinkContainerNames.end() ) {
        m_inputLinkContainerNames[ aLinkContainer ] = &anInputCollName;
      }
      m_inputLinkContainerList.push_back( aLinkContainer );
    }
    else if ( evtStore()->contains< xAOD::IParticleContainer >( anInputCollName ) ) {
      // This container holds an xAOD::IParticleContainer
      const xAOD::IParticleContainer* aContainer(0);
      ATH_CHECK( evtStore()->retrieve( aContainer, anInputCollName ) );
      ATH_MSG_DEBUG ( "Input collection = '" << anInputCollName
                      << "' retrieved from StoreGate which has " << aContainer->size() << " entries." );

      // If any input container has zero size, we won't be able to build a
      // CompositeParticle. Thus, we can stop here.
      if (aContainer->size() == 0) {
        allInputContainersAreFull &= false;
        ATH_MSG_VERBOSE("Found an empty input container");
        break;
      }

      // Now, create the vector of ElementLinks pointing to the real container
      if ( m_alreadyUsedInputContainers.find( aContainer ) == m_alreadyUsedInputContainers.end() ) {
        xAOD::IParticleLinkContainer* newLinkContainer = new xAOD::IParticleLinkContainer();
        newLinkContainer->reserve( aContainer->size() );
        m_inputLinkContainerListToDelete.push_back( newLinkContainer );

        for ( const auto* aPart : *aContainer ) {
          // Have to use the container() and index() method to be able to also
          // use an input container with SG::VIEW_ELEMENTS, i.e., this way,
          // the IParticleLink will correctly point to the element in the
          // original SG::OWN_ELEMENTS container.
          xAOD::IParticleLink iPartLink( static_cast<const xAOD::IParticleContainer&>(*(aPart->container())),
                                         aPart->index() );
          newLinkContainer->push_back( iPartLink );
        }

        m_alreadyUsedInputContainers[ aContainer ] = newLinkContainer;


        if ( m_inputLinkContainerNames.find( newLinkContainer ) == m_inputLinkContainerNames.end() ) {
          m_inputLinkContainerNames[ newLinkContainer ] = &anInputCollName;
        }
        // Add this new container of ElementLinks to the vector
        m_inputLinkContainerList.push_back( newLinkContainer );
      }
      else {
        // Add an already existing container of ElementLinks again to the vector
        m_inputLinkContainerList.push_back( m_alreadyUsedInputContainers[ aContainer ] );
      }
    }
    else if ( evtStore()->contains< xAOD::MissingETContainer >( anInputCollName ) ) {
      // This container holds an xAOD::IParticleContainer
      const xAOD::MissingETContainer* aMetContainer(0);
      ATH_CHECK( evtStore()->retrieve( aMetContainer, anInputCollName ) );
      ATH_MSG_DEBUG ( "Input missing et container = '" << anInputCollName
                      << "' retrieved from StoreGate." );

      nMet += 1;
      metObject = (*aMetContainer)[m_metName.value()];
      if ( !metObject ) {
        ATH_MSG_WARNING( "There was a problem getting the xAOD::MissingET "
                         << " object with name '" << m_metName
                         << "' from the xAOD::MissingETContainer with "
                         << "name '" << anInputCollName << "'." );
      }
    }
    else {
      if ( m_nEventsProcessed <= 10 ) {
        ATH_MSG_WARNING ( "Input link collection  = '" << anInputCollName
                          << "' could not be retrieved from StoreGate! "
                          << " This message will only be repeated 10 times..." );
      }
      else {
        ATH_MSG_DEBUG ( "Input link collection  = '" << anInputCollName
                        << "' could not be retrieved from StoreGate! " );
      }
      return StatusCode::SUCCESS;
    } // End: if/elif/else is link container
  } // End: Loop over the list of input containers

  // Make sure that we have at most one missing ET container
  if ( nMet > 1 ) {
    ATH_MSG_ERROR( "We seem to have gotten " << nMet << " xAOD::MissingETContainers "
                   << "in the InputContainerList. At most one is allowed... exiting!" );
    return StatusCode::FAILURE;
  }

  // One more sanity check that we didn't loose a container in between
  if ( m_inCollKeyList.value().size() != ( m_inputLinkContainerList.size() + nMet ) ) {
    ATH_MSG_DEBUG( "Inconsistent number of input containers after some processing!"
                   << " This should only happen when any of the input containers has zero size." );
    ATH_MSG_DEBUG( "  m_inCollKeyList.size() = " << m_inCollKeyList.value().size() );
    ATH_MSG_DEBUG( "  m_inputLinkContainerList.size() = " << m_inputLinkContainerList.size() );
    ATH_MSG_DEBUG( "  nMet = " << nMet );
    ATH_MSG_DEBUG( "  allInputContainersAreFull = " << allInputContainersAreFull );
  }


  // //-----------------------------------------
  // // Retrieve the MCEventCollection, if needed
  // //-----------------------------------------
  // if ( !m_mcEventCollKey.empty() && (
  //                                    m_mcTruthRequireSameMotherPdgID  ||
  //                                    m_mcTruthRequireSameMotherBarcode
  //                                    )
  //      )
  //   {
  //     ATH_CHECK( evtStore()->retrieve( m_mcEventColl, m_mcEventCollKey ) );
  //     ATH_MSG_DEBUG ( "MCEventCollection = '" << m_mcEventCollKey << "' retrieved from StoreGate" );
  //   } // End: need to retrieve MCEventCollection




  //-----------------------------------------
  // Create the output composite particle container and record it in StoreGate
  //-----------------------------------------
  xAOD::CompositeParticleContainer* outContainer = new xAOD::CompositeParticleContainer( SG::OWN_ELEMENTS );
  ATH_CHECK( evtStore()->record ( outContainer, m_outCollKey.value() ) );
  if ( m_writeSplitAux.value() ) {
    xAOD::AuxContainerBase* outAuxContainer = new xAOD::AuxContainerBase();
    ATH_CHECK( evtStore()->record( outAuxContainer, m_outCollKey.value() + "Aux." ) );
    outContainer->setStore( outAuxContainer );
  }
  else {
    xAOD::CompositeParticleAuxContainer* compPartAuxCont = new xAOD::CompositeParticleAuxContainer();
    ATH_CHECK( evtStore()->record( compPartAuxCont, m_outCollKey.value() + "Aux." ) );
    outContainer->setStore( compPartAuxCont );
  }
  ATH_MSG_DEBUG( "Recorded xAOD composite particles with key: " << m_outCollKey.value() );



  //-----------------------------------------
  // In case at least one of the input containers has zero size,
  //  no CompositeParticle can be found. So only try building
  //  CompositeParticles if every input container has at least
  //  one entry.
  //-----------------------------------------
  if ( allInputContainersAreFull ) {

    //-----------------------------------------
    // Do the combinations of particles
    //-----------------------------------------

    // Use the odometer algorithm to perform the combination of particles
    //  m_inputLinkContainerList is the std::vector of links

    // get the label and number of elements in each link container.
    // pass those to the OdoMeter, and ask the OdoMeter to return
    // the unique sets of indices for objects in the containers

    // To create the odometer object, we need to assign each container pointer
    // a string name that uniquely idenfies that container. The same container
    // (even used multiple times) will always have the same name. The name/entries
    // info will be used to initialize the odometer.
    ATH_MSG_DEBUG ( "Input LinkContainer list contains " << m_inputLinkContainerList.size() << " entries." );

    // Clear needed vectors and maps
    m_containerLabels.clear();
    m_containerMaxN.clear();
    m_alreadyUsedContainers.clear();

    for ( const xAOD::IParticleLinkContainer* aLinkContainer  :  m_inputLinkContainerList ) {
      const std::string& containerName = *(m_inputLinkContainerNames[ aLinkContainer ]);

      // determine if this container has already been used in the list.
      // if not, assign it a unique name. If so, get the name of the existing container
      if ( m_containerLabels.size() == 0 ||
           m_alreadyUsedContainers.find( aLinkContainer ) == m_alreadyUsedContainers.end() ) {
        m_containerLabels.push_back( containerName );
        m_containerMaxN[ containerName ] = aLinkContainer->size();
        m_alreadyUsedContainers[ aLinkContainer ] = containerName;
      }
      else {
        // already used this container. Use the old container's name
        m_containerLabels.push_back( containerName );
      }
      ATH_MSG_DEBUG ( "A container/size pair for the Odometer: "
                      << m_containerLabels.at( m_containerLabels.size() - 1) << "/"
                      << m_containerMaxN[ m_containerLabels.at( m_containerLabels.size() - 1) ] );
    }


    // Create and initialize an instance of the odometer
    OdoMeter anOdometer(m_containerLabels, m_containerMaxN);

    // find all unique groupings and build composites
    while ( anOdometer.increment() ) {
      // Clear the vector
      m_anIPartLinkList.clear();

      // Get the current combinatoric
      std::vector<int> aUniqueChoice = anOdometer.getVector();
      int nContainers = static_cast<int>(aUniqueChoice.size());

      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "Odometer - using this unique combination: ";
        for ( unsigned int ichoice = 0; ichoice < aUniqueChoice.size(); ichoice++ ) {
          msg(MSG::VERBOSE)  << aUniqueChoice.at(ichoice) << ",";
        }
        msg(MSG::VERBOSE) << endmsg;
      }

      // Loop over all containers
      for( int aContainerIndex = 0; aContainerIndex < nContainers; ++aContainerIndex ) {
        const xAOD::IParticleLinkContainer* aLinkContainer = m_inputLinkContainerList.at( aContainerIndex );
        int indexInContainer = aUniqueChoice.at( aContainerIndex );

        const xAOD::IParticleLink& aPartLink = aLinkContainer->at( indexInContainer );

        const xAOD::IParticleContainer* ptrIPartCont =
        dynamic_cast< const xAOD::IParticleContainer* >( aPartLink.getStorableObjectPointer() );

        if ( ptrIPartCont != NULL ) {
          const xAOD::IParticleLink aParticleLink(*ptrIPartCont, aPartLink.index());
          m_anIPartLinkList.push_back( aParticleLink );
        }
      } // end the loop over the available containers to build INav4MomLink objects

      // Now, actually build the CompositeParticle from the list of ElementLinks to INavigable4Momentum
      ATH_MSG_DEBUG("building composite particle");
      ATH_CHECK( buildComposite( outContainer, m_anIPartLinkList, metObject ) );
      ATH_MSG_DEBUG("done");

    } // End: while ( anOdometer.increment() )

  } // End: Found an empty input collection; thus no CompositeParticle can be possible... do nothing.


  //-----------------------------------------
  // Set the output container of composite particles as const
  // Actually, for now, don't do this as analyzers might want to decorate the
  // new xAOD::CompositeParticles further downstream.
  //-----------------------------------------
  // ATH_CHECK( evtStore()->setConst( outContainer ) );
  // ATH_CHECK( evtStore()->setConst( compPartAuxCont ) );
  // ATH_MSG_DEBUG ( "Output collection = '" << m_outCollKey.value() << "' set to const in StoreGate" );

  // Print a final message about the composite particles
  ATH_MSG_DEBUG ( "Found " << outContainer->size() << " composite particles in event number " << m_nEventsProcessed );

  // // Delete needed things
  for ( std::size_t i=0; i<m_inputLinkContainerListToDelete.size(); ++i ) {
    if ( m_inputLinkContainerListToDelete[i] ) {
      delete m_inputLinkContainerListToDelete[i];
    }
  }

  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ParticleCombinerTool::finalize()
{
  ATH_MSG_DEBUG ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );

  return StatusCode::SUCCESS;
}






//=============================================================================
// Building the composite particle
// Here, we check if any of the potential constituents (anIPartLinkList) is
// overlapping (is the same or share the same constituent) with any of the other
// potential constituents
//=============================================================================
StatusCode ParticleCombinerTool::buildComposite( xAOD::CompositeParticleContainer* outContainer,
                                                 xAOD::IParticleLinkContainer& anIPartLinkList,
                                                 const xAOD::MissingET* metObject ) const
{
  // Check if the vector does not have zero size
  if ( anIPartLinkList.size() == 0 ) {
    return StatusCode::SUCCESS;
  }

  // Create a list of all ParticleLinks that are already checked
  std::vector< const xAOD::IParticleLink* > theParticleLinks;

  bool ParticlesAreValid = true;

  ATH_MSG_VERBOSE("Now in buildComposite with outContainer size=" << outContainer->size()
                  << ", anIPartLinkList size=" << anIPartLinkList.size() << ", and a met object with address=" << metObject );
  
  //Loop over all ElementLinks to INavigable4Momenta and get the INavigable4Momenta
  for ( const xAOD::IParticleLink& aParticleLink  :  anIPartLinkList ) {
    ATH_MSG_VERBOSE("buildComposite: Looping over a particle link");
    // Check if this ElementLink is valid
    if ( aParticleLink.isValid() ) {
      // Get the particle from the ElementLink
      const xAOD::IParticle* aParticle = *aParticleLink;
      ATH_MSG_VERBOSE("buildComposite: Particle is valid");

      if (aParticle) {
        ATH_MSG_VERBOSE("buildComposite: Have a particle");
        // determine if the particle has any daughter in common with
        // a previous particle in the composite:
        for ( const xAOD::IParticleLink* otherPartLink  :  theParticleLinks ) {
          ATH_MSG_VERBOSE("Looking for shared daughters");
          // Now, make the check for having any constituent shared.
          // Note that we don't need to check if the otherPartLink is valid
          // since this happened already when we filled that vector
          if ( this->shareSameConstituents( aParticle, **otherPartLink ) ) {
            ATH_MSG_DEBUG ( "buildComposite: Found aParticle overlaps with another INavigable4Momentum in the composite!" );
            ParticlesAreValid &= false;
            break;
          }
        } // End: loop over all already checked particle links

        // push the particle onto the candidate's input list
        if ( ParticlesAreValid ) {
          ATH_MSG_VERBOSE("buildComposite: Particles are valid");
          theParticleLinks.push_back( &aParticleLink );
        }
      }
      else {
        ATH_MSG_DEBUG ( "buildComposite: Found non-valid particle at an ElementLink location!" );
        ParticlesAreValid &= false;
      } // check if aParticle exists at all
    }
    else {
      ATH_MSG_DEBUG ( "buildComposite: Found non-valid ElementLink for a particle!" );
      ParticlesAreValid &= false;
    } // check the link to a particle

    if ( false == ParticlesAreValid ) {
      ATH_MSG_VERBOSE("buildComposite: particles are not valid");
      break;
    }
  } // End: loop over vector of ElementLinks


  ATH_MSG_VERBOSE("buildComposite: Done looping over vector of ElementLinks");
  
  //-----------------------------------------
  // Do the combination
  //-----------------------------------------
  if ( ParticlesAreValid ) {
    ATH_MSG_VERBOSE("buildComposite: Particles are valid");  
    // Sort the constituents in decending pt order, if requested
    if ( m_sortConstit.value() ) {
      std::sort( anIPartLinkList.begin(), anIPartLinkList.end(),
                 [](const xAOD::IParticleLink& a, const xAOD::IParticleLink& b) {
                   return CxxUtils::fpcompare::greater( (*a)->pt(), (*b)->pt() );
                 } );
    }
    ATH_MSG_VERBOSE("buildComposite: Particles are sorted");

    // Actually create the composite particle
    //--------------------------------------------------------------
    // Try to inferr the charge from the constituents
    bool hasCharge(true);
    float chargeSum(0.0);
    xAOD::CompositeParticle* compPart = new xAOD::CompositeParticle();
    compPart->makePrivateStore();
    for ( const xAOD::IParticleLink& aParticleLink  :  anIPartLinkList ) {
      ATH_MSG_VERBOSE("buildComposite: Adding constituet");
      chargeSum += this->getCharge( aParticleLink, hasCharge );
      compPart->addPart( aParticleLink );
    }
    // Add also the missing ET object to the composite particle, if we have one
    if (metObject) {
      ATH_MSG_VERBOSE("buildComposite: Adding met object");
      compPart->setMissingET(metObject);
    }
    // Set the PDG ID for this composite particle
    compPart->setPdgId( m_pdgId.value() );
    // Set the charge (if we were able to calculate it) for this composite particle
    if ( hasCharge ) {
      ATH_MSG_VERBOSE("buildComposite: Setting the charge of the current composite particle to " << chargeSum );
      compPart->setCharge(chargeSum);
    }
    else {
      ATH_MSG_DEBUG("buildComposite: Couldn't set the charge of the composite particle");
    }

    // Check if this composite particle has been found before
    if ( !compositeParticleAlreadyFound( outContainer, compPart ) ) {
      // Add this composite particle to the output container
      // if it passes all the selections
      //--------------------------------------------------------------
//      bool passAll = true;

      // // charge selection
      // if ( passAll ) {
      //   passAll = m_filterTool->chargeFilter( compPart,
      //                                         m_chargeMin,
      //                                         m_chargeMax,
      //                                         m_allowChargeConj );
      // }

      // // MC Truth selection
      // // ONLY run this if MCTruth information is available AND
      // // MCTruth selections are configured!
      // if ( !m_mcEventCollKey.empty() && (
      //                                    m_mcTruthRequireSameMotherPdgID  ||
      //                                    m_mcTruthRequireSameMotherBarcode
      //                                    )
      //      )
      //   {
      //     if ( passAll )
      //       {
      //         passAll = mcTruthSelections( compPart );
      //       }
      //   }



      // Write out the composite particles if all cuts are passed
      // AK: Commented the conditional statements until "passAll" is actually
      //     used here. To silence a Coverity warning.
//      if ( passAll ) {
      ATH_MSG_VERBOSE("buildComposite: Adding composite particle to container");
      outContainer->push_back( compPart );
    }
    else {
      ATH_MSG_VERBOSE("buildComposite: deleting composite particle");
      delete compPart;
    }
    
    ATH_MSG_VERBOSE("buildComposite: End: if compositeParticleAlreadyFound");
    
  } // End: Saveguard check if all particles are valide

  ATH_MSG_VERBOSE("Returning from buildComposite");
  return StatusCode::SUCCESS;
}




// Try to get the charge for all types
float ParticleCombinerTool::getCharge( const xAOD::IParticleLink& aParticleLink,
                                       bool& hasCharge ) const
{
  if (!hasCharge) { return 0.0; }
  if (!aParticleLink.isValid()) {
    hasCharge = false;
    return 0.0;
  }

  // Now, start testing the different particle types
  const xAOD::CompositeParticle* compPart = dynamic_cast<const xAOD::CompositeParticle*>(*aParticleLink);
  if ( compPart ) {
    if ( !(compPart->hasCharge()) ) {
      hasCharge = false;
      return 0.0;
    }
    return compPart->charge();
  }
  const xAOD::Particle* part = dynamic_cast<const xAOD::Particle*>(*aParticleLink);
  if ( part ) {
    if ( !(part->hasCharge()) ) {
      hasCharge = false;
      return 0.0;
    }
    return part->charge();
  }
  if ( (*aParticleLink)->type() == xAOD::Type::Electron ) {
    const xAOD::Electron* ele = static_cast<const xAOD::Electron*>(*aParticleLink);
    return ele->charge();
  }
  if ( (*aParticleLink)->type() == xAOD::Type::Photon ) {
    return 0.0;
  }
  if ( (*aParticleLink)->type() == xAOD::Type::Muon ) {
    const xAOD::Muon* muon = static_cast<const xAOD::Muon*>(*aParticleLink);
    return muon->charge();
  }
  if ( (*aParticleLink)->type() == xAOD::Type::Tau ) {
    const xAOD::TauJet* tau = static_cast<const xAOD::TauJet*>(*aParticleLink);
    return tau->charge();
  }
  if ( (*aParticleLink)->type() == xAOD::Type::Jet ) {
    // jets don't have a charge (yet?)
    return 0.0;
  }
  if ( (*aParticleLink)->type() == xAOD::Type::TrackParticle ) {
    const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(*aParticleLink);
    return trk->charge();
  }
  if ( (*aParticleLink)->type() == xAOD::Type::NeutralParticle ) {
    return 0.0;
  }
  // if ( (*aParticleLink)->type() == xAOD::Type::TruthParticle ) {
  //   const xAOD::TruthParticle* tp = static_cast<const xAOD::TruthParticle*>(*aParticleLink);
  //   return static_cast<float>(tp->charge());
  // }
  if ( (*aParticleLink)->type() == xAOD::Type::ParticleFlow ) {
    const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>(*aParticleLink);
    return pfo->charge();
  }
  // If we couldn't determine the type, do this:
  hasCharge = false;
  return 0.0;
}




//=============================================================================
// Check if the composite particle at hand was already found before
//=============================================================================
bool ParticleCombinerTool::compositeParticleAlreadyFound( xAOD::CompositeParticleContainer* compContainer,
                                                          xAOD::CompositeParticle* compPart ) const
{
  // default return
  bool foundIdentical = false;

  // Count the number of constituents of the test particle
  std::size_t nConstituentsTest = compPart->nParts();

  // Get the first input container and loop over it
  for ( const xAOD::CompositeParticle* contCompPart : *compContainer ) {
    // Check if an identical composite particle is already found
    if ( !foundIdentical ) {
      // Count the number of constituents of the reference particle
      std::size_t nConstituentsReference = contCompPart->nParts();

      // Check that both have the same number of constituents
      if ( nConstituentsTest==nConstituentsReference ) {
        // Loop over all constituents of the composite particle to be tested
        bool allConstituentsSame = true;
        for ( const xAOD::IParticleLink& constitLink : compPart->partLinks() ) {
          if ( allConstituentsSame && contCompPart->contains(constitLink) ) {
            allConstituentsSame = true;
          }
          else {
            allConstituentsSame = false;
          }
        } // End: loop over constituents

        // Now, propagete the decission
        if ( allConstituentsSame ) {
          foundIdentical = true;
        }
        else {
          foundIdentical = false;
        }
      } // End: if ( nConstituentsTest==nConstituentsReference )
      else {
        foundIdentical = false;
      } // End: if/else ( nConstituentsTest==nConstituentsReference )
    } // End: if ( !foundIdentical )
  } // End: loop over container

  // Output message
  ATH_MSG_VERBOSE ( "Checking if this composite particle was already found before..."
                    << " foundIdentical=" << foundIdentical );

  return foundIdentical;
}


// The test if two particles are equal.
// One would need a barcode to do this proper
bool ParticleCombinerTool::isEqual( const xAOD::IParticle* part1,
                                    const xAOD::IParticle* part2 ) const
{
  // TODO: Add special implementation for TruthParticle, i.e., check barcode

  // Since we don't have a barcode here, we have to rely on pointer equality
  if ( part1  &&  part1 == part2 ) {
    return true;
  }
  else {
    return false;
  }
  // This would be better:
  // part1->hasSameAthenaBarCodeExceptVersion(*part2);
}



//=============================================================================
// Check that two particles are not the same or, if they are
// composite particles, that they don't share the same constitutents
//=============================================================================
bool ParticleCombinerTool::shareSameConstituents( const xAOD::IParticle* part1,
                                                    const xAOD::IParticle* part2 ) const
{
  const xAOD::CompositeParticle* compPart1 =
    dynamic_cast<const xAOD::CompositeParticle*> (part1) ;
  const xAOD::CompositeParticle* compPart2 =
    dynamic_cast<const xAOD::CompositeParticle*> (part2) ;
  ATH_MSG_VERBOSE("in shareSameConstituents('IParticle','IParticle')");

  // Neither of the two is a composite particle
  if ( !compPart1 && !compPart2 ) {
    return this->isEqual( part1, part2 );
  }
  // One of them is a composite
  else if ( compPart1 && !compPart2 ) {
    return this->shareSameConstituents( part2, compPart1 );
  }
  else if ( !compPart1 && compPart2 ) {
    return this->shareSameConstituents( part1, compPart2 );
  }
  // Both are composite candidates
  // AK: By this time only one options remains: That both of them are composite
  //     particles. So it's not necessary to do any more checks. This silences
  //     a Covery warning.
  else {
    return this->shareSameConstituents( compPart1, compPart2 );
  }
}



//=============================================================================
// Check that two particles are not the same or, if they are
// composite particles, that they don't share the same constitutents
// -------------------------- HELPER ---------------------
//=============================================================================
bool ParticleCombinerTool::shareSameConstituents( const xAOD::IParticle* part1,
                                                  const xAOD::CompositeParticle* compPart2 ) const
{
  // Default return
  bool isConstituent = false;
  ATH_MSG_VERBOSE("in shareSameConstituents('IParticle','CompositeParticle')");
  
  // Loop over all constituents of the composite particle to be tested
  const std::size_t nConstit = compPart2->nParts();
  for( std::size_t i=0; i<nConstit; ++i ) {
    const xAOD::IParticle* part2 = compPart2->part(i);

    // Check if this constituent itself is a composite particle
    const xAOD::CompositeParticle* constitCP =
      dynamic_cast<const xAOD::CompositeParticle*> (part2) ;

    if ( !constitCP ) {
      isConstituent = this->isEqual( part1, part2 );
    }
    else {
      isConstituent = shareSameConstituents( part1, constitCP );
    }
    if ( isConstituent ) {
      return true;
    }
  } // End: loop over constituents

  return isConstituent;
}



//=============================================================================
// Check that two particles are not the same or, if they are
// composite particles, that they don't share the same constitutents
// -------------------------- HELPER ---------------------
//=============================================================================
bool ParticleCombinerTool::shareSameConstituents( const xAOD::CompositeParticle* compPart1,
                                                  const xAOD::CompositeParticle* compPart2 ) const
{
  // Default return
  bool isConstituent = false;
  ATH_MSG_VERBOSE("in shareSameConstituents('CompositeParticle','CompositeParticle')");

  // Loop over all constituents of the composite particle to be tested
  const std::size_t nConstit1 = compPart1->nParts();
  const std::size_t nConstit2 = compPart2->nParts();
  for( std::size_t i=0; i<nConstit1; ++i ) {
    const xAOD::IParticle* part1 = compPart1->part(i);
    ATH_MSG_VERBOSE("looking at compositeParticle1 constituent " << i << "/" << nConstit1);
    
    // Check if this constituent itself is a composite particle
    const xAOD::CompositeParticle* constitCP1 =
      dynamic_cast<const xAOD::CompositeParticle*> (part1) ;

    for( std::size_t j=0; j<nConstit2; ++j ) {
      ATH_MSG_VERBOSE("looking at compositeParticle2 constituent " << j << "/" << nConstit2);
      const xAOD::IParticle* part2 = compPart2->part(j);

      // Check if this constituent itself is a composite particle
      const xAOD::CompositeParticle* constitCP2 =
        dynamic_cast<const xAOD::CompositeParticle*> (part2) ;

      if ( !constitCP1 && !constitCP2 ) {
        isConstituent = this->isEqual( part1, part2 );
      }
      if ( !constitCP1 && constitCP2 ) {
        isConstituent = this->shareSameConstituents( part1, constitCP2 );
      }
      if ( constitCP1 && !constitCP2 ) {
        isConstituent = this->shareSameConstituents( part2, constitCP1 );
      }
      if ( constitCP1 && constitCP2 ) {
        isConstituent = this->shareSameConstituents( constitCP1, constitCP2 );
      }
      if ( isConstituent ) {
        return true;
      }
    } // End: loop over constituents 2
  } // End: loop over constituents 1

  return isConstituent;
}



