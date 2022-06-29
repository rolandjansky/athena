// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTMatrixIO.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date May 29th, 2020
 */

#include "HTTMatrixIO.h"

///////////////////////////////////////////////////////////////////////////////
// Reader
///////////////////////////////////////////////////////////////////////////////

HTTMatrixReader::HTTMatrixReader(TTree *tree, size_t nLayers, size_t nCoords) :
  m_tree(tree),
  m_entry(0),
  m_nEntries(tree->GetEntries()),
  m_modules(nLayers),
  m_acc(nLayers, nCoords),
  m_pQoP(&m_bins_QoP),
  m_pphi(&m_bins_phi),
  m_pd0(&m_bins_d0),
  m_pz0(&m_bins_z0),
  m_peta(&m_bins_eta)
{
  tree->SetBranchAddress("sectorID", m_acc.FTK_modules.data());
  tree->SetBranchAddress("hashID", m_modules.data());

  tree->SetBranchAddress("tmpC", &m_acc.pars.qOverPt);
  tree->SetBranchAddress("tmpD", &m_acc.pars.d0);
  tree->SetBranchAddress("tmpPhi", &m_acc.pars.phi);
  tree->SetBranchAddress("tmpCoto", &m_acc.pars.eta);
  tree->SetBranchAddress("tmpZ", &m_acc.pars.z0);

  tree->SetBranchAddress("Vec", m_acc.hit_coords.data());
  tree->SetBranchAddress("VecG", m_acc.hit_coordsG.data());
  tree->SetBranchAddress("tmpxC", m_acc.hit_x_QoP.data());
  tree->SetBranchAddress("tmpxGC", m_acc.hit_xG_HIP.data());
  tree->SetBranchAddress("tmpxD", m_acc.hit_x_d0.data());
  tree->SetBranchAddress("tmpxPhi", m_acc.hit_x_phi.data());
  tree->SetBranchAddress("tmpxCoto", m_acc.hit_x_eta.data());
  tree->SetBranchAddress("tmpxGCoto", m_acc.hit_xG_eta.data());
  tree->SetBranchAddress("tmpxZ", m_acc.hit_x_z0.data());
  tree->SetBranchAddress("tmpcovx", m_acc.covariance.data());
  tree->SetBranchAddress("tmpcovxG", m_acc.covarianceG.data());

  tree->SetBranchAddress("tmpintc", &m_pQoP);
  tree->SetBranchAddress("tmpintphi", &m_pphi);
  tree->SetBranchAddress("tmpintd0", &m_pd0);
  tree->SetBranchAddress("tmpintz0", &m_pz0);
  tree->SetBranchAddress("tmpinteta", &m_peta);
}


void HTTMatrixReader::readEntry(size_t entry)
{
  m_tree->GetEntry(entry);

  m_acc.track_bins.clear();
  for (size_t j = 0; j < m_bins_QoP.size(); j++)
    {
      HTTTrackParsI bins;
      bins.qOverPt = m_bins_QoP[j];
      bins.phi = m_bins_phi[j];
      bins.d0 = m_bins_d0[j];
      bins.z0 = m_bins_z0[j];
      bins.eta = m_bins_eta[j];
      m_acc.track_bins.push_back(bins);
    }
}

bool HTTMatrixReader::nextEntry()
{
  if (m_entry >= m_nEntries) return false;

  readEntry(m_entry);
  m_entry++;
  return true;
}


/**
 * Reads a matrix tree, accumulating its entries into a map.
 * Note this functions sets and resets branch addresses.
 *
 * @param map - map to be added to
 * @param tree - tree to be read
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
void readTree(AccumulateMap & map, TTree *tree, size_t nLayers, size_t nCoords)
{
  HTTMatrixReader reader(tree, nLayers, nCoords);
  while (reader.nextEntry())
    accumulate(map, reader.getModules(), reader.getAccumulator());
}


/**
 * Reads a matrix tree, accumulating its entries into a vector, in the order they
 * are read. Note this functions sets and resets branch addresses.
 *
 * @param tree - tree to be read
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
std::vector<std::pair<std::vector<module_t>, HTTMatrixAccumulator>> readTree(TTree *tree, size_t nLayers, size_t nCoords)
{
  std::vector<std::pair<std::vector<module_t>, HTTMatrixAccumulator>> matrices;

  HTTMatrixReader reader(tree, nLayers, nCoords);
  while (reader.nextEntry())
    matrices.push_back({ reader.getModules(), reader.getAccumulator() });

  return matrices;
}


///////////////////////////////////////////////////////////////////////////////
// Writer
///////////////////////////////////////////////////////////////////////////////


HTTMatrixWriter::HTTMatrixWriter(TTree *tree, int nLayers, int nCoords) :
  m_tree(tree),
  m_nEntries(0),
  m_nLayers(nLayers),
  m_nCoords(nCoords),
  m_nCoords2(nCoords * nCoords),
  m_pQoP(&m_bins_QoP),
  m_pphi(&m_bins_phi),
  m_pd0(&m_bins_d0),
  m_pz0(&m_bins_z0),
  m_peta(&m_bins_eta)
{
  // Dummy variables for typing (the branch address will be set for each sector)
  int anInt;
  double aDouble;

  m_tree->Branch("ndim",      &m_nCoords,  "ndim/I");
  m_tree->Branch("ndim2",     &m_nCoords2, "ndim2/I");
  m_tree->Branch("nplanes",   &m_nLayers,  "nplanes/I");

  m_tree->Branch("sectorID",  &anInt, "sectorID[nplanes]/I");
  m_tree->Branch("hashID",    &anInt, "hashID[nplanes]/I");

  m_tree->Branch("tmpC",      &aDouble, "tmpC/D");
  m_tree->Branch("tmpD",      &aDouble, "tmpD/D");
  m_tree->Branch("tmpPhi",    &aDouble, "tmpPhi/D");
  m_tree->Branch("tmpCoto",   &aDouble, "tmpCoto/D");
  m_tree->Branch("tmpZ",      &aDouble, "tmpZ/D");

  m_tree->Branch("Vec",       &aDouble, "Vec[ndim]/D");
  m_tree->Branch("VecG",      &aDouble, "VecG[ndim]/D");
  m_tree->Branch("tmpxC",     &aDouble, "tmpxC[ndim]/D");
  m_tree->Branch("tmpxGC",    &aDouble, "tmpxGC[ndim]/D");
  m_tree->Branch("tmpxD",     &aDouble, "tmpxD[ndim]/D");
  m_tree->Branch("tmpxPhi",   &aDouble, "tmpxPhi[ndim]/D");
  m_tree->Branch("tmpxCoto",  &aDouble, "tmpxCoto[ndim]/D");
  m_tree->Branch("tmpxGCoto", &aDouble, "tmpxGCoto[ndim]/D");
  m_tree->Branch("tmpxZ",     &aDouble, "tmpxZ[ndim]/D");
  m_tree->Branch("tmpcovx",   &aDouble, "tmpcovx[ndim2]/D");
  m_tree->Branch("tmpcovxG",  &aDouble, "tmpcovxG[ndim2]/D");

  m_tree->Branch("nhit",      &m_coverage, "nhit/F");
  m_tree->Branch("tmpintc",   &m_pQoP);
  m_tree->Branch("tmpintphi", &m_pphi);
  m_tree->Branch("tmpintd0",  &m_pd0);
  m_tree->Branch("tmpintz0",  &m_pz0);
  m_tree->Branch("tmpinteta", &m_peta);
}

size_t HTTMatrixWriter::fill(std::vector<module_t> modules, HTTMatrixAccumulator & acc)
{

  m_tree->SetBranchAddress("sectorID", acc.FTK_modules.data());
  m_tree->SetBranchAddress("hashID", modules.data());

  m_tree->SetBranchAddress("tmpC", &acc.pars.qOverPt);
  m_tree->SetBranchAddress("tmpD", &acc.pars.d0);
  m_tree->SetBranchAddress("tmpPhi", &acc.pars.phi);
  m_tree->SetBranchAddress("tmpCoto", &acc.pars.eta);
  m_tree->SetBranchAddress("tmpZ", &acc.pars.z0);

  m_tree->SetBranchAddress("Vec", acc.hit_coords.data());
  m_tree->SetBranchAddress("VecG", acc.hit_coordsG.data());
  m_tree->SetBranchAddress("tmpxC", acc.hit_x_QoP.data());
  m_tree->SetBranchAddress("tmpxGC", acc.hit_xG_HIP.data());
  m_tree->SetBranchAddress("tmpxD", acc.hit_x_d0.data());
  m_tree->SetBranchAddress("tmpxPhi", acc.hit_x_phi.data());
  m_tree->SetBranchAddress("tmpxCoto", acc.hit_x_eta.data());
  m_tree->SetBranchAddress("tmpxGCoto", acc.hit_xG_eta.data());
  m_tree->SetBranchAddress("tmpxZ", acc.hit_x_z0.data());
  m_tree->SetBranchAddress("tmpcovx", acc.covariance.data());
  m_tree->SetBranchAddress("tmpcovxG", acc.covarianceG.data());

  m_bins_QoP.clear();
  m_bins_phi.clear();
  m_bins_z0.clear();
  m_bins_d0.clear();
  m_bins_eta.clear();
  for (HTTTrackParsI const & pars : acc.track_bins)
    {
      m_bins_QoP.push_back(pars.qOverPt);
      m_bins_phi.push_back(pars.phi);
      m_bins_d0.push_back(pars.d0);
      m_bins_z0.push_back(pars.z0);
      m_bins_eta.push_back(pars.eta);
    }
  m_coverage = static_cast<float>(acc.track_bins.size());
  m_tree->Fill();
  return m_nEntries++;
}


/**
 * Writes the contents of an AccumulateMap into the supplied tree (one entry per sector).
 *
 * @param map - map to be written
 * @param tree - tree to be written to; this function will create the branches
 * @param nLayers - number of logical layers
 * @param nCoords - number of hit coordinates in a full (13/13) track
 */
void fillTree(AccumulateMap & map, TTree *tree, int nLayers, int nCoords)
{
  fillTree(map.begin(), map.end(), tree, nLayers, nCoords);
}
