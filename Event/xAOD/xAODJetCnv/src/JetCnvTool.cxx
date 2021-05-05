/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEvent/JetCollection.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"


#include "JetCnvTool.h"

namespace xAODMaker {


  JetCnvTool::JetCnvTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
    : AthAlgTool( type, name, parent ) {

    // Declare the interface(s) provided by the tool:
    declareInterface< IJetCnvTool >( this );
  }

  /**
   * This is the important function of the tool. It takes the muon RoI objects
   * from a LVL1_ROI container, and fills an xAOD::MuonRoIContainer with them.
   *
   * @param aod The AOD object to take the muon RoIs from
   * @param xaod The xAOD container to fill the RoIs into
   * @returns <code>StatusCode::SUCCESS</code> if all went fine,
   *          something else if not
   */
  StatusCode JetCnvTool::convert( const JetCollection* aodCont,
                                  xAOD::JetContainer* xaodCont,
                                  DataLink<xAOD::IParticleContainer> xaodConstitCont,
                                  bool constitSearch) const
  {
     

    JetCollection::const_iterator it  = aodCont->begin();
    JetCollection::const_iterator itE = aodCont->end();

    std::vector< std::string > momentKeys ;

    if( it != itE ) {
      momentKeys= (*it)->getMomentKeys();

      if( xaodConstitCont.isDefault() &&
          (*it)->begin() != (*it)->end() )
      {
        // try to retrieve its name from SG and the original jet 

        // this doesn't work : getConstituents() is protected
        // std::string key =        (*it)->navigableBase().getConstituents()[0].key();
        // StatusCode sc = evtStore()->retrieve(xaodConstitCont, key);
        // if(sc.isFailure() ) ATH_MSG_DEBUG("Could not retrieve xaod constiuents with key "<< key );

        SG::DataProxy* proxy = evtStore()->proxy( (*it)->getContainer( (*it)->begin() ) );
        if(proxy) {
          xaodConstitCont.toIdentifiedObject (proxy->name());
          if( !xaodConstitCont ) ATH_MSG_DEBUG("Could not retrieve xaod constiuents with key "<< proxy->name() );
        }
      }
    }

    
    for( ; it!= itE; ++it){
      const Jet * jet = *it;
      Jet::const_iterator cit  = jet->begin();
      Jet::const_iterator citE = jet->end();
    
      xAOD::Jet* xjet = new xAOD::Jet();
      xaodCont->push_back( xjet );
    
      xjet->setJetP4( xAOD::JetFourMom_t(jet->pt(), jet->eta(), jet->phi(), jet->m() ) );
    
      // convert constituents if we have a valid xaod constituent container.
      // If constitSearch is false, this assumes the ordering
      // in aod and xaod containers are the same !
      if(xaodConstitCont) {
        for( ; cit!= citE; ++cit){
          if (constitSearch) {
            double min_dr2 = 0.1 * 0.1;
            int iconstit = -1;
            int ip = 0;
            for (const xAOD::IParticle* p : *xaodConstitCont) {
              double deta = p->eta() - (*cit)->eta();
              double dphi = TVector2::Phi_mpi_pi (p->phi() - (*cit)->phi());
              double dr2 = deta*deta + dphi*dphi;
              if (dr2 < min_dr2) {
                min_dr2 = dr2;
                iconstit = ip;
              }
              ++ip;
            }
            if (iconstit >= 0)
              xjet->addConstituent
                (ElementLink<xAOD::IParticleContainer> (xaodConstitCont.key(), iconstit));
          }
          else {
            size_t index;
            if( jet->getIndex( cit , index) )
              xjet->addConstituent
                (ElementLink<xAOD::IParticleContainer> (xaodConstitCont.key(), index));
          }
        }    
      }

      for(size_t i=0;i<momentKeys.size(); i++){
        xjet->setAttribute<float>( momentKeys[i], jet->getMoment( momentKeys[i]) );
      }
      ATH_MSG_DEBUG( "Converted " << jet  << "  into "<< xjet << "   pt="<< xjet->pt()<< "  numConst="<< xjet->numConstituents() );    
    
      xAOD::JetScale s = xAOD::JetScale::JetConstitScaleMomentum;
      P4SignalState::State aod_s = P4SignalState::JETCONSTITUENTSCALE;
      xjet->setJetP4( s, xAOD::JetFourMom_t( jet->pt(aod_s), jet->eta(aod_s) , jet->phi(aod_s), jet->m(aod_s) ) );
      s = xAOD::JetScale::JetEMScaleMomentum;
      aod_s = P4SignalState::JETEMSCALE;
      xjet->setJetP4( s, xAOD::JetFourMom_t( jet->pt(aod_s), jet->eta(aod_s) , jet->phi(aod_s), jet->m(aod_s) ) );

      P4SignalState::State aod_constit_s = jet->constituentSignalState() ;
      xjet->setConstituentsSignalState( (xAOD::JetConstitScale) aod_constit_s );
    
      // // Track assoc if any :
      // const Analysis::TrackAssociation * trackA = jet-> getAssociation<Analysis::TrackAssociation>("TrackAssoc");
      // if (trackA) {
      //   Analysis::TrackAssociation::object_iter trk_it = trackA->begin();
      //   Analysis::TrackAssociation::object_iter trk_itE= trackA->end();
      //   size_t trk_index;
      //   for( ; trk_it != trk_itE ; ++trk_it){        
      //     if( trackA->getIndex( trk_it, trk_index) ) 
      //     if( (*it)->pt() > m_trackMinPt && (m_jetvtxtrkHelper->trackInfo(*it).selected() ) )  
      //                              preSelTracks.push_back(*it);
    
    }

    return StatusCode::SUCCESS;
  }


}

