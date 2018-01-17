/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/PDFWeight.h"

#include "xAODParticleEvent/CompositeParticle.h"

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/PDFSet.h"

#include "xAODTruth/TruthEventContainer.h"

using namespace LHAPDF;

PDFWeight::PDFWeight( const std::string& name )
  : WeightToolBase( name ),
    m_eventInfoName("EventInfo") {
  declareProperty("index", m_index = 0);
  declareProperty("PDFName", m_pdfName = "CT10");
  declareProperty("TruthEventContainerName", m_truthEventContainerName = "TruthEvents");

  m_weight = 0;
}

StatusCode PDFWeight::initialize() {
  StatusCode sc = WeightToolBase::initialize();
  if (sc.isFailure()) {
    return StatusCode::FAILURE;
  }
  try {
    LHAPDF::pathsPrepend("/cvmfs/atlas.cern.ch/repo/sw/Generators/lhapdfsets/current"); // hack!
    if(m_pdf) delete m_pdf;
    m_pdf = mkPDF(m_pdfName, m_index);
  } catch (...) {
    m_pdf = 0;
  }
  return StatusCode::SUCCESS;
}

// returns: the value that was calculated from the xAOD::IParticle composite built event
// This is the implementation of the interface
double PDFWeight::evaluate( const xAOD::IParticle* /*part*/ ) const
{
  ATH_MSG_DEBUG ("Evaluating " << name() << "...");

  // Get the proper weight (from EventInfo)
  return this->getWeight();
}

double PDFWeight::computeWeight(const xAOD::EventInfo* evtInfo) const {
  // don't do anything for data
  if(!evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    ATH_MSG_DEBUG (" Returning weight=1. for data.");
    return 1.;
  }
  if (!m_pdf)
    return 1.;

  int pdf_id1 = 0;
  int pdf_id2 = 0;
  int pdg_id1 = 0;
  int pdg_id2 = 0;
  float x1 = 0;
  float x2 = 0;
  float q = 0;
  float xf1 = 0;
  float xf2 = 0;

  const xAOD::TruthEventContainer* truthEventContainer(nullptr);
  StatusCode sc = evtStore()->retrieve(truthEventContainer, m_truthEventContainerName);
  if (sc.isFailure()) { ATH_MSG_ERROR("Could not retrieve TruthEventContainer."); return 0; }
 
  float weight = 1.0;

  for (auto truthEvent : *truthEventContainer) {

    try {
      if ( !truthEvent->pdfInfoParameter( pdg_id1, xAOD::TruthEvent::PdfParam::PDGID1 ) ) {
        ATH_MSG_DEBUG("Could not retrieve PDG id1.");
      }
      if ( !truthEvent->pdfInfoParameter( pdg_id2, xAOD::TruthEvent::PdfParam::PDGID2 ) ) {
        ATH_MSG_DEBUG("Could not retrieve PDG id2.");
      }
      if ( !truthEvent->pdfInfoParameter( pdf_id1, xAOD::TruthEvent::PdfParam::PDFID1 ) ) {
        ATH_MSG_DEBUG("Could not retrieve PDF id1.");
      }
      if ( !truthEvent->pdfInfoParameter( pdf_id2, xAOD::TruthEvent::PdfParam::PDFID2 ) ) {
        ATH_MSG_DEBUG("Could not retrieve PDF id2.");
      }
      if ( !truthEvent->pdfInfoParameter( x1, xAOD::TruthEvent::PdfParam::X1 ) ) {
        ATH_MSG_DEBUG("Could not retrieve x_1.");
      }
      if ( !truthEvent->pdfInfoParameter( x2, xAOD::TruthEvent::PdfParam::X2 ) ) {
        ATH_MSG_DEBUG("Could not retrieve x_2.");
      }
      if ( !truthEvent->pdfInfoParameter( q, xAOD::TruthEvent::PdfParam::Q ) ) {
        ATH_MSG_DEBUG("Could not retrieve Q.");
      }
      if ( !truthEvent->pdfInfoParameter( xf1, xAOD::TruthEvent::PdfParam::XF1 ) ) {
        ATH_MSG_DEBUG("Could not retrieve x_f1.");
      }
      if ( !truthEvent->pdfInfoParameter( xf2, xAOD::TruthEvent::PdfParam::XF2 ) ) {
         ATH_MSG_DEBUG("Could not retrieve x_f2.");
      }
    } catch (...) {
      // set this to debug only because this is a frequent problem in ttbar samples
      // sending the message in every event will only flood the logs until a newer version is available
      ATH_MSG_DEBUG("Could not retrieve PDF information. xAODs generated with a version earlier than AtlasProduction-19.2.3.7 might have a bug that causes this. Please check your input xAOD. All PDF weights will be 1.");
      return 1.0;
    }

    if (xf1 == 0 || xf2 == 0) {
      ATH_MSG_DEBUG("Skipping event, as this TruthEvent is not reliable.");
      // this can happen for some PDFs which cross zero and go negative at NLO
      // not sure this is the best to do, so leaving the weight == 0 for now
      weight = 0;
      continue;
    }

    float reweighted_xf1 = 0;
    float reweighted_xf2 = 0;
    try {
      reweighted_xf1 = m_pdf->xfxQ(pdg_id1, x1, q);
      reweighted_xf2 = m_pdf->xfxQ(pdg_id2, x2, q);
    } catch (...) {
      ATH_MSG_DEBUG("LHAPDF exception.");
    }
    weight = reweighted_xf1*reweighted_xf2/(xf1*xf2);

    break;
  }

  return weight;
}

double PDFWeight::getWeight() const {
 //Retrieveing eventInfo
  const xAOD::EventInfo* evtInfo;
  StatusCode sc = evtStore()->retrieve( evtInfo, m_eventInfoName );
  if(sc.isFailure() || !evtInfo) {
    ATH_MSG_ERROR (" EventInfo could not be retrieved !!");
    return 0.;
  }

  double weight = this->computeWeight(evtInfo);
  ATH_MSG_DEBUG ("     " << name() << " returning weight= " << weight << ".");
  return weight;
}

