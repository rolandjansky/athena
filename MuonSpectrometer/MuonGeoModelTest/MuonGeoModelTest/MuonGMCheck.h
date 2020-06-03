/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
 test MuonGeoModel from digits to pos. in space
 ----------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonGMCheck.h,v 1.15 2009-03-28 10:59:00 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_MUONGMCHECK_H
# define MUONGEOMODEL_MUONGMCHECK_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"


class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class sTgcIdHelper;
class MmIdHelper;
class Identifier;

namespace MuonGM
{    
  class MuonReadoutElement;
  class MuonDetectorManager;
  class RpcReadoutElement;
}

class MuonGMCheck: public AthAlgorithm
{
public:
    
    MuonGMCheck(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonGMCheck();
    
    StatusCode	initialize();
    StatusCode 	execute();
    StatusCode 	finalize();
    void clearCache() const;

private:
    // User setable properties
    int m_event_loop;
    int m_minimal_checks;
    
    int m_check_mdt;
    int m_check_rpc;
    int m_check_tgc;
    int m_check_csc;
    int m_check_stgc;
    int m_check_mm;

    int m_check_cscrsmap;
    int m_check_rpcrsmap;
    int m_check_mdtrsmap;
    int m_check_tgcrsmap;

    int m_testMdtCache;
    int m_testRpcCache;
    int m_testTgcCache;
    int m_testCscCache;
    
    int m_testMdtDetectorElementHash;
    int m_testRpcDetectorElementHash;
    int m_testTgcDetectorElementHash;
    int m_testCscDetectorElementHash;

    int m_check_first_last;
    int m_check_parent;
    int m_check_regsel;
    int m_check_blines;
    int m_check_surfaces;
    int m_check_surfaces_details;
    int m_check_rpc_distToReadout;
    int		m_print_level;
    int		m_tgcgood;
    int		m_rpcgood;
    int		m_mdtgood;

    

    const MuonGM::MuonDetectorManager*	p_MuonMgr;
    const RpcIdHelper*            p_RpcIdHelper;
    const TgcIdHelper*            p_TgcIdHelper;
    const CscIdHelper*            p_CscIdHelper;
    const MdtIdHelper*            p_MdtIdHelper;
    const sTgcIdHelper*           p_sTgcIdHelper;
    const MmIdHelper*             p_MmIdHelper;

    ToolHandle<MuonCalib::IIdToFixedIdTool> m_fixedIdTool;

    int m_mem; //<! counter for memory allocated VmSize values read from /proc/<pid>/status 
    int m_cpu[2]; //<! counter for cpu time read from /proc/<pid>/cpu
    void showVmemCpu(std::string message);
    void getVmemCpu(int& dVmem, int& duCpu, int& dsCpu);

    void testMdtCache();
    void testRpcCache();
    void testTgcCache();
    void testCscCache();
    void testMdtCache_here();
    void testRpcCache_here();
    void testTgcCache_here();
    void testCscCache_here();
    
    void test_MM_IdHelpers();
    void test_sTGC_IdHelpers();
    
    void getEtaPhiPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
				  double& etamin, double& etamax,
				  double& phimin, double& phimax);
    void getZPhiPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
				double& zmin, double& zmax,
				double& phimin, double& phimax);
    void getEtaPhiActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
					double& etamin, double& etamax,
					double& phimin, double& phimax);
    void getZPhiActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
				      double& zmin, double& zmax,
				      double& phimin, double& phimax);
    void getPanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
			    double& etamin, double& etamax,
			    double& phimin, double& phimax,
			    double& zmin, double& zmax);
    void getActivePanelBoundaries(const MuonGM::RpcReadoutElement* rpc, Identifier& chid,
				  double& etamin, double& etamax,
				  double& phimin, double& phimax,
				  double& zmin,   double& zmax);
    void getPanelEdgeCenter(const MuonGM::RpcReadoutElement* rpc, Identifier& chid, 
			    double& xC, double& yC, double& zC , 
			    double& xFirstPhiS, double& yFirstPhiS, double& zFirstPhiS, 
			    double& xLastPhiS, double& yLastPhiS, double& zLastPhiS);  
      
    void buildCscRegionSelectorMap();
    void buildRpcRegionSelectorMap();
    void buildMdtRegionSelectorMap();
    void buildTgcRegionSelectorMap();
    void checkreadoutstgcgeo();
    void checkreadoutmmgeo();
    void checkreadoutmdtgeo();
    void checkreadoutcscgeo();
    void checkreadoutrpcgeo();
    void checkParentStation();
    void checkreadouttgcgeo();
    void checkRegionSelectorMap();
//     void showPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc, Identifier id);
//     void showMdtPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc);
//     void showTgcPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc);

    void testMdtDetectorElementHash();
    void testRpcDetectorElementHash();
    void testTgcDetectorElementHash();
    void testCscDetectorElementHash();
    
    
};


#endif // MUONGEOMODEL_MUONGMCHECK_H
