/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/RPC_Technology.h"
#include "MuonGeoModel/Rpc.h"
#include "MuonGeoModel/RpcLayer.h"
#include "MuonGeoModel/MYSQL.h"
#include "GeoModelKernel/GeoCutVolAction.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iomanip>
#include <cassert>

#define skip_rpc false

namespace MuonGM {


Rpc::Rpc(Component* ss): DetectorElement(ss->name)
{
   double tol = 1.e-3;
   RpcComponent* s = (RpcComponent*)ss;
   width = s->dx1;
   longWidth = s->dx2;
   thickness = s->GetThickness();
   length = s->dy - tol;
   m_component = s;
   idiv = s->ndivy;
   jdiv = s->ndivz;
}


GeoFullPhysVol* Rpc::build(int minimalgeo)
{
   std::vector<Cutout*> vcutdef;
   int cutoutson = 0;
   return build(minimalgeo, cutoutson, vcutdef);
}


GeoFullPhysVol* Rpc::build(int minimalgeo, int cutoutson,
                           std::vector<Cutout*> vcutdef)
{
   MYSQL* mysql = MYSQL::GetPointer();
   std::string geometry_version = mysql->getGeometryVersion();
   RPC* r = (RPC*)mysql->GetTechnology(name);

//   std::cout << std::setiosflags(std::ios::fixed);
//   std::cout << std::setprecision(4);
   double thickness = r->maxThickness;
   width = width/idiv;
   longWidth = longWidth/idiv;
   length = length/jdiv;

   if (RPCprint) std::cout << " RPC build: " << name << " has thickness = "
                           << thickness << " long width = " << longWidth
                           << " width = " << width << " length = " << length 
                           << std::endl;  
 
   const GeoShape* srpc = new GeoTrd(thickness/2, thickness/2,
                                     width/2, longWidth/2, length/2);
   // Apply cutouts to mother volume

   if (cutoutson && vcutdef.size() > 0) {
     Cutout* cut = 0;
     GeoShape* cutoutShape = 0;
     GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
     for (unsigned i = 0; i < vcutdef.size(); i++) {
       cut = vcutdef[i];
       cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                cut->widthXs/2., cut->widthXl/2., cut->lengthY/2.);
       cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);
       srpc = &(srpc->subtract( (*cutoutShape) << cutTrans) ); 
     }
//     std::cout << " Cutouts for Rpc " << std::endl;
//     for (unsigned i = 0; i < vcutdef.size(); i++) std::cout
//         << " x = " << vcutdef[i]->dx << " y = " << vcutdef[i]->dy
//         << " width = " << vcutdef[i]->widthXs << " length = " << vcutdef[i]->lengthY
//         << std::endl;
   }

   const GeoMaterial* mrpc = matManager->getMaterial("std::Air");
   GeoLogVol* lrpc = new GeoLogVol(logVolName, srpc, mrpc);
   GeoFullPhysVol* prpc = new GeoFullPhysVol(lrpc);

   if (minimalgeo == 1) return prpc;
	
   if (geometry_version.substr(0,1) != "M") {
     // here layout P and following (hopefully!)
     if (idiv * jdiv != 1) assert(0);

     // Note: the standard RPC (iswap != -1) has DED at the top:
     // from bottom to top it consists of the external support panel  
     // and then the RPC doublet

     double newpos = -thickness/2.;

     // here the bottom/external/pre-bent support panel
     // shape of the al skin of the support panel
     double extSupThick = r->externalSupPanelThickness; 
     GeoTrd* slpan = new GeoTrd(extSupThick/2, extSupThick/2,
                                width/2, longWidth/2, length/2);
     double extAlSupThick = r->externalAlSupPanelThickness;
     GeoTrd* sholpan = new GeoTrd(extSupThick/2 - extAlSupThick,
                                  extSupThick/2 - extAlSupThick,
                                  width/2 - extAlSupThick,
                                  longWidth/2 - extAlSupThick,
                                  length/2 - extAlSupThick);
     const GeoShape* sallpan = slpan;
     const GeoShape* sholpan2 = sholpan;
     const GeoMaterial* mallpan = matManager->getMaterial("std::Aluminium");
     GeoLogVol* lallpan = new GeoLogVol("RPC_AL_extsuppanel", sallpan, mallpan);
     GeoPhysVol* pallpan = new GeoPhysVol(lallpan);
     const GeoMaterial* mholpan = matManager->getMaterial("muo::RpcAlHonC");
     GeoLogVol* lholpan = new GeoLogVol("RPC_honeyc_extsuppanel", sholpan2, mholpan);
     GeoPhysVol* pholpan = new GeoPhysVol (lholpan);
     pallpan->add(pholpan);   // this way the honeycomb is a child of its al skin

     // Apply cutouts
     if (cutoutson && vcutdef.size() > 0) {
       GeoPhysVol* tempPhys = 0;
       Cutout* cut = 0;
       GeoShape* cutoutShape = 0;
       GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
       for (unsigned i = 0; i < vcutdef.size(); i++) {
         cut = vcutdef[i];
         cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                  cut->widthXs/2., cut->widthXl/2., cut->lengthY/2.);
         cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);

         GeoCutVolAction cutAction(*cutoutShape, cutTrans);
         pallpan->apply(&cutAction);
         tempPhys = cutAction.getPV();
         pallpan->ref(); pallpan->unref();
         pallpan = tempPhys;
       }
     }

     newpos += r->externalSupPanelThickness/2.;
     GeoTransform* tlpan = new GeoTransform(GeoTrf::TranslateX3D( newpos ));
     if (RPCprint) std::cout << " Rpc:: put ext.sup panel at " << newpos
                             << " from centre" << std::endl;
     if (!skip_rpc) {
       prpc->add(tlpan);
       prpc->add(pallpan);
     }

     // bottom RpcLayer
     newpos += r->externalSupPanelThickness/2.;
     RpcLayer* rl = new RpcLayer(name,this);
     GeoVPhysVol* plowergg;
     if (cutoutson && vcutdef.size() > 0) {
       plowergg = rl->build(cutoutson, vcutdef);
     } else {
       plowergg = rl->build();
     }

     newpos += r->rpcLayerThickness/2.;
     GeoTransform* tlgg = new GeoTransform(GeoTrf::TranslateX3D( newpos ));
     if (RPCprint) std::cout << " Rpc:: put lower RPC layer at " << newpos
                             << " from centre " << std::endl;
     if (!skip_rpc) {
       prpc->add(new GeoIdentifierTag(1));
       prpc->add(tlgg);
       prpc->add(plowergg);
     }

     // central support panel 
     newpos += r->rpcLayerThickness/2.;
     double centSupThick = r->centralSupPanelThickness;
     GeoTrd* scpan = new GeoTrd(centSupThick/2, centSupThick/2,
                                width/2, longWidth/2, length/2);
     double centAlSupThick = r->centralAlSupPanelThickness;
     GeoTrd* shocpan = new GeoTrd(centSupThick/2 - centAlSupThick,
                                  centSupThick/2 - centAlSupThick,
                                  width/2 - centAlSupThick,
                                  longWidth/2 - centAlSupThick,
                                  length/2 - centAlSupThick);

     const GeoShape* salcpan = scpan;
     const GeoShape* shocpan2 = shocpan;
     GeoLogVol* lalcpan = new GeoLogVol("RPC_AL_midsuppanel", salcpan, mallpan);
     GeoPhysVol* palcpan = new GeoPhysVol(lalcpan);
     const GeoMaterial* mhocpan = matManager->getMaterial("muo::RpcPapHonC");
     GeoLogVol* lhocpan = new GeoLogVol("RPC_honeyc_midsuppanel", shocpan2, mhocpan);
     GeoPhysVol* phocpan = new GeoPhysVol (lhocpan);
     palcpan->add(phocpan);  // this way the honeycomb is a child of its al skin

     // Apply cutouts
     if (cutoutson && vcutdef.size() > 0) {
       GeoPhysVol* tempPhys = 0;
       Cutout* cut = 0;
       GeoShape* cutoutShape = 0;
       GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
       for (unsigned i = 0; i < vcutdef.size(); i++) {
         cut = vcutdef[i];
         cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                  cut->widthXs/2., cut->widthXl/2., cut->lengthY/2.);
         cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);

         GeoCutVolAction cutAction(*cutoutShape, cutTrans);
         palcpan->apply(&cutAction);
         tempPhys = cutAction.getPV();
         palcpan->ref(); palcpan->unref();
         palcpan = tempPhys;
       }
     }

     newpos += r->centralSupPanelThickness/2.;
     GeoTransform* tcpan = new GeoTransform(GeoTrf::TranslateX3D( newpos ));
     if (RPCprint) std::cout << " Rpc:: put central sup panel at " << newpos 
                             << " from centre" << std::endl;
     if (!skip_rpc) {
       prpc->add(tcpan);
       prpc->add(palcpan);
     }

     // top RpcLayer
     newpos += r->centralSupPanelThickness/2.;
     RpcLayer* ru = new RpcLayer(name,this);
     GeoVPhysVol* puppergg;
     if (cutoutson && vcutdef.size() > 0) {
       // This code required to take into account the various
       // 180 degree rotations of RPC panels in BMS chambers 
       int subtype = 0;
       int ijob = 0;
       for (unsigned int i = 0; i < vcutdef.size(); i++) {
         subtype = vcutdef[i]->subtype;
         ijob = vcutdef[i]->ijob;

         // For BMS2, BMS13
         if (name == "RPC06" && ijob == 3) {
           if (subtype == 2) {
             vcutdef[i]->dy = 0;
           } else if (subtype == 13) {
             vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
           }
         }
              
         // For BMS5, BMS9
         if (name == "RPC07") {
           if (subtype == 5) { // BMS5
             if (ijob == 24) vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
             if (ijob == 32) vcutdef[i]->dy = 0.;
           } else if (subtype == 9) { // BMS9
             if (ijob == 32) vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
             if (ijob == 24) vcutdef[i]->dy = 0.;
           }
         }

         // For BMS6, BMS11
         if (subtype == 6) {
           if (ijob == 29 && name == "RPC07") vcutdef[i]->dy = this->length - vcutdef[i]->lengthY;
           if (ijob == 21 && name == "RPC08") vcutdef[i]->dy = 0;
         } else if (subtype == 11) {
           if (ijob == 29 && name == "RPC07") vcutdef[i]->dy = 0;
           if (ijob == 21 && name == "RPC08") vcutdef[i]->dy = this->length - vcutdef[i]->lengthY - vcutdef[i]->dy;
         }
       }

       puppergg = ru->build(cutoutson, vcutdef);
     } else {
       puppergg = ru->build();
     }

     newpos += r->rpcLayerThickness/2.;
     GeoTransform* tugg = new GeoTransform(GeoTrf::TranslateX3D(newpos) );
     if (RPCprint) std::cout<< " Rpc:: put upper RPC layer at " << newpos
                            << " from centre " << std::endl;
     GeoTransform* rugg = new GeoTransform(GeoTrf::RotateY3D(180*Gaudi::Units::deg) );
     if (!skip_rpc) {
       prpc->add(new GeoIdentifierTag(2));
       prpc->add(tugg);
       prpc->add(rugg);
       prpc->add(puppergg);            
     }

     // release memory allocated for the builders 
     delete ru;
     delete rl;
     ru = 0;
     rl = 0;
   }

   return prpc;
}

void Rpc::print()
{
   std::cout <<"Rpc " << name.c_str() <<" :"<<std::endl;
}
} // namespace MuonGM
