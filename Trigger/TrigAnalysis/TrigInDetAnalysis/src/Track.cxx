// emacs: this is -*- c++ -*- 
/** @file Track.cxx */

#include "TrigInDetAnalysis/Track.h"

ClassImp(TIDA::Track)

TIDA::Track::Track()  :
  m_eta(0),  m_phi(0),  m_z0(0),  m_a0(0),  m_pT(0), m_chi2(0),
  m_dof(0),
  m_deta(0), m_dphi(0), m_dz0(0), m_da0(0), m_dpT(0),
  m_bLayerHits(0), m_pixelHits(0), m_sctHits(0),  m_siHits(0),
  m_strawHits(0), m_trHits(0), 
  m_hitPattern(0),
  m_multiPattern(0),
  m_author(0),
  m_hasTruth(0),  
  m_barcode(0),
  m_match_barcode(0), 
  m_expectBL(0),
  m_id(0)
{}
 
  
TIDA::Track::Track(double  eta, double  phi, double  z0, double  a0, double  pT, double chi2, double dof,
				double deta, double dphi, double dz0, double da0, double dpT, 
				int bLayerHits, int pixelHits, int sctHits, int siHits,
				int strawHits,  int trHits, 
				unsigned hitPattern,
				unsigned multiPattern,
				int author,
				bool hasTruth,
				int barcode,
				int match_barcode,
				bool expectBL, 
				unsigned long id) :
    m_eta(eta),   m_phi(phi),   m_z0(z0),   m_a0(a0),   m_pT(pT), m_chi2(chi2), 
    m_dof(dof),
    m_deta(deta), m_dphi(dphi), m_dz0(dz0), m_da0(da0), m_dpT(dpT),
    m_bLayerHits(bLayerHits), m_pixelHits(pixelHits), m_sctHits(sctHits),  m_siHits(siHits),
    m_strawHits(strawHits), m_trHits(trHits), 
    m_hitPattern(hitPattern),
    m_multiPattern(multiPattern),
    m_author(author),
    m_hasTruth(hasTruth),
    m_barcode(barcode),
    m_match_barcode(match_barcode),
    m_expectBL(expectBL),
    m_id(id)
{} 


TIDA::Track::~Track() { } 

