///////////////////////////////////////////////////////////////////
// LayerProviderXML.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/LayerProviderXML.h"
#include "InDetTrackingGeometryXML/BarrelBuilderXML.h"
#include "InDetTrackingGeometryXML/EndcapBuilderXML.h"
#include "InDetTrackingGeometryXML/IModuleProvider.h"
#include "TrkGeometryAlpine/AlpineLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/DiscTrapezoidalBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"

InDet::LayerProviderXML::LayerProviderXML(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_identification("Pixel"),
  m_doPix(true),
  m_doSCT(true),
  m_startLayer(0),
  m_endLayer(-1),
  m_startEndcap(0),
  m_endEndcap(-1),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","InDetXMLReaderSvc"),
  m_pixBarrelBuilder("InDet::BarrelBuilderXML/PixelsBarrelBuilder"),
  m_sctBarrelBuilder("InDet::BarrelBuilderXML/SCTBarrelBuilder"),
  m_pixEndcapBuilder("InDet::EndcapBuilderXML/PixelEndcapBuilder"),
  m_sctEndcapBuilder("InDet::EndcapBuilderXML/SCTEndcapBuilder"),
  m_moduleProvider("InDet::SiModuleProvider/SiModuleProvider")
{
  declareInterface<Trk::ILayerProvider>(this);

  declareProperty("Identification",     m_identification);
  declareProperty("InDetXMLReader",     m_xmlReader);
  declareProperty("PixelBarrelBuilder", m_pixBarrelBuilder);
  declareProperty("SCTBarrelBuilder",   m_sctBarrelBuilder);
  declareProperty("PixelEndcapBuilder", m_pixEndcapBuilder);
  declareProperty("SCTEndcapBuilder",   m_sctEndcapBuilder);
  declareProperty("ModuleProvider",     m_moduleProvider);
  declareProperty("doPix",              m_doPix);
  declareProperty("doSCT",              m_doSCT);
  declareProperty("startLayer",         m_startLayer);
  declareProperty("endLayer",           m_endLayer);
  declareProperty("startEndcap",        m_startEndcap);
  declareProperty("endEndcap",          m_endEndcap);
  m_surfcoll = 0;
  m_SiSurface = 0;
  m_ndumpRZ = 0;
}

InDet::LayerProviderXML::~LayerProviderXML()
{
}

const std::string& InDet::LayerProviderXML::identification() const
{
  return m_identification;
}

StatusCode InDet::LayerProviderXML::initialize()
{
  ATH_MSG_INFO("InDet::LayerProviderXML::initialize()");  

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_INFO("InDet::XMLReaderSvc successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("InDet::XMLReaderSvc: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  // Set pixel and SCT cases
  m_pixBarrelBuilder->setPixelCase(true);
  m_pixEndcapBuilder->setPixelCase(true);
  m_sctBarrelBuilder->setPixelCase(false);
  m_sctEndcapBuilder->setPixelCase(false);

  // Initialize SurfaceCollection for VP1
  StatusCode sc = evtStore()->retrieve(m_surfcoll,"SurfaceCollection");
  if ( sc.isFailure() ) {
    m_surfcoll = new SurfaceCollection(SG::VIEW_ELEMENTS );
    StatusCode sc = evtStore()->record(m_surfcoll,"SurfaceCollection");
    if ( sc.isFailure() )
      ATH_MSG_ERROR("Could not register SurfaceCollection in StoreGate");  
    else 
      ATH_MSG_INFO("SurfaceCollection with " << m_surfcoll->size() << " elements successfully registered in StoreGate");
   }

  // create root file for simple vizualisation of modules positions
  createMainFileRZ("InDetLayoutRZ");
  std::string filename = "InDet";
  //if(m_doSCT || m_startLayer>0)
  filename += m_identification;
  filename += "LayoutRZ";
  openDumpFileRZ(filename);

  return StatusCode::SUCCESS;
}

StatusCode InDet::LayerProviderXML::finalize()
{
  // close root file  
  closeDumpFileRZ();

  ATH_MSG_INFO( "################ Total Silicon Surface: "<< m_SiSurface << " m2 ###############");

  return StatusCode::SUCCESS;
}

// LayerBuilder interface method - returning Barrel-like layers
const std::vector< const Trk::Layer* > InDet::LayerProviderXML::centralLayers() const
{
  // construct the vector of active layers
  std::vector< const Trk::Layer* > layers;
  // We are smarter than "const"
  unsigned int* ndump = (unsigned int*) &m_ndumpRZ;

  if(m_doPix && m_pixBarrelBuilder->nbOfLayers() > 0){ 
    *ndump = m_surfcoll->size();
    createPixelBarrel(layers);
    dumpRZ("Pixel_Barrel");
  }
  if(m_doSCT && m_sctBarrelBuilder->nbOfLayers() > 0){
    *ndump = m_surfcoll->size();
    createSCTBarrel(layers);
    dumpRZ("SCT_Barrel");
  }
  
//   FOR DEBUGGING
//   for (unsigned int ll = 0; ll< layers.size(); ll++) {
//     // get the subsurface array
//     const Trk::SurfaceArray* surfArray = layers.at(ll)->surfaceArray();
//     if (surfArray) {
//       const std::vector<const Trk::Surface*>& layerSurfaces = surfArray->arrayObjects();
//       ATH_MSG_DEBUG(__PRETTY_FUNCTION__ << "   ---> has " << layerSurfaces.size() << " surfaces on the layer.");
//     }
//   }    

  return layers;
} 

// LayerBuilder interface method - returning Endcap-like layers
const std::vector< const Trk::Layer* > InDet::LayerProviderXML::negativeLayers() const
{
  // construct the vector of layers
  std::vector< const Trk::Layer* > layers;

  // Get around "const" for dumper
  unsigned int* ndump = (unsigned int*) &m_ndumpRZ;

  if(m_doPix && m_pixEndcapBuilder->nbOfLayers() > 0) {
    *ndump = m_surfcoll->size();
    createPixelEndcap(layers,-1);
    dumpRZ("Pixel_Endcap_neg");
  }
  if(m_doSCT && m_sctEndcapBuilder->nbOfLayers() > 0){
    *ndump = m_surfcoll->size();
    createSCTEndcap(layers,-1);
    dumpRZ("SCT_Endcap_neg");
  }

  return layers;
}

// LayerBuilder interface method - returning Endcap-like layers
const std::vector< const Trk::Layer* > InDet::LayerProviderXML::positiveLayers() const
{
  // construct the vector of layers
  std::vector< const Trk::Layer* > layers;

  // Get around "const" for dumper
  unsigned int* ndump = (unsigned int*) &m_ndumpRZ;

  if(m_doPix && m_pixEndcapBuilder->nbOfLayers() > 0) {
    *ndump = m_surfcoll->size();
    createPixelEndcap(layers,1);
    dumpRZ("Pixel_Endcap_pos");
  }
  if(m_doSCT && m_sctEndcapBuilder->nbOfLayers() > 0) {
    *ndump = m_surfcoll->size();
    createSCTEndcap(layers,1);
    dumpRZ("SCT_Endcap_pos");
  }

  return layers;
}

void InDet::LayerProviderXML::createPixelBarrel(std::vector< const Trk::Layer* >& cylinderLayers) const
{
  bool isPixel = true;
  unsigned int nTemplates = m_pixBarrelBuilder->nbOfLayers();

  for(unsigned int itmp=0;itmp<nTemplates;itmp++) {
    Trk::CylinderLayer* layer = m_pixBarrelBuilder->createActiveLayer(itmp,m_startLayer,m_endLayer);
    
    if(layer==0) {
      if(m_startLayer==0 && m_endLayer==-1) 
	ATH_MSG_WARNING("Pixel cylinder for active layer template: " << itmp << " was not created");
      continue;
    }

    // Register surfaces for VP1 visualization and DetElements for digitization
    bool isAlpine = m_pixBarrelBuilder->isAlpineLayer(itmp);

    if(isAlpine) {
      Trk::AlpineLayer* alayer = (Trk::AlpineLayer*) layer;
      alpineToCollections(isPixel, alayer);
    } else {
      layerToCollections(isPixel, layer);
    }

    // Save layer in vector
    cylinderLayers.push_back(layer);
  }

//   FOR DEBUGGING
//   for (unsigned int ll = 0; ll< cylinderLayers.size(); ll++) {
//     // get the subsurface array
//     const Trk::SurfaceArray* surfArray = cylinderLayers.at(ll)->surfaceArray();
//     if (surfArray) {
//       const std::vector<const Trk::Surface*>& layerSurfaces = surfArray->arrayObjects();
//       ATH_MSG_DEBUG(__PRETTY_FUNCTION__ << "   ---> has " << layerSurfaces.size() << " surfaces on the layer.");
//     }
//   }  
}

void InDet::LayerProviderXML::createSCTBarrel(std::vector< const Trk::Layer* >& cylinderLayers) const
{
  bool isPixel = false;
  unsigned int nTemplates = m_sctBarrelBuilder->nbOfLayers();

  // create active layers
  for(unsigned int itmp=0;itmp<nTemplates;itmp++) {
    Trk::AlpineLayer* layer = (Trk::AlpineLayer*) m_sctBarrelBuilder->createActiveLayer(itmp,m_startLayer,m_endLayer);
    if(layer==0) {
      if(m_startLayer==0 && m_endLayer==-1) 
	ATH_MSG_WARNING("Can not create SCT cylinder for active layer template: " << itmp << " was not created");
      continue;
    }						

    // Register surfaces for VP1 visualization and DetElements for digitization
    bool isAlpine = m_sctBarrelBuilder->isAlpineLayer(itmp);

    if(isAlpine) {
      Trk::AlpineLayer* alayer = (Trk::AlpineLayer*) layer;
      alpineToCollections(isPixel, alayer);
    } else {
      layerToCollections(isPixel, layer);
    }

    // Save layer in vector
    cylinderLayers.push_back(layer);
  }
}

void InDet::LayerProviderXML::createPixelEndcap(std::vector< const Trk::Layer* >& discLayers, int side) const
{
  bool isPixel = true;
  unsigned int nTemplates = m_pixEndcapBuilder->nbOfLayers();

  for(unsigned int itmp=0;itmp<nTemplates;itmp++){  
    std::vector< Trk::Layer* > layers;
    //m_pixEndcapBuilder->createActiveLayers(itmp,side,m_startEndcap,m_endEndcap,layers);
    m_pixEndcapBuilder->createActiveLayers(itmp,side,m_startLayer,m_endLayer,layers);
    if(layers.size()==0) {
      if(m_startLayer==0 && m_endLayer==-1) ATH_MSG_WARNING("PixelEndcap Disc for active layer: " <<  itmp << " was not created");
      continue;
    }
    // Register surfaces for VP1 visualization and DetElements for digitization
    for(unsigned int il=0; il < layers.size();il++) {
      layerToCollections(isPixel, layers.at(il));
      discLayers.push_back(layers.at(il));
    }
    ATH_MSG_DEBUG("        --> " <<  layers.size() << " DiscLayers created"); 
  }
}

void InDet::LayerProviderXML::createSCTEndcap(std::vector< const Trk::Layer* >& discLayers, int side) const
{
  bool isPixel = false;
  unsigned int nTemplates = m_sctEndcapBuilder->nbOfLayers();

  for(unsigned int itmp=0;itmp<nTemplates;itmp++){  
    std::vector< Trk::Layer* > layers;
    m_sctEndcapBuilder->createActiveLayers(itmp,side,m_startLayer,m_endLayer,layers);    
    if(layers.size()==0) {
      if(m_startLayer==0 && m_endLayer==-1) ATH_MSG_WARNING("PixelEndcap Disc for active layer: " <<  itmp<< " was not created");
      continue;
    }
    // Register surfaces for VP1 visualization and DetElements for digitization
    for(unsigned int il=0; il < layers.size();il++) {
      layerToCollections(isPixel, layers.at(il));
      discLayers.push_back(layers.at(il));
    }
    ATH_MSG_DEBUG("       --> " <<  layers.size() << " DiscLayers created"); 
  }
}

void InDet::LayerProviderXML::alpineToCollections(bool isPixel, Trk::AlpineLayer *layer) const
{

  std::vector<const Trk::Surface*> lsurf = layer->getSurfaces();
  for (unsigned int is=0;is<lsurf.size();is++) {
    m_surfcoll->push_back(lsurf.at(is));
    m_moduleProvider->mapSurfaceDetElement(isPixel, lsurf.at(is));
  }
}

void InDet::LayerProviderXML::layerToCollections(bool isPixel, Trk::Layer *layer) const
{
  const Trk::SurfaceArray* array = layer->surfaceArray();
  const std::vector<const Trk::Surface*> lsurf = array->arrayObjects();
  for (unsigned int is=0;is<lsurf.size();is++) {
    const Trk::TrkDetElementBase* pElement  = dynamic_cast<const Trk::TrkDetElementBase*>((lsurf.at(is))->associatedDetectorElement());
    m_moduleProvider->mapSurfaceDetElement(isPixel, lsurf.at(is));
    if (pElement) {
      const std::vector<const Trk::Surface*> surfaces = pElement->surfaces();
      m_surfcoll->insert(m_surfcoll->end(),surfaces.begin(),surfaces.end());
    } else {
      m_surfcoll->push_back(lsurf.at(is));  
    }
  }
}

void InDet::LayerProviderXML::createMainFileRZ(std::string filename)
{ 
  m_dumpfileRZ.open(filename+".C");
  m_dumpfileRZ << "#ifndef " << filename << "_C" << std::endl;
  m_dumpfileRZ << "#define " << filename << "_C" << std::endl;
  m_dumpfileRZ << "#include \"InDetPixelLayoutRZ.C\"" << std::endl;
  m_dumpfileRZ << "#include \"InDetSCTLayoutRZ.C\"" << std::endl;
  m_dumpfileRZ << std::endl;

  m_dumpfileRZ << "void " << filename << "() {" << std::endl;
  m_dumpfileRZ << "   double area = 0;" << std::endl;
  if(m_xmlReader->doSCT()) {
    m_dumpfileRZ << "   area += InDetSCTLayoutRZ(true);" << std::endl;
    m_dumpfileRZ << "   area += InDetPixelLayoutRZ(false);" << std::endl;  
  } else {
    m_dumpfileRZ << "   area += InDetPixelLayoutRZ(true);" << std::endl;  
  }
  m_dumpfileRZ << "    std::cout <<  \"=========================================\" << std::endl;" << std::endl;
  m_dumpfileRZ << "    std::cout <<  \"Total Silicon area = \" << area << \" m2\" << std::endl;" << std::endl;
  m_dumpfileRZ << "}" << std::endl;
  m_dumpfileRZ << std::endl;

  m_dumpfileRZ << "#endif" << std::endl;
  m_dumpfileRZ.close();
}

void InDet::LayerProviderXML::openDumpFileRZ(std::string filename)
{ 
  m_dumpfileRZ.open(filename+".C");
  m_dumpfileRZ << "#ifndef " << filename << "_C" << std::endl;
  m_dumpfileRZ << "#define " << filename << "_C" << std::endl;
  if(m_startLayer>0) {
    m_dumpfileRZ << "#include \"InDetPixel0LayoutRZ.C\"" << std::endl;
  }
  m_dumpfileRZ << std::endl;

  m_dumpfileRZ << "double " << filename << "(bool createCanvas=true) {" << std::endl;
  m_dumpfileRZ << "    TLine* l;" << std::endl;
  m_dumpfileRZ << "    TBox* b;" << std::endl;
  m_dumpfileRZ << "    // Pixel layer 0 area if splitted" << std::endl;
  m_dumpfileRZ << "    double area_Pixel0 = 0;" << std::endl;

  m_dumpfileRZ << std::endl;
  m_dumpfileRZ << "    if(createCanvas) {" << std::endl;
  m_dumpfileRZ << "      TCanvas* crz = new TCanvas(\"InDetLayoutRZ\",\"InDet Layout (r,z)\", 1200,600);" << std::endl;
  m_dumpfileRZ << "      crz->Divide(1,1);" << std::endl;
  m_dumpfileRZ << "      crz->cd(1);" << std::endl;
  m_dumpfileRZ << "      gPad->SetGridx();" << std::endl;
  m_dumpfileRZ << "      gPad->SetGridy();" << std::endl;
  if(m_doSCT) m_dumpfileRZ << "      TH2F *hrz = new TH2F(\"InDetLayoutRZ\",\"InDet Layout (r,z)\",32,0.,3200.,110,0.,1100.);" << std::endl;
  else        m_dumpfileRZ << "      TH2F *hrz = new TH2F(\"InDetLayoutRZ\",\"InDet Layout (r,z)\",32,0.,3200., 40,0.,400.);" << std::endl;
  m_dumpfileRZ << "      hrz->SetStats(0);" << std::endl;
  m_dumpfileRZ << "      hrz->GetYaxis()->SetTitle(\"R [mm]\");" << std::endl;
  m_dumpfileRZ << "      hrz->GetXaxis()->SetTitle(\"z [mm]\");" << std::endl;
  m_dumpfileRZ << "      hrz->Draw(\"h\");" << std::endl;
  m_dumpfileRZ << std::endl;
  m_dumpfileRZ << "      TLatex latex;" << std::endl;
  m_dumpfileRZ << "      latex.SetTextSize(0.03);" << std::endl;
  if(m_doSCT) {
    m_dumpfileRZ << "      latex.SetTextColor(kBlue+2);" << std::endl;
    m_dumpfileRZ << "      latex.DrawText(950,1050,\"SCT barrel\");" << std::endl;
    m_dumpfileRZ << "      latex.SetTextColor(kBlue);" << std::endl;
    m_dumpfileRZ << "      latex.DrawText(2650,1050,\"SCT endcap\");" << std::endl;
  }
  dumpEtaLinesRZ(3200.,1100.);
  dumpTubesRZ(0); // Set zMax argument to 0 if dumpEtaLinesRZ is called to avoid double definition of zMax
  m_dumpfileRZ << "      latex.SetTextColor(kMagenta+2);" << std::endl;
  m_dumpfileRZ << "      latex.DrawText(500,300,\"Pixel barrel\");" << std::endl;
  m_dumpfileRZ << "      latex.SetTextColor(kMagenta);" << std::endl;
  m_dumpfileRZ << "      latex.DrawText(2650,50,\"Pixel endcap\");" << std::endl;
  m_dumpfileRZ << "    }" << std::endl;
  m_dumpfileRZ << std::endl;
  if(m_startLayer>0) {
    m_dumpfileRZ << "    // Draw Pixel layer 0 if needed" << std::endl;
    m_dumpfileRZ << "    area_Pixel0 += InDetPixel0LayoutRZ(false);" << std::endl;
    m_dumpfileRZ << std::endl;
  }
  m_dumpfileRZ << "    // R,Z representation of surface, no thickness" << std::endl;
}

void InDet::LayerProviderXML::closeDumpFileRZ()
{ 
  if(m_doPix && m_pixEndcapBuilder->nbOfLayers() > 0) {
    m_dumpfileRZ << "    double area_Pixel_Endcap = area_Pixel_Endcap_neg + area_Pixel_Endcap_pos;" << std::endl;
    m_dumpfileRZ << "    std::cout <<  \"" << m_identification << " Silicon area = \" << area_Pixel_Endcap << \" m2 for Pixel_Endcap\" << std::endl;" << std::endl;
  }
  if(m_doSCT && m_sctEndcapBuilder->nbOfLayers() > 0) {
    m_dumpfileRZ << "    double area_SCT_Endcap = area_SCT_Endcap_neg + area_SCT_Endcap_pos;" << std::endl;
    m_dumpfileRZ << "    std::cout <<  \"" << m_identification << " Silicon area = \" << area_SCT_Endcap << \" m2 for SCT_Endcap\" << std::endl;" << std::endl;
  }

  m_dumpfileRZ << "    if(area_Pixel0!=0) std::cout <<  \"" << m_identification << " Silicon area = \" << " << m_SiSurface << " << \" + \" << area_Pixel0 << \" = \" << " << m_SiSurface << " + area_Pixel0 << \" m2\" << std::endl;" << std::endl;
  m_dumpfileRZ << "    else               std::cout <<  \"" << m_identification << " Silicon area = \" << " << m_SiSurface << " << \" m2\" << std::endl;" << std::endl;
  m_dumpfileRZ << "    return " << m_SiSurface << " + area_Pixel0;" << std::endl;
  m_dumpfileRZ << "}" << std::endl;
  m_dumpfileRZ << std::endl;
  m_dumpfileRZ << "#endif " << std::endl;

  m_dumpfileRZ.close();
}

void InDet::LayerProviderXML::dumpRZ(std::string title) const
{ 
  unsigned int nsurfaces = m_surfcoll->size();

  int ntodo =  nsurfaces-m_ndumpRZ;

  std::ofstream *dumpfile = (std::ofstream *) &m_dumpfileRZ;

  *dumpfile << "    // ---------------------- " << title << " number of surfaces found: " << ntodo << std::endl;

  double surface = 0;

  for(unsigned int i=m_ndumpRZ;i<nsurfaces;i++) {
    const Trk::Surface* sf = m_surfcoll->at(i);

    // to compute the module area
    double halfY    = 0.;  // Y is half the trapezoid height
    double maxHalfX = 0.;  // Xmax is half the large base
    double minHalfX = 0.;  // Xmin is half the small base

    // check bounds    
    const Trk::SurfaceBounds* sfbnd = (&(sf->bounds()));

    // it is a rectangular surface
    if(sfbnd->type() == Trk::SurfaceBounds::Rectangle) {
      const Trk::RectangleBounds* bnd = dynamic_cast<const Trk::RectangleBounds*>(&(sf->bounds()));
      halfY    = bnd->halflengthY();  
      maxHalfX = bnd->halflengthX();
      minHalfX = bnd->halflengthX();
    // or a trapezoid surface
    } else if (sfbnd->type() == Trk::SurfaceBounds::Trapezoid){
      const Trk::TrapezoidBounds* bnd = dynamic_cast<const Trk::TrapezoidBounds*>(&(sf->bounds()));
      halfY    = bnd->halflengthY();  
      maxHalfX = bnd->maxHalflengthX();
      minHalfX = bnd->minHalflengthX();
    // or a disc trapezoidal surface
    } else if (sfbnd->type() == Trk::SurfaceBounds::DiscTrapezoidal) {
      const Trk::DiscTrapezoidalBounds* bnd = dynamic_cast<const Trk::DiscTrapezoidalBounds*>(&(sf->bounds()));
      halfY    = bnd->halflengthY();  
      maxHalfX = bnd->maxHalflengthX();
      minHalfX = bnd->minHalflengthX();
    } else { 
      ATH_MSG_WARNING("No valid bounds found for surface (not rectangular nor trapezoid)" << *sf);      
      continue;
    }

    surface += 0.5*(2*maxHalfX+2*minHalfX)*(2*halfY)*0.001*0.001 ; // surface in m2

    // Local to global
    Amg::Vector2D loc1(0,-halfY); 
    Amg::Vector2D loc2(0, halfY); 

    const Amg::Vector3D *v1=sf->localToGlobal(loc1);
    const Amg::Vector3D *v2=sf->localToGlobal(loc2);

    double z1   = v1->z();
    double phi1 = v1->phi();

    if(phi1>0.397)  continue; // if surface past  45/2 degrees don't draw
    if(phi1<-0.397) continue; // if surface past -45/2 degrees don't draw
    if(z1<0) continue; // If negative side, don't draw

    double x1   = v1->x();
    double y1   = v1->y();
    double x2   = v2->x();
    double y2   = v2->y();
    double z2   = v2->z();
    double r1   = sqrt(x1*x1+y1*y1);
    double r2   = sqrt(x2*x2+y2*y2);    

    // Setup color
    std::string kcolor = "kRed"; // Default
    if(title.find("SCT") != std::string::npos) {
      if(title.find("Barrel") != std::string::npos) kcolor = "kBlue"; // SCT-Barrel
      else kcolor = "kBlue-2"; // SCT-Endcap
    } else {
      if(title.find("Barrel") != std::string::npos) kcolor = "kMagenta"; // Pixel-Barrel
      else kcolor = "kMagenta-2"; // Pixel-Endcap
    }

    // Root TLine
    *dumpfile << "    l = new TLine(" 
	      << z1 << "," << r1 << "," << z2 << "," << r2 
	      << "); l->SetLineColor(" << kcolor.c_str() << "+2); l->SetLineWidth(4); l->Draw(\"same\"); // surface " 
	      << i << std::endl;
  }

  ATH_MSG_INFO("=== " << title << " contains " << ntodo << " surfaces (" << surface << " m2 of silicon)");

  *dumpfile << "    double area_" << title << " = " << surface << ";" << std::endl;
  *dumpfile << "    std::cout <<  \"" << m_identification << " Silicon area = \" << area_" << title << " << \" m2 for " << title << " with " << ntodo << " surfaces \" << std::endl;" << std::endl;
  m_SiSurface += surface;
}

void InDet::LayerProviderXML::dumpEtaLinesRZ(double zMax, double rMax) const
{
  std::ofstream *dumpfile = (std::ofstream *) &m_dumpfileRZ;

  *dumpfile << "      TLine line1;" << std::endl;
  *dumpfile << "      double zMax = " << zMax << ";" << std::endl;
  *dumpfile << "      double rMax = " << rMax << ";" << std::endl;
  *dumpfile << "      // draw eta lines - Zvertex =0" << std::endl;
  *dumpfile << "      line1.SetLineStyle(2);" << std::endl;
  *dumpfile << "      line1.SetLineColor(kGray);" << std::endl;
  *dumpfile << "      for (int ieta=2;ieta<48;ieta+=2){" << std::endl;
  *dumpfile << "        double eta = ieta/10.;" << std::endl;
  *dumpfile << "        double angle = 2*atan(exp(-eta));" << std::endl;
  *dumpfile << "        double r = zMax*tan(angle);" << std::endl;
  *dumpfile << "        double z = zMax;" << std::endl;
  *dumpfile << "        if(r>rMax) {" << std::endl;
  *dumpfile << "          r = rMax;" << std::endl;
  *dumpfile << "          z = rMax/tan(angle);" << std::endl;
  *dumpfile << "        }" << std::endl;
  *dumpfile << "        if(ieta%10==0) line1.SetLineWidth(2);" << std::endl;
  *dumpfile << "        else line1.SetLineWidth(1);" << std::endl;
  *dumpfile << "        line1.DrawLine(0.,0.,z,r);" << std::endl;
  *dumpfile << "      }" << std::endl;
}

void InDet::LayerProviderXML::dumpTubesRZ(double zMax) const
{
  std::ofstream *dumpfile = (std::ofstream *) &m_dumpfileRZ;

  *dumpfile << "      TBox box1;" << std::endl;
  if(zMax>0) *dumpfile << "      double zMax = " << zMax << ";" << std::endl;
  *dumpfile << "      box1.SetFillColor(kRed);" << std::endl;
  *dumpfile << "      // draw beampipe" << std::endl;
  *dumpfile << "      box1.DrawBox(0,32.1,zMax,37.4);" << std::endl;
  *dumpfile << "      // draw IST" << std::endl;
  *dumpfile << "      if(zMax>3200.0) zMax = 3000.0;" << std::endl;
  *dumpfile << "      box1.DrawBox(0,133.0,zMax,139.0);" << std::endl;
  *dumpfile << "      // draw PST" << std::endl;
  *dumpfile << "      box1.DrawBox(0,341.0,zMax,361.75);" << std::endl;
}
