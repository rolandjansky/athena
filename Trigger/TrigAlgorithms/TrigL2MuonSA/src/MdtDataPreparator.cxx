/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtDataPreparator.h"

#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MdtRegionDefiner.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonReadoutGeometry/MuonStation.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoModelUtilities/GeoGetIds.h"

namespace {
  // the tube number of a tube in a tubeLayer in encoded in the GeoSerialIdentifier (modulo maxNTubesPerLayer)
  static constexpr unsigned int const maxNTubesPerLayer = 120;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtDataPreparator::MdtDataPreparator(const std::string& type,
						   const std::string& name,
						   const IInterface*  parent):
   AthAlgTool(type,name,parent),
   m_regionSelector("RegSelTool/RegSelTool_MDT",this),
   m_BMGpresent(false),
   m_BMGid(-1)
{
  declareProperty("RegSel_MDT", m_regionSelector);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::initialize()
{

  // Locate RegionSelector
  ATH_CHECK( m_regionSelector.retrieve());

  ATH_CHECK( m_mdtRegionDefiner.retrieve() );
  ATH_MSG_DEBUG("Retrieved service " << m_mdtRegionDefiner);

  ATH_CHECK(m_muDetMgrKey.initialize());

  const MuonGM::MuonDetectorManager* muonDetMgr=nullptr;
  ATH_CHECK( detStore()->retrieve(muonDetMgr,"Muon") );
  ATH_MSG_DEBUG("Retrieved GeoModel from DetectorStore.");
  ATH_CHECK( m_idHelperSvc.retrieve() );

  m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
    for(int phi=6; phi<8; phi++) { // phi sectors - BMGs are ony in (6 aka 12) and (7 aka 14)
      for(int eta=1; eta<4; eta++) { // eta sectors - BMGs are in eta 1 to 3
	for(int side=-1; side<2; side+=2) { // side - both sides have BMGs
	  if( !muonDetMgr->getMuonStation("BMG", side*eta, phi) ) continue;
	  for(int roe=1; roe<=( muonDetMgr->getMuonStation("BMG", side*eta, phi) )->nMuonReadoutElements(); roe++) { // iterate on readout elemets
	    const MuonGM::MdtReadoutElement* mdtRE =
	      dynamic_cast<const MuonGM::MdtReadoutElement*> ( ( muonDetMgr->getMuonStation("BMG", side*eta, phi) )->getMuonReadoutElement(roe) ); // has to be an MDT
	    if(mdtRE) initDeadChannels(mdtRE);
	  }
	}
      }
    }
  }

  ATH_CHECK(m_mdtPrepContainerKey.initialize());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::prepareData(const TrigRoiDescriptor*    p_roids,
							const TrigL2MuonSA::RpcFitResult& rpcFitResult,
							TrigL2MuonSA::MuonRoad&  muonRoad,
							TrigL2MuonSA::MdtRegion& mdtRegion,
							TrigL2MuonSA::MdtHits&   mdtHits_normal) const
{
  // define regions
  ATH_CHECK( m_mdtRegionDefiner->getMdtRegions(p_roids, rpcFitResult, muonRoad, mdtRegion) );

  ATH_CHECK( getMdtHits(p_roids, muonRoad, mdtHits_normal) );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::prepareData(const TrigRoiDescriptor*          p_roids,
							const TrigL2MuonSA::TgcFitResult& tgcFitResult,
							TrigL2MuonSA::MuonRoad&           muonRoad,
							TrigL2MuonSA::MdtRegion&          mdtRegion,
							TrigL2MuonSA::MdtHits&            mdtHits_normal) const
{
  // define regions
  ATH_CHECK( m_mdtRegionDefiner->getMdtRegions(p_roids, tgcFitResult, muonRoad, mdtRegion) );

  ATH_CHECK( getMdtHits(p_roids, muonRoad, mdtHits_normal) );

  return StatusCode::SUCCESS;
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::getMdtHits(const TrigRoiDescriptor* p_roids,
						       TrigL2MuonSA::MuonRoad& muonRoad,
						       TrigL2MuonSA::MdtHits& mdtHits_normal) const
{
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  // preload ROBs
  std::vector<uint32_t> v_robIds;
  std::vector<IdentifierHash> mdtHashList;
  if (m_use_RoIBasedDataAccess) {

    ATH_MSG_DEBUG("Use RoI based data access");

    const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

    m_regionSelector->HashIDList(*iroi, mdtHashList);
    ATH_MSG_DEBUG("mdtHashList.size()=" << mdtHashList.size());

    m_regionSelector->ROBIDList(*iroi, v_robIds);

  } else {

    ATH_MSG_DEBUG("Use full data access");

    TrigRoiDescriptor fullscan_roi( true );
    m_regionSelector->HashIDList(fullscan_roi, mdtHashList);
    ATH_MSG_DEBUG("mdtHashList.size()=" << mdtHashList.size());

    m_regionSelector->ROBIDList(fullscan_roi, v_robIds);
  }

  ATH_CHECK( collectMdtHitsFromPrepData(mdtHashList, mdtHits_normal, muonRoad, muDetMgr) );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtDataPreparator::collectMdtHitsFromPrepData(const std::vector<IdentifierHash>& v_idHash,
								       TrigL2MuonSA::MdtHits& mdtHits,
								       const TrigL2MuonSA::MuonRoad& muonRoad,
                                                                       const MuonGM::MuonDetectorManager* muDetMgr) const
{

  // Get MDT container
  if (v_idHash.empty()) {
    ATH_MSG_DEBUG("Hash list is empty");
    return StatusCode::SUCCESS;
  }

  const Muon::MdtPrepDataContainer* mdtPrds;
  auto mdtPrepContainerHandle = SG::makeHandle(m_mdtPrepContainerKey);
  mdtPrds = mdtPrepContainerHandle.cptr();
  if (!mdtPrepContainerHandle.isValid()) {
    ATH_MSG_ERROR(" Cannot retrieve MDT PRD Container " << m_mdtPrepContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Get MDT collections
  ///// Vectors of prep data collections
  std::vector<const Muon::MdtPrepDataCollection*> mdtCols;

  for(const IdentifierHash& id : v_idHash) {

    auto MDTcoll = mdtPrds->indexFindPtr(id);

    if( MDTcoll == nullptr ) {
      ATH_MSG_DEBUG("MDT prep data collection not found in Hash ID" << (int)id);
      continue;
    }

    if( MDTcoll->size() == 0 ) {
      ATH_MSG_DEBUG("MDT prep data collection is empty in Hash ID" << (int)id);
      continue;
    }

    mdtCols.push_back(MDTcoll);

    ATH_MSG_DEBUG("Selected Mdt Collection: "
		  << m_idHelperSvc->mdtIdHelper().show_to_string(MDTcoll->identify())
		  << " with size " << MDTcoll->size()
		  << "in Hash ID" << (int)id);
  }

  for( const Muon::MdtPrepDataCollection* mdtCol : mdtCols ){

    mdtHits.reserve( mdtHits.size() + mdtCol->size() );
    for( const Muon::MdtPrepData* mdt : *mdtCol ) {

      const MuonGM::MdtReadoutElement* mdtReadout = mdt->detectorElement();
      if (!mdtReadout) continue;

      const MuonGM::MuonStation* muonStation = mdtReadout->parentMuonStation();

      int StationPhi = mdtReadout->getStationPhi();
      int StationEta = mdtReadout->getStationEta();
      int MultiLayer = mdtReadout->getMultilayer();
      double cXmid;
      double cYmid;
      double cAmid = 0;
      double cPhip;

      Identifier id = mdt->identify();
      int adc       = mdt->adc();
      int drift     = mdt->tdc();

      int TubeLayers = mdtReadout->getNLayers();
      int TubeLayer = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
      if(TubeLayer > TubeLayers) TubeLayer -= TubeLayers;
      int Layer = (MultiLayer-1)*TubeLayers + TubeLayer;
      int Tube = m_idHelperSvc->mdtIdHelper().tube(id);

      double OrtoRadialPos = mdtReadout->getStationS();
      std::string chamberType = mdtReadout->getStationType();
      char st = chamberType[1];

      int chamber = 0;
      if (chamberType[0]=='E') {
	/// Endcap
	if (st=='I') chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
	if (st=='M') chamber = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
	if (st=='O') chamber = xAOD::L2MuonParameters::Chamber::EndcapOuter;
	if (st=='E') chamber = xAOD::L2MuonParameters::Chamber::EndcapExtra;
      } else {
	/// Barrel
	if (st=='I') chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
	if (st=='M') chamber = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
	if (st=='O') chamber = xAOD::L2MuonParameters::Chamber::BarrelOuter;
	if (st=='E' && chamberType[2]=='E') chamber = xAOD::L2MuonParameters::Chamber::BEE;
	if (st=='M' && chamberType[2]=='E') chamber = xAOD::L2MuonParameters::Chamber::BME;
	if (st=='M' && chamberType[2]=='G') chamber = xAOD::L2MuonParameters::Chamber::Backup;
      }

      double R = -99999., Z = -99999.;
      if(m_BMGpresent && m_idHelperSvc->mdtIdHelper().stationName(id) == m_BMGid ) {
        std::map<Identifier, std::vector<Identifier> >::const_iterator myIt = m_DeadChannels.find( muDetMgr->getMdtReadoutElement(id)->identify() );
        if( myIt != m_DeadChannels.end() ){
          if( std::find( (myIt->second).begin(), (myIt->second).end(), id) != (myIt->second).end() ) {
            ATH_MSG_DEBUG("Skipping tube with identifier " << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
            continue;
          }
        }
      }
      R = mdtReadout->center(TubeLayer, Tube).perp();
      Z = mdtReadout->center(TubeLayer, Tube).z();

      Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*muonStation->getNominalAmdbLRSToGlobal());
      if(muonStation->endcap()==0){
	cXmid = (trans*Amg::Vector3D(0.,0.,0.)).z();
	double halfRadialThicknessOfMultilayer = muonStation->RsizeMdtStation()/2.;
	cYmid = ((trans*Amg::Vector3D(0.,0.,0.)).perp()+halfRadialThicknessOfMultilayer);
      }
      else{
	cXmid = (trans*Amg::Vector3D(0.,0.,0.)).perp();
	double halfZThicknessOfMultilayer = muonStation->ZsizeMdtStation()/2.;
	cYmid = (trans*Amg::Vector3D(0.,0.,0.)).z();
	if(cYmid>0) cYmid += halfZThicknessOfMultilayer;
	else cYmid -= halfZThicknessOfMultilayer;
      }
      cPhip = (trans*Amg::Vector3D(0.,0.,0.)).phi();

      double dphi  = 0;
      double cphi  = muonRoad.phi[chamber][0];
      if( cPhip*cphi>0 ) {
	dphi = std::abs(cPhip - cphi);
      } else {
	if(std::abs(cphi) > M_PI/2.) {
	  double phi1 = (cPhip>0.)? cPhip-M_PI : cPhip+M_PI;
	  double phi2 = (cphi >0.)? cphi -M_PI : cphi +M_PI;
	  dphi = std::abs(phi1) + std::abs(phi2);
	}
	else {
	  dphi = std::abs(cPhip) + std::abs(cphi);
	}
      }

      if(muonStation->endcap()==1)
	R = std::sqrt(R*R+R*R*std::tan(dphi)*std::tan(dphi));

      Amg::Vector3D OrigOfMdtInAmdbFrame =
	Amg::Hep3VectorToEigen( muonStation->getBlineFixedPointInAmdbLRS() );
      double Rmin =(trans*OrigOfMdtInAmdbFrame).perp();

      float cInCo = 1./std::cos(std::abs(std::atan(OrtoRadialPos/Rmin)));
      float cPhi0 = cPhip - std::atan(OrtoRadialPos/Rmin);
      if(cPhi0 > M_PI) cPhip -= 2*M_PI;
      if(cPhip<0. && (std::abs(M_PI+cPhip) < 0.05) ) cPhip = std::acos(0.)*2.;

      ATH_MSG_DEBUG(" ...MDT hit Z/R/chamber/MultiLater/TubeLayer/Tube/Layer/adc/tdc = "
		    << Z << "/" << R << "/" << chamber << "/" << MultiLayer << "/" << TubeLayer << "/"
		    << Tube << "/" << Layer << "/" << adc << "/" << drift);

      // no residual check for the moment
      // (residual check at pattern finder)
      if(Layer!=0 && Tube !=0) {

	// create the new digit
	TrigL2MuonSA::MdtHitData tmp;
	tmp.name       = 0;
	tmp.StationEta = StationEta;
	tmp.StationPhi = StationPhi;
	tmp.Multilayer = MultiLayer;
	tmp.Layer      = Layer - 1;
	tmp.TubeLayer  = TubeLayer;
	tmp.Tube       = Tube;
	tmp.cYmid      = cYmid;
	tmp.cXmid      = cXmid;
	tmp.cAmid      = cAmid;
	tmp.cPhip      = cPhip;
	tmp.cInCo      = cInCo;
	tmp.cPhi0      = cPhi0;
	for(unsigned int i=0; i<4; i++) { tmp.cType[i] = chamberType[i]; }
	tmp.Z          = Z;
	tmp.R          = R;
	tmp.DriftTime  = drift;
	tmp.DriftSpace = 0.;
	tmp.DriftSigma = 0;
	tmp.Adc        = adc;
	tmp.OnlineId   = 0;
	tmp.LeadingCoarseTime  = 0;
	tmp.LeadingFineTime    = 0;
	tmp.TrailingCoarseTime = 0;
	tmp.TrailingFineTime   = 0;
	tmp.Residual  = 0;
	tmp.isOutlier = 0;
	tmp.Chamber = chamber;
	tmp.Id = id;

	mdtHits.push_back(tmp);
      }
    } // end of MdtPrepDataCollection loop
  } // end of MdtPrepDataCollection vector loop

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtDataPreparator::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  std::vector<int> tubes;
  geoGetIds ([&] (int id) { tubes.push_back (id); }, &*cv);
  std::sort (tubes.begin(), tubes.end());

  Identifier detElId = mydetEl->identify();

  int name = m_idHelperSvc->mdtIdHelper().stationName(detElId);
  int eta = m_idHelperSvc->mdtIdHelper().stationEta(detElId);
  int phi = m_idHelperSvc->mdtIdHelper().stationPhi(detElId);
  int ml = m_idHelperSvc->mdtIdHelper().multilayer(detElId);
  std::vector<Identifier> deadTubes;

  std::vector<int>::iterator it = tubes.begin();
  for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
    for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
      int want_id = layer*maxNTubesPerLayer + tube;
      if (it != tubes.end() && *it == want_id) {
        ++it;
      }
      else {
        it = std::lower_bound (tubes.begin(), tubes.end(), want_id);
        if (it != tubes.end() && *it == want_id) {
          ++it;
        }
        else {
          Identifier deadTubeId = m_idHelperSvc->mdtIdHelper().channelID( name, eta, phi, ml, layer, tube );
          deadTubes.push_back( deadTubeId );
          ATH_MSG_VERBOSE("adding dead tube (" << tube  << "), layer(" <<  layer
                          << "), phi(" << phi << "), eta(" << eta << "), name(" << name
                          << "), multilayerId(" << ml << ") and identifier " << deadTubeId <<" .");
        }
      }
    }
  }
  std::sort(deadTubes.begin(), deadTubes.end());
  m_DeadChannels[detElId] = deadTubes;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
