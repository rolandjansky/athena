/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------//
//                                                                             //
//  TRTDetectorFactory_Full                                                    //
//                                                                             //
//  Authors:  Joe Boudreau, Andrei and Iouris Zalite, Thomas Kittelmann        //
//                                                                             //
//  This class creates the TRT Geometry. Cf. the README file                   //
//                                                                             //
//-----------------------------------------------------------------------------//

#ifndef TRTDetectorFactory_Full_h
#define TRTDetectorFactory_Full_h 1
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h" //for Argon
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include <string>

class GeoPhysVol;
class GeoFullPhysVol;
class TRTParameterInterface;
class InDetMaterialManager;

class TRTDetectorFactory_Full : public InDetDD::DetectorFactoryBase  {

 public:
  
  //--------------------------Public Interface:--------------------------------//
  //                                                                           //
  // Constructor:                                                              //
  TRTDetectorFactory_Full(const InDetDD::AthenaComps * athenaComps,            //
			  ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc,  //
			  bool useOldActiveGasMixture,                         //
			  bool DC2CompatibleBarrelCoordinates,                 //
			  int overridedigversion,                              //
			  bool alignable,                                      //
			  bool doArgon,                                        //
			  bool doKrypton,                                      //
			  bool useDynamicAlignmentFolders);                    //
  //                                                                           //
  // Destructor:                                                               //
  ~TRTDetectorFactory_Full();                                                  //
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


 private:  

  //---------------------------Illegal operations:---------------------------------//
  //                                                                               //
  const TRTDetectorFactory_Full & operator=(const TRTDetectorFactory_Full &right); //
  TRTDetectorFactory_Full(const TRTDetectorFactory_Full &right);                   //
  //                                                                               //
  //-------------------------------------------------------------------------------//

  // Gas mixture enumerator
  enum ActiveGasMixture
    {
    GM_XENON,
    GM_KRYPTON,
    GM_ARGON
    };

  ActiveGasMixture DecideGasMixture(int strawStatusHT);

  // private helper methods:
  const GeoShape * makeModule ( double length, GeoTrf::Vector2D corner1 ,  GeoTrf::Vector2D corner2, GeoTrf::Vector2D corner3,
				GeoTrf::Vector2D corner4, GeoTrf::Transform3D & absolutePosition, double shrinkDist=0 ) const;
  //GeoPhysVol * makeStraw( double& activeGasZPosition, bool hasLargeDeadRegion=false ) const;
  GeoPhysVol * makeStraw( double& activeGasZPosition, bool hasLargeDeadRegion=false, ActiveGasMixture gasMixture = GM_XENON) const;
  //GeoFullPhysVol  *makeStrawPlane( size_t w ) const;
  GeoFullPhysVol  *makeStrawPlane( size_t w , ActiveGasMixture gasMixture = GM_XENON) const;

  // private member data:
  InDetDD::TRT_DetectorManager * m_detectorManager;
  InDetMaterialManager         * m_materialManager;
  TRTParameterInterface        * m_data;

  bool m_useOldActiveGasMixture;
  bool m_DC2CompatibleBarrelCoordinates;
  int m_overridedigversion;
  bool m_alignable;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc; // added for Argon
  bool m_strawsvcavailable;
  bool m_doArgon;
  bool m_doKrypton;
  bool m_useDynamicAlignFolders;
};

#endif // TRTDetectorFactory_Full_h
