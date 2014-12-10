/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The sTgc detector = an assembly module = STGC in amdb 
 ----------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
***************************************************************************/


//<doc><file>	$Id: sTgcReadoutElement.cxx,v 1.5 2009-05-13 15:03:47 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/NSWenumeration.h"
#include "MuonReadoutGeometry/NSWgeometry.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#define sTgcReadout_verbose false

namespace MuonGM {

  sTgcReadoutElement::sTgcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
					 int zi, int fi, int mL, bool is_mirrored,
					 MuonDetectorManager* mgr)
    : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr)
  {
    _ml = mL;
    m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:sTgcReadoutElement");
    //MsgStream log(Athena::getMessageSvc(), "MuGM:sTgcReadoutElement");

    // get the setting of the caching flag from the manager
    setCachingFlag(mgr->cachingFlag());

    std::string vName = pv->getLogVol()->getName();
    std::string sName = vName.substr(vName.find("-")+1);
    std::string fixName = (sName[2]=='L') ? "STL" : "STS";
	
	// std::cout<<"sTgcReadoutElement: name,zi,fi,mL "<<stName<<" "<<zi<<" "<<fi<<" "<<mL<<std::endl;

    //setStationName(stName);
    setStationName(fixName);
    setStationEta(zi);
    setStationPhi(fi);
    setChamberLayer(mL);
    // in principle here one can set the identifier, with stName, stEta, stPhi, mL
    // build here the identifier and set it 
    Identifier id = mgr->stgcIdHelper()->channelID(fixName,zi,fi,mL,1,2,1);
    setIdentifier(id);

    m_nlayers=0;

    m_halfX=std::vector<double>(4);
    m_minHalfY=std::vector<double>(4);
    m_maxHalfY=std::vector<double>(4);
    
    if (mgr->MinimalGeoFlag() == 0) {
      GeoPhysVol* pvc = NULL;
      pvc = (GeoPhysVol*)pv;
      if (pvc != NULL) {
	unsigned int nchildvol = pvc->getNChildVols();
	int llay = 0;
	std::string::size_type npos;
	for (unsigned ich=0; ich<nchildvol; ++ich) {
	  PVConstLink pc = pvc->getChildVol(ich);
	  std::string childname = (pc->getLogVol())->getName();
	  if ((npos = childname.find("Sensitive")) != std::string::npos ) {
	    llay ++;
            if (llay > 4) {
	      std::cerr<<"number of sTGC layers > 4: increase transform array size"<< std::endl;
              continue;
	    }
	    //std::cerr<<" navigating STGC named "
	    //       <<childname<<" child "
	    //	     <<ich<<" is a layer with tag "<<llay<<"," << pvc->getXToChildVol(ich).getTranslation()<< std::endl;
	    _Xlg[llay-1] = Amg::CLHEPTransformToEigen(pvc->getXToChildVol(ich));
            // save layer dimensions
	    if (llay==1 || abs(zi)<3 ) {
			  // std::cout<<" \tType "<<pc->getLogVol()->getShape()->type()<<std::endl;
			  if (pc->getLogVol()->getShape()->type()=="Shift") {
			  	const GeoShapeShift* myshift = dynamic_cast<const GeoShapeShift*> (pc->getLogVol()->getShape());
				if(!myshift) {
				  std::cerr<<"sTgcReadoutElement : even though the shape is of type shift it's not a shift - better crashing ..."<< std::endl;
				  throw;
				}
				//std::cout<<" \t\ttype "<<myshift->getOp()->type()<<std::endl;
				//std::cout<<" \t\t translation "<<myshift->getX().getTranslation()<<" "<<std::endl;
				const GeoSimplePolygonBrep* poly=dynamic_cast<const GeoSimplePolygonBrep*>(myshift->getOp());
				if(!poly) {
				  std::cerr<<"sTgcReadoutElement : the sTGC is no polygon even though it should - better crashing ..."<< std::endl;
				  throw;
				}
				//std::cout<<" \t\t\t\t GeoPolygonBrep: dz: "<<poly->getDZ()<<std::endl;
				//for (int i=0;i<poly->getNVertices();i++)
				//{
				//	std::cout<<" \t\t\t\t Point "<<poly->getXVertex(i)<<" "<<poly->getYVertex(i)<<std::endl;
				//}
				if (poly->getNVertices()==4)
				{
					m_halfX[llay-1] = poly->getYVertex(0);
                    m_minHalfY[llay-1] = poly->getXVertex(3);
                    m_maxHalfY[llay-1] = poly->getXVertex(0);
				}
				else if (poly->getNVertices()==6)
				{
					m_halfX[llay-1] = poly->getYVertex(0);
                    m_minHalfY[llay-1] = poly->getXVertex(4);
					double d1=poly->getXVertex(5)-poly->getXVertex(4);
					double d2=poly->getYVertex(5)-poly->getYVertex(4);
					double x=2*poly->getYVertex(0)*d1/d2;
                    m_maxHalfY[llay-1] = m_minHalfY[llay-1]+x;
				}
				
			  }
              else if (pc->getLogVol()->getShape()->type()=="Trd") {
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (pc->getLogVol()->getShape());
		//std::cerr<<"sTGC layer shape dimensions:minX,maxX,minY,maxY,halfZ:"<< trd->getXHalfLength1()<<"," << trd->getXHalfLength2()
		//	 <<"," << trd->getYHalfLength1() <<"," << trd->getYHalfLength2() <<"," << trd->getZHalfLength() << std::endl;
                m_halfX[llay-1] = trd->getZHalfLength();
                m_minHalfY[llay-1] = trd->getYHalfLength1();
                m_maxHalfY[llay-1] = trd->getYHalfLength2();
 
 	      }	else if (pc->getLogVol()->getShape()->type()=="Subtraction") {
		const GeoShapeSubtraction* sh = dynamic_cast<const GeoShapeSubtraction*> (pc->getLogVol()->getShape());
                const GeoShape* subA = sh->getOpA();  
                const GeoShape* subB = sh->getOpB();  

                while (subA->type()=="Subtraction") {
                   sh = dynamic_cast<const GeoShapeSubtraction*> (subA);
                   subA = sh->getOpA();
                   subB = sh->getOpB();                       
		}
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (subA);
		const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (subB);
                
                if (!trd) {
		  std::cerr<<"sTGC layer base shape not trapezoid? " << subA->type()<< std::endl;
		} else {
		  //std::cerr<<"sTGC layer shape dimensions:minX,maxX,minY,maxY,halfZ:"<< trd->getXHalfLength1()<<"," << trd->getXHalfLength2()
		  //	 <<"," << trd->getYHalfLength1() <<"," << trd->getYHalfLength2() <<"," << trd->getZHalfLength() << std::endl;
		  m_halfX[llay-1] =  trd->getZHalfLength();
		  m_minHalfY[llay-1] = trd->getYHalfLength1();
		  m_maxHalfY[llay-1] = trd->getYHalfLength2();
                }
                
                if (!shift) {
		  std::cerr<<"sTGC layer subtraction not shift? " << subB->type()<< std::endl;
		} else {
                  const GeoBox* box=dynamic_cast<const GeoBox*> (shift->getOp());
		  //std::cerr<<"sTGC layer subtraction:stEta,layer:halfX,halfY,halfZ:"<<zi<<","<<llay<<":"<<shift->getX().getTranslation()<<std::endl;
		  //if (box) std::cerr<< box->getXHalfLength()<<"," << box->getYHalfLength() <<"," << box->getZHalfLength() << std::endl;
                  if (box) {
                    double dx = shift->getX().getTranslation().z();
		    double dd = fabs(dx)- box->getZHalfLength();
                    double diff = dd - m_halfX[llay-1];
                    if (diff<0.) {
                      double fr = 0.5*diff/m_halfX[llay-1];
                      m_halfX[llay-1] += 0.5*diff;  // diff negative-> reduce surface size
		      if (dx<0.) {
                        m_minHalfY[llay-1] -= fr*(m_maxHalfY[llay-1] - m_minHalfY[llay-1]); 
		      } else {
                        m_maxHalfY[llay-1] += fr*(m_maxHalfY[llay-1] - m_minHalfY[llay-1]); 
		      }
                      // move layer
                      double dz = dx<0.? -0.5*diff : 0.5*diff;
		      _Xlg[llay-1]= Amg::Translation3D(0.,0.,dz)*_Xlg[llay-1];                            	    
		    }
		  }
                }


              } else {
	        std::cerr<<"sTGC layer shape not recognized:" <<pc->getLogVol()->getShape()->type()<< std::endl;
	      }
	    }
	  }
	}
        m_nlayers=llay;
      } else {
	std::cerr<<"Cannot read the GeoModel tree ! "<<std::endl;
      }
    }

    //fillCache();
    
  }


  sTgcReadoutElement::~sTgcReadoutElement()
  {
    clearCache();
  }


  void  sTgcReadoutElement::setIdentifier(Identifier id)
  {
    m_id = id;
    const sTgcIdHelper* idh = manager()->stgcIdHelper();
    IdentifierHash collIdhash = 0;
    IdentifierHash detIdhash = 0;
    // set parent data collection hash id
    int gethash_code = idh->get_module_hash(id, collIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
     	     <<"sTgcReadoutElement --  collection hash Id NOT computed for id = "
     	     <<idh->show_to_string(id)<<std::endl;
    m_idhash = collIdhash;
    // // set RE hash id 
    gethash_code = idh->get_detectorElement_hash(id, detIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
     	     <<"sTgcReadoutElement --  detectorElement hash Id NOT computed for id = "
     	     <<idh->show_to_string(id)<<endreq;
    m_detectorElIdhash = detIdhash;
  }



  void sTgcReadoutElement::initDesign(double /*largeX*/, double /*smallX*/, double /*lengthY*/, double stripPitch, double /*wirePitch*/,
                                      double stripWidth, double /*wireWidth*/, double thickness) {

    int sector=getStationName().substr(2,1)=="L" ? 1 : 0;
    int stEta=abs(getStationEta());

    m_phiDesign = std::vector<MuonChannelDesign>(m_nlayers); 
    m_etaDesign = std::vector<MuonChannelDesign>(m_nlayers); 
    m_padDesign = std::vector<MuonPadDesign>(m_nlayers); 

    for (int il=0; il<m_nlayers; il++) {

      // identifier of the first channel - strip plane - to retrieve max number of strips
      Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),_ml, il+1, 1, 1);
      int chMax =  manager()->stgcIdHelper()->channelMax(id);
      if (chMax<0) chMax = 350;

      //std::cerr<<"maximal number of strips:eta:layer:"<< stEta<<","<<chMax << std::endl;

      m_etaDesign[il].type=0;
      
      m_etaDesign[il].xSize    = stEta>2 ? 2*m_halfX[0] : 2*m_halfX[il];
      m_etaDesign[il].minYSize = stEta>2 ? 2*m_minHalfY[0] : 2*m_minHalfY[il];
      m_etaDesign[il].maxYSize = stEta>2 ? 2*m_maxHalfY[0] : 2*m_maxHalfY[il];

      m_etaDesign[il].deadO = 0.;
      m_etaDesign[il].deadI = 0.;
      m_etaDesign[il].deadS = 0.;

      m_etaDesign[il].inputPitch = stripPitch;
      m_etaDesign[il].inputLength = m_etaDesign[il].minYSize;
      m_etaDesign[il].inputWidth = stripWidth;
      m_etaDesign[il].thickness = thickness;
      	
      m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize + 0.5*stripPitch;
      m_etaDesign[il].sAngle = 0.;
      m_etaDesign[il].signY  = 1 ;
      
      m_etaDesign[il].nch = (int) (m_etaDesign[il].xSize/stripPitch) + 1;

      if (m_etaDesign[il].nch > chMax) {    // fix with help of dead zone

	std::cerr<<"number of strips exceeds the maximum, adjusted:"<<m_etaDesign[il].nch<<"->"<<chMax << std::endl;

	double dead = 0.5*(m_etaDesign[il].xSize - chMax*stripPitch);
	m_etaDesign[il].deadO = dead;
	m_etaDesign[il].deadI = dead;
	m_etaDesign[il].firstPos += dead;
	m_etaDesign[il].nch = chMax;
      
      }
           
      m_nStrips.push_back(m_etaDesign[il].nch);
      
      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch << ", nstrips " << m_etaDesign[il].nch << endreq;
      
    }

    for (int il=0; il<m_nlayers; il++) {

      // identifier of the first channel - wire plane - to retrieve max number of wire gangs
      Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),_ml, il+1, 2, 1);
      int chMax =  manager()->stgcIdHelper()->channelMax(id);

      if (chMax<0) chMax = (getStationName()).substr(2,1)=="L" ? 32 : 22;

      m_phiDesign[il].type=1;

      m_phiDesign[il].xSize    = stEta>2 ? 2*m_halfX[0] : 2*m_halfX[il];
      m_phiDesign[il].minYSize = stEta>2 ? 2*m_minHalfY[0] : 2*m_minHalfY[il];
      m_phiDesign[il].maxYSize = stEta>2 ? 2*m_maxHalfY[0] : 2*m_maxHalfY[il];

      m_phiDesign[il].deadO = 0.;
      m_phiDesign[il].deadI = 0.;
      m_phiDesign[il].deadS = 0.;

      m_phiDesign[il].nch = chMax;
      
      double gangPitch = m_phiDesign[il].maxYSize/chMax;

      m_phiDesign[il].inputPitch = gangPitch;
      m_phiDesign[il].inputLength = m_phiDesign[il].xSize;
      m_phiDesign[il].inputWidth = gangPitch;
      m_phiDesign[il].thickness = thickness;
      	
      m_phiDesign[il].firstPos = -0.5*m_phiDesign[il].maxYSize + 0.5*gangPitch;
      m_phiDesign[il].sAngle = 0.;            // handled by surface rotation
      m_phiDesign[il].signY  = 1 ;
      	     
      m_nWires.push_back(chMax);

      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", wireGang pitch " << m_phiDesign[il].inputPitch << ", nWireGangs "<< m_phiDesign[il].nch << endreq;

    }

    for (int il=0; il<m_nlayers; il++) {

      m_padDesign[il].xSize    = stEta>2 ? 2*m_halfX[0] : 2*m_halfX[il];
      m_padDesign[il].minYSize = stEta>2 ? 2*m_minHalfY[0] : 2*m_minHalfY[il];
      m_padDesign[il].maxYSize = stEta>2 ? 2*m_maxHalfY[0] : 2*m_maxHalfY[il];

      m_padDesign[il].deadO = 0.;
      m_padDesign[il].deadI = 0.;
      m_padDesign[il].deadS = 0.;

      m_padDesign[il].padPhiMin = INDEX_LEFTMOST_COL[2*sector+(_ml-1)][stEta-1][il];
      m_padDesign[il].padPhiMax = INDEX_RIGHTMOST_COL[2*sector+(_ml-1)][stEta-1][il];
      m_padDesign[il].firstPhiPos = PAD_COL_PHI0[2*sector+(_ml-1)][stEta-1][il];
      m_padDesign[il].inputPhiPitch = stEta<2 ? PAD_PHI_DIVISION/PAD_PHI_SUBDIVISION : PAD_PHI_DIVISION ;
      m_padDesign[il].inputPhiWidth = m_padDesign[il].inputPhiPitch;
      
      m_padDesign[il].padEtaMin = FIRST_PAD_ROW_DIVISION[2*sector+(_ml-1)][stEta-1][il];
      m_padDesign[il].padEtaMax = m_padDesign[il].padEtaMin + PAD_ROWS[2*sector+(_ml-1)][stEta-1][il];
      m_padDesign[il].firstRowPos = H_PAD_ROW_0[2*sector+(_ml-1)][il];
      m_padDesign[il].inputRowPitch = PAD_HEIGHT[2*sector+(_ml-1)][il];
      m_padDesign[il].inputRowWidth = m_padDesign[il].inputRowPitch;

      m_padDesign[il].thickness = thickness;

      m_padDesign[il].sAngle = 0.;            // handled by surface rotation
      m_padDesign[il].signY  = 1 ;

      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", phi pad size " << m_padDesign[il].inputPhiPitch << ", eta pad size "<< m_padDesign[il].inputRowPitch << endreq;

    }

  }

  void sTgcReadoutElement::fillCache() const
  {
    if( !m_surfaceData ) m_surfaceData = new SurfaceData();
    else{
      reLog()<<MSG::WARNING<<"calling fillCache on an already filled cache" << endreq;
      return;
    }

    if (abs(getStationEta())>2) {
      m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX[0], m_minHalfY[0], m_maxHalfY[0]));  // strips
      m_surfaceData->m_surfBounds.push_back( new Trk::TrapezoidBounds( m_minHalfY[0], m_maxHalfY[0], m_halfX[0]));         // wires           
    }
    
    for( int layer = 0; layer < m_nlayers; ++layer ){

      if (abs(getStationEta())<3) {
	m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX[layer], m_minHalfY[layer], m_maxHalfY[layer]));  // strips
	m_surfaceData->m_surfBounds.push_back( new Trk::TrapezoidBounds( m_minHalfY[layer], m_maxHalfY[layer], m_halfX[layer]));         // wires           
      }

      // identifier of the first channel - wire plane - locX along phi, locY max->min R
      Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),_ml, layer+1, 2, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*_Xlg[layer]*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,1.,0.))*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,0.,1.)));

      // is this cache really needed ? 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

      //std::cerr<<"center of wire plane, layer:"<<layer<<","<< m_surfaceData->m_layerCenters.back().perp()<< std::endl;

      // strip plane moved along normal, pad plane in the opposite direction
      double shift = 0.5*m_etaDesign[layer].thickness;

      // identifier of the first channel - strip plane
      id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),_ml, layer+1, 1, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*_Xlg[layer]*
						 Amg::Translation3D(shift,0.,0.)*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) );
      // is this cache really needed ? 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

      // identifier of the first channel - pad plane
      id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),_ml, layer+1, 0, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*_Xlg[layer]*
						 Amg::Translation3D(-shift,0.,0.)*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,1.,0.))*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,0.,1.)) );

      // is this cache really needed ? 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

      // update the padDesign info
      const MuonGM::MuonPadDesign* padDesign=this->getPadDesign(id);
      if (padDesign) padDesign->setR(m_surfaceData->m_layerCenters.back().perp());

    }
  }

  bool sTgcReadoutElement::containsId(Identifier id) const
  {
 
    if (manager()->stgcIdHelper()->stationEta(id)!= getStationEta() ) return false;
    if (manager()->stgcIdHelper()->stationPhi(id)!= getStationPhi() ) return false;

    if (manager()->stgcIdHelper()->multilayerID(id)!= _ml ) return false;
    
    int gasgap     = manager()->stgcIdHelper()->gasGap(id);
    if (gasgap  <1 || gasgap > m_nlayers) return false;

    int strip      = manager()->stgcIdHelper()->channel(id);
    if (manager()->stgcIdHelper()->channelType(id)==1 && (strip <1 || strip > m_nStrips[gasgap-1]) ) return false;
    if (manager()->stgcIdHelper()->channelType(id)==2 && (strip <1 || strip > m_nWires[gasgap-1]) )  return false;
    if (manager()->stgcIdHelper()->channelType(id)==0 && (strip <1 || strip > m_nPads[gasgap-1]) )   return false;

    return true;
  }


  Amg::Vector3D sTgcReadoutElement::localToGlobalCoords(Amg::Vector3D locPos, Identifier id) const
  {
    int gg = manager()->stgcIdHelper()->gasGap(id);
    
    Amg::Vector3D  locP = _Xlg[gg-1]*locPos;
    std::cout<<"locPos in the gg      r.f. "<<locPos<<std::endl;
    std::cout<<"locP in the multilayer r.f. "<<locP<<std::endl;
    
    return absTransform()*locP;
  }


} // namespace MuonGM
