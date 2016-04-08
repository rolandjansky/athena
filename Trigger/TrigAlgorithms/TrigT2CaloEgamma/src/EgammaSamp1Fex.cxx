/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaSamp1Fex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   M.P. Casado
// 
//   Energy Ratio calculation modifications by Steve Armstrong
//                                             8 May 2003
//
// ********************************************************************

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloEgamma/EgammaSamp1Fex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

#include "IRegionSelector/IRoiDescriptor.h"

inline double proxim(double b,double a){ return b+2.*M_PI*round((a-b)*(1./(2.*M_PI))) ;}


EgammaSamp1Fex::EgammaSamp1Fex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IAlgToolCalo(type, name, parent)
		   {
#ifndef NDEBUG
	// Create Geometry object
        // 0 -> CaloType EM, 1 -> First Layer
//        m_geometry[0] = new T2Geometry(0,1);
#endif
}

EgammaSamp1Fex::~EgammaSamp1Fex(){
}

StatusCode EgammaSamp1Fex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				   const IRoiDescriptor& roi ) { 
  
	// Time total AlgTool time 
	if (!m_timersvc.empty()) m_timer[0]->start();      
        // reset error
        m_error=0x0;

        bool cluster_in_barrel = true;
        if ( m_caloDDE )
          cluster_in_barrel = m_caloDDE->is_lar_em_barrel();

        ATH_MSG_DEBUG( "in execute(TrigEMCluster &)" );

        // Time to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->start();

        // Region Selector, sampling 1
	int sampling = 1;
        // Get detector offline ID's for Collections
        m_data->RegionSelector(sampling, roi );

        // Finished to access RegionSelector
        if (!m_timersvc.empty()) m_timer[1]->stop();
        // Time to access Collection (and ByteStreamCnv ROBs)
        if (!m_timersvc.empty()) m_timer[2]->start();

        if ( m_data->LoadCollections(m_iBegin,m_iEnd).isFailure() ){
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
	}
	m_error|=m_data->report_error();
/*
        if ( m_error ) {
                if (!m_timersvc.empty()) m_timer[2]->stop();
                return StatusCode::SUCCESS;
        }
*/
        if ( m_saveCells ){
           m_data->storeCells(m_iBegin,m_iEnd,*m_CaloCellContPoint,m_cellkeepthr);
        }
        // Finished to access Collection
        if (!m_timersvc.empty()) m_timer[2]->stop();
        // Algorithmic time
        if (!m_timersvc.empty()) m_timer[3]->start();


  double totalEnergy = 0;
  double etaEnergyS1 = 0;
  CaloSampling::CaloSample samp;  

  double energyEta = rtrigEmCluster.eta();
  double energyPhi = rtrigEmCluster.phi();
  if ( m_caloDDE ){
	energyEta = m_caloDDE->eta();
	energyPhi = m_caloDDE->phi();
  }


    
  // begin SRA mod: set regions via LAr way

  // Fix for 3x7 cluster size
  double z_etamin = energyEta-(0.075/2.);
  double z_etamax = energyEta+(0.075/2.);
  double z_phimin = energyPhi-(7.0*0.09817477/4.0)/2.0;
  double z_phimax = energyPhi+(7.0*0.09817477/4.0)/2.0;
  // Fix for 5x5 clusters
  if ( ! cluster_in_barrel ) {
   z_etamin = energyEta-(0.125/2.);
   z_etamax = energyEta+(0.125/2.);
   z_phimin = energyPhi-(5.0*0.09817477/4.0)/2.0;
   z_phimax = energyPhi+(5.0*0.09817477/4.0)/2.0;
  }
  
  double z_eta = z_etamin + (z_etamax - z_etamin)*0.5;
  double z_phi = z_phimin + (z_phimax - z_phimin)*0.5;

  // Make sure these boundaries are valid
  
  if (z_etamin < -2.4) z_etamin = -2.4 ; 
  if (z_etamin < -1.4 && z_etamin > -1.5) z_etamin = -1.4;
  if (z_etamin >  1.4 && z_etamin <  1.5) z_etamin =  1.5;
  
  if (z_etamax >  2.4) z_etamax = 2.4 ;
  if (z_etamax < -1.4 && z_etamax > -1.5) z_etamax = -1.5;
  if (z_etamax >  1.4 && z_etamax <  1.5) z_etamax =  1.4;

  bool signals = false;
  if ( (z_etamin < 0) && (z_etamax>0) ) signals = true;

  // identify region 
  double etareg[6] = {-2.4,-2.0,-1.8,1.8,2.0,2.4}; 
  double etagra[5] = {0.025/4,0.025/6,0.025/8,0.025/6,0.025/4}; 
  int imax = -9; 
  int icrk = 1 ; 
  double dgra = 0.; 
  // find the region, and check if the eta range cross region. 
  for(int ir=0;ir<5;ir++) {  
    // make sure you're not outside acceptance of 2.4
    if(fabs(z_eta)<2.4 && z_eta>=etareg[ir] && z_eta<=etareg[ir+1]) {
      dgra= etagra[ir];
      imax = ir; 
      if(z_etamin>=etareg[ir] && z_etamax<=etareg[ir+1]) icrk=0; 
    }
  }
  const double inv_dgra = dgra != 0 ? 1. / dgra : 1;

  // end SRA modification

      
  // begin SRA mod: pick the eta around which to scan
  int strip_border;
  double etanew;
  double aeta = fabs(z_eta); 
  if (aeta <= 1.4) {
    strip_border = (int)rint(aeta*inv_dgra);
    // this is -0.5 in atrecon: qgcshap.F ..   Makes more sense to be +0.5
    etanew = ((double)strip_border - 0.5)*dgra;
  }
  if (aeta <= 1.5) {
    etanew = -99.;
  }
  if (aeta <= 1.8) {
    strip_border = (int)rint(aeta*inv_dgra);
    // this is -0.5 in atrecon: qgcshap.F ..   Makes more sense to be +0.5
    etanew = ((double)strip_border - 0.5)*dgra;
  }
  else if (aeta <= 2.0) {
    strip_border = (int)rint((aeta-1.8)*inv_dgra);
    etanew = 1.8 + ((double)strip_border - 0.5)*dgra;
  }
  else if (aeta <= 2.4) {
    strip_border = (int)rint((aeta-2.0)*inv_dgra);
    etanew = 2.0 + ((double)strip_border - 0.5)*dgra;
  }
  else {
    etanew = -99.;
  }
  
  etanew = (z_eta < 0.) ? -etanew : etanew;


  // check border crossing. 
  int ibin = 0 ;
  double dgra1 = 0.; 
  int iadd = 0; 
  if((icrk==1) && (imax!=-9)){
    if(imax>0 && z_etamin>etareg[imax-1] && z_etamin<=etareg[imax]){
      dgra1 = etagra[imax-1]; 
      ibin=(int)rint((etareg[imax]+0.5*dgra-etanew)*inv_dgra)+20; 
      iadd=0; 
    } else{
      if(imax<4 && z_etamax>etareg[imax+1] && z_etamax<=etareg[imax+2]) {
	dgra1 = etagra[imax+1]; 
	ibin = (int)rint((etareg[imax+1]-0.5*dgra-etanew)*inv_dgra)+20; 
	iadd=1; 
      }
    }
    if ( imax==0 ) {
		dgra1 = etagra[0];
		ibin=(int)rint((etareg[0]+0.5*dgra-etanew)*inv_dgra)+20;
		iadd=1;
    }
    if ( imax==4 ) {
		 dgra1 = etagra[4];
		 ibin = (int)rint((etareg[5]-0.5*dgra-etanew)*inv_dgra)+20; 
		 iadd=0;
    }
  }
  const double inv_dgra1 = dgra1 != 0 ? 1. / dgra1 : 1;
  
  // end SRA mod

  double z_enecell[40];  //SRA
  double z_etacell[40];  //SRA
  //double z_phicell[40];  //SRA

  for(int iii=0;iii<40;iii++){
    z_enecell[iii]=0.;
    z_etacell[iii]=0.;
    //z_phicell[iii]=0.;
  }

  double deta = 0.;           // eta difference current cell - seed
  double dphi = 0.;           // phi difference current cell - seed
  int ncells = 0;

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {                                       // Should be revised for London scheme
    ncells++;
    const LArCell* larcell = (*m_it);
    double etaCell = larcell->caloDDE()->eta_raw();
    double phiCell = larcell->phi();
    double energyCell = larcell->energy();

    if ( fabs(etanew) != 99.) {

	double eta_cell = etaCell; //SRA
	//double eta_cell = larcell->eta(); //SRA
	int ieta;                          //SRA


	// begin SRA mod 
	if (eta_cell >= z_etamin && eta_cell <= z_etamax) {
	  // adjust for possible 2*pi offset. 
	  double phi_cell0=larcell->phi(); 
	  double phi_cell = proxim(phi_cell0,z_phi) ;
	  if (phi_cell >= z_phimin && phi_cell <= z_phimax) {
	    if(icrk == 0){
	      // single region
	      ieta = (int)rint((eta_cell-etanew)*inv_dgra)+20; 
	      // correction for eta=0 spacing
              if ( signals && eta_cell < 0 ) 
		ieta=(int)rint((eta_cell-etanew+0.007)*inv_dgra)+20;
	    } else {
	      if(eta_cell>etareg[imax] && eta_cell<etareg[imax+1])
		{ 
		  // same region 
		  ieta = (int)rint((eta_cell-etanew)*inv_dgra)+20; 
		} else{
		  // different region 
		  ieta =
		    (int)rint((eta_cell-etareg[imax+iadd]-(0.5-iadd)*dgra1)*inv_dgra1)+ibin;
		}
	    } //  icrk==0 
	    
	    double z_e = energyCell; 
	  
	    if(ieta>=1 && ieta<=40){
	      z_enecell[ieta-1] +=z_e ; 
	      z_etacell[ieta-1]  =eta_cell ; 
	      //z_phicell[ieta-1]  =phi_cell0 ; 
	    }        
	  } // phi range
	} // eta range 
	//end SRA mod
    } //endif for veto of clusters in crack outside acceptance
    
	// Find the standard em cluster energy (3*7 cell, now sampling 1)
	deta = fabs( etaCell - energyEta );
	dphi = fabs( phiCell - energyPhi );
	if ( dphi > M_PI ) dphi = 2.* M_PI - dphi;   // wrap 0 -> 6.28

	bool condition37 = cluster_in_barrel && ( deta <= 0.0375 && dphi <= 0.0875 );
	bool condition55 = (!cluster_in_barrel) && ( deta <= 0.0625 && dphi <= 0.0625 );

	if ( condition37 || condition55 ) {
	  totalEnergy += energyCell;
	  etaEnergyS1 += energyCell * etaCell;
	  //samp = CaloSampling::getSampling(*larcell);
	  samp = larcell->caloDDE()->getSampling();
	  rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp)
				+ energyCell);
	  rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp)
				+ energyCell);

	}



  } // end of loop over sampling 1

        // Finished lgorithmic time
        if (!m_timersvc.empty()) m_timer[3]->stop();
	// Started Save EMShowerMinimal time
        if (!m_timersvc.empty()) m_timer[4]->start();

  //SRA mod: Emax!
  double z_emax = -999.0;
    int z_ncmax = -999;
  for(int ic=0; ic<40; ic++) { 
    if(z_enecell[ic]>z_emax){
      z_emax = z_enecell[ic]; 
      z_ncmax=ic;
    }
  }

  // Include now setWstot
  double wtot = 0.;
  double etot = 0.;
  double wstot = -9999.;
  double wstot_deta = (z_etamax-z_etamin)*0.5;
  double eta_center = z_etacell[z_ncmax];
  if ( getCaloDetDescrElement() != 0 ) {
    eta_center = getCaloDetDescrElement()->eta();
    wstot_deta = getCaloDetDescrElement()->deta()*2.5;
  }
  
  for(int ic=0; ic<40; ic++) { 
    if( (z_etacell[ic]>=(eta_center-wstot_deta)) && (z_etacell[ic]<=(eta_center+wstot_deta))  ){
      wtot += z_enecell[ic] * (ic - z_ncmax) * (ic - z_ncmax);
      etot += z_enecell[ic] ;
    }
  }
  if ( etot>0 ) {
   wtot = wtot/etot;
   wstot = wtot> 0 ? sqrt(wtot) : -9999.;
  }

  //SRA mod: Emax2!
  //int z_ncsec1=0;
  double z_esec1=-999.0;  // energy of strip with second max 2

  double ecand1; 
  
  for(int iic=1; iic<39; iic++) { 
    if( (z_enecell[iic]>z_enecell[iic-1]) && (z_enecell[iic]>z_enecell[iic+1]) ){
      if(iic!=z_ncmax){
	ecand1 = z_enecell[iic];
	if(ecand1>z_esec1){
	  z_esec1=ecand1; 
	  //z_ncsec1=iic; 
	}
      }
    }
  }


  if ((z_emax + z_esec1) < 0.) z_emax = -(100./98.)* z_esec1;

  // do frac73 calculation: sum energies +-3 strips
  // and +-1 strip around highest energy strip
  double strip7 = 0.;  // temporary variables to sum energy
  double strip3 = 0.;
  double frac73  = 0.;  // (E7-E3)/E3 in layer 1

  // use Steve's array to calculate frac73    MW
  //
  int smax = z_ncmax  ;  
  if ( z_ncmax>2 && z_ncmax<37 ){
    strip7 = z_enecell[smax-3]+z_enecell[smax-2]+z_enecell[smax-1]
      +z_enecell[smax+1]+z_enecell[smax+2]+z_enecell[smax+3]
      +z_enecell[smax];
    strip3 = z_enecell[smax-1]+z_enecell[smax]+z_enecell[smax+1];
  } else if (z_ncmax==2){
    strip7 = z_enecell[smax-2]+z_enecell[smax-1]
      +z_enecell[smax+1]+z_enecell[smax+2]+z_enecell[smax+3]
      +z_enecell[smax];
    strip3 = z_enecell[smax-1]+z_enecell[smax]+z_enecell[smax+1];
  } else if (z_ncmax==37){
    strip7 = z_enecell[smax-3]+z_enecell[smax-2]+z_enecell[smax-1]
      +z_enecell[smax+1]+z_enecell[smax+2]
      +z_enecell[smax];
    strip3 = z_enecell[smax-1]+z_enecell[smax]+z_enecell[smax+1];
  } else if (z_ncmax==1){
    strip7 = z_enecell[smax-1]
      +z_enecell[smax+1]+z_enecell[smax+2]+z_enecell[smax+3]
      +z_enecell[smax];
    strip3 = z_enecell[smax-1]+z_enecell[smax]+z_enecell[smax+1];
  } else if (z_ncmax==38){
    strip7 = z_enecell[smax-3]+z_enecell[smax-2]+z_enecell[smax-1]
      +z_enecell[smax+1]
      +z_enecell[smax];
    strip3 = z_enecell[smax-1]+z_enecell[smax]+z_enecell[smax+1];
  }
  strip3 > 1.e-6 ?             // protect against small values of strip3
    frac73 = (strip7 - strip3)/strip3:
    frac73 = 99.;  
    
  // Update Cluster Variables
  
  rtrigEmCluster.setNCells(ncells+rtrigEmCluster.nCells());
  rtrigEmCluster.setEmaxs1(z_emax);
  rtrigEmCluster.setWstot(wstot);
  rtrigEmCluster.setE2tsts1(z_esec1);
  rtrigEmCluster.setFracs1(frac73);
  if ( totalEnergy != 0.0 ) 
     rtrigEmCluster.setEta1( etaEnergyS1/totalEnergy );
  else
     rtrigEmCluster.setEta1( 99.0 );
  rtrigEmCluster.setRawEnergy( rtrigEmCluster.rawEnergy() + totalEnergy);

#ifndef NDEBUG
  if ( msg().level() <= MSG::DEBUG ) {
        if ( m_geometryTool->EtaPhiRange(0,1,energyEta, energyPhi))
                msg() << MSG::ERROR << "problems with EtaPhiRange" << endreq;
        PrintCluster(totalEnergy,0,1,CaloSampling::EMB1,CaloSampling::EME1);
  }
#endif
        
	// Finish Save EMShowerMinimal time
        if (!m_timersvc.empty()) m_timer[4]->stop();
  // Time total AlgTool time 
  if (!m_timersvc.empty()) m_timer[0]->stop();      

  return StatusCode::SUCCESS;
}

