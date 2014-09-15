/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyDetectorLimitsTool.h"

//----------------------------------------------------------------//

namespace Muon {

MboyDetectorLimitsTool::MboyDetectorLimitsTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyDetectorLimitsTool>(this);

//Declare the properties
  declareProperty("RadiusID"                   , m_RadiusID    =  1050.0) ;
  declareProperty("fabsZID"                    , m_fabsZID     =  3200.0) ;
  declareProperty("RadiusCalo"                 , m_RadiusCalo  =  4250.0) ;
  declareProperty("fabsZCalo"                  , m_fabsZCalo   =  6750.0) ;
  declareProperty("EpsMinus"                   , m_EpsMinus   =      0.1) ;
  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

}

MboyDetectorLimitsTool::~MboyDetectorLimitsTool(){}

// Initialize
StatusCode MboyDetectorLimitsTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are     " );
  ATH_MSG_INFO( "= ToolName                        " << m_ToolName                               ) ;
  ATH_MSG_INFO(  "= RadiusID                       " << m_RadiusID                                     );
  ATH_MSG_INFO(  "= fabsZID                        " << m_fabsZID                                      );
  ATH_MSG_INFO(  "= RadiusCalo                     " << m_RadiusCalo                                   );
  ATH_MSG_INFO(  "= fabsZCalo                      " << m_fabsZCalo                                    );
  ATH_MSG_INFO(  "= EpsMinus                       " << m_EpsMinus                                    );
  ATH_MSG_INFO( "================================" );

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboyDetectorLimitsTool::finalize(){return StatusCode::SUCCESS;}

int MboyDetectorLimitsTool::AreaTag(const Trk::TrackParameters* pTrackParameters)
{
  return AreaTag( (pTrackParameters->position()).x() , (pTrackParameters->position()).y() , (pTrackParameters->position()).z() );
}

int MboyDetectorLimitsTool::AreaTag(
              double XPosi  ,
              double YPosi  ,
              double ZPosi  
){

  double Radius = sqrt ( XPosi*XPosi + YPosi*YPosi );
  double fabsZ  = fabs ( ZPosi );
 
  if ( Radius <= (m_RadiusID  -m_EpsMinus) && fabsZ < (m_fabsZID  -m_EpsMinus) ) return 1 ;
  if ( Radius <= (m_RadiusCalo-m_EpsMinus) && fabsZ < (m_fabsZCalo-m_EpsMinus) ) return 2 ;
  return 3;

}
int MboyDetectorLimitsTool::IsOnCylinder(const Trk::TrackParameters* pTrackParameters)
{

  double XCrossing = (pTrackParameters->position()).x()   ;
  double YCrossing = (pTrackParameters->position()).y()   ;
  double ZCrossing = (pTrackParameters->position()).z()   ;
  double RCrossing = sqrt ( XCrossing*XCrossing + YCrossing*YCrossing );
  ZCrossing = fabs(ZCrossing);

  if ( RCrossing/ZCrossing > m_RadiusCalo/m_fabsZCalo ) return 1 ;
  return 0 ;

}

}
