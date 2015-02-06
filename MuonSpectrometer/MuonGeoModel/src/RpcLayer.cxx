/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Rpc.h"
#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/RpcLayer.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/MYSQL.h"
#include "GeoModelKernel/GeoCutVolAction.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "MuonReadoutGeometry/GlobalUtilities.h"
#include <iomanip>
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"

namespace MuonGM {


RpcLayer::RpcLayer(std::string s, Rpc* t): DetectorElement(s),
  lwidth(0.), llength(0.), thickness(0.), llongWidth(0.)
{
  m = t;
}


GeoVPhysVol *RpcLayer::build()
{
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(cutoutson, vcutdef);
}


GeoVPhysVol* 
RpcLayer::build(int cutoutson, std::vector<Cutout*> vcutdef)
{
    std::cout << std::setiosflags(std::ios::fixed);
    std::cout << std::setprecision(4);

    double eps = 0.000001;
    double tol = 1.e-6;
    
    MYSQL* mysql = MYSQL::GetPointer();	
    RPC* r = (RPC*)mysql->GetTechnology(name);

    double thickness = r->rpcLayerThickness - tol;
    double length = m->length;
    double width  = m->width;

    if (RPCprint) std::cout << " RpcLayer: building an RPC layer with width = " << width
                            << " length = " << length << " and thickness = " << thickness 
                            << std::endl;
        
    const GeoShape* srpcl = new GeoTrd(thickness/2, thickness/2, width/2, 
                                       width/2, length/2);
    const GeoMaterial* mrpcl = matManager->getMaterial("std::Air");
    GeoLogVol* lrpcl = new GeoLogVol("Rpclayer", srpcl, mrpcl);
    GeoPhysVol* prpcl = new GeoPhysVol(lrpcl);

    double newpos = -thickness/2.;

    // phi strip panel(s)
    double strpanThickness = r->stripPanelThickness - tol;
    double strpanLength = length/r->NstripPanels_in_z;
    double strpanWidth = width /r->NstripPanels_in_s - eps;
    if (RPCprint) std::cout << " RpcLayer:: Building a strip panel:: div_s, div_z = "
                            << r->NstripPanels_in_s << " "
                            << r->NstripPanels_in_z << std::endl;
    if (RPCprint) std::cout << " RpcLayer:: Building a strip panel:: w,l,t = "
                            << strpanWidth << " " << strpanLength << " "
                            << strpanThickness << std::endl;
    GeoTrd* sstrpan = new GeoTrd (strpanThickness/2., strpanThickness/2.,
                                  strpanWidth/2., strpanWidth/2., strpanLength/2.);

    GeoTrd* sfoamstrpan = new GeoTrd (strpanThickness/2.-r->stripPanelCopperSkinThickness,
                                      strpanThickness/2.-r->stripPanelCopperSkinThickness,
                                      strpanWidth/2.-r->stripPanelCopperSkinThickness,
                                      strpanWidth/2.-r->stripPanelCopperSkinThickness,
                                      strpanLength/2.-r->stripPanelCopperSkinThickness);
    const GeoShape* scustrpan = sstrpan;
    GeoLogVol* lcustrpan = new GeoLogVol("RPC_StripPanelCuSkin", scustrpan,
                                         matManager->getMaterial("std::Copper"));        
    GeoLogVol* lfoamstrpan = new GeoLogVol("RPC_StripPanelFoam", sfoamstrpan,
                                           matManager->getMaterial("muo::RpcFoam"));

    newpos += strpanThickness/2. + tol/2.;
    GeoPhysVol* pcustrpan11 = new GeoPhysVol(lcustrpan);
    GeoPhysVol* pfoamstrpan11 = new GeoPhysVol(lfoamstrpan);
    GeoTransform* tx = new GeoTransform(HepGeom::TranslateX3D(newpos) );
    GeoTransform* ty1;
    GeoTransform* ty2;

    if (r->NstripPanels_in_s == 2) {
      if (RPCprint) std::cout << " RpcLayer::NstripPanels_in_s == 2 " << std::endl;
      GeoPhysVol* pcustrpan12 = new GeoPhysVol(lcustrpan);
      GeoPhysVol* pfoamstrpan12 = new GeoPhysVol(lfoamstrpan);
            ty1 = new GeoTransform(HepGeom::TranslateY3D(-width/4.));
            ty2 = new GeoTransform(HepGeom::TranslateY3D( width/4.));
            prpcl->add(tx);
            prpcl->add(ty1);
            if (RPCprint) std::cout<<"RpcLayer:: Locating the 1st phi strip panel at x, y "
                     <<newpos<<" "
                     <<-width/4.<<" of width ="
                     <<strpanWidth<<" in a box of width ="<<width<<std::endl;
            prpcl->add(pcustrpan11);
            pcustrpan11->add(pfoamstrpan11);
            prpcl->add(tx);
            prpcl->add(ty2);
            if (RPCprint) std::cout<<"RpcLayer:: Locating the 2nd phi strip panel at x, y "
                     <<newpos<<" "
                     <<width/4.<<" of width ="
                     <<strpanWidth<<" in a box of width ="<<width<<std::endl;            
            prpcl->add(pcustrpan12);
            pcustrpan12->add(pfoamstrpan12);
            
    } else if (r->NstripPanels_in_s == 1) {
      if (RPCprint) std::cout << " RpcLayer::NstripPanels_in_s == 1 " << std::endl;
      prpcl->add(tx);
      prpcl->add(pcustrpan11);
      pcustrpan11->add(pfoamstrpan11);
      if (RPCprint) std::cout << " RpcLayer:: Locating a single phi strip panel at x, y "
                              << newpos << " 0 " << " of width = "
                              << strpanWidth << " in a box of width = " << width << std::endl;            
    }
        
    newpos += strpanThickness/2.;
    double ggThickness = 2.*r->bakeliteThickness + r->gasThickness + r->totalAirThickness; 
    newpos += ggThickness/2.;
    // this brings to the center of the gas gap

        double ggLength    = length;
        double ggWidth     = width/r->NGasGaps_in_s-eps;
        double gasLength   = ggLength - 2.*r->bakeliteframesize;
        double gasWidth    = ggWidth- 2.*r->bakeliteframesize;
        if (RPCprint) {
            std::cout<<"RpcLayer:: Building the gasgap:: "<<r->NGasGaps_in_s<<" in s direction"<<std::endl;
            std::cout<<"RpcLayer:: Building the gasgap:: w,l,t "<<ggWidth<<" "<<ggLength<<" "<<ggThickness<<std::endl;
            std::cout<<"RpcLayer:: Building the gas:: w,l,t "<<gasWidth<<" "
                     <<gasLength<<" "<<r->gasThickness<<std::endl;
        }
        GeoTrd *sgg  = new GeoTrd (ggThickness/2., ggThickness/2.,
                                   ggWidth/2.,     ggWidth/2.,
                                   ggLength/2.);
        GeoTrd *sgas = new GeoTrd (r->gasThickness/2., r->gasThickness/2., 
                                   gasWidth/2., gasWidth/2.,
                                   gasLength/2.);
        const GeoShape *sbak = sgg;
        GeoLogVol *lbak = new GeoLogVol("gas volume:"
                                        +MuonGM::buildString(r->NGasGaps_in_s,0)+"gg_in_s_"
                                        +MuonGM::buildString(r->NstripPanels_in_s,0)+"sp_in_s", sbak,
                                         matManager->getMaterial("std::Bakelite"));
        GeoLogVol *lgas = new GeoLogVol("gazGap", sgas,
                                        matManager->getMaterial("muo::RPCgas"));
        GeoPhysVol *pbak1 =  new GeoPhysVol (lbak);
        GeoPhysVol *pgas1 =  new GeoPhysVol (lgas);
        tx = new GeoTransform(HepGeom::TranslateX3D(newpos));
        if (r->NGasGaps_in_s == 2)
        {
            GeoPhysVol *pbak2 =  new GeoPhysVol (lbak);
            GeoPhysVol *pgas2 =  new GeoPhysVol (lgas);
            ty1 = new GeoTransform(HepGeom::TranslateY3D(-width/4.));
            ty2 = new GeoTransform(HepGeom::TranslateY3D(width/4.));
            prpcl->add(tx);
            prpcl->add(ty1);
            if (RPCprint) std::cout<<"RpcLayer:: put 1st gas gap centre at depth, s "<<newpos<<" "<<-width/4.<<std::endl;
	    prpcl->add(new GeoIdentifierTag(0));
            prpcl->add(pbak1);
	    pbak1->add(new GeoIdentifierTag(1));
            pbak1->add(pgas1);
            prpcl->add(tx);
            prpcl->add(ty2);
            if (RPCprint) std::cout<<"RpcLayer:: put 2nd gas gap centre at depth, s "<<newpos<<" "<<width/4.<<std::endl;
            prpcl->add(new GeoIdentifierTag(10));
            prpcl->add(pbak2);
            pbak2->add(new GeoIdentifierTag(1));
            pbak2->add(pgas2);
        }
        else if (r->NGasGaps_in_s == 1)
        {
            prpcl->add(tx);
            prpcl->add(new GeoIdentifierTag(0));
            prpcl->add(pbak1);
            if (RPCprint) std::cout<<"RpcLayer:: put a single gas gap at depth, s "<<newpos<<" 0 "<<std::endl;
            pbak1->add(new GeoIdentifierTag(1));
            pbak1->add(pgas1);
        }
        
	newpos += ggThickness/2.;

        //now eta strip panel
        newpos += strpanThickness/2.;
                
        GeoPhysVol *pcustrpan21   = new GeoPhysVol(lcustrpan);
        GeoPhysVol *pfoamstrpan21 = new GeoPhysVol(lfoamstrpan);
        tx = new GeoTransform(HepGeom::TranslateX3D(newpos));
        if (r->NstripPanels_in_s == 2) 
        {
            GeoPhysVol *pcustrpan22 = new GeoPhysVol(lcustrpan);
            GeoPhysVol *pfoamstrpan22 = new GeoPhysVol(lfoamstrpan);
            ty1 = new GeoTransform(HepGeom::TranslateY3D(-width/4.));
            ty2 = new GeoTransform(HepGeom::TranslateY3D( width/4.));
            prpcl->add(tx);
            prpcl->add(ty1);
            if (RPCprint) std::cout<<"RpcLayer:: Locating the 1st eta panel at x, y "
                     <<newpos<<" "
                     <<-width/4.<<" of width ="
                     <<strpanWidth<<" in a box of width ="<<width<<std::endl;
            prpcl->add(pcustrpan21);
            pcustrpan21->add(pfoamstrpan21);
            prpcl->add(tx);
            prpcl->add(ty2);
            if (RPCprint) std::cout<<"RpcLayer:: Locating the 2nd eta panel at x, y "
                     <<newpos<<" "
                     <<width/4.<<" of width ="
                     <<strpanWidth<<" in a box of width ="<<width<<std::endl;
            prpcl->add(pcustrpan22);
            pcustrpan22->add(pfoamstrpan22);
        }
        else if (r->NstripPanels_in_s == 1)
        {
            prpcl->add(tx);
            if (RPCprint) std::cout<<"RpcLayer:: Locating a single eta panel at x, y "
                     <<newpos<<" 0 "
                     <<std::endl;
            prpcl->add(pcustrpan21);
            pcustrpan21->add(pfoamstrpan21);
        }

    // Apply cutouts
    if (cutoutson && vcutdef.size() > 0) {
      GeoPhysVol* tempPhys = 0;
      Cutout* cut = 0;
      GeoShape* cutoutShape = 0;
      HepGeom::Transform3D cutTrans;
      for (unsigned i = 0; i < vcutdef.size(); i++) {
        cut = vcutdef[i];
        cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                 cut->widthXs/2.+0.5, cut->widthXl/2.+0.5, cut->lengthY/2.+tol);
        cutTrans = HepGeom::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);

        GeoCutVolAction cutAction(*cutoutShape, cutTrans);
        prpcl->apply(&cutAction);
        tempPhys = cutAction.getPV();
        prpcl->ref(); prpcl->unref();
        prpcl = tempPhys;
      }
    }

    return prpcl;
}


void RpcLayer::print()
{
  std::cout << " Rpc Layer " << name+"Layer" << " :" << std::endl;

}
} // namespace MuonGM
