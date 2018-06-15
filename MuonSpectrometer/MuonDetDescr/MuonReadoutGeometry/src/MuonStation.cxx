/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Holds the info related to a full station 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonStation.cxx,v 1.4 2009-04-08 17:51:02 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <iomanip>

namespace MuonGM {

MuonStation::MuonStation(std::string stName,
                         double Ssize, double Rsize, double Zsize,
                         double LongSsize, double LongRsize, double LongZsize, int zi, int fi, 
                         bool descratzneg)
{
    IMessageSvc* msgSvc=Athena::getMessageSvc();
    m_MsgStream = new MsgStream(msgSvc,"MuGM:MuonStation");
   
  m_statname    = stName;
  m_Ssize       = Ssize      ;
  m_Rsize       = Rsize      ;
  m_Zsize       = Zsize      ;
  m_LongSsize   = LongSsize  ;
  m_LongRsize   = LongRsize  ;
  m_LongZsize   = LongZsize  ;
  m_descratzneg = descratzneg;
  m_xAmdbCRO     = 0.;
 
  m_statEtaIndex = zi;
  m_statPhiIndex = fi;
  m_key = " ";
  m_transform       = NULL;
  m_native_to_amdbl = NULL;
  m_amdbl_to_global = NULL;
  m_delta_amdb_frame = NULL;
  m_rots = 0.;
  m_rotz = 0.;
  m_rott = 0.;
  m_hasALines = false;
  m_hasBLines = false;
  m_BlineFixedPointInAmdbLRS.setX(0.);
  m_BlineFixedPointInAmdbLRS.setY(0.);
  m_BlineFixedPointInAmdbLRS.setZ(0.);
  m_XTomoData = NULL;
    
  //m_msg = MsgStream(msgSvc, "MuGM:MuonStation");
  //  m_REinStation = new std::map<int, const MuonReadoutElement*>;  
  m_REwithAlTransfInStation = new std::map<int, pairRE_AlignTransf >;  
  m_firstRequestBlineFixedP = true; 
}


MuonStation::~MuonStation()
{
  delete m_native_to_amdbl;
  delete m_amdbl_to_global;
  delete m_delta_amdb_frame;
  delete m_REwithAlTransfInStation;
  delete m_MsgStream;
}
    
void MuonStation::setNominalAmdbLRSToGlobal(HepGeom::Transform3D xf)
{
    if (m_amdbl_to_global == NULL) m_amdbl_to_global = new HepGeom::Transform3D(xf);
    else *m_amdbl_to_global = xf;
    if ( reLog().level() <= MSG::DEBUG ) 
      reLog()<<MSG::DEBUG<<"setNominalAmdbLRSToGlobal: stationName/Jff/Jzz "
	     <<getStationType()<<" "<<getPhiIndex()<<" "<<getEtaIndex()<<" Origin of AmdbLocalFrame= "
	     <<(*m_amdbl_to_global)*HepGeom::Point3D<double>(0.,0.,0.)<<endmsg;
}
void MuonStation::setBlineFixedPointInAmdbLRS(double s0, double z0, double t0)
{
   if( reLog().level() <= MSG::DEBUG ) 
     {
       if ((fabs(s0)+fabs(z0)+fabs(t0))>0.01) reLog()<<MSG::DEBUG <<"Station "<<getStationType()
	      <<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	      <<" setting fixed point for B-lines at s0,z0,t0 =     "<<s0<<" "<<z0<<" "<<t0<<endmsg;
     }

  m_BlineFixedPointInAmdbLRS.setX(s0);
  m_BlineFixedPointInAmdbLRS.setY(z0);
  m_BlineFixedPointInAmdbLRS.setZ(t0);
  if ( reLog().level() <= MSG::DEBUG ) 
    {
      reLog()<<MSG::DEBUG<<"setBlineFixedPointInAmdbLRS: stationName/Jff/Jzz "
	<<getStationType()<<" "<<getPhiIndex()<<" "<<getEtaIndex()<<" nominal(i.e. from-station-envelop-only) B-line fixed point "<<m_BlineFixedPointInAmdbLRS<<endmsg;
    }

}

HepGeom::Point3D<double>  MuonStation::getBlineFixedPointInAmdbLRS() const
{
  // needed to update the station-level BlineFixedPoint with data from second multilayer
  return m_BlineFixedPointInAmdbLRS;
}

HepGeom::Point3D<double>  MuonStation::getUpdatedBlineFixedPointInAmdbLRS() const
{
  if (!m_firstRequestBlineFixedP) return m_BlineFixedPointInAmdbLRS;

  // Before correction m_BlineFixedPointInAmdbLRS has a z set at the edge of
  // lowest-z tube of the first layer of one of the two multilayers.
  // For endcap A, endcap C, and barrel A, this is correct, given the tube staggering
  // For barrel side C, given the tube staggering, the z should be at the
  // edge at the second layer, i.e. the z should be corrected by a half tube
  // pitch. Correction is thus computed only for barrel side C.
  if (barrel() && (getEtaIndex()<0)) {
    std::map< int, pairRE_AlignTransf >::const_iterator it=m_REwithAlTransfInStation->begin();
    for (; it!=m_REwithAlTransfInStation->end(); ++it) {
      const MuonReadoutElement* muonRE =  ((*it).second).first;
      if (muonRE->getTechnologyType()=="MDT") {
	const MdtReadoutElement* mdtRE = dynamic_cast<const MdtReadoutElement*> (muonRE);
	if (mdtRE) {
	  // Correct for tube staggering on barrel side C
	  double shiftInZ = -0.5 * mdtRE->tubePitch();

	  // in addition, correct for 35µm glue width incorrectly applied
	  double multilayerRealSize = 0;
	  for (int ilayer=1; ilayer<=2; ++ilayer) {
	    double val;
	    bool wellDefined = mdtRE->getWireFirstLocalCoordAlongZ(ilayer,val);
	    if (!wellDefined) {
	      reLog() << MSG::WARNING
		<< "getUpdatedBlineFixedPointInAmdbLRS: stationName/Jff/Jzz "
		<< getStationType() << " " << getPhiIndex() << " " << getEtaIndex()
		<< " cannot get wire coordinates for second tube layer" << endmsg;
	      val = 0.;
	    }
	    if ((ilayer==1) || (val > multilayerRealSize))
	      multilayerRealSize = val;
	  }
	  multilayerRealSize += (mdtRE->getNtubesperlayer()-1) * mdtRE->tubePitch();
	  multilayerRealSize += mdtRE->outerTubeRadius(); // last tube: no glue width
	  shiftInZ += mdtRE->getZsize() - multilayerRealSize;

	  m_BlineFixedPointInAmdbLRS.setY(m_BlineFixedPointInAmdbLRS.y()+shiftInZ);
	  if ( reLog().level() <= MSG::DEBUG ) {
	    reLog() << MSG::DEBUG
	      << "getUpdatedBlineFixedPointInAmdbLRS: stationName/Jff/Jzz "
	      << getStationType() << " " << getPhiIndex() << " " << getEtaIndex()
	      << " shiftInZ = " << shiftInZ
	      << " re-set B-line fixed point "
	      << m_BlineFixedPointInAmdbLRS.x() << ","
	      << m_BlineFixedPointInAmdbLRS.y() << ","
	      << m_BlineFixedPointInAmdbLRS.z() << endmsg;
	  }
	  break;
	} else {
	  reLog() << MSG::WARNING
	    << "getUpdatedBlineFixedPointInAmdbLRS: stationName/Jff/Jzz "
	    << getStationType() << " " << getPhiIndex() << " " << getEtaIndex()
	    << " failing to cast to const MdtReadoutElement* the RE named "
	    << muonRE->getStationName()
	    << " with tech=" << muonRE->getTechnologyName() << endmsg;
	}
      }
    }
  }


  m_firstRequestBlineFixedP = false;
  return m_BlineFixedPointInAmdbLRS;

}
void MuonStation::setDeltaAmdbLRS(HepGeom::Transform3D xf)
{
  if (m_delta_amdb_frame== NULL) m_delta_amdb_frame= new HepGeom::Transform3D(xf);
  else *m_delta_amdb_frame= xf;


  if( reLog().level() <= MSG::DEBUG ) {
      reLog()<<MSG::DEBUG <<"Station "<<getStationType()
			<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
			<<" adding Aline     "
			<<endmsg;
      reLog()<<MSG::DEBUG <<"  native_to_amdbl computed from A-line "<< endmsg <<
        (*m_native_to_amdbl)[0][0] << " " <<
        (*m_native_to_amdbl)[0][1] << " " <<
        (*m_native_to_amdbl)[0][2] << " " <<
        (*m_native_to_amdbl)[0][3] << " " << endmsg <<
        (*m_native_to_amdbl)[1][0] << " " <<
        (*m_native_to_amdbl)[1][1] << " " <<
        (*m_native_to_amdbl)[1][2] << " " <<
        (*m_native_to_amdbl)[1][3] << " " << endmsg <<
        (*m_native_to_amdbl)[2][0] << " " <<
        (*m_native_to_amdbl)[2][1] << " " <<
        (*m_native_to_amdbl)[2][2] << " " <<
        (*m_native_to_amdbl)[2][3] << " " <<endmsg;
      reLog()<<MSG::DEBUG <<"Station "
			<<"  amdbl_to_global "<< endmsg <<
	  (*m_amdbl_to_global)[0][0] << " " <<
	  (*m_amdbl_to_global)[0][1] << " " <<
	  (*m_amdbl_to_global)[0][2] << " " <<
	  (*m_amdbl_to_global)[0][3] << " " <<endmsg <<
	  (*m_amdbl_to_global)[1][0] << " " <<
	  (*m_amdbl_to_global)[1][1] << " " <<
	  (*m_amdbl_to_global)[1][2] << " " <<
	  (*m_amdbl_to_global)[1][3] << " " <<endmsg <<
	  (*m_amdbl_to_global)[2][0] << " " <<
	  (*m_amdbl_to_global)[2][1] << " " <<
	  (*m_amdbl_to_global)[2][2] << " " <<
	  (*m_amdbl_to_global)[2][3] << " " <<endmsg;
  }
  

  m_transform->setDelta( m_native_to_amdbl->inverse() * (*m_delta_amdb_frame) * (*m_native_to_amdbl) );
}

void 
MuonStation::setDelta_fromAline(double tras, double traz, double trat, 
                                double rots, double rotz, double rott)
{        
  // store here the angles of A-line
  m_rots = rots;
  m_rotz = rotz;
  m_rott = rott;
  
  HepGeom::Transform3D delta_amdb = HepGeom::Transform3D::Identity;
  if (fabs(tras)+fabs(traz)+fabs(trat)+(fabs(rots)+fabs(rotz)+fabs(rott))*1000. > 0.01)
  {
      // compute the delta transform in the local AMDB frame 
      delta_amdb = HepGeom::TranslateX3D(tras)*HepGeom::TranslateY3D(traz)*
                   HepGeom::TranslateZ3D(trat)*HepGeom::RotateX3D(rots)*
                   HepGeom::RotateY3D(rotz)*HepGeom::RotateZ3D(rott);
      m_hasALines = true;
  }
   
  // store the delta transform in the local AMDB frame 
  setDeltaAmdbLRS(delta_amdb);

  // debugging session 
  if ( reLog().level() <= MSG::DEBUG ) 
    {
      reLog()<<MSG::DEBUG <<"Station "<<getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	     <<" adding Aline     "
	     <<setiosflags(std::ios::fixed) << std::setprecision(6) << std::setw(12)
	     <<tras<<" "<<traz<<" "<<trat<<" "<<rots<<" "<<rotz<<" "<<rott<<endmsg;
      reLog()<<MSG::DEBUG <<"  delta_amdb computed from A-line "<< endmsg <<
        (delta_amdb)[0][0] << " " <<
        (delta_amdb)[0][1] << " " <<
        (delta_amdb)[0][2] << " " <<
        (delta_amdb)[0][3] << " " << endmsg <<
        (delta_amdb)[1][0] << " " <<
        (delta_amdb)[1][1] << " " <<
        (delta_amdb)[1][2] << " " <<
        (delta_amdb)[1][3] << " " << endmsg <<
        (delta_amdb)[2][0] << " " <<
        (delta_amdb)[2][1] << " " <<
        (delta_amdb)[2][2] << " " <<
        (delta_amdb)[2][3] << " " << endmsg;
    }
}

const MuonReadoutElement* MuonStation::getMuonReadoutElement(int jobIndex) const
{
  if (m_REwithAlTransfInStation->find(jobIndex)==m_REwithAlTransfInStation->end()) return NULL;
  if (reLog().level()<=MSG::VERBOSE) reLog()<<MSG::VERBOSE<<"getMuonReadoutElement at Job="<<jobIndex<<" for station "
					  <<getStationName()<<" at zi/fi = "<<getEtaIndex()<<"/"<<getPhiIndex()
					  <<endmsg;
  return ((*m_REwithAlTransfInStation)[jobIndex]).first;
}

GeoAlignableTransform* MuonStation::getComponentAlTransf(int jobIndex) const
{
  if (m_REwithAlTransfInStation->find(jobIndex)==m_REwithAlTransfInStation->end()) return NULL;
  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"getComponentAlTransf at Job="<<jobIndex<<" for station "
					  <<getStationName()<<" at zi/fi = "<<getEtaIndex()<<"/"<<getPhiIndex()
					  <<endmsg;
  return ((*m_REwithAlTransfInStation)[jobIndex]).second;
}

void 
MuonStation::addMuonReadoutElementWithAlTransf(const MuonReadoutElement* a, GeoAlignableTransform* ptrsf, int jobIndex)
{
  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"addMuonReadoutElementWithAlTransf for station "
					  <<getStationName()<<" at zi/fi = "<<getEtaIndex()<<"/"<<getPhiIndex()
					  <<" adding new component with Alignable transf... "<<a->getStationName()<<" job ondex = "<<jobIndex<<endmsg;
  pairRE_AlignTransf * myPair = new pairRE_AlignTransf(a,ptrsf);
  //if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<" my new Pair Location is "<<myPair<<endmsg;
  (*m_REwithAlTransfInStation)[jobIndex]= (*myPair);
  //(*m_REwithAlTransfInStation)[jobIndex]= pairRE_AlignTransf(a,ptrsf);
  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"addMuonReadoutElementWithAlTransf for station "
					  <<getStationName()<<" at zi/fi = "<<getEtaIndex()<<"/"<<getPhiIndex()
					  <<" added new component - now size of map is  "<<m_REwithAlTransfInStation->size()<<endmsg;
  delete myPair;
}

void 
MuonStation::setDelta_fromAline_forComp(int jobindex, 
					double tras, double traz, double trat, 
					double rots, double rotz, double rott)
{
  GeoAlignableTransform* parentToChild = getComponentAlTransf(jobindex);
  if (parentToChild == NULL) 
  {
      reLog()<<MSG::WARNING <<"setDelta_fromAline_forComp: WARNING: component for index "
	       <<jobindex<<" not found in MuonStation named "
	       <<getStationName()<<" at zi/fi = "<<getEtaIndex()<<"/"<<getPhiIndex()<<endmsg;
      return;
  }
  if (fabs(tras)+fabs(traz)+fabs(trat)+(fabs(rots)+fabs(rotz)+fabs(rott))*1000. < 0.01)
  {
    if ( reLog().level() <= MSG::DEBUG ) reLog()<<MSG::DEBUG <<"setDelta_fromAline_forComp: A-line ignored --- too small (translations < 10microns & rotations <10microrad)"<<endmsg;
      return;
  }


  //////////////////// this is what happens for a full station :    m_transform->setDelta( m_native_to_amdbl->inverse() * (*m_delta_amdb_frame) * (*m_native_to_amdbl) );

  HepGeom::Transform3D parentToChildT = parentToChild->getTransform();
  HepGeom::Transform3D delta_amdb     = HepGeom::TranslateX3D(tras)*HepGeom::TranslateY3D(traz)*HepGeom::TranslateZ3D(trat)*
                                  HepGeom::RotateX3D(rots)*HepGeom::RotateY3D(rotz)*HepGeom::RotateZ3D(rott);
  //The station to component transform is static and must be computed in terms of "nominal geometry parameters"; fixing here bug 87693 - SS 9/11/2011
  //HepGeom::Point3D<double> thisREcenter = getMuonReadoutElement(jobindex)->center();
  //std::cout<<" thisREcenter = "<<thisREcenter<<std::endl;
  //std::cout<<" corresponding center of defTRF thisREcenter = "<<(getMuonReadoutElement(jobindex)->defTransform())*HepGeom::Point3D<double>(0.,0.,0.)<<std::endl;
  HepGeom::Point3D<double> thisREnominalCenter=(getMuonReadoutElement(jobindex)->defTransformCLHEP())*HepGeom::Point3D<double>(0.,0.,0.); 
  double  Rcomp =  thisREnominalCenter.perp()-(getMuonReadoutElement(jobindex)->getRsize())/2.;
  double  DZcomp = fabs(thisREnominalCenter.z())-fabs(((*m_amdbl_to_global)*HepGeom::Point3D<double>(0.,0.,0)).z())-fabs((getMuonReadoutElement(jobindex)->getZsize())/2.);

  HepGeom::Transform3D  childToLocAmdbComponent;
  HepGeom::Transform3D  childToLocAmdbStation = HepGeom::Transform3D(*m_native_to_amdbl)*HepGeom::Transform3D(parentToChildT);
  HepGeom::Transform3D  locAmdbStatToLocAmdbComp = HepGeom::Transform3D::Identity;
  // the following line is needed to go for scenario B in last slide of http://www.fisica.unisalento.it/~spagnolo/allow_listing/TGC_Alines/TGC-ALines_2011_03_01.pdf
  // COMMENT next line            to go for scenario A in last slide of http://www.fisica.unisalento.it/~spagnolo/allow_listing/TGC_Alines/TGC-ALines_2011_03_01.pdf
  if  (getStationType().substr(0,1)=="T") locAmdbStatToLocAmdbComp = HepGeom::TranslateY3D(-Rcomp)*HepGeom::TranslateZ3D(-DZcomp);
  childToLocAmdbComponent = locAmdbStatToLocAmdbComp*childToLocAmdbStation;

  if ( reLog().level() <= MSG::DEBUG ) reLog()<<MSG::DEBUG
					      <<"setDelta_fromAline_forComp: stationName/Jff/Jzz "<<getStationType()<<" "<<getPhiIndex()<<" "<<getEtaIndex()<<" Job "<<jobindex
					      <<" Origin of component/station AmdbLocalFrame= "<<(*m_amdbl_to_global)*locAmdbStatToLocAmdbComp.inverse()*HepGeom::Point3D<double>(0.,0.,0.)
					      <<" / "<<(*m_amdbl_to_global)*HepGeom::Point3D<double>(0.,0.,0.)<<endmsg;
  parentToChild->setDelta(childToLocAmdbComponent.inverse() * delta_amdb * childToLocAmdbComponent);
  if ( reLog().level() <= MSG::DEBUG ) reLog()<<MSG::DEBUG
					      <<"setDelta_fromAline_forComp2:stationName/Jff/Jzz "<<getStationType()<<" "<<getPhiIndex()<<" "<<getEtaIndex()<<" Job "<<jobindex
					      <<" Origin of component/station AmdbLocalFrame= "<<(*m_amdbl_to_global)*locAmdbStatToLocAmdbComp.inverse()*HepGeom::Point3D<double>(0.,0.,0.)
					      <<" / "<<(*m_amdbl_to_global)*HepGeom::Point3D<double>(0.,0.,0.)<<endmsg;

  // debugging session 
  if (reLog().level()<=MSG::DEBUG ) 
  {
      reLog()<<MSG::DEBUG <<"Station "<<getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
		      <<" adding Aline     "
		      <<tras<<" "<<traz<<" "<<trat<<" "<<rots<<" "<<rotz<<" "<<rott
		      <<" for component with index ="<<jobindex<<endmsg;
      reLog()<<MSG::DEBUG <<"Station "<<getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
             <<"  delta_amdb computed from A-line "<< endmsg <<
        (delta_amdb)[0][0] << " " <<
        (delta_amdb)[0][1] << " " <<
        (delta_amdb)[0][2] << " " <<
        (delta_amdb)[0][3] << " " << endmsg <<
        (delta_amdb)[1][0] << " " <<
        (delta_amdb)[1][1] << " " <<
        (delta_amdb)[1][2] << " " <<
        (delta_amdb)[1][3] << " " << endmsg <<
        (delta_amdb)[2][0] << " " <<
        (delta_amdb)[2][1] << " " <<
        (delta_amdb)[2][2] << " " <<
        (delta_amdb)[2][3] << " " << endmsg;
  }
}


void MuonStation::clearCache() const
{
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();

  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"n. of RE in this station is "<<m_REwithAlTransfInStation->size()<<endmsg;
 
  int i = 0;
  for (;it!=itEnd; ++it) 
  {
    ++i;
    if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"Clearing cache .... for RE ... iteration n. "<<i<<endmsg;
      const MuonReadoutElement * re = ((*it).second).first;
      if (re==NULL){
	reLog()<<MSG::WARNING<<" in MuonStation:clearCache "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	       <<" trying to get a not existing RE (iteration n. )   "<<i<<" RE is null, skipping"<<endmsg;
	continue;
      }
      re->clearCache();
      if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"cache cleared "<<endmsg;
  }
}

void MuonStation::refreshCache() const
{
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();

  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"n. of RE in this station is "<<m_REwithAlTransfInStation->size()<<endmsg;
 
  int i = 0;
  for (;it!=itEnd; ++it) 
  {
    ++i;
    if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"Refreshing cache .... for RE ... iteration n. "<<i<<endmsg;
      const MuonReadoutElement * re = ((*it).second).first;
      if (re==NULL){
	reLog()<<MSG::WARNING<<" in MuonStation:refreshCache "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	       <<" trying to get a not existing RE (iteration n. )   "<<i<<" RE is null, skipping"<<endmsg;
	continue;
      }
      re->refreshCache();
  }
}


void MuonStation::fillCache() const
{
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();

  for (;it!=itEnd; it++) {
    const MuonReadoutElement * re = ((*it).second).first;
    if (re==NULL) {
	reLog()<<MSG::WARNING<<" in MuonStation:fillCache "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	       <<" trying to get a not existing RE, skipping"<<endmsg;
	continue;
      }
    re->fillCache();
  }
}

void MuonStation::setBline(BLinePar * bline) 
{
  m_hasBLines = true;
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();

  for (;it!=itEnd; ++it) 
  {
    const MuonReadoutElement * re = ((*it).second).first;
    if (re==NULL) 
      {
	reLog()<<MSG::WARNING<<" in setBLine "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	       <<" trying to get a null MuonReadoutElement, skipping"<<endmsg;
	continue;
      }
    if ( re->getTechnologyType().substr(0,3)=="MDT" )
    {
      const MdtReadoutElement* mdt = (const MdtReadoutElement*)re;
      mdt->setBLinePar(bline);
    }
  }
}

void MuonStation::clearBLineCache() const
{
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();
  int i=0;
  for (;it!=itEnd; ++it) 
  {
    ++i;
    const MuonReadoutElement * re = ((*it).second).first;
    if (re==NULL)  {
      reLog()<<MSG::WARNING<<" in MuonStation:clearBLineCache "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	     <<" trying to get a not existing RE (iteration n. )   "<<i<<" RE is null, skipping"<<endmsg;
      continue;
    }
    if ( re->getTechnologyType().substr(0,3)=="MDT" )
    {
      const MdtReadoutElement* mdt = (const MdtReadoutElement*)re;
      mdt->clearBLineCache();
    }
  }
}
void MuonStation::fillBLineCache() const
{
  std::map<int, pairRE_AlignTransf>::const_iterator it = m_REwithAlTransfInStation->begin();
  std::map<int, pairRE_AlignTransf>::const_iterator itEnd = m_REwithAlTransfInStation->end();

  for (;it!=itEnd; ++it) 
  {
    const MuonReadoutElement * re = ((*it).second).first;
    if (re==NULL)  {
      reLog()<<MSG::WARNING<<" in MuonStation:fillBLineCache "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()
	     <<" trying to get a non existing RE, skipping"<<endmsg;
      continue;
    }
    if ( re->getTechnologyType().substr(0,3)=="MDT" )
    {
      const MdtReadoutElement* mdt = (const MdtReadoutElement*)re;
      mdt->fillBLineCache();
    }
  }
}


double MuonStation::RsizeMdtStation() const
{
  if (getStationName().substr(0,1)=="T" || getStationName().substr(0,1)=="C") return 0.; // TGC and CSC stations
  double Rsize = 0.;

  Amg::Vector3D  RposFirst;
  Amg::Vector3D  Rpos;
  bool first = true;
  int nmdt=0;
 
  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"RsizeMdtStation for "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()<<" nRE = "<<nMuonReadoutElements()<<endmsg;
  for (int j=1; j<30; ++j)
    {
      const MuonReadoutElement* activeComponent = getMuonReadoutElement(j);
      if (activeComponent==NULL) continue;
      if (activeComponent->getTechnologyType()=="MDT") 
	{
	  ++nmdt;
	  
	  Rsize += activeComponent->getRsize()/2.;
	  Rpos   = activeComponent->toParentStation()*Amg::Vector3D(0.,0.,0.);
	  //std::cout<<" RsizeMdtStation: component "<<j<<" comp. Rsize, Rpos "<<activeComponent->getRsize()<<" "
	  //	   <<Rpos<<" barrel = "<<barrel()<<" first "<<first<<std::endl;
	  if (first) 
	    {
	      RposFirst = Rpos;
	      first=false;
	    }
	  else 
	    {
	      if (barrel()) Rsize += fabs(Rpos.x()-RposFirst.x());
	      else  Rsize += fabs(Rpos.y()-RposFirst.y());
	    }
	  //std::cout<<" RsizeMdtStation: component "<<j<<" station Rsize "<<Rsize<<std::endl;
	}
    }
  if (nmdt==1) Rsize=2.*Rsize;
  return Rsize;

}
double MuonStation::ZsizeMdtStation() const
{
  if (getStationName().substr(0,1)=="T" || getStationName().substr(0,1)=="C") return 0.; // TGC and CSC stations
  double Zsize = 0.;

  Amg::Vector3D  ZposFirst;
  Amg::Vector3D  Zpos;
  bool first = true;
  //std::cout<<" n. of comp.s "<<nMuonReadoutElements()<<std::endl;
  int nmdt=0;
  if (reLog().level()<=MSG::DEBUG) reLog()<<MSG::DEBUG<<"ZsizeMdtStation for "<< getStationType()<<" at zi/fi "<<getEtaIndex()<<"/"<<getPhiIndex()<<" nRE = "<<nMuonReadoutElements()<<endmsg;
 
  for (int j=1; j<30; ++j)
    {
      const MuonReadoutElement* activeComponent = getMuonReadoutElement(j);
      if (activeComponent==NULL) continue;
      if (activeComponent->getTechnologyType()=="MDT") 
	{
	  ++nmdt;
	  
	  Zsize += activeComponent->getZsize()/2.;
	  Zpos   = activeComponent->toParentStation()*Amg::Vector3D(0.,0.,0.);
	  //std::cout<<" ZsizeMdtStation: component "<<j<<" comp. Zsize, Zpos "<<activeComponent->getZsize()<<" "<<Zpos<<std::endl;	  
	  if (first) 
	    {
	      ZposFirst = Zpos;
	      first=false;
	    }
	  else 
	    {
	      if (barrel()) Zsize += fabs(Zpos.z()-ZposFirst.z());
	      else  Zsize += fabs(Zpos.x()-ZposFirst.x());
	    }
	  //std::cout<<" ZsizeMdtStation: component "<<j<<" station Zsize "<<Zsize<<std::endl;
	}
    }
  if (nmdt==1) Zsize=2.*Zsize;

  return Zsize;

}

bool MuonStation::barrel() const
{
    if (getStationName().substr(0,1)=="B") return true;
    return false;
}
bool MuonStation::endcap() const
{
  return !barrel();
}

MdtAsBuiltPar* MuonStation::getMdtAsBuiltParams() const {
   if (!hasMdtAsBuiltParams()) {
      reLog() << MSG::WARNING << "No Mdt AsBuilt parameters for chamber " << getStationName() << endmsg;
   }
   return m_XTomoData;
}

void MuonStation::setMdtAsBuiltParams(MdtAsBuiltPar* xtomo) {
   m_XTomoData = xtomo;
}

}


