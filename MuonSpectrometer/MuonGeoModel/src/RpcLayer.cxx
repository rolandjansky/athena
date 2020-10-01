/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <TString.h> // for Form
#include "GeoModelKernel/GeoShapeSubtraction.h" // for cutouts

namespace {
  static constexpr double const& rpc3GapLayerThickness = 11.8; // gas vol. + ( bakelite + graphite + PET )x2
  static constexpr double const& rpc3GapStrPanThickness = 3.5;
  static constexpr double const& rpc3GapStrPanCopThickness = 0.3;
  static constexpr double const& rpc3GapBakelThickness = 1.2;
  static constexpr double const& rpc3GapGThickness = 1.0;
  static constexpr double const& rpc3GapTotAirThickness = 0.52; // corresponds to PET+glue, which are not simulated?
}

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

    if (m->nGasGaps()==3 && r->NstripPanels_in_s!=1) throw std::runtime_error(Form("File: %s, Line: %d\nRpcLayer::build() - NstripPanels_in_s = %d for BIS RPC, not possible", __FILE__, __LINE__, r->NstripPanels_in_s));

    double thickness = r->rpcLayerThickness - tol;
    if (m->nGasGaps()==3) thickness = rpc3GapLayerThickness - tol; // for BIS RPCs
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
    double strpanCopperThickness = r->stripPanelCopperSkinThickness;
    if (m->nGasGaps()==3) { // for BIS RPCs
      strpanThickness = rpc3GapStrPanThickness - tol;
      strpanCopperThickness = rpc3GapStrPanCopThickness;
    }
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

    GeoTrd* sfoamstrpan = new GeoTrd (strpanThickness/2.-strpanCopperThickness,
                                      strpanThickness/2.-strpanCopperThickness,
                                      strpanWidth/2.-strpanCopperThickness,
                                      strpanWidth/2.-strpanCopperThickness,
                                      strpanLength/2.-strpanCopperThickness);
    const GeoShape* scustrpan = sstrpan;
    GeoLogVol* lcustrpan = new GeoLogVol("RPC_StripPanelCuSkin", scustrpan,
                                         matManager->getMaterial("std::Copper"));        
    auto stripMaterial = matManager->getMaterial("muo::RpcFoam");
    if (m->nGasGaps()==3) { // for BIS RPCs
      stripMaterial = matManager->getMaterial("muo::Forex");
    }
    GeoLogVol* lfoamstrpan = new GeoLogVol("RPC_StripPanelFoam", sfoamstrpan,  // "foam" in variable name is legacy, it is Forex for BIS
                                           stripMaterial);

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
    double bakelThickness = r->bakeliteThickness;
    double gThickness = r->gasThickness;
    double totAirThickness = r->totalAirThickness;
    if (m->nGasGaps()==3) { // for BIS RPCs
      bakelThickness = rpc3GapBakelThickness;
      gThickness = rpc3GapGThickness;
      totAirThickness = rpc3GapTotAirThickness;
    }

    double ggThickness = 2.*bakelThickness + gThickness + totAirThickness;
    newpos += ggThickness/2.;
    // this brings to the center of the gas gap

        double ggLength    = length;
        double ggWidth     = width/r->NGasGaps_in_s-eps;
        double gasLength   = ggLength - 2.*r->bakeliteframesize;
        double gasWidth    = ggWidth- 2.*r->bakeliteframesize;

        double y_translation;
        double z_translation;
        if (m->nGasGaps()==3) { // for BIS RPCs
          if (name == "RPC26" ) { //big RPC7
            gasLength   = ggLength - 93.25; // ggLength - deadframesizeEta
            gasWidth    = ggWidth - 109.52; // ggWidth - deadframesizePhi
            y_translation = -9.1;
            z_translation = 3.22;
          } else if (name == "RPC27" ){//small RPC7
            gasLength   = ggLength - 93.12; // ggLength - deadframesizeEta
            gasWidth    = ggWidth - 109.52; // ggWidth - deadframesizePhi
            y_translation = -9.1;
            z_translation = 3.06;
          } else if (name == "RPC28"){//big RPC8
            gasLength   = ggLength - 93.04; // ggLength - deadframesizeEta
            gasWidth    = ggWidth - 109.52; // ggWidth - deadframesizePhi
            y_translation = -27.7;
            z_translation = 3.11;
          } else if (name == "RPC29"){//small RPC8
            gasLength   = ggLength - 93.04; // ggLength - deadframesizeEta
            gasWidth    = ggWidth - 109.2; // ggWidth - deadframesizePhi
            y_translation = -8.8;
            z_translation = 3.11;
          }
        }

        if (RPCprint) {
            std::cout<<"RpcLayer:: Building the gasgap:: "<<r->NGasGaps_in_s<<" in s direction"<<std::endl;
            std::cout<<"RpcLayer:: Building the gasgap:: w,l,t "<<ggWidth<<" "<<ggLength<<" "<<ggThickness<<std::endl;
            std::cout<<"RpcLayer:: Building the gas:: w,l,t "<<gasWidth<<" "
                     <<gasLength<<" "<<gThickness<<std::endl;
        }
        GeoTrd *sgg  = new GeoTrd (ggThickness/2., ggThickness/2.,
                                   ggWidth/2.,     ggWidth/2.,
                                   ggLength/2.);
        GeoTrd *sgas = new GeoTrd (gThickness/2., gThickness/2., 
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
            if (m->nGasGaps()==3) { // for BIS RPCs
              GeoTransform* ty = new GeoTransform(HepGeom::TranslateY3D(y_translation));
              GeoTransform* tz = new GeoTransform(HepGeom::TranslateZ3D(z_translation));
              prpcl->add(ty);
              prpcl->add(tz);
            }
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
