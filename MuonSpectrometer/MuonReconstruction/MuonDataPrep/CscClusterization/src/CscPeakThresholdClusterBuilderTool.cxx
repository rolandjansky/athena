/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscPeakThresholdClusterBuilderTool.cxx
#include "CscPeakThresholdClusterBuilderTool.h"
#include "GaudiKernel/Property.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkSurfaces/Surface.h"
#include "MuonPrepRawData/CscClusterStatus.h"

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <sstream>

using std::ostringstream;
using std::vector;
using std::setw;
using MuonGM::CscReadoutElement;
using Muon::CscClusterStatus;
using Muon::CscPrepData;
using Muon::CscPrepDataCollection;
using Muon::CscPrepDataContainer;
using Muon::CscStripPrepData;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepDataContainer;

//******************************************************************************
// Local definitions.
namespace {
//******************************************************************************

// Convert chamber identifier to string.
std::string chamber(int istation, int zsec, int phi) {
  ostringstream ssout;
  if ( istation == 1 ) ssout << "CSS";
  else if ( istation ==2 ) ssout << "CSL";
  else ssout << "???";
  if ( zsec == -1 ) ssout << "-";
  else if ( zsec == 1 ) ssout << "+";
  else ssout << "?";
  ssout << phi;
  return ssout.str();
}

// Convert measphi to string.
std::string setaphi(bool measphi) {
  if (measphi) return "phi";
  return "eta";
}

//******************************************************************************
}  // end unnamed namespace
//******************************************************************************

CscPeakThresholdClusterBuilderTool::
CscPeakThresholdClusterBuilderTool(const std::string &type, const std::string &aname, const IInterface* parent)
  : AthAlgTool(type, aname, parent),
    m_digit_key("CSC_Measurements"), m_cluster_handle("CSC_Clusters"),
    m_pstrip_fitter("CalibCscStripFitter/CalibCscStripFitter"),
    m_pfitter_def("SimpleCscClusterFitter/SimpleCscClusterFitter"),
    m_pfitter_prec("QratCscClusterFitter/QratCscClusterFitter"),
    m_pfitter_split("CscSplitClusterFitter/CscSplitClusterFitter"),
    m_pmuon_detmgr(0), m_phelper(0), m_fullEventDone(false)
{

  declareInterface<ICscClusterBuilder>(this);
  
  declareProperty("qpeak_threshold_eta", m_qpeak_threshold_eta = 21000.0);
  declareProperty("qpeak_threshold_phi", m_qpeak_threshold_phi = 33000.0);
  declareProperty("q3sum_threshold_eta", m_q3sum_threshold_eta = 38000.0);
  declareProperty("q3sum_threshold_phi", m_q3sum_threshold_phi = 33000.0);
  declareProperty("digit_key",  m_digit_key );
  declareProperty("cluster_key",  m_cluster_handle );
  declareProperty("strip_fitter", m_pstrip_fitter);
  declareProperty("default_fitter", m_pfitter_def);
  declareProperty("precision_fitter", m_pfitter_prec);
  declareProperty("split_fitter", m_pfitter_split);
}

//******************************************************************************

// Destructor.

CscPeakThresholdClusterBuilderTool::~CscPeakThresholdClusterBuilderTool()
{ }

//******************************************************************************

StatusCode CscPeakThresholdClusterBuilderTool::initialize(){
  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_CHECK( m_digit_key.initialize() );
  // Display algorithm properties.
  ATH_MSG_DEBUG ( "Properties for " << name() << ":" );
  ATH_MSG_DEBUG ( "  Cluster qpeak threshold is eta/phi " << m_qpeak_threshold_eta << "/" << m_qpeak_threshold_phi);
  ATH_MSG_DEBUG ( "  Cluster q3sum threshold is eta/phi " << m_q3sum_threshold_eta << "/" << m_q3sum_threshold_phi);
  ATH_MSG_DEBUG ( "  Strip fitter is " << m_pstrip_fitter.typeAndName() );
  ATH_MSG_DEBUG ( "  Default cluster fitter is " << m_pfitter_def.typeAndName() );
  ATH_MSG_DEBUG ( "  Precision cluster fitter is " << m_pfitter_prec.typeAndName() );
  ATH_MSG_DEBUG ( "  Split cluster fitter is " << m_pfitter_split.typeAndName() );
  ATH_MSG_DEBUG ( "  Input digit key is " << m_digit_key.key() );
  ATH_MSG_DEBUG ( "  Output cluster key is " << m_cluster_handle.key() );

  // Retrieve the strip fitting tool.
  if ( m_pstrip_fitter.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve strip fitting tool " << m_pstrip_fitter );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_DEBUG ( "Retrieved strip fitting tool " << m_pstrip_fitter );
  
  // Retrieve the default cluster fitting tool.
  if ( m_pfitter_def.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve CSC default cluster fitting tool "
                    << m_pfitter_def->name() );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved CSC default cluster fitting tool" );

  // Retrieve the precision cluster fitting tool.
  if ( m_pfitter_prec.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve CSC precision cluster fitting tool "
                    << m_pfitter_prec->name() );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved CSC precision cluster fitting tool" );

  // Retrieve the split cluster fitting tool.
  if ( m_pfitter_split.retrieve().isFailure() ) {
    ATH_MSG_FATAL ( "Unable to retrieve CSC split cluster fitting tool "
                    << m_pfitter_split->name() );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_DEBUG ( "Retrieved CSC split cluster fitting tool" );
                 
  if ( detStore()->retrieve(m_pmuon_detmgr).isFailure() ) {
    ATH_MSG_FATAL ( " Cannot retrieve MuonGeoModel " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved geometry." );

  m_phelper = m_pmuon_detmgr->cscIdHelper();


  return StatusCode::SUCCESS;
}

//******************************************************************************

StatusCode CscPeakThresholdClusterBuilderTool::getClusters(std::vector<IdentifierHash>& givenIDs, std::vector<IdentifierHash>& decodedIds) {

  // clear output vector of selected data collections containing data
  decodedIds.clear();

  if (!m_cluster_handle.isPresent()) {
    /// clean up the PrepRawData container
    auto object = std::make_unique<CscPrepDataContainer>(m_phelper->module_hash_max());
    
    /// record the container in storeGate
    if ( m_cluster_handle.record(std::move(object)).isFailure() ) {
      ATH_MSG_ERROR ( "Could not record container of CSC Cluster PrepData at "
                      << m_cluster_handle.key() );
      return StatusCode::RECOVERABLE;
    }
    m_fullEventDone=false;
    if (givenIDs.size() == 0) m_fullEventDone=true;
    
  } else {
    ATH_MSG_DEBUG ( "CSC Cluster PrepData Container is already in StoreGate " );
    if (m_fullEventDone) {
      ATH_MSG_DEBUG ( "Whole event has already been processed; nothing to do");
      return StatusCode::SUCCESS;
    }
    if (givenIDs.size() == 0) m_fullEventDone = true;
  }
  
  if (givenIDs.size()!=0) {
    for (unsigned int i=0; i<givenIDs.size(); ++i) {
      if ( getClusters(givenIDs[i],decodedIds).isFailure() ) {
        ATH_MSG_ERROR ( "Unable to decode CSC RDO " << i << "th into CSC PrepRawData" );
        return StatusCode::RECOVERABLE;
      }
    }  
  } else {
    // Clusterization is done for every area
    if ( getClusters(decodedIds).isFailure()) {
      ATH_MSG_ERROR ( "Unable to decode CSC RDO into CSC PrepRawData" );
      return StatusCode::RECOVERABLE;
    }
  }
  return StatusCode::SUCCESS;
}


//******************************************************************************

StatusCode CscPeakThresholdClusterBuilderTool::getClusters(IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIds) {

  // identifiers of collections already decoded and stored in the container will be skipped
  if (m_cluster_handle->indexFind(givenHashId) != m_cluster_handle->end()) {
    decodedIds.push_back(givenHashId);
    ATH_MSG_DEBUG ( "A collection already exists in the container for offline id hash. "
                    << (int) givenHashId );
    return StatusCode::SUCCESS;
  }


  // Retrieve the CSC digits for this event.
  SG::ReadHandle<CscStripPrepDataContainer> pdigcon(m_digit_key);
  if ( pdigcon.isValid() ) {
    ATH_MSG_DEBUG ( "Retrieved strip container " << m_digit_key.key() << " with "
                    << pdigcon->size() << " entries." );
  } else {
    ATH_MSG_WARNING ( "Failure to retrieve strip container " << m_digit_key.key() );
    return StatusCode::SUCCESS;
  }
  

  //**********************************************
  // retrieve specific collection for the givenID
  CscStripPrepDataContainer::const_iterator it_coll = pdigcon->indexFind(givenHashId);
  if (pdigcon->end() ==  it_coll) {
    unsigned int coll_hash = givenHashId;
    ATH_MSG_WARNING ( "Specific CSC Strip PrepData collection retrieving failed for collection hash = "
                      << coll_hash );
    return StatusCode::SUCCESS;
  }
  const CscStripPrepDataCollection * col = *it_coll;
  
  ATH_MSG_DEBUG ( "Retrieved " << col->size() << " CSC Strip PrepDatas." );
  
  Identifier colid = col->identify();
  int istation = m_phelper->stationName(colid) - 49;
  int zsec = m_phelper->stationEta(colid);
  int phisec = m_phelper->stationPhi(colid);

  ATH_MSG_DEBUG ( "  Strip collection " << chamber(istation, zsec, phisec)
                  << " has " << col->size() << " strips" );
  
  // Create arrays to hold digits and cathode plane parameters.
  vector<const CscStripPrepData*> strips[8];
  int maxstrip[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  
  IdentifierHash hash;
  // Loop over digits and fill these arrays.
  for ( CscStripPrepDataCollection::const_iterator idig=col->begin();
        idig!=col->end(); ++idig ) {
    const CscStripPrepData& dig = **idig;
    Identifier did = dig.identify();
    hash=dig.collectionHash();
    const CscReadoutElement* pro = m_pmuon_detmgr->getCscReadoutElement(did);
    int wlay = m_phelper->wireLayer(did);
    int measphi = m_phelper->measuresPhi(did);
    int idx = 2*(wlay-1) + measphi;
    // First entry for a cathode plane, initialize.
    if ( maxstrip[idx] == 0 ) {
      maxstrip[idx] = pro->maxNumberOfStrips(measphi);
      for ( int istrip=0; istrip<maxstrip[idx]; ++istrip ) 
        strips[idx].push_back(0);
    }
    int istrip = m_phelper->strip(did) - 1;
    if ( istrip<0 || istrip>=maxstrip[idx] ) {
      ATH_MSG_WARNING ( "Invalid strip number" );
      continue;
    }
    strips[idx][istrip] = &dig;
  }
  
  // Cluster.
  CscPrepDataCollection* newCollection = 0;
  for ( int measphi=0; measphi<2; ++measphi ) {
    for ( int wlay=1; wlay<5; ++wlay ) {
      int idx = 2*(wlay-1) + measphi;
      if ( maxstrip[idx] ) {
        make_clusters(measphi, strips[idx],newCollection);
        ATH_MSG_DEBUG (  "    " << wlay << "th layer ");
      }
    }
  }
  if (newCollection){
    if ( m_cluster_handle->addCollection(newCollection, hash).isFailure() ) {
      ATH_MSG_ERROR ( "Couldn't add CscPrepdataCollection to container!" );
      return StatusCode::FAILURE;
    }
    decodedIds.push_back(hash); //Record that this collection contains data
  }
  
  return StatusCode::SUCCESS;
}


//******************************************************************************

StatusCode CscPeakThresholdClusterBuilderTool::getClusters(std::vector<IdentifierHash>& decodedIds) {
      
  // Retrieve the CSC digits for this event.
  
  SG::ReadHandle<CscStripPrepDataContainer> pdigcon(m_digit_key);
  if ( pdigcon.isValid() ) {
    ATH_MSG_DEBUG ( "Retrieved strip container " << m_digit_key.key() << " with "
                    << pdigcon->size() << " entries." );
  } else {
    ATH_MSG_WARNING ( "Failure to retrieve strip container " << m_digit_key.key() );
    return StatusCode::SUCCESS;
  }
  
  // Loop over digit collections.
  // This a loop over chambers (each with 4 wire planes).
  const CscStripPrepDataContainer& con = *pdigcon;
  for (CscStripPrepDataContainer::const_iterator icol=con.begin();
       icol!=con.end(); ++icol) {
    const CscStripPrepDataCollection& col = **icol;
    Identifier colid = col.identify();
    int istation = m_phelper->stationName(colid) - 49;
    int zsec = m_phelper->stationEta(colid);
    int phisec = m_phelper->stationPhi(colid);
    ATH_MSG_DEBUG ( "  Strip collection " << chamber(istation, zsec, phisec)
                    << " has " << col.size() << " strips" );
    
    // Create arrays to hold digits and cathode plane parameters.
    vector<const CscStripPrepData*> strips[8];
    int maxstrip[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    IdentifierHash hash;
    // Loop over digits and fill these arrays.
    for ( CscStripPrepDataCollection::const_iterator idig=col.begin();
          idig!=col.end(); ++idig ) {
      const CscStripPrepData& dig = **idig;
      Identifier did = dig.identify();
      hash=dig.collectionHash();
      const CscReadoutElement* pro = m_pmuon_detmgr->getCscReadoutElement(did);
      int wlay = m_phelper->wireLayer(did);
      int measphi = m_phelper->measuresPhi(did);
      int idx = 2*(wlay-1) + measphi;
      // First entry for a cathode plane, initialize.
      if ( maxstrip[idx] == 0 ) {
        maxstrip[idx] = pro->maxNumberOfStrips(measphi);
        for ( int istrip=0; istrip<maxstrip[idx]; ++istrip ) 
          strips[idx].push_back(0);
      }
      int istrip = m_phelper->strip(did) - 1;
      if ( istrip<0 || istrip>=maxstrip[idx] ) {
        ATH_MSG_WARNING ( "Invalid strip number" );
        continue;
      }
      strips[idx][istrip] = &dig;
    }
    
    // Cluster.
    CscPrepDataCollection* newCollection = 0;
    for ( int measphi=0; measphi<2; ++measphi ) {
      for ( int wlay=1; wlay<5; ++wlay ) {
        int idx = 2*(wlay-1) + measphi;
        if ( maxstrip[idx] ) {
          make_clusters(measphi, strips[idx],newCollection);
          ATH_MSG_DEBUG (  "    " << wlay << "th layer ");
        }
      }
    }
    if (newCollection){
      if ( m_cluster_handle->addCollection(newCollection, hash).isFailure() ) {
        ATH_MSG_ERROR ( "Couldn't add CscPrepdataCollection to container!" );
        return StatusCode::FAILURE;
      }
      decodedIds.push_back(hash); //Record that this collection contains data
    }
  }  // end loop over chambers
  
  return StatusCode::SUCCESS;
}
//******************************************************************************

StatusCode CscPeakThresholdClusterBuilderTool::finalize() {
  ATH_MSG_VERBOSE ( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}

//******************************************************************************

// Build clusters.
//   dump - whether to write messages
//   dstrip = CSC digit pointer for each strip.
//   qstrip - charge on each strip
// Note strip numbering is 0, maxstA, shifted by 1 from ATLAS strip numbers.
// Center of strip is at pitch * (istrip + 0.5 - maxstrip/2).

int CscPeakThresholdClusterBuilderTool::
make_clusters(bool measphi, const vector<const CscStripPrepData*>& strips,CscPrepDataCollection *&newCollection) {

  //CscPrepDataCollection* newCollection = 0;

  // Loop over channels.
  unsigned int maxstrip = strips.size();

  ATH_MSG_DEBUG ( "    Clustering for "
                  << setaphi(measphi) << " plane with " << maxstrip << " strips" );

  // Loop over strips and fetch the charge and time for each.
  // Also set flag indicating if this strip is active, i.e. should be included
  // in a cluster.
  ICscClusterFitter::StripFitList stripfits;
  std::vector<bool> astrip;
  IdentifierHash cscHashId;

  for ( unsigned int istrip=0; istrip<strips.size(); ++istrip ) {
    const CscStripPrepData* pstrip = strips[istrip];
    ICscClusterFitter::StripFit res;
    res.charge = 0.0;
    res.time = -666.;
    bool active = false;

    if ( pstrip ) {
      if (!newCollection) {
        Identifier elementId = m_phelper->elementID(pstrip->identify());
        cscHashId=pstrip->collectionHash(); 
        newCollection = new CscPrepDataCollection(cscHashId);
        newCollection->setIdentifier(elementId);
      }
      if ( ! m_pstrip_fitter ) {
        ATH_MSG_WARNING ( "Unable to locate strip fitter" );
        return StatusCode::SUCCESS;
      }
      res = m_pstrip_fitter->fit(*pstrip);
      active = res.charge > 0.0; // Allow all the positive charged strips...
      // Log message.
      ATH_MSG_DEBUG ( "      Strip " << setw(3) << istrip+1
                      << ": charge= " << setw(7) << int(res.charge)
                      << " time=" << setw(3) << int(res.time+0.5) );
      //        if ( active ) *m_log << " *";
      //        else *m_log << " .";
      //        if ( res.status ) *m_log << " x";
      //        else *m_log << " o";
      //        *m_log );
      
    }
    
    //    if (res.charge == 0.0) res.charge =40.;
    stripfits.push_back(res);
    astrip.push_back(active);
  }

  // Loop over strips and create clusters.
  bool incluster = false;
  int first_strip = 0;                 // First strip in the cluster.
  double qpeak =0;
  ICscClusterFitter::StripFitList sfits;
  std::vector<const CscStripPrepData*> clusterStrips;
  std::vector<Identifier> prd_digit_ids;
  for ( unsigned int istrip=0; istrip<strips.size(); ++istrip ) {
    const CscStripPrepData* pstrip = strips[istrip];
    ICscClusterFitter::StripFit sfit = stripfits[istrip];
    double q = sfit.charge;

    // If the current strip is not active, skip it.
    if ( ! astrip[istrip] ) continue;
    assert( pstrip != 0 );

    // First strip in cluster: initialize a new cluster.
    if ( ! incluster ) {
      incluster = true;
      qpeak = q;
      first_strip = istrip;
      sfits.clear();
      clusterStrips.clear();
      prd_digit_ids.clear();
      incluster = true;
    }

    // Add strip to the current cluster.
    sfits.push_back(sfit);
    clusterStrips.push_back(pstrip);
    prd_digit_ids.push_back(pstrip->identify());
    if ( q > qpeak ) qpeak = q;

    
    // If this is not the last strip in the plane, and the next
    // strip is active, add the next strip to the cluster.
    if ( istrip!=maxstrip-1 && astrip[istrip+1] ) continue;

    ///////////////////////////
    // Create a cluster
    ATH_MSG_VERBOSE ( "      Creating cluster" );

    // QPeak Threshold Requirement.................

    if ( qpeak < (measphi ? m_qpeak_threshold_phi : m_qpeak_threshold_eta) ) { // discard cluster and find new one....
      incluster = false;
      continue;
    }

    int fitresult = 99;
    std::vector<ICscClusterFitter::Result> results;
    // Precision fit.
    if ( ! measphi && m_pfitter_prec ) {
      ATH_MSG_VERBOSE ( "      Performing precision fit with " << m_pfitter_prec );

      results = m_pfitter_prec->fit(sfits);
      fitresult = results[0].fitStatus;

      ATH_MSG_VERBOSE ( "        Precision fit result return=" << fitresult );
    }

    if (fitresult ==6) {  // in case of multipeak cluster
      ATH_MSG_VERBOSE ( "      Performing split fit with " << m_pfitter_split );

      results = m_pfitter_split->fit(sfits);
      fitresult = results[0].fitStatus;

      for (unsigned int i=0; i<results.size(); ++i)
        ATH_MSG_VERBOSE ( "        Split fit result return=" << results[i].fitStatus );
    }
    // if splitcluster is succeeded, fitresult should be either 20 or 21
    // if not, it's 19.
    
    // Default fit.
    if ( measphi || (fitresult >0 && fitresult <20) ) { // including measphi case
      ATH_MSG_VERBOSE ( "      Performing default fit with " << m_pfitter_def );
      ICscClusterFitter::Result res;
      if (!measphi) res = results[0];
      CscClusterStatus oldclustatus = res.clusterStatus;
      results = m_pfitter_def->fit(sfits);
      res = results[0];
      fitresult = results[0].fitStatus;
      if ( fitresult ) {
        ATH_MSG_VERBOSE ( "        Default fit failed: return=" << fitresult );
        return 1;
      } else {
        ATH_MSG_VERBOSE ( "        Default fit succeeded" );
      }
      // Keep the status from the first fit if it is defined.
      if ( oldclustatus != Muon::CscStatusUndefined ) {
        res.clusterStatus = oldclustatus;
        // we want to keep oldcluster status 
        results[0] = res;
      }
    }
    
    // Check results.
    unsigned int nresults = results.size();
    for (unsigned int ire=0; ire<nresults; ++ire) {

      if ( results[ire].qpeak
           < (measphi ? m_qpeak_threshold_phi : m_qpeak_threshold_eta) )  continue;
      double q3sum = results[ire].qleft+results[ire].qpeak+results[ire].qright;
      if ( q3sum
           < (measphi ? m_q3sum_threshold_phi : m_q3sum_threshold_eta) )  continue;

      CscClusterStatus clustatus = results[ire].clusterStatus;
      Muon::CscTimeStatus timeStatus = results[ire].timeStatus;
      double pos = results[ire].position;
      double err = results[ire].dposition;
      unsigned int id_strip = results[ire].strip;
      double cluster_charge = results[ire].charge;
      double cluster_time = results[ire].time;
      if ( clustatus == Muon::CscStatusUndefined ) 
        ATH_MSG_DEBUG ( "      Csc Cluster Status is not defined." );

      if ( id_strip >= sfits.size() ) {
        ATH_MSG_WARNING ( "        Fit size check failed: " );
        return 1;
      }
      // Fetch the strip used to identify this cluster.
      const CscStripPrepData* pstrip_id = 0;
      if ( id_strip < clusterStrips.size() ) pstrip_id = clusterStrips[id_strip];
      if ( ! pstrip_id ) {
        ATH_MSG_WARNING ( "        Fit ID check failed: " );
        return 1;
      }
      
      // Create ATLAS CSC cluster.
      Identifier cluster_id = pstrip_id->identify();
      IdentifierHash cluster_hash = pstrip_id->collectionHash();
      int zsec = m_phelper->stationEta(cluster_id);
      int wlay = m_phelper->wireLayer(cluster_id);
      // This local position is in the muon (not tracking) coordinate system.
      //      const CscReadoutElement* pro = pstrip_id->detectorElement();
      const CscReadoutElement* pro = m_pmuon_detmgr->getCscReadoutElement(cluster_id);
      Amg::Vector3D local_pos = pro->nominalLocalClusterPos(zsec, wlay, measphi, pos);
      Amg::MatrixX* cov = new Amg::MatrixX(1,1);
      (*cov)(0,0) = err*err;
      Amg::Vector2D plpos( measphi ? local_pos.y(): local_pos.z(), measphi ? local_pos.z(): local_pos.y());

      if( msgLvl(MSG::DEBUG) ){
	ATH_MSG_DEBUG ( "        Cluster parameters: " << nresults );
	ATH_MSG_DEBUG ( "                ID strip: " << first_strip + id_strip
			<< "(" << first_strip << ":" << id_strip << ")" );
	ATH_MSG_DEBUG ( "          local position: "
			<< plpos.x() << " " << plpos.y() );
	ATH_MSG_DEBUG ( "                  error: " << Amg::toString(*cov) );	
	ATH_MSG_DEBUG ( "                  charge: " << cluster_charge );
	ATH_MSG_DEBUG ( "                    time: " << cluster_time );
	ATH_MSG_DEBUG ( "                  status: " << Muon::toString(clustatus) );
      }
      std::vector<Identifier> prd_digit_ids_submit;
      unsigned int fstrip =results[ire].fstrip;
      unsigned int lstrip =results[ire].lstrip;
      prd_digit_ids_submit.reserve(lstrip+1);
      for ( unsigned int ids_index=fstrip; ids_index<lstrip+1; ++ids_index ) {
        if( ids_index >= prd_digit_ids.size() ) ATH_MSG_WARNING(" bad index " << ids_index << " maximum " << prd_digit_ids.size() );
        else prd_digit_ids_submit.push_back(prd_digit_ids[ids_index]);
      }
      
      CscPrepData* pclus = new CscPrepData(cluster_id, cluster_hash, plpos, prd_digit_ids_submit, 
                                           cov, pro, int(cluster_charge+0.5), cluster_time,
                                           clustatus, timeStatus);
      pclus->setHashAndIndex(newCollection->identifyHash(), newCollection->size());
    
      newCollection->push_back(pclus);

    }
    // Reset incluster.
    incluster = false;
  }  // end loop over strips
  return 0;
}

//******************************************************************************
