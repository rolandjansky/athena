// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTConstGenAlgo.cxx
 * @author Unknown; major rewrite Riley Xu - riley.xu@cern.ch
 * @date May 28th, 2020
 * @brief Algorithm to generate fit constants.
 */


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTObjects/HTTConstants.h"

#include "HTTConstGenAlgo.h"
#include "HTTMatrixIO.h"

#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cassert>

#include <TVectorD.h>
#include <TDecompLU.h>

#define MIN_TRACK_SEC 15 // min number of tracks per sector

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////

HTTConstGenAlgo::HTTConstGenAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}


StatusCode HTTConstGenAlgo::initialize()
{
    ATH_MSG_DEBUG("initialize()");
    ATH_MSG_DEBUG("Are we going to dump missing hist constants? " << m_dumpMissingHitsConstants);
    ATH_CHECK(m_HTTMapping.retrieve());

    m_pmap = (m_isSecondStage ? m_HTTMapping->PlaneMap_2nd() : m_HTTMapping->PlaneMap_1st() );

    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_tHistSvc.retrieve());
    if (m_Monitor) ATH_CHECK(bookHistograms());

    // Input file
    ATH_MSG_DEBUG("reading " << m_cfpath.value());
    m_mafile = TFile::Open(m_cfpath.value().c_str());
    gROOT->cd();

    // Copy the slice tree
    ATH_CHECK(copySliceTree(m_mafile));

    // Read sizing
    m_matrix_tree = (TTree*)m_mafile->Get(("am" + std::to_string(0)).c_str());
    m_matrix_tree->SetBranchAddress("ndim", &m_nCoords);
    m_matrix_tree->SetBranchAddress("nplanes", &m_nLayers);
    m_matrix_tree->GetEntry(0);
    m_nKAverages = m_nCoords - HTTTrackPars::NPARS;
    m_nKernel = m_nKAverages * m_nCoords;
    m_nCoords_2 = m_nCoords * m_nCoords;

    if (static_cast<size_t>(m_nLayers) == m_HTTMapping->PlaneMap_2nd()->getNLogiLayers())
        m_pmap = m_HTTMapping->PlaneMap_2nd();
    else if (static_cast<size_t>(m_nLayers) == m_HTTMapping->PlaneMap_1st()->getNLogiLayers())
        m_pmap = m_HTTMapping->PlaneMap_1st();
    else
        ATH_MSG_ERROR("nLayers " << m_nLayers << " doesn't match any pmap");

    if (m_dumpMissingHitsConstants) m_geo_consts_with_missinghit.resize(m_nLayers);

    // Read skip list
    if (!m_skipFile.empty()) readSkipList(m_matrix_tree->GetEntries());

    // Prepare output contstants tree
    ATH_CHECK(prepareOutputTree());

    // Create the good tree
    std::string tree_name = "am" + std::to_string(0);
    std::string tree_title = "Ambank " + std::to_string(0) + " matrices";
    m_good_tree = new TTree(tree_name.c_str(), tree_title.c_str());
    ATH_CHECK(m_tHistSvc->regTree(("/TRIGHTTTREEGOODOUT/"+tree_name).c_str(), m_good_tree));

    // Run the constant generation
    generate_constants();

    return StatusCode::SUCCESS;
}


StatusCode HTTConstGenAlgo::bookHistograms()
{
    m_h_vc = new TH1F("h_vc","h_vc",500,-1e-4,1e-4);
    m_h_vd = new TH1F("h_vd","h_vd",500,-1e-2,1e-2);
    m_h_vf = new TH1F("h_vf","h_vf",500,-1e-2,1e-2);
    m_h_vz = new TH1F("h_vz","h_vz",500,-1e-2,1e-2);
    m_h_veta = new TH1F("h_veta","h_veta",500,-1e-2,1e-2);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTTREEGOODOUT/h_vc",m_h_vc));
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTTREEGOODOUT/h_vd",m_h_vd));
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTTREEGOODOUT/h_vf",m_h_vf));
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTTREEGOODOUT/h_vz",m_h_vz));
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTTREEGOODOUT/h_veta",m_h_veta));

    return StatusCode::SUCCESS;
}


StatusCode HTTConstGenAlgo::copySliceTree(TFile *file)
{
    // Read old tree
    TTree *old_tree = (TTree*)file->Get("slice");
    old_tree->SetBranchAddress("c_max", &m_sliceMax.qOverPt);
    old_tree->SetBranchAddress("c_min", &m_sliceMin.qOverPt);
    old_tree->SetBranchAddress("c_slices", &m_sliceNBins.qOverPt);

    old_tree->SetBranchAddress("phi_max", &m_sliceMax.phi);
    old_tree->SetBranchAddress("phi_min", &m_sliceMin.phi);
    old_tree->SetBranchAddress("phi_slices", &m_sliceNBins.phi);

    old_tree->SetBranchAddress("d0_max", &m_sliceMax.d0);
    old_tree->SetBranchAddress("d0_min", &m_sliceMin.d0);
    old_tree->SetBranchAddress("d0_slices", &m_sliceNBins.d0);

    old_tree->SetBranchAddress("z0_max", &m_sliceMax.z0);
    old_tree->SetBranchAddress("z0_min", &m_sliceMin.z0);
    old_tree->SetBranchAddress("z0_slices", &m_sliceNBins.z0);

    old_tree->SetBranchAddress("eta_max", &m_sliceMax.eta);
    old_tree->SetBranchAddress("eta_min", &m_sliceMin.eta);
    old_tree->SetBranchAddress("eta_slices", &m_sliceNBins.eta);

    old_tree->GetEntry(0);

    // Write new tree
    TTree *new_tree = new TTree("slice", "Slice boundaries");
    ATH_CHECK(m_tHistSvc->regTree("/TRIGHTTTREEGOODOUT/slice", new_tree));

    new_tree->Branch("c_max", &m_sliceMax.qOverPt);
    new_tree->Branch("c_min", &m_sliceMin.qOverPt);
    new_tree->Branch("c_slices", &m_sliceNBins.qOverPt);

    new_tree->Branch("phi_max", &m_sliceMax.phi);
    new_tree->Branch("phi_min", &m_sliceMin.phi);
    new_tree->Branch("phi_slices", &m_sliceNBins.phi);

    new_tree->Branch("d0_max", &m_sliceMax.d0);
    new_tree->Branch("d0_min", &m_sliceMin.d0);
    new_tree->Branch("d0_slices", &m_sliceNBins.d0);

    new_tree->Branch("z0_max", &m_sliceMax.z0);
    new_tree->Branch("z0_min", &m_sliceMin.z0);
    new_tree->Branch("z0_slices", &m_sliceNBins.z0);

    new_tree->Branch("eta_max", &m_sliceMax.eta);
    new_tree->Branch("eta_min", &m_sliceMin.eta);
    new_tree->Branch("eta_slices", &m_sliceNBins.eta);

    new_tree->Fill();

    return StatusCode::SUCCESS;
}


StatusCode HTTConstGenAlgo::prepareOutputTree()
{
    // Dummy variables for typing
    int anInt;
    float aFloat;
    double aDouble;

    std::string tree_name = "am" + std::to_string(0);
    std::string tree_title = "Ambank " + std::to_string(0) + " constants";
    m_ctree = new TTree(tree_name.c_str(), tree_title.c_str());

    m_ctree->Branch("ndim",       &m_nCoords,    "ndim/I");
    m_ctree->Branch("ndim2",      &m_nCoords_2,  "ndim2/I");
    m_ctree->Branch("nkernel",    &m_nKernel,    "nkernel/I");
    m_ctree->Branch("nkaverages", &m_nKAverages, "nkaverages/I");
    m_ctree->Branch("nplane",     &m_nLayers,    "nplane/I");

    m_ctree->Branch("sectorID", &anInt, "sectorID[nplane]/I");
    m_ctree->Branch("hashID",   &anInt, "hashID[nplane]/I");
    m_ctree->Branch("nhit",     &aFloat, "nhit/F");

    m_ctree->Branch("Cd",  &aDouble, "Cd/D");
    m_ctree->Branch("Cc",  &aDouble, "Cc/D");
    m_ctree->Branch("Cf",  &aDouble, "Cf/D");
    m_ctree->Branch("Cz0", &aDouble, "Cz0/D");
    m_ctree->Branch("Co",  &aDouble, "Co/D");

    m_ctree->Branch("Vc",  &aDouble, "Vc[ndim]/D");
    m_ctree->Branch("Vd",  &aDouble, "Vd[ndim]/D");
    m_ctree->Branch("Vf",  &aDouble, "Vf[ndim]/D");
    m_ctree->Branch("Vz0", &aDouble, "Vz0[ndim]/D");
    m_ctree->Branch("Vo",  &aDouble, "Vo[ndim]/D");

    m_ctree->Branch("kernel",    &aDouble, "kernel[nkernel]/D");
    m_ctree->Branch("kaverages", &aDouble, "kaverages[nkaverages]/D");

    ATH_CHECK(m_tHistSvc->regTree(("/TRIGHTTCTREEOUT/"+tree_name).c_str(), m_ctree));
    return StatusCode::SUCCESS;
}


void HTTConstGenAlgo::readSkipList(size_t nSectors)
{
    m_skipList.resize(nSectors);

    std::ifstream file(m_skipFile);
    if (!file.is_open()) return;

    size_t count = 0;
    size_t sector;
    while (file >> sector)
    {
        if (sector >= nSectors)
        {
            ATH_MSG_ERROR("Bad sector " << sector << "/" << nSectors);
            return;
        }
        m_skipList[sector] = true;
        count++;
    }
    ATH_MSG_INFO("Skipping " << count << " sectors out of " << nSectors);
    file.close();
}



// Main function. Generates constants into m_geo_consts, and copies the good matrices into m_good_tree.
void HTTConstGenAlgo::generate_constants()
{
    HTTMatrixReader reader(m_matrix_tree, m_nLayers, m_nCoords);
    HTTMatrixWriter writer(m_good_tree, m_nLayers, m_nCoords);
    for (size_t entry = 0; entry < (size_t)m_matrix_tree->GetEntries(); entry++)
    {
        // Read the entry
        if (!m_skipList.empty() && m_skipList[entry]) continue;
        reader.readEntry(entry);
        std::vector<module_t> & modules = reader.getModules();
        HTTMatrixAccumulator & acc = reader.getAccumulator();
	
        // Check for sufficient statistics
        if (acc.track_bins.size() < MIN_TRACK_SEC)
        {
	    ATH_MSG_DEBUG("Insufficient tracks in sector " << reader.getEntry());
            continue;
        }

        // Scale and normalize the accumulated variables
        HTTMatrixAccumulator acc_norm = normalize(acc);

        // Create the constants
        geo_constants geo(m_nCoords);
        bool success = GetConstants(acc_norm, geo, entry);
        if (!success) continue;

        // Fill the constant tree and good matrix tree
        fillConstTree(modules, acc, geo);
        writer.fill(modules, acc);
        m_geo_consts.push_back(geo);

        // If needed, we generate the same as above but dropping/ignoring each potential hit
        if (m_dumpMissingHitsConstants) createMissingHitsConstants(acc_norm, entry);
    }
}

void HTTConstGenAlgo::createMissingHitsConstants(HTTMatrixAccumulator const & acc_norm, size_t entry)
{
    for (int ip = 0; ip < m_nLayers; ip++)
    {
        int missing = m_pmap->getCoordOffset(ip); // this is the coordinate we are missing
        if (!acc_norm.coords_usable[missing]) // just skip ahead if we aren't using this coordinate already
        {
            geo_constants emptyGeo(m_nCoords);
            m_geo_consts_with_missinghit[ip].push_back(emptyGeo);
            continue;
        }

        unsigned int nusable = acc_norm.nusable() - 1;
        std::vector<bool> coordsToUse = acc_norm.coords_usable;
        coordsToUse[missing] = false;

        if (m_pmap->getDim(ip) == 2)
        {
            coordsToUse[missing+1] = false;
            nusable--;
        }


        geo_constants geo(m_nCoords);
        bool success = GetConstants(acc_norm, geo, entry, coordsToUse, nusable);
        if (!success)
        {
            // push this back to keep the order correct (ie same numbering as nominal constants)
            geo_constants emptyGeo(m_nCoords);
            m_geo_consts_with_missinghit[ip].push_back(emptyGeo);
        }
        else
        {
            m_geo_consts_with_missinghit[ip].push_back(geo);
        }
    }
}

bool HTTConstGenAlgo::GetConstants(HTTMatrixAccumulator const &acc_norm, geo_constants &geo, int entryNumber)
{
    return GetConstants(acc_norm, geo, entryNumber, acc_norm.coords_usable, acc_norm.nusable());
}

bool HTTConstGenAlgo::GetConstants(HTTMatrixAccumulator const &acc_norm, geo_constants &geo, int entryNumber, std::vector<bool> const &coordsToUse, unsigned int nusable)
{
    // Get the reduced matrix and invert it
    TMatrixD mtx_reduced = getReducedMatrix(m_nCoords, acc_norm.covariance, coordsToUse, nusable);
    if (isSingular(mtx_reduced))
    {
        ATH_MSG_DEBUG("Singular matrix in sector " << entryNumber);
        return false;
    }
    std::vector<double> inv_covariance = invert(m_nCoords, mtx_reduced, coordsToUse);

    // Calculate the eigen system
    std::vector<double> eigvals;
    vector2D<double> eigvecs;
    eigen(nusable, m_nCoords, mtx_reduced, coordsToUse, eigvals, eigvecs);

    // Calculate the constants
    geo = makeConsts(acc_norm, coordsToUse, inv_covariance, eigvals, eigvecs);

    if (failedConstants(geo, coordsToUse))
    {
        ATH_MSG_DEBUG("Failed constants in sector " << entryNumber);
        return false;
    }

    return true;
}

void HTTConstGenAlgo::fillConstTree(std::vector<module_t> & modules, HTTMatrixAccumulator & acc, geo_constants & geo)
{
    float coverage = static_cast<float>(acc.track_bins.size());
    m_ctree->SetBranchAddress("sectorID", acc.FTK_modules.data());
    m_ctree->SetBranchAddress("hashID",   modules.data());
    m_ctree->SetBranchAddress("nhit",     &coverage);

    m_ctree->SetBranchAddress("Cc",  &geo.pars.qOverPt);
    m_ctree->SetBranchAddress("Cd",  &geo.pars.d0);
    m_ctree->SetBranchAddress("Cf",  &geo.pars.phi);
    m_ctree->SetBranchAddress("Cz0", &geo.pars.z0);
    m_ctree->SetBranchAddress("Co",  &geo.pars.eta);

    m_ctree->SetBranchAddress("Vc",  geo.Vcurvature.data());
    m_ctree->SetBranchAddress("Vd",  geo.Vd0.data());
    m_ctree->SetBranchAddress("Vf",  geo.Vphi.data());
    m_ctree->SetBranchAddress("Vz0", geo.Vz0.data());
    m_ctree->SetBranchAddress("Vo",  geo.Veta.data());

    m_ctree->SetBranchAddress("kaverages", geo.kaverages.data());
    m_ctree->SetBranchAddress("kernel",    geo.kernel.data());

    m_ctree->Fill();

    if (m_Monitor)
    {
        for (int i = 0; i < m_nCoords; i++)
        {
            m_h_vc->Fill(geo.Vcurvature[i]);
            m_h_vd->Fill(geo.Vd0[i]);
            m_h_vf->Fill(geo.Vphi[i]);
            m_h_vz->Fill(geo.Vz0[i]);
            m_h_veta->Fill(geo.Veta[i]);
        }
    }
}


bool HTTConstGenAlgo::isNAN(double value, const char* name)
{
    if (TMath::IsNaN(value))
    {
        ATH_MSG_WARNING("NaN found in " << name);
        return true;
    }
    return false;
}

#define CHECK_NAN(var) (isNAN((var), #var))

// Check if constants are bad (eg, contain "nan")
bool HTTConstGenAlgo::failedConstants(geo_constants const & geo, std::vector<bool> const & usable)
{
  geo_constants gco = calculate_gcorth(geo, m_nCoords, usable);

    if (CHECK_NAN(gco.pars.qOverPt)) return true;
    if (CHECK_NAN(geo.pars.qOverPt)) return true;
    if (CHECK_NAN(gco.pars.d0)) return true;
    if (CHECK_NAN(geo.pars.d0)) return true;
    if (CHECK_NAN(gco.pars.phi)) return true;
    if (CHECK_NAN(geo.pars.phi)) return true;
    if (CHECK_NAN(gco.pars.z0)) return true;
    if (CHECK_NAN(geo.pars.z0)) return true;
    if (CHECK_NAN(gco.pars.eta)) return true;
    if (CHECK_NAN(geo.pars.eta)) return true;

    for (int i = 0; i < m_nCoords; i++)
    {
        if (CHECK_NAN(gco.Vcurvature[i])) return true;
        if (CHECK_NAN(geo.Vcurvature[i])) return true;
        if (CHECK_NAN(gco.Vd0[i])) return true;
        if (CHECK_NAN(geo.Vd0[i])) return true;
        if (CHECK_NAN(gco.Vphi[i])) return true;
        if (CHECK_NAN(geo.Vphi[i])) return true;
        if (CHECK_NAN(gco.Vz0[i])) return true;
        if (CHECK_NAN(geo.Vz0[i])) return true;
        if (CHECK_NAN(gco.Veta[i])) return true;
        if (CHECK_NAN(geo.Veta[i])) return true;
    }

    for (int i = 0; i < m_nCoords - HTTTrackPars::NPARS; i++)
    {
        if (CHECK_NAN(geo.kaverages[i])) return true;
        for (int j = 0; j < m_nCoords; j++)
            if (CHECK_NAN(geo.kernel(i,j))) return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Arithmetic Functions
///////////////////////////////////////////////////////////////////////////////

/*
 * This function normalizes the accumulated variables in acc_raw into the actual
 * averages needed for constant generation. The pars and hit_coords are averaged
 * using the coverage. The covariance field, which currently stores sum(x_i x_j),
 * is converted into the actual covariance matrix. The hix_x_par fields are
 * converted from sum(x_k p_i) to <x_k p_i> - <x_k> <p_i>.
 */
HTTMatrixAccumulator HTTConstGenAlgo::normalize(HTTMatrixAccumulator const & acc_raw)
{
    HTTMatrixAccumulator acc(acc_raw);
    double coverage = static_cast<double>(acc.track_bins.size());
    size_t nCoords = acc.hit_coords.size();

    for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
        acc.pars[i] /= coverage;

    for (unsigned i = 0; i < nCoords; i++)
    {
        acc.hit_coords[i] /= coverage;
        if (std::abs(acc.hit_coords[i]) < MTX_TOLERANCE) acc.coords_usable[i] = false;
        else acc.coords_usable[i] = true;
    }

    // Divide by n-1 for the sample variance (Bessel's correction)
    for (unsigned i = 0; i < nCoords; i++)
    {
        acc.hit_x_QoP[i] = (acc.hit_x_QoP[i] - acc.hit_coords[i] * acc.pars.qOverPt   * coverage) / (coverage-1);
        acc.hit_x_d0[i]  = (acc.hit_x_d0[i]  - acc.hit_coords[i] * acc.pars.d0        * coverage) / (coverage-1);
        acc.hit_x_z0[i]  = (acc.hit_x_z0[i]  - acc.hit_coords[i] * acc.pars.z0        * coverage) / (coverage-1);
        acc.hit_x_phi[i] = (acc.hit_x_phi[i] - acc.hit_coords[i] * acc.pars.phi       * coverage) / (coverage-1);
        acc.hit_x_eta[i] = (acc.hit_x_eta[i] - acc.hit_coords[i] * acc.pars.eta       * coverage) / (coverage-1);

        // Covariance (and symmetrize)
        for (unsigned j = i ; j < nCoords; ++j)
        {
            acc.covariance[j * nCoords + i] = acc.covariance[i * nCoords + j] =
                (acc.covariance[i * nCoords + j] - acc.hit_coords[i] * acc.hit_coords[j] * coverage) / (coverage-1);
	}
    }

    return acc;
}


geo_constants HTTConstGenAlgo::makeConsts(HTTMatrixAccumulator const & acc, std::vector<bool> const & usable,
			 std::vector<double> const & inv_covariance,
			 std::vector<double> const & eigvals, vector2D<double> const & eigvecs)
{
    size_t nCoords = acc.hit_coords.size();
    geo_constants geo(nCoords);

    geo.Vcurvature = matrix_multiply(inv_covariance, acc.hit_x_QoP);
    geo.Vd0        = matrix_multiply(inv_covariance, acc.hit_x_d0);
    geo.Vphi       = matrix_multiply(inv_covariance, acc.hit_x_phi);
    geo.Vz0        = matrix_multiply(inv_covariance, acc.hit_x_z0);
    geo.Veta       = matrix_multiply(inv_covariance, acc.hit_x_eta);

    for (size_t i = 0; i < nCoords - HTTTrackPars::NPARS; i++)
    {
        if (!usable[i]) continue; // vectors are filled with 0's by default
        for (size_t j = 0; j < nCoords; j++)
        {
            if (!usable[j]) continue;
            if (eigvals[i] > 0)
                geo.kernel(i, j) = eigvecs(i, j) / sqrt(eigvals[i]);

            geo.kaverages[i] += -geo.kernel(i, j) * acc.hit_coords[j];
        }
    }

    geo.pars = acc.pars;
    for (size_t i = 0; i < nCoords; i++)
    {
        geo.pars.d0         += -geo.Vd0[i]        * acc.hit_coords[i];
        geo.pars.qOverPt    += -geo.Vcurvature[i] * acc.hit_coords[i];
        geo.pars.phi        += -geo.Vphi[i]       * acc.hit_coords[i];
        geo.pars.z0         += -geo.Vz0[i]        * acc.hit_coords[i];
        geo.pars.eta        += -geo.Veta[i]       * acc.hit_coords[i];
    }

    geo.real = static_cast<int>(acc.track_bins.size());
    return geo;
}


/*
 * Calculates matrix multiplication x = A * b;
 *
 * @param A - (n*n) matrix
 * @param b - length-n vector
 * @return A * b (length-n vector)
 */
std::vector<double> HTTConstGenAlgo::matrix_multiply(std::vector<double> const & A, std::vector<double> const & b)
{
    size_t n = b.size();
    std::vector<double> x(n);

    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            x[i] += A[i * n + j] * b[j];

    return x;
}


bool HTTConstGenAlgo::isSingular(TMatrixD mtx)
{
    TDecompLU dc(mtx); // note mtx is a copy
    bool ok = dc.InvertLU(mtx, MTX_TOLERANCE);
    return !ok;
}


/**
 * Removes the rows/columns specified by !usable.
 *
 * @param n - Initial size of matrix
 * @param mtx_v - Matrix to be reduced (size n*n)
 * @param usable - Which rows/columns are to be kept (size n)
 * @param nDimToUse - Must be count(usable), size of output matrix
 */
TMatrixD HTTConstGenAlgo::getReducedMatrix(size_t n, std::vector<double> const & mtx_v, std::vector<bool> const & usable, size_t nDimToUse)
{
    TMatrixD mtx(n, n, mtx_v.data());
    TMatrixD newmtx(nDimToUse, nDimToUse);

    size_t counteri = 0;
    for (size_t i = 0; i < n; i++)
    {
        if (!usable[i]) continue;

        size_t counterj = 0;
        for (size_t j = 0; j < n; j++)
        {
            if (!usable[j]) continue;
            newmtx[counteri][counterj] = mtx[i][j]; // if we use this coordinate, copy it over
            counterj++; // iterate counter
        }
        assert(counterj == nDimToUse);
        counteri++;
    }
    assert(counteri == nDimToUse);

    return newmtx;
}


/**
 * Inverts a reduced matrix, then pads with zeros to recover a full-sized matrix.
 *
 * @param n_full - Size of full (output) system
 * @param mtx - A reduced matrix to be inverted (size n_redu * n_redu)
 * @param usable - Which rows/columns were kept (size n_full, count(usable == n_redu))
 */
std::vector<double> HTTConstGenAlgo::invert(size_t n_full, TMatrixD mtx, std::vector<bool> const & usable)
{
    // Output
    std::vector<double> inv(n_full * n_full); // filled with 0s

    mtx.Invert();

    size_t counteri = 0;
    for (size_t i = 0; i < n_full; i++)
    {
        if (!usable[i]) continue;

        size_t counterj = 0;
        for (size_t j = 0; j < n_full; j++)
        {
            if (!usable[j]) continue;
            inv[i*n_full + j] = mtx[counteri][counterj];
            counterj++;
        }
        counteri++;
    }

    return inv;
}


/*
 * Calculates the eigensystem of mtx_v, storing it into eigvals_v and eigvecs_v.
 * This functions accepts a reduced matrix (0 rows and columns removed) but
 * returns the full-size eigensystem (padding with 0s as defined by usable).
 * The eigensystem is sorted by increasing |eigenvalue|.
 *
 * @param n_redu - Size of reduced system
 * @param n_full - Size of full system
 * @param mtx_v - Input matrix (n_redu * n_redu)
 * @param usable - List of usable rows/columns in the full matrix (n_full)
 * @param eigvals_v - Output eigenvalues (n_full), with !usable indices filled with 0
 * @param eigvecs_v - Output eigenvectors (n_full * n_full, ROW! oriented), with !usable rows & columns filled with 0
 */
void HTTConstGenAlgo::eigen(size_t n_redu, size_t n_full, TMatrixD &mtx, std::vector<bool> const & usable, std::vector<double> & eigvals_full, vector2D<double> & eigvecs_full)
{

    // Reduced system (these are sorted by decreasing |eigenvalue|)
    TVectorD eigvals_redu;
    TMatrixD eigvecs_redu = mtx.EigenVectors(eigvals_redu);

    // Full system (to be copied into)
    eigvals_full.resize(n_full, 0);
    eigvecs_full.resize(n_full, n_full, 0);

    // Reverse order AND transpose. First row in eigvecs_full = last column in eigvecs_redu.
    // Here, i = row index, j = column index.
    size_t j_redu = n_redu - 1;
    for (size_t i_full = 0; i_full < n_full; i_full++)
    {
        if (!usable[i_full]) continue;

        size_t i_redu = 0;
        for (size_t j_full = 0; j_full < n_full; j_full++)
        {
            if (!usable[j_full]) continue;
            eigvecs_full(i_full, j_full) = eigvecs_redu[i_redu][j_redu];
            i_redu++;
        }
        eigvals_full[i_full] = eigvals_redu[j_redu];
        j_redu--;
    }
}


double HTTConstGenAlgo::dot(const double* vec1, const double* vec2, size_t size)
{
    double total = 0;
    for (size_t i = 0; i < size; i++)
        total += vec1[i] * vec2[i];
    return total;
}


geo_constants HTTConstGenAlgo::calculate_gcorth(geo_constants geo, int nCoords, std::vector<bool> const & usable)
{
    for (int i = 0; i < nCoords - HTTTrackPars::NPARS;i++)
    {
        if (!usable[i]) continue;
        double norm = dot(geo.kernel[i], geo.kernel[i], nCoords);

        auto project = [&](std::vector<double> & hit_x_par, double & par)
        {
            double pr = dot(hit_x_par.data(), geo.kernel[i], nCoords) / norm;
            for (int j = 0; j < nCoords; j++) hit_x_par[j] += -geo.kernel(i,j) * pr;
            par += -geo.kaverages[i] * pr;
        };

        project(geo.Vd0, geo.pars.d0);
        project(geo.Vcurvature, geo.pars.qOverPt);
        project(geo.Vphi, geo.pars.phi);
        project(geo.Vz0, geo.pars.z0);
        project(geo.Veta, geo.pars.eta);
    }

    return geo;
}


StatusCode HTTConstGenAlgo::execute()
{
  // Do nothing; this class does not process events. The main algorithm is
  // called in initialize() and finalize().
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTConstGenAlgo::finalize()
{
    ATH_MSG_DEBUG("finalize()");

    std::string filename = "corrgen_raw_" + std::to_string(m_nLayers) + "L_reg" + std::to_string(m_region) + "_checkGood" + std::to_string(m_CheckGood2ndStage) + ".gcon";

    DumpConstants(m_geo_consts, filename);

    if (m_dumpMissingHitsConstants) {
      for (int missing = 0; missing < m_nLayers; missing++) {
	filename = "corrgen_raw_" + std::to_string(m_nLayers) + "L_reg" + std::to_string(m_region) + "_checkGood" + std::to_string(m_CheckGood2ndStage) + "_skipPlane" + std::to_string(missing) + ".gcon";
	// pick up only the ones for this missing plane
	DumpConstants(m_geo_consts_with_missinghit[missing], filename);
      }
    }

    writeSectors();

    ATH_CHECK(m_tHistSvc->finalize());
    m_mafile->Close();

    return StatusCode::SUCCESS;
}


void HTTConstGenAlgo::writeSectors()
{
    // Create HTTSectorSlice
    HTTTrackPars copymin = m_sliceMin;
    HTTTrackPars copymax = m_sliceMax;
    HTTSectorSlice slice(m_geo_consts.size(), m_sliceNBins, copymin, copymax);

    // Open files
    std::string sector_filename = "sectors_raw_" + std::to_string(m_nLayers) + "L_reg" + std::to_string(m_region) + "_checkGood" + std::to_string(m_CheckGood2ndStage) + ".patt";
    std::string sectorHW_filename = "sectorsHW_raw_" + std::to_string(m_nLayers) + "L_reg" + std::to_string(m_region) + "_checkGood" + std::to_string(m_CheckGood2ndStage) + ".patt";
    FILE *sector_file = fopen(sector_filename.c_str(),"w");
    FILE *sectorHW_file = fopen(sectorHW_filename.c_str(),"w");

    fprintf(sector_file,"%lu %d\n",m_geo_consts.size(),m_nLayers);
    fprintf(sectorHW_file,"%lu %d\n",m_geo_consts.size(),m_nLayers);

    // Write sectors
    HTTMatrixReader reader(m_good_tree, m_nLayers, m_nCoords);
    while (reader.nextEntry())
    {
        HTTMatrixAccumulator const & acc = reader.getAccumulator();
        size_t sector = reader.getEntry();

        fprintf(sector_file,"%lu ", sector);
        fprintf(sectorHW_file,"%lu ", sector);
        for(int i=0;i<m_nLayers;i++)
        {
            fprintf(sector_file,"%d ", acc.FTK_modules[i]);
            fprintf(sectorHW_file,"%d ", reader.getModules()[i]);
        }
        fprintf(sector_file,"0 %lu", acc.track_bins.size());
        fprintf(sector_file,"\n");
        fprintf(sectorHW_file,"0 %lu", acc.track_bins.size());
        fprintf(sectorHW_file,"\n");

        for (HTTTrackParsI const & pars : acc.track_bins)
            slice.addSectorToSlice(sector, pars);
    }

    fclose(sector_file);
    fclose(sectorHW_file);
    std::string slice_filename = "slices_" + std::to_string(m_nLayers) + "L_reg" + std::to_string(m_region) + ".root";
    slice.saveSlices(slice_filename);
}


// ASCII file writeout
void HTTConstGenAlgo::DumpConstants(std::vector<geo_constants> &geo_consts, std::string & filename)
{
    FILE *const_file = fopen(filename.c_str(),"w");

    fprintf(const_file,"! ***           RECONSTRUCTION GEOMETRY CONSTANTS               ***\n");
    fprintf(const_file,"\n");
    fprintf(const_file,"Version 2       ! File format version number\n");
    fprintf(const_file,"\n");
    fprintf(const_file,"! ***           PHI is now in GLOBAL coordinate system          ***\n");
    fprintf(const_file," NPLANES\n");
    fprintf(const_file," %d\n",m_nLayers);
    fprintf(const_file," NSECTORS\n");
    fprintf(const_file,"%lu\n",geo_consts.size());
    fprintf(const_file," NDIM\n");
    fprintf(const_file," 2\n");

    std::string str_hex;

    for (size_t sector = 0; sector < geo_consts.size(); sector++)
    {
        //gcon file
        fprintf(const_file,"sector\n");
        fprintf(const_file,"%lu\n", sector);

        fprintf(const_file," Vc \n");
        for(int i=0;i<m_nCoords;i++){
	  fprintf(const_file,"%e\n",geo_consts[sector].Vcurvature[i]);
        }

        fprintf(const_file," Vd \n");
        for(int i=0;i<m_nCoords;i++){
	  fprintf(const_file,"%e\n",geo_consts[sector].Vd0[i]);
        }

        fprintf(const_file," Vf \n");
        for(int i=0;i<m_nCoords;i++){
	  fprintf(const_file,"%e\n",geo_consts[sector].Vphi[i]);
        }

        fprintf(const_file," Vz0 \n");
        for(int i=0;i<m_nCoords;i++){
	  fprintf(const_file,"%e\n",geo_consts[sector].Vz0[i]);
        }

        fprintf(const_file," Vo \n");
        for(int i=0;i<m_nCoords;i++){
	  fprintf(const_file,"%e\n",geo_consts[sector].Veta[i]);
        }

        fprintf(const_file,"kaverages\n");
        for(int i=0;i<m_nCoords-HTTTrackPars::NPARS;i++){
	  fprintf(const_file,"%e\n",m_geo_consts[sector].kaverages[i]);
        }

        fprintf(const_file,"kernel\n");
        for(int i=0;i<m_nCoords-HTTTrackPars::NPARS;i++){
	  for(int j=0;j<m_nCoords;j++){
	    fprintf(const_file,"%e\n",geo_consts[sector].kernel(i, j));
	  }
        }

        fprintf(const_file,"Cc\n");
	fprintf(const_file,"%e\n",geo_consts[sector].pars.qOverPt);

        fprintf(const_file,"Cd\n");
	fprintf(const_file,"%e\n",geo_consts[sector].pars.d0);

        fprintf(const_file,"Cf\n");
	fprintf(const_file,"%e\n",geo_consts[sector].pars.phi);

        fprintf(const_file,"Cz0\n");
	fprintf(const_file,"%e\n",geo_consts[sector].pars.z0);

        fprintf(const_file,"Co\n");
	fprintf(const_file,"%e\n",geo_consts[sector].pars.eta);
    }

    fclose(const_file);

}

