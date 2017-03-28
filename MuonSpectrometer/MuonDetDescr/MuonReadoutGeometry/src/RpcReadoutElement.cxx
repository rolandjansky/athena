/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The Rpc detector = an assembly module = RPC in amdb 
 ----------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
***************************************************************************/


//<doc><file>	$Id: RpcReadoutElement.cxx,v 1.5 2009-05-13 15:03:47 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"

#define RpcReadout_verbose false

namespace MuonGM {


  RpcReadoutElement::RpcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
				       int zi, int fi, bool is_mirrored,
				       MuonDetectorManager* mgr)
    : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr),
      m_hasDEDontop(false), m_nphigasgaps(-1), m_netagasgaps(-1),
      m_gasgapssize(-9999.), m_gasgapzsize(-9999.), m_nphistrippanels(-1),
      m_netastrippanels(-1), m_nphistripsperpanel(-1), m_netastripsperpanel(-1),
      m_phistripwidth(-9999.), m_etastripwidth(-9999.), m_phistrippitch(-9999.),
      m_etastrippitch(-9999.), m_phistriplength(-9999.), m_etastriplength(-9999.),
      m_phipaneldead(-9999.), m_etapaneldead(-9999.), m_exthonthick(-9999.)
  {
    m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:RpcReadoutElement");
    //MsgStream log(Athena::getMessageSvc(), "MuGM:RpcReadoutElement");
    std::string gVersion = manager()->geometryVersion();

    // get the setting of the caching flag from the manager
    setCachingFlag(mgr->cachingFlag());

    m_dbR=m_dbZ=m_dbPhi=0;
    
    m_descratzneg  = false;
    if (zi<0 && !is_mirrored) m_descratzneg = true;

    setStationName(stName);

    if (mgr->MinimalGeoFlag() == 0) {
      if (GeoFullPhysVol* pvc = dynamic_cast<GeoFullPhysVol*> (pv)) {
	unsigned int nchildvol = pvc->getNChildVols();
	int lgg = 0;
	int llay = 0;
	std::string::size_type npos;
	for (unsigned ich=0; ich<nchildvol; ++ich) {
	  PVConstLink pc = pvc->getChildVol(ich);
	  std::string childname = (pc->getLogVol())->getName();
	  if ((npos = childname.find("layer")) != std::string::npos ) {
	    llay ++;
	    //std::cerr<<" navigating RPC named "
	    //       <<tname<<" child "
	    //       <<ich<<" is a layer with tag "<<llay<<std::endl;
	    unsigned int nch1 = pc->getNChildVols();
	    lgg = 0;
	    for (unsigned ngv=0; ngv<nch1; ++ngv) {
	      PVConstLink pcgv = pc->getChildVol(ngv);
	      std::string childname1 = (pcgv->getLogVol())->getName();
	      if ((npos = childname1.find("gas volume")) != std::string::npos ) {
		lgg ++;
		//std::cerr<<" navigating RPC named "
		//       <<tname<<" child "
		//       <<ngv<<" is a gas volume  with tag "<<lgg<<std::endl;
		PVConstLink pcgg = pcgv->getChildVol(0);
		HepGeom::Transform3D trans = pvc->getXToChildVol(ich)*pc->getXToChildVol(ngv)*pcgv->getXToChildVol(0);
		m_Xlg[llay-1][lgg-1] = Amg::CLHEPTransformToEigen(trans);
	      }
	    }
	  }
	}
      } else {
	std::cerr<<"Cannot perform a dynamic cast ! "<<std::endl;
      }
    }
    
  }


  RpcReadoutElement::~RpcReadoutElement()
  {
    clearCache();
  }


  double RpcReadoutElement::localStripSCoord(int doubletZ, int doubletPhi, int measphi, int strip) const
  {
    if ((doubletZ != m_dbZ && m_netastrippanels == 1)  ||
        (m_netastrippanels != 1 && (doubletZ <1 || doubletZ >m_netastrippanels))) {
      std::cerr<<"RpcReadoutElement::localStripSCoord doubletZ "
	       <<doubletZ<<" outside range 1-"<<m_netastrippanels
	       <<"/ created with m_dbZ="<<m_dbZ<<std::endl;
      /// here add debug printout
      std::cerr<<"RpcReadoutElement::localStripSCoord "<<" stName/Eta/Phi/dbR/dbZ/dbPhi "
	       <<getStationName()<<"/"<<getStationEta()<<"/"<<getStationPhi()<<"/"<<getDoubletR()<<"/"<<getDoubletZ()<<"/"<<getDoubletPhi()
	       <<std::endl;
      throw;
    }
    bool notintheribs = !inTheRibs();
    //std::cout<<" notintheribs "<<notintheribs<<std::endl;
    if ((doubletPhi != m_dbPhi && m_nphistrippanels == 1 && notintheribs ) ||
        (m_nphistrippanels != 1 && (doubletPhi < 1 || doubletPhi>m_nphistrippanels))) {
      std::cerr<<"RpcReadoutElement::localStripSCoord doubletPhi "
	       <<doubletPhi<<" outside range 1-"<<m_nphistrippanels
	       <<"/ created with m_dbPhi="<<m_dbPhi<<" for station "<<getStationName()
	       <<" and RPC type "<<getTechnologyName()<<std::endl;
      throw;
    }
    int maxstrip = 0;
    if (measphi==1) maxstrip = NphiStrips();
    else maxstrip = NetaStrips();    
    if ( strip<1 || strip>maxstrip ) {
      std::cerr<<"RpcReadoutElement::localStripSCoord strip "
	       <<strip<<" outside range 1-"<<maxstrip<<" for measphi="<<measphi<<" stName/Eta/Phi/dbR/dbZ/dbPhi "
	       <<getStationName()<<"/"<<getStationEta()<<"/"<<getStationPhi()<<"/"<<getDoubletR()<<"/"<<getDoubletZ()<<"/"<<getDoubletPhi()
	       <<std::endl;
      throw;
    }
    
    double local_s=0.;
    int dbphi = doubletPhi-1;
    if (m_nphistrippanels == 1) dbphi=0;
    if (measphi == 1) local_s = m_first_phistrip_s[dbphi]+(strip-1)*StripPitch(measphi);
    else  local_s = m_etastrip_s[dbphi];

    if (RpcReadout_verbose)
      std::cout<<"RpcReadoutElement:: Ssize, ndvs, nstr/pan, spitch, 1st-strp "<<m_Ssize<<" "
	       <<m_nphistrippanels<<" "<<m_nphistripsperpanel<<" "<< m_phistrippitch
	       <<" "<<m_first_phistrip_s[doubletPhi-1]<<std::endl;
    if (RpcReadout_verbose)
      std::cout<<"RpcReadoutElement::localStripSCoord local_s is "<<local_s<<" for dbZ/dbP/mphi/strip "
	       <<doubletZ<<" "<<doubletPhi<<" "<<measphi<<"/"<<strip<<std::endl;
    return local_s;
  }
  double RpcReadoutElement::localStripZCoord(int doubletZ, int doubletPhi, int measphi, int strip) const
  {
    if ((doubletZ != m_dbZ && m_netastrippanels == 1)  ||
        (m_netastrippanels != 1 && (doubletZ < 1 || doubletZ > m_netastrippanels ))) {
      std::cerr<<"RpcReadoutElement::localStripZCoord doubletZ "
	       <<doubletZ<<" outside range 1-"<<m_netastrippanels
	       <<"/ created with m_dbZ="<<m_dbZ<<std::endl;
      std::cerr<<"RpcReadoutElement::localStripZCoord  stName/Eta/Phi/dbR/dbZ/dbPhi "
	       <<getStationName()<<"/"<<getStationEta()<<"/"<<getStationPhi()<<"/"<<getDoubletR()<<"/"<<getDoubletZ()<<"/"<<getDoubletPhi()
	       <<std::endl;
      throw;
    }
    bool notintheribs = !inTheRibs();
    //std::cout<<" notintheribs "<<notintheribs<<std::endl;
    if ((doubletPhi != m_dbPhi && m_nphistrippanels == 1 && notintheribs ) ||
        (m_nphistrippanels != 1 && (doubletPhi < 1 || doubletPhi>m_nphistrippanels))) {
      std::cerr<<"RpcReadoutElement::localStripZCoord doubletPhi "
	       <<doubletPhi<<" outside range 1-"<<m_nphistrippanels
	       <<"/ created with m_dbPhi="<<m_dbPhi<<" stName/Eta/Phi/dbR/dbZ/dbPhi "
	       <<getStationName()<<"/"<<getStationEta()<<"/"<<getStationPhi()<<"/"<<getDoubletR()<<"/"<<getDoubletZ()<<"/"<<getDoubletPhi()
	       <<" and RPC type "<<getTechnologyName()<<std::endl;
      throw;
    }
    int maxstrip = 0;
    if (measphi==1) maxstrip = NphiStrips();
    else maxstrip = NetaStrips();
    if ( strip<1 || (measphi==1 && strip>maxstrip) ) {
      std::cerr<<"RpcReadoutElement::localStripZCoord strip "
	       <<strip<<" outside range 1-"<<maxstrip<<" for measphi="<<measphi
	       <<" stName/Eta/Phi/dbR/dbZ/dbPhi "
	       <<getStationName()<<"/"<<getStationEta()<<"/"<<getStationPhi()<<"/"<<getDoubletR()<<"/"<<getDoubletZ()<<"/"<<getDoubletPhi()<<std::endl;
      throw;
    }
    
    double local_z=0.;
    if (measphi == 0) {
      double xx = m_first_etastrip_z[0];
      if (m_netastrippanels>1 && doubletZ>1)  xx = m_first_etastrip_z[doubletZ-1];
      local_z = xx + (strip-1)*StripPitch(measphi);
    }
    else  {
      double xx = m_phistrip_z[0];
      if (m_netastrippanels>1 && doubletZ>1)  xx =  m_phistrip_z[doubletZ-1];
      local_z = xx;
    }
    
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement:: Zsize, ndvz, nstr/pan, zpitch, 1st-strp "
                                     <<m_Zsize<<" "
                                     <<m_netastrippanels<<" "<<m_netastripsperpanel<<" "<< m_etastrippitch
                                     <<" "<<m_first_etastrip_z[doubletZ-1]<<std::endl;
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localStripZCoord local_z is "
                                     <<local_z<<" for dbZ/dbP/mphi/strip "
                                     <<doubletZ<<" "<<doubletPhi<<" "<<measphi<<"/"<<strip<<std::endl;
    return local_z;
  }

  const Amg::Vector3D RpcReadoutElement::stripPos(int doubletR, int doubletZ,
							     int doubletPhi, int gasGap,
							     int measPhi, int strip) const
  {
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::stripPos for dbr/dbz/dbp/gg/mp/strip "
				     <<doubletR<<" "
				     <<doubletZ<<" "
				     <<doubletPhi<<" "
				     <<gasGap<<" "
				     <<measPhi<<" "
				     <<strip<<std::endl;
    
    // global position of a generic strip !!!!!

    const Amg::Vector3D localP = localStripPos(doubletR,    doubletZ,
							  doubletPhi,  gasGap,
							  measPhi,     strip);

    const Amg::Transform3D rpcTrans = absTransform();

    if (RpcReadout_verbose) {
      std::cout<<"RpcReadoutElement::stripPos got localStripPos "<<localP<<std::endl;
      Amg::Vector3D trans = rpcTrans*Amg::Vector3D(0.,0.,0);
      std::cout<<"RpcReadoutElement::stripPos gl. transl. R, phi "
	       <<sqrt(trans.x()*trans.x()+trans.y()*trans.y())<<" "
	       <<trans.phi()
	       <<" R-Rsize/2 "<<sqrt(trans.x()*trans.x()+trans.y()*trans.y())-m_Rsize/2.<<std::endl;
    }
    return rpcTrans * localP;
  }

  const Amg::Vector3D RpcReadoutElement::localStripPos(int doubletR, int doubletZ,
								  int doubletPhi, int gasGap,
								  int measPhi, int strip) const
  {
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localstripPos for dbr/dbz/dbp/gg/mp/strip "
				     <<doubletR<<" "
				     <<doubletZ<<" "
				     <<doubletPhi<<" "
				     <<gasGap<<" "
				     <<measPhi<<" "
				     <<strip<<std::endl;
    
    // global position of a generic strip !!!!!

    // local coordinates are defined for a module at pos. Z with DED on top !!!!
    // if such module is located at StationPhi=0 then xgeolocal//xglobal
    //                                                ygeolocal//yglobal
    //                                                zgeolocal//yglobal

    // if there's a DED at the bottom, the Rpc is rotated by 180deg around its local y axis
    // gg numbering is swapped
    // -> eta strip n. 1 (offline id) is "last" eta strip (local)
    int lstrip = strip;
    int lgg = gasGap;
    if (!m_hasDEDontop) {
      if (measPhi ==0) lstrip = NetaStrips()-strip+1;
      lgg++;
      if (lgg>2) lgg=1;
      if (RpcReadout_verbose)
	std::cout<<"RpcReadoutElement::localstripos  m_hasDEDontop ="<<m_hasDEDontop
		 <<" lstrip, lgg "<<lstrip<<" "<<lgg<<std::endl;
    }
    

    int ldoubletPhi = doubletPhi;
    int ldoubletZ = doubletZ;
    // if the station is mirrored, the Rpc is rotated by 180deg around its local x axis
    // numbering of phi strips must be reversed;
    // numbering of eta strips is unchanged;
    // numbering of doubletPhi must be reversed if m_nphistrippanels>1
    // numbering of doubletZ   is unchanged;
    if (isMirrored())
      {
        if (measPhi==1) lstrip = NphiStrips()-lstrip+1;
        if (m_nphistrippanels != 1) 
	  {
            ldoubletPhi = doubletPhi+1;
            if (ldoubletPhi>2) ldoubletPhi=1;
	  }
        if (RpcReadout_verbose)
	  std::cout<<"RpcReadoutElement::localstrippos  isMirrored ="<<isMirrored()
		   <<" lstrip, ldoubletPhi "<<lstrip<<" "<<ldoubletPhi<<std::endl;
      }

    // the special case of chambers at Z<0 but not mirrored
    // numbering of eta strips must be reversed;
    // numbering of phi strips is unchanged;
    // numbering of doubletZ must be reversed if  m_netastrippanels>1
    // numbering of doubletPhi   is unchanged;
    if (m_descratzneg)
      {
	//         std::cerr<<"Show measphi, doubletZ, strip "<<measPhi<<" "<<doubletZ<<" "<<strip;
	//         std::cerr<<" for chamber "<<getStationName()<<"/"<<getTechnologyName()<<std::endl;

        if (m_netastrippanels>1){
	  ldoubletZ++;
	  if (ldoubletZ > 2) ldoubletZ=1;
        }
        if (measPhi ==0) lstrip = NetaStrips()-lstrip+1;
        if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localstrippos special not mirrored at eta<0 ="
					 <<" lstrip, ldoublerZ "<<lstrip<<" "<<ldoubletZ<<std::endl;
      }
        
    Amg::Vector3D localP(
				    localGasGapDepth(lgg),
				    localStripSCoord(ldoubletZ, ldoubletPhi, measPhi, lstrip),
				    localStripZCoord(ldoubletZ, ldoubletPhi, measPhi, lstrip));

    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localstrippos = "<<localP<<std::endl;
    
    return localP;
  }

  double RpcReadoutElement::localGasGapDepth(int gasGap) const
  {
    const GenericRPCCache* r = manager()->getGenericRpcDescriptor();
    double xgg = - m_Rsize/2.;
    xgg = xgg + m_exthonthick + r->stripPanelThickness + r->GasGapThickness/2.;
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement:: localGasGapDepth 1st "<<xgg<<std::endl;
    if (gasGap == 1) return xgg;
    xgg = xgg + r->rpcLayerThickness + r->centralSupPanelThickness;
 
    if (RpcReadout_verbose) std::cout<<"RpcReadoutElement:: localGasGapDepth selected is "<<xgg<<std::endl;
    return xgg;
  }

  const Amg::Vector3D RpcReadoutElement::localStripPos(Identifier id) const
  {
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int doubletR   = idh->doubletR(id);
    int doubletZ   = idh->doubletZ(id);
    int doubletPhi = idh->doubletPhi(id);
    int gasgap     = idh->gasGap(id);
    int measPhi    = idh->measuresPhi(id);
    int strip      = idh->strip(id);

    return localStripPos(doubletR, doubletZ, doubletPhi, gasgap, measPhi, strip);
  }


  const Amg::Vector3D RpcReadoutElement::stripPos(Identifier id) const
  {
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int doubletR   = idh->doubletR(id);
    int doubletZ   = idh->doubletZ(id);
    int doubletPhi = idh->doubletPhi(id);
    int gasgap     = idh->gasGap(id);
    int measPhi    = idh->measuresPhi(id);
    int strip      = idh->strip(id);
    return stripPos(doubletR, doubletZ, doubletPhi, gasgap, measPhi, strip);
  }

  bool RpcReadoutElement::rotatedRpcModule() const
  {
    return (!m_hasDEDontop);
  }
  bool RpcReadoutElement::rotatedGasGap(Identifier id) const
  {
    return localTopGasGap(id);
  }
  bool RpcReadoutElement::rotatedGasGap(int gasGap) const
  {
    return localTopGasGap(gasGap);
  }

  bool RpcReadoutElement::localTopGasGap(int gasGap) const
  {
    // top gas gap is rotated around y => z coordinates are reversed

    // global position of a generic strip !!!!!

    // local coordinates are defined for a module at pos. Z with DED on top !!!!
    // if such module is located at StationPhi=0 then xgeolocal//xglobal
    //                                                ygeolocal//yglobal
    //                                                zgeolocal//yglobal

    // if there's a DED at the bottom, the Rpc is rotated by 180deg around its local y axis
    // gg numbering is swapped
    // -> eta strip n. 1 (offline id) is "last" eta strip (local)
    int lgg = gasGap;
    if (!m_hasDEDontop) {
      lgg++;
      if (lgg>2) lgg=1;
    }

    bool topgg = false;
    if (lgg==2) topgg=true;    
    return topgg;
  }

  bool RpcReadoutElement::localTopGasGap(Identifier id) const
  {
    // top gas gap is rotated around y => z coordinates are reversed
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int gasgap     = idh->gasGap(id);

    // global position of a generic strip !!!!!

    // local coordinates are defined for a module at pos. Z with DED on top !!!!
    // if such module is located at StationPhi=0 then xgeolocal//xglobal
    //                                                ygeolocal//yglobal
    //                                                zgeolocal//yglobal

    // if there's a DED at the bottom, the Rpc is rotated by 180deg around its local y axis
    // gg numbering is swapped
    // -> eta strip n. 1 (offline id) is "last" eta strip (local)
    int lgg = gasgap;
    if (!m_hasDEDontop) {
      lgg++;
      if (lgg>2) lgg=1;
    }

    bool topgg = false;
    if (lgg==2) topgg=true;    
    return topgg;
  }


  const Amg::Vector3D RpcReadoutElement::gasGapPos(Identifier id) const
  {
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int doubletZ   = idh->doubletZ(id);
    int doubletPhi = idh->doubletPhi(id);
    int gasgap     = idh->gasGap(id);
    return gasGapPos(doubletZ, doubletPhi, gasgap);
  }

  const Amg::Vector3D RpcReadoutElement::gasGapPos(int doubletZ, int doubletPhi, int gasgap) const
  {
    const Amg::Vector3D localP = localGasGapPos(doubletZ,
							   doubletPhi,
							   gasgap);
    
    const Amg::Transform3D rpcTrans = absTransform();
    
    if (RpcReadout_verbose) {
      std::cout<<"RpcReadoutElement::gasGapPos got localGasGapPos "<<localP<<std::endl;
    }
    return rpcTrans * localP;
  }
  const Amg::Vector3D RpcReadoutElement::localGasGapPos(Identifier id) const
  {
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int doubletZ   = idh->doubletZ(id);
    int doubletPhi = idh->doubletPhi(id);
    int gasgap     = idh->gasGap(id);

    return localGasGapPos(doubletZ, doubletPhi, gasgap);
  }
  const Amg::Vector3D RpcReadoutElement::localGasGapPos(int doubletZ, int doubletPhi, int gasgap) const
  {
    
    // global position of a generic strip !!!!!

    // local coordinates are defined for a module at pos. Z with DED on top !!!!
    // if such module is located at StationPhi=0 then xgeolocal//xglobal
    //                                                ygeolocal//yglobal
    //                                                zgeolocal//yglobal

    // if there's a DED at the bottom, the Rpc is rotated by 180deg around its local y axis
    // gg numbering is swapped
    // -> eta strip n. 1 (offline id) is "last" eta strip (local)
    int lgg = gasgap;
    if (!m_hasDEDontop) {
      lgg++;
      if (lgg>2) lgg=1;
      if (RpcReadout_verbose)
	std::cout<<"RpcReadoutElement::localgasgapPos  m_hasDEDontop ="<<m_hasDEDontop
		 <<" lgg "<<lgg<<std::endl;
    }
    

    int ldoubletPhi = doubletPhi;
    int ldoubletZ = doubletZ;
    // if the station is mirrored, the Rpc is rotated by 180deg around its local x axis
    // numbering of phi strips must be reversed;
    // numbering of eta strips is unchanged;
    // numbering of doubletPhi must be reversed if m_nphistrippanels>1
    // numbering of doubletZ   is unchanged;
    if (isMirrored())
      {
        if (m_nphistrippanels != 1) 
	  {
            ldoubletPhi = doubletPhi+1;
            if (ldoubletPhi>2) ldoubletPhi=1;
	  }
        if (RpcReadout_verbose)
	  std::cout<<"RpcReadoutElement::localgasgapPos  isMirrored ="<<isMirrored()
		   <<" ldoubletPhi "<<ldoubletPhi<<std::endl;
      }

    // the special case of chambers at Z<0 but not mirrored
    // numbering of eta strips must be reversed;
    // numbering of phi strips is unchanged;
    // numbering of doubletZ must be reversed if  m_netastrippanels>1
    // numbering of doubletPhi   is unchanged;
    if (m_descratzneg)
      {
        if (m_netastrippanels>1){
	  ldoubletZ++;
	  if (ldoubletZ > 2) ldoubletZ=1;
        }
        if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localgasgapPos  special not mirrored at eta<0 ="
					 <<" ldoublerZ "<<ldoubletZ<<std::endl;
      }


    double ggwidth = m_Ssize;
    double gglength = m_Zsize;
    int lggPhi = ldoubletPhi;
    int lggZ   = ldoubletZ;
    if (m_nphigasgaps == 1)
      {
        lggPhi = 1;
      }
    else
      {
        ggwidth = ggwidth/2.;
      }
    //std::cout<<" m_nphigasgaps, lggPhi, ggwidth "<<m_nphigasgaps<<" "<<lggPhi<<" "<<ggwidth<<std::endl;
    
    if (m_netagasgaps == 1)
      {
        lggZ = 1;
      }
    else 
      {
        gglength= gglength/2.;
      }
    //    std::cout<<" m_netagasgaps, lggZ, gglength "<<m_netagasgaps<<" "<<lggZ<<" "<<gglength<<std::endl;
    double local_s, local_z;
    if (m_nphigasgaps == 1) local_s = 0.;
    else local_s = -m_Ssize/4. + (lggPhi-1)*ggwidth;
    if (m_netagasgaps == 1) local_z = 0.;
    else local_z = -m_Zsize/4. + (lggZ  -1)*gglength;

    Amg::Vector3D localPold(localGasGapDepth(lgg), local_s, local_z);
    Amg::Vector3D localP1 = localPold;
    if (manager()->MinimalGeoFlag() == 0)
      {
       localP1 = m_Xlg[lgg-1][lggPhi-1].translation();
        if ( fabs(localP1.x()-localPold.x())>0.01 || fabs(localP1.y()-localPold.y())>0.01 || fabs(localP1.z()-localPold.z())>0.01 )
	  {
            const RpcIdHelper* idh = manager()->rpcIdHelper();
            reLog()<<MSG::WARNING
                   <<"LocalGasGapPos computed here doesn't match the one retrieved from the GeoPhysVol for rpc RE "
                   <<idh->show_to_string(identify())<<" and dbZ/dbPhi/gg "<<doubletZ<<"/"<<doubletPhi<<"/"<<gasgap<<endmsg;
            reLog()<<" Computed here "<<localPold<<" from GeoPhysVol "<<localP1<<endmsg; 
	  }
        else 
	  {
            if (reLog().level() <= MSG::VERBOSE) {
	      reLog()<<MSG::VERBOSE
		     <<"LocalGasGapPos computed here matches the one retrieved from the GeoPhysVol for rpc RE "
		     <<manager()->rpcIdHelper()->show_to_string(identify())<<" and dbZ/dbPhi/gg "<<doubletZ<<"/"<<doubletPhi<<"/"<<gasgap<<endmsg;
	      reLog()<<MSG::VERBOSE<<"Computed here "<<localPold<<" from GeoPhysVol "<<localP1<<endmsg; 
            }
	  }
        
      }
    //Amg::Vector3D localP(localP1.x(),localP1.y(),localP1.z());
    
    //     std::cout<<"RpcReadoutElement computed  gg-centre "<<localPold <<std::endl;
    //     std::cout<<"RpcReadoutElement raw-geom  gg-centre "<<localP<<std::endl;
    
    return localP1;
  }
  const Amg::Vector3D RpcReadoutElement::localStripPanelPos(int doubletZ, int doubletPhi, int gasgap) const
  {
    
    // global position of a generic strip !!!!!

    // local coordinates are defined for a module at pos. Z with DED on top !!!!
    // if such module is located at StationPhi=0 then xgeolocal//xglobal
    //                                                ygeolocal//yglobal
    //                                                zgeolocal//yglobal

    // if there's a DED at the bottom, the Rpc is rotated by 180deg around its local y axis
    // sp numbering is swapped
    // -> eta strip n. 1 (offline id) is "last" eta strip (local)
    int lsp = gasgap;
    if (!m_hasDEDontop) {
      lsp++;
      if (lsp>2) lsp=1;
      if (RpcReadout_verbose)
	std::cout<<"RpcReadoutElement::localStripPanelPos  m_hasDEDontop ="<<m_hasDEDontop
		 <<" lsp "<<lsp<<std::endl;
    }
    

    int ldoubletPhi = doubletPhi;
    int ldoubletZ = doubletZ;
    // if the station is mirrored, the Rpc is rotated by 180deg around its local x axis
    // numbering of phi strips must be reversed;
    // numbering of eta strips is unchanged;
    // numbering of doubletPhi must be reversed if m_nphistrippanels>1
    // numbering of doubletZ   is unchanged;
    if (isMirrored())
      {
        if (m_nphistrippanels != 1) 
	  {
            ldoubletPhi = doubletPhi+1;
            if (ldoubletPhi>2) ldoubletPhi=1;
	  }
        if (RpcReadout_verbose)
	  std::cout<<"RpcReadoutElement::localgasgapPos  isMirrored ="<<isMirrored()
		   <<" ldoubletPhi "<<ldoubletPhi<<std::endl;
      }

    // the special case of chambers at Z<0 but not mirrored
    // numbering of eta strips must be reversed;
    // numbering of phi strips is unchanged;
    // numbering of doubletZ must be reversed if  m_netastrippanels>1
    // numbering of doubletPhi   is unchanged;
    if (m_descratzneg)
      {
        if (m_netastrippanels>1){
	  ldoubletZ++;
	  if (ldoubletZ > 2) ldoubletZ=1;
        }
        if (RpcReadout_verbose) std::cout<<"RpcReadoutElement::localgasgapPos  special not mirrored at eta<0 ="
					 <<" ldoublerZ "<<ldoubletZ<<std::endl;
      }


    double spwidth = m_Ssize;
    double splength = m_Zsize;
    int lspPhi = ldoubletPhi;
    int lspZ   = ldoubletZ;
    if (m_nphistrippanels == 1)
      {
        lspPhi = 1;
      }
    else
      {
        spwidth = spwidth/2.;
      }
    //std::cout<<" m_nphistrippanels, lspPhi, spwidth "<<m_nphigasgaps<<" "<<lspPhi<<" "<<spwidth<<std::endl;
    
    if (m_netastrippanels == 1)
      {
        lspZ = 1;
      }
    else 
      {
        splength= splength/2.;
      }
    //    std::cout<<" m_netastrippanels, lspZ, splength "<<m_netagasgaps<<" "<<lspZ<<" "<<splength<<std::endl;
    double local_s, local_z;
    if (m_nphistrippanels == 1) local_s = 0.;
    else local_s = -m_Ssize/4. + (lspPhi-1)*spwidth;
    if (m_netastrippanels == 1) local_z = 0.;
    else local_z = -m_Zsize/4. + (lspZ  -1)*splength;

    Amg::Vector3D localP(localGasGapDepth(lsp), local_s, local_z);
    return localP;
  }


  const Amg::Vector3D RpcReadoutElement::SDtoModuleCoords(Amg::Vector3D x, Identifier id) const
  {
    const Amg::Vector3D gasgapP = localGasGapPos(id);
    const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
    if (rotatedGasGap(id)) return xfp*Amg::Vector3D(-x.x(), x.y(), -x.z());
    else return xfp*x;
  }
  const Amg::Vector3D RpcReadoutElement::localToGlobalCoords(Amg::Vector3D x, Identifier id) const
  {
    const Amg::Vector3D gasgapP = localGasGapPos(id);
    const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
    if (rotatedGasGap(id)) return absTransform()*xfp*Amg::Vector3D(-x.x(), x.y(), -x.z());
    else return absTransform()*xfp*x;
  }
  const Amg::Transform3D RpcReadoutElement::localToGlobalTransf(Identifier id) const
  {
    Amg::Vector3D gasgapP = localGasGapPos(id);
    Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
    Amg::Transform3D trans = absTransform();
    if (rotatedGasGap(id)) return trans*xfp*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(0.,1.,0.));
    else return trans*xfp;
  }
  const Amg::Transform3D RpcReadoutElement::localToGlobalStripPanelTransf(int dbZ, int dbPhi, int gasGap) const
  {
    const Amg::Vector3D locP = localStripPanelPos(dbZ, dbPhi, gasGap);
    //std::cout<<"LocaltoGlobalStripPanelTransf: id "<<manager()->rpcIdHelper()->show_to_string(identify())<<" dbZ, dbPhi, gasGap "<<dbZ<<" "<<dbPhi<<" "<<gasGap<<std::endl;
    //std::cout<<"strip panel pos "<<locP<<std::endl;
    //Amg::Vector3D gasgapP = localGasGapPos(dbZ, dbPhi, gasGap);
    //std::cout<<"gas   gap   pos "<<gasgapP<<std::endl;
    //if (fabs(locP.x()-gasgapP.x())>0.01 || fabs(locP.y()-gasgapP.y())>0.01 || fabs(locP.z()-gasgapP.z())>0.01) std::cout<<"look at me"<<std::endl;
    const Amg::Translation3D xfp(locP.x(),locP.y(), locP.z());
    if (rotatedGasGap(gasGap)) return absTransform()*xfp*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(0.,1.,0.));
    else return absTransform()*xfp;
  }
  const Amg::Transform3D RpcReadoutElement::localToGlobalTransf(int dbZ, int dbPhi, int gasGap) const
  {
    const Amg::Vector3D gasgapP = localGasGapPos(dbZ, dbPhi, gasGap);
    const Amg::Translation3D xfp(gasgapP.x(),gasgapP.y(), gasgapP.z());
    if (rotatedGasGap(gasGap)) return absTransform()*xfp*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(0.,1.,0.));
    else return absTransform()*xfp;
  }
  const Amg::Transform3D RpcReadoutElement::globalToLocalTransf(Identifier id) const
  {
    return localToGlobalTransf(id).inverse();
  }
  const Amg::Vector3D RpcReadoutElement::globalToLocalCoords(Amg::Vector3D x, Identifier id) const
  {
    return globalToLocalTransf(id)*x;
  }

  void  RpcReadoutElement::setIdentifier(Identifier id)
  {
    m_id = id;
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    IdentifierHash collIdhash = 0;
    IdentifierHash detIdhash = 0;
    // set parent data collection hash id
    int gethash_code = idh->get_module_hash(id, collIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
	     <<"RpcReadoutElement --  collection hash Id NOT computed for id = "
	     <<idh->show_to_string(id)<<std::endl;
    m_idhash = collIdhash;
    // set RE hash id 
    gethash_code = idh->get_detectorElement_hash(id, detIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
	     <<"RpcReadoutElement --  detectorElement hash Id NOT computed for id = "
	     <<idh->show_to_string(id)<<endmsg;
    m_detectorElIdhash = detIdhash;
  }



  double RpcReadoutElement::distanceToPhiReadout(const Amg::Vector3D& P, const Identifier& id) const
  {
    // P is a point in the global reference frame
    // we want to have the distance from the side of the phi readout (length travelled along a phi strip) from a signal produced at P)
    RpcReadoutSet Set(manager(), identify());
    int ndbz = Set.NdoubletZ();  
    double dist = -999.;
    
    if (ndbz == 1) {
      double zPoint = P.z();
      double zLow = REcenter().z() - getZsize()/2.;
      double zUp = REcenter().z() + getZsize()/2.;
      if (zPoint < zLow || zPoint > zUp) {
	//MsgStream log(m_msgSvc, "MuGM:RpcReadoutElement");
	const RpcIdHelper* idh = manager()->rpcIdHelper();
	reLog()<<MSG::DEBUG<<"RpcReadoutElement with id "<<idh->show_to_string(identify())
	       <<" ::distanceToPhiReadout --- z of the Point  "<<P.z()<<" is out of the rpc-module range ("<<zLow<<","<<zUp<<")"
	       <<" /// input id(never used) = "<<idh->show_to_string(id)<<endmsg;
	// return dist;
	if( zPoint < zLow ) zPoint = zLow;
	else if( zPoint > zUp ) zPoint = zUp;
      }
      if (sideC()) dist = zUp - zPoint;
      else dist = zPoint - zLow;

    } else {
      double zUp    = REcenter().z() + getZsize()/2.;
      double zLow   = REcenter().z() - getZsize()/2.;
      double zPoint = P.z();
      if (zPoint < zLow || zPoint > zUp) {
	//  MsgStream log(Athena::getMessageSvc(), "MuGM:RpcReadoutElement");
	const RpcIdHelper* idh = manager()->rpcIdHelper();
	reLog()<<MSG::DEBUG<<"RpcReadoutElement with id "<<idh->show_to_string(identify())
	       <<" ::distanceToPhiReadout --- z of the Point  "<<P.z()<<" is out of the rpc-module range ("<<zLow<<","<<zUp<<")"
	       <<" /// input id(never used) = "<<idh->show_to_string(id)<<endmsg;
	// return dist;
	if( zPoint < zLow ) zPoint = zLow;
	else if( zPoint > zUp ) zPoint = zUp;
      }
      if (m_dbZ == 1 || m_dbZ == 3) {
	if (sideC()) dist = zUp - zPoint;
	else
	  dist = zPoint - zLow;

      } else if (m_dbZ == 2) {
	if (sideC()) dist = zPoint - zLow;
	else
	  dist = zUp - zPoint;
      }        
    }            
    return dist;
  }

  double RpcReadoutElement::distanceToEtaReadout(const Amg::Vector3D& P) const
  {
    return distanceToEtaReadout(P, identify());
  }
  double RpcReadoutElement::distanceToPhiReadout(const Amg::Vector3D& P) const
  {
    return distanceToPhiReadout(P, identify());
  }

  double RpcReadoutElement::distanceToEtaReadout(const Amg::Vector3D& P, const Identifier& /*id*/) const
  {
    //id is actually never used !!!!!!!!!!!!!!!!!
    double dist = -999999.;
    double pAmdbL = GlobalToAmdbLRSCoords(P).x();
    double myCenterAmdbL = GlobalToAmdbLRSCoords(REcenter()).x();
    double sdistToCenter = pAmdbL - myCenterAmdbL;
    // std::cout<<"distanceToEtaReadout: point (ATLAS frame) is "<<P.x()<<" "<<P.y()<<" "<<P.z()<<" sSize of the chamber is "<<getSsize()<<std::endl;
    // std::cout<<"distanceToEtaReadout: x of the point the AMDB LRS "<<pAmdbL<<std::endl;
    // std::cout<<"distanceToEtaReadout: x RE-center of the AMDB LRS "<<myCenterAmdbL<<std::endl;
    // std::cout<<"distanceToEtaReadout: RE center()= "<<REcenter().x()<<" "<<REcenter().y()<<" "<<REcenter().z()<<std::endl;
    if (fabs(sdistToCenter)>getSsize()/2.) 
      {
        //MsgStream log(m_msgSvc, "MuGM:RpcReadoutElement");
        const RpcIdHelper* idh = manager()->rpcIdHelper();
        reLog()<<MSG::DEBUG<<"RpcReadoutElement with id "<<idh->show_to_string(identify())
	       <<" ::distanceToEtaReadout --- in amdb local frame x of the point  "<<pAmdbL<<" is out of the rpc-module range ("
	       <<myCenterAmdbL-getSsize()/2.<<","<<myCenterAmdbL+getSsize()/2.<<")"<<endmsg;
        // if (fabs(sdistToCenter)-getSsize()/2.>5*CLHEP::cm) {
        //   reLog()<<MSG::WARNING<<"by more then 5cm - distance will be set to -999999."<<endmsg;
        //   return dist;
        // }
        // else reLog()<<MSG::WARNING<<"computed distance might be negative"<<endmsg;
        if( sdistToCenter > 0 ) {
          sdistToCenter = getSsize()/2.;
          reLog()<<MSG::DEBUG<<"setting distance to "<< sdistToCenter <<endmsg;
        }
        else if ( sdistToCenter < 0 ) {
          sdistToCenter = -getSsize()/2.;
          reLog()<<MSG::DEBUG<<"setting distance to "<< sdistToCenter <<endmsg;
        }
      }    
    if (m_nphistrippanels == 2) {
      dist = getSsize()/2.-fabs(sdistToCenter);
    }
    else
      {
        // assumes readout is at smallest phi
        dist = getSsize()/2.+(sdistToCenter);
      }
    return dist;
  }


  void RpcReadoutElement::initDesign() {
    
    m_phiDesigns.reserve(NphiStripPanels());
    m_etaDesigns.reserve(NphiStripPanels());
    
    for( int i=1;i<=NphiStripPanels();++i){

      Amg::Transform3D gToSurf = MuonClusterReadoutElement::transform(surfaceHash(i,1,1)).inverse();
      Amg::Vector3D stripPos1 = stripPos(getDoubletR(),getDoubletZ(),i,1,1,1);
      Amg::Vector3D stripPos2 = stripPos(getDoubletR(),getDoubletZ(),i,1,1,2);
      Amg::Vector3D locStripPos1 = gToSurf*stripPos1;
      Amg::Vector3D locStripPos2 = gToSurf*stripPos2;

      Amg::Transform3D gToSurfEta = MuonClusterReadoutElement::transform(surfaceHash(i,1,0)).inverse();
      Amg::Vector3D stripPosEta1 = stripPos(getDoubletR(),getDoubletZ(),i,1,0,1);
      Amg::Vector3D stripPosEta2 = stripPos(getDoubletR(),getDoubletZ(),i,1,0,2);
      Amg::Vector3D locStripPosEta1 = gToSurfEta*stripPosEta1;
      Amg::Vector3D locStripPosEta2 = gToSurfEta*stripPosEta2;
      
      MuonStripDesign phiDesign;
      phiDesign.nstrips = NphiStrips();
      phiDesign.stripPitch = StripPitch(1);
      phiDesign.stripLength = StripLength(1);
      phiDesign.stripWidth = StripWidth(1);
    

      phiDesign.firstStripPos[0] = locStripPos1.x();
      phiDesign.firstStripPos[1] = locStripPos1.y();
      if( locStripPos2.x()-locStripPos1.x() < 0. ) phiDesign.stripPitch *= -1.;
      phiDesign.invStripPitch = 1./phiDesign.stripPitch;
      phiDesign.readoutLocY = 0.;
      phiDesign.signY = 0.;

      Amg::Vector2D pos1;
      phiDesign.stripPosition(1,pos1);
      Amg::Vector2D pos2;
      phiDesign.stripPosition(2,pos2);

      // now calculate distance to eta RO for the two phi strips 
//       double dist1 = distanceToPhiReadout(stripPosEta1);
//       double dist2 = distanceToPhiReadout(stripPosEta2);

//       Amg::Vector3D locS1Phi = gToSurf*stripPosEta1;
//       Amg::Vector3D locS2Phi = gToSurf*stripPosEta2;
      
//       double signRO = (locS2Phi.y()-locS1Phi.y())/(dist2-dist1);
//       double roPhi = locS1Phi.y() + signRO*dist1;
//       double roPhi2 = locS2Phi.y() + signRO*dist2;
//       phiDesign.readoutLocY = roPhi;
//       phiDesign.signY = signRO;

//       std::cout << " RO calc: d1 " << dist1 << " d2 " << dist2 << " lp1 " << locS1Phi << " lp2 " << locS2Phi 
// 		<< " signRO " << signRO << " ro1 " << roPhi << " ro2 " << roPhi2 << std::endl;
    
      if( fabs(pos1.x()-locStripPos1.x()) > 1e-6 ) {
 	std::cout << " bad local strip pos " << std::endl;
	std::cout << " phi local strip positions " << locStripPos1 << "   " << locStripPos2 
		  << " first strip " << phiDesign.firstStripPos << " pitch " << phiDesign.stripPitch
		  << " from calc " << locStripPos2.x()-locStripPos1.x() << std::endl;
	std::cout << " checking strip position: phi design  " << pos1 << " " << pos2 << std::endl;
      }

      m_phiDesigns.push_back(phiDesign);


      MuonStripDesign etaDesign;
      etaDesign.nstrips = NetaStrips();
      etaDesign.stripPitch = StripPitch(0);
      etaDesign.stripLength = StripLength(0);
      etaDesign.stripWidth = StripWidth(0);
    
      etaDesign.firstStripPos[0] = locStripPosEta1.x();
      etaDesign.firstStripPos[1] = locStripPosEta1.y();
      if( locStripPosEta2.x()-locStripPosEta1.x() < 0. ) etaDesign.stripPitch *= -1.;
      etaDesign.invStripPitch = 1./etaDesign.stripPitch;
      etaDesign.readoutLocY = 0.;
      etaDesign.signY = 0.;

      etaDesign.stripPosition(1,pos1);
      etaDesign.stripPosition(2,pos2);

      // now calculate distance to eta RO for the two phi strips 
//       double distEta1 = distanceToEtaReadout(stripPos1);
//       double distEta2 = distanceToEtaReadout(stripPos2);

//       Amg::Vector3D locS1Eta = gToSurf*stripPos1;
//       Amg::Vector3D locS2Eta = gToSurf*stripPos2;
      
//       double signROEta = (locS2Eta.y()-locS1Eta.y())/(distEta2-distEta1);
//       double roEta = locS1Eta.y() + signROEta*distEta1;
//       double roEta2 = locS2Eta.y() + signROEta*distEta2;
//       etaDesign.readoutLocY = roEta;
//       etaDesign.signY = signROEta;

//       std::cout << " RO calc: d1 " << distEta1 << " d2 " << distEta2 << " lp1 " << locS1Eta << " lp2 " << locS2Eta 
// 		<< " signRO " << signROEta << " ro1 " << roEta << " ro2 " << roEta2 << std::endl;
    
       if( fabs(pos1.x()-locStripPosEta1.x()) > 1e-6 ) {
 	std::cout << " bad local strip pos " << std::endl;
	std::cout << " eta local strip positions " << locStripPosEta1 << "   " << locStripPosEta2 
		  << " first strip " << etaDesign.firstStripPos << " pitch " << etaDesign.stripPitch
		  << " from calc " << locStripPosEta2.x()-locStripPosEta1.x() << std::endl;
	std::cout << " checking strip position: eta design  " << pos1 << " " << pos2 << std::endl;
       }

      m_etaDesigns.push_back(etaDesign);
    }
  }

  void RpcReadoutElement::fillCache() const
  {

    if( !m_surfaceData ) m_surfaceData = new SurfaceData();
    else{
      reLog()<<MSG::WARNING<<"calling fillCache on an already filled cache" << endmsg;
      return;
      //clearCache();
      //m_surfaceData = new SurfaceData();
    }
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    Identifier parentID = idh->parentID(identify());
    if( NetaStripPanels() != 1 ){
      std::cout << " aaarrgggg more than one eta strip pannel " << NetaStripPanels() << " "  << idh->print_to_string(identify()) << std::endl;
    }
    for( int dbPhi=1;dbPhi<=NphiStripPanels();++dbPhi ){
      for( int gasGap=1;gasGap<=numberOfLayers(true);++gasGap ){

	const Amg::Vector3D locP = localStripPanelPos(getDoubletZ(), dbPhi, gasGap);
	//const Amg::Translation3D xfp(locP.x(),locP.y(), locP.z());
	const Amg::Translation3D xfp(locP.x(),locP.y(), locP.z());
	Amg::Transform3D trans3D = rotatedGasGap(gasGap) ?  absTransform()*xfp*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) : absTransform()*xfp;

	// surface()
	bool hasSpecialRot = (rotatedGasGap(gasGap) && (!rotatedRpcModule())) || (!rotatedGasGap(gasGap) && (rotatedRpcModule()));
	Amg::RotationMatrix3D muonTRotation(trans3D.rotation());
	if (isMirrored())          muonTRotation = muonTRotation*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(1.,0.,0.));
	if ( hasSpecialRot )       muonTRotation = muonTRotation*Amg::AngleAxis3D(180.*CLHEP::deg,Amg::Vector3D(0.,1.,0.));
      
	Amg::RotationMatrix3D surfaceTRotation;
	surfaceTRotation.col(0) = muonTRotation.col(1);
	surfaceTRotation.col(1) = muonTRotation.col(2);
	surfaceTRotation.col(2) = muonTRotation.col(0);
      
	for( int measPhi=1;measPhi>=0;--measPhi ){
	
	  Identifier id = idh->channelID(parentID,getDoubletZ(), dbPhi, gasGap, measPhi,1);
	
	  Amg::Transform3D trans(surfaceTRotation);
	  if( measPhi == 0 ) trans *= Amg::AngleAxis3D(M_PI/2.,Amg::Vector3D(0.,0.,1.));
	  trans.pretranslate(trans3D.translation());

	  m_surfaceData->m_layerTransforms.push_back( trans );
	  m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );

	  if( measPhi == 1 ) {
	    m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back()*Amg::Vector3D(0.,0.,0.));
	    m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,1.));
	  }
	}	
      }
    }

    // phi at index=0
    m_surfaceData->m_surfBounds.push_back( new Trk::RectangleBounds( (m_Ssize/m_nphistrippanels)/2., (m_Zsize/m_netastrippanels)/2. ));
    m_surfaceData->m_surfBounds.push_back( new Trk::RectangleBounds( (m_Zsize/m_netastrippanels)/2., (m_Ssize/m_nphistrippanels)/2. ));
  }


  bool RpcReadoutElement::containsId(Identifier id) const
  {
    const RpcIdHelper* idh = manager()->rpcIdHelper();
    int doubletR   = idh->doubletR(id);
    if (doubletR != getDoubletR() ) return false;
    
    int doubletZ   = idh->doubletZ(id);
    if (doubletZ != getDoubletZ() ) {
      //std::cout<<" dbZ mismatch: doubletZ, getDoubletZ()  "<<doubletZ<<"  "<< getDoubletZ()<<std::endl;
      return false;
    }
    

    int doubletPhi = idh->doubletPhi(id);
    if ( doubletPhi != getDoubletPhi() && m_nphistrippanels == 1 ) {
      //std::cout<<" dbPhi mismatch: doubletPhi, getDoubletPhi()  "<<doubletPhi<<"  "<< getDoubletPhi()<<" m_nphistrippanels = "<<m_nphistrippanels<<std::endl;
      return false;
    }
    if ( doubletPhi < 1 || doubletPhi > m_nphistrippanels  )    {
      //std::cout<<" dbPhi mismatch: doubletPhi > m_nphistrippanels "<<doubletPhi<<" "<< m_nphistrippanels<<std::endl;
      if ( !(doubletPhi == 2 && inTheRibs()) ) {
	//std::cout<<"stName/tech "<<getStationName()<<"/"<<getTechnologyName()<<std::endl;
	return false;
      }
      //else std::cout<<" OK"<<std::endl;
    }
    
    
    int gasgap     = idh->gasGap(id);
    if (gasgap  <1 || gasgap >2) return false;
    
    int measPhi    = idh->measuresPhi(id);
    int strip      = idh->strip(id);
    if (measPhi == 0) 
      {
        if (strip <1 || strip > NetaStrips()) return false;
      }
    else if (measPhi == 1)
      {
        if (strip <1 || strip > NphiStrips()) return false;
      }
    else return false;    

    return true;
  }

  bool RpcReadoutElement::inTheRibs() const 
  {
    return (( getStationName().substr(0,3) == "BMS" ) &&
	    ( getTechnologyName() == "RPC07" || getTechnologyName() == "RPC08" ));
  }

  // **************************** interfaces related to Tracking *****************************************************)

} // namespace MuonGM
