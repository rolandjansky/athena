// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTMatrixReductionAlgo.cxx
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 21st, 2020
 * @brief Algorithm to reduce matrix files from 2nd stage to 1st stage.
 */

#include "HTTMatrixReductionAlgo.h"
#include "HTTMatrixIO.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

#include "TFile.h"
#include "TTree.h"

#include <sstream>
#include <cassert>


///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////

HTTMatrixReductionAlgo::HTTMatrixReductionAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode HTTMatrixReductionAlgo::initialize()
{
  ATH_CHECK(m_HTTMapping.retrieve());
  ATH_CHECK(m_tHistSvc.retrieve());

  m_pmap_1st = m_HTTMapping->PlaneMap_1st();
  m_pmap_2nd = m_HTTMapping->PlaneMap_2nd();

  // Setup the boundaries for the merge
  if (m_allregion)
    {
      m_region_start = 0;
      m_region_end = m_nRegions;
    }
  else
    {
      m_region_start = m_region;
      m_region_end = m_region + 1;
    }
  m_sector_cum.resize(m_nRegions);

  ATH_MSG_INFO("Reading " << m_filePath);
  TFile *file = TFile::Open(m_filePath.value().c_str(), "");
  ATH_CHECK(copySliceTree(file));

  // Main execution function
  extract_1stStage(file);

  return StatusCode::SUCCESS;
}


StatusCode HTTMatrixReductionAlgo::copySliceTree(TFile *file)
{
  // Is there a better way to do this? Idk how the interface to tHistSvc works
  HTTTrackPars max;
  HTTTrackPars min;
  HTTTrackParsI nBins;

  // Read old tree
  TTree *old_tree = (TTree*)file->Get("slice");
  old_tree->SetBranchAddress("c_max", &max.qOverPt);
  old_tree->SetBranchAddress("c_min", &min.qOverPt);
  old_tree->SetBranchAddress("c_slices", &nBins.qOverPt);

  old_tree->SetBranchAddress("phi_max", &max.phi);
  old_tree->SetBranchAddress("phi_min", &min.phi);
  old_tree->SetBranchAddress("phi_slices", &nBins.phi);

  old_tree->SetBranchAddress("d0_max", &max.d0);
  old_tree->SetBranchAddress("d0_min", &min.d0);
  old_tree->SetBranchAddress("d0_slices", &nBins.d0);

  old_tree->SetBranchAddress("z0_max", &max.z0);
  old_tree->SetBranchAddress("z0_min", &min.z0);
  old_tree->SetBranchAddress("z0_slices", &nBins.z0);

  old_tree->SetBranchAddress("eta_max", &max.eta);
  old_tree->SetBranchAddress("eta_min", &min.eta);
  old_tree->SetBranchAddress("eta_slices", &nBins.eta);

  old_tree->GetEntry(0);

  // Write new tree
  TTree *new_tree = new TTree("slice", "Slice boundaries");
  ATH_CHECK(m_tHistSvc->regTree("/TRIGHTTMATRIX1STSTAGEOUT/slice", new_tree));

  new_tree->Branch("c_max", &max.qOverPt);
  new_tree->Branch("c_min", &min.qOverPt);
  new_tree->Branch("c_slices", &nBins.qOverPt);

  new_tree->Branch("phi_max", &max.phi);
  new_tree->Branch("phi_min", &min.phi);
  new_tree->Branch("phi_slices", &nBins.phi);

  new_tree->Branch("d0_max", &max.d0);
  new_tree->Branch("d0_min", &min.d0);
  new_tree->Branch("d0_slices", &nBins.d0);

  new_tree->Branch("z0_max", &max.z0);
  new_tree->Branch("z0_min", &min.z0);
  new_tree->Branch("z0_slices", &nBins.z0);

  new_tree->Branch("eta_max", &max.eta);
  new_tree->Branch("eta_min", &min.eta);
  new_tree->Branch("eta_slices", &nBins.eta);

  new_tree->Fill();

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////


/*
 * Matches first stage layers to second stage layers.
 *
 * @return { layer_map, coord_map }
 *      - layer_map[1st stage layer] = 2nd stage layer
 *      - coord_map[1st stage coord] = 2nd stage coord (index)
 *
 * TODO this should be handled by the pmap?
 */
std::pair<std::vector<size_t>, std::vector<size_t>> HTTMatrixReductionAlgo::matchStages()
{
  size_t iCoord_1st = 0;

  std::vector<size_t> layers_1st_to_2nd(m_pmap_1st->getNLogiLayers());
  std::vector<size_t> coords_1st_to_2nd(m_pmap_1st->getNCoords());

  for (unsigned layer_1st = 0; layer_1st < m_pmap_1st->getNLogiLayers(); layer_1st++)
    {
      LayerInfo const & info_1st = m_pmap_1st->getLayerInfo(layer_1st, 0);
      // all sections in stage 1 should also be in stage 2, so can just use 0 to retrieve info
      LayerSection const & ls_2nd = m_pmap_2nd->getLayerSection(info_1st);

      int inversion(0); /// ultimately we may want a function to get inversion for each specific layer, aka getInversion(ls_2nd.layer), so this is a placeholder;
      layers_1st_to_2nd[layer_1st] = ls_2nd.layer + inversion;
      for (size_t iDim = 0; iDim < m_pmap_1st->getDim(layer_1st); iDim++)
        {
	  coords_1st_to_2nd[iCoord_1st] = m_pmap_2nd->getCoordOffset(ls_2nd.layer) + iDim + inversion;
	  iCoord_1st++;
        }
    }

  assert(iCoord_1st == m_pmap_1st->getNCoords());
  return { layers_1st_to_2nd, coords_1st_to_2nd };
}


/*
 * Reduces a second stage sector and corresponding matrix to first stage.
 *
 * This simply copies the sector matrices from the input file,
 * but only keeping 1st stage layers/coordinates. In python pseudocode,
 *          layer_filter = [ is1stStage(layer_2nd) for layer_2nd in range(# 2nd stage layers) ]
 *          modules_1st = modules_2nd[layer_filter]
 * and similarly for the coordinate-length members.
 *
 * @param modules_2nd - Second stage sector definition (read from file)
 * @param acc_2nd - Second stage accumulator (read from file)
 * @param layers_1st_to_2nd - Layer mapping from s1 to s2
 * @param coords_1st_to_2nd - Coordinate (index) mapping from s1 to s2
 *
 * @return { modules_1st, acc_1st}
 *      - modules_1st - First stage sector definition
 *      - acc_1st - First stage matrix accumulator
 */
std::pair<std::vector<module_t>, HTTMatrixAccumulator> HTTMatrixReductionAlgo::reduce(
										      std::vector<module_t> const & modules_2nd, HTTMatrixAccumulator const & acc_2nd,
										      std::vector<size_t> const & layers_1st_to_2nd, std::vector<size_t> const & coords_1st_to_2nd)
{
  std::vector<module_t> modules_1st(m_pmap_1st->getNLogiLayers()); // First stage sector definition (to be filled)
  HTTMatrixAccumulator acc_1st(m_pmap_1st->getNLogiLayers(), m_pmap_1st->getNCoords()); // First stage accumulator (to be filled)

  acc_1st.pars = acc_2nd.pars;
  acc_1st.track_bins = acc_2nd.track_bins;

  // For each first stage layer, copy relavant info from matching second stage layer
  for (size_t layer_1st = 0; layer_1st < m_pmap_1st->getNLogiLayers(); layer_1st++)
    {
      size_t layer_2nd = layers_1st_to_2nd[layer_1st];
      modules_1st[layer_1st] = modules_2nd[layer_2nd];
      acc_1st.FTK_modules[layer_1st] = acc_2nd.FTK_modules[layer_2nd];
    }

  // Copy coordinates from matching second stage coordinate
  for (size_t iCoord_1st = 0; iCoord_1st < m_pmap_1st->getNCoords(); iCoord_1st++)
    {
      size_t iCoord_2nd = coords_1st_to_2nd[iCoord_1st];

      acc_1st.hit_coords[iCoord_1st] = acc_2nd.hit_coords[iCoord_2nd];
      acc_1st.hit_x_QoP [iCoord_1st] = acc_2nd.hit_x_QoP [iCoord_2nd];
      acc_1st.hit_x_d0  [iCoord_1st] = acc_2nd.hit_x_d0  [iCoord_2nd];
      acc_1st.hit_x_z0  [iCoord_1st] = acc_2nd.hit_x_z0  [iCoord_2nd];
      acc_1st.hit_x_eta [iCoord_1st] = acc_2nd.hit_x_eta [iCoord_2nd];
      acc_1st.hit_x_phi [iCoord_1st] = acc_2nd.hit_x_phi [iCoord_2nd];

      // Covariance calculation. This requires a second loop through layers/coordinates,
      // so postfix those variables with _j, as in (i,j). Only need triangular matrix
      // so start loop at iCoord_1st.
      for (size_t iCoord_1st_j = iCoord_1st; iCoord_1st_j < m_pmap_1st->getNCoords(); iCoord_1st_j++)
        {
	  size_t iCoord_2nd_j = coords_1st_to_2nd[iCoord_1st_j];
	  acc_1st.covariance[iCoord_1st * m_pmap_1st->getNCoords() + iCoord_1st_j] =
            acc_2nd.covariance[iCoord_2nd * m_pmap_2nd->getNCoords() + iCoord_2nd_j];
        }
    }

  return { modules_1st, acc_1st };
}


/*
 * Runs the main algorithm, accumulating the 1st stage matrices from file into m_sector_cum.
 */
void HTTMatrixReductionAlgo::extract_1stStage(TFile *file)
{
  auto matched_layers_coords = matchStages();
  // Read 2nd stage info from file
  TTree *tree_2nd = (TTree*)file->Get(Form("am0"));
  HTTMatrixReader reader(tree_2nd, m_pmap_2nd->getNLogiLayers(), m_pmap_2nd->getNCoords());
      
  // Create 1st stage matrix and modules, and accumulate into m_sector_cum
  while (reader.nextEntry())
    {
      auto sector_acc = reduce(reader.getModules(), reader.getAccumulator(), matched_layers_coords.first, matched_layers_coords.second);
      ::accumulate(m_sector_cum[0], sector_acc.first, sector_acc.second);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTMatrixReductionAlgo::execute()
{
  // Do nothing; this class does not process events. The main algorithm is
  // called in initialize() and finalize().
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTMatrixReductionAlgo::finalize()
{
  {
    // Create the tree
    std::stringstream name;
    std::stringstream title;
    name << "am0";
    title << "Ambank 0" << " parameters";
    TTree* tree = new TTree(name.str().c_str(), title.str().c_str());
    ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTMATRIX1STSTAGEOUT/%s",tree->GetName()), tree));

    // Fill the tree
    ::fillTree(m_sector_cum[0], tree, m_pmap_1st->getNLogiLayers(), m_pmap_1st->getNCoords());
  }

  ATH_CHECK(m_tHistSvc->finalize());
  return StatusCode::SUCCESS;
}


