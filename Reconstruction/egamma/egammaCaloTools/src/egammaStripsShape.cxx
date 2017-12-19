/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaStripsShape.h"
#include "egammaInterfaces/Iegammaqweta1c.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "SGTools/DataProxy.h" 

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <cfloat>

using CLHEP::GeV;

double proxim(const double b, const double a){ return b+2.*M_PI*round((a-b)/(2.*M_PI)) ;}
double dim(const double a,  const double b){ return a - std::min(a,b); } 

class StripArrayHelper
{
public:
  StripArrayHelper() {
    eta=0.;
    etaraw=0.;
    deta=0.;
    ncell=0;
    energy=0.;
  }
  bool operator < (const StripArrayHelper & cell2) const {
    return etaraw<cell2.etaraw;
  }
  double eta;
  double etaraw;
  double energy;
  double deta;
  int ncell;
};

// Constructor 
egammaStripsShape::egammaStripsShape(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cluster(0), 
    m_cellContainer(0),
    m_calo_dd(0),
    m_sizearrayeta(0),
    m_deta(0), 
    m_dphi(0), 
    m_eallsamples(0),
    m_e1(0),
    m_sam(CaloSampling::EMB1), 
    m_samgran(CaloSampling::EMB2), 
    m_offset(CaloSampling::PreSamplerB),
    m_subcalo(CaloCell_ID::LAREM), 
    m_barrel(0),
    m_sampling_or_module(0),
    m_etaseed(0),
    m_phiseed(0)
{ 

  // declare Interface
  declareInterface<IegammaStripsShape>(this);

  InitVariables();
  // initialize the arrays  
  std::fill (m_enecell,     m_enecell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_etacell,     m_etacell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_gracell,     m_gracell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_ncell,       m_ncell+STRIP_ARRAY_SIZE,       0);

}

// ====================================================================
// DESTRUCTOR:
egammaStripsShape::~egammaStripsShape()
{ 
}

// ========================================================================
// INITIALIZE METHOD:
StatusCode egammaStripsShape::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaStripsShape");

  // retrieve all helpers from det store
  m_calo_dd = CaloDetDescrManager::instance();

  // Create egammaqweta1c Tool
  if(m_egammaqweta1c.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaqweta1c);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaqweta1c << " retrieved"); 
  
  // Create egammaEnergyPositionAllSamples Tool
  if(m_egammaEnergyPositionAllSamples.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve "<<m_egammaEnergyPositionAllSamples);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Tool " << m_egammaEnergyPositionAllSamples << " retrieved"); 

  return StatusCode::SUCCESS;
}

// =====================================================================
// FINALIZE METHOD:
StatusCode egammaStripsShape::finalize()
{
  return StatusCode::SUCCESS;
}

// =====================================================================
// ATHENA METHOD:
StatusCode egammaStripsShape::execute(const xAOD::CaloCluster *cluster, 
				      const CaloCellContainer *cell_container) 
{
  //
  // Estimate shower shapes from first compartment
  // based on hottest cell in 2nd sampling , the  deta,dphi,
  // And the barycenter in the 1st sampling (seed) 
  //
  ATH_MSG_DEBUG(" egammaStripsShape: execute");
  // check if cluster is available
  if(!cluster) { 
    ATH_MSG_DEBUG(" egammaStripsShape: Invalid pointer to cluster");
    return StatusCode::SUCCESS;
  }
  // check if cell container is available
  if(!cell_container) { 
    ATH_MSG_DEBUG(" egammaStripsShape: Invalid pointer to cell_container");
    return StatusCode::SUCCESS;
  }

  // check if cluster is in barrel or in the end-cap
  if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
    ATH_MSG_DEBUG(" egammaStripsShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
  }
  
  m_cluster = cluster;
  m_cellContainer = cell_container;

  // initialisation of variables
  InitVariables();
  // retrieve energy in all samplings
  StatusCode sc = m_egammaEnergyPositionAllSamples->execute(m_cluster);
  sc.ignore(); // should we check for errors?
  m_eallsamples = m_egammaEnergyPositionAllSamples->e();
  // retrieve energy in 1st sampling
  m_e1 = m_egammaEnergyPositionAllSamples->e1();

  // initialize the arrays  
  std::fill (m_enecell,     m_enecell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_etacell,     m_etacell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_gracell,     m_gracell+STRIP_ARRAY_SIZE,     0);
  std::fill (m_ncell,       m_ncell+STRIP_ARRAY_SIZE,       0);

  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  if (!FindPosition()) {return StatusCode::SUCCESS;}
  
  // Fill the array in energy and eta from which all relevant
  // quantities are estimated
  setArray(m_sam,m_etamax,m_phimax,m_deta,m_dphi,
	   m_enecell,m_etacell,m_gracell,m_ncell);
  // find the corresponding index of the seed
  setIndexSeed(m_etaseed);

  // calculate fraction of energy in strips
  m_f1 = fabs(m_eallsamples) > 0. ? m_e1/m_eallsamples : 0.;

  // calculate energy and bin where the energy strip is maximum
  setEmax();

  // Energy in in +/-1 and in +/-7 strips
  if (m_ExecAllVariables) {
    setEnergy();
    setF1core();
  }

  // calculate total width 
  setWstot(m_etamax,m_deta);
  // width in three strips
  setWs3(m_sam);

  if (m_ExecAllVariables && m_ExecOtherVariables) setAsymmetry();

  // Using strips centered on the hottest cell
  // position in eta from +/- 1 strips  
  if (m_ExecAllVariables && m_ExecOtherVariables) 
    m_deltaEtaTrackShower = setDeltaEtaTrackShower(1,m_ncetamax);

  // Using strips centered on the seed cell
  // position in eta from +/- 7 strips 
  m_deltaEtaTrackShower7 = setDeltaEtaTrackShower(7,m_ncetaseed);

  // calculate the fraction of energy int the two highest energy strips
  if (m_ExecAllVariables && m_ExecOtherVariables) setF2();
  
  // Shower width in 5 strips around the highest energy strips
  if (m_ExecAllVariables && m_ExecOtherVariables) setWidths5();
  
  // calculate energy of the second local maximum
  int ncsec1 =  setEmax2();

  // calculate the energy of the strip with the minimum energy
  setEmin(ncsec1);
  
  // two followings are not used anymore
  // calculate M.S's valley
  if (m_ExecAllVariables && m_ExecOtherVariables) setValley();

  // calculate M.S's fraction
  setFside();

  m_success = true; 
  return  StatusCode::SUCCESS; 
}

// ====================================================================
void egammaStripsShape::InitVariables()
{
  //
  // initialisation
  //
  double x=0.;
  double x1=-999.;

  m_etamax               = x;
  m_phimax               = x;
  m_ncetamax             = -1;
  m_ncetaseed            = -1;
  m_wstot                = x1;
  m_etot                 = x;
  m_etas3                = x;  
  m_deltaEtaTrackShower  = x1; 
  m_deltaEtaTrackShower7 = x1;
  m_e132                 = x; 
  m_e1152                = x; 
  m_ws3                  = x1; 
  m_ws3c                 = x1; 
  m_poscs1               = x1; 
  m_etaincell            = x1; 
  m_asymmetrys3          = x1; 
  m_f1                   = x;  
  m_f1core               = x;  
  m_f2                   = x; 
  m_widths5              = x1; 
  m_esec                 = x; 
  m_val                  = x;  
  m_fside                = x;
  m_emaxs1               = x; 
  m_emins1               = x; 
  m_esec1                = x;
  m_success              = false ;
}

// =====================================================================
bool egammaStripsShape::FindPosition()
{
  //
  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  // For this we use the tool egammaEnergyAllSamples
  // which uses the xAOD::CaloCluster method inBarrel() and inEndcap()
  // but also, in case close to the crack region where both 
  // boolean can be true, the energy reconstructed in the sampling
  // (actually in strips we use the energy reconstructed in the 2nd sampling)
  //

  m_sizearrayeta = 0;

  // check if cluster is in barrel or end-cap
  // sam is used in SetArray to check that cells belong to strips
  // samgran is used to estimated window to use cells in eta
  // it is based on granularity of middle layer
  // For phi we use the strip layer granularity  
  bool in_barrel =  m_egammaEnergyPositionAllSamples->inBarrel();
  // define accordingly position of xAOD::CaloCluster
  if (in_barrel) {
    m_sam     = CaloSampling::EMB1;
    m_samgran = CaloSampling::EMB2;
    m_offset  = CaloSampling::PreSamplerB;
  } else {
    m_sam     = CaloSampling::EME1;
    m_samgran = CaloSampling::EME2;
    m_offset  = CaloSampling::PreSamplerE; 
  }
  
  // get eta and phi of the seed
  m_etaseed = m_cluster->etaSample(m_sam);  
  m_phiseed = m_cluster->phiSample(m_sam);

  // get eta and phi of the hottest cell in the second sampling
  m_etamax = m_cluster->etamax(m_samgran);  
  m_phimax = m_cluster->phimax(m_samgran);

  // add this protection
  // possible for soft electrons to be at -999
  if ( (m_etamax==0. && m_phimax==0.) || fabs(m_etamax) > 100. ) 
    return false;
  // check if we are in a crack or outside area where strips are well defined
  if (fabs(m_etamax) > 2.4) return false;
  if (fabs(m_etamax) > 1.4 && fabs(m_etamax) < 1.5) return false;
  
  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  // use samgran = granularity in second sampling for eta !!!!
  m_calo_dd->decode_sample(m_subcalo, m_barrel, m_sampling_or_module, 
			   (CaloCell_ID::CaloSample) m_samgran);
  const CaloDetDescrElement* dde =
    m_calo_dd->get_element(m_subcalo,m_sampling_or_module,m_barrel,
                           m_etamax ,m_phimax);
  // if no object then exit
  if (!dde) return false;
  // width in eta is granularity (dde->deta()) times number of cells (m_neta)
  m_deta = dde->deta()*m_neta/2.0;

  // use samgran = granularity in first sampling for phi !!!!
  m_calo_dd->decode_sample(m_subcalo, m_barrel, m_sampling_or_module, 
			   (CaloCell_ID::CaloSample) m_sam);
  dde = m_calo_dd->get_element(m_subcalo, m_sampling_or_module, m_barrel,
			       m_etamax ,m_phimax);
  // if no object then exit
  if (!dde) return false;
  // width in phi is granularity (dde->dphi()) times number of cells (m_nphi)
  m_dphi = dde->dphi()*m_nphi/2.0;

  m_sizearrayeta = (int)STRIP_ARRAY_SIZE;

  return true;
}

// =====================================================================
void egammaStripsShape::setArray(CaloSampling::CaloSample sam,
				 double eta, double phi, 
				 double deta, double dphi,
				 double* enecell_arr,
				 double* etacell_arr,
				 double* gracell_arr,
				 int* ncell_arr)
{
  //
  // Put in an array the energies,eta,phi values contained in 
  // a window (deta,dphi) 
  //

  // two ways can be used to create the array
  // 1- From the list of cells attached to the cluster
  // 2 -Use a calo Cell List
  // temporary array of cell 
  StripArrayHelper stripArray[BIG_STRIP_ARRAY_SIZE];

  //Raw --> Calo Frame 
  //Other variables ATLAS Frame
  //Difference  is important in end-cap which is shifted by about 4 cm
  //
  double etaraw = eta;
  double phiraw = phi;
  // look for the corresponding DetDescrElement
  const CaloDetDescrElement* dde =
    m_calo_dd->get_element (sam == CaloSampling::EMB1 ? CaloCell_ID::EMB1 :
			    CaloCell_ID::EME1, eta, phi);
  // if dde is found 
  if (dde) {
    etaraw = dde->eta_raw();
    phiraw = dde->phi_raw();
  }
  else{
    return;
  }

  //The selection will be done in Raw co-ordinates
  //defines the boundaries around which to select cells
  double etamin = etaraw - deta;
  double etamax = etaraw + deta;
  double phimin = phiraw - dphi;
  double phimax = phiraw + dphi;  
  // index of elements of the array
  int index_array = 0;
  double eta_cell  = 0.;
  double phi_cell0 = 0.;
  double phi_cell  = 0.;
  
  if(m_UseCellsFromCluster){
    // Now loop over all cells in the cluster  
    xAOD::CaloCluster::const_cell_iterator first = m_cluster->cell_begin();
    xAOD::CaloCluster::const_cell_iterator last  = m_cluster->cell_end();
    for (; first != last; ++first) {        
      // ensure we are in 1st sampling
      const CaloCell* theCell = *first;
      if (!theCell){
	continue;
      }
      if( theCell->caloDDE()->getSampling() == sam ) {
	// retrieve the eta,phi of the cell
	eta_cell = theCell->caloDDE()->eta_raw();
	// adjust for possible 2*pi offset. 
	phi_cell0 = theCell->caloDDE()->phi_raw();
	phi_cell  = proxim(phi_cell0,phiraw) ;
	// check if we are within boundaries
	if (eta_cell >= etamin && eta_cell <= etamax) {
	  if (phi_cell >= phimin && phi_cell <= phimax) {	    
	    // a protection is put to avoid to have an array larger 
	    // than 2*STRIP_ARRAY_SIZE
	    if (index_array<BIG_STRIP_ARRAY_SIZE) {
	      // energy
	      stripArray[index_array].energy = theCell->energy()*(first.weight()); 
	      // eta 
	      stripArray[index_array].eta  = theCell->eta();
	      // eta raw
	      stripArray[index_array].etaraw = theCell->caloDDE()->eta_raw();
	      // eta granularity
	      stripArray[index_array].deta  = theCell->caloDDE()->deta();
	      // index/number of cells in the array
	      stripArray[index_array].ncell++;
	      // increase index 
	      index_array++;
	    }
	  }	  
	}
      }
    }
  }
  else {
    CaloCellList *ccl = new CaloCellList(m_cellContainer);
    //CaloCellList uses ATLAS co-ordinates
    ccl->select(dde->eta(),dde->phi_raw(),deta,dphi,sam);
    // defines the cells interators
    CaloCellList::list_iterator first=ccl->begin();
    CaloCellList::list_iterator last =ccl->end();
    for (; first != last; ++first) {        
      //Avoid EME1 being inner endcap
      if(! ((*first)->caloDDE()->is_lar_em_endcap_inner()) )
      // ensure we are in 1st sampling
      if( (*first)->caloDDE()->getSampling() == sam ) {
	// retrieve the eta,phi of the cell
	eta_cell = (*first)->caloDDE()->eta_raw();
	// adjust for possible 2*pi offset. 
	phi_cell0 = (*first)->caloDDE()->phi_raw();
	phi_cell  = proxim(phi_cell0,phiraw) ;
	// check if we are within boundaries
	if (eta_cell >= etamin && eta_cell <= etamax) {
	  if (phi_cell >= phimin && phi_cell <= phimax) {	    
	    // a protection is put to avoid to have an array larger 
	    // than 2*STRIP_ARRAY_SIZE
	    if (index_array<BIG_STRIP_ARRAY_SIZE) {
	      // energy
	      stripArray[index_array].energy = (*first)->energy(); 
	      // eta 
	      stripArray[index_array].eta  = (*first)->eta();
	      // eta raw
	      stripArray[index_array].etaraw  = (*first)->caloDDE()->eta_raw();
	      // eta granularity
	      stripArray[index_array].deta  = (*first)->caloDDE()->deta();
	      // index/number of cells in the array
	      stripArray[index_array].ncell++;
	      // increase index 
	      index_array++;
	    }
	  }	  
	}
      }
    }
  }
  // Exit early if no cells.
  if (index_array == 0){
    return;
  }
  // sort intermediate array with eta
  std::sort(stripArray,stripArray+index_array);

  // loop on intermediate array and merge two cells in phi (when they exist)
  int ieta = 0;
  bool next = false;
  // start loop on 2nd element
  for (int i=0;i<index_array-1;i++) {
    // protection against too big array
    if (ieta<STRIP_ARRAY_SIZE) {
      // energy
      if (enecell_arr) enecell_arr[ieta] += stripArray[i].energy;
      // eta 
      if (etacell_arr) etacell_arr[ieta] = stripArray[i].eta;
      // eta granularity
      if (gracell_arr) gracell_arr[ieta] = stripArray[i].deta;
      // index/number of cells in the array
      if (ncell_arr) ++ncell_arr[ieta];
      // check if eta of this element is equal to the pevious one
      // in which case the two cells have to be merged
      //if (fabs(stripArray[i].eta-stripArry[i+1]).eta>0.00001) next = true;
      if (fabs(stripArray[i].etaraw-stripArray[i+1].etaraw)>0.00001) next = true;
      if (next) {
	//Increment the final array only if do not want to merge
	//otherwise continue as to merge
	ieta++;
	next = false;
      }
    }
  }
  // special case for last element which was not treated yet
  int index = index_array-1;
  // if previous element had a different eta then append the array
  // NB: this could happen if only one cell in phi was available
  if (index == 0 ||
      fabs(stripArray[index].etaraw-stripArray[index-1].etaraw)>0.00001){
    // energy
    if (enecell_arr) enecell_arr[ieta] = stripArray[index].energy;
  }
  if (index != 0 &&
      fabs(stripArray[index].etaraw-stripArray[index-1].etaraw)<0.00001){
    // energy
    if (enecell_arr) {enecell_arr[ieta] += stripArray[index].energy;}
  }
  // eta 
  if (etacell_arr) {etacell_arr[ieta] = stripArray[index].eta;}
  // eta granularity
  if (gracell_arr) {gracell_arr[ieta] = stripArray[index].deta;}
  // index/number of cells in the array
  if (ncell_arr) {++ncell_arr[ieta];}
  
  return;
}

// =====================================================================
void egammaStripsShape::setIndexSeed(double eta) 
{  
  //
  // Look for the index of the seed in the array previously filled
  //
  
  double demi_eta = 0.;
  double eta_min = 0.;
  double eta_max = 0.;
  for(int ieta=0; ieta<m_sizearrayeta-1; ieta++) {     
    // from the eta of the cell it is necessary 
    // to have the boundaries of this cell +/- half of its granularity
    demi_eta = m_gracell[ieta]/2.;
    eta_min = m_etacell[ieta]-demi_eta;
    eta_max = m_etacell[ieta]+demi_eta;
    
    // Beware that list is arranged from larger values to smaller ones
    if ((fabs(eta)>fabs(eta_min) && fabs(eta)<=fabs(eta_max)) ||
	(fabs(eta)<=fabs(eta_min) && fabs(eta)>fabs(eta_max)))
      m_ncetaseed = ieta;
  }
}
  
// =====================================================================
void egammaStripsShape::setWstot(double eta, double deta) 
{
  //
  // calculate width in half the region (that's the one used for e-ID)
  //

  // We take only half of the region in eta not in phi
  double etamin = eta - deta/2.;
  double etamax = eta + deta/2.;

  double mean = 0.; 
  double wtot = 0.;
  double etot = 0.;

  // loop on each element of the array
  for(int ieta=0; ieta<m_sizearrayeta; ieta++) { 
    if (m_ncell[ieta] == 0) continue;
    if (m_etacell[ieta] >= etamin && 
	m_etacell[ieta] <= etamax) {
      wtot  += m_enecell[ieta]*(ieta-m_ncetamax)*(ieta-m_ncetamax); 
      mean  += m_enecell[ieta]*(ieta-m_ncetamax); 
      etot  += m_enecell[ieta]; 
    }
  }

  // width are defined only if total energy is positive
  if( etot>0 ) {
    m_etot = etot;
    wtot = wtot/etot;
    mean = mean/etot; 
    m_wstot  = wtot >0 ? sqrt(wtot) : -9999.; 
  } 
  
  return;
}

// =====================================================================
void egammaStripsShape::setF2()
{
  // 
  // Fraction of energy in two highest energy strips
  //
  double e1 = m_emaxs1; 

  // strip on left of highest energetic strips
  double eleft  = m_ncetamax > 0 ? m_enecell[m_ncetamax-1] : 0;
  // strip on right of highest energetic strip
  double eright = m_ncetamax < m_sizearrayeta-1 ? m_enecell[m_ncetamax+1] : 0;
  // find hottest of these strips
  double e2     = std::max(eleft,eright); 
  
  // calculate fraction of the two highest energy strips
  m_f2 = m_eallsamples > 0. ? (e1+e2)/m_eallsamples : 0.;  

  return;
}

// =====================================================================
void egammaStripsShape::setEnergy()
{
  // 
  // Energy in the strips in a cluster of 15 strips
  // and in a cluster of 3 strips - two cells are merge in phi
  //
 
 if ( m_ncetamax < 0 || m_ncetamax > m_sizearrayeta ) return ; 
  
  double energy=0.;  
  int nlo = std::max(m_ncetamax-1,0); 
  int nhi = std::min(m_ncetamax+1,m_sizearrayeta-1); 
  for(int ieta=nlo;ieta<=nhi;ieta++){
    energy   += m_enecell[ieta];
  }  
  m_e132   = energy;  

  energy=0.; 
  // define index of the array from max-7 strips (if possible)
  nlo = std::max(m_ncetamax-7,0); 
  // define index of the array from max+7 strips (if possible)
  nhi = std::min(m_ncetamax+7,m_sizearrayeta-1); 
  for(int ieta=nlo;ieta<=nhi;ieta++) energy   += m_enecell[ieta];
  
  m_e1152   = energy;  

  return;
}

// =====================================================================
void egammaStripsShape::setAsymmetry() 
{
  // 
  // Asymmetry of the shower in +/- 3 strips
  // (E(-1)-E(+1))/(E(-1)+E(+1))
  //

  if ( m_ncetamax < 0 || m_ncetamax > m_sizearrayeta ) return ; 
  double asl    = 0.;
  double asr    = 0.;

  // define index of the array from max-1 strips (if possible)
  int nlo = std::max(m_ncetamax-1,0); 
  // define index of the array from max+1 strips (if possible)
  int nhi = std::min(m_ncetamax+1,m_sizearrayeta-1); 
  for(int ieta=nlo;ieta<=m_ncetamax;ieta++) asl += m_enecell[ieta];
  for(int ieta=m_ncetamax;ieta<=nhi;ieta++) asr += m_enecell[ieta];
  
  if ( asl+asr > 0. ) m_asymmetrys3 = (asl-asr)/(asl+asr);

  return;
}  


// =====================================================================
void egammaStripsShape::setWs3(xAOD::CaloCluster::CaloSample sam) 
{
  //
  // Width in three strips centered on the strip with the largest energy
  // 

  double energy = 0.; 
  double width3 = 0.;
  double eta1   = 0.;
  // highest and lowest indexes
  int nlo = std::max(m_ncetamax-1,0); 
  int nhi = std::min(m_ncetamax+1,m_sizearrayeta-1); 
  for(int ieta=nlo;ieta<=nhi;ieta++){
    if (m_ncell[ieta] == 0) continue;
    width3  += m_enecell[ieta]*(ieta-m_ncetamax)*
      (ieta-m_ncetamax); 
    eta1    += m_enecell[ieta]*m_etacell[ieta]; 
    energy  += m_enecell[ieta];
  }    

  if(energy>0) {    
    if ( width3>0 ) m_ws3 = sqrt(width3/ energy);
    m_etas3   = eta1 / energy;
    // corrected width for position inside the cell
    // estimated from the first sampling
    m_ws3c = m_egammaqweta1c->Correct(m_cluster->etaSample(sam),m_cluster->etamax(sam),m_ws3); 
    m_poscs1 =  m_egammaqweta1c->RelPosition(m_cluster->etaSample(sam),m_cluster->etamax(sam));    
  } 

  return;
}

// =====================================================================
double egammaStripsShape::setDeltaEtaTrackShower(int nstrips,int ieta)
{
  //
  // Shower position 
  // using +/- "nstrips" strips centered on the strip ieta
  // this could be the hottest cell or the seed cell
  // 

  double energy = 0.; 
  double pos    = 0.;

  if ( ieta < 0 ) return  -9999.;
  
  // define index of the array from max-n strips strips (if possible)
  int nlo = std::max(ieta-nstrips,0); 
  // define index of the array from max+n strips strips (if possible)
  int nhi = std::min(ieta+nstrips,m_sizearrayeta-1); 

  // loop on all strips
  for(int i=nlo;i<=nhi;i++){
    // position in number of cells (wrt ieta) weighted by energy
    pos     += m_enecell[i]*(i-ieta); 
    // sum of energy
    energy  += m_enecell[i];
  }
  
  // delta eta value is defined only if total energy is positive
  if(energy>0) { 
    pos = pos / energy;    
    return pos;
  } 
  // NB list ost of cells in this code is in decreasing order compared to past
  pos *= -1;;
  
  return -9999.;
}

// =====================================================================
void egammaStripsShape::setWidths5()
{
  //
  // Shower width using 5 strips
  // 

  // threshold (tuned ?)
  double Small = 1.e-10; 
  double eall=0.; 
  double eave=0.;
  double width5=0.;
  
  // define index of the array from max-2 strips strips (if possible)
  int nlo = std::max(m_ncetamax-2,0); 
  // define index of the array from max+2 strips strips (if possible)
  int nhi = std::min(m_ncetamax+2,m_sizearrayeta-1); 
  // loop on all strips
  for( int ieta= nlo;ieta<=nhi;ieta++){ 
    if(ieta>=0 && ieta<m_sizearrayeta){ 
      // sum of energy
      eall +=m_enecell[ieta]; 
      // position in number of cells (wrt ncetamax) weighted by energy
      eave +=m_enecell[ieta]*(ieta-m_ncetamax);
      // width
      width5 +=m_enecell[ieta]*
	(ieta-m_ncetamax)*(ieta-m_ncetamax); 
    }
  }
  
  if(eall<Small) return; 
  eave=eave/eall; 
  width5=width5/eall; 
  width5=width5-eave*eave; 
  if(width5<0) return; 

  m_widths5 = sqrt(width5); 
  
  return;
}

// =====================================================================
void egammaStripsShape::setEmax()
{
  //
  // calculate energy of strip with maximum energy
  //

  for(int ieta=0; ieta<m_sizearrayeta; ieta++) {     
    if(m_enecell[ieta]>m_emaxs1){
      m_emaxs1 = m_enecell[ieta]; 
      m_ncetamax=ieta;
    } 
  } 
  
  return;
}

// =====================================================================
int egammaStripsShape::setEmax2()
{
  //
  // energy of the second local maximum (m_esec)
  // energy of the strip with second max 2 (m_esec1)
  //

  //int ncetasec=0;
  int ncetasec1=0;
  
  double ecand, ecand1; 
  double escalesec = 0;
  double escalesec1 = 0;
  
  for(int ieta=1; ieta<m_sizearrayeta-1; ieta++) { 
    if (m_ncell[ieta] == 0) continue;

    int ieta_left = ieta - 1;
    while (ieta_left >= 0 && m_ncell[ieta_left] == 0){
      --ieta_left;
    }
    if (ieta_left < 0) {continue;}

    int ieta_right = ieta + 1;
    while (ieta_right < m_sizearrayeta && m_ncell[ieta_right] ==0){
      ++ieta_right;
    }
    if (ieta_right >= m_sizearrayeta) {
      continue;
    }

    double e = m_enecell[ieta]/ m_gracell[ieta];
    double e_left  = m_enecell[ieta_left] / m_gracell[ieta_left];
    double e_right = m_enecell[ieta_right] / m_gracell[ieta_right];

    // check that cell is hotter than left or right one
    if(e > e_left && e > e_right) {
      // this ensure that this cell is not the hottest one
      // this + previous line implies that hottest cell and 2nd one
      // are separated by more than one strip
      if(ieta!=m_ncetamax){
	ecand = m_enecell[ieta]+m_enecell[ieta_left]+m_enecell[ieta_right]; 
        double escalecand = e + e_left + e_right;
	ecand1 = m_enecell[ieta];

	// test energy of the three strips
	if(escalecand>escalesec){
            escalesec = escalecand;
	    m_esec    = ecand; 
	    //ncetasec  = ieta; 
	}
	// test energy of 2nd hottest local maximum
	if(e>escalesec1){
          escalesec1 = e;
	  m_esec1    = ecand1; 
	  ncetasec1  = ieta; 
	}
      }
    }
  }
  
  return ncetasec1;
}

// =====================================================================
void egammaStripsShape::setEmin(int ncsec1) 
{
  //
  // energy deposit in the strip with the minimal value
  // between the first and the second maximum
  //
  m_emins1 = 0.; 
  // Divide by a number smaller than the eta-width of any cell.
  double escalemin = m_emaxs1 / 0.001;
  
  if ( ncsec1<=0  ) return;

  // define index of the array from max+1 strips strips (if possible)
  int nlo = std::min(m_ncetamax,ncsec1)+1; 
  // define index of the array from max-1 strips strips (if possible)
  int nhi = std::max(m_ncetamax,ncsec1)-1; 
  // loop on these strips
  for(int ieta=nlo;ieta<=nhi;ieta++){
    if (m_ncell[ieta] == 0) continue;
    // correct energy of the strips with its granularity
    // in order to be compared with other energy strips
    // (potentially in other regions of granularity)
    double escale = m_enecell[ieta] / m_gracell[ieta];
    if ( escale < escalemin){
      escalemin = escale;
      m_emins1  = m_enecell[ieta]; 
    } 
  }

  return;
} 

// =====================================================================
void egammaStripsShape::setValley() 
{
  //
  // Variable defined originally by Michal Seman
  // (was tuned on DC0 data ! never since !!!)
  // 

  double Ehsthr=0.06*GeV; 
  double val=0.;

  // search for second peak to the right   
  for(int ieta=m_ncetamax+2; ieta<m_sizearrayeta-1; ieta++) { 
    if(m_enecell[ieta]>Ehsthr && 
       m_enecell[ieta]>
       std::max(m_enecell[ieta-1],m_enecell[ieta+1])){
      double valley=0;
      for(int jc=m_ncetamax;jc<=ieta-1;jc++){
	valley+=dim(m_enecell[ieta],m_enecell[jc]); 
      }
      if(valley>val) val=valley; 
    }
  }

  // search for second peak to the left 
  for(int ieta=1; ieta<=m_ncetamax-2; ieta++) { 
    if(m_enecell[ieta]>Ehsthr && 
       m_enecell[ieta]>
       std::max(m_enecell[ieta-1],m_enecell[ieta+1])){
      double valley=0;
      for(int jc=ieta+1;jc<=m_ncetamax;jc++){
	valley+=dim(m_enecell[ieta],m_enecell[jc]); 
      }
      if(valley>val) val=valley; 
    }
  }

  // energy of hottest strip
  double e1 = m_emaxs1; 
  // energy of strip on the left
  double eleft = m_ncetamax > 0 ? m_enecell[m_ncetamax-1] : 0;
  // energy of strip on the right
  double eright = m_ncetamax < m_sizearrayeta-1 ? m_enecell[m_ncetamax+1] : 0;
  // find hottest of these strips
  double e2 = std::max(eleft,eright); 
  
  if ( fabs(e1+e2) > 0. ) m_val = val/(e1+e2); 

  return;
}

// =====================================================================
void egammaStripsShape::setFside()
{
  //
  // fraction of energy outside shower core 
  // (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips)
  // 
  // NB: threshold defined by M. Seman for DC0 data (or before ?), never tuned since
  double Ehsthr = 0.06*GeV; 
  // Local variable with max energy in strips
  double e1     = m_emaxs1; 
  // left index defined as max-1
  int ileft = m_ncetamax-1;
  while (ileft > 0 && m_ncell[ileft] == 0){
    --ileft;
  }
  double eleft  = ileft >= 0 ? m_enecell[ileft] : 0;

  // right index defined as max+1
  int iright = m_ncetamax+1;
  while (iright < m_sizearrayeta-1 && m_ncell[iright] == 0){
    ++iright;
  }
  double eright = iright < m_sizearrayeta ? m_enecell[iright] : 0;

  double fracm=0.;
  
  // define index of the array from max-3 strips strips (if possible)
  int nlo = std::max(m_ncetamax-3,0); 
  // define index of the array from max+3 strips strips (if possible)
  int nhi = std::min(m_ncetamax+3,m_sizearrayeta-1); 

  if(e1>Ehsthr) {
    for(int ieta=nlo;ieta<=nhi;ieta++){
      if (m_ncell[ieta] == 0) continue;
      fracm += (m_gracell[ieta] > DBL_MIN) ? (m_enecell[ieta] / m_gracell[ieta]) : 0.; 
    }
    if ( fabs(eleft+eright+e1) > 0. ) {
      if ((e1 != 0) && (m_gracell[m_ncetamax] > DBL_MIN))
        e1 /= m_gracell[m_ncetamax];
      if (eleft != 0)
        eleft /= m_gracell[ileft];
      if (eright != 0)
        eright /= m_gracell[iright];

      m_fside = (fabs(eleft+eright+e1) > 0.) ? fracm/(eleft+eright+e1) - 1. : 0.;
    }
  }

  return;
}

// =================================================================  
void egammaStripsShape::setF1core()
{ 
  //
  // Fraction of energy reconstructed in the core of the shower
  // core = e132, i.e energy in 3 strips
  //

  double x=-9999.;

  // energy in 3 strips in the 1st sampling
  double e132  = (m_e132>x) ? m_e132 : 0.;
  // total ennergy
  double energy = m_cluster->e();
  // build fraction only if both quantities are well defined
  if ( fabs(energy) > 0. && e132 > x ){
    m_f1core = e132/energy;
  }
  return;  
}

// ======================================================================
