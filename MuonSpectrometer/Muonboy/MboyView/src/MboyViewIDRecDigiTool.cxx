/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TruthHelper/IsGenStable.h"

#include "GaudiKernel/SystemOfUnits.h"

// ID
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
//SCT
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetIdentifier/SCT_ID.h"
//Pixel
#include "InDetRawData/PixelRDORawData.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GeoPrimitives/GeoPrimitives.h"

// GeoModel
#include "GeoModelInterfaces/IGeoModelSvc.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewIDRecDigiTool.h"

using namespace TruthHelper;

static const InterfaceID IID_IMboyViewIDRecDigiTool("MboyViewIDRecDigiTool", 1, 0);

const InterfaceID& MboyViewIDRecDigiTool::interfaceID()
{
  return IID_IMboyViewIDRecDigiTool;
}

MboyViewIDRecDigiTool::MboyViewIDRecDigiTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ):AthAlgTool(t,n,p),
                                             
 p_ITRT_DriftFunctionTool         ( "TRT_DriftFunctionTool/InDetTRT_DriftFunctionTool"                ) 
{

  declareInterface<MboyViewIDRecDigiTool>(this);

  declareProperty("TRT_DriftFunctionTool"         , p_ITRT_DriftFunctionTool         ) ;

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewIDRecDigiTool::~MboyViewIDRecDigiTool(){}

// Initialize
StatusCode MboyViewIDRecDigiTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  sc =detStore()->retrieve(m_trtmgr,"TRT");
  if (sc.isFailure() || !m_trtmgr) {
    ATH_MSG_DEBUG( "Could not retrieve  TRT_DetectorManager" ) ;
    m_trtmgr = 0 ;
  }

  sc =detStore()->retrieve(m_sctmgr,"SCT");
  if (sc.isFailure() || !m_sctmgr) {
    ATH_MSG_DEBUG( "Could not retrieve  SCT_DetectorManager") ;
    m_sctmgr = 0 ;
  }

  sc =detStore()->retrieve(m_pixelmgr,"Pixel");
  if (sc.isFailure() || !m_pixelmgr) {
    ATH_MSG_DEBUG( "Could not retrieve  Pixel_DetectorManager") ;
    m_pixelmgr = 0 ;
  }

  // Pixel
  if (detStore()->retrieve(p_PixelID, "PixelID").isFailure()) {
    ATH_MSG_DEBUG( "Could not retrieve Pixel ID helper" ) ;
    p_PixelID = 0 ;
  }

  // SCT
  if (detStore()->retrieve(p_SCT_ID, "SCT_ID").isFailure()) {
    ATH_MSG_DEBUG( "Could not retrieve SCT ID helper" ) ;
    p_SCT_ID = 0 ;
  }

  // TRT
  if (detStore()->retrieve(p_TRT_ID, "TRT_ID").isFailure()) {
    ATH_MSG_DEBUG( "Could not retrieve TRT ID helper" ) ;
    p_TRT_ID = 0 ;
  }

  //-- get a handle on the tool service -/
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_DEBUG( " Can't get ToolSvc" ) ;
    return( StatusCode::FAILURE );
  }

  if ( p_ITRT_DriftFunctionTool.retrieve().isFailure() ) {
    ATH_MSG_DEBUG( "Failed to retrieve tool " << p_ITRT_DriftFunctionTool ) ;
    p_ITRT_DriftFunctionTool = 0 ;
  }else{
    ATH_MSG_INFO( "Retrieved tool " << p_ITRT_DriftFunctionTool ) ;
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewIDRecDigiTool::finalize(){return StatusCode::SUCCESS;}

//---------------------------------------------------------------------------------
StatusCode MboyViewIDRecDigiTool::TRTRDORetrive(std::string TRTRDOContainerName,
                                                int m_ToTMin,int m_ToTMax,
                                                std::ofstream* pOut){
//---------------------------------------------------------------------------------

  StatusCode sc = StatusCode::SUCCESS;
  if ( m_trtmgr == 0 ) return sc ;
  if ( p_TRT_ID == 0 ) return sc ;
  if ( p_ITRT_DriftFunctionTool == 0 ) return sc ;

  std::string m_trtmodstr[4]={"BRA","ECA","BRC","ECC"};

  // retrieve collection and get a pointer
  const TRT_RDO_Container* p_rdocontainer;
  sc = evtStore()->retrieve(p_rdocontainer,TRTRDOContainerName );
  if(sc.isFailure()  ||  !p_rdocontainer){
   ATH_MSG_WARNING( "No TRT_RDO_Container found in StoreGate " <<TRTRDOContainerName) ;
    return sc;
  }

  TRT_RDO_Container::const_iterator containerItr;
  for( containerItr=p_rdocontainer->begin(); containerItr!=p_rdocontainer->end(); ++containerItr) {

   const InDetRawDataCollection< TRT_RDORawData > *rdoCollection=&(**containerItr);
   if (!rdoCollection) continue;
   DataVector<TRT_RDORawData>::const_iterator collectionItr;

    for(collectionItr=rdoCollection->begin();collectionItr!=rdoCollection->end() ; ++collectionItr) {

     const TRT_RDORawData *rdoData = *collectionItr;  
     Identifier rdoID = rdoData->identify();
     IdentifierHash IHc   = rdoCollection ->identifyHash();
     const InDetDD::TRT_BaseElement* pE = m_trtmgr->getElement(IHc);
  // retrieve the id helpers:
//    const TRT_ID* p_TRT_ID=m_trtmgr->getIdHelper();
     if(!p_TRT_ID) continue;

 // drift time in bin
     int m_dT = rdoData->driftTimeBin();
     bool m_hlevel = rdoData->highLevel();
 // Time over threshold in ns for valid digits; zero otherwise:
     double m_dToT = rdoData->timeOverThreshold();
     int m_barrel_ec = p_TRT_ID->barrel_ec(rdoID);
     int m_phi_module = p_TRT_ID->phi_module(rdoID);
     int m_layer_or_wheel =p_TRT_ID->layer_or_wheel(rdoID);
     int m_straw_layer =p_TRT_ID->straw_layer(rdoID);
     int m_straw = p_TRT_ID->straw(rdoID);

     int istr= abs(m_barrel_ec+1);

     int    newtdcvalue  = m_dT;
     bool   isOK;
     //fix me
     double t0=0.;
     double rawTime   = p_ITRT_DriftFunctionTool->rawTime(newtdcvalue);
     double radius    = p_ITRT_DriftFunctionTool->driftRadius(rawTime,rdoID,t0,isOK);
     //      double driftTime = rawTime-t0;


     Amg::Vector3D axis=pE->strawAxis(m_straw);
     axis.normalize();
     // Amg::Vector3D mv=(pE->strawLength()/2.)*axis;
     Amg::Translation3D transl1((pE->strawLength()/2.)*axis);
     Amg::Translation3D transl2(-(pE->strawLength()/2.)*axis);

     Amg::Vector3D strawend1= transl1*(pE->strawCenter(m_straw));

     Amg::Vector3D strawend2= transl2*(pE->strawCenter(m_straw));

      if(m_dToT>=m_ToTMin&&m_dToT<=m_ToTMax){
       *pOut<<"TRT   "
           <<"  "<<std::setw(5)<<m_trtmodstr[istr]
           <<"  "<<std::setw(5)<<m_phi_module
           <<"  "<<std::setw(5)<<m_layer_or_wheel
           <<"  "<<std::setw(5)<<m_straw_layer
           <<"  "<<std::setw(5)<<m_straw
           <<"  "<<std::setw(5)<<m_hlevel
           <<"  "<<std::setw(10)<<std::setprecision(3)<<m_dToT
           <<"  "<<std::setw(10)<<std::setprecision(3)<<radius
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend1.x()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend1.y()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend1.z()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend2.x()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend2.y()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<strawend2.z()
           <<std::endl;
     }
    }//  end of TRT_RDORawData loop
  } //end of TRT_RDO_Container loop

  return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------------
StatusCode MboyViewIDRecDigiTool::SCTRDORetrive(std::string SCTRDOContainerName,std::ofstream* pOut){
//---------------------------------------------------------------------------------

  StatusCode sc = StatusCode::SUCCESS;
  if ( m_sctmgr == 0 ) return sc ;
  if ( p_SCT_ID == 0 ) return sc ;
 
  std::string m_sctmodstr[4]={"BAR","ECA","BAR","ECC"};

  const SCT_RDO_Container *p_rdocontainer;
  sc = evtStore()->retrieve(p_rdocontainer,SCTRDOContainerName);
  if(sc.isFailure()  ||  !p_rdocontainer){
   ATH_MSG_WARNING( "No SCT_RDO_Container found in StoreGate " <<SCTRDOContainerName) ;
    return sc;
  }

  SCT_RDO_Container::const_iterator containerItr;
  for (containerItr=p_rdocontainer->begin(); containerItr!=p_rdocontainer->end(); containerItr++) {
    const DataVector<SCT_RDORawData> *rdoCollection = *containerItr;
    if (!rdoCollection) continue;
   // loop on all RDOs
    DataVector<SCT_RDORawData>::const_iterator collectionItr;
 
   for (collectionItr=rdoCollection->begin(); collectionItr!=rdoCollection->end(); collectionItr++) {
     //   for(p_rdo=p_rdocoll->begin(); p_rdo!=p_rdocoll->end() ; ++p_rdo) {
     const SCT_RDORawData *rdoData = *collectionItr;
     Identifier rdoId = rdoData->identify();

     const InDetDD::SiDetectorElement *element = m_sctmgr->getDetectorElement(rdoId);
//   const SCT_ID* p_SCT_ID = m_sctmgr->getIdHelper();
     if(!p_SCT_ID) continue;

     int m_barrel_ec= p_SCT_ID->barrel_ec(rdoId);
     if(m_barrel_ec==0)m_barrel_ec++;
     int istr= abs(m_barrel_ec+1);

     int m_layer_or_disk = p_SCT_ID->layer_disk(rdoId);
     int m_phi_module =  p_SCT_ID->phi_module(rdoId);
     int m_eta_module = p_SCT_ID->eta_module(rdoId);
     int m_side = p_SCT_ID->side(rdoId);
     int m_strip =rdoData->getStrip();


    // Returns position (center) of cell. These are corrected for the Lorentz shift 
    Amg::Vector2D localPos = element->localPositionOfCell(rdoId);
    // Special method for SCT to retrieve the two ends of a "strip"
    // Returned coordinates are in global frame.
     const std::pair<Amg::Vector3D, Amg::Vector3D > endsOfStrip = element->endsOfStrip(localPos);

     *pOut<<"SCT   "
          <<"  "<<std::setw(5)<<m_sctmodstr[istr]
          <<"  "<<std::setw(5)<<m_layer_or_disk
          <<"  "<<std::setw(5)<<m_phi_module
          <<"  "<<std::setw(5)<<m_eta_module
          <<"  "<<std::setw(5)<<m_side
          <<"  "<<std::setw(5)<<m_strip
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.first.x()
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.first.y()
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.first.z()
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.second.x()
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.second.y()
          <<"  "<<std::setw(10)<<std::setprecision(3)<<endsOfStrip.second.z()
          <<std::endl;

    } // end of SCT_RDORawData loop
  } //end of SCT_RDO_Container loop


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------
StatusCode MboyViewIDRecDigiTool::PixelRDORetrive(std::string PixelRDOContainerName,std::ofstream* pOut){
//---------------------------------------------------------------------------------

  StatusCode sc = StatusCode::SUCCESS;
  if ( m_pixelmgr == 0 ) return sc ;
  if ( p_PixelID  == 0 ) return sc ;

  std::string m_pixelmodstr[4]={"BAR","ECA","BAR","ECC"};

  const PixelRDO_Container *p_rdocontainer;
  sc = evtStore()->retrieve(p_rdocontainer,PixelRDOContainerName);
  if(sc.isFailure()  ||  !p_rdocontainer){
   ATH_MSG_WARNING( "No Pixel_RDO_Container found in StoreGate " <<PixelRDOContainerName) ;
    return sc;
  }

  PixelRDO_Container::const_iterator containerItr;
  for (containerItr=p_rdocontainer->begin(); containerItr!=p_rdocontainer->end(); containerItr++) {
   const DataVector<PixelRDORawData> *rdoCollection = *containerItr;
   if (!rdoCollection) continue;
   // loop on all RDOs
   DataVector<PixelRDORawData>::const_iterator collectionItr;

   for (collectionItr=rdoCollection->begin(); collectionItr!=rdoCollection->end(); collectionItr++) {
    const PixelRDORawData *rdoData = *collectionItr;
    Identifier rdoId = rdoData->identify();
//  const PixelID* p_PixelID   = m_pixelmgr->getIdHelper();
    if(!p_PixelID) continue;

    int m_barrel_ec= p_PixelID->barrel_ec(rdoId);
    if(m_barrel_ec==0) m_barrel_ec++;
    int istr= abs(m_barrel_ec+1);
    int m_layer_or_disk = p_PixelID->layer_disk(rdoId);
    int m_phi_module    = p_PixelID->phi_module(rdoId);
    int m_eta_module    = p_PixelID->eta_module(rdoId);
    int m_col           = p_PixelID->eta_index(rdoId);
    int m_row           = p_PixelID->phi_index(rdoId);
    
     const InDetDD::SiDetectorElement *element = m_pixelmgr->getDetectorElement(rdoId);
     //
     //  const PixelID* idHelper = dynamic_cast<const PixelID*>(element->getIdHelper());
     // if(!idHelper) continue;
  
     Amg::Vector2D localPos = element->localPositionOfCell(rdoId);
     Amg::Vector3D globalPos = element->globalPosition(localPos);

      *pOut<<"PIXEL "
           <<"  "<<std::setw(5)<<m_pixelmodstr[istr]
           <<"  "<<std::setw(5)<<m_layer_or_disk
           <<"  "<<std::setw(5)<<m_phi_module
           <<"  "<<std::setw(5)<<m_eta_module
           <<"  "<<std::setw(5)<<m_col
           <<"  "<<std::setw(5)<<m_row
           <<"  "<<std::setw(10)<<std::setprecision(3)<<globalPos.x()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<globalPos.y()
           <<"  "<<std::setw(10)<<std::setprecision(3)<<globalPos.z()
           <<std::endl;


   }//  end of PixelRDORawData loop
  }//end of PixelRDO_Container loop

  return StatusCode::SUCCESS;
}

