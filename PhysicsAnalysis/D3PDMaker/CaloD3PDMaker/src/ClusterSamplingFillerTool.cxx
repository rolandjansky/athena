/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterSamplingFillerTool.cxx
 * @author maarten boonekamp snyder <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for samplings from a CaloCluster.
 */


#include "ClusterSamplingFillerTool.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>
#include <algorithm>


namespace {


double vget (const std::vector<double>& v, size_t i)
{
  if (i < v.size())
    return v[i];
  return 0;
}


} // anonymous namespace


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ClusterSamplingFillerTool::ClusterSamplingFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("Samplings", m_samplings,
                   "Samplings to write.  Empty means to write all.");
  declareProperty ("EmHadEnergies",    m_writeEmHadEnergies = true,
                   "Should total E/Had energies be written?");
  declareProperty ("SamplingEnergies", m_writeSamplingEnergies = false,
                   "Should per-sampling energies be written?");
  declareProperty ("SamplingEtaPhi",   m_writeSamplingEtaPhi = false,
                   "Should per-sampling eta/phi be written?");
  declareProperty ("SamplingEtaPhiRaw",   m_writeSamplingEtaPhiRaw = false,
                   "Should per-sampling raw eta/phi be written?");
  declareProperty ("SamplingEtamax",   m_writeSamplingEtamax = false,
                   "Should per-sampling maximum eta be written?");
  declareProperty ("WriteRecoStatus",m_writeRecoStatus=false,
                   "Should reconstruction status word be written?");

  m_Eem = 0;
  m_Ehad = 0;
  m_RecoStatus = 0;
  std::fill (m_Es,       m_Es + NSAMP,       (float*)0);
  std::fill (m_etas,     m_etas + NSAMP,     (float*)0);
  std::fill (m_phis,     m_phis + NSAMP,     (float*)0);
  std::fill (m_raw_etas, m_raw_etas + NSAMP, (float*)0);
  std::fill (m_raw_phis, m_raw_phis + NSAMP, (float*)0);
  std::fill (m_etamax,   m_etamax + NSAMP,   (float*)0);
}

StatusCode ClusterSamplingFillerTool::initialize()
{
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());
  return  StatusCode::SUCCESS;
}

/**
 * @brief Book variables for this block.
 */
StatusCode ClusterSamplingFillerTool::book()
{
  if( m_writeEmHadEnergies ) {
    CHECK( addVariable ("E_em", m_Eem) );
    CHECK( addVariable ("E_had", m_Ehad) );
  }

  std::string samplingnames[NSAMP] = 
    {"PreSamplerB", "EMB1", "EMB2", "EMB3",
     "PreSamplerE", "EME1", "EME2", "EME3",
     "HEC0", "HEC1", "HEC2", "HEC3",
     "TileBar0", "TileBar1", "TileBar2",
     "TileGap1", "TileGap2", "TileGap3",
     "TileExt0", "TileExt1", "TileExt2",
     "FCAL0", "FCAL1", "FCAL2"};

  if (m_samplings.empty()) {
    for (unsigned int i = 0; i < NSAMP; i++)
      m_samplings.push_back (i);
  }

  if (m_samplings.size() > NSAMP) {
    REPORT_MESSAGE(MSG::ERROR) << "Too many samplings requested: "
                               << m_samplings.size()
                               << "; max is " << /*NSAMP*/24;
    return StatusCode::FAILURE;
  }
  
  for (unsigned int i = 0; i < m_samplings.size(); i++) {
    unsigned int s = m_samplings[i];
    if (s >= NSAMP) {
      REPORT_MESSAGE(MSG::ERROR) << "Requested sampling index "
                                 << s << " which is out of range.";
      return StatusCode::FAILURE;
    }

    const std::string& sname = samplingnames[s];

    if( m_writeSamplingEnergies )
      CHECK( addVariable ("E_" + sname,   m_Es[i],
                          "Energy in sampling " + sname) );

    if( m_writeSamplingEtaPhi ) {
      CHECK( addVariable ("eta_" + sname, m_etas[i],
                          "Aligned eta barycenter in sampling " + sname) );
      CHECK( addVariable ("phi_" + sname, m_phis[i],
                          "Aligned phi barycenter in sampling " + sname) );
    }

    if( m_writeSamplingEtaPhiRaw ) {
      CHECK( addVariable ("raw_eta_" + sname, m_raw_etas[i],
                          "Raw eta barycenter in sampling " + sname,
                          -999) );
      CHECK( addVariable ("raw_phi_" + sname, m_raw_phis[i],
                          "Raw phi barycenter in sampling " + sname,
                          -999) );
    }

    if( m_writeSamplingEtamax ) {
      CHECK( addVariable ("etamax_" + sname, m_etamax[i],
                          "Eta of maximum cell in sampling " + sname) );
    }
  }
  
  if(m_writeRecoStatus)
      CHECK(addVariable("RecoStatus",m_RecoStatus));
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode ClusterSamplingFillerTool::fill (const CaloCluster& p)
{
  std::vector<double> eSamp;
  p.getEInSamples(eSamp);

  std::vector<double> etaSamp;
  p.getEtaInSamples(etaSamp);

  std::vector<double> phiSamp;
  p.getPhiInSamples(phiSamp);

  CHECK( fillEtamax(p) );
  CHECK( fillSamplings (eSamp, etaSamp, phiSamp) );

  if(m_writeRecoStatus)
    *m_RecoStatus= p.getRecoStatus().getStatusWord();

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode ClusterSamplingFillerTool::fill (const xAOD::CaloCluster& p)
{

  const size_t nsamp=m_samplings.size();
  std::vector<double> eSamp (nsamp);
  std::vector<double> etaSamp (nsamp);
  std::vector<double> phiSamp (nsamp);

  for (unsigned int i=0; i < nsamp; i++) {
    eSamp[i] = p.eSample (static_cast<CaloCell_ID::CaloSample>(m_samplings[i]));
    etaSamp[i] = p.etaSample (static_cast<CaloCell_ID::CaloSample>(m_samplings[i]));
    phiSamp[i] = p.phiSample (static_cast<CaloCell_ID::CaloSample>(m_samplings[i]));
  }

  CHECK( fillEtamax(p) );
  CHECK( fillSamplings (eSamp, etaSamp, phiSamp) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill etamax/reco status.
 * @param p The input object.
 */
template <class T>
StatusCode ClusterSamplingFillerTool::fillEtamax (const T& p)
{
  if( m_writeSamplingEtamax ) {
    for(unsigned int i=0; i<m_samplings.size(); i++)
      *m_etamax[i] =
        p.etamax (static_cast<CaloSampling::CaloSample>(m_samplings[i]));
  }
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill sampling variables.
 * @param eSamp Energy per sample.
 * @param etaSamp Eta per sample.
 * @param phiSamp Phi per sample.
 */
StatusCode
ClusterSamplingFillerTool::fillSamplings (const std::vector<double>& eSamp,
                                          const std::vector<double>& etaSamp,
                                          const std::vector<double>& phiSamp)
{
  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle { m_caloDetDescrMgrKey };
  if( m_writeEmHadEnergies && eSamp.size() >= NSAMP) {
    *m_Eem = 
      vget (eSamp, CaloSampling::PreSamplerB) +
      vget (eSamp, CaloSampling::EMB1) +
      vget (eSamp, CaloSampling::EMB2) +
      vget (eSamp, CaloSampling::EMB3) +
      vget (eSamp, CaloSampling::PreSamplerE) +
      vget (eSamp, CaloSampling::EME1) +
      vget (eSamp, CaloSampling::EME2) +
      vget (eSamp, CaloSampling::EME3) +
      vget (eSamp, CaloSampling::FCAL0);
    *m_Ehad =
      vget (eSamp, CaloSampling::HEC0) + 
      vget (eSamp, CaloSampling::HEC1) + 
      vget (eSamp, CaloSampling::HEC2) + 
      vget (eSamp, CaloSampling::HEC3) + 
      vget (eSamp, CaloSampling::TileBar0) + 
      vget (eSamp, CaloSampling::TileBar1) + 
      vget (eSamp, CaloSampling::TileBar2) + 
      vget (eSamp, CaloSampling::TileGap1) + 
      vget (eSamp, CaloSampling::TileGap2) + 
      vget (eSamp, CaloSampling::TileGap3) + 
      vget (eSamp, CaloSampling::TileExt0) + 
      vget (eSamp, CaloSampling::TileExt1) + 
      vget (eSamp, CaloSampling::TileExt2) + 
      vget (eSamp, CaloSampling::FCAL1) + 
      vget (eSamp, CaloSampling::FCAL2);
  }

  if( m_writeSamplingEnergies ) {
    for(unsigned int i=0; i<m_samplings.size(); i++)
      *m_Es[i] = vget (eSamp, m_samplings[i]);
  }


  for(unsigned int i=0; i<m_samplings.size(); i++) {
    if (m_writeSamplingEtaPhi) {
      *m_etas[i] = vget (etaSamp, m_samplings[i]);
      *m_phis[i] = vget (phiSamp, m_samplings[i]);
    }
    if (m_writeSamplingEtaPhiRaw) {
      double eta_raw=0, phi_raw=0;
      if (CaloCellDetPos::getDetPosition(
            **caloDetDescrMgrHandle,
            static_cast<CaloCell_ID::CaloSample>(m_samplings[i]),
            vget(etaSamp, m_samplings[i]),
            vget(phiSamp, m_samplings[i]),
            eta_raw,
            phi_raw)) {
        *m_raw_etas[i] = eta_raw;
        *m_raw_phis[i] = phi_raw;
      }
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
