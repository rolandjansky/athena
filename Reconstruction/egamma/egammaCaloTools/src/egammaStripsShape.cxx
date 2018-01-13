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

namespace {
    const int STRIP_ARRAY_SIZE =40;
}

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
: AthAlgTool(type, name, parent)
{ 
    // declare Interface
    declareInterface<IegammaStripsShape>(this);

}

egammaStripsShape::~egammaStripsShape(){ 
}

StatusCode egammaStripsShape::initialize(){
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

StatusCode egammaStripsShape::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode egammaStripsShape::execute(const xAOD::CaloCluster& cluster, 
        const CaloCellContainer& cell_container, Info& info) const  {
    //
    // Estimate shower shapes from first compartment
    // based on hottest cell in 2nd sampling , the  deta,dphi,
    // And the barycenter in the 1st sampling (seed) 
    //
    ATH_MSG_DEBUG(" egammaStripsShape: execute");

    // check if cluster is in barrel or in the end-cap
    if(!cluster->inBarrel() && !cluster->inEndcap() ) { 
        ATH_MSG_WARNING(" egammaStripsShape: Cluster is neither in Barrel nor in Endcap, cannot calculate ShowerShape ");
        return StatusCode::SUCCESS;
    }

    // retrieve energy in all samplings
    StatusCode sc = m_egammaEnergyPositionAllSamples->execute(cluster);
    sc.ignore(); // should we check for errors?

    double eallsamples = m_egammaEnergyPositionAllSamples->e();
    // retrieve energy in 1st sampling
    double e1 = m_egammaEnergyPositionAllSamples->e1();

    //check if cluster is in barrel or end-cap
    // sam is used in SetArray to check that cells belong to strips
    // samgran is used to estimate the window to use cells in eta
    // it is based on the granularity of the middle layer
    // For phi we use the strip layer granularity  
    bool in_barrel =  m_egammaEnergyPositionAllSamples->inBarrel();
    CaloSampling::CaloSample sam=CaloSampling::EMB1;
    CaloSampling::CaloSample samgran=CaloSampling::EMB2;
    CaloSampling::CaloSample offset=CaloSampling::PreSamplerB;
    CaloCell_ID::SUBCALO subcalo= CaloCell_ID::LAREM;
    if (in_barrel) {
        sam     = CaloSampling::EMB1;
        samgran = CaloSampling::EMB2;
        offset  = CaloSampling::PreSamplerB;
    } else {
        sam     = CaloSampling::EME1;
        samgran = CaloSampling::EME2;
        offset  = CaloSampling::PreSamplerE; 
    }
    // get eta and phi of the seed
    info.etaseed = cluster.etaSample(sam);  
    info.phiseed = cluster.phiSample(sam);
    // get eta and phi of the hottest cell in the second sampling
    info.etamax = cluster.etamax(samgran);  
    info.phimax = cluster.phimax(samgran);

    // possible for soft electrons to be at -999
    if ( (info.etamax==0. && info.phimax==0.) || fabs(info.etamax) > 100. ){ 
        return StatusCode::SUCCESS;
    }
    // check if we are in a crack or outside area where strips are well defined
    if (fabs(info.etamax) > 2.4) {
        return StatusCode::SUCCESS
    };
    if (fabs(info.etamax) > 1.4 && fabs(m_etamax) < 1.5) {
        return StatusCode::SUCCESS;
    }
    // CaloCellList needs both enums: subCalo and CaloSample
    // use samgran = granularity in second sampling for eta !!!!
    int sampling_or_module=0;
    m_calo_dd->decode_sample(subcalo, barrel, sampling_or_module, 
            (CaloCell_ID::CaloSample) samgran);
    const CaloDetDescrElement* dde = m_calo_dd->get_element(subcalo,sampling_or_module,barrel,
            info.etamax ,info.phimax);
    // if no object then exit
    if (!dde) {
        return StatusCode::SUCCESS;   
    };
    // width in eta is granularity (dde->deta()) times number of cells (m_neta)
    deta = dde->deta()*m_neta/2.0;
    // use samgran = granularity in first sampling for phi !!!!
    m_calo_dd->decode_sample(subcalo, barrel, sampling_or_module, 
            (CaloCell_ID::CaloSample) sam);
    dde = m_calo_dd->get_element(subcalo, sampling_or_module, barrel,
            info.etamax ,info.phimax);
    // if no object then exit
    if (!dde) {
        return StatusCode::SUCCESS;
    }
    // width in phi is granularity (dde->dphi()) times number of cells (m_nphi)
    dphi = dde->dphi()*m_nphi/2.0;


    /* initialize the arrays*/ 
    double enecell[STRIP_ARRAY_SIZE];       
    double etacell[STRIP_ARRAY_SIZE]; 
    double gracell[STRIP_ARRAY_SIZE]; 
    int ncell[STRIP_ARRAY_SIZE]; 
    std::fill (enecell,     enecell+STRIP_ARRAY_SIZE,     0);
    std::fill (etacell,     etacell+STRIP_ARRAY_SIZE,     0);
    std::fill (gracell,     gracell+STRIP_ARRAY_SIZE,     0);
    std::fill (ncell,       ncell+STRIP_ARRAY_SIZE,       0);
    // Fill the array in energy and eta from which all relevant
    // quantities are estimated
    setArray(cluster,sam,info.etamax,info.phimax,deta,dphi,
            enecell,etacell,gracell,ncell,cluster);


    /* Fill the the rest of the shapes*/
    // find the corresponding index of the seed
    setIndexSeed(info,etacell,gracell);
    // calculate fraction of energy in strips
    info.f1 = fabs(eallsamples) > 0. ? e1/eallsamples : 0.;
    // calculate energy and bin where the energy strip is maximum
    setEmax(info,enecell);
    // calculate total width 
    setWstot(info,deta);
    // width in three strips
    setWs3(info,sam,cluster,enecell,etacell);
    // Energy in in +/-1 and in +/-7 strips
    if (m_ExecAllVariables) {
        setEnergy(info,enecell);
        setF1core(info,cluster);
    }

    if (m_ExecAllVariables && m_ExecOtherVariables){
        setAsymmetry(info);
        info.deltaEtaTrackShower = setDeltaEtaTrackShower(1,info.ncetamax,info);
        // Using strips centered on the seed cell
        // position in eta from +/- 7 strips 
        info.deltaEtaTrackShower7 = setDeltaEtaTrackShower(7,info.ncetaseed,info);
        // calculate the fraction of energy int the two highest energy strips
        setF2(info);
        // Shower width in 5 strips around the highest energy strips
        setWidths5(info);
        // calculate energy of the second local maximum
        int ncsec1 =  setEmax2(info);
        // calculate the energy of the strip with the minimum energy
        setEmin(ncsec1,info);
        // calculate M.S's valley
        setValleyi(info);
        // calculate M.S's fraction
        setFside(info);
        info.success = true; 
    }
    return  StatusCode::SUCCESS; 
}

void setArray(const xAOD::CaloCluster& cluster ,CaloSampling::CaloSample sam,
        double eta, double phi,  double deta, double dphi,
        double* enecell, double* etacell, double* gracell,
        int* ncell) const {
    //
    // Put in an array the energies,eta,phi values contained in 
    // a window (deta,dphi) 

    StripArrayHelper stripArray[STRIP_ARRAY_SIZE*3];
    //Raw --> Calo Frame 
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
    int indexay = 0;
    double eta_cell  = 0.;
    double phi_cell0 = 0.;
    double phi_cell  = 0.;

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
                    if (indexay<BIG_STRIP_ARRAY_SIZE) {
                        // energy
                        stripArray[indexay].energy = theCell->energy()*(first.weight()); 
                        // eta 
                        stripArray[indexay].eta  = theCell->eta();
                        // eta raw
                        stripArray[indexay].etaraw = theCell->caloDDE()->eta_raw();
                        // eta granularity
                        stripArray[indexay].deta  = theCell->caloDDE()->deta();
                        // index/number of cells in the array
                        stripArray[indexay].ncell++;
                        // increase index 
                        indexay++;
                    }
                }	  
            }
        }
    }
    // Exit early if no cells.
    if (indexay == 0){
        return;
    }
    // sort intermediate array with eta
    std::sort(stripArray,stripArray+indexay);

    // loop on intermediate array and merge two cells in phi (when they exist)
    int ieta = 0;
    bool next = false;
    // start loop on 2nd element
    for (int i=0;i<indexay-1;i++) {
        // protection against too big array
        if (ieta<STRIP_ARRAY_SIZE) {
            // energy
            if (enecell) enecell[ieta] += stripArray[i].energy;
            // eta 
            if (etacell) etacell[ieta] = stripArray[i].eta;
            // eta granularity
            if (gracell) gracell[ieta] = stripArray[i].deta;
            // index/number of cells in the array
            if (ncell) ++ncell[ieta];
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
    int index = indexay-1;
    // if previous element had a different eta then append the array
    // NB: this could happen if only one cell in phi was available
    if (index == 0 ||
            fabs(stripArray[index].etaraw-stripArray[index-1].etaraw)>0.00001){
        // energy
        if (enecell) enecell[ieta] = stripArray[index].energy;
    }
    if (index != 0 &&
            fabs(stripArray[index].etaraw-stripArray[index-1].etaraw)<0.00001){
        // energy
        if (enecell) {enecell[ieta] += stripArray[index].energy;}
    }
    // eta 
    if (etacell) {etacell[ieta] = stripArray[index].eta;}
    // eta granularity
    if (gracell) {gracell[ieta] = stripArray[index].deta;}
    // index/number of cells in the array
    if (ncell) {++ncell[ieta];}

    return;
}

// =====================================================================
void egammaStripsShape::setIndexSeed(Info& info,
        double* etacell,
        double* gracell) const {  
    //
    // Look for the index of the seed in the array previously filled
    //
    double demi_eta = 0.;
    double eta_min = 0.;
    double eta_max = 0.;
    for(int ieta=0; ieta<STRIP_ARRAY_SIZE-1; ieta++) {     
        // from the eta of the cell it is necessary 
        // to have the boundaries of this cell +/- half of its granularity
        demi_eta = gracell[ieta]/2.;
        eta_min = etacell[ieta]-demi_eta;
        eta_max = etacell[ieta]+demi_eta;
        // Beware that list is arranged from larger values to smaller ones
        if ((fabs(info.etaseed)>fabs(eta_min) && fabs(info.etaseed)<=fabs(eta_max)) ||
                (fabs(info.etaseed)<=fabs(eta_min) && fabs(info.etaseed)>fabs(eta_max)))
            info.ncetaseed = ieta;
    }
}

// =====================================================================
void egammaStripsShape::setWstot(Info& info, double deta) const {
    //
    // calculate width in half the region (that's the one used for e-ID)
    //
    // We take only half of the region in eta not in phi
    double etamin = info.etamax - deta/2.;
    double etamax = info.etamax + deta/2.;
    double mean = 0.; 
    double wtot = 0.;
    double etot = 0.;
    // loop on each element of the array
    for(int ieta=0; ieta<STRIP_ARRAY_SIZE; ieta++) { 
        if (ncell[ieta] == 0) continue;
        if (etacell[ieta] >= etamin && 
                etacell[ieta] <= etamax) {
            wtot  += enecell[ieta]*(ieta-info.ncetamax)*(ieta-info.ncetamax); 
            mean  += enecell[ieta]*(ieta-info.ncetamax); 
            etot  += enecell[ieta]; 
        }
    }
    // width is defined only if total energy is positive
    if( etot>0 ) {
        info.etot = etot;
        wtot = wtot/etot;
        mean = mean/etot; 
        info.wstot  = wtot >0 ? sqrt(wtot) : -9999.; 
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
    double eleft  = m_ncetamax > 0 ? enecell[m_ncetamax-1] : 0;
    // strip on right of highest energetic strip
    double eright = m_ncetamax < STRIP_ARRAY_SIZE-1 ? enecell[m_ncetamax+1] : 0;
    // find hottest of these strips
    double e2     = std::max(eleft,eright); 

    // calculate fraction of the two highest energy strips
    m_f2 = eallsamples > 0. ? (e1+e2)/eallsamples : 0.;  

    return;
}

// =====================================================================
void egammaStripsShape::setEnergy(Info& info , double enecell){
    // 
    // Energy in the strips in a cluster of 15 strips
    // and in a cluster of 3 strips - two cells are merge in phi
    //
    if ( info.ncetamax < 0 || info.ncetamax > STRIP_ARRAY_SIZE ) return ; 
    double energy=0.;  
    int nlo = std::max(info.ncetamax-1,0); 
    int nhi = std::min(info.ncetamax+1,STRIP_ARRAY_SIZE-1); 
    for(int ieta=nlo;ieta<=nhi;ieta++){
        energy   += enecell[ieta];
    }  
    info.e132   = energy;  

    energy=0.; 
    nlo = std::max(info.ncetamax-7,0); 
    nhi = std::min(info.ncetamax+7,STRIP_ARRAY_SIZE-1); 
    for(int ieta=nlo;ieta<=nhi;ieta++) {
        energy   += enecell[ieta];
    }
    info.e1152   = energy;  
    return;
}

// =====================================================================
void egammaStripsShape::setAsymmetry() {
    // 
    // Asymmetry of the shower in +/- 3 strips
    // (E(-1)-E(+1))/(E(-1)+E(+1))
    //
    if ( m_ncetamax < 0 || m_ncetamax > STRIP_ARRAY_SIZE ) return ; 
    double asl    = 0.;
    double asr    = 0.;

    // define index of the array from max-1 strips (if possible)
    int nlo = std::max(m_ncetamax-1,0); 
    // define index of the array from max+1 strips (if possible)
    int nhi = std::min(m_ncetamax+1,STRIP_ARRAY_SIZE-1); 
    for(int ieta=nlo;ieta<=m_ncetamax;ieta++) asl += enecell[ieta];
    for(int ieta=m_ncetamax;ieta<=nhi;ieta++) asr += enecell[ieta];

    if ( asl+asr > 0. ) m_asymmetrys3 = (asl-asr)/(asl+asr);

    return;
}  


// =====================================================================
void egammaStripsShape::setWs3(Info& info, 
        const xAOD::CaloCluster::CaloSample sam, const xAOD::CaloCluster& cluster 
        double enecell, doube etacell) const {
    //
    // Width in three strips centered on the strip with the largest energy
    // 

    double energy = 0.; 
    double width3 = 0.;
    double eta1   = 0.;
    // highest and lowest indexes
    int nlo = std::max(info.ncetamax-1,0); 
    int nhi = std::min(info.ncetamax+1,STRIP_ARRAY_SIZE-1); 
    for(int ieta=nlo;ieta<=nhi;ieta++){
        if (ncell[ieta] == 0) continue;
        width3  += enecell[ieta]*(ieta-info.ncetamax)*(ieta-info.ncetamax); 
        eta1    += enecell[ieta]*etacell[ieta]; 
        energy  += enecell[ieta];
    }    

    if(energy>0) {    
        if ( width3>0 ) {
            info.ws3 = sqrt(width3/ energy);
        }
        info.etas3   = eta1 / energy;
        // corrected width for position inside the cell
        // estimated from the first sampling
        info.ws3c = m_egammaqweta1c->Correct(cluster->etaSample(sam),cluster->etamax(sam),info.ws3); 
        info.poscs1 =  m_egammaqweta1c->RelPosition(cluster->etaSample(sam),cluster->etamax(sam));    
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
    int nhi = std::min(ieta+nstrips,STRIP_ARRAY_SIZE-1); 

    // loop on all strips
    for(int i=nlo;i<=nhi;i++){
        // position in number of cells (wrt ieta) weighted by energy
        pos     += enecell[i]*(i-ieta); 
        // sum of energy
        energy  += enecell[i];
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
    int nhi = std::min(m_ncetamax+2,STRIP_ARRAY_SIZE-1); 
    // loop on all strips
    for( int ieta= nlo;ieta<=nhi;ieta++){ 
        if(ieta>=0 && ieta<STRIP_ARRAY_SIZE){ 
            // sum of energy
            eall +=enecell[ieta]; 
            // position in number of cells (wrt ncetamax) weighted by energy
            eave +=enecell[ieta]*(ieta-m_ncetamax);
            // width
            width5 +=enecell[ieta]*
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
void egammaStripsShape::setEmax(Info& info, double* enecell) const {{
    //
    // calculate energy of strip with maximum energy
    //
    for(int ieta=0; ieta<STRIP_ARRAY_SIZE; ieta++) {     
        if(enecell[ieta]>info.emaxs1){
            info.emaxs1 = enecell[ieta]; 
            info.ncetamax=ieta;
        } 
    } 
    return;
}

// =====================================================================
int egammaStripsShape::setEmax2(){
    //
    // energy of the second local maximum (m_esec)
    // energy of the strip with second max 2 (m_esec1)
    //
    //int ncetasec=0;
    int ncetasec1=0;
    double ecand, ecand1; 
    double escalesec = 0;
    double escalesec1 = 0;

    for(int ieta=1; ieta<STRIP_ARRAY_SIZE-1; ieta++) { 
        if (ncell[ieta] == 0) continue;

        int ieta_left = ieta - 1;
        while (ieta_left >= 0 && ncell[ieta_left] == 0){
            --ieta_left;
        }
        if (ieta_left < 0) {continue;}

        int ieta_right = ieta + 1;
        while (ieta_right < STRIP_ARRAY_SIZE && ncell[ieta_right] ==0){
            ++ieta_right;
        }
        if (ieta_right >= STRIP_ARRAY_SIZE) {
            continue;
        }

        double e = enecell[ieta]/ gracell[ieta];
        double e_left  = enecell[ieta_left] / gracell[ieta_left];
        double e_right = enecell[ieta_right] / gracell[ieta_right];

        // check that cell is hotter than left or right one
        if(e > e_left && e > e_right) {
            // this ensure that this cell is not the hottest one
            // this + previous line implies that hottest cell and 2nd one
            // are separated by more than one strip
            if(ieta!=m_ncetamax){
                ecand = enecell[ieta]+enecell[ieta_left]+enecell[ieta_right]; 
                double escalecand = e + e_left + e_right;
                ecand1 = enecell[ieta];

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
        if (ncell[ieta] == 0) continue;
        // correct energy of the strips with its granularity
        // in order to be compared with other energy strips
        // (potentially in other regions of granularity)
        double escale = enecell[ieta] / gracell[ieta];
        if ( escale < escalemin){
            escalemin = escale;
            m_emins1  = enecell[ieta]; 
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
    for(int ieta=m_ncetamax+2; ieta<STRIP_ARRAY_SIZE-1; ieta++) { 
        if(enecell[ieta]>Ehsthr && 
                enecell[ieta]>
                std::max(enecell[ieta-1],enecell[ieta+1])){
            double valley=0;
            for(int jc=m_ncetamax;jc<=ieta-1;jc++){
                valley+=dim(enecell[ieta],enecell[jc]); 
            }
            if(valley>val) val=valley; 
        }
    }

    // search for second peak to the left 
    for(int ieta=1; ieta<=m_ncetamax-2; ieta++) { 
        if(enecell[ieta]>Ehsthr && 
                enecell[ieta]>
                std::max(enecell[ieta-1],enecell[ieta+1])){
            double valley=0;
            for(int jc=ieta+1;jc<=m_ncetamax;jc++){
                valley+=dim(enecell[ieta],enecell[jc]); 
            }
            if(valley>val) val=valley; 
        }
    }

    // energy of hottest strip
    double e1 = m_emaxs1; 
    // energy of strip on the left
    double eleft = m_ncetamax > 0 ? enecell[m_ncetamax-1] : 0;
    // energy of strip on the right
    double eright = m_ncetamax < STRIP_ARRAY_SIZE-1 ? enecell[m_ncetamax+1] : 0;
    // find hottest of these strips
    double e2 = std::max(eleft,eright); 

    if ( fabs(e1+e2) > 0. ) m_val = val/(e1+e2); 

    return;
}

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
    while (ileft > 0 && ncell[ileft] == 0){
        --ileft;
    }
    double eleft  = ileft >= 0 ? enecell[ileft] : 0;

    // right index defined as max+1
    int iright = m_ncetamax+1;
    while (iright < STRIP_ARRAY_SIZE-1 && ncell[iright] == 0){
        ++iright;
    }
    double eright = iright < STRIP_ARRAY_SIZE ? enecell[iright] : 0;

    double fracm=0.;

    // define index of the array from max-3 strips strips (if possible)
    int nlo = std::max(m_ncetamax-3,0); 
    // define index of the array from max+3 strips strips (if possible)
    int nhi = std::min(m_ncetamax+3,STRIP_ARRAY_SIZE-1); 

    if(e1>Ehsthr) {
        for(int ieta=nlo;ieta<=nhi;ieta++){
            if (ncell[ieta] == 0) continue;
            fracm += (gracell[ieta] > DBL_MIN) ? (enecell[ieta] / gracell[ieta]) : 0.; 
        }
        if ( fabs(eleft+eright+e1) > 0. ) {
            if ((e1 != 0) && (gracell[m_ncetamax] > DBL_MIN))
                e1 /= gracell[m_ncetamax];
            if (eleft != 0)
                eleft /= gracell[ileft];
            if (eright != 0)
                eright /= gracell[iright];

            m_fside = (fabs(eleft+eright+e1) > 0.) ? fracm/(eleft+eright+e1) - 1. : 0.;
        }
    }

    return;
}

void egammaStripsShape::setF1core(Info& info, const xAOD::CaloCluster& cluster) const { 
    // Fraction of energy reconstructed in the core of the shower
    // core = e132, i.e energy in 3 strips
    //
    double x=-9999.;
    // energy in 3 strips in the 1st sampling
    double e132  = (info.e132>x) ? info.e132 : 0.;
    // total ennergy
    double energy = cluster->e();
    // build fraction only if both quantities are well defined
    if ( fabs(energy) > 0. && e132 > x ){
        info.f1core = e132/energy;
    }
    return;  
}
