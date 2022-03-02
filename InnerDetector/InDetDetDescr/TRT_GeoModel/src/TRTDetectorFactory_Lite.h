/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------//
//                                                                             //
//  TRTDetectorFactory_Lite                                                    //
//                                                                             //
//  Authors:  Joe Boudreau, Andrei and Iouris Zalite, Thomas Kittelmann        //
//                                                                             //
//  This class creates the TRT Geometry. Cf. the README file                   //
//                                                                             //
//-----------------------------------------------------------------------------//

#ifndef TRTDetectorFactory_Lite_h
#define TRTDetectorFactory_Lite_h 1
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h" //for Argon
#include "GeoModelKernel/GeoDefinitions.h"
#include "CxxUtils/checker_macros.h"

#include <string>

class GeoPhysVol;
class GeoFullPhysVol;
class TRTParameterInterface;

namespace GeoModelIO {
  class ReadGeoModel;
}

class TRTDetectorFactory_Lite : public InDetDD::DetectorFactoryBase  {

 public:
  
  //--------------------------Public Interface:--------------------------------//
  //                                                                           //
  // Constructor:                                                              //
  TRTDetectorFactory_Lite(GeoModelIO::ReadGeoModel *sqliteReader,              //
			  InDetDD::AthenaComps * athenaComps,                  //
			  const ITRT_StrawStatusSummaryTool * sumTool,         //
			  bool useOldActiveGasMixture,                         //
			  bool DC2CompatibleBarrelCoordinates,                 //
			  int overridedigversion,                              //
			  bool alignable,                                      //
			  bool useDynamicAlignmentFolders);                    //
  //                                                                           //
  // Destructor:                                                               //
  ~TRTDetectorFactory_Lite();                                                  //
  //                                                                           //
  // Creation of geometry:                                                     //
  virtual void create(GeoPhysVol *world);                                      //
  //                                                                           //
  // Access to the results:                                                    //
  virtual const InDetDD::TRT_DetectorManager * getDetectorManager() const;     //
  //                                                                           //
  //---------------------------------------------------------------------------//

  const std::string& name() const { 
    static const std::string n("TRT_GeoModel::TRTDetectorFactory"); 
    return n;
  }

 typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer;
 private:  

  //---------------------------Illegal operations:---------------------------------//
  //                                                                               //
  const TRTDetectorFactory_Lite & operator=(const TRTDetectorFactory_Lite &right); //
  TRTDetectorFactory_Lite(const TRTDetectorFactory_Lite &right);                   //
  //                                                                               //
  //-------------------------------------------------------------------------------//


  double activeGasZPosition(bool hasLargeDeadRegion=false) const;

  void setEndcapTransformField(size_t w);

  // private member data:
  GeoModelIO::ReadGeoModel                      *m_sqliteReader;
  InDetDD::TRT_DetectorManager                  *m_detectorManager = nullptr; // ownership handed to calleer.
  std::unique_ptr<TRTParameterInterface>        m_data;

  bool m_useOldActiveGasMixture;
  bool m_DC2CompatibleBarrelCoordinates;
  int m_overridedigversion;
  bool m_alignable;
  const ITRT_StrawStatusSummaryTool* m_sumTool; // added for Argon
  bool m_useDynamicAlignFolders;

};

#endif // TRTDetectorFactory_Lite_h
