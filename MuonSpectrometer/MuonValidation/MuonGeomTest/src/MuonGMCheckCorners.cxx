/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonGeoModel code for finding global coordinates of the 8 corners of each 
 multilayer of every muon chamber
 -----------------------------------------
 Based on MuonGMCheck of stefspa
 Modified by lkashif to include determination of chamber corner coordinates
***************************************************************************/

#include "MuonGMCheckCorners.h"

#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"
#include "TrkSurfaces/Surface.h"

#include <fstream>
#include <sstream>

typedef std::istringstream mystream;

MuonGMCheckCorners::MuonGMCheckCorners(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm               ( name, pSvcLocator ),
    p_EventStore            ( 0 ),
    p_ActiveStore           ( 0 )
{

    m_check_surfaces = 0;
    declareProperty("check_surfaces", m_check_surfaces);

    m_check_mdt = 0;
    declareProperty("check_mdt", m_check_mdt);
    m_check_mdtrsmap = 0;
    declareProperty("buildMdtRegionSelectorMap", m_check_mdtrsmap);
    m_check_rpc = 0;
    declareProperty("check_rpc", m_check_rpc);
    m_check_rpcrsmap = 0;
    declareProperty("buildRpcRegionSelectorMap", m_check_rpcrsmap);
    m_check_tgc = 0;
    declareProperty("check_tgc", m_check_tgc);
    m_check_tgcrsmap = 0;
    declareProperty("buildTgcRegionSelectorMap", m_check_tgcrsmap);
    m_check_csc = 0;
    declareProperty("check_csc", m_check_csc);
    m_check_parent = 0;
    declareProperty("check_ParentStation", m_check_parent);

    m_print_level	   =	0;
    declareProperty("print_level",     m_print_level);
    m_mdtgood = 0;
    declareProperty("print_mdt_good_hits", m_mdtgood);
    m_rpcgood = 0;
    declareProperty("print_rpc_good_hits", m_rpcgood);
    m_tgcgood = 0;
    declareProperty("print_tgc_good_hits", m_tgcgood);
    
    m_station_index = 0;
    declareProperty("station_index", m_station_index);    
}

StatusCode
MuonGMCheckCorners::initialize()
{
    ATH_CHECK(AthAlgorithm::initialize());
    ATH_MSG_DEBUG("MuonGMCheckCorners::initialize()");
    // Locate the StoreGateSvc and initialize our local ptr
    ATH_CHECK(serviceLocator()->service("StoreGateSvc", p_EventStore));
    ATH_MSG_DEBUG("StoreGateSvc found");
    // Locate the ActiveStoreSvc and initialize our local ptr
    ATH_CHECK(serviceLocator()->service("ActiveStoreSvc", p_ActiveStore));
    ATH_MSG_DEBUG("ActiveStoreSvc found");

    if (m_check_csc) checkreadoutcscgeo();
    if (m_check_mdt) checkreadoutmdtgeo();
    if (m_check_tgc) checkreadouttgcgeo();
    if (m_check_rpc) checkreadoutrpcgeo();
    if (m_check_rpcrsmap) buildRpcRegionSelectorMap();
    if (m_check_mdtrsmap) buildMdtRegionSelectorMap();
    if (m_check_tgcrsmap) buildTgcRegionSelectorMap();
    if (m_check_parent) checkParentStation();
    
    // Access the output file
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");

    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/10");
    if ( !nt )    {    // Check if already booked
      nt = ntupleSvc()->book ("/NTUPLES/FILE1/10", CLID_ColumnWiseTuple, "Coords");
      if ( nt )    {
        ATH_MSG_DEBUG("booked ntuple");
        ATH_CHECK(nt->addItem ("ChamberPhi", m_statPhi));
        ATH_CHECK(nt->addItem ("Multilayer", m_statML));
        ATH_CHECK(nt->addItem ("Corner", m_MLcorner));
        ATH_CHECK(nt->addItem ("x", m_x));
        ATH_CHECK(nt->addItem ("y", m_y));
        ATH_CHECK(nt->addItem ("z", m_z));
      }
    } else {
        ATH_MSG_WARNING("Failed to book ntuple");
    }
    return StatusCode::SUCCESS;
}

void MuonGMCheckCorners::checkreadoutrpcgeo()
 {
     
 }
void MuonGMCheckCorners::checkParentStation()
{
    
}

void MuonGMCheckCorners::checkreadoutmdtgeo()
{
  
}

void MuonGMCheckCorners::checkreadouttgcgeo()
{
     
}
void MuonGMCheckCorners::checkreadoutcscgeo()
{
     
}
void MuonGMCheckCorners::buildRpcRegionSelectorMap()
{
   
}

void MuonGMCheckCorners::buildMdtRegionSelectorMap()
{
    
}
void MuonGMCheckCorners::buildTgcRegionSelectorMap()
{

}  
