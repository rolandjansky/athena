/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

 
/********************************************************************

NAME:     CaloClusterBuilderSW.h 
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma 
CREATED:  Apr 2001

Orig:     LArCalorimeter/LArClusterRec/LArSlidingWindow

PURPOSE:  Build calorimeter cluster with Sliding Window algorithm

          Input:  Calorimeter towers
          Output: Cluster in a container 
          Parameters: 
	   primary window size in eta,phi for finding maximum
	   secondary window size for calculating positions. 
	   et threshold          

	   Assumes that the TowerBuilder has been run before the Clustering. 
	   Will find Clusters with SlidingWindowFinder.	   

Updated:  May 2001, use StoreGate, change Gaudi headers (SR)
Updated:  April 2002, repackaging (SR)

Updated:  April 2, 2004 (PLoch)
          New Calo EDM adapted

Updated:  April 27, 2004 (SMenke)
          New CaloClusterMakerTool base class

Updated:  Sep 6, 2005    (DLelas)
          Modifications needed for proper filling of LAr clusters
          -Cells are put in the clusters according to a flag steered
           by a jobOption.
          -Combined clustering is unchanged
          -LAr clustering: Only seeds and zero cells are put into clusters. 
           LAr cells are put later in CaloFillRectangularCluster, a 
           derivative of CaloComputeEMlayers

Updated:  Feb 09, 2005 (DLelas)
          New functionality added:
          - it is now possible to steer independently
            cluster-seed finding, checks for duplicate seeds 
            and cell filling 

Updated: Aug 25, 2014 (Evgenii Baldin)
         Error fix:
         - taking into account phi ``lopped bounds'' during cluster
           energy and coordinates calculation

********************************************************************/
// INCLUDE HEADER FILES:
#include "CaloClusterBuilderSW.h"

#include "CaloUtils/CaloClusterStoreHelper.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
// towers 
#include "CaloEvent/CaloTowerContainer.h"

// sliding window algorithm
#include "CaloUtils/SlidingWindowFinder.h"

// #include "LArDetDescr/LArID.h"

// Athena related 
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>


using CLHEP::GeV;

namespace Calo_Detail{ 
// comparison,  order cluster by Et
 class gt_cluster_et
 {

  public:
  inline   gt_cluster_et () {} ;
  inline   bool operator () (CaloClusterBuilderSW::ClusterWithCenter& c1, 
			CaloClusterBuilderSW::ClusterWithCenter& c2)
  {
    return c1.et > c2.et ;
  }  
  
 };
}

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
CaloClusterBuilderSW::CaloClusterBuilderSW(const std::string& type, 
					   const std::string& name,
					   const IInterface* parent)
  
  : AthAlgTool(type, name, parent),
    m_neta(5), m_nphi(5), m_netap(-1), m_nphip(-1), 
    m_ethreshold(5.0 * GeV ),
    m_towerContainerKey(""),
    m_cellContainerKey("AllCalo"),
    m_FillClusterCells(true),
    m_nextra(0),
    m_etaSeedGrid(5),m_phiSeedGrid(5),m_etaDuplicate(5),m_phiDuplicate(5)
{
  declareInterface<CaloClusterCollectionProcessor> (this);
// Read the following properties from jobOptions File :
// (If not found in jobOptions, the defaults are as given above)

  declareProperty("TowerContainer",m_towerContainerKey);
  declareProperty("CaloCellContainer",m_cellContainerKey);
  declareProperty("eta_size",m_neta);
  declareProperty("phi_size",m_nphi);
  declareProperty("eta_sizep",m_netap);
  declareProperty("phi_sizep",m_nphip);
  declareProperty("e_threshold",m_ethreshold);
  declareProperty("FillClusterCells",m_FillClusterCells);
  declareProperty("nextra",m_nextra);
  declareProperty("eta_SeedGrid",m_etaSeedGrid);
  declareProperty("phi_SeedGrid",m_phiSeedGrid);
  declareProperty("eta_Duplicate",m_etaDuplicate);
  declareProperty("phi_Duplicate",m_phiDuplicate);


}

// INITIALIZE:
StatusCode CaloClusterBuilderSW::initialize()
{ 

  if(m_netap < 0 ) m_netap = m_neta; 
  if(m_nphip < 0 ) m_nphip = m_nphi; 
  ATH_CHECK(m_towerContainerKey.initialize());
  ATH_CHECK(m_cellContainerKey.initialize());
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode
CaloClusterBuilderSW::execute(const EventContext& ctx,
                              xAOD::CaloClusterContainer* clusColl) const
{  
  ATH_MSG_DEBUG("Executing CaloClusterBuilderSW");

  // Get the tower collection from TDS
  SG::ReadHandle<CaloTowerContainer> towerContainer(m_towerContainerKey, ctx);

  if (!towerContainer.isValid()) {
    ATH_MSG_ERROR( "Could not get towerContainer " << towerContainer.name() );
    return StatusCode::FAILURE;
  }

  if((!m_FillClusterCells) && m_nextra>0){
    ATH_MSG_WARNING( "FillClusterCells = FALSE && nextra>0. nextra is meaningless quantity!!!" );
  }
  
  ATH_MSG_DEBUG("Tower container size " << towerContainer->size());

  const CaloTowerSeg& towerSeg = towerContainer->towerseg();
  ATH_MSG_DEBUG("retrieved tower segmentation from CaloTowerContainer @"
		<< towerContainer.cptr() 
		<< ", stored in object @" 
		<< &towerSeg
		<< " with "
		<< towerSeg.neta() << " eta bins and "
		<< towerSeg.nphi() << " phi bins.");
	      
  /*clusColl->setTowerSeg(towerSeg);  
  ATH_MSG_DEBUG("retrieved tower segmentation from CaloClusterContainer "
		<< "@"
		<< clusColl
		<< ", stored in object @" 
		<< &(clusColl->getTowerSeg())
		<< " with "
		<< clusColl->getTowerSeg().neta() << " eta bins and "
		<< clusColl->getTowerSeg().nphi() << " phi bins.");
  */
		
  SG::ReadHandle<CaloCellContainer> cellCont(m_cellContainerKey, ctx);
  if (!cellCont.isValid()) {
    ATH_MSG_ERROR( "Failed to retrieve CaloCellContainer with key " << m_cellContainerKey.key()  );
    return StatusCode::FAILURE;
  }

  // get a sliding window cluster size, window size=(Neta, Nphi), threshold 
  int eta_size=(m_neta-1)/2;
  int phi_size=(m_nphi-1)/2; 
  
    // get a sliding seed-finder grid size
  int eta_SeedGrid=(m_etaSeedGrid-1)/2;
  int phi_SeedGrid=(m_phiSeedGrid-1)/2;

  // get a check seed duplicate grid size
  int eta_Duplicate=(m_etaDuplicate-1)/2;
  int phi_Duplicate=(m_phiDuplicate-1)/2;

  // tower grid min bounds
  double etaMin=towerContainer->etamin();
  double phiMin=towerContainer->phimin();

  // tower grid phi size 
  int nGridPhi=towerContainer->nphi();
  // tower grid step size
  double stepSizeGridEta=towerContainer->deta();
  double stepSizeGridPhi=towerContainer->dphi();
 

 //  float threshold=5. * GeV ; 
  //  float threshold=m_ethreshold; 

  ATH_MSG_DEBUG( "*********** E threshold: " << m_ethreshold << " MeV "  );
  ATH_MSG_DEBUG( "*********** eta size: " << eta_size  );
  ATH_MSG_DEBUG( "*********** phi size: " << phi_size  );
  ATH_MSG_DEBUG( "*********** E threshold: " << m_ethreshold << " MeV "  );
  ATH_MSG_DEBUG( "*********** eta size: " << eta_size  );
  ATH_MSG_DEBUG( "*********** phi size: " << phi_size  );
  ATH_MSG_DEBUG( "*********** eta SeedGrid: " << eta_SeedGrid  );
  ATH_MSG_DEBUG( "*********** phi SeedGrid: " << phi_SeedGrid  );
  ATH_MSG_DEBUG( "*********** eta Duplicate: " << eta_Duplicate  );
  ATH_MSG_DEBUG( "*********** phi Duplicate: " << phi_Duplicate  );

  SlidingWindowFinder swin(eta_SeedGrid,phi_SeedGrid,m_ethreshold,towerContainer.cptr(),m_FillClusterCells,m_nextra) ;

  std::list<ClusterWithCenter> list_cwc; 

  int ieta ;
  int iphi ;

  // get the next cluster 
  while(swin.nextcluster()) {

    ieta = swin.eta();
    iphi = swin.phi();
    ATH_MSG_DEBUG("  found a cluster" << ieta<<" "<<iphi);


    // calculate the position based on the second size (m_netap,m_nphip)
    // 
    int eta_sizep=(m_netap-1)/2;
    int phi_sizep=(m_nphip-1)/2; 
    ATH_MSG_DEBUG(" etap phip  " << eta_sizep<<" "<<phi_sizep);

    double eta0=0;
    double phi0=0;
    double et0 =0; 
    int idxphi=0;
    for (int ie=ieta-eta_sizep;ie<=ieta+eta_sizep;ie++)  {
      for (int ip=iphi-phi_sizep;ip<=iphi+phi_sizep;ip++) {
	// taking into account phi ``looped bounds''
	idxphi=(ip<1)?(nGridPhi+ip):(ip>nGridPhi)?(ip-nGridPhi):(ip); 
	float et = towerContainer->et(ie,idxphi);
	et0 += et;
	eta0 += et*ie;
	phi0 += et*ip;
      }
    }

    ATH_MSG_DEBUG(" et0 eta0 phi0  " << et0 << " " << eta0 <<" "<<phi0);


    // local coordinate calculation
    if(et0>(0.25*m_ethreshold)) {
      eta0=eta0/et0;
      phi0=phi0/et0; 
    }
    else {
      eta0 = ieta;
      phi0 = iphi;
    }

    if (phi0<1) phi0+=nGridPhi;
    else if (phi0>nGridPhi) phi0-=nGridPhi;

    // translates to global coordinate. 
    eta0 = etaMin + (eta0-0.5)*stepSizeGridEta;
    phi0 = phiMin + (phi0-0.5)*stepSizeGridPhi;
    // (-pi,+pi) -> (0,2pi)
    if (phi0<0) phi0+=2*TMath::Pi();


    // use full size to estimate et0 in case we don't fill from cells
    if (!m_FillClusterCells) {
       et0=0.;
       for (int ie=ieta-eta_size;ie<=ieta+eta_size;ie++)  {
         for (int ip=iphi-phi_size;ip<=iphi+phi_size;ip++) {
	   // taking into account phi ``lopped bounds''
	   idxphi=(ip<1)?(nGridPhi+ip):(ip>nGridPhi)?(ip-nGridPhi):(ip);
           float et = towerContainer->et(ie,idxphi);
           et0 += et;
         } 
       }
    }

    ATH_MSG_DEBUG("final  et0 eta0 phi0  " << et0 << " " << eta0 <<" "<<phi0);

    std::unique_ptr<xAOD::CaloCluster> cluster_ptr (CaloClusterStoreHelper::makeCluster(cellCont.cptr(),eta0,phi0,xAOD::CaloCluster::SW_35ele));

    SlidingWindowFinder::data_iterator ifirst = swin.cell_begin();
    SlidingWindowFinder::data_iterator ilast  = swin.cell_end();
    for( ;ifirst!=ilast;ifirst++)  {
      const CaloCellContainer* aContainer = swin.getCellContainer(ifirst); 
      if (aContainer!=cellCont.cptr()) {
	msg(MSG::ERROR) << "Attept to build a cluster from different CaloCellContainers" << endmsg;
	return StatusCode::FAILURE;
      }
      size_t theIndex = swin.getCellIndex(ifirst);
      cluster_ptr->addCell(theIndex,1.);
    } 
    // ** Calculate Kine **
    CaloClusterKineHelper::calculateKine(cluster_ptr.get(),false,true); //No weight at this point! 
    ClusterWithCenter cwc;
    cwc.cluster=std::move(cluster_ptr);
    cwc.eta=ieta;
    cwc.phi=iphi;  
    // Note: cluster::et will be zero if we're not filling cells here.
    cwc.et = m_FillClusterCells ? cwc.cluster->p4().Et() : et0;
    list_cwc.emplace_back(std::move (cwc)); 
  }/////while
  

  std::list<ClusterWithCenter>::iterator it1 = list_cwc.begin();

  // loop over all combinations, check duplicate.
  // iterator already incremented if eliminated.
  for(; it1!=list_cwc.end();    ){
    std::list<ClusterWithCenter>::iterator it2 = it1; 
    int elim; 
    elim=0; 
    for(++it2; it2!=list_cwc.end();  ){ 
       elim = eliminate( *towerContainer, *it1, *it2 );
       if(elim==1){ // eliminate it1
         break; 
       } else 
       if(elim==2){ // eliminate it2; 
         it2=list_cwc.erase(it2); 
       } else 
       { 
         // keep going. 
         ++it2; 
       }
    }

    if(elim==1){ // this one has been eliminated
	it1=list_cwc.erase(it1); 
    } else 
    {
      ++it1;
    }
  }         
  // Sort the cluster according to Et 

  Calo_Detail::gt_cluster_et gcet; 
  list_cwc.sort(gcet); 

   
  // add to cluster container.
  for (ClusterWithCenter& cwc : list_cwc) {
    clusColl->push_back(std::move (cwc.cluster));
  } 

  return StatusCode::SUCCESS;
}


int CaloClusterBuilderSW::eliminate(const CaloTowerContainer& towers,
                                    ClusterWithCenter& cwc1,
                                    ClusterWithCenter& cwc2    ) const
{
  int nphi_tower = towers.nphi(); 

  int deta = abs(cwc1.eta-cwc2.eta); 
  int dphi = abs(cwc1.phi-cwc2.phi); 

  int eta_Duplicate=(m_etaDuplicate-1)/2;
  int phi_Duplicate=(m_phiDuplicate-1)/2; 

  if(abs(dphi)>nphi_tower/2) 
   {	dphi =  nphi_tower - dphi ;
   }
  
  // match 
  if(deta<=eta_Duplicate && dphi<=phi_Duplicate) {
  // compare total et, pick the higher et if et is significantly different.
  //                                              10% diff
     if(cwc1.et <0.9*cwc2.et ){ 
       return 1; 
     } else 
     if(cwc2.et <0.9*cwc1.et ){ 
      return 2; 
     } else  
  // if et close, then pick the one with high et in the center tower.
     if(towers.et(cwc1.eta,cwc1.phi)< 
        towers.et(cwc2.eta,cwc2.phi) )
     { 
       return 1; 
     } else 
     {
       return 2; 
     } 

  }   

  return 0; // nomatch 
}
