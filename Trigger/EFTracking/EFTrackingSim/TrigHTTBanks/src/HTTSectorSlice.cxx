// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTSectorSlice.h
 * @author unkown
 * @date nope
 * @brief See header.
 */

#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTObjects/HTTTypes.h"

#include <TFile.h>
#include <TTree.h>
#include <TBits.h>
#include <AsgMessaging/MessageCheck.h>

#include <iostream>

using namespace asg::msgUserCode;


void largest_region_wrap(std::vector<bool> const & good_bin, int & i_lower, int & i_upper);


// Max is exclusive
int clamp(int bin, int min, int max)
{
    if (bin < min) return min;
    else if (bin >= max) return max - 1;
    else return bin;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor/Initialization


HTTSectorSlice::HTTSectorSlice(size_t nSectors,
        HTTTrackParsI const & nBins, HTTTrackPars const & min, HTTTrackPars const & max)
    :
        m_nBins(nBins),
        m_min(min),
        m_max(max)
{
    m_bits_phi      = new TClonesArray("TBits", m_nBins.phi);
    m_bits_c        = new TClonesArray("TBits", m_nBins.qOverPt);
    m_bits_d0       = new TClonesArray("TBits", m_nBins.d0);
    m_bits_z0       = new TClonesArray("TBits", m_nBins.z0);
    m_bits_eta   = new TClonesArray("TBits", m_nBins.eta);

    for (int i = 0; i < m_nBins.phi; i++)       new ((*m_bits_phi)[i])      TBits(nSectors);
    for (int i = 0; i < m_nBins.qOverPt; i++) new ((*m_bits_c)[i])        TBits(nSectors);
    for (int i = 0; i < m_nBins.d0; i++)        new ((*m_bits_d0)[i])       TBits(nSectors);
    for (int i = 0; i < m_nBins.z0; i++)        new ((*m_bits_z0)[i])       TBits(nSectors);
    for (int i = 0; i < m_nBins.eta; i++)       new ((*m_bits_eta)[i])   TBits(nSectors);

    calcDependentVals();
}


HTTSectorSlice::HTTSectorSlice(std::string const & filepath) 
{
  ANA_MSG_INFO("Reading " << filepath);
    TFile *slice_file = TFile::Open(filepath.c_str());

    m_bits_phi    = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_phi"));
    m_bits_c      = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_c"));
    m_bits_d0     = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_d"));
    m_bits_z0     = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_z0"));

    // kludge for backward compatability
    if ((slice_file->Get("c_bits_eta")) != nullptr)
      m_bits_eta = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_eta"));
    else
      m_bits_eta = dynamic_cast<TClonesArray*>(slice_file->Get("c_bits_ctheta"));


    TTree *slice_tree = dynamic_cast<TTree*>(slice_file->Get("bin_info"));

    slice_tree->SetBranchAddress("qOverPt_max", &m_max.qOverPt);
    slice_tree->SetBranchAddress("d0_max",        &m_max.d0);
    slice_tree->SetBranchAddress("phi_max",       &m_max.phi);
    slice_tree->SetBranchAddress("z0_max",        &m_max.z0);
    slice_tree->SetBranchAddress("eta_max",       &m_max.eta);

    slice_tree->SetBranchAddress("qOverPt_min", &m_min.qOverPt);
    slice_tree->SetBranchAddress("d0_min",        &m_min.d0);
    slice_tree->SetBranchAddress("phi_min",       &m_min.phi);
    slice_tree->SetBranchAddress("z0_min",        &m_min.z0);
    slice_tree->SetBranchAddress("eta_min",       &m_min.eta);

    slice_tree->SetBranchAddress("qOverPt_bins",&m_nBins.qOverPt);
    slice_tree->SetBranchAddress("d0_bins",       &m_nBins.d0);
    slice_tree->SetBranchAddress("phi_bins",      &m_nBins.phi);
    slice_tree->SetBranchAddress("z0_bins",       &m_nBins.z0);
    slice_tree->SetBranchAddress("eta_bins",      &m_nBins.eta);

    // For backwards compatability
    if (slice_tree->GetBranch("qOverPt_max") == nullptr) slice_tree->SetBranchAddress("halfInvPt_max", &m_max.qOverPt);
    if (slice_tree->GetBranch("qOverPt_min") == nullptr) slice_tree->SetBranchAddress("halfInvPt_min", &m_min.qOverPt);
    if (slice_tree->GetBranch("qOverPt_bins") == nullptr) slice_tree->SetBranchAddress("halfInvPt_bins", &m_nBins.qOverPt);


    slice_tree->GetEntry(0);

    calcDependentVals();

    delete slice_file;
}



void HTTSectorSlice::calcDependentVals()
{

    m_step.phi = (m_max.phi - m_min.phi) / m_nBins.phi;
    m_step.qOverPt = (m_max.qOverPt - m_min.qOverPt) / m_nBins.qOverPt;
    m_step.d0 = (m_max.d0 - m_min.d0) / m_nBins.d0;
    m_step.z0 = (m_max.z0 - m_min.z0) /m_nBins.z0;
    m_step.eta = (m_max.eta - m_min.eta) / m_nBins.eta;
}


///////////////////////////////////////////////////////////////////////////////
// Slice writing functions

void HTTSectorSlice::addSectorToSlice(sector_t sector, HTTTrackParsI const & bins)
{
    int bin;

    bin = clamp(bins.phi, 0, m_nBins.phi);
    dynamic_cast<TBits*>(m_bits_phi->UncheckedAt(bin))->SetBitNumber(sector);

    bin = clamp(bins.qOverPt, 0, m_nBins.qOverPt);
    dynamic_cast<TBits*>(m_bits_c->UncheckedAt(bin))->SetBitNumber(sector);

    bin = clamp(bins.d0, 0, m_nBins.d0);
    dynamic_cast<TBits*>(m_bits_d0->UncheckedAt(bin))->SetBitNumber(sector);

    bin = clamp(bins.z0, 0, m_nBins.z0);
    dynamic_cast<TBits*>(m_bits_z0->UncheckedAt(bin))->SetBitNumber(sector);

    bin = clamp(bins.eta, 0, m_nBins.eta);
    dynamic_cast<TBits*>(m_bits_eta->UncheckedAt(bin))->SetBitNumber(sector);
}


void HTTSectorSlice::saveSlices(const std::string & filepath)
{
    TFile ofile(filepath.c_str(), "recreate");

    m_bits_phi->Write("c_bits_phi", TObject::kSingleKey);
    m_bits_c->Write("c_bits_c", TObject::kSingleKey);
    m_bits_d0->Write("c_bits_d", TObject::kSingleKey);
    m_bits_z0->Write("c_bits_z0", TObject::kSingleKey);
    m_bits_eta->Write("c_bits_eta", TObject::kSingleKey);

    TTree *slice_tree = new TTree("bin_info", "Slice binning information");

    slice_tree->Branch("qOverPt_max", &m_max.qOverPt);
    slice_tree->Branch("d0_max",        &m_max.d0);
    slice_tree->Branch("phi_max",       &m_max.phi);
    slice_tree->Branch("z0_max",        &m_max.z0);
    slice_tree->Branch("eta_max",       &m_max.eta);

    slice_tree->Branch("qOverPt_min", &m_min.qOverPt);
    slice_tree->Branch("d0_min",        &m_min.d0);
    slice_tree->Branch("phi_min",       &m_min.phi);
    slice_tree->Branch("z0_min",        &m_min.z0);
    slice_tree->Branch("eta_min",       &m_min.eta);

    slice_tree->Branch("qOverPt_bins",&m_nBins.qOverPt);
    slice_tree->Branch("d0_bins",       &m_nBins.d0);
    slice_tree->Branch("phi_bins",      &m_nBins.phi);
    slice_tree->Branch("z0_bins",       &m_nBins.z0);
    slice_tree->Branch("eta_bins",      &m_nBins.eta);

    slice_tree->Fill();

    ofile.Write();
    ofile.Close();
}


///////////////////////////////////////////////////////////////////////////////
// searchSectors and helpers


bool HTTSectorSlice::checkTrackPars(HTTTrackPars const & pars) const
{
    if (pars.phi < m_min.phi || pars.phi > m_max.phi) return false;
    if (pars.qOverPt < m_min.qOverPt || pars.qOverPt > m_max.qOverPt) return false;
    if (pars.d0 < m_min.d0 || pars.d0 > m_max.d0) return false;
    if (pars.z0 < m_min.z0 || pars.z0 > m_max.z0) return false;
    if (pars.eta < m_min.eta || pars.eta > m_max.eta) return false;
    return true;
}


// If the bin is underrun or overrun set to 0 or clamp - 1
int getBin(double x, double min, double step, int clamp_max)
{
    int bin = static_cast<int>((x - min) / step);
    return clamp(bin, 0, clamp_max);
}


std::vector<sector_t> HTTSectorSlice::searchSectors(HTTTrackPars const & pars) const
{
    std::vector<sector_t> sectors;
    if (!checkTrackPars(pars))
    {
      ANA_MSG_ERROR("searchSectors() Bad track parameters: " << pars);
        return sectors;
    }

    // Special case for phi
    int bin = static_cast<int>((pars.phi - m_min.phi) / m_step.phi);
    bin = (bin + m_nBins.phi) % m_nBins.phi; // modulo operator: phi wraps around
    TBits result_bits = *(TBits*)(m_bits_phi->UncheckedAt(bin));
    
    result_bits &= *(TBits*)(m_bits_c->UncheckedAt(getBin(pars.qOverPt, m_min.qOverPt, m_step.qOverPt, m_nBins.qOverPt)));
    result_bits &= *(TBits*)(m_bits_d0->UncheckedAt(getBin(pars.d0, m_min.d0, m_step.d0, m_nBins.d0)));
    result_bits &= *(TBits*)(m_bits_z0->UncheckedAt(getBin(pars.z0, m_min.z0, m_step.z0, m_nBins.z0)));
    result_bits &= *(TBits*)(m_bits_eta->UncheckedAt(getBin(pars.eta, m_min.eta, m_step.eta, m_nBins.eta)));

    unsigned int curPos = result_bits.FirstSetBit(0);
    while (curPos != result_bits.GetNbits())
    {
        sectors.push_back(static_cast<sector_t>(curPos));
        curPos = result_bits.FirstSetBit(curPos + 1);
    }

    return sectors;
}



///////////////////////////////////////////////////////////////////////////////
// getBoundaries and helpers

// Calculates the parameter boundaries for where bitmasks are non-empty.
// Returns (min, max).
std::pair<HTTTrackPars, HTTTrackPars> HTTSectorSlice::getBoundaries() const
{
    HTTTrackPars min;
    HTTTrackPars max;

    getBoundary(m_bits_phi, m_min.phi, m_max.phi, min.phi, max.phi, true, "phi");
    getBoundary(m_bits_c,  m_min.qOverPt,  m_max.qOverPt,  min.qOverPt,  max.qOverPt,  false, "c");
    getBoundary(m_bits_d0, m_min.d0, m_max.d0, min.d0, max.d0, false, "d0");
    getBoundary(m_bits_z0, m_min.z0, m_max.z0, min.z0, max.z0, false, "z0");
    getBoundary(m_bits_eta, m_min.eta, m_max.eta, min.eta, max.eta, false, "eta");

    ANA_MSG_INFO("getBoundaries() returning " << min << "; " << max);
    return { min, max };
}


/**
 * Given a range [x_min, x_max] split in bins corresponding to the size of `bitmasks`,
 * finds a tighter range of x values for where the bitmasks are non-empty.
 *
 * @param bitmasks - A TBits vector with sector bitmasks for each bin.
 * @param bound_min - Return value for the boundary min x
 * @param bound_max - Return value for the boundary max x
 */
void HTTSectorSlice::getBoundary(const TClonesArray *bitmasks, double x_min, double x_max,
        double &bound_min, double &bound_max, bool wraps, const char *debug) const
{
    int nbin = bitmasks->GetEntries();

    // Find the bounds of where there are bits, i_lower and i_upper are inclusive.
    int i_lower, i_upper;
    if (!wraps)
    {
        for (i_lower = 0; i_lower < nbin-1; i_lower++)
            if (dynamic_cast<TBits const *>(bitmasks->UncheckedAt(i_lower))->CountBits() > 0) break;
        for (i_upper = nbin-1; i_upper > 0; i_upper--)
            if (dynamic_cast<TBits const *>(bitmasks->UncheckedAt(i_upper))->CountBits() > 0) break;
    }
    else // special case for cyclic values that wrap around, like phi
    {
        // Mark bins with nonzero bits, with a safety margin
        std::vector<bool> good_bins(nbin);
        for (int j = 0; j < nbin; j++)
        {
            if (dynamic_cast<TBits const *>(bitmasks->UncheckedAt(j))->CountBits() > 0)
            {
                // add safety margin: +/- 1 bin
                // if the good bins are not actually continuous, the region found below will leave out some bins
                good_bins[(j+nbin-1) % nbin] = true; // mod to handle wrap-around
                good_bins[j] = true;
                good_bins[(j+1) % nbin] = true;
            }
        }

        // Locate largest continuous region
        largest_region_wrap(good_bins, i_lower, i_upper);
    }

    if (i_lower >= i_upper) ANA_MSG_WARNING("getBoundary() Completely empty bitmask array");
    bound_min = x_min + ((x_max-x_min) * i_lower) / nbin;
    bound_max = x_min + ((x_max-x_min) * (i_upper+1)) / nbin;

    ANA_MSG_INFO("getBoundary() " << debug << " nbin=" << nbin
		 << " range=[" << x_min << ", " << x_max
		 << "], index=[" << i_lower << ", " << i_upper
		 << "], result=[" << bound_min << ", " << bound_max << "]");
}


void largest_region_wrap(std::vector<bool> const & good_bins, int & i_lower, int & i_upper)
{
    int start = -1; // start of current region
    int best_start = -1; // start of best region
    int best_size = 0; // size of best region
    for (int j = 0; j < 2*(int)good_bins.size(); j++) // j < 2*size to get regions across j=0
    {
        if (good_bins[j % good_bins.size()])
        {
            if (start < 0) start = j; // start region
            int size = j + 1 - start;
            if (size > best_size)
            {
                best_start = start;
                best_size = size;
                if (best_size == (int)good_bins.size()) break; // All bins are filled
            }
        }
        else // end region
        {
            start = -1;
        }
    }
    i_lower = best_start;
    i_upper = best_start + best_size - 1; // inclusive
        // May be larger than nbin, which causes max phi to be larger than 2pi, e.g.
}


