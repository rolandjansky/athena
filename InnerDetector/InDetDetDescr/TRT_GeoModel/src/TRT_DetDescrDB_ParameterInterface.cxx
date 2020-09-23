/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DetDescrDB_ParameterInterface.h"
#include "GeoModelKernel/Units.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "InDetGeoModelUtils/DistortedMaterialManager.h"
#include "InDetGeoModelUtils/TopLevelPlacements.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

//_________________________________________________________________________________________
TRT_DetDescrDB_ParameterInterface::TRT_DetDescrDB_ParameterInterface(InDetDD::AthenaComps * athenaComps) :
  TRTParameterInterface(),  m_athenaComps(athenaComps), m_distortedMatManager(0), m_placements(0)
{ SetValues(); }

//_________________________________________________________________________________________
TRT_DetDescrDB_ParameterInterface::~TRT_DetDescrDB_ParameterInterface() {

  delete[] barrelNumberOfStrawLayersInModule;
  delete[] barrelXOfFirstGlobalAlignmentStraw;
  delete[] barrelYOfFirstGlobalAlignmentStraw;
  delete[] barrelXOfSecondGlobalAlignmentStraw;
  delete[] barrelYOfSecondGlobalAlignmentStraw;
  delete[] barrelNumberOfStrawsInModule;
  delete[] barrelIndexOfSecondGlobalAlignmentStraw;

  for (unsigned int iRing = 0; iRing < nBarrelRings; iRing++) {
    delete[] barrelNumberOfStrawsInStrawLayer[iRing];
    delete[] barrelXOfCoolingTube[iRing];
    delete[] barrelYOfCoolingTube[iRing];
    delete[] shellCornerXPosition[iRing];
    delete[] shellCornerYPosition[iRing];
    
    // The values get copied to ArrayFunction objects so there are safe to delete.
    delete[] strawXPosition[iRing];
    delete[] strawYPosition[iRing];
  };

  delete[] endCapLayerZPositionA;
  delete[] endCapLayerZPositionB;
  delete[] endCapLayerZPositionC;


  delete m_distortedMatManager;
  delete m_placements;
  

}

//_________________________________________________________________________________________
void TRT_DetDescrDB_ParameterInterface::SetValues ATLAS_NOT_THREAD_SAFE () { // Thread unsafe AthenaComps::rdbAccessSvc const method is used.

  /////////////////////////////////////////////////////////////////////////////////////////
  //                                 Initialize Services                                 //
  /////////////////////////////////////////////////////////////////////////////////////////

  msg(MSG::DEBUG) << "Starting TRT_DetDescrDB_ParameterInterface" <<endmsg;

  // RDBAccessSvc (Interface to the DD database).
  IRDBAccessSvc* iAccessSvc = m_athenaComps->rdbAccessSvc();
 
  // Get tag key and corresponding node 
  DecodeVersionKey versionKey(m_athenaComps->geoDbTagSvc(),"TRT");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(m_athenaComps->geoDbTagSvc(),"InnerDetector");


  /////////////////////////////////////////////////////////////////////////////////////////
  //                                  Version Information                                //
  /////////////////////////////////////////////////////////////////////////////////////////
  versionTag = iAccessSvc->getChildTag("TRT", versionKey.tag(), versionKey.node());

  /////////////////////////////////////////////////////////////////////////////////////////
  //                                    Special Flags                                    //
  /////////////////////////////////////////////////////////////////////////////////////////

  IRDBRecordset_ptr switchSet = iAccessSvc->getRecordsetPtr("TRTSwitches", detectorKey, detectorNode);
  const IRDBRecord    *switches  = (*switchSet)[0];

  oldConfiguration = true;
  if (!switches->isFieldNull("VERSIONNAME")) {
    oldConfiguration = false;
    versionName = switches->getString("VERSIONNAME");
    layout      = switches->getString("LAYOUT");
    versionDescription =  switches->getString("DESCRIPTION");
  }

  // Flags isCosmicRun and initialLayout don't have any effect if TRTTopLevel table is present.
  // The latter controls which parts are present.
  isCosmicRun = false;
  if (!switches->isFieldNull("COSMICLAYOUT")) {
    isCosmicRun = switches->getInt("COSMICLAYOUT");
  }
  //NB: DONT DO: bool useoldactivegasmixture ( switches->getInt("GASVERSION") == 0 );
  //Since user might have overriden it from joboptions!!

  initialLayout = switches->getInt("INITIALLAYOUT");

  IRDBRecordset_ptr RDB_DigVersion = iAccessSvc->getRecordsetPtr("TRTDigVersion", detectorKey, detectorNode);
  const IRDBRecord    *RDBVars_DigVersion  = (*RDB_DigVersion)[0];
  digversion = RDBVars_DigVersion->getInt("DIGVERSION");
  digversionname = RDBVars_DigVersion->getString("DIGVERSIONNAME");

  /////////////////////////////////////////////////////////////////////////////////////////
  //                                   Top Level Placements                              //
  /////////////////////////////////////////////////////////////////////////////////////////
  if (!oldConfiguration) {
    IRDBRecordset_ptr topLevelTable = iAccessSvc->getRecordsetPtr("TRTTopLevel", detectorKey, detectorNode);
    m_placements = new TopLevelPlacements(topLevelTable);
  }
  else
    m_placements = new TopLevelPlacements(IRDBRecordset_ptr());

  /////////////////////////////////////////////////////////////////////////////////////////
  //                                   Extra Scaled Material                             //
  /////////////////////////////////////////////////////////////////////////////////////////

  // NB. This is at Inner Detector node level
  m_scalingTable = iAccessSvc->getRecordsetPtr("TrtMatScaling",  indetVersionKey.tag(), indetVersionKey.node());

  
  /////////////////////////////////////////////////////////////////////////////////////////
  //                                   ExtraMaterial                                     //
  /////////////////////////////////////////////////////////////////////////////////////////
  m_distortedMatManager = new InDetDD::DistortedMaterialManager;


  /////////////////////////////////////////////////////////////////////////////////////////
  //                     Variables common between Barrel and End Cap                     //
  /////////////////////////////////////////////////////////////////////////////////////////

  IRDBRecordset_ptr RDB_CommonPars  = iAccessSvc->getRecordsetPtr("TRTCommonPars",detectorKey,detectorNode);
  const IRDBRecord* RDBVars_CommonPars = (*RDB_CommonPars)[0];

  lengthOfDeadRegion = RDBVars_CommonPars->getFloat("LENGTHOFDEADREGION")*GeoModelKernelUnits::mm;
  outerRadiusOfWire  = RDBVars_CommonPars->getFloat("OUTERRADIUSOFWIRE")*GeoModelKernelUnits::mm;
  innerRadiusOfStraw = RDBVars_CommonPars->getFloat("INNERRADIUSOFSTRAW")*GeoModelKernelUnits::mm;
  outerRadiusOfStraw = RDBVars_CommonPars->getFloat("OUTERRADIUSOFSTRAW")*GeoModelKernelUnits::mm;

  /////////////////////////////////////////////////////////////////////////////////////////
  //                        Variables related to the Barrel part                         //
  /////////////////////////////////////////////////////////////////////////////////////////

  IRDBRecordset_ptr RDB_BarrelOverallPars  = iAccessSvc->getRecordsetPtr("TRTBarrelOverallPars",detectorKey,detectorNode);
  const IRDBRecord* RDBVars_BarrelOverallPars = (*RDB_BarrelOverallPars)[0];
  IRDBRecordset_ptr RDB_BarrelStrawCoord  = iAccessSvc->getRecordsetPtr("TRTBarrelStrawCoord",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_BarrelRingDepPars = iAccessSvc->getRecordsetPtr("TRTBarrelRingDepPars",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_BarrelNStrawInLay = iAccessSvc->getRecordsetPtr("TRTBarrelNStrawInLay",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_BarrelServices = iAccessSvc->getRecordsetPtr("TRTBarrelServices",detectorKey,detectorNode);

  includeBarServiceAndFlange = (RDB_BarrelServices->size()!=0);

  if (includeBarServiceAndFlange) {

    const IRDBRecord* RDBVars_BarrelServices = (*RDB_BarrelServices)[0];
    barFlangeZMin = RDBVars_BarrelServices->getDouble("BARFLANGEZMIN")*GeoModelKernelUnits::mm;
    barFlangeZMax = RDBVars_BarrelServices->getDouble("BARFLANGEZMAX")*GeoModelKernelUnits::mm;
    barFlangeRMin = RDBVars_BarrelServices->getDouble("BARFLANGERMIN")*GeoModelKernelUnits::mm;
    barFlangeRMax = RDBVars_BarrelServices->getDouble("BARFLANGERMAX")*GeoModelKernelUnits::mm;

    barServicesZMin = RDBVars_BarrelServices->getDouble("BARSERVICESZMIN")*GeoModelKernelUnits::mm;
    barServicesZMax = RDBVars_BarrelServices->getDouble("BARSERVICESZMAX")*GeoModelKernelUnits::mm;
    barServicesRMin = RDBVars_BarrelServices->getDouble("BARSERVICESRMIN")*GeoModelKernelUnits::mm;
    barServicesRMax = RDBVars_BarrelServices->getDouble("BARSERVICESRMAX")*GeoModelKernelUnits::mm;

  } else if (!includeBarServiceAndFlange) {

    barFlangeZMin = -1.;
    barFlangeZMax = -1.;
    barFlangeRMin = -1.;
    barFlangeRMax = -1.;

    barServicesZMin = -1.;
    barServicesZMax = -1.;
    barServicesRMin = -1.;
    barServicesRMax = -1.;
  }

  if (!RDBVars_BarrelOverallPars->isFieldNull("BRLVIRTVOLZCLEARANCE")) {
    barrelVirtualVolumeZClearance    =  RDBVars_BarrelOverallPars->getFloat("BRLVIRTVOLZCLEARANCE")*GeoModelKernelUnits::mm;
    barrelVirtualVolumeRMinClearance =  RDBVars_BarrelOverallPars->getFloat("BRLVIRTVOLRMINCLEAR")*GeoModelKernelUnits::mm;
    barrelVirtualVolumeRMaxClearance =  RDBVars_BarrelOverallPars->getFloat("BRLVIRTVOLRMAXCLEAR")*GeoModelKernelUnits::mm;
  } else {
    barrelVirtualVolumeZClearance    = 0.001*GeoModelKernelUnits::micrometer;//fixme: use epsilon(?).
    barrelVirtualVolumeRMinClearance = 0.001*GeoModelKernelUnits::micrometer;
    barrelVirtualVolumeRMaxClearance = 0.001*GeoModelKernelUnits::micrometer;
  }

  innerRadiusOfBarrelVolume = RDBVars_BarrelOverallPars->getFloat("INNRADIUSOFBARRELVOL")*GeoModelKernelUnits::mm;
  outerRadiusOfBarrelVolume = RDBVars_BarrelOverallPars->getFloat("OUTRADIUSOFBARRELVOL")*GeoModelKernelUnits::mm;
  lengthOfBarrelVolume = RDBVars_BarrelOverallPars->getFloat("LENGTHOFBARRELVOLUME")*GeoModelKernelUnits::mm;
  barrelOuterRadiusOfCoolingTube = RDBVars_BarrelOverallPars->getFloat("OUTRADIUSOFCOOLTUBE")*GeoModelKernelUnits::mm;
  barrelInnerRadiusOfCoolingTube = RDBVars_BarrelOverallPars->getFloat("INNRADIUSOFCOOLTUBE")*GeoModelKernelUnits::mm;
  barrelThicknessOfModuleWalls = RDBVars_BarrelOverallPars->getFloat("THICKOFMODULEWALLS")*GeoModelKernelUnits::mm;
  barrelNumberOfLayersWithLargeDeadRegion = RDBVars_BarrelOverallPars->getInt("NLAYWTHLRGDEADREGION");
  thicknessOfBarrelInnerSupport = RDBVars_BarrelOverallPars->getFloat("THICKOFBRLINNSUPPORT")*GeoModelKernelUnits::mm;
  thicknessOfBarrelOuterSupport = RDBVars_BarrelOverallPars->getFloat("THICKOFBRLOUTSUPPORT")*GeoModelKernelUnits::mm;
  barrelOuterRadiusOfStrawHole = RDBVars_BarrelOverallPars->getFloat("OUTRADIUSOFSTRAWHOLE")*GeoModelKernelUnits::mm;

  virtualBarrelVolumeLength = std::max(std::max(barFlangeZMax,barServicesZMax),lengthOfBarrelVolume/2)
    + barrelVirtualVolumeZClearance;
  virtualBarrelOuterRadius  = std::max(std::max(barFlangeRMax,barServicesRMax),outerRadiusOfBarrelVolume)
    + barrelVirtualVolumeRMaxClearance;

  virtualBarrelInnerRadius  = innerRadiusOfBarrelVolume;
  if (barServicesRMin>0.0 &&virtualBarrelInnerRadius>barServicesRMin) virtualBarrelInnerRadius = barServicesRMin;
  if (barFlangeRMin>0.0 && virtualBarrelInnerRadius>barFlangeRMin) virtualBarrelInnerRadius = barFlangeRMin;
  virtualBarrelInnerRadius -= barrelVirtualVolumeRMinClearance;

  nBarrelModules = RDBVars_BarrelOverallPars->getInt("NBARRELMODULES");
  nBarrelModulesUsed = nBarrelModules; //Hardwired to be all modules. Silly variable anyway.
  nBarrelPhi = RDBVars_BarrelOverallPars->getInt("NBARRELPHI");

  barrelLengthOfStraw  = RDBVars_BarrelOverallPars->getFloat("LENGTHOFSTRAW")*GeoModelKernelUnits::mm;
  barrelLengthOfTwister         = RDBVars_BarrelOverallPars->getFloat("LENGTHOFTWISTER")*GeoModelKernelUnits::mm;
  barrelLengthOfLargeDeadRegion = RDBVars_BarrelOverallPars->getFloat("LENLARGEDEADREGION")*GeoModelKernelUnits::mm;

  barrelNumberOfStrawLayersInModule = new unsigned int[nBarrelRings];
  for (unsigned int i=0; i<nBarrelRings; i++) {
    const IRDBRecord* ringVars = (*RDB_BarrelRingDepPars)[i];
    barrelNumberOfStrawLayersInModule[i] = ringVars->getInt("NSTRAWLAYERSINMODULE");
  };


  for (unsigned int i = 0; i < nBarrelRings; i++)
    barrelNumberOfStrawsInStrawLayer[i] = new unsigned int[ barrelNumberOfStrawLayersInModule[i] ];

  unsigned int cumulLayer = 0;
  for (unsigned int i=0; i<nBarrelRings; i++)
    for (unsigned int j=0; j<barrelNumberOfStrawLayersInModule[i]; j++) {
      const IRDBRecord* layerVars = (*RDB_BarrelNStrawInLay)[cumulLayer++];
      barrelNumberOfStrawsInStrawLayer[i][j] = layerVars->getInt("NSTRAWSINSTRAWLAYER");
    }

  barrelXOfFirstGlobalAlignmentStraw  = new double[nBarrelRings];
  barrelYOfFirstGlobalAlignmentStraw  = new double[nBarrelRings];
  barrelXOfSecondGlobalAlignmentStraw = new double[nBarrelRings];
  barrelYOfSecondGlobalAlignmentStraw = new double[nBarrelRings];
  barrelNumberOfStrawsInModule            = new unsigned int[nBarrelRings];
  barrelIndexOfSecondGlobalAlignmentStraw = new unsigned int[nBarrelRings];

  nCoolingTubes = RDBVars_BarrelOverallPars->getInt("NCOOLINGTUBES");
  if ( nCoolingTubes != 2 ) {
    msg(MSG::WARNING) << "DD Database indicates nCoolingTubes != 2, but the code" << " can't cope with that. Putting nCoolingTubes=2" << endmsg;
    nCoolingTubes = 2;
  };

  nShellCorners = RDBVars_BarrelOverallPars->getInt("NSHELLCORNERS");
  if ( nShellCorners != 4 ) {
    msg(MSG::WARNING) << "DD Database indicates nShellCorners != 4, but the code" << " can't cope with that. Putting nShellCorners=4" << endmsg;
    nShellCorners = 4;
  };

  for (unsigned int iRing = 0; iRing < nBarrelRings; iRing++) {
    const IRDBRecord* row = (*RDB_BarrelRingDepPars)[iRing];
    barrelXOfFirstGlobalAlignmentStraw[iRing]  = row->getFloat("XOFGLOBALALIGNSTRW1")*GeoModelKernelUnits::mm;
    barrelYOfFirstGlobalAlignmentStraw[iRing]  = row->getFloat("YOFGLOBALALIGNSTRW1")*GeoModelKernelUnits::mm;
    barrelXOfSecondGlobalAlignmentStraw[iRing] = row->getFloat("XOFGLOBALALIGNSTRW2")*GeoModelKernelUnits::mm;
    barrelYOfSecondGlobalAlignmentStraw[iRing] = row->getFloat("YOFGLOBALALIGNSTRW2")*GeoModelKernelUnits::mm;
    barrelIndexOfSecondGlobalAlignmentStraw[iRing] = row->getInt("INDEX2NDGLBALGNSTRW");
    barrelNumberOfStrawsInModule[iRing] = row->getInt("NSTRAWSINMODULE");

    barrelXOfCoolingTube[iRing] = new double[nCoolingTubes];
    barrelXOfCoolingTube[iRing][0] = row->getFloat("XOFCOOLINGTUBE1")*GeoModelKernelUnits::mm;
    barrelXOfCoolingTube[iRing][1] = row->getFloat("XOFCOOLINGTUBE2")*GeoModelKernelUnits::mm;

    barrelYOfCoolingTube[iRing] = new double[nCoolingTubes];
    barrelYOfCoolingTube[iRing][0] = row->getFloat("YOFCOOLINGTUBE1")*GeoModelKernelUnits::mm;
    barrelYOfCoolingTube[iRing][1] = row->getFloat("YOFCOOLINGTUBE2")*GeoModelKernelUnits::mm;

    shellCornerXPosition[iRing] = new double[nShellCorners];
    shellCornerXPosition[iRing][0] = row->getFloat("SHELLCORNERXPOS1")*GeoModelKernelUnits::mm;
    shellCornerXPosition[iRing][1] = row->getFloat("SHELLCORNERXPOS2")*GeoModelKernelUnits::mm;
    shellCornerXPosition[iRing][2] = row->getFloat("SHELLCORNERXPOS3")*GeoModelKernelUnits::mm;
    shellCornerXPosition[iRing][3] = row->getFloat("SHELLCORNERXPOS4")*GeoModelKernelUnits::mm;

    shellCornerYPosition[iRing] = new double[nShellCorners];
    shellCornerYPosition[iRing][0] = row->getFloat("SHELLCORNERYPOS1")*GeoModelKernelUnits::mm;
    shellCornerYPosition[iRing][1] = row->getFloat("SHELLCORNERYPOS2")*GeoModelKernelUnits::mm;
    shellCornerYPosition[iRing][2] = row->getFloat("SHELLCORNERYPOS3")*GeoModelKernelUnits::mm;
    shellCornerYPosition[iRing][3] = row->getFloat("SHELLCORNERYPOS4")*GeoModelKernelUnits::mm;

  };

  unsigned int cumulativeStrawNumber = 0;
  for (unsigned int i = 0; i < nBarrelRings; i++) {
    strawXPosition[i] = new double[ barrelNumberOfStrawsInModule[i] ];
    strawYPosition[i] = new double[ barrelNumberOfStrawsInModule[i] ];
    for (unsigned int j = 0; j < barrelNumberOfStrawsInModule[i]; j++) {
      const IRDBRecord* row = (*RDB_BarrelStrawCoord)[cumulativeStrawNumber++];
      strawXPosition[i][j]=row->getFloat("STRAWXPOSITION")*GeoModelKernelUnits::mm;
      strawYPosition[i][j]=row->getFloat("STRAWYPOSITION")*GeoModelKernelUnits::mm;
    };
  };


  /////////////////////////////////////////////////////////////////////////////////////////
  //                        Variables related to the End Cap part                        //
  /////////////////////////////////////////////////////////////////////////////////////////

  IRDBRecordset_ptr RDB_EndCapOverallPars = iAccessSvc->getRecordsetPtr("TRTEndCapOverallPars",detectorKey,detectorNode);
  const IRDBRecord* RDBVars_EndCapOverallPars = (*RDB_EndCapOverallPars)[0];
  IRDBRecordset_ptr RDB_ECWheelTypDepPars = iAccessSvc->getRecordsetPtr("TRTECWheelTypDepPars",detectorKey,detectorNode);
  const IRDBRecord* RDBVars_A_Wheel = (*RDB_ECWheelTypDepPars)[0];
  const IRDBRecord* RDBVars_B_Wheel = (*RDB_ECWheelTypDepPars)[1];
  const IRDBRecord* RDBVars_C_Wheel = (*RDB_ECWheelTypDepPars)[2];
  IRDBRecordset_ptr RDB_EndCapStrwLayZPos = iAccessSvc->getRecordsetPtr("TRTEndCapStrwLayZPos",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_ECMembranePars = iAccessSvc->getRecordsetPtr("TRTECMembranePars",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_ECHeatExchAndFoil = iAccessSvc->getRecordsetPtr("TRTECHeatExchAndFoil",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_ECWheelDepPars = iAccessSvc->getRecordsetPtr("TRTECWheelDepPars",detectorKey,detectorNode);
  IRDBRecordset_ptr RDB_ECEndCapDepPars = iAccessSvc->getRecordsetPtr("TRTECEndCapDepPars",detectorKey,detectorNode);

  innerRadiusOfEndCapVolumeAB = RDBVars_EndCapOverallPars->getFloat("INNERRADIUSOFECVOLAB")*GeoModelKernelUnits::mm;
  outerRadiusOfEndCapVolumeAB = RDBVars_EndCapOverallPars->getFloat("OUTERRADIUSOFECVOLAB")*GeoModelKernelUnits::mm;
  innerRadiusOfEndCapVolumeC  = RDBVars_EndCapOverallPars->getFloat("INNERRADIUSOFECVOLC")*GeoModelKernelUnits::mm;
  outerRadiusOfEndCapVolumeC  = RDBVars_EndCapOverallPars->getFloat("OUTERRADIUSOFECVOLC")*GeoModelKernelUnits::mm;
  lengthOfEndCapVolumeAB    = RDBVars_EndCapOverallPars->getFloat("LENGTHOFENDCAPVOLAB")*GeoModelKernelUnits::mm; 
  positionOfEndCapVolumeAB = RDBVars_EndCapOverallPars->getFloat("POSFORENDCAPVOLAB")*GeoModelKernelUnits::mm;
  lengthOfEndCapVolumeC    = RDBVars_EndCapOverallPars->getFloat("LENGTHOFENDCAPVOLC")*GeoModelKernelUnits::mm;
  positionOfEndCapVolumeC  = RDBVars_EndCapOverallPars->getFloat("POSITIONOFENDCAPVOLC")*GeoModelKernelUnits::mm;//rename!!!

  if ( RDBVars_A_Wheel->getFloat("OUTERRADIUSOFINNERSUP") != RDBVars_B_Wheel->getFloat("OUTERRADIUSOFINNERSUP") )
    msg(MSG::WARNING) << "DD Database indicates different outer radii of inner support for A and B wheels." << " The code assumes otherwise. Using value of A wheel for both!" << endmsg;
  if ( RDBVars_A_Wheel->getFloat("INNERRADIUSOFOUTERSUP") != RDBVars_B_Wheel->getFloat("INNERRADIUSOFOUTERSUP") )
    msg(MSG::WARNING) << "DD Database indicates different inner radii of outer support for A and B wheels." << " The code assumes otherwise. Using value of A wheel for both!" << endmsg;
  endcapOuterRadiusOfInnerSupport_wheelAB = RDBVars_A_Wheel->getFloat("OUTERRADIUSOFINNERSUP")*GeoModelKernelUnits::mm;
  endcapInnerRadiusOfOuterSupport_wheelAB = RDBVars_A_Wheel->getFloat("INNERRADIUSOFOUTERSUP")*GeoModelKernelUnits::mm;
  endcapOuterRadiusOfInnerSupport_wheelC  = RDBVars_C_Wheel->getFloat("OUTERRADIUSOFINNERSUP")*GeoModelKernelUnits::mm;
  endcapInnerRadiusOfOuterSupport_wheelC  = RDBVars_C_Wheel->getFloat("INNERRADIUSOFOUTERSUP")*GeoModelKernelUnits::mm;

  endCapShiftForEachRotation = RDBVars_EndCapOverallPars->getFloat("SHIFTFOREACHROTATION")*GeoModelKernelUnits::radian;

  endCapPhiOfFirstStraw = 0; // Temporary

 
  endCapInnerRadiusOfSupportA = RDBVars_A_Wheel->getFloat("INNERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;
  endCapInnerRadiusOfSupportB = RDBVars_B_Wheel->getFloat("INNERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;
  endCapInnerRadiusOfSupportC = RDBVars_C_Wheel->getFloat("INNERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;

  endCapOuterRadiusOfSupportA = RDBVars_A_Wheel->getFloat("OUTERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;
  endCapOuterRadiusOfSupportB = RDBVars_B_Wheel->getFloat("OUTERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;
  endCapOuterRadiusOfSupportC = RDBVars_C_Wheel->getFloat("OUTERRADIUSOFSUPPORT")*GeoModelKernelUnits::mm;

  endCapRadialThicknessOfInnerSupportA = RDBVars_A_Wheel->getFloat("RADIALTHICKOFINNERSUP")*GeoModelKernelUnits::mm;
  endCapRadialThicknessOfInnerSupportB = RDBVars_B_Wheel->getFloat("RADIALTHICKOFINNERSUP")*GeoModelKernelUnits::mm;
  endCapRadialThicknessOfInnerSupportC = RDBVars_C_Wheel->getFloat("RADIALTHICKOFINNERSUP")*GeoModelKernelUnits::mm;

  endCapRadialThicknessOfOuterSupportA = RDBVars_A_Wheel->getFloat("RADIALTHICKOFOUTERSUP")*GeoModelKernelUnits::mm;
  endCapRadialThicknessOfOuterSupportB = RDBVars_B_Wheel->getFloat("RADIALTHICKOFOUTERSUP")*GeoModelKernelUnits::mm;
  endCapRadialThicknessOfOuterSupportC = RDBVars_C_Wheel->getFloat("RADIALTHICKOFOUTERSUP")*GeoModelKernelUnits::mm;

  endCapRadialDistFromRadToOuterSupportA = RDBVars_A_Wheel->getFloat("RADIALDISTRADTOOUTERSUP")*GeoModelKernelUnits::mm;
  endCapRadialDistFromRadToOuterSupportB = RDBVars_B_Wheel->getFloat("RADIALDISTRADTOOUTERSUP")*GeoModelKernelUnits::mm;
  endCapRadialDistFromRadToOuterSupportC = RDBVars_C_Wheel->getFloat("RADIALDISTRADTOOUTERSUP")*GeoModelKernelUnits::mm;

  endCapMainRadiatorThicknessA = RDBVars_A_Wheel->getFloat("MAINRADIATORTHICK")*GeoModelKernelUnits::mm;
  endCapMainRadiatorThicknessB = RDBVars_B_Wheel->getFloat("MAINRADIATORTHICK")*GeoModelKernelUnits::mm;
  endCapMainRadiatorThicknessC = RDBVars_C_Wheel->getFloat("MAINRADIATORTHICK")*GeoModelKernelUnits::mm;

  endCapThinRadiatorThicknessA = RDBVars_A_Wheel->getFloat("THINRADIATORTHICK")*GeoModelKernelUnits::mm;
  endCapThinRadiatorThicknessB = RDBVars_B_Wheel->getFloat("THINRADIATORTHICK")*GeoModelKernelUnits::mm;
  endCapThinRadiatorThicknessC = RDBVars_C_Wheel->getFloat("THINRADIATORTHICK")*GeoModelKernelUnits::mm;

  endCapMiddleRadiatorThicknessB = RDBVars_B_Wheel->getFloat("MIDDLERADIATORTHICK")*GeoModelKernelUnits::mm;
  if ( RDBVars_A_Wheel->getFloat("MIDDLERADIATORTHICK")*GeoModelKernelUnits::mm != 0 )
    msg(MSG::WARNING) << "DD Database indicates a nonzero middle radiator thickness in the A wheel" << " where middle radiators doesn't exist. Ignoring parameter." << endmsg;
  if ( RDBVars_C_Wheel->getFloat("MIDDLERADIATORTHICK")*GeoModelKernelUnits::mm != 0 )
    msg(MSG::WARNING) << "DD Database indicates a nonzero middle radiator thickness in the C wheel" << " where middle radiators doesn't exist. Ignoring parameter." << endmsg;

  endCapLengthOfWheelsA = RDBVars_A_Wheel->getFloat("LENGTHOFWHEELS")*GeoModelKernelUnits::mm; 
  endCapLengthOfWheelsB = RDBVars_B_Wheel->getFloat("LENGTHOFWHEELS")*GeoModelKernelUnits::mm; 

  endCapLengthOfWheelsC = RDBVars_C_Wheel->getFloat("LENGTHOFWHEELS")*GeoModelKernelUnits::mm;

  endCapNumberOfStrawLayersPerWheelA = RDBVars_A_Wheel->getInt("NSTRAWLAYERSPERWHEEL");
  endCapNumberOfStrawLayersPerWheelB = RDBVars_B_Wheel->getInt("NSTRAWLAYERSPERWHEEL");
  endCapNumberOfStrawLayersPerWheelC = RDBVars_C_Wheel->getInt("NSTRAWLAYERSPERWHEEL");


  endcapNumberOfAWheels = RDBVars_A_Wheel->getInt("NWHEELSOFTYPE");
  endcapNumberOfBWheels = RDBVars_B_Wheel->getInt("NWHEELSOFTYPE");
  endcapNumberOfCWheels = RDBVars_C_Wheel->getInt("NWHEELSOFTYPE");



  endcapNumberOfStrawsInStrawLayer_AWheels = RDBVars_A_Wheel->getInt("NSTRAWSINSTRAWLAYER");
  endcapNumberOfStrawsInStrawLayer_BWheels = RDBVars_B_Wheel->getInt("NSTRAWSINSTRAWLAYER");
  endcapNumberOfStrawsInStrawLayer_CWheels = RDBVars_C_Wheel->getInt("NSTRAWSINSTRAWLAYER");

  includeECFoilHeatExchangerAndMembranes = (RDB_ECHeatExchAndFoil->size()!=0);
  
  if ( includeECFoilHeatExchangerAndMembranes ) {
    
    const IRDBRecord* RDBECEndCapDepPars_EndCap_C = (*RDB_ECEndCapDepPars)[0];
    const IRDBRecord* RDBECEndCapDepPars_EndCap_A = (*RDB_ECEndCapDepPars)[1];
  

    
    endCapPositionOfFirstWheelA[0] =  RDBECEndCapDepPars_EndCap_A->getDouble("ZPOSOFFIRSTAWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelA[1] =  RDBECEndCapDepPars_EndCap_C->getDouble("ZPOSOFFIRSTAWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelB[0] =  RDBECEndCapDepPars_EndCap_A->getDouble("ZPOSOFFIRSTBWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelB[1] =  RDBECEndCapDepPars_EndCap_C->getDouble("ZPOSOFFIRSTBWHEEL")*GeoModelKernelUnits::mm;

    for (unsigned int iiWheel = 0; iiWheel < endcapNumberOfAWheels; iiWheel++) {
      const IRDBRecord* row = (*RDB_ECWheelDepPars)[iiWheel];
      endCapDistanceBetweenWheelCentersA[0][iiWheel]=row->getDouble("DISTBETWWHEELCENTECC")*GeoModelKernelUnits::mm;
      endCapDistanceBetweenWheelCentersA[1][iiWheel]=row->getDouble("DISTBETWWHEELCENTECA")*GeoModelKernelUnits::mm;
    };
    for (unsigned int iiWheel = endcapNumberOfAWheels; iiWheel < (endcapNumberOfAWheels+endcapNumberOfBWheels); iiWheel++) {
      const IRDBRecord* row = (*RDB_ECWheelDepPars)[iiWheel];
      endCapDistanceBetweenWheelCentersB[0][iiWheel]=row->getDouble("DISTBETWWHEELCENTECC")*GeoModelKernelUnits::mm;
      endCapDistanceBetweenWheelCentersB[1][iiWheel]=row->getDouble("DISTBETWWHEELCENTECA")*GeoModelKernelUnits::mm;
    };
  } else {
    
    endCapPositionOfFirstWheelA[0] = RDBVars_A_Wheel->getFloat("POSITIONOFFIRSTWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelA[1] = RDBVars_A_Wheel->getFloat("POSITIONOFFIRSTWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelB[0] = RDBVars_B_Wheel->getFloat("POSITIONOFFIRSTWHEEL")*GeoModelKernelUnits::mm;
    endCapPositionOfFirstWheelB[1] = RDBVars_B_Wheel->getFloat("POSITIONOFFIRSTWHEEL")*GeoModelKernelUnits::mm;
    endCapDistanceBetweenWheelCentersA[0][0]=0.;
    endCapDistanceBetweenWheelCentersA[1][0]=0.;
    endCapDistanceBetweenWheelCentersB[0][endcapNumberOfAWheels]=0.;
    endCapDistanceBetweenWheelCentersB[1][endcapNumberOfAWheels]=0.;
    for (unsigned int iiWheel = 1; iiWheel < endcapNumberOfAWheels; iiWheel++) {
      endCapDistanceBetweenWheelCentersA[0][iiWheel]=RDBVars_A_Wheel->getFloat("DISTBETWWHEELCENTERS")*GeoModelKernelUnits::mm;
      endCapDistanceBetweenWheelCentersA[1][iiWheel]=RDBVars_A_Wheel->getFloat("DISTBETWWHEELCENTERS")*GeoModelKernelUnits::mm;
    };
    for (unsigned int iiWheel = (endcapNumberOfAWheels+1); iiWheel < (endcapNumberOfAWheels+endcapNumberOfBWheels); iiWheel++) {
      endCapDistanceBetweenWheelCentersB[0][iiWheel]=RDBVars_B_Wheel->getFloat("DISTBETWWHEELCENTERS")*GeoModelKernelUnits::mm;
      endCapDistanceBetweenWheelCentersB[1][iiWheel]=RDBVars_B_Wheel->getFloat("DISTBETWWHEELCENTERS")*GeoModelKernelUnits::mm;
    };
  };
  
  endCapDistanceBetweenWheelCentersC = RDBVars_C_Wheel->getFloat("DISTBETWWHEELCENTERS")*GeoModelKernelUnits::mm;
  endCapPositionOfFirstWheelC = RDBVars_C_Wheel->getFloat("POSITIONOFFIRSTWHEEL")*GeoModelKernelUnits::mm;
  
  
  endCapLayerZPositionA = new double[endCapNumberOfStrawLayersPerWheelA];
  endCapLayerZPositionB = new double[endCapNumberOfStrawLayersPerWheelB];
  endCapLayerZPositionC = new double[endCapNumberOfStrawLayersPerWheelC];
  
  for (unsigned int iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelA; iiPlane++) {
    const IRDBRecord* row = (*RDB_EndCapStrwLayZPos)[iiPlane];
    endCapLayerZPositionA[iiPlane] = row->getFloat("LAYERZPOSAWHEELS")*GeoModelKernelUnits::mm; 
  };
  for (unsigned int iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelB; iiPlane++) {
    const IRDBRecord* row = (*RDB_EndCapStrwLayZPos)[iiPlane];
    endCapLayerZPositionB[iiPlane] = row->getFloat("LAYERZPOSBWHEELS")*GeoModelKernelUnits::mm; 
  };
  for (unsigned int iiPlane = 0; iiPlane < endCapNumberOfStrawLayersPerWheelC; iiPlane++) {
    const IRDBRecord* row = (*RDB_EndCapStrwLayZPos)[iiPlane];
    endCapLayerZPositionC[iiPlane] = row->getFloat("LAYERZPOSCWHEELS")*GeoModelKernelUnits::mm;
  };
 

  
  if (includeECFoilHeatExchangerAndMembranes) {

    const IRDBRecord* RDBVars_ECHeatExchAndFoil = (*RDB_ECHeatExchAndFoil)[0];

    // Faraday foils kapton
    endCapFaradayFoilThickness = RDBVars_ECHeatExchAndFoil->getDouble("ECFARADAYFOILTHICK")*GeoModelKernelUnits::mm;
    
    // Heat Exchanger
    endCapRMinOfHeatExchanger  = RDBVars_ECHeatExchAndFoil->getDouble("ECHEATEXCHANGERRMIN")*GeoModelKernelUnits::mm;
    endCapRMaxOfHeatExchanger  = RDBVars_ECHeatExchAndFoil->getDouble("ECHEATEXCHANGERRMAX")*GeoModelKernelUnits::mm;
    endCapHeatExchangerThicknessA  = RDBVars_ECHeatExchAndFoil->getDouble("ECHEATEXCHTHICKNESSA")*GeoModelKernelUnits::mm;
    endCapHeatExchangerThicknessB  = RDBVars_ECHeatExchAndFoil->getDouble("ECHEATEXCHTHICKNESSB")*GeoModelKernelUnits::mm;
    
    // Membranes 
    const IRDBRecord* RDBVars_ECMembranePars = (*RDB_ECMembranePars)[0];

    endCapRMinOfMbrane = RDBVars_ECMembranePars->getDouble("RMINOFENDCAPMBRANE")*GeoModelKernelUnits::mm;
    endCapRMaxOfMbrane = RDBVars_ECMembranePars->getDouble("RMAXOFENDCAPMBRANE")*GeoModelKernelUnits::mm;
    endCapZMinOfMbraneWheelA1 = RDBVars_ECMembranePars->getDouble("ZMINPOSMBRANEWHEELA1")*GeoModelKernelUnits::mm;
    endCapZMaxOfMbraneWheelA1 = RDBVars_ECMembranePars->getDouble("ZMAXPOSMBRANEWHEELA1")*GeoModelKernelUnits::mm;
    endCapZMinOfMbraneWheelA2 = RDBVars_ECMembranePars->getDouble("ZMINPOSMBRANEWHEELA2")*GeoModelKernelUnits::mm;
    endCapZMaxOfMbraneWheelA2 = RDBVars_ECMembranePars->getDouble("ZMAXPOSMBRANEWHEELA2")*GeoModelKernelUnits::mm;
    endCapZMinOfMbraneWheelB1 = RDBVars_ECMembranePars->getDouble("ZMINPOSMBRANEWHEELB1")*GeoModelKernelUnits::mm;
    endCapZMaxOfMbraneWheelB1 = RDBVars_ECMembranePars->getDouble("ZMAXPOSMBRANEWHEELB1")*GeoModelKernelUnits::mm;
    endCapZMinOfMbraneWheelB2 = RDBVars_ECMembranePars->getDouble("ZMINPOSMBRANEWHEELB2")*GeoModelKernelUnits::mm;
    endCapZMaxOfMbraneWheelB2 = RDBVars_ECMembranePars->getDouble("ZMAXPOSMBRANEWHEELB2")*GeoModelKernelUnits::mm;

    endCapThicknessOfMbrane = endCapZMaxOfMbraneWheelA1 - endCapZMinOfMbraneWheelA1;

  }
}
const InDetDD::DistortedMaterialManager * 
TRT_DetDescrDB_ParameterInterface::distortedMatManager() const
{
  return m_distortedMatManager;
}

const GeoTrf::Transform3D &
TRT_DetDescrDB_ParameterInterface::partTransform(const std::string & partName) const
{
  if (m_placements) return m_placements->transform(partName);
  static const GeoTrf::Transform3D unitTransform = GeoTrf::Transform3D::Identity();
  return unitTransform;
}


bool
TRT_DetDescrDB_ParameterInterface::partPresent(const std::string & partName) const
{
  if (!m_placements) return true;
  return m_placements->present(partName);
}

