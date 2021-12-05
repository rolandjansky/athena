/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaCaloUtils/egammaStripsShape.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"
#include "egammaUtils/egammaqweta1c.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cfloat>
#include <cmath>
#include <vector>

using CLHEP::GeV;

namespace {
// 5 cells second sampling granularity ~0.025 in eta
constexpr int neta = 5;
// 2 strips in phi and cover a region of +-1.1875
constexpr int nphi = 2;
// 8 strips per cell in barrel
constexpr int STRIP_ARRAY_SIZE = 8 * neta;

double
proxim(const double b, const double a)
{
  return b + 2. * M_PI * round((a - b) / (2. * M_PI));
}

double
dim(const double a, const double b)
{
  return a - std::min(a, b);
}

struct StripArrayHelper
{
  StripArrayHelper()
    : eta(0.)
    , etaraw(0.)
    , energy(0.)
    , deta(0.)
    , ncell(0.)
  {}
  bool operator<(const StripArrayHelper& cell2) const
  {
    return etaraw < cell2.etaraw;
  }
  double eta;
  double etaraw;
  double energy;
  double deta;
  int ncell;
};

/*
From the original (eta,phi) position, find the location
  (sampling, barrel/end-cap, granularity)
set an array of energies,eta,phi in ~40 strips around max
 */
void
setArray(const xAOD::CaloCluster& cluster,
         const CaloDetDescrManager& cmgr,
         CaloSampling::CaloSample sam,
         double eta,
         double phi,
         double deta,
         double dphi,
         double* enecell,
         double* etacell,
         double* gracell,
         int* ncell)
{
  // Raw --> Calo Frame
  // Difference  is important in end-cap which is shifted by about 4 cm
  double etaraw = eta;
  double phiraw = phi;
  // look for the corresponding DetDescrElement
  const CaloDetDescrElement* dde = cmgr.get_element(
    sam == CaloSampling::EMB1 ? CaloCell_ID::EMB1 : CaloCell_ID::EME1,
    eta,
    phi);
  // if dde is found
  if (dde) {
    etaraw = dde->eta_raw();
    phiraw = dde->phi_raw();
  } else {
    return;
  }
  // The selection will be done in Raw co-ordinates
  // defines the boundaries around which to select cells
  double etamin = etaraw - deta;
  double etamax = etaraw + deta;
  double phimin = phiraw - dphi;
  double phimax = phiraw + dphi;
  // Loop over all cells in the cluster
  xAOD::CaloCluster::const_cell_iterator first = cluster.cell_begin();
  xAOD::CaloCluster::const_cell_iterator last = cluster.cell_end();
  // fill in a std::vector the energies,eta,phi values contained in
  // a window (deta,dphi)
  std::vector<StripArrayHelper> stripArray;
  stripArray.reserve(2 * STRIP_ARRAY_SIZE);
  // positon of elements
  double eta_cell = 0.;
  double phi_cell0 = 0.;
  double phi_cell = 0.;

  for (; first != last; ++first) {
    // ensure we are in 1st sampling
    const CaloCell* theCell = *first;
    if (!theCell) {
      continue;
    }
    if (theCell->caloDDE()->getSampling() == sam) {
      // retrieve the eta,phi of the cell
      eta_cell = theCell->caloDDE()->eta_raw();
      // adjust for possible 2*pi offset.
      phi_cell0 = theCell->caloDDE()->phi_raw();
      phi_cell = proxim(phi_cell0, phiraw);
      // check if we are within boundaries
      if (eta_cell >= etamin && eta_cell <= etamax) {
        if (phi_cell >= phimin && phi_cell <= phimax) {
          StripArrayHelper stripHelp;
          // energy
          stripHelp.energy = theCell->energy() * (first.weight());
          // eta
          stripHelp.eta = theCell->eta();
          // eta raw
          stripHelp.etaraw = theCell->caloDDE()->eta_raw();
          // eta granularity
          stripHelp.deta = theCell->caloDDE()->deta();
          // index/number of cells in the array
          stripHelp.ncell++;
          stripArray.push_back(stripHelp);
        }
      }
    }
  }
  const size_t elementCount = stripArray.size();
  // Exit early if no cells.
  if (elementCount == 0) {
    return;
  }
  // sort vector in eta
  std::sort(stripArray.begin(), stripArray.end());
  // loop on intermediate array and merge two cells in phi (when they exist)
  int ieta = 0;
  bool next = false;
  // merge in phi
  for (size_t i = 0; i < (elementCount - 1); ++i) {
    // Maximum STRIP_ARRAY_SIZE elements
    if (ieta < STRIP_ARRAY_SIZE) {
      // energy
      enecell[ieta] += stripArray[i].energy;
      // eta
      etacell[ieta] = stripArray[i].eta;
      // eta granularity
      gracell[ieta] = stripArray[i].deta;
      // index/number of cells in the array
      ++ncell[ieta];
      // check if eta of this element is equal to the pevious one
      // in which case the two cells have to be merged
      if (std::abs(stripArray[i].etaraw - stripArray[i + 1].etaraw) > 1e-05)
        next = true;
      if (next) {
        // Increment the final array only if do not want to merge
        // otherwise continue as to merge
        ieta++;
        next = false;
      }
    }
  }
  // special case for last element which was not treated yet
  int index = elementCount - 1;
  // if previous element had a different eta then append the array
  // NB: this could happen if only one cell in phi was available
  if (index == 0 || std::abs(stripArray[index].etaraw -
                             stripArray[index - 1].etaraw) > 1e-05) {
    // energy
    enecell[ieta] = stripArray[index].energy;
  }
  if (index != 0 && std::abs(stripArray[index].etaraw -
                             stripArray[index - 1].etaraw) < 1e-05) {
    // energy
    enecell[ieta] += stripArray[index].energy;
  }
  // eta
  etacell[ieta] = stripArray[index].eta;
  // eta granularity
  gracell[ieta] = stripArray[index].deta;
  // index/number of cells in the array
  ++ncell[ieta];
}

/** @brief check index of seed in the array*/
void
setIndexSeed(egammaStripsShape::Info& info,
             const double* etacell,
             const double* gracell)
{
  //
  // Look for the index of the seed in the array previously filled
  //
  double demi_eta = 0.;
  double eta_min = 0.;
  double eta_max = 0.;
  for (int ieta = 0; ieta < STRIP_ARRAY_SIZE - 1; ieta++) {
    // from the eta of the cell it is necessary
    // to have the boundaries of this cell +/- half of its granularity
    demi_eta = gracell[ieta] / 2.;
    eta_min = etacell[ieta] - demi_eta;
    eta_max = etacell[ieta] + demi_eta;
    // Beware that list is arranged from larger values to smaller ones
    if ((std::abs(info.etaseed) > std::abs(eta_min) &&
         std::abs(info.etaseed) <= std::abs(eta_max)) ||
        (std::abs(info.etaseed) <= std::abs(eta_min) &&
         std::abs(info.etaseed) > std::abs(eta_max)))
      info.ncetaseed = ieta;
  }
}

/** @brief set total width in strips*/
void
setWstot(egammaStripsShape::Info& info,
         double deta,
         const double* enecell,
         const double* etacell,
         const int* ncell)
{
  //
  // calculate width in half the region (that's the one used for e-ID)
  //
  // We take only half of the region in eta not in phi
  double etamin = info.etamax - deta / 2.;
  double etamax = info.etamax + deta / 2.;
  double mean = 0.;
  double wtot = 0.;
  double etot = 0.;
  // loop on each element of the array
  for (int ieta = 0; ieta < STRIP_ARRAY_SIZE; ieta++) {
    if (ncell[ieta] == 0)
      continue;
    if (etacell[ieta] >= etamin && etacell[ieta] <= etamax) {
      wtot += enecell[ieta] * (ieta - info.ncetamax) * (ieta - info.ncetamax);
      mean += enecell[ieta] * (ieta - info.ncetamax);
      etot += enecell[ieta];
    }
  }
  // width is defined only if total energy is positive
  if (etot > 0) {
    info.etot = etot;
    wtot = wtot / etot;
    mean = mean / etot;
    info.wstot = wtot > 0 ? std::sqrt(wtot) : -9999.;
  }
}

/** @brief set fraction of energy in 2nd sampling*/
void
setF2(egammaStripsShape::Info& info,
      const double* enecell,
      const double eallsamples)
{
  //
  // Fraction of energy in two highest energy strips
  //
  double e1 = info.emaxs1;
  // strip on left of highest energetic strips
  double eleft = info.ncetamax > 0 ? enecell[info.ncetamax - 1] : 0;
  // strip on right of highest energetic strip
  double eright =
    info.ncetamax < STRIP_ARRAY_SIZE - 1 ? enecell[info.ncetamax + 1] : 0;
  // find hottest of these strips
  double e2 = std::max(eleft, eright);
  // calculate fraction of the two highest energy strips
  info.f2 = eallsamples > 0. ? (e1 + e2) / eallsamples : 0.;
}

/** @brief set energy in 3x1 and in 15x3 strips*/
void
setEnergy(egammaStripsShape::Info& info, const double* enecell)
{
  //
  // Energy in the strips in a cluster of 15 strips
  // and in a cluster of 3 strips - two cells are merge in phi
  //
  if (info.ncetamax < 0 || info.ncetamax > STRIP_ARRAY_SIZE)
    return;
  double energy = 0.;
  int nlo = std::max(info.ncetamax - 1, 0);
  int nhi = std::min(info.ncetamax + 1, STRIP_ARRAY_SIZE - 1);
  for (int ieta = nlo; ieta <= nhi; ieta++) {
    energy += enecell[ieta];
  }
  info.e132 = energy;

  energy = 0.;
  nlo = std::max(info.ncetamax - 7, 0);
  nhi = std::min(info.ncetamax + 7, STRIP_ARRAY_SIZE - 1);
  for (int ieta = nlo; ieta <= nhi; ieta++) {
    energy += enecell[ieta];
  }
  info.e1152 = energy;
}

/** @brief set asymmetry*/
void
setAsymmetry(egammaStripsShape::Info& info, const double* enecell)
{
  //
  // Asymmetry of the shower in +/- 3 strips
  // (E(-1)-E(+1))/(E(-1)+E(+1))
  //
  if (info.ncetamax < 0 || info.ncetamax > STRIP_ARRAY_SIZE)
    return;
  double asl = 0.;
  double asr = 0.;

  // define index of the array from max-1 strips (if possible)
  int nlo = std::max(info.ncetamax - 1, 0);
  // define index of the array from max+1 strips (if possible)
  int nhi = std::min(info.ncetamax + 1, STRIP_ARRAY_SIZE - 1);
  for (int ieta = nlo; ieta <= info.ncetamax; ieta++)
    asl += enecell[ieta];
  for (int ieta = info.ncetamax; ieta <= nhi; ieta++)
    asr += enecell[ieta];

  if (asl + asr > 0.)
    info.asymmetrys3 = (asl - asr) / (asl + asr);
}

/** @brief set width in three strips*/
void
setWs3(egammaStripsShape::Info& info,
       const xAOD::CaloCluster::CaloSample sam,
       const xAOD::CaloCluster& cluster,
       const double* enecell,
       const double* etacell,
       const int* ncell)
{
  //
  // Width in three strips centered on the strip with the largest energy
  //

  double energy = 0.;
  double width3 = 0.;
  double eta1 = 0.;
  // highest and lowest indexes
  int nlo = std::max(info.ncetamax - 1, 0);
  int nhi = std::min(info.ncetamax + 1, STRIP_ARRAY_SIZE - 1);
  for (int ieta = nlo; ieta <= nhi; ieta++) {
    if (ncell[ieta] == 0)
      continue;
    width3 += enecell[ieta] * (ieta - info.ncetamax) * (ieta - info.ncetamax);
    eta1 += enecell[ieta] * etacell[ieta];
    energy += enecell[ieta];
  }

  if (energy > 0) {
    if (width3 > 0) {
      info.ws3 = std::sqrt(width3 / energy);
    }
    info.etas3 = eta1 / energy;
    // corrected width for position inside the cell
    // estimated from the first sampling
    info.ws3c = egammaqweta1c::Correct(
      cluster.etaSample(sam), cluster.etamax(sam), info.ws3);
    info.poscs1 =
      egammaqweta1c::RelPosition(cluster.etaSample(sam), cluster.etamax(sam));
  }
}

/** @brief set difference between eta of max and eta of cells*/
double
setDeltaEtaTrackShower(int nstrips, int ieta, const double* enecell)
{
  //
  // Shower position
  // using +/- "nstrips" strips centered on the strip ieta
  // this could be the hottest cell or the seed cell
  double energy = 0.;
  double pos = 0.;
  if (ieta < 0) {
    return -9999.;
  }
  // define index of the array from max-n strips strips (if possible)
  int nlo = std::max(ieta - nstrips, 0);
  // define index of the array from max+n strips strips (if possible)
  int nhi = std::min(ieta + nstrips, STRIP_ARRAY_SIZE - 1);
  // loop on all strips
  for (int i = nlo; i <= nhi; i++) {
    // position in number of cells (wrt ieta) weighted by energy
    pos += enecell[i] * (i - ieta);
    // sum of energy
    energy += enecell[i];
  }
  // delta eta value is defined only if total energy is positive
  if (energy > 0) {
    pos = pos / energy;
    return pos;
  }
  pos *= -1;
  return -9999.;
}

/** @brief set width in 5 strips*/
void
setWidths5(egammaStripsShape::Info& info, const double* enecell)
{
  //
  // Shower width using 5 strips
  //
  // threshold (tuned ?)
  double Small = 1.e-10;
  double eall = 0.;
  double eave = 0.;
  double width5 = 0.;
  // define index of the array from max-2 strips strips (if possible)
  int nlo = std::max(info.ncetamax - 2, 0);
  // define index of the array from max+2 strips strips (if possible)
  int nhi = std::min(info.ncetamax + 2, STRIP_ARRAY_SIZE - 1);
  // loop on all strips
  for (int ieta = nlo; ieta <= nhi; ieta++) {
    if (ieta >= 0 && ieta < STRIP_ARRAY_SIZE) {
      // sum of energy
      eall += enecell[ieta];
      // position in number of cells (wrt ncetamax) weighted by energy
      eave += enecell[ieta] * (ieta - info.ncetamax);
      // width
      width5 += enecell[ieta] * (ieta - info.ncetamax) * (ieta - info.ncetamax);
    }
  }

  if (eall < Small) {
    return;
  }
  eave = eave / eall;
  width5 = width5 / eall;
  width5 = width5 - eave * eave;
  if (width5 < 0) {
    return;
  }

  info.widths5 = std::sqrt(width5);
}

/** @brief set energy of strip with maximum energy*/
void
setEmax(egammaStripsShape::Info& info, const double* enecell)
{
  //
  // calculate energy of strip with maximum energy
  //
  for (int ieta = 0; ieta < STRIP_ARRAY_SIZE; ieta++) {
    if (enecell[ieta] > info.emaxs1) {
      info.emaxs1 = enecell[ieta];
      info.ncetamax = ieta;
    }
  }
}

/** @brief set energy of the second local maximum*/
int
setEmax2(egammaStripsShape::Info& info,
         const double* enecell,
         const double* gracell,
         const int* ncell)
{
  //
  // energy of the second local maximum (info.esec)
  // energy of the strip with second max 2 (info.esec1)
  //
  int ncetasec1 = 0;
  double ecand;
  double ecand1;
  double escalesec = 0;
  double escalesec1 = 0;

  for (int ieta = 1; ieta < STRIP_ARRAY_SIZE - 1; ieta++) {
    if (ncell[ieta] == 0)
      continue;

    int ieta_left = ieta - 1;
    while (ieta_left >= 0 && ncell[ieta_left] == 0) {
      --ieta_left;
    }
    if (ieta_left < 0) {
      continue;
    }

    int ieta_right = ieta + 1;
    while (ieta_right < STRIP_ARRAY_SIZE && ncell[ieta_right] == 0) {
      ++ieta_right;
    }
    if (ieta_right >= STRIP_ARRAY_SIZE) {
      continue;
    }

    double e = enecell[ieta] / gracell[ieta];
    double e_left = enecell[ieta_left] / gracell[ieta_left];
    double e_right = enecell[ieta_right] / gracell[ieta_right];
    // check that cell is hotter than left or right one
    if (e > e_left && e > e_right) {
      // this ensure that this cell is not the hottest one
      // this + previous line implies that hottest cell and 2nd one
      // are separated by more than one strip
      if (ieta != info.ncetamax) {
        ecand = enecell[ieta] + enecell[ieta_left] + enecell[ieta_right];
        double escalecand = e + e_left + e_right;
        ecand1 = enecell[ieta];

        // test energy of the three strips
        if (escalecand > escalesec) {
          escalesec = escalecand;
          info.esec = ecand;
          // ncetasec  = ieta;
        }
        // test energy of 2nd hottest local maximum
        if (e > escalesec1) {
          escalesec1 = e;
          info.esec1 = ecand1;
          ncetasec1 = ieta;
        }
      }
    }
  }
  return ncetasec1;
}

/** @brief set energy of strip with minimum energy*/
void
setEmin(int ncsec1,
        egammaStripsShape::Info& info,
        const double* enecell,
        const double* gracell,
        const int* ncell)
{
  //
  // energy deposit in the strip with the minimal value
  // between the first and the second maximum
  //
  info.emins1 = 0.;
  // Divide by a number smaller than the eta-width of any cell.
  double escalemin = info.emaxs1 / 0.001;

  if (ncsec1 <= 0)
    return;

  // define index of the array from max+1 strips strips (if possible)
  int nlo = std::min(info.ncetamax, ncsec1) + 1;
  // define index of the array from max-1 strips strips (if possible)
  int nhi = std::max(info.ncetamax, ncsec1) - 1;
  // loop on these strips
  for (int ieta = nlo; ieta <= nhi; ieta++) {
    if (ncell[ieta] == 0)
      continue;
    // correct energy of the strips with its granularity
    // in order to be compared with other energy strips
    // (potentially in other regions of granularity)
    double escale = enecell[ieta] / gracell[ieta];
    if (escale < escalemin) {
      escalemin = escale;
      info.emins1 = enecell[ieta];
    }
  }
}

/** @brief set M.S's valley*/
void
setValley(egammaStripsShape::Info& info, double* enecell)
{
  //
  // Variable defined originally by Michal Seman
  // (was tuned on DC0 data ! never since !!!)
  //

  double Ehsthr = 0.06 * GeV;
  double val = 0.;

  // search for second peak to the right
  for (int ieta = info.ncetamax + 2; ieta < STRIP_ARRAY_SIZE - 1; ieta++) {
    if (enecell[ieta] > Ehsthr &&
        enecell[ieta] > std::max(enecell[ieta - 1], enecell[ieta + 1])) {
      double valley = 0;
      for (int jc = info.ncetamax; jc <= ieta - 1; jc++) {
        valley += dim(enecell[ieta], enecell[jc]);
      }
      if (valley > val)
        val = valley;
    }
  }

  // search for second peak to the left
  for (int ieta = 1; ieta <= info.ncetamax - 2; ieta++) {
    if (enecell[ieta] > Ehsthr &&
        enecell[ieta] > std::max(enecell[ieta - 1], enecell[ieta + 1])) {
      double valley = 0;
      for (int jc = ieta + 1; jc <= info.ncetamax; jc++) {
        valley += dim(enecell[ieta], enecell[jc]);
      }
      if (valley > val)
        val = valley;
    }
  }

  // energy of hottest strip
  double e1 = info.emaxs1;
  // energy of strip on the left
  double eleft = info.ncetamax > 0 ? enecell[info.ncetamax - 1] : 0;
  // energy of strip on the right
  double eright =
    info.ncetamax < STRIP_ARRAY_SIZE - 1 ? enecell[info.ncetamax + 1] : 0;
  // find hottest of these strips
  double e2 = std::max(eleft, eright);

  if (std::abs(e1 + e2) > 0.)
    info.val = val / (e1 + e2);
}
/** @brief set fraction of energy outside shower core
  (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */

void
setFside(egammaStripsShape::Info& info,
         const double* enecell,
         const double* gracell,
         const int* ncell)
{
  //
  // fraction of energy outside shower core
  // (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips)
  //
  // NB: threshold defined by M. Seman for DC0 data (or before ?), never tuned
  // since
  double Ehsthr = 0.06 * GeV;
  // Local variable with max energy in strips
  double e1 = info.emaxs1;
  // left index defined as max-1
  int ileft = info.ncetamax - 1;
  while (ileft > 0 && ncell[ileft] == 0) {
    --ileft;
  }
  double eleft = ileft >= 0 ? enecell[ileft] : 0;

  // right index defined as max+1
  int iright = info.ncetamax + 1;
  while (iright < STRIP_ARRAY_SIZE - 1 && ncell[iright] == 0) {
    ++iright;
  }
  double eright = iright < STRIP_ARRAY_SIZE ? enecell[iright] : 0;

  double fracm = 0.;

  // define index of the array from max-3 strips strips (if possible)
  int nlo = std::max(info.ncetamax - 3, 0);
  // define index of the array from max+3 strips strips (if possible)
  int nhi = std::min(info.ncetamax + 3, STRIP_ARRAY_SIZE - 1);

  if (e1 > Ehsthr) {
    for (int ieta = nlo; ieta <= nhi; ieta++) {
      if (ncell[ieta] == 0)
        continue;
      fracm += (gracell[ieta] > DBL_MIN) ? (enecell[ieta] / gracell[ieta]) : 0.;
    }
    if (std::abs(eleft + eright + e1) > 0.) {
      if ((e1 != 0) && (gracell[info.ncetamax] > DBL_MIN))
        e1 /= gracell[info.ncetamax];
      if (eleft != 0)
        eleft /= gracell[ileft];
      if (eright != 0)
        eright /= gracell[iright];

      info.fside = (std::abs(eleft + eright + e1) > 0.)
                     ? fracm / (eleft + eright + e1) - 1.
                     : 0.;
    }
  }
}

/** @brief set F1core*/
void
setF1core(egammaStripsShape::Info& info, const xAOD::CaloCluster& cluster)
{
  // Fraction of energy reconstructed in the core of the shower
  // core = e132, i.e energy in 3 strips
  //
  double x = -9999.;
  // energy in 3 strips in the 1st sampling
  double e132 = (info.e132 > x) ? info.e132 : 0.;
  // total ennergy
  double energy = cluster.e();
  // build fraction only if both quantities are well defined
  if (std::abs(energy) > 0. && e132 > x) {
    info.f1core = e132 / energy;
  }
}

} // end of anonymous namespace

StatusCode
egammaStripsShape::execute(const xAOD::CaloCluster& cluster,
                           const CaloDetDescrManager& cmgr,
                           Info& info,
                           bool ExecAllVariables) 
{
  //
  // Estimate shower shapes from first compartment
  // based on hottest cell in 2nd sampling , the  deta,dphi,
  // And the barycenter in the 1st sampling (seed)
  //

  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    return StatusCode::SUCCESS;
  }
  // retrieve energy in all samplings
  const double eallsamples = egammaEnergyPositionAllSamples::e(cluster);
  // retrieve energy in 1st sampling
  const double e1 = egammaEnergyPositionAllSamples::e1(cluster);

  // check if cluster is in barrel or end-cap
  // sam is used in SetArray to check that cells belong to strips
  // samgran is used to estimate the window to use cells in eta
  // it is based on the granularity of the middle layer
  // For phi we use the strip layer granularity
  bool in_barrel = egammaEnergyPositionAllSamples::inBarrel(cluster, 2);
  CaloSampling::CaloSample sam = CaloSampling::EMB1;
  CaloSampling::CaloSample samgran = CaloSampling::EMB2;
  CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LAREM;
  if (in_barrel) {
    sam = CaloSampling::EMB1;
    samgran = CaloSampling::EMB2;
  } else {
    sam = CaloSampling::EME1;
    samgran = CaloSampling::EME2;
  }
  // get eta and phi of the seed
  info.etaseed = cluster.etaSample(sam);
  info.phiseed = cluster.phiSample(sam);
  // get eta and phi of the hottest cell in the second sampling
  info.etamax = cluster.etamax(samgran);
  info.phimax = cluster.phimax(samgran);
  // possible for soft electrons to be at -999
  if ((info.etamax == 0. && info.phimax == 0.) ||
      std::abs(info.etamax) > 100.) {
    return StatusCode::SUCCESS;
  }
  // check if we are in a crack or outside area where strips are well defined
  if (std::abs(info.etamax) > 2.4) {
    return StatusCode::SUCCESS;
  }
  if (std::abs(info.etamax) > 1.4 && std::abs(info.etamax) < 1.5) {
    return StatusCode::SUCCESS;
  }
  // We eeds both enums: subCalo and CaloSample
  // use samgran = granularity in second sampling for eta !!!!
  double deta = 0;
  double dphi = 0;
  bool barrel = false;
  int sampling_or_module = 0;
  CaloDetDescrManager::decode_sample(
    subcalo, barrel, sampling_or_module, (CaloCell_ID::CaloSample)samgran);
  const CaloDetDescrElement* dde = cmgr.get_element(
    subcalo, sampling_or_module, barrel, info.etamax, info.phimax);
  // if no object then exit
  if (!dde) {
    return StatusCode::SUCCESS;
  }
  // width in eta is granularity (dde->deta()) times number of cells (neta)
  deta = dde->deta() * neta / 2.0;
  // use samgran = granularity in first sampling for phi
  CaloDetDescrManager::decode_sample(
    subcalo, barrel, sampling_or_module, (CaloCell_ID::CaloSample)sam);
  dde = cmgr.get_element(
    subcalo, sampling_or_module, barrel, info.etamax, info.phimax);
  // if no object then exit
  if (!dde) {
    return StatusCode::SUCCESS;
  }
  // width in phi is granularity (dde->dphi()) times number of cells (nphi)
  dphi = dde->dphi() * nphi / 2.0;

  /* initialize the arrays*/
  double enecell[STRIP_ARRAY_SIZE] = { 0 };
  double etacell[STRIP_ARRAY_SIZE] = { 0 };
  double gracell[STRIP_ARRAY_SIZE] = { 0 };
  int ncell[STRIP_ARRAY_SIZE] = { 0 };

  // Fill the array in energy and eta from which all relevant
  // quantities are estimated
  setArray(cluster,
           cmgr,
           sam,
           info.etamax,
           info.phimax,
           deta,
           dphi,
           enecell,
           etacell,
           gracell,
           ncell);
  /* Fill the the rest of the shapes*/
  // find the corresponding index of the seed
  setIndexSeed(info, etacell, gracell);
  // calculate fraction of energy in strips
  info.f1 = std::abs(eallsamples) > 0. ? e1 / eallsamples : 0.;
  // calculate energy and bin where the energy strip is maximum
  setEmax(info, enecell);
  // calculate total width
  setWstot(info, deta, enecell, etacell, ncell);
  // width in three strips
  setWs3(info, sam, cluster, enecell, etacell, ncell);
  // Energy in in +/-1 and in +/-7 strips
  if (ExecAllVariables) {
    setEnergy(info, enecell);
    setF1core(info, cluster);

    setAsymmetry(info, enecell);
    // Using strips centered on the hottest cell
    // position in eta from +/- 1 strips
    info.deltaEtaTrackShower =
      setDeltaEtaTrackShower(1, info.ncetamax, enecell);
    // Using strips centered on the seed cell
    // position in eta from +/- 7 strips
    info.deltaEtaTrackShower7 =
      setDeltaEtaTrackShower(7, info.ncetaseed, enecell);
    // calculate the fraction of energy int the two highest energy strips
    setF2(info, enecell, eallsamples);
    // Shower width in 5 strips around the highest energy strips
    setWidths5(info, enecell);
    // calculate energy of the second local maximum
    int ncsec1 = setEmax2(info, enecell, gracell, ncell);
    // calculate the energy of the strip with the minimum energy
    setEmin(ncsec1, info, enecell, gracell, ncell);
    // calculate M.S's valley
    setValley(info, enecell);
    // calculate M.S's fraction
    setFside(info, enecell, gracell, ncell);
    info.success = true;
  }
  return StatusCode::SUCCESS;
}

