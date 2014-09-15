/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MboyEDMToolInterfaces/IMboyDetectorLimitsTool.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboySortingTool.h"

//----------------------------------------------------------------//

namespace Muon {

MboySortingTool::MboySortingTool(const std::string& t, 
                                             const std::string& n,
                                             const IInterface*  p ):AthAlgTool(t,n,p)
, p_IMboyDetectorLimitsTool          ( "Muon::MboyDetectorLimitsTool/MboyDetectorLimitsTool" )
{

  declareInterface<IMboySortingTool>(this);

  declareProperty("IMboyDetectorLimitsTool"    , p_IMboyDetectorLimitsTool ) ;

  declareProperty("ToolName"          , m_ToolName          = n                           ) ;

  m_DoDbg = 0 ;

}

MboySortingTool::~MboySortingTool(){}

// Initialize
StatusCode MboySortingTool::initialize(){

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
  ATH_MSG_INFO( "================================" );

//Retrieve p_IMboyDetectorLimitsTool
  if ( p_IMboyDetectorLimitsTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << p_IMboyDetectorLimitsTool );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Retrieved tool " << p_IMboyDetectorLimitsTool );

  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MboySortingTool::finalize(){return StatusCode::SUCCESS;}


void MboySortingTool::SortPositions(
                              int /*DoDbg*/ ,
                              std::vector<int>&  LinearIndexSorted ,
                              std::vector<int>&  AreaTagSorted ,
                              double XrefStart,
                              double YrefStart,
                              double ZrefStart,
                              std::vector<double> PointXdca ,
                              std::vector<double> PointYdca ,
                              std::vector<double> PointZdca ,
                              std::vector<int>    PointInde  
){
  LinearIndexSorted.clear();
  
  int    iNew = 1 ;
  int    IndexKeep = 0 ;
  double XrefStartNew = 0. ;
  double YrefStartNew = 0. ;
  double ZrefStartNew = 0. ;
  double DistanceToRefMAX =-99999999. ; 

  int TotalNberOfElementsTOBESORTED = PointXdca.size();
  for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
    double Xdca = PointXdca[Index] ;
    double Ydca = PointYdca[Index] ;
    double Zdca = PointZdca[Index] ;
   
    double DistanceToZero = sqrt ( 
                                  (XrefStart-Xdca)*(XrefStart-Xdca)
                                + (YrefStart-Ydca)*(YrefStart-Ydca)
                                + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                );
    if ( DistanceToZero > DistanceToRefMAX )DistanceToRefMAX = DistanceToZero;

    int Index2Start = Index + 1 ;
    for (int Index2=Index2Start; Index2<TotalNberOfElementsTOBESORTED; Index2++){
      double Xdca2 = PointXdca[Index2] ;
      double Ydca2 = PointYdca[Index2] ;
      double Zdca2 = PointZdca[Index2] ;
      
      double DistanceInter= sqrt ( 
                                    (Xdca2-Xdca)*(Xdca2-Xdca)
                                  + (Ydca2-Ydca)*(Ydca2-Ydca)
                                  + (Zdca2-Zdca)*(Zdca2-Zdca)
                                  );
      if ( DistanceInter > DistanceToRefMAX )DistanceToRefMAX = DistanceInter;
      
    }
    
  }

  DistanceToRefMAX = 2. * DistanceToRefMAX ;
    
  double DistanceToRefmin = DistanceToRefMAX ; 
    
  while ( iNew == 1 ){
    iNew = 0 ; 
    for (int Index=0; Index<TotalNberOfElementsTOBESORTED; Index++){
      int IndexTrue = PointInde[Index];
      int IsExcluded = 0 ;
      int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
      for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
        if ( LinearIndexSorted[iSorted] == IndexTrue ) IsExcluded = 1 ;
        if ( LinearIndexSorted[iSorted] == IndexTrue ) break ;
      }
      if ( IsExcluded == 0 ){
        iNew = 1 ;

        double Xdca = PointXdca[Index] ;
        double Ydca = PointYdca[Index] ;
        double Zdca = PointZdca[Index] ;
        double DistanceToRef = sqrt ( 
                                      (XrefStart-Xdca)*(XrefStart-Xdca)
                                    + (YrefStart-Ydca)*(YrefStart-Ydca)
                                    + (ZrefStart-Zdca)*(ZrefStart-Zdca)
                                    );
        if ( DistanceToRef < DistanceToRefmin ){
          DistanceToRefmin = DistanceToRef ;
          IndexKeep = IndexTrue ;
          XrefStartNew = Xdca ;
          YrefStartNew = Ydca ;
          ZrefStartNew = Zdca ;
        }
      }
    }
    if ( iNew == 1 ){
      LinearIndexSorted.push_back(IndexKeep) ;
      XrefStart = XrefStartNew ;
      YrefStart = YrefStartNew ;
      ZrefStart = ZrefStartNew ;
      int TheAreaTag = p_IMboyDetectorLimitsTool->AreaTag(XrefStart,YrefStart,ZrefStart);
      AreaTagSorted.push_back(TheAreaTag) ;
      DistanceToRefmin = DistanceToRefMAX ; 
    }
  }
  
  if ( m_DoDbg == 1 ) {
    int TotalNberOfElementsSorted = LinearIndexSorted.size(); 
    ATH_MSG_INFO( " SortElements: "
        << " TotalNberOfElementsTOBESORTED       " << std::setw(8) << TotalNberOfElementsTOBESORTED
        << " TotalNberOfElementsSorted " << std::setw(8) << TotalNberOfElementsSorted
        );
    for (int iSorted=0; iSorted<TotalNberOfElementsSorted; iSorted++){
      int LinearId = LinearIndexSorted[iSorted] ;
      
      double Xdca = PointXdca[LinearId] ;
      double Ydca = PointYdca[LinearId] ;
      double Zdca = PointZdca[LinearId] ;
      double DistanceToIP = sqrt ( 
                                    Xdca*Xdca 
                                  + Ydca*Ydca
                                  + Zdca*Zdca
                                  );
      ATH_MSG_INFO( std::setw(5)<< iSorted  
         << std::setw(12)<<std::setprecision(4)<< DistanceToIP  
         << std::setw(12)<<std::setprecision(4)<< Xdca  
         << std::setw(12)<<std::setprecision(4)<< Ydca 
         << std::setw(12)<<std::setprecision(4)<< Zdca 
         );
    }
  }
  
}

}
