/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/StandardComponent.h"
#include "MuonGeoModel/SupComponent.h"
#include "MuonGeoModel/TgcComponent.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <iostream>
#include <algorithm>
#include <cassert>

namespace MuonGM {

Station::Station(std::string s):m_name(s)
{
  m_msgSvc = Athena::getMessageSvc();
  MsgStream log(m_msgSvc, "MuonGeoModel");
    //    log << MSG::DEBUG<<"creating station "<<m_name<<" "<<this<<" with name "<<s<<endmsg;
    m_thickness=m_width1=m_width2=m_length=0;
    m_amdbOrigine_along_length = 0;
    m_amdbOrigine_along_thickness = 0;
    m_mdthalfpitch = 0.;
    MYSQL *mysql=MYSQL::GetPointer();
    m_hasMdts = false;
    mysql->StoreStation(this);
    //    log << MSG::DEBUG<<" stored in MYSQL "<<mysql<<endmsg;
}

Station::Station()
{
    m_msgSvc = Athena::getMessageSvc();
    m_thickness=m_width1=m_width2=m_length=0;
    m_amdbOrigine_along_length = 0;
    m_amdbOrigine_along_thickness = 0;
    m_mdthalfpitch = 0.;
    m_name = "unknown";
    m_hasMdts = false;
    MsgStream log(m_msgSvc, "MuonGeoModel");
    log << MSG::DEBUG << "Creating a Station..." << endmsg;
}

Station::Station(const Station& s)
{
  m_msgSvc = Athena::getMessageSvc();
	MsgStream log(m_msgSvc, "MuonGeoModel");
	log << MSG::DEBUG << "Creating a Station..." << endmsg;
	m_thickness=m_width1=m_width2=m_length=0;
        m_amdbOrigine_along_length = 0;
        m_amdbOrigine_along_thickness = 0;
	m_mdthalfpitch = 0.;
        m_hasMdts = s.m_hasMdts;
	m_name=s.m_name;
	for (unsigned int i=0;i<s.m_components.size();i++)
		m_components.push_back(s.m_components[i]);
	for (unsigned int i=0;i<s.m_cutouts.size();i++)
		m_cutouts.push_back(s.m_cutouts[i]);
}


Station::~Station()
{
    for (unsigned int i=0;i<m_components.size();i++)
    {
        delete m_components[i];
    }
    for (unsigned int i=0;i<m_cutouts.size();i++)
    {
        delete m_cutouts[i];
    }    
}

void Station::SetAlignPos(const AlignPos& p)
{
  MsgStream log(m_msgSvc, "MuonGeoModel");
  if (FindAlignPos(p.zindex, p.phiindex) != m_alignpositions.end() && p.jobindex==0) {
    log << MSG::WARNING<<" this alignposition already exists !!!"<<endmsg;
    log << MSG::WARNING<<" for station named "<<m_name
	     <<" setting alignposition at z,phi, key "
	     <<p.zindex<<" "
	     <<p.phiindex<<" "
	<<p.zindex*100+p.phiindex<<" and jobIndex = 0"<<endmsg;
    assert(0);
  }    
  int key=p.zindex*100+p.phiindex;
  //m_alignpositions[p.zindex*100+p.phiindex]=p;
  m_alignpositions.insert(std::pair<int, AlignPos>(key, p));

  /*
    log << MSG::DEBUG <<" setting position with key "<<
    p.zindex*100+p.phiindex<<" zi, phi = "
    <<p.zindex<<" "<<p.phiindex<<endmsg;
  */
}

AlignPosIterator Station::getFirstAlignPosInRange(int iz, int iphi, AlignPosIterator& lastAlignPosInRange) const
{
  int key = iz*100+iphi;
  std::pair<AlignPosIterator, AlignPosIterator> ppp = m_alignpositions.equal_range(key);
  lastAlignPosInRange = ppp.second;
  return ppp.first;
}

AlignPosIterator Station::FindAlignPos(int iz, int iphi) const
{
  // imt - probably needs to be different key for align pos
  int key = iz*100+iphi;
  //std::cout<<" looking for align pos. with key "<<key<<std::endl;
  return m_alignpositions.find(key);
}

int Station::CountAlignPos(int iz, int iphi) const
{
  // imt - probably needs to be different key for align pos
  int key = iz*100+iphi;
  //std::cout<<" looking for align pos. with key "<<key<<std::endl;
  return m_alignpositions.count(key);
}

AlignPosIterator Station::abegin() const
{
	return m_alignpositions.begin();
}
AlignPosIterator Station::aend() const
{
	return m_alignpositions.end();
}


void Station::SetComponent(Component* c)
{
	m_components.push_back(c);
}

void Station::SetCutout(Cutout* c)
{
	m_cutouts.push_back(c);
}

Component* Station::GetComponent(int i)  const 
{
	return m_components[i];
}

Cutout* Station::GetCutout(int i)  const 
{
	return m_cutouts[i];
}

void Station::SetPosition(Position p)
{
  MsgStream log(m_msgSvc, "MuonGeoModel");
    if (FindPosition(p.zindex, p.phiindex) != end()) {
        log << MSG::WARNING <<" this position already exists !!!"<<endmsg;
        log << MSG::WARNING
	    <<" for station named "<<m_name<<" setting position at z,phi, key "
                 <<p.zindex<<" "
                 <<p.phiindex<<" "
                 <<p.zindex*100+p.phiindex<<endmsg;
        assert(0);
    }
    else
    {
        p.isAssigned = true;
        m_positions[p.zindex*100+p.phiindex]=p;
        //     log << MSG::DEBUG<<" setting position with key "
	//         <<p.zindex*100+p.phiindex<<" zi, phi = "
        //              <<p.zindex<<" "<<p.phiindex<<endmsg;
    }
    
}

PositionIterator Station::FindPosition(int iz, int iphi) const
{
    int key = iz*100+iphi;
    //    std::cout<<" looking for pos. with key "<<key<<std::endl;
    return m_positions.find(key);
}
int Station::Npositions() const
{
    return m_positions.size();
}

PositionIterator Station::begin() const
{
	return m_positions.begin();
}
PositionIterator Station::end() const
{
	return m_positions.end();
}
std::string Station::GetName() const
{
    //	cout<<"name "<<m_name<<endl;
	return m_name;
}
double Station::GetThickness() const
{
  MsgStream log(m_msgSvc, "MuonGeoModel");
  std::string geov = MYSQL::GetPointer()->getGeometryVersion();
  //    log << MSG::DEBUG<<" station thickness for stat = "<<m_name<<endmsg;
    
  if (m_thickness) {
//        log << MSG::DEBUG<<" Station::thickness already defined = "<<m_thickness<<" for station "<<m_name<<endmsg;
    return m_thickness;
  }
    
  else 
    {
      //            log << MSG::DEBUG<<"    not defined yet "<<endmsg;
      double thick=0;
      if (m_name[0]=='T')
	{                    
	  for (unsigned int i=0;i<m_components.size();i++)
	    {
	      //std::string n=m_components[i]->m_name.substr(0,3);
	      //if (n=="TGC")
	      //{
	      TgcComponent *t=(TgcComponent *)m_components[i];
	      thick=thick > t->GetThickness()+t->posz ? thick : t->GetThickness()+t->posz;
	    }
	}
      else
        {
	  double zstart = 999999999.;
	  for (unsigned int i=0;i<m_components.size();i++)
	    {
	      StandardComponent *s=(StandardComponent *)m_components[i];
	      thick = thick > s->GetThickness()+s->posz ? thick : s->GetThickness()+s->posz;
	      if (i==0 || s->posz < zstart) zstart = s->posz;
              if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"Station "<<m_name<<" calculating  Thinkness = "<<thick<<" and zstart = "<<zstart<<endmsg;	
              
            }
	  if (fabs(zstart) > 0.001)
            {
	      //                 std::cout<<"getThickness for station "
	      //                          <<m_name<<" zstart = "
	      //                          <<zstart
	      //                          <<std::endl;
	      if (geov.substr(0,3) != "P03" ) {
                  // std::cout<<"station "<<m_name<<" zstart "<<zstart<<" redefining thick "<<thick<<" "<< thick - zstart<<std::endl;
                  thick = thick - zstart;
                  m_amdbOrigine_along_thickness = -zstart;
                  if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"Station "<<m_name<<" redefining Thinkness = "<<thick<<" because zstart = "<<zstart
                     <<"; then amdbOrigine_along_thickness = "<<m_amdbOrigine_along_thickness<<endmsg;
              }                
            }
        }
      m_thickness=thick;
      return thick;
    }
}
//                    //                log << MSG::DEBUG<<" component "<<s->m_name<<" found"<<endmsg;
//                     if ((s->m_name).substr(0,3) == "SUP") {
//                         //log << MSG::DEBUG<<" thick "<<s->GetThickness()<<" posz "<<s->posz<<endmsg;
//                         //thick > s->GetThickness() ? thick : s->GetThickness();
//                     }
//                    else 
//                    {
//                        thick=
//                            thick > s->GetThickness()+s->posz ? thick : s->GetThickness()+s->posz;
//                    }
//                     //log << MSG::DEBUG<<" thickness is now = "<<thick<<endmsg;
//                }
//            }
//            m_thickness=thick;
//            return thick;
//	}
//}
double Station::GetExtraTopThickness() const
{
  MsgStream log(m_msgSvc, "MuonGeoModel");
    //    log << MSG::DEBUG<<" GetExtraTopThickness for stat = "<<m_name<<endmsg;
    if (m_name[0] != 'B') return 0.;

    return 0.;
/* COMMENTING IS JUST TO MAKE COVERITY HAPPY 
   WHY THIS CODE WAS NEVER ENTERED HAS TO BE FIGURED OUT
    double xupsup = 0.;
    double deltaup = 0.;
    int nsup=0;
    for (unsigned int i=0;i<m_components.size();i++)
    {
        std::string n=m_components[i]->m_name.substr(0,3);
        if (n=="SUP")
        {
            ++nsup;
            SupComponent *s=(SupComponent *)m_components[i];
            deltaup = s->posz + s->topsizewrtAMDB0();
//             log << MSG::DEBUG<<" deltaup: s->posz, s->topsizewrtAMDB0 "
//                      <<s->posz<<" "
//                      <<s->topsizewrtAMDB0()<<endmsg;
            double pos = -m_thickness/2. + deltaup;
//            log << MSG::DEBUG<<" pos, m_thickness/2., deltaup "<<pos<<" "
//	    <<-m_thickness/2.<<" "<<deltaup<<endmsg;
            //xupsup > pos ? xupsup : pos;
            if (xupsup <= pos) xupsup=pos;
//             log << MSG::DEBUG<<" SUP component named "<<s->m_name
//                       <<" xupsup, m_thickness/2. ="<<xupsup<<" "<<m_thickness/2.<<endmsg;
        }
    }
    double dtop=xupsup - m_thickness/2.;
    //log << MSG::DEBUG<<" dtop = "<<dtop<<endmsg;
    if (dtop > 0.) {
      if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE<<" GetExtraTopThickness for stat = "
	  <<m_name<<" with "<<nsup<<" SUPs is "<<dtop<<endmsg;
        return dtop;
    }
    
    return 0.;
*/
}
double Station::GetExtraBottomThickness() const
{
  MsgStream log(m_msgSvc, "MuonGeoModel");
    //    log << MSG::DEBUG<<" GetExtraBottomThickness  for stat = "<<m_name<<endmsg;
    if (m_name[0] != 'B') return 0.;

    return 0.;
/* COMMENTING IS JUST TO MAKE COVERITY HAPPY 
   WHY THIS CODE WAS NEVER ENTERED HAS TO BE FIGURED OUT
    double xdownsup = 0.;
    double deltadown = 0.;
    int nsup =0;
    for (unsigned int i=0;i<m_components.size();i++)
    {
        std::string n=m_components[i]->m_name.substr(0,3);
        if (n=="SUP")
        {
            ++nsup;
            SupComponent *s=(SupComponent *)m_components[i];
            deltadown = s->posz - s->bottomsizewrtAMDB0();
//             log << MSG::DEBUG<<" deltadown: s->posz, s->bottomsizewrtAMDB0 "
//                      <<s->posz<<" "
//                      <<s->bottomsizewrtAMDB0()<<endmsg;
            double pos = -m_thickness/2. + deltadown;
            // log << MSG::DEBUG<<" pos, m_thickness/2., deltadown "<<pos
	    //	    <<" "<<-m_thickness/2.<<" "<<deltadown<<endmsg;
            //xdownsup < pos ? xdownsup : pos;
            if (xdownsup >=pos ) xdownsup=pos;
//             log << MSG::DEBUG<<" SUP component named "<<s->m_name
//                      <<" xdownsup, -m_thickness/2. ="<<xdownsup<<" "<<-m_thickness/2.<<endmsg;
        }
    }
    double dbottom = - xdownsup - m_thickness/2.;
    if (dbottom > 0.) {
      if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE <<" GetExtraBottomThickness for stat = "
	  <<m_name<<" with "<<nsup<<" SUPs is "<<dbottom<<endmsg;
        return dbottom;
    }
    return 0.;
*/
}
double Station::GetLength() const 
{
    MsgStream log(m_msgSvc, "MuonGeoModel-GetLength");
    MYSQL *mysql=MYSQL::GetPointer();
    if (m_length) {
        return m_length;
    }
    else
    {
        double len=0;
        if (m_name[0] == 'T')
        {
	    double innerrad = 99999.;
	    double outerrad = 0.;
            for (unsigned int i=0;i<m_components.size();i++)
            {
	      TgcComponent *tg=(TgcComponent *)m_components[i];
	      if (tg->posy < innerrad) {
		innerrad = tg->posy;
	      }
	      if (tg->posy+tg->dy > outerrad) {
		outerrad = tg->posy+tg->dy;
	      }
	    }
	    len = outerrad - innerrad;
        }
        else
        {
            double ystart = 999999.;
            for (unsigned int i=0;i<m_components.size();i++)
            {
                StandardComponent *sc=(StandardComponent *)m_components[i];
                //if (m_name == "EIL8" || m_name == "EIL9")
                if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"Station "<<m_name<<" *** comp "<<i<<" named "<<sc->name
                       <<" posy "<<sc->posy<<" dy "<<sc->dy<<" len "<<len<<" ystart "<<ystart<<endmsg;
                //if (m_name == "CSS1")std::cout<<" getLength() --- Station "<<m_name<<" *** comp "<<i<<" named "<<sc->m_name
                //       <<" posy "<<sc->posy<<" dy "<<sc->dy<<" len "<<len<<" ystart "<<ystart<<std::endl;
                if ((sc->dy + sc->posy) > len) len = sc->dy+sc->posy;
                if (i==0 || sc->posy < ystart ) ystart = sc->posy;
                //if (m_name == "EIL8" || m_name == "EIL9")
                if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<" now len = "<<len<<" ystart = "<<ystart<<endmsg;
                //if (m_name == "CSS1")std::cout<<"Now length = "<<len<<" ystart = "<<ystart<<std::endl;
            }
            if (fabs(ystart) > 0.001)
            {
//                 std::cout<<"getlength for station "
//                          <<m_name<<" ystart = "
//                          <<ystart
//                          <<std::endl;
                if (mysql->getGeometryVersion().substr(0,3) != "P03" ) {
                    // std::cout<<"station "<<m_name<<" ystart "<<ystart<<" redefining len "<<len<<" "<< len - ystart<<std::endl;
                    len = len - ystart;
		    m_amdbOrigine_along_length = -ystart;
                    //if (m_name == "EIL8" || m_name == "EIL9")
                    if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"Station "<<m_name<<" redefining len = "<<len<<" because ystart = "
                       <<ystart<<endmsg;
                }
            }
        }
//    else
//    {
//        //log << MSG::DEBUG<<" building length "<<endmsg;        
//        double len=0;
//        //std::cerr<<"Station "<<m_name<<" calculating the length "<<std::endl;
//        for (unsigned int i=0;i<m_components.size();i++)
//        {
//            StandardComponent *cc=(StandardComponent *)m_components[i];
//            std::string n=m_components[i]->m_name.substr(0,3);
//            if (n=="TGC")
//            {
//                len+=m_components[i]->dy;
//            }
//            else
//            {
//                
//                if (mysql->getGeometryVersion().substr(0,3) == "P03" )
//                {
//                    // this is the P03 implementation which is wrong in general,
//                    // due to the fact that sometimes  cc->posy can be <0 !!!
//                    if ((m_components[i]->dy+cc->posy) > len) len = m_components[i]->dy+cc->posy;
//                }
//                else
//                {
//                    if ((m_components[i]->dy+fabs(cc->posy)) > len) len = m_components[i]->dy+cc->posy;
//                    if (m_name.substr(0,2)=="CS") std::cout<<" station "<<m_name<<" comp. "<<m_components[i]->m_name
//                                                         <<" len, m_components[i]->dy, cc->posy "
//                                                         <<len<<" "
//                                                         <<m_components[i]->dy<<" "
//                                                         <<cc->posy<<std::endl;
//                    if (cc->posy < 0.) {
//                        len = len + fabs(cc->posy);
//                        std::cout<<" This is happening for station "<<m_name
//                                 <<" comp. "<<m_components[i]->m_name
//                                 <<" offset "<<cc->posy<<std::endl;
//                        if (m_name.substr(0,2)=="CS") std::cout<<"CORRECT station "<<m_name<<" comp. "<<m_components[i]->m_name
//                                                             <<" len, m_components[i]->dy, cc->posy "
//                                                             <<len<<" "
//                                                             <<m_components[i]->dy<<" "
//                                                             <<cc->posy<<std::endl;
//                    }
//                }
//            }            
//        }
        m_length = len;
        //if (m_name == "CSS1")std::cout<<"In the end length is = "<<m_length<<std::endl;
        return len;
    }
}

double Station::getYMin() const {
  if (m_name[0] != 'T') {
    double ystart = 999999.;
    for (unsigned int i=0; i<m_components.size(); i++) {
      StandardComponent* sc = (StandardComponent*) m_components[i];
      if (i==0 || sc->posy < ystart ) ystart = sc->posy;
    }
    return ystart;
  }
  return 0.;
}

double Station::GetWidth1() const 
{
	MsgStream log(m_msgSvc, "MuonGeoModel");
	if (m_width1) return m_width1;
	else
	{
            double maxdxmin = -99999.;
            double ymin= getYMin();
            //double ymax= GetLength()-getAmdbOrigine_along_length();
	  //            log << MSG::DEBUG<<" building m_width1"<<endmsg;
	        double w=0;
		for (unsigned int i=0;i<m_components.size();i++)
		{
                    std::string n=m_components[i]->name.substr(0,3);
                    if (n=="TGC")
                    {
		        double dw = 20.;
			std::string typetgc = m_components[i]->name.substr(3,2);
			// in case of station containing one module
			if (typetgc == "01" || typetgc == "06" ||
			    typetgc == "12" || typetgc == "18" ||
			    typetgc == "19" || typetgc == "20" ||
			    typetgc == "21" ) {
			  dw = 0.;
			}
                        if (w==0)
                        {
                            w=m_components[i]->dx1+dw;
                        }
                        else {
                            if (w>m_components[i]->dx1+dw)
                            {
                                w=m_components[i]->dx1+dw;
                            }
                        }
                    }
                    else
                    {
                        double dxmin = 0.;
                        if (fabs(m_components[i]->dy)<1.e-10) dxmin = m_components[i]->dx1;
                        else 
                        {
                            double num = (m_components[i]->dx2-m_components[i]->dx1)/2.;
                            double tantheta = num != 0 ? num / m_components[i]->dy : 0;
                            double y = ((StandardComponent*)m_components[i])->posy;
                            dxmin    =  m_components[i]->dx1 + 2.*tantheta * (ymin-y);
                        }
                        //std::cout<<"dxmin for component "<<dxmin<<" "<< m_components[i]->m_name<<" dx1, dx2, y, ymin "<<m_components[i]->dx1<<" "<<m_components[i]->dx2<<" "<<y<<" "<<ymin<<std::endl;
                        if ( maxdxmin < dxmin && (n.substr(0,2) != "LB" || m_name[0]=='B') ) maxdxmin = dxmin;
                        //std::cout<<"Maxdxmin = "<<maxdxmin<<std::endl;

                        /* 060602007 fix envelop for trapezoidal m_components shifted one w.r.t. the other
                        if (w<m_components[i]->dx1)
                        {
                            // imt do NOT let dx1 of the longer LB
                            //     determine m_width1 of EC chambers!
                            if (n.substr(0,2) != "LB" || m_name[0]=='B')
                            {
                                w=m_components[i]->dx1;
                                //std::cout<<" calculating short width "<<w<<" after component "<<n<<std::endl;
                            }
                            else
                            {
                                log << MSG::VERBOSE
                                    << "Component "
                                    << m_components[i]->m_name
                                    << " has longer dx1 but"
                                    << " is not at narrow point"
                                    << " so should not determine"
                                    << " short width of endcap station "
                                    << m_name
                                    <<  endmsg;
                            }
                            }*/
                    }
		}
//                 if (m_name.substr(0,1)!="T")
//                 {
//                     std::cout<<"shortWidth of station "<<m_name<<" std = "<<w<<" new "<<maxdxmin<<std::endl;
//                     if (fabs(maxdxmin - w)>0.01) std::cout<<" AAAAAAAAAAAAAAAAAAAAATTENZIONE abs(maxdxmin - w)="<<fabs(maxdxmin - w)<<" in station "<<m_name<<std::endl;
//                 }
		if (m_name.substr(0,1)=="T")m_width1=w;
                else m_width1=maxdxmin;
                //std::cout<<"Station m_name is "<<m_name<<" m_width1 = "<<m_width1<<std::endl;
                return m_width1;
	}
}
double Station::GetWidth2() const
{
    MsgStream log(m_msgSvc, "MuonGeoModel");
    if (m_width2) return m_width2;
    else
    {
        //double ymin= -getAmdbOrigine_along_length();
        double ymax= getYMin() + GetLength();
        double maxdxmax = -99999.;
        double w=0;
        for (unsigned int i=0;i<m_components.size();i++)
        {
            if (w< m_components[i]->dx2) 	 
            { 	 
                w=m_components[i]->dx2; 	 
                //std::cout<<" calculating long width "<<w<<" after component "<<m_components[i]->m_name<<std::endl; 	 
            }
            std::string n=m_components[i]->name.substr(0,3);
            if (n == "TGC") {
                double dw = 20.;
                std::string typetgc = m_components[i]->name.substr(3,2);
                // in case of one station containing one module
                if (typetgc == "01" || typetgc == "06" ||
                    typetgc == "12" || typetgc == "18" ||
                    typetgc == "19" || typetgc == "20" ||
                    typetgc == "21" ) {
                    dw = 0.;
                }
                w += dw;
            }
            else 
            {
                /* fix envelope for trapezoidal m_components shifted one w.r.t. the other
                if (w< m_components[i]->dx2)
                {
                    w=m_components[i]->dx2;
                    //std::cout<<" calculating long width "<<w<<" after component "<<m_components[i]->m_name<<std::endl;
                    }*/
                double dxmax = 0.;
                if (fabs(m_components[i]->dy)<1.e-10) dxmax =  m_components[i]->dx2;
                else 
                {
                    double num = (m_components[i]->dx2-m_components[i]->dx1)/2.;
                    double tantheta = num != 0 ? num / m_components[i]->dy : 0;
                    double y = ((StandardComponent*)m_components[i])->posy;
                    dxmax    =  m_components[i]->dx1 + 2.*tantheta * (ymax-y);
                }
//                 std::cout<<"dxmax for component "<<dxmax<<" "<< m_components[i]->m_name
//                          <<" dx1, dx2, y, ymax "<<m_components[i]->dx1<<" "<<m_components[i]->dx2
//                          <<" "<<y<<" "<<ymax<<" m_length = "<<GetLength()<<std::endl;
                if (maxdxmax < dxmax ) maxdxmax = dxmax;
//                 std::cout<<"Maxdxmax = "<<maxdxmax<<std::endl;
            }                       
        }
//         if (m_name.substr(0,1)!="T")
//         {
//             std::cout<<"longWidth of station "<<m_name<<" std = "<<w<<" new "<<maxdxmax<<std::endl;
//             if (fabs(maxdxmax - w)>0.01) std::cout<<" AAAAAAAAAAAAAAAAAAAAATTENZIONE abs(maxdxmax - w)="<<fabs(maxdxmax - w)<<" in station "<<m_name<<std::endl;
//         }
        if (m_name.substr(0,1)=="T") m_width2=w;
        else m_width2=maxdxmax;
        //        log << MSG::DEBUG<<" it is "<<w<<endmsg;
        //std::cout<<"Station m_name is "<<m_name<<" m_width2 = "<<m_width2<<std::endl;
        return m_width2;
    }
}

int Station::GetNrOfComponents() const
{
	return m_components.size();
}

int Station::GetNrOfCutouts() const
{
	return m_cutouts.size();
}

std::ostream& operator<<(std::ostream& os,const Station& s)
{
	os<<"Station m_name: "<<s.m_name<<" "<<s.m_components.size()<<std::endl;
	for (unsigned int i=0;i<s.m_components.size();i++)
		os<<"\t"<<s.m_components[i]<<std::endl;
	PositionIterator k;
	for (k=s.begin();k!=s.end();k++)
		os<<"\t\t"<<(*k).second<<std::endl;
	AlignPosIterator ak;
	for (ak=s.abegin();ak!=s.aend();ak++)
		os<<"\t\t"<<(*ak).second<<std::endl;
	os<<"--------------------------------------------------"<<std::endl;
	return os;
}


double Station::mdtHalfPitch() const {
  if (m_mdthalfpitch) return m_mdthalfpitch;

  MsgStream log(m_msgSvc, "mdtHalfPitch");
  MYSQL* mysql=MYSQL::GetPointer();
  MDT       *mdtobj= (MDT *)mysql->GetATechnology("MDT0");
  m_mdthalfpitch = 0.5 * (mdtobj->pitch);
  if (hasMdts()) {
    for (int icomp = 0; icomp<GetNrOfComponents(); ++icomp) {
      const Component* c = GetComponent(icomp);
      if (c->name.substr(0,3) != "MDT") continue;
      MDT* mdtobj= (MDT *)mysql->GetATechnology(c->name);
      if (!mdtobj) {
	log << MSG::ERROR << "Cannot find MDT definition for component " << c->name << std::endl;
	continue;
      }
      m_mdthalfpitch = 0.5 * (mdtobj->pitch);
      log << MSG::DEBUG << "Setting halfpitch " << m_mdthalfpitch << " for station " << m_name << std::endl;
      break;
    }
  }
  return m_mdthalfpitch;
}


//this is really needed 
HepGeom::Transform3D Station::native_to_tsz_frame( const Position & p ) const {

    MsgStream log(m_msgSvc, "MGM-native_to_tsz");
	const bool dLvl = log.level()<=MSG::DEBUG;
	const bool vLvl = log.level()<=MSG::VERBOSE;
    std::string geov = MYSQL::GetPointer()->getGeometryVersion();
    int amdbVersion = MYSQL::GetPointer()->getNovaReadVersion();
    if (amdbVersion > 0 && amdbVersion < 7 && m_name[0]!='B')
      if (dLvl)
	  log << MSG::DEBUG << "For AMDB version " << amdbVersion
		<< " a left-handed chamber coordinate system was used "
		<< " for endcap side A so be very careful."
		<< endmsg;
    
    HepGeom::Transform3D nominalTransf;
    
    // first apply here the mirror symmetry: (we, in fact, apply a rotation)
    HepGeom::Transform3D mirrsym=HepGeom::Transform3D::Identity;
    if (p.isMirrored) 
    {
        if (m_name[0]=='B' )
        {
            mirrsym = HepGeom::RotateX3D(180.*CLHEP::deg);
        }
    }

    // define the translation to position the chamber in the tzs frame
    HepGeom::Translate3D AMDBorgTranslation;
    if ( (m_name[0]=='B' || p.isBarrelLike) && p.zindex<0 && (!p.isMirrored) && hasMdts())
    {
        //MYSQL* mysql=MYSQL::GetPointer();
        //MDT       *mdtobj= (MDT *)mysql->GetATechnology("MDT0");
        //double halfpitch = (mdtobj->pitch)/2.;
	double halfpitch = mdtHalfPitch();
        AMDBorgTranslation =
            HepGeom::Translate3D(GetThickness()/2.-getAmdbOrigine_along_thickness(),
                           0.,
			   GetLength()/2.-(getAmdbOrigine_along_length()+halfpitch));
                           //GetLength()/2.-(halfpitch));
        if (vLvl)
		log<<MSG::VERBOSE<<" GetThickness / getAmdbO_thick / GetLength() / getAmdbO_length "
           <<GetThickness()<<" "
           <<getAmdbOrigine_along_thickness()<<" "
           <<GetLength()<<" "
           <<getAmdbOrigine_along_length()+halfpitch<<endmsg;
    }
    else
    {
      if (m_name[0]=='T')
        AMDBorgTranslation =
            HepGeom::Translate3D(GetThickness()/2.-getAmdbOrigine_along_thickness(),
                           0.,
                           GetLength()/2.-getAmdbOrigine_along_length()+((TgcComponent *)GetComponent(0))->posy);
      else
        AMDBorgTranslation =
            HepGeom::Translate3D(GetThickness()/2.-getAmdbOrigine_along_thickness(),
                           0.,
                           GetLength()/2.-getAmdbOrigine_along_length());
      if (vLvl)
        log<<MSG::VERBOSE<<" GetThickness / getAmdbO_thick / GetLength() / getAmdbO_length "
           <<GetThickness()<<" "
           <<getAmdbOrigine_along_thickness()<<" "
           <<GetLength()<<" "
           <<getAmdbOrigine_along_length()<<endmsg;
    }

    // // define the rotations by alpha, beta, gamma
    // HepGeom::Rotate3D ralpha = HepGeom::RotateX3D(p.alpha*CLHEP::deg);
    // HepGeom::Rotate3D rbeta  = HepGeom::RotateZ3D(p.beta*CLHEP::deg);
    // HepGeom::Rotate3D rgamma;
    // if (m_name[0]=='B' || p.isBarrelLike  || geov != "CTB2004") // pay attention here !!!!!!!!!
    // {
    //     rgamma = HepGeom::RotateY3D(p.gamma*CLHEP::deg);
    //     log<<MSG::VERBOSE<<" gamma is not changing sign - original "<<p.gamma<<" new one "<<p.gamma<<endmsg;
    // }
    // else
    // {
    //     log<<MSG::VERBOSE<<" gamma is changing sign - original "<<p.gamma<<" new one "<<-p.gamma<<endmsg;
    //     rgamma = HepGeom::RotateY3D(-p.gamma*CLHEP::deg);  //WHY???
    // }
    // log<<MSG::VERBOSE<<" alpha / beta "<<p.alpha<<" "<<p.beta<<endmsg;

    // // apply all transform in sequence 
    // //    HepGeom::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;  // works for barrel and barrel-like
    // // imt: tested for CTB2004, seems to work for all amdb versions...
    // HepGeom::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;
    HepGeom::Transform3D to_tsz = AMDBorgTranslation*mirrsym;

    return to_tsz;
}
HepGeom::Transform3D Station::tsz_to_native_frame( const Position & p ) const {

    return (native_to_tsz_frame( p )).inverse();
}
// this is really needed 
HepGeom::Transform3D Station::tsz_to_global_frame( const Position & p ) const {

    MsgStream log(m_msgSvc, "MGM tsz_to_global_frame");
	const bool pLvl=log.level()<=MSG::VERBOSE;
    
    HepGeom::Transform3D nominalTransf= HepGeom::Transform3D::Identity;
    
    std::string geov = MYSQL::GetPointer()->getGeometryVersion();
    CLHEP::Hep3Vector vec;
    double RAD;
    if (m_name[0]=='T')
      {
	//RAD=((TgcComponent *)GetComponent(0))->posy;
	RAD=p.radius;
      }
    else
        RAD=p.radius;
    vec.setX(RAD*cos(p.phi*CLHEP::deg));
    vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
    vec.setY(RAD*sin(p.phi*CLHEP::deg));
    vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
    // 
    if (p.isMirrored)
        if ( (p.isBarrelLike) || (m_name[0]=='B') )
        {
            // correct the z location (=-p.z-m_length) for possible m_amdbOrigine_along_length
            if (geov.substr(0,1) != "P")
            {
                vec.setZ(p.z + getAmdbOrigine_along_length());
            }
            else vec.setZ(p.z);
//             std::cout<<" tsz_to_global for station "<<m_name<<" at fi/zi "<<p.phiindex+1<<"/"<<p.zindex<<" isMirr. "<<p.isMirrored
//                      <<" transl. to "<<vec<<" p.z = "<<p.z<<" length "<<GetLength()<<" AmdbOrigine_along_length "<<getAmdbOrigine_along_length()<<std::endl;
        }
        else
        {
            vec.setZ(p.z + GetThickness()); // re-establish the amdb z location (with a - sign)
        }
    else
    {
        if ( (p.isBarrelLike) || (m_name[0]=='B' && p.zindex<0 && hasMdts()) )
        {
            //MYSQL* mysql=MYSQL::GetPointer();
            //MDT       *mdtobj= (MDT *)mysql->GetATechnology("MDT0");
            //double halfpitch = (mdtobj->pitch)/2.;
	    double halfpitch = mdtHalfPitch();
            //vec.setZ(p.z + (-getAmdbOrigine_along_length()+halfpitch));
            vec.setZ(p.z + halfpitch);
        }
        else
            vec.setZ(p.z);
    }
    
	if (pLvl)
    log<<MSG::VERBOSE<<" translation according to "<<vec.x()<<" "<<vec.y()<<" "<<vec.z()<<endmsg;

    /////// NEWEWEWWEWEWEWEWEWEWEWEWEW
    // // define the rotations by alpha, beta, gamma
    HepGeom::Rotate3D ralpha = HepGeom::RotateX3D(p.alpha*CLHEP::deg);
    HepGeom::Rotate3D rbeta  = HepGeom::RotateZ3D(p.beta*CLHEP::deg);
    HepGeom::Rotate3D rgamma;
    rgamma = HepGeom::RotateY3D(p.gamma*CLHEP::deg);
    if (pLvl)
      log<<MSG::VERBOSE<<" gamma is not changing sign - original "<<p.gamma<<" new one "<<p.gamma<<endreq;
    if (pLvl)
    log<<MSG::VERBOSE<<" alpha / beta "<<p.alpha<<" "<<p.beta<<endreq;

    // // apply all transform in sequence 
    // //    HepGeom::Transform3D to_tsz = rgamma*rbeta*ralpha*AMDBorgTranslation*mirrsym;  // works for barrel and barrel-like
    // // imt: tested for CTB2004, seems to work for all amdb versions...
    HepGeom::Transform3D abgRot = rgamma*rbeta*ralpha;
    /////// NEWEWEWWEWEWEWEWEWEWEWEWEW



    if ( m_name[0]=='B' || p.isBarrelLike )
    {    
        // here all Barrel chambers 
        nominalTransf =  HepGeom::RotateZ3D(p.phi*CLHEP::deg);
    }
    else
    {
// replace this with the folowing lines 8/06/2006 SS because, EC not mirrored chambers have anyway to be rotated
// by 180deg around z to mov ecoherently their local reference frame and the tube-layer numbering
//         if ( p.z>=0 || ( p.z<0 && !(p.isMirrored) ) ){
//             nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
// 					    HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg));
//         }
//         else if (p.z<0 && p.isMirrored){
//             nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
//                                             HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg)*
//                                             HepGeom::RotateZ3D(180*CLHEP::deg));
//         }
        if ( p.z>=0 ){
            nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
					    HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg));
        }
        else if ( p.z<0 ){
            nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
                                            HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg)*
                                            HepGeom::RotateZ3D(180*CLHEP::deg));
        }
	else log << MSG::WARNING<<" AAAAAA problem here p.z, mirrored "
		    <<p.z<<" "<<p.isMirrored<<endmsg;
    }
    return HepGeom::Translate3D(vec)*nominalTransf*abgRot;
}

HepGeom::Transform3D Station::global_to_tsz_frame( const Position & p ) const {
    
    return (tsz_to_global_frame( p )).inverse();
}
HepGeom::Transform3D Station::getNominalTransform(const Position & p) const 
{
    //    std::cout<<"Station::getNominalTransform for Position P defined below *********"<<std::endl;
    //    std::cout<<p<<std::endl;
    return  tsz_to_global_frame( p ) * native_to_tsz_frame( p );
}
HepGeom::Transform3D Station::getAlignedTransform(const AlignPos & ap, const Position & p) const 
{
    return  tsz_to_global_frame( p ) * getDeltaTransform_tszFrame( ap ) * native_to_tsz_frame( p );
}
HepGeom::Transform3D Station::getDeltaTransform_tszFrame(const AlignPos & ap) const
{
  MsgStream log(m_msgSvc, "MGM getDeltaTransform_tszFrame");
  std::string geov = MYSQL::GetPointer()->getGeometryVersion();
  if (ap.tras!=0 ||ap.trat!= 0 ||ap.traz!=0 ||
      ap.rots!=0 || ap.rott!=0||ap.rotz!=0)
    {
	if (log.level()<=MSG::VERBOSE) 
	{
	    log << MSG::VERBOSE << "Setting corrections." << endmsg;
	    log << MSG::VERBOSE << "For station " << m_name <<
		" corrections sent are " << ap.tras << " " <<
		ap.traz << " " << ap.trat << " " << ap.rots << " " << ap.rotz <<
		" " << ap.rott << " isBarrel=" << ap.isBarrel << endmsg;
	    log << MSG::VERBOSE << "length="<< GetLength()<<" m_thickness="
		<<GetThickness()<<endmsg;
	}
    }

  HepGeom::Rotate3D rott = HepGeom::RotateX3D(ap.rott);
  HepGeom::Rotate3D rotz = HepGeom::RotateZ3D(ap.rotz);
  HepGeom::Rotate3D rots = HepGeom::RotateY3D(ap.rots);
  HepGeom::Transform3D trans = HepGeom::TranslateY3D(ap.tras)*
                         HepGeom::TranslateZ3D(ap.traz)*HepGeom::TranslateX3D(ap.trat);


  HepGeom::Transform3D delta = trans*rots*rotz*rott;
  	
  if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE<<" delta transform in the tsz frame --------------"<<endmsg
      <<
      (delta)[0][0] << " " <<
      (delta)[0][1] << " " <<
      (delta)[0][2] << " " <<
      (delta)[0][3] << " " << endmsg <<
      (delta)[1][0] << " " <<
      (delta)[1][1] << " " <<
      (delta)[1][2] << " " <<
      (delta)[1][3] << " " << endmsg <<
      (delta)[2][0] << " " <<
      (delta)[2][1] << " " <<
      (delta)[2][2] << " " <<
      (delta)[2][3] << " " << endmsg;  

  // our delta transform must be applied in the tsz frame:
  return delta;
}
HepGeom::Transform3D Station::getDeltaTransform(const AlignPos & ap, const Position & p) const
{
    // GM applies Delta transform like transform*delta
    HepGeom::Transform3D deltaGM = tsz_to_native_frame(p) *
                             getDeltaTransform_tszFrame( ap ) * native_to_tsz_frame(p);
    return deltaGM;
}


double Station::getAmdbOrigine_along_length() const 
{
  if (m_length) 
    return m_amdbOrigine_along_length; 
  else { 
    m_length = GetLength(); 
    return m_amdbOrigine_along_length; 
  } 
}

double Station::getAmdbOrigine_along_thickness() const 
{
    if (m_thickness) return m_amdbOrigine_along_thickness;
    else 
    {
        m_thickness = GetThickness();
        return m_amdbOrigine_along_thickness;
    }    
//     if (m_amdbOrigine_along_thickness) return m_amdbOrigine_along_thickness;
//     else return 0.;
}


// /* 17/06/2008 This is not needed anywhere 
// HepGeom::Transform3D Station::getTransform( const AlignPos & ap) const {
//   MsgStream log(m_msgSvc, "MuonGeoModel");
//   HepGeom::Translate3D theTranslation, AMDBorgTranslation;
//   HepGeom::Transform3D theRotation;
//   std::string geov = MYSQL::GetPointer()->getGeometryVersion();
//   if (geov == "CTB2004")
//     {
//       log << MSG::INFO << "Calling obsolete getTransform(AlignPos) for "
// 		<< m_name << "...  Better switch to getDeltaTransform!"
// 		<< endmsg;
//     }
//   float tras=0, traz=0, trat=0, rots=0, rotz=0, rott=0;
//   bool isBarrel = false;
//   isBarrel = ap.isBarrel;
//   tras = ap.tras;
//   traz = ap.traz;
//   trat = ap.trat;
//   rots = ap.rots;
//   rotz = ap.rotz;
//   rott = ap.rott;
//   if (tras!=0 || trat!= 0 || traz!=0 || rots!=0 || rott!=0||rotz!=0)
//     {
//       log << MSG::DEBUG << "Setting corrections." << endmsg;
//       log << MSG::DEBUG << "For station " << m_name <<
// 	" corrections sent are " << tras << " " <<
// 	traz << " " << trat << " " << rots << " " << rotz <<
// 	" " << rott << " isBarrel=" << isBarrel << endmsg;
//       log << MSG::DEBUG << "length="<< GetLength()<<" m_thickness="
// 		<<GetThickness()<<endmsg;
//     }

//   if (ap.isBarrel && !(ap.isTrapezoid))
//     {   // rectangular barrel chambers
//       // AMDB org is at z=0, t=0 (not z=m_length/2, t=m_thickness/2)
//       AMDBorgTranslation = HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//       // RSZ = tsz
//       theTranslation = HepGeom::Translate3D(trat,tras,traz);
//       // SZR = szt
//       theRotation = HepGeom::RotateY3D(rots)*HepGeom::RotateZ3D(rotz)*
// 	HepGeom::RotateX3D(rott);
//     }
//   else if (ap.isBarrel && ap.isTrapezoid)
//     {   // we can get rid of isTrapezoid after AMDB version 7.
//       theTranslation = HepGeom::Translate3D(trat,-tras,traz);
//       theRotation = HepGeom::RotateY3D(-rots)*HepGeom::RotateZ3D(rotz)*
// 	HepGeom::RotateX3D(rott);      
//       AMDBorgTranslation = HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//     }
//   else
//     { // assumed to be trapezoidal endcap chambers
// //       
// //       // RSZ = zst 
// //       theTranslation = HepGeom::Translate3D(traz,tras,trat);
// //       // SZR = stz  so you would expect:
// //       theRotation = HepGeom::RotateY3D(rots)*HepGeom::RotateZ3D(rott)*
// //       HepGeom::RotateX3D(rotz);
// //       
//       // but in fact the s-axis is flipped in MuonGM wrt AMDB,
//       // and the z and t axes are exchanged:
//       theTranslation = HepGeom::Translate3D(trat,-tras,traz);
//       theRotation = HepGeom::RotateY3D(-rots)*HepGeom::RotateZ3D(rotz)*
// 	HepGeom::RotateX3D(rott);      
//       AMDBorgTranslation = HepGeom::Translate3D(GetLength()/2.,0.,GetThickness()/2.);
//     }
//   //  return theTranslation*theRotation;
//   // put it in AMDB coordinates for the transformation, then go back to MuonGM
//   return AMDBorgTranslation.inverse()*theTranslation*theRotation*
//     AMDBorgTranslation;
// }
// */
// not needed 17/06/2008 
// HepGeom::Transform3D Station::getAmdbOrgTrans(const Position & p) const
// { // in tsz coordinates:
//   //MsgStream log(m_msgSvc, "MGM getAmdbOrgTrans");
//   HepGeom::Translate3D AMDBorgTranslation;
//   if (m_name[0]=='B' || p.isBarrelLike )
//     {
//         AMDBorgTranslation =
//             HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//     }
//   else
//     {
//         AMDBorgTranslation =
//             HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//     }
//   //log << MSG::VERBOSE<< "length = "<<GetLength()<<" m_thickness="<<GetThickness()
//   //	    << endmsg;
//   return AMDBorgTranslation;
// }

// 17/06/2008 not needed anywhere
// HepGeom::Transform3D Station::getTransform( const Position & p) const {
//   MsgStream log(m_msgSvc, "MuonGeoModel");
//     std::string geov = MYSQL::GetPointer()->getGeometryVersion();
//     if (geov == "CTB2004") 
//     {
//       /* IMT - replace this whole section with call to getNominalTransform
//         HepGeom::Transform3D nominalTransf;
//         HepGeom::Translate3D AMDBorgTranslation;
//         static CLHEP::Hep3Vector vec;
//         static CLHEP::Hep3Vector vecRot;
//         if (m_name[0]=='B' || p.isBarrelLike) {
//             // The chamber orientation is given by the direction of the
//             //  R-axis (T-axis), NOT by a vector pointing to the centre
//             //  of the chamber or even to the AMDB origin!
//             vecRot.setX(cos(p.phi*CLHEP::deg));
//             vecRot.setY(sin(p.phi*CLHEP::deg));
//             vecRot.setZ(1.);
//             if (p.isBarrelLike)  // EC chamber treated as barrel, be careful!
//             {
//                 // defining the position of the centre of any station
//                 vec.setX((p.radius+GetLength()/2.)*cos(p.phi*CLHEP::deg));
//                 vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//                 vec.setY((p.radius+GetLength()/2.)*sin(p.phi*CLHEP::deg));
//                 vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//                 vec.setZ(p.z+GetThickness()/2.);
//                 AMDBorgTranslation =HepGeom::Translate3D(GetLength()/2.,0.,GetThickness()/2.);
//             }
//             else  // normal barrel case:
//             {
//                 // defining the position of the centre of any station
//                 // here using the stations in DBSC (described in amdb + the ones at z<0)
//                 vec.setX((p.radius+GetThickness()/2.)*cos(p.phi*CLHEP::deg));
//                 vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//                 vec.setY((p.radius+GetThickness()/2.)*sin(p.phi*CLHEP::deg));
//                 vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//                 vec.setZ(p.z+GetLength()/2.);
//                 AMDBorgTranslation =HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//             }
//         }
//         else {      // if (m_name[0]=='T') 
//             // defining the position of the centre of stations at z><0
//             double RAD;
//             // IMT this should be version dependent, bad for CTB geom...
//             //if (m_name[0]=='T') RAD=((TgcComponent *)GetComponent(0))->posy;
//             //else
//             RAD=p.radius;
//             //        vec.setX((RAD+GetLength()/2.)*cos(p.phi*CLHEP::deg));
//             vec.setX((RAD+GetThickness()/2.)*cos(p.phi*CLHEP::deg));
//             vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//             //        vec.setY((RAD+GetLength()/2.)*sin(p.phi*CLHEP::deg));
//             vec.setY((RAD+GetThickness()/2.)*sin(p.phi*CLHEP::deg));
//             vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//             if (p.z<0) {
//                 //            vec.setZ(p.z+GetThickness()/2.);
//                 vec.setZ(p.z+GetLength()/2.);
//             }
//             else { 
//                 //            vec.setZ(p.z+GetThickness()/2.);
//                 vec.setZ(p.z+GetLength()/2.);
//             }
//             // The chamber orientation is given by the direction of the
//             //  R-axis (T-axis), NOT by a vector pointing to the centre
//             //  of the chamber or even to the AMDB origin!
//             vecRot.setX(cos(p.phi*CLHEP::deg));
//             vecRot.setY(sin(p.phi*CLHEP::deg));
//             vecRot.setZ(1.);
//             AMDBorgTranslation =
//                 //	  HepGeom::Translate3D(GetLength()/2.,0.,GetThickness()/2.);
//                 HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//         }
//           //log << MSG::DEBUG << "AMDB org is (" << GetLength()/2. << ", " << 0
//           //<< ", " << GetThickness()/2.
//           //<< ") which is (m_length/2, 0, m_thickness/2)" << endmsg;
//         const HepVector3D zaxis   = HepVector3D(0.,0.,1.);
//         //const HepVector3D raxis   = HepVector3D(vec.x(), vec.y(), 0.);
//         // imt redefine these to use vectors given by phi-direction only...
//         const HepVector3D raxis   = HepVector3D(vecRot.x(), vecRot.y(), 0.);
//         const HepVector3D phiaxis = HepVector3D(-raxis.y(), raxis.x(), 0.); // phi = z cross r
//         // order of extra rotations is alpha(r), then beta(z), then gamma(phi)
//         // imt alpha is rotation about station T-axis and beta about Z-axis.
//         //     these are swapped around for the EC chambers.
//           //HepGeom::Rotate3D ralpha(p.alpha*CLHEP::deg, raxis);
//           //HepGeom::Rotate3D rbeta (p.beta*CLHEP::deg,  zaxis);
//           ////  the minus is here temporary
//           //HepGeom::Rotate3D rgamma(-p.gamma*CLHEP::deg, phiaxis);
//         HepGeom::Rotate3D ralpha, rbeta, rgamma;
//         if (m_name[0]=='B' || p.isBarrelLike)
//         {
//             ralpha=HepGeom::Rotate3D(p.alpha*CLHEP::deg, raxis);
//             rbeta=HepGeom::Rotate3D(p.beta*CLHEP::deg,  zaxis);
//             rgamma=HepGeom::Rotate3D(p.gamma*CLHEP::deg, phiaxis);
//         }
//         else
//         {
//              // ralpha=HepGeom::Rotate3D(p.beta*CLHEP::deg, raxis);
//              // rbeta=HepGeom::Rotate3D(p.alpha*CLHEP::deg,  zaxis);
//             ralpha=HepGeom::Rotate3D(p.alpha*CLHEP::deg,  zaxis);// which is really T-axis
//             rbeta=HepGeom::Rotate3D(p.beta*CLHEP::deg, raxis);   // which is really Z-axis
//             rgamma=HepGeom::Rotate3D(p.gamma*CLHEP::deg, phiaxis);
//         }
//         if (m_name[0]=='B' || p.isBarrelLike)
//         {    
//             // here all Barrel chambers 
//             if (p.isMirrored) nominalTransf = HepGeom::RotateZ3D(p.phi*CLHEP::deg)*HepGeom::RotateX3D(180.*CLHEP::deg);
//             else nominalTransf =  HepGeom::RotateZ3D(p.phi*CLHEP::deg);
//         }
//           //else if (p.isBarrelLike)
//           //{
//           //// here all encap chambers positioned like barrel chambers
//           //// initial 180 CLHEP::deg Y-flip is because of intrinsic GeoTrap orientation
//           //if (p.isMirrored) nominalTransf =
// 	  //HepGeom::RotateZ3D(p.phi*CLHEP::deg)*HepGeom::RotateX3D(180.*CLHEP::deg)*
// 	  //HepGeom::RotateY3D(180.*CLHEP::deg);
//           //else nominalTransf =  HepGeom::RotateZ3D(p.phi*CLHEP::deg)*HepGeom::RotateY3D(180.*CLHEP::deg);
//           //}
//         else
//         {	
//             if ( p.z>=0 || ( p.z<0 && !(p.isMirrored) ) ){
//                 nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg));
//             }
//             else if (p.z<0 && p.isMirrored){
//                 nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
//                                                 HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg)*
//                                                 HepGeom::RotateZ3D(180*CLHEP::deg));
//             }
//             else log << MSG::WARNING <<" AAAAAA problem here p.z, mirrored "
// 	    <<p.z<<" "<<p.isMirrored<<endmsg;
//         }
//         HepGeom::Transform3D transf = HepGeom::Translate3D(vec)*AMDBorgTranslation.inverse()*rgamma*rbeta*ralpha*AMDBorgTranslation*nominalTransf;
//         HepGeom::Transform3D dummyRot = rgamma*rbeta*ralpha;
//         return transf;
//       */
//       return getNominalTransform(p); // works for barrel & barrel-like
//     }
//     else
//     {
//         HepGeom::Transform3D nominalTransf;
//         HepGeom::Transform3D AMDBorgTranslation = HepGeom::Transform3D::Identity;    
//         static CLHEP::Hep3Vector vec;
//         if (m_name[0]=='B') {
//             // defining the position of the centre of any station
//             // here using the stations in DBSC (described in amdb + the ones at z<0)
//             vec.setX((p.radius+GetThickness()/2.)*cos(p.phi*CLHEP::deg));
//             vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//             vec.setY((p.radius+GetThickness()/2.)*sin(p.phi*CLHEP::deg));
//             vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//             vec.setZ(p.z+GetLength()/2.);
//             AMDBorgTranslation = HepGeom::Translate3D(GetThickness()*cos(p.phi*CLHEP::deg)/2.,
//                                                 GetThickness()*sin(p.phi*CLHEP::deg)/2.,
//                                                 GetLength()/2.);
//         }
//         else {      // if (m_name[0]=='T') 
//             // defining the position of the centre of stations at z><0
//             double RAD;
//             // IMT this should be version dependent, bad for CTB geom...
//             if (m_name[0]=='T')
//                 RAD=((TgcComponent *)GetComponent(0))->posy;
//             else
//                 RAD=p.radius;
//             if (m_name[0]!='C') 
//             {
//                 vec.setX((RAD+GetLength()/2.)*cos(p.phi*CLHEP::deg));
//                 vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//                 vec.setY((RAD+GetLength()/2.)*sin(p.phi*CLHEP::deg));
//                 vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//                 vec.setZ(p.z+GetThickness()/2.);
//             }
//             else 
//             {
//                 vec.setX(RAD*cos(p.phi*CLHEP::deg));
//                 vec.setX(vec.x()-p.shift*sin((p.phi)*CLHEP::deg));
//                 vec.setY(RAD*sin(p.phi*CLHEP::deg));
//                 vec.setY(vec.y()+p.shift*cos((p.phi)*CLHEP::deg));
//                 if (p.z>0) vec.setZ(p.z);
//                 else vec.setZ(p.z+GetThickness());
//             }
//         }
// //         HepGeom::Transform3D AMDBorgTranslation = HepGeom::Transform3D::Identity;
// //         AMDBorgTranslation = HepGeom::Translate3D(GetThickness()/2.,
// //                                             0.,
// //                                             GetLength()/2.);        
//         const HepVector3D zaxis   = HepVector3D(0.,0.,1.);
//         const HepVector3D raxis   = HepVector3D(vec.x(), vec.y(), 0.);
//         const HepVector3D phiaxis = HepVector3D(-raxis.y(), raxis.x(), 0.); // phi = z cross r
//         // order of extra rotations is alpha(r), then beta(z), then gamma(phi)
//         HepGeom::Rotate3D ralpha, rbeta, rgamma;
//         ralpha = HepGeom::Rotate3D(p.alpha*CLHEP::deg, raxis);
//         rbeta  = HepGeom::Rotate3D(p.beta*CLHEP::deg,  zaxis);
//         if ( p.zindex<0 && !(m_name[0] == 'B') ) {
//             rgamma = HepGeom::Rotate3D(p.gamma*CLHEP::deg, phiaxis);
//             //            if (m_name[0]=='C') log << MSG::DEBUG <<"zi,fi  gamma applied "<<m_name<<" "<<p.zindex<<" "<<p.phiindex<<" "<<p.gamma<<endmsg;
//         }
//         else {
//             rgamma = HepGeom::Rotate3D(-p.gamma*CLHEP::deg, phiaxis);        
//             //            if (m_name[0]=='C') log << MSG::DEBUG<<"zi,fi  gamma applied "<<m_name<<" "<<p.zindex<<" "<<p.phiindex<<" "<<-p.gamma<<endmsg;
//         }        
//         if (m_name[0]=='B' || p.isBarrelLike)
//         {    
//             // here all Barrel chambers 
//             if (p.isMirrored) nominalTransf = HepGeom::RotateZ3D(p.phi*CLHEP::deg)*HepGeom::RotateX3D(180.*CLHEP::deg);
//             else nominalTransf =  HepGeom::RotateZ3D(p.phi*CLHEP::deg);
//         }
//         else
//         {	
//             if ( p.z>=0 || ( p.z<0 && !(p.isMirrored) ) ){
//                 nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg));
//             }
//             else if (p.z<0 && p.isMirrored){
//                 nominalTransf =  HepGeom::Transform3D(HepGeom::RotateY3D(-90*CLHEP::deg)*
//                                                 HepGeom::RotateX3D(p.phi*CLHEP::deg-180*CLHEP::deg)*
//                                                 HepGeom::RotateZ3D(180*CLHEP::deg));
//             }
//             else log << MSG::WARNING<<" AAAAAA problem here p.z, mirrored "
// 		     <<p.z<<" "<<p.isMirrored<<endmsg;
//         }
//         HepGeom::Transform3D transf;
//         if (m_name[0]!='C') transf = HepGeom::Translate3D(vec)*rgamma*rbeta*ralpha*nominalTransf;
//         else transf = HepGeom::Translate3D(vec)*nominalTransf*
//                       HepGeom::RotateY3D(p.gamma*CLHEP::deg)*
//                       HepGeom::Translate3D(GetThickness()/2.,0.,GetLength()/2.);
//         return transf;
//     }
// }

} // namespace MuonGM
