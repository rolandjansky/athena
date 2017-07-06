/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCorrections/PDFScaleFactorCalculator.h"

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"


#include "xAODTruth/TruthEventContainer.h"
#include "xAODEventInfo/EventInfo.h"

namespace top {

  PDFScaleFactorCalculator::PDFScaleFactorCalculator( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr){

    declareProperty( "config" , m_config );

  }

  StatusCode PDFScaleFactorCalculator::initialize()
  {

    ATH_MSG_INFO(" top::PDFScaleFactorCalculator initialize" );

    // This prints all the available PDFsets we can use
    //printAvailablePDFs();

    for( const std::string& set_name : m_config->LHAPDFSets() ) {
      m_pdf_sets[ set_name ] = PDFSet( set_name );
    }

    m_base_pdf_name = m_config->baseLHAPDF();
    if (!m_base_pdf_name.empty()) {
      ATH_MSG_INFO("Enabling " << m_base_pdf_name << " to recompute PDF weights.\n   Content of sumWeight and totalSumWeight trees might be inconsistent with this recalculation.\n   Use only PDFSumWeight and for PDF uncertainty estimates.\n");
      m_basepdf = LHAPDF::mkPDF(m_base_pdf_name, 0);
    }

    return StatusCode::SUCCESS;

  }

  StatusCode PDFScaleFactorCalculator::execute()
  {

    // Get the event info for the MC weight
    const xAOD::EventInfo* event_info(nullptr);
    top::check(evtStore()->retrieve(event_info, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    const xAOD::TruthEventContainer* truthEventContainer(nullptr);
    top::check( evtStore()->retrieve(truthEventContainer, m_config->sgKeyTruthEvent()) , "Failed to retrieve truth PDF info" );

//     float mc_weight = event_info->mcEventWeight();
    float mc_weight = truthEventContainer->at(0)->weights()[0];// FIXME temporary bugfix

    // try this...
    top::check( ( truthEventContainer->size() == 1 ), "More than one truth event, not sure how to cope with PDF info" );
    int PDFID1(0), PDFID2(0), PDGID1(0), PDGID2(0);
    float X1(0), X2(0) ,Q(0), XF1(0), XF2(0);

    for (auto truthEvent : *truthEventContainer){

      top::check( truthEvent->pdfInfoParameter( PDGID1, xAOD::TruthEvent::PdfParam::PDGID1 ), "Failed to get PDFInfo: PDGID1" );
      top::check( truthEvent->pdfInfoParameter( PDGID2, xAOD::TruthEvent::PdfParam::PDGID2 ), "Failed to get PDFInfo: PDGID2" );
      top::check( truthEvent->pdfInfoParameter( PDFID1, xAOD::TruthEvent::PdfParam::PDFID1 ), "Failed to get PDFInfo: PDFID1" );
      top::check( truthEvent->pdfInfoParameter( PDFID2, xAOD::TruthEvent::PdfParam::PDFID2 ), "Failed to get PDFInfo: PDFID2" );
      top::check( truthEvent->pdfInfoParameter( X1, xAOD::TruthEvent::PdfParam::X1 ), "Failed to get PDFInfo: X1" );
      top::check( truthEvent->pdfInfoParameter( X2, xAOD::TruthEvent::PdfParam::X2 ), "Failed to get PDFInfo: X2" );
      top::check( truthEvent->pdfInfoParameter( Q, xAOD::TruthEvent::PdfParam::Q ), "Failed to get PDFInfo: Q" );
      top::check( truthEvent->pdfInfoParameter( XF1, xAOD::TruthEvent::PdfParam::XF1 ), "Failed to get PDFInfo: XF1" );
      top::check( truthEvent->pdfInfoParameter( XF2, xAOD::TruthEvent::PdfParam::XF2 ), "Failed to get PDFInfo: XF2" );

      // This isn't working ( nor the helper to get the PDF Info )
      //top::check( pdfInfo.valid(), "PDFInfo not valid" );

      if( XF1*XF2==0 ) {
	if( !m_base_pdf_name.empty()) {
	  XF1 = m_basepdf->xfxQ( PDGID1, X1, Q );
	  XF2 = m_basepdf->xfxQ( PDGID2, X2, Q );
	} else {
	  ATH_MSG_FATAL("Not enough info to recompute PDF weights (empty XF1,XF2).\n Please try to set LHAPDFBaseSet to a valid PDF set.\n XF1=" << XF1 << " XF2=" << XF2 << " LHAPDFBaseSet=" << m_base_pdf_name);
	  return StatusCode::FAILURE;
	}
      }

      for( auto& pdf : m_pdf_sets ){

	// Being cautious...
	pdf.second.event_weights.clear();
	pdf.second.event_weights.resize(pdf.second.pdf_members.size());

	int i = 0;

	for( const auto& pdf_member : pdf.second.pdf_members ){

	  float new_xf1 = pdf_member->xfxQ( PDGID1, X1, Q );
	  float new_xf2 = pdf_member->xfxQ( PDGID2, X2, Q );

	  float weight = (new_xf1*new_xf2)/(XF1*XF2);

	  // This is the reweighting each event
	  pdf.second.event_weights[i] = weight;
	  // This is the sum of all event weights for a final scaling
	  pdf.second.sum_of_event_weights[i] += (weight*mc_weight);
	  i++;
	}

	// decorate each truth event with a vector of PDF weights
	if( m_config->saveLHAPDFEvent() )
	  truthEvent->auxdecor< std::vector< float > >( "AnalysisTop_"+pdf.first+"_Weights" ) = pdf.second.event_weights;

      }

    }

    return StatusCode::SUCCESS;

  }

  StatusCode PDFScaleFactorCalculator::finalize()
  {

    for( auto& pdf : m_pdf_sets )
      m_config->addLHAPDFResult( pdf.first, pdf.second.sum_of_event_weights );

    if (m_basepdf) delete m_basepdf;

    return StatusCode::SUCCESS;

  }

}
