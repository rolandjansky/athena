/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonDetectorFactoryLite.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPerfUtils.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MuonChamberLite.h"
#include "MuonGeoModel/RDBReaderAtlas.h"
#include "MuonGeoModel/Station.h"
#include "MuonReadoutGeometry/GenericCSCCache.h"
#include "MuonReadoutGeometry/GenericMDTCache.h"
#include "MuonReadoutGeometry/GenericRPCCache.h"
#include "MuonReadoutGeometry/GenericTGCCache.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "StoreGate/StoreGateSvc.h"

namespace MuonGM {

  // Helper function to retrieve ID Helpers:
  template <class T> 
  const T  *getIdHelper (StoreGateSvc *pDetStore, const std::string & helperName) {
    const T *helper;
    StatusCode sc = pDetStore->retrieve(helper, helperName);
    if (sc.isFailure()) {
      return nullptr;
    }	    
    else {
      return helper;
    }
  };
  



  MuonDetectorFactoryLite::MuonDetectorFactoryLite(StoreGateSvc *pDetStore, GeoModelIO::ReadGeoModel *sqliteReader):
    m_pDetStore{pDetStore},
    m_sqliteReader(sqliteReader) {
    }

  MuonDetectorFactoryLite::~MuonDetectorFactoryLite() =default;

  const MuonDetectorManager *MuonDetectorFactoryLite::getDetectorManager() const { return m_manager; }
  MuonDetectorManager *MuonDetectorFactoryLite::getDetectorManager() { return m_manager; }

  void MuonDetectorFactoryLite::create(GeoPhysVol *) {

    std::map<std::string, GeoAlignableTransform*> mapAXF = m_sqliteReader->getPublishedNodes<std::string, GeoAlignableTransform*>("MuonSys");                     
    std::map<std::string, GeoFullPhysVol*>        mapFPV = m_sqliteReader->getPublishedNodes<std::string, GeoFullPhysVol*>("MuonSys");                                            

    bool hasCsc=std::find_if(mapFPV.begin(),mapFPV.end(),[](std::pair<std::string, GeoFullPhysVol*> s)->bool {return s.first.find("CS")!=std::string::npos;})!=mapFPV.end();
    bool hasMm=std::find_if(mapFPV.begin(),mapFPV.end(),[](std::pair<std::string, GeoFullPhysVol*> s)->bool {return s.first.find("MM")!=std::string::npos;})!=mapFPV.end();
    bool hasSTgc=std::find_if(mapFPV.begin(),mapFPV.end(),[](std::pair<std::string, GeoFullPhysVol*> s)->bool {return s.first.find("STGC")!=std::string::npos;})!=mapFPV.end();



    MsgStream log(Athena::getMessageSvc(), "MuGM:MuonFactory");

    if (!m_manager)
      m_manager = new  MuonDetectorManager();


    m_manager->setControlAlinesFlag(false);
    m_manager->setApplyCscIntAlignment(false);
    m_manager->setCscIlinesFlag(false);
    m_manager->setCscFromGM(true);

    // here create the MYSQL singleton
    MYSQL::LockedMYSQL mysql = MYSQL::GetPointer();
    mysql->set_amdb_from_RDB(true);

    // Set up the ID helpers:
    const MdtIdHelper  * mdtidh =getIdHelper<MdtIdHelper>  (m_pDetStore,"MDTIDHELPER");
    const RpcIdHelper  * rpcidh =getIdHelper<RpcIdHelper>  (m_pDetStore,"RPCIDHELPER");
    const TgcIdHelper  * tgcidh =getIdHelper<TgcIdHelper>  (m_pDetStore,"TGCIDHELPER");
    const CscIdHelper  * cscidh =hasCsc  ? getIdHelper<CscIdHelper>  (m_pDetStore,"CSCIDHELPER") :nullptr;
    const MmIdHelper   * mmidh  =hasMm   ? getIdHelper<MmIdHelper>   (m_pDetStore,"MMIDHELPER")  :nullptr;
    const sTgcIdHelper * stgcidh=hasSTgc ? getIdHelper<sTgcIdHelper> (m_pDetStore,"STGCIDHELPER"):nullptr;
    if (mdtidh)  m_manager->set_mdtIdHelper(mdtidh);
    if (rpcidh)  m_manager->set_rpcIdHelper(rpcidh);
    if (tgcidh)  m_manager->set_tgcIdHelper(tgcidh);
    if (cscidh)  m_manager->set_cscIdHelper(cscidh);
    if (mmidh)   m_manager->set_mmIdHelper(mmidh);
    if (stgcidh) m_manager->set_stgcIdHelper(stgcidh);



    std::unique_ptr<RDBReaderAtlas> dbr;
    {        
      // We never alter these in the "Lite" workflow
      std::string oracleTag="";
      std::string oracleNode="";
      bool dumpAlines=false;
      bool useCscIntAlinesFromGM=true;
      bool controlCscIntAlines=false;
      bool dumpCscIntAlines=false;
	  
      dbr =std::make_unique< RDBReaderAtlas>(m_pDetStore, m_pRDBAccess, oracleTag, oracleNode, dumpAlines, useCscIntAlinesFromGM, dumpCscIntAlines, nullptr);
	  
      dbr->setControlCscIntAlines(controlCscIntAlines);
    }



    // set here the flag deciding whether to include cutouts:
    // m_includeCutouts = 1 => include cutouts
    // m_includeCutouts = 0 => no cutouts
    m_manager->setCutoutsFlag(true);
    m_manager->setCutoutsBogFlag(true);
    mysql->setControlAlines(false);


    StatusCode sc = StatusCode::SUCCESS;

    dbr->setManager(getDetectorManager());
    sc = dbr->ProcessDB(*mysql);
    if (sc != StatusCode::SUCCESS) {
      log << MSG::ERROR << " FAILURE in DB access; Muon node will not be built" << endmsg;
      return;
    }
       
    // now store detector-generic-descriptors into the manager
    const RPC *r = dynamic_cast<const RPC*>(mysql->GetATechnology("RPC0"));
    GenericRPCCache rpcCache;
    rpcCache.stripSeparation = r->stripSeparation;
    rpcCache.stripPanelThickness = r->stripPanelThickness;
    rpcCache.rpcLayerThickness = r->rpcLayerThickness;
    rpcCache.centralSupPanelThickness = r->centralSupPanelThickness;
    rpcCache.GasGapThickness = r->GasGapThickness;
    rpcCache.frontendBoardWidth = r->frontendBoardWidth;
    m_manager->setGenericRpcDescriptor(rpcCache);

    const MDT *mdtobj = dynamic_cast<const MDT*>(mysql->GetATechnology("MDT0"));
    GenericMDTCache mdtCache;
    mdtCache.innerRadius = mdtobj->innerRadius;
    mdtCache.outerRadius = mdtobj->innerRadius + mdtobj->tubeWallThickness;

    m_manager->setGenericMdtDescriptor(mdtCache);

    const TGC *t = dynamic_cast<const TGC*>(mysql->GetATechnology("TGC0"));
    GenericTGCCache tgcCache;
    tgcCache.frame_h = t->frame_h;
    tgcCache.frame_ab = t->frame_ab;
    tgcCache.nlayers = t->nlayers;

    m_manager->setGenericTgcDescriptor(tgcCache);

    GenericCSCCache cscCache;
    m_manager->setGenericCscDescriptor(cscCache);

    GeoFullPhysVol *p4 = mapFPV["MuonTreeTop"];
    m_manager->addTreeTop(p4); // This is the top!
    log << MSG::INFO << " TreeTop added to the Manager" << endmsg;

    // Look through the map explicitly for Stations
    std::set<std::string> mappedStations;
    for (auto i: mapFPV) {
      size_t pos=i.first.find("_Station");
      if (pos!=std::string::npos) mappedStations.insert(i.first.substr(0,pos));
    }

    for (auto i: mappedStations) {
      auto it=std::find_if(mysql->StationBegin(),mysql->StationEnd(),[i](std::pair<std::string, Station *> s)->bool {return i==s.second->GetName();});
      if (it==mysql->StationEnd()) {
	throw std::runtime_error("Raw/readout geometry mismatch");
      }

      Station *station = (*it).second;
      std::string stname(station->GetName(), 0, 3);
  
      bool isAssembly = false;
      if (station->GetNrOfCutouts() > 0 && stname.substr(0, 1) != "T")
	isAssembly = true;

      // BIR have multilayers of diff. length and overall station volume clashes with toroids
      if (stname == "BIR")
	isAssembly = true;

      MuonChamberLite l(*mysql, station,&mapFPV,&mapAXF); // here is where we start to create a MuonChamber with all readoutelements

      PositionIterator pit;
      AlignPos ap;
      for ( auto const& [key, position] : *station) {
	int zi = position.zindex;
	int fi = position.phiindex;

	// here build the physical volume (tree) associated to the chamber
	if (stname == "CSL")  isAssembly = true;

	// CSL because coffin shape of the station mother volume
	l.addReadoutLayers(*mysql, getDetectorManager(), zi, fi, position.isMirrored, isAssembly);

	// here define the GeoAlignableTransform associated to the chamber
	// nominal transform first
	GeoAlignableTransform *xf = mapAXF[station->GetName()];

	// alignment issues and readout geometry for station
	MuonStation *mst = m_manager->getMuonStation(station->GetName(), zi, fi + 1);
	if (!mst) {
	  log << MSG::WARNING << "For Station with nameTag=<" << station->GetName() << "> at zi/fi = " << zi << "/" << fi
	      << " no MuonStation found => no possibility to align" << endmsg;
	  continue;
	}

	mst->setTransform(xf);
	GeoTrf::Transform3D tsz_to_szt = GeoTrf::RotateZ3D(-90 * Gaudi::Units::degree) * GeoTrf::RotateY3D(-90 * Gaudi::Units::degree);
	mst->setNativeToAmdbLRS(Amg::EigenTransformToCLHEP(tsz_to_szt * station->native_to_tsz_frame(*mysql, position)));
	mst->setNominalAmdbLRSToGlobal(Amg::EigenTransformToCLHEP(station->tsz_to_global_frame(*mysql, position) * tsz_to_szt.inverse()));

	int nAlines = station->CountAlignPos(zi, fi);
	if (nAlines == 0) {
	  ap.tras = ap.traz = ap.trat = ap.rots = ap.rotz = ap.rott = 0.;
	  ap.jobindex = 0;
	  mst->setDelta_fromAline(ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott);
	} else if (nAlines > 0) {
	  AlignPosIterator alast;
	  AlignPosIterator afirst = station->getFirstAlignPosInRange(zi, fi, alast);

	  for (AlignPosIterator acurrent = afirst; acurrent != alast; ++acurrent) {
	    ap = acurrent->second;
	    if (ap.phiindex != fi || ap.zindex != zi) {
	      log << MSG::ERROR << "Inconsistent AlignedPosition found in the static Geometry DB: aligPos.fi, zi = " << ap.phiindex << ", " << ap.zindex
		  << " for station " << station->GetName() << " at fi/zi = " << fi << "/" << zi << " AlignPos indices fi/zi " << fi << "/" << zi << endmsg;
	    }
	    if (ap.jobindex == 0) {
	      mst->setDelta_fromAline(ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott);
	    } else {
	      mst->setDelta_fromAline_forComp(ap.jobindex, ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott);
	    }
	  }
	}
      } // end loop on positions
    } // for ( it = sel.begin(); it != sel.end(); it++ ) {
    delete mysql.get();
  } // MuonDetectorFactoryLite::create

} // namespace MuonGM
