/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "KeyWriterTool.h"

#include <limits>

namespace TrigConf {

   KeyWriterTool::KeyWriterTool( const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent ) :
      base_class(type, name, parent)
   {}

   KeyWriterTool::~KeyWriterTool()
   {}

   StatusCode KeyWriterTool::initialize() {
      ATH_CHECK( m_l1PrescaleSetInputKey.initialize( m_includeL1PrescaleKey ) );
      ATH_CHECK( m_hltPrescaleSetInputKey.initialize() );
      ATH_CHECK( m_hltMenuInputKey.initialize() );

      ATH_CHECK( m_writeKeys.initialize( SG::AllowEmpty ) );
      return StatusCode::SUCCESS;
   }


   StatusCode KeyWriterTool::writeKeys(const EventContext& ctx) const {
      uint32_t a, b, c; // Unused
      return writeKeys(ctx, a, b, c);
   }


   StatusCode KeyWriterTool::writeKeys(const EventContext& ctx, uint32_t&smk, uint32_t& l1psk, uint32_t& hltpsk) const {

      if (m_includeL1PrescaleKey) {
         SG::ReadCondHandle<TrigConf::L1PrescalesSet> l1psRH(m_l1PrescaleSetInputKey, ctx);
         ATH_CHECK( l1psRH.isValid() );
         const TrigConf::L1PrescalesSet* l1PrescaleSet{*l1psRH};
         l1psk = l1PrescaleSet->psk();
      } else {
         l1psk = 0;
      }

      SG::ReadCondHandle<TrigConf::HLTPrescalesSet> hltpsRH(m_hltPrescaleSetInputKey, ctx);
      ATH_CHECK( hltpsRH.isValid() );
      const TrigConf::HLTPrescalesSet* hltPrescaleSet{*hltpsRH};
      hltpsk = hltPrescaleSet->psk();

      SG::ReadHandle<TrigConf::HLTMenu> hltRH(m_hltMenuInputKey, ctx);
      ATH_CHECK( hltRH.isValid() );
      smk = hltRH->smk();

      if (!m_writeKeys.empty()) {
         SG::WriteHandle<xAOD::TrigConfKeys> writeKeys(m_writeKeys, ctx);
         ATH_CHECK( writeKeys.record( std::make_unique<xAOD::TrigConfKeys>(smk, l1psk, hltpsk) ) );
      }

      return StatusCode::SUCCESS;
   }

}
