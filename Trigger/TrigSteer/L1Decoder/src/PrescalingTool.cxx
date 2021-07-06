/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "PrescalingTool.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "xAODEventInfo/EventInfo.h"

const std::function< CLHEP::HepRandomEngine*(void) > PSTRanluxFactory = [](void)->CLHEP::HepRandomEngine*{
   return new CLHEP::Ranlux64Engine();
};

PrescalingTool::PrescalingTool( const std::string& type, 
                                const std::string& name, 
                                const IInterface* parent ) :
   base_class(type, name, parent),
   m_RNGEngines( PSTRanluxFactory, SG::getNSlots() )
{}

PrescalingTool::~PrescalingTool()
{}

StatusCode
PrescalingTool::initialize()
{
   ATH_CHECK(m_hltPrescaleSetInputKey.initialize( ! m_hltPrescaleSetInputKey.key().empty() ));
   ATH_CHECK( m_HLTMenuKey.initialize() );
   ATH_CHECK( m_eventInfoKey.initialize() );

   if ( !m_monTool.empty() ) ATH_CHECK(m_monTool.retrieve());

   m_prescaleForUnknownChain = { m_keepUnknownChains.value(), 1 };
   m_costChainID = HLT::Identifier("HLT_noalg_CostMonDS_L1All");
   return StatusCode::SUCCESS;
}


StatusCode PrescalingTool::start() {
   SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
   ATH_CHECK( hltMenuHandle.isValid() );

   std::map<std::string, std::set<std::string>> l1SeedsCheckForCPS;
   for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
      HLT::Identifier chainID{ chain.name() };
      int isCPS = 0;
      for ( const auto& group: chain.groups() ) {
         if ( group.find("CPS") != std::string::npos) {
            m_CPSGroups[group].push_back( chainID );
            l1SeedsCheckForCPS[group].insert(chain.l1item());
            isCPS++;
         }
      }
      if ( isCPS ==0 )
         m_nonCPSChains.insert( chainID );
      if ( isCPS > 1 ) { 
         ATH_MSG_ERROR("Chain " << chainID << " belongs to more than one CPS groups");
         return StatusCode::FAILURE;
      }

   }
   for ( auto [group, chains]: m_CPSGroups ) {
      if ( chains.size() == 1 ) {
         ATH_MSG_ERROR("Only one chain " << chains.front() << " in CPS group " << group << " that makes no sense");
         return StatusCode::FAILURE;
      }
   }

   for ( auto [group, l1SeedsSet]: l1SeedsCheckForCPS) {
      if ( l1SeedsSet.size() != 1 ) {
         ATH_MSG_ERROR("Chains in CPS group " << group << " have several different L1 seeds " << std::vector<std::string>(l1SeedsSet.begin(), l1SeedsSet.end()));
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode PrescalingTool::prescaleChains( const EventContext& ctx,  
                                           const HLT::IDVec& initiallyActive,
                                           HLT::IDVec& remainActive ) const
{
   if ( initiallyActive.empty() )
      return StatusCode::SUCCESS;

   // clear the output just in case
   remainActive.clear();

   if ( m_hltPrescaleSetInputKey.key().empty() ) {
      // if no prescaling key is configured, treat all prescales according to propery KeepUnknownChains
      if( m_keepUnknownChains ) {
         remainActive.reserve( initiallyActive.size() );
         for( const auto & ch : initiallyActive ) {
            remainActive.push_back(ch);
         }
      }
      return StatusCode::SUCCESS;
   }

   SG::ReadCondHandle<TrigConf::HLTPrescalesSet> hltpssRH(m_hltPrescaleSetInputKey, ctx);
   const TrigConf::HLTPrescalesSet* hltPrescaleSet{*hltpssRH};
   if(hltPrescaleSet == nullptr) {
      ATH_MSG_ERROR("Failed to retrieve HLTPrescalesSet " << hltPrescaleSet->name());
      return StatusCode::FAILURE;
   }

   // access to psk
   ATH_MSG_DEBUG("Using HLT PSK " << hltPrescaleSet->psk());
   auto mon_lb = Monitored::Scalar<int>("LB", [&](){ return ctx.eventID().lumi_block(); });
   auto mon_psk = Monitored::Scalar<std::string>("HLTPSK", [&](){ return std::to_string(hltPrescaleSet->psk()); });
   auto mon = Monitored::Group(m_monTool, mon_lb, mon_psk);

   // prepare the result
   remainActive.reserve( initiallyActive.size() );

   // create the seed from the event time
   /**
       Note: the event time needs to be taken from the EventInfo instead EventContext.eventID, which is commonly done!
       This is due to the special case when the trigger is run in a partition with preloaded data and the parameter @c
       HLTEventLoopMgr.forceStartOfRunTime is set >0. In that case the @c EventContext.EventID is forced to the be the 
       SOR time for each event. Using the @c EventContext.eventID would lead to a constant seed and a scewed prescaling.
    */
   auto eventInfoHandle = SG::makeHandle( m_eventInfoKey, ctx );
   CHECK( eventInfoHandle.isValid() );
   size_t seed = eventInfoHandle->timeStamp() ^ eventInfoHandle->timeStampNSOffset();


   CLHEP::HepRandomEngine* engine = m_RNGEngines.getEngine( ctx );
   engine->setSeed( seed, 0 );

   auto getPrescale = [&](const HLT::Identifier& ch) -> const TrigConf::HLTPrescalesSet::HLTPrescale& {
      try {
         return hltPrescaleSet->prescale( ch.numeric() );
      } catch(const std::out_of_range & ex) {
         // if chain with that name is not found in the prescale set
         ATH_MSG_DEBUG("No prescale value for chain " << ch << ", will " << (m_prescaleForUnknownChain.enabled ? "" : "not ") << "keep it.");
      }
      return m_prescaleForUnknownChain;
   };

   auto decisionPerChain = [&](const HLT::Identifier& ch, double prescaleValue ) -> bool {
      auto flat = engine->flat();
      if(ch == m_costChainID) { // this is to explicitly monitor the cost chain
         auto mon_rndm = Monitored::Scalar<double>("Random", flat);
         Monitored::Group(m_monTool, mon_rndm);
      }
      return flat < 1./ prescaleValue;
   };

   struct ChainAndPrescale {
      HLT::Identifier id;
      TrigConf::HLTPrescalesSet::HLTPrescale ps;
      double relativePrescale{};
   };
   
   ChainSet activeChainSet{ initiallyActive.begin(), initiallyActive.end() };

   for ( auto [groupName, chainIDs]: m_CPSGroups) {
      if ( std::find(initiallyActive.begin(), initiallyActive.end(), chainIDs.front()) != initiallyActive.end() ) { // this group is seeded
         std::vector<ChainAndPrescale> psValueSorted;
         for ( const HLT::Identifier& ch: chainIDs ) {
            psValueSorted.emplace_back( ChainAndPrescale({ch, getPrescale(ch)}) );
         }

         std::sort(psValueSorted.begin(), psValueSorted.end(), [](const ChainAndPrescale& a, const ChainAndPrescale& b){ 
            if ( a.ps.enabled  and b.ps.enabled ) return  a.ps.prescale < b.ps.prescale;
            else if ( !a.ps.enabled  and b.ps.enabled ) return false;
            else if ( a.ps.enabled  and !b.ps.enabled ) return true;
            else /*( !a.prescale.enabled  and !b.prescale.enabled )*/ return a.ps.prescale < b.ps.prescale; // irrelevant but sorting needs consistent ordering
            }
         );
         // setup relative prescales
         psValueSorted.front().relativePrescale = psValueSorted.front().ps.prescale; // the first chain (with the lowest PS is relative w.r.t the all events)
         for ( auto i = psValueSorted.begin()+1; i < psValueSorted.end(); ++i ) {
            i->relativePrescale = i->ps.prescale / (i-1)->ps.prescale ;

         }
         ATH_MSG_DEBUG("Chains in CPS group '"<< groupName <<"' sorted by PS : ");
         for ( const ChainAndPrescale& ch: psValueSorted )
            ATH_MSG_DEBUG("  "<< ch.id <<" " << (ch.ps.enabled  ? " prescale relative to the above " + std::to_string(ch.relativePrescale) : "disabled" ) );
         // do actual prescaling
         for ( const ChainAndPrescale& ch: psValueSorted ) {
            if ( not ch.ps.enabled ) break;
            const bool decision = decisionPerChain(ch.id, ch.relativePrescale);
            if ( not decision ) break;
            remainActive.push_back( ch.id );
         }
      }
   }

   // go through all active chains that are not in CPS groups
   for ( const HLT::Identifier& ch: m_nonCPSChains ) {
      if ( std::find( initiallyActive.begin(), initiallyActive.end(), ch ) != initiallyActive.end() ) {
         auto prescale = getPrescale(ch);
         if ( prescale.enabled ) {
            const bool decision = decisionPerChain(ch, prescale.prescale);
            if ( decision )
               remainActive.push_back( ch );
            ATH_MSG_DEBUG("Prescaling decision for chain " << ch << " " << decision);
         } else {
            ATH_MSG_DEBUG("Chain " << ch << " is disabled, won't keep" );
         }
      } 
   }

   return StatusCode::SUCCESS;
}


