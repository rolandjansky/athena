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
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "AGDDModel/AGDDParameterStore.h"

#define sTgcReadout_verbose false

namespace MuonGM {

  sTgcReadoutElement::sTgcReadoutElement(GeoVFullPhysVol* pv, std::string stName,
					 int zi, int fi, int mL, bool is_mirrored,
					 MuonDetectorManager* mgr)
    : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr)
  {
    m_ml = mL;
    m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:sTgcReadoutElement");

    // get the setting of the caching flag from the manager
    setCachingFlag(mgr->cachingFlag());

    std::string vName = pv->getLogVol()->getName();
    std::string sName = vName.substr(vName.find("-")+1);
    std::string fixName = (sName[1]=='L') ? "STL" : "STS";
	
    //setStationName(stName);
    setStationName(fixName);
    setStationEta(zi);
    setStationPhi(fi);
    setChamberLayer(mL);
    // in principle here one can set the identifier, with stName, stEta, stPhi, mL
    // build here the identifier and set it 
    Identifier id = mgr->stgcIdHelper()->channelID(fixName,zi,fi,mL,1,2,1);
    setIdentifier(id);

    //m_nlayers=0;

    sTGCDetectorHelper sTGC_helper;

    std::string sTGCname = std::string("sTG1-")+sName;
    
    reLog() << MSG::INFO << "sTGCname: " << sTGCname << endmsg;
    sTGCDetectorDescription *sTGC = sTGC_helper.Get_sTGCDetectorType(sTGCname);
    if(sTGC)
      reLog() << MSG::INFO << "Found sTGC detector: " << sTGCname << " " << sTGC << endmsg;

    static const int nLayers = 4;

    m_halfX=std::vector<double>(nLayers);
    m_minHalfY=std::vector<double>(nLayers);
    m_maxHalfY=std::vector<double>(nLayers);

    for(int layer = 0; layer < nLayers; layer++){
      double length = sTGC->Length(); //Distance between parallel sides of the trapezoid
      double sWidth = sTGC->sWidth(); //Width on short side of trapezoid 
      double lWidth = sTGC->lWidth(); //Widht on long side

      double ysFrame = sTGC->ysFrame(); //Frame thickness on short parallel edge
      double ylFrame = sTGC->ylFrame(); //Frame thickness on long parallel edge
      double xFrame = sTGC->xFrame(); //Frame thickness of non parallel edge

      reLog() << MSG::INFO << "length: " << length << " ysFrame: " << ysFrame << " ylFrame: " << ylFrame << endmsg; 

      m_halfX[layer] = (length - ysFrame - ylFrame)/2.0;
      m_minHalfY[layer] = (sWidth - 2.0*xFrame)/2.0;
      m_maxHalfY[layer] = (lWidth - 2.0*xFrame)/2.0;
    }

    
    if (mgr->MinimalGeoFlag() == 0) {
      if (GeoFullPhysVol* pvc = dynamic_cast<GeoFullPhysVol*> (pv)) {
	unsigned int nchildvol = pvc->getNChildVols();
	int llay = 0;
	std::string::size_type npos;
	for (unsigned ich=0; ich<nchildvol; ++ich) {
	  PVConstLink pc = pvc->getChildVol(ich);
	  std::string childname = (pc->getLogVol())->getName();
	  reLog() << MSG::INFO << "Volume Type: " << pc->getLogVol()->getShape()->type() << endmsg;
	  if ((npos = childname.find("Sensitive")) != std::string::npos ) {
	    llay ++;
            if (llay > 4) {
	      reLog() << MSG::WARNING << "number of sTGC layers > 4: increase transform array size"<< endmsg;
              continue;
	    }
	    m_Xlg[llay-1] = Amg::CLHEPTransformToEigen(pvc->getXToChildVol(ich));
	    /*if (llay==1 || abs(zi)<3 ) {
	      if (pc->getLogVol()->getShape()->type()=="Shift") {
		const GeoShapeShift* myshift = dynamic_cast<const GeoShapeShift*> (pc->getLogVol()->getShape());
		if(!myshift) {
		  reLog() << MSG::ERROR << "sTgcReadoutElement : even though the shape is of type shift it's not a shift - better crashing ..." << endmsg;
		  throw;
		}
		const GeoSimplePolygonBrep* poly=dynamic_cast<const GeoSimplePolygonBrep*>(myshift->getOp());
		if(!poly) {
		  reLog() << MSG::ERROR << "sTgcReadoutElement : the sTGC is no polygon even though it should - better crashing ..." << endmsg;
		  throw;
		}
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
		    double d1 = poly->getXVertex(5)-poly->getXVertex(4);
		    double d2 = poly->getYVertex(5)-poly->getYVertex(4);
		    double x = 2*poly->getYVertex(0)*d1/d2;
                    m_maxHalfY[llay-1] = m_minHalfY[llay-1]+x;
		  }
		
	      }
	      else {
	        reLog() << MSG::WARNING << "sTGC layer shape not recognized:" << pc->getLogVol()->getShape()->type() << endmsg;
	      }
	      }*/
	  }
	}
        m_nlayers=llay;
      } else {
	reLog() << MSG::WARNING << "Cannot read the GeoModel tree" << endmsg;
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
     	     <<idh->show_to_string(id)<<endmsg;
    m_idhash = collIdhash;
    // // set RE hash id 
    gethash_code = idh->get_detectorElement_hash(id, detIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
     	     <<"sTgcReadoutElement --  detectorElement hash Id NOT computed for id = "
     	     <<idh->show_to_string(id)<<endmsg;
    m_detectorElIdhash = detIdhash;
  }



  void sTgcReadoutElement::initDesign(double /*largeX*/, double /*smallX*/, double /*lengthY*/, double /*stripPitch*/, double /*wirePitch*/,
                                      double /*stripWidth*/, double /*wireWidth*/, double thickness) {


    // int sector=getStationName().substr(2,1)=="L" ? 1 : 0;
    char sector_l=getStationName().substr(2,1)=="L" ? 'L' : 'S';

    int stEta=abs(getStationEta());
    int Etasign=getStationEta()/stEta;
    std::string side;
    if (Etasign > 0) side="A";
    else side="C"; //This needs to be checked 

    sTGCDetectorHelper aHelper;
    reLog() << MSG::INFO<<getStationName()<<endmsg;

    sTGCDetectorDescription* stgc = aHelper.Get_sTGCDetector(sector_l,stEta,getStationPhi(),m_ml,side.back());
    if (stgc) 
      reLog() << MSG::INFO
	      << "Found sTGC Detector " << stgc->GetName() << endmsg;
    else {
      reLog() << MSG::INFO << "No sTGC Detector" << endmsg;
      reLog() << MSG::INFO << sector_l <<"  " << getStationEta() << " " << getStationPhi() << "  " <<m_ml << " "<<sector_l <<endmsg;
    }

    auto tech=stgc->GetTechnology();


    m_phiDesign = std::vector<MuonChannelDesign>(m_nlayers); 
    m_etaDesign = std::vector<MuonChannelDesign>(m_nlayers); 
    m_padDesign = std::vector<MuonPadDesign>(m_nlayers); 

    //Get Chamber length, width and frame widths
    double length = stgc->Length(); //Distance between parallel sides of the trapezoid
    double sWidth = stgc->sWidth(); //Width on short side of trapezoid 
    double lWidth = stgc->lWidth(); //Width on long side
    
    double ysFrame = stgc->ysFrame(); //Frame thickness on short parallel edge
    double ylFrame = stgc->ylFrame(); //Frame thickness on long parallel edge
    double xFrame  = stgc->xFrame(); //Frame thickness of non parallel edges
    double yCutout = stgc->yCutout();// y of cutout of trapezoid (only in outermost detectors)

    sTGCReadoutParameters roParam = stgc->GetReadoutParameters();

    // AGDDParameterBagsTGCTech* parameterBagTech = dynamic_cast<AGDDParameterBagsTGCTech*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(stgc->GetName()));

    double stripStagger[4] = {1.6, 3.2, 1.6, 3.2}; // First Strip in sTGC may be staggered. This value corresponds to first strip width

    for (int il=0; il<m_nlayers; il++) {

      // identifier of the first channel - strip plane - to retrieve max number of strips
      /*Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, il+1, 1, 1);
      int chMax =  manager()->stgcIdHelper()->channelMax(id);
      if (chMax<0) chMax = 350;*/

      m_etaDesign[il].type=0;

      m_etaDesign[il].xSize    = length - ysFrame - ylFrame;
      m_etaDesign[il].xLength  = length;
      m_etaDesign[il].ysFrame  = ysFrame;
      m_etaDesign[il].ylFrame  = ylFrame;
      m_etaDesign[il].minYSize = sWidth - 2.0*xFrame;
      m_etaDesign[il].maxYSize = lWidth - 2.0*xFrame;
      
      m_etaDesign[il].deadO = 0.;
      m_etaDesign[il].deadI = 0.;
      m_etaDesign[il].deadS = 0.;

      m_etaDesign[il].inputPitch = 3.2; // parameterBagTech->stripPitch;
      m_etaDesign[il].inputLength = m_etaDesign[il].minYSize;
      m_etaDesign[il].inputWidth = 2.7; // parameterBagTech->stripWidth;
      if (!tech){
	reLog()<<MSG::ERROR <<"Failed To get Technology for stgc element :" << stgc->GetName() << endmsg;      
	m_etaDesign[il].thickness = 0;
      }
      else{
	m_etaDesign[il].thickness = stgc->GetTechnology()->gasThickness;//+stgc->GetTechnology()->pcbThickness;
      }
	
      // m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize + 0.5*(parameterBagTech->stripPitch);
      m_etaDesign[il].firstPos = -0.5*length + ysFrame + stripStagger[il]; // First Strip position is start of active region + first strip width
      reLog() << MSG::INFO
	      << "firstPos: " << m_etaDesign[il].firstPos << endmsg;
      m_etaDesign[il].sAngle = 0.;
      m_etaDesign[il].signY  = 1 ;
      
      m_etaDesign[il].nch = roParam.nStrips;
           
      m_nStrips.push_back(m_etaDesign[il].nch);
      
      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch << ", nstrips " << m_etaDesign[il].nch << endmsg;
      
    }

    for (int il=0; il<m_nlayers; il++) {

      m_phiDesign[il].type=1;

      m_phiDesign[il].xSize    = length - ysFrame - ylFrame;
      m_phiDesign[il].minYSize = sWidth - 2.0*xFrame;
      m_phiDesign[il].maxYSize = lWidth - 2.0*xFrame;

      m_phiDesign[il].deadO = 0.;
      m_phiDesign[il].deadI = 0.;
      m_phiDesign[il].deadS = 0.;

      m_phiDesign[il].inputPitch = 1.8; // parameterBagTech->wirePitch;
      m_phiDesign[il].inputLength = m_phiDesign[il].xSize;
      m_phiDesign[il].inputWidth = 0.015; // parameterBagTech->wireWidth;
      m_phiDesign[il].thickness = stgc->Tck();
      	
      // m_phiDesign[il].firstPos = -0.5*m_phiDesign[il].maxYSize + 0.5*(parameterBagTech->wirePitch);
      m_phiDesign[il].firstPos = -0.5*m_phiDesign[il].maxYSize + 0.5*(1.8);
      m_phiDesign[il].sAngle = 0.;            // handled by surface rotation
      m_phiDesign[il].signY  = 1 ;
      	     
      m_phiDesign[il].nch = roParam.nWires[il];

      m_nWires.push_back(m_phiDesign[il].nch);

      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", wireGang pitch " << m_phiDesign[il].inputPitch << ", nWireGangs "<< m_phiDesign[il].nch << endmsg;

    }

    for (int il=0; il<m_nlayers; il++) {

	
      m_padDesign[il].Length = length ;
      m_padDesign[il].sWidth = sWidth ;
      m_padDesign[il].lWidth = lWidth ;
      m_padDesign[il].xFrame = xFrame ;
      m_padDesign[il].ysFrame = ysFrame ;
      m_padDesign[il].ylFrame = ylFrame ;
      m_padDesign[il].yCutout = yCutout ;
      m_padDesign[il].etasign = Etasign;

      m_padDesign[il].sPadWidth = roParam.sPadWidth;
      m_padDesign[il].lPadWidth = roParam.lPadWidth;
 
      
      m_padDesign[il].deadO = 0.;
      m_padDesign[il].deadI = 0.;
      m_padDesign[il].deadS = 0.;	

      m_padDesign[il].nPadColumns = roParam.nPadPhi[il];
      if (side == "A") m_padDesign[il].firstPhiPos = roParam.firstPadPhiDivision_A[il];
      else if (side == "C") m_padDesign[il].firstPhiPos = roParam.firstPadPhiDivision_C[il];//PAD_COL_PHI0[2*sector+(m_ml-1)][stEta-1][il];

      m_padDesign[il].inputPhiPitch = roParam.anglePadPhi;//stEta<2 ?  PAD_PHI_DIVISION/PAD_PHI_SUBDIVISION : PAD_PHI_DIVISION ;

      if (side == "A")m_padDesign[il].PadPhiShift = roParam.PadPhiShift_A[il];
      else if (side == "C")m_padDesign[il].PadPhiShift = roParam.PadPhiShift_C[il];

      m_padDesign[il].padEtaMin =  roParam.firstPadRow[il];//FIRST_PAD_ROW_DIVISION[2*sector+(m_ml-1)][stEta-1][il];
      m_padDesign[il].nPadH = roParam.nPadH[il];     
      m_padDesign[il].padEtaMax = m_padDesign[il].padEtaMin +  roParam.nPadH[il];//PAD_ROWS[2*sector+(m_ml-1)][stEta-1][il];

      m_padDesign[il].firstRowPos   = roParam.firstPadH[il];// H_PAD_ROW_0[2*sector+(m_ml-1)][il];
      m_padDesign[il].inputRowPitch = roParam.padH[il];// PAD_HEIGHT[2*sector+(m_ml-1)][il];

      if (sector_l == 'L'){
    	  m_padDesign[il].isLargeSector = 1;
      	  m_padDesign[il].sectorOpeningAngle = m_padDesign[il].largeSectorOpeningAngle;
      }
      else{
    	  m_padDesign[il].isLargeSector = 0;
      	  m_padDesign[il].sectorOpeningAngle = m_padDesign[il].smallSectorOpeningAngle;
      }


/*   Sum Height Check
 *
	double ActiveAreaHeight = m_padDesign[il].Length-m_padDesign[il].ysFrame-m_padDesign[il].ylFrame;
	double sumheightpads =  m_padDesign[il].firstRowPos + (m_padDesign[il].inputRowPitch*(m_padDesign[il].nPadH-1));
	double diff = ActiveAreaHeight-sumheightpads;
	int good=0;
	if (m_padDesign[il].inputRowPitch-abs(diff)>0){
		 good =1;
	}

reLog() << MSG::INFO<<"initDesign  Sum Height Check: "<<stgc->GetName()<<" stgc->Length(): "<<stgc->Length()<<"ActiveArea Height "<<ActiveAreaHeight<<" sumheightpads: "<<sumheightpads<<" diff: "<< diff<<" padH: "<< m_padDesign[il].inputRowPitch <<" good : "<<good<<endmsg;
*/
      m_padDesign[il].thickness = thickness;

      m_padDesign[il].sAngle = 0.;            // handled by surface rotation
      m_padDesign[il].signY  = 1 ;


	
	reLog() << MSG::INFO<<"initDesign stationname "<<getStationName()<<" layer " << il << ",pad phi angular width " << m_padDesign[il].inputPhiPitch << ", eta pad size "<< m_padDesign[il].inputRowPitch <<"  Length: "<< m_padDesign[il].Length<<" sWidth: "<< m_padDesign[il].sWidth<<" lWidth: "<<m_padDesign[il].lWidth<<" firstPhiPos:"<<m_padDesign[il].firstPhiPos<<" padEtaMin:"<<m_padDesign[il].padEtaMin<<" padEtaMax:"<<m_padDesign[il].padEtaMax<<" firstRowPos:"<<m_padDesign[il].firstRowPos<<" inputRowPitch:"<<m_padDesign[il].inputRowPitch<<" thickness:"<<m_padDesign[il].thickness<<" sPadWidth: " <<m_padDesign[il].sPadWidth<<" lPadWidth: "<< m_padDesign[il].lPadWidth<<" xFrame: "<< m_padDesign[il].xFrame <<" ysFrame: "<< m_padDesign[il].ysFrame<<" ylFrame: "<< m_padDesign[il].ylFrame << " yCutout: "<< m_padDesign[il].yCutout<<endmsg;



    }

  }

  void sTgcReadoutElement::fillCache() const
  {
    if( !m_surfaceData ) m_surfaceData = new SurfaceData();
    else{
      reLog()<<MSG::WARNING<<"calling fillCache on an already filled cache" << endmsg;
      return;
    }
    /*
    if (abs(getStationEta())>2) {
      m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX[0], m_minHalfY[0], m_maxHalfY[0]));  // strips
      m_surfaceData->m_surfBounds.push_back( new Trk::TrapezoidBounds( m_minHalfY[0], m_maxHalfY[0], m_halfX[0]));         // wires           
    }
    */
    for( int layer = 0; layer < m_nlayers; ++layer ){

      //if (abs(getStationEta())<3) {
      m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX[layer], m_minHalfY[layer], m_maxHalfY[layer]));  // strips
      m_surfaceData->m_surfBounds.push_back( new Trk::TrapezoidBounds( m_minHalfY[layer], m_maxHalfY[layer], m_halfX[layer]));         // wires           
	// }

      // identifier of the first channel - wire plane - locX along phi, locY max->min R
      Identifier id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, layer+1, 2, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      //m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(new Amg::Transform3D(), new Trk::RotatedTrapezoidBounds( m_halfX[layer], m_minHalfY[layer], m_maxHalfY[layer])));
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,1.,0.))*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,0.,1.)));

      // is this cache really needed ? 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

      //std::cerr<<"center of wire plane, layer:"<<layer<<","<< m_surfaceData->m_layerCenters.back().perp()<< std::endl;

      // strip plane moved along normal, pad plane in the opposite direction
      double shift = 0.5*m_etaDesign[layer].thickness;

      // identifier of the first channel - strip plane
      id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, layer+1, 1, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*
						 Amg::Translation3D(shift,0.,0.)*
						 Amg::AngleAxis3D(-90*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) );

      // is this cache really needed ? 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

      // identifier of the first channel - pad plane
      id = manager()->stgcIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, layer+1, 0, 1);

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*
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

    if (manager()->stgcIdHelper()->multilayerID(id)!= m_ml ) return false;
    
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
    
    Amg::Vector3D  locP = m_Xlg[gg-1]*locPos;
    reLog() << MSG::INFO << "locPos in the gg      r.f. "<<locPos<<endmsg;
    reLog() << MSG::INFO << "locP in the multilayer r.f. "<<locP<<endmsg;
    
    return absTransform()*locP;
  }



} // namespace MuonGM
