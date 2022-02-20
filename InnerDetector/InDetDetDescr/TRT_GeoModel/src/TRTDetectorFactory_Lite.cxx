/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPrimitives/GeoPrimitives.h"
#include "TRTDetectorFactory_Lite.h"
#include "TRT_DetDescrDB_ParameterInterface.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "TRT_ReadoutGeometry/TRT_BarrelDescriptor.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapDescriptor.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/Version.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GeoModelRead/ReadGeoModel.h"
#include "ArrayFunction.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include <vector>
#include <sstream>
#include <cmath>

//TK: get rid of these and use GeoGenfun:: and GeoXF:: instead
using namespace GeoGenfun;
using namespace GeoXF;


/////////////////////////////////// Constructor //////////////////////////////////
//
TRTDetectorFactory_Lite::TRTDetectorFactory_Lite(GeoModelIO::ReadGeoModel *sqliteReader, 
						 InDetDD::AthenaComps * athenaComps,
						 const ITRT_StrawStatusSummaryTool* sumTool, // added for Argon
						 bool useOldActiveGasMixture,
						 bool DC2CompatibleBarrelCoordinates,
						 int overridedigversion,
						 bool alignable,
						 bool useDynamicAlignmentFolders)
  : InDetDD::DetectorFactoryBase(athenaComps), 
    m_sqliteReader (sqliteReader),
    m_useOldActiveGasMixture(useOldActiveGasMixture),
    m_DC2CompatibleBarrelCoordinates(DC2CompatibleBarrelCoordinates),
    m_overridedigversion(overridedigversion),
    m_alignable(alignable),
    m_sumTool(sumTool),
    m_useDynamicAlignFolders(useDynamicAlignmentFolders)
{ 
}
//////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// Destructor ///////////////////////////////////
//
TRTDetectorFactory_Lite::~TRTDetectorFactory_Lite() 
{ 
}
//////////////////////////////////////////////////////////////////////////////////



///////////////////////////// getDetectorManager /////////////////////////////////
//
//  The method that actually returns the TRT_DetectorManager, which was created
//  and filled by the create() method
//
const InDetDD::TRT_DetectorManager * TRTDetectorFactory_Lite::getDetectorManager() const
{
  //TK: Maybe check that m_detectorManager!=0 ?
  return m_detectorManager;
}
//////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// create ///////////////////////////////////////
//
//  This is where the actual building of the geometry is performed.
//
//  The purpose of this is to create a new TRT_DetectorManager and fill it with
//  all the information relevant for detector description.
//
//  The TRT_DetectorManager itself, along with helper classes, descriptors, etc.
//  is located in InDetDetDescr/InDetReadoutGeometry.
//
void TRTDetectorFactory_Lite::create(GeoPhysVol *)
{

  std::map<std::string, GeoFullPhysVol*>        mapFPV = m_sqliteReader->getPublishedNodes<std::string, GeoFullPhysVol*>("TRT");
  std::map<std::string, GeoAlignableTransform*> mapAX  = m_sqliteReader->getPublishedNodes<std::string, GeoAlignableTransform*>("TRT");

  // The top level volumes
  GeoFullPhysVol *pBarrelVol = mapFPV["TRTBarrel"];
  GeoFullPhysVol *pEndCapABPlus  = mapFPV["TRTEndCapABPlus"];
  GeoFullPhysVol *pEndCapCPlus   = mapFPV["TRTEndCapCPlus"];
  GeoFullPhysVol *pEndCapABMinus = mapFPV["TRTEndCapABMinus"];
  GeoFullPhysVol *pEndCapCMinus  = mapFPV["TRTEndCapCPlus"];;

  

  // Create a new detectormanager.
  m_detectorManager = new InDetDD::TRT_DetectorManager;

  //---------------------- Initialize the parameter interface ------------------------//

  ATH_MSG_DEBUG( " Getting primary numbers from the Detector Description Database " );  
  TRT_DetDescrDB_ParameterInterface * parameterInterface = new TRT_DetDescrDB_ParameterInterface(getAthenaComps());
  m_data.reset(parameterInterface);


  IRDBAccessSvc* iAccessSvc = rdbAccessSvc();
 

  std::vector<GeoTrf::Transform3D> shellPosVec;  
  IRDBRecordset_ptr trtShellPosVecRecordSet = iAccessSvc->getRecordsetPtr("TRTShellPosVec","","","");
  for (size_t r=0;r<trtShellPosVecRecordSet->size();r++) {
    const IRDBRecord     * trtShellPosVecRecord  = (*trtShellPosVecRecordSet)[r];
    double xx=trtShellPosVecRecord->getDouble("xx");
    double xy=trtShellPosVecRecord->getDouble("xy");
    double xz=trtShellPosVecRecord->getDouble("xz");

    double yx=trtShellPosVecRecord->getDouble("yx");
    double yy=trtShellPosVecRecord->getDouble("yy");
    double yz=trtShellPosVecRecord->getDouble("yz");

    double zx=trtShellPosVecRecord->getDouble("zx");
    double zy=trtShellPosVecRecord->getDouble("zy");
    double zz=trtShellPosVecRecord->getDouble("zz");

    double dx=trtShellPosVecRecord->getDouble("dx");
    double dy=trtShellPosVecRecord->getDouble("dy");
    double dz=trtShellPosVecRecord->getDouble("dz");
    Eigen::Matrix4d M;
    M(0,0)=xx; M(0,1)=xy; M(0,2)=xz; M(0,3)=dx;
    M(1,0)=yx; M(1,1)=yy; M(1,2)=yz; M(1,3)=dy;
    M(2,0)=zx; M(2,1)=zy; M(2,2)=zz; M(2,3)=dz;
    M(3,0)= 0; M(3,1)= 0; M(3,2)= 0; M(3,3)=1;
    GeoTrf::Transform3D T;
    T.matrix()=M;
    shellPosVec.push_back(T);
  }
  //---------------------- Initialize ID Helper ------------------------------------//
  const TRT_ID *idHelper = nullptr;

  if (detStore()->retrieve(idHelper, "TRT_ID").isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve TRT ID Helper");
  }

  m_detectorManager->setIdHelper(idHelper,false);

  //---------------------- Set and Print Version Information ------------------------------------//

  //Set active gas type information.
  if (m_useOldActiveGasMixture) m_detectorManager->setGasType(InDetDD::TRT_DetectorManager::oldgas);
  else m_detectorManager->setGasType(InDetDD::TRT_DetectorManager::newgas);

  // Set Version information
  // Some of these get overwritten for new configurations.
  std::string versionTag = m_data->versionTag;
  std::string versionName = "DC2";
  std::string layout = "Final";
  std::string description = "DC2 Geometry";
  int versionMajorNumber = 2;
  int versionMinorNumber = 1;
  int versionPatchNumber = 0;

  if (m_data->initialLayout) layout = "Initial";
  //In principle we dont need to let the minor number reflect the
  //gastype anymore, but it doesn't hurt:
  if (m_useOldActiveGasMixture) versionMinorNumber = 0;
  if (!m_DC2CompatibleBarrelCoordinates) {
    versionMajorNumber = 3;
    versionName = "Rome";
    description = "Geometry for Rome 2005";
  }

  if (m_data->isCosmicRun) {
    layout = "SR1";
    description = "Geometry for SR1";
  }


  // If new configuration we get the version information from the database.
  // The version numbers can be incremented as one sees fit.
  // In principle they should be changed whenever there are any code changes.
  if (!m_data->oldConfiguration) {
    versionName = m_data->versionName;
    layout = m_data->layout;
    description = m_data->versionDescription;
    versionMajorNumber = 4;
    versionMinorNumber = 1;
    versionPatchNumber = 1;
  }

  InDetDD::Version version(versionTag,
			   versionName,
			   layout,
			   description,
			   versionMajorNumber,
			   versionMinorNumber,
			   versionPatchNumber);

  m_detectorManager->setVersion(version);


  // Print version information.
  ATH_MSG_INFO( "In TRT Detector Factory Lite" );
  ATH_MSG_INFO( " " << version.fullDescription() );


  //---------- Alignmnent and Conditions -----------//
  // Register the channels for alignment constants
  // and the level corresponding to the channel.
  // Not the levels are an internal definition . They are not the same as
  // the usual alignment levels
  const int AlignmentLevelSubWheel  = 1; // Level 2 in endcap. Not used in barrel
  const int AlignmentLevelModule    = 2; // Level 2 in barrel. Deprecated (wheel level) in endcap.
  const int AlignmentLevelTop       = 3; // Level 1

  if (m_alignable) {

    if (!m_useDynamicAlignFolders){
      m_detectorManager->addAlignFolderType(InDetDD::static_run1);
      m_detectorManager->addFolder("/TRT/Align");
      m_detectorManager->addChannel("/TRT/Align/TRT", AlignmentLevelTop, InDetDD::global);

      if (pBarrelVol) {
        m_detectorManager->addChannel("/TRT/Align/B0",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/Align/B1",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/Align/B2",  AlignmentLevelModule, InDetDD::global);
      }
      if (pEndCapABPlus) { // EndcapA
        m_detectorManager->addChannel("/TRT/Align/L2A", AlignmentLevelSubWheel, InDetDD::global);
      }
      if (pEndCapABMinus) {// EndcapC
        m_detectorManager->addChannel("/TRT/Align/L2C", AlignmentLevelSubWheel, InDetDD::global);
      }
    }

    else {
      m_detectorManager->addAlignFolderType(InDetDD::timedependent_run2);

      m_detectorManager->addGlobalFolder("/TRT/AlignL1/TRT");
      m_detectorManager->addChannel("/TRT/AlignL1/TRT", AlignmentLevelTop, InDetDD::global);
      m_detectorManager->addFolder("/TRT/AlignL2");

      if (pBarrelVol) {
        m_detectorManager->addChannel("/TRT/AlignL2/B0",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/AlignL2/B1",  AlignmentLevelModule, InDetDD::global);
        m_detectorManager->addChannel("/TRT/AlignL2/B2",  AlignmentLevelModule, InDetDD::global);
      }

      if (pEndCapABPlus) { // EndcapA 
        m_detectorManager->addChannel("/TRT/AlignL2/L2A", AlignmentLevelSubWheel, InDetDD::global);
      }
      if (pEndCapABMinus) {// EndcapC 
        m_detectorManager->addChannel("/TRT/AlignL2/L2C", AlignmentLevelSubWheel, InDetDD::global);
      }
    }

    // Unchanged in Run1 and new Run2 schema                                                     
    m_detectorManager->addSpecialFolder("/TRT/Calib/DX");
  }
                                               
  

  //Uncomment for testing:
  //  m_data->ShowValues();

  //---------- Digitization Version Info for dig. and recon r-t -----------//
  if (m_overridedigversion < 0 ) {
    m_detectorManager->setDigitizationVersion(m_data->digversion,m_data->digversionname);
  } else {
    m_detectorManager->setDigitizationVersion(m_overridedigversion,"CUSTOMOVERRIDDEN");
    ATH_MSG_INFO( "Digversion overridden via joboptions from " 
		  << m_data->digversion << " ('" << m_data->digversionname << "') to " 
		  << m_detectorManager->digitizationVersion()<< " ('" 
		  << m_detectorManager->digitizationVersionName()<<"')" );
  }


  //----------------------Initialize the numerology------------------------//

  for (unsigned int m=0;m<m_data->nBarrelRings;m++) {
    m_detectorManager->getNumerology()->setNBarrelLayers(m, m_data->barrelNumberOfStrawLayersInModule[m]);
  }

  m_detectorManager->getNumerology()->setNBarrelRings(m_data->nBarrelRings);
  //Note: This next line is now consistent with TRT_TestBeamDetDescr.
  m_detectorManager->getNumerology()->setNBarrelPhi(m_data->nBarrelModulesUsed);

  unsigned int nEndcapWheels = 0;
  if (pEndCapABPlus||pEndCapABMinus) nEndcapWheels += m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels;
  if (pEndCapCPlus||pEndCapCMinus)   nEndcapWheels += m_data->endcapNumberOfCWheels;

  m_detectorManager->getNumerology()->setNEndcapWheels(nEndcapWheels);
  m_detectorManager->getNumerology()->setNEndcapPhi(m_data->nEndcapPhi);

  for (unsigned int w=0;w<m_detectorManager->getNumerology()->getNEndcapWheels();w++) {
    unsigned int nlayers;
    if ( w < m_data->endcapNumberOfAWheels )
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelA;
    else if ( w < ( m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels ) )
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelB;
    else
      nlayers = m_data->endCapNumberOfStrawLayersPerWheelC;
    m_detectorManager->getNumerology()->setNEndcapLayers(w, nlayers ) ;
  }


  //
  // Barrel volume:
  //

  if (pBarrelVol) {

    ATH_MSG_DEBUG( "Virtual TRT Barrel volume defined by RMin = "<<m_data->virtualBarrelInnerRadius 
                  <<", Rmax = "<<m_data->virtualBarrelOuterRadius<<" Zmax = "<<m_data->virtualBarrelVolumeLength );

    // Common Endcap volumes (one for forward, one for backward):
    //GeoPhysVol *pCommonEndcapVolume[2];

    GeoAlignableTransform * barrelTransform = mapAX["TRTBarrel"];

    m_detectorManager->addTreeTop(pBarrelVol);
    // Use barrel_ec_id = -1 (+ve and -ve barrel is treated as one alignable object)
    Identifier id = idHelper->barrel_ec_id(-1);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, barrelTransform, pBarrelVol); // global if other selected

  }

  if (pEndCapABPlus) {

    GeoAlignableTransform * transform = mapAX["TRTEndCapABPlus"];

    m_detectorManager->addTreeTop(pEndCapABPlus);
    Identifier id = idHelper->barrel_ec_id(2);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, transform, pEndCapABPlus); // global if other selected
  }

  if (pEndCapABMinus) {

    GeoAlignableTransform * transform = mapAX["TRTEndCapABMinus"];

    m_detectorManager->addTreeTop(pEndCapABMinus);
    Identifier id = idHelper->barrel_ec_id(-2);
    m_detectorManager->addAlignableTransform(AlignmentLevelTop, id, transform, pEndCapABMinus); // global if other selected
  }

  if (pEndCapCPlus)  {
    m_detectorManager->addTreeTop(pEndCapCPlus);
  }
  if (pEndCapCMinus) {
    m_detectorManager->addTreeTop(pEndCapCMinus);
  }
  // Pointers to the Endcap volumes (index 0: for forward, index 1: for backward):
  GeoFullPhysVol *pCommonEndcapAB[]={pEndCapABPlus,pEndCapABMinus};
  GeoFullPhysVol *pCommonEndcapC[]={pEndCapCPlus,pEndCapCMinus};
  //-----------------------------------------------------------------------//
  //                                                                       //
  // Barrel                                                                //
  //                                                                       //
  //-----------------------------------------------------------------------//


  if (pBarrelVol) {


    //-----------------------------------------------------------------------//
    //                                                                       //
    // Barrel Modules                                                        //
    //                                                                       //
    //-----------------------------------------------------------------------//

    std::vector<InDetDD::TRT_BarrelDescriptor *> bDescriptor;

    // Create some shared stuff to stick into each module.

    // The barrel straw (including the "hole" in the radiator around it):
    double activeGasZPositionNormalStraws= activeGasZPosition();
    // The straws in the inner layers of module A have a large dead region, and are thus different.
    double activeGasZPositionStrawsWithLargeDeadRegion= activeGasZPosition(true);
   
    // The modules themselves.
    for (size_t iABC=0;iABC<m_data->nBarrelRings;iABC++) {

      GeoTrf::Transform3D shellPosition=shellPosVec[iABC];
 
      //----------------------------------------------------------------------------------------------------------------
      // Parameterize all of the straws and put them within the radiator.

      // Figure out how many straws have a large dead region
      size_t nStrawsWithLargeDeadRegion = 0;
      if (iABC==0) {
	for (size_t iLayer = 0; iLayer<m_data->barrelNumberOfLayersWithLargeDeadRegion; iLayer++) {
	  nStrawsWithLargeDeadRegion += m_data->barrelNumberOfStrawsInStrawLayer[iABC][iLayer];
	}
      }
    
      // Generators:
      GeoTrf::TranslateX3D Xx(1.0);
      GeoTrf::TranslateY3D Xy(1.0);

      GENFUNCTION  fx = ArrayFunction(&m_data->strawXPosition[iABC][0+nStrawsWithLargeDeadRegion],
				      &m_data->strawXPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      //TK: why ..[0]+n and not ..[n] ?
      GENFUNCTION  fy = ArrayFunction(&m_data->strawYPosition[iABC][0+nStrawsWithLargeDeadRegion],
				      &m_data->strawYPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      TRANSFUNCTION tx1 = Pow(Xx,fx)*Pow(Xy,fy);

      //TK: Quick fix, might waste a few KB of memory.
      //TK: only use when iABC==0
      GENFUNCTION  fxAll = ArrayFunction(&m_data->strawXPosition[iABC][0], &m_data->strawXPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      GENFUNCTION  fyAll = ArrayFunction(&m_data->strawYPosition[iABC][0], &m_data->strawYPosition[iABC][0]+m_data->barrelNumberOfStrawsInModule[iABC]);
      TRANSFUNCTION tx1All = Pow(Xx,fxAll)*Pow(Xy,fyAll);


      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //Calculation of needed transforms
      //First get the global and local positions of the two alignment straws:
      //USE HEP2VECTORS!!!

      GeoTrf::Vector3D Align1Global(m_data->barrelXOfFirstGlobalAlignmentStraw[iABC],  m_data->barrelYOfFirstGlobalAlignmentStraw[iABC], 0);
      GeoTrf::Vector3D Align2Global(m_data->barrelXOfSecondGlobalAlignmentStraw[iABC], m_data->barrelYOfSecondGlobalAlignmentStraw[iABC],0);
      GeoTrf::Vector3D Align1Local(m_data->strawXPosition[iABC][0],m_data->strawYPosition[iABC][0],0);
      GeoTrf::Vector3D Align2Local(m_data->strawXPosition[iABC][m_data->barrelIndexOfSecondGlobalAlignmentStraw[iABC]],
				   m_data->strawYPosition[iABC][m_data->barrelIndexOfSecondGlobalAlignmentStraw[iABC]],0);

      //We need to make first a translation which puts the first alignment straw into place:

      //And we need to make a rotation which puts the second one on its position:

      GeoTrf::Vector2D local12((Align2Local - Align1Local).x(),(Align2Local  - Align1Local).y());
      GeoTrf::Vector2D global12((Align2Global - Align1Global).x(),(Align2Global - Align1Global).y());
      double zrotang = global12.phi()-local12.phi();

      //Here we combine these two into a GeoTrf::Transform3D:

      GeoTrf::Transform3D absStrawXForm = GeoTrf::Translate3D(Align1Global.x(),Align1Global.y(),Align1Global.z())
	*GeoTrf::RotateZ3D( zrotang )
	*GeoTrf::Translate3D(-Align1Local.x(),-Align1Local.y(),-Align1Local.z());

      //
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      //Why not use radiator instead of shell?
      TRANSFUNCTION tx2=shellPosition.inverse()*absStrawXForm*tx1;
      TRANSFUNCTION tx2All=shellPosition.inverse()*absStrawXForm*tx1All;
      if (iABC==0) {
	//TK: move rest of ...All stuff here?
	m_detectorManager->setBarrelTransformField(iABC,tx2All.clone());
      } else {
	m_detectorManager->setBarrelTransformField(iABC,tx2.clone());
      }


      // Adds one straw from each layer (reformulate..) (should be done via m_data from database)
      double oldx=-999*GeoModelKernelUnits::cm, oldz=-999*GeoModelKernelUnits::cm;
      unsigned int c=0;
      size_t iLayer=0;
      while (c< m_data->barrelNumberOfStrawsInModule[iABC] ) {

	GeoTrf::Vector3D p(0,0,0);
	if (iABC==0)
	  p = tx2All(c)*p;
	else
	  p = tx2(c)*p;

	double x = p.x();
	double z = p.z();

	//TK: use arrays!! update this...
	if (sqrt((x-oldx)*(x-oldx)+ (z-oldz)*(z-oldz))> 5*GeoModelKernelUnits::cm) {
	  iLayer++;
	  bDescriptor.push_back(new InDetDD::TRT_BarrelDescriptor());
          m_detectorManager->setBarrelDescriptor(bDescriptor.back());
	  bDescriptor.back()->setStrawTransformField(m_detectorManager->barrelTransformField(iABC),c);

	  //TK: Next, we are providing information about the Z
	  //dimensions of the active gas, to be used for reconstruction
	  //purposes. Personally I find "strawZDead" to be a slightly
	  //confusing choice of name for that method.

	  if((iABC==0)&&(iLayer<=m_data->barrelNumberOfLayersWithLargeDeadRegion )) {
	    //TK: these things should come back from makestraw...
	    double lengthOfActiveGas=
	      (m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - m_data->lengthOfDeadRegion-m_data->barrelLengthOfLargeDeadRegion;
	    double startZOfActiveGas=activeGasZPositionStrawsWithLargeDeadRegion-lengthOfActiveGas/2.0;
	    bDescriptor.back()->strawZPos(activeGasZPositionStrawsWithLargeDeadRegion);
	    bDescriptor.back()->strawZDead(startZOfActiveGas);
	    bDescriptor.back()->strawLength(lengthOfActiveGas);
	  } else {
	    double lengthOfActiveGas=(m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - 2*m_data->lengthOfDeadRegion;
	    double startZOfActiveGas=activeGasZPositionNormalStraws-lengthOfActiveGas/2.0;
	    bDescriptor.back()->strawZPos(activeGasZPositionNormalStraws);
	    bDescriptor.back()->strawZDead(startZOfActiveGas);
	    bDescriptor.back()->strawLength(lengthOfActiveGas);
	  }

	}
	bDescriptor.back()->addStraw(z,x);
	oldx=x; oldz=z;
	c++;

      }


      // Now create m_data->nBarrelModulesUsed unique modules within each layer.
      for (size_t iMod = 0; iMod<m_data->nBarrelModulesUsed;iMod++) {
        GeoFullPhysVol * pShell = mapFPV["TRTShell-"+std::to_string(iABC)+"-"+std::to_string(iMod)];
	GeoAlignableTransform * xfx1 = mapAX["TRTShell-"+std::to_string(iABC)+"-"+std::to_string(iMod)];


	// Register the alignable transfrom to the manager
	// +ve and -ve are part of the same barrel. We use barrel_ec = -1.
	Identifier idModule = idHelper->module_id(-1, iMod, iABC);
	// In barrel frame (generally the same as the global frame)
	m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfx1, pShell, pBarrelVol);
	
	//-------------------------------------------------------------------//
	//                                                                   //
	// Barrel readout:                                                   //
	//                                                                   //
	//-------------------------------------------------------------------//

	unsigned int nStrawLayers = m_detectorManager->getNumerology()->getNBarrelLayers(iABC);
	for (unsigned int iStrawLayer=0;iStrawLayer<nStrawLayers; iStrawLayer++) { // limit stored as float!

	  unsigned int jStrawLayer=iStrawLayer;
	  if (iABC>0) jStrawLayer += m_detectorManager->getNumerology()->getNBarrelLayers(0);
	  if (iABC>1) jStrawLayer += m_detectorManager->getNumerology()->getNBarrelLayers(1);
	  //TK: just go from jStrawLayer=layerstart;jStrawLayer<layerend ?

	  InDetDD::TRT_BarrelDescriptor *bD=bDescriptor[jStrawLayer];

	  InDetDD::TRT_BarrelElement *element0 = new InDetDD::TRT_BarrelElement(pShell, bD, 0  , iABC, iMod, iStrawLayer, idHelper, m_detectorManager->conditions());
	  InDetDD::TRT_BarrelElement *element1 = new InDetDD::TRT_BarrelElement(pShell, bD, 1  , iABC, iMod, iStrawLayer, idHelper, m_detectorManager->conditions());

	  m_detectorManager->manageBarrelElement(element0);
	  m_detectorManager->manageBarrelElement(element1);
	}

      }//End "for (size_t iMod = ..." loop.

    }

    // Set up the nearest neighbor pointers: in R.
    for (unsigned int e=0;e<2;e++) {
      for  (unsigned int iMod=0;iMod<m_data->nBarrelModulesUsed; iMod++) {
	InDetDD::TRT_BarrelElement *prev=nullptr;
	for (unsigned int iABC=0;iABC<m_data->nBarrelRings;iABC++) {
	  for (unsigned int s=0;s<m_detectorManager->getNumerology()->getNBarrelLayers(iABC); s++) {
	    InDetDD::TRT_BarrelElement *current = m_detectorManager->getBarrelElement(e,iABC, iMod, s);
	    if (prev && current) {
	      prev->setNextInR(current);
	      current->setPreviousInR(prev);
	    }
	    prev=current;
	  }
	}
      }
    }

    // Set up the nearest neighbor pointers: in Phi.
    for (unsigned int e=0;e<2;e++) {
      for (unsigned int iABC=0;iABC<m_data->nBarrelRings;iABC++) {
	for (unsigned int s=0;s<m_detectorManager->getNumerology()->getNBarrelLayers(iABC); s++) {
	  InDetDD::TRT_BarrelElement *prev=nullptr;
	  for  (unsigned int iMod=0;iMod<m_data->nBarrelModulesUsed; iMod++) {
	    InDetDD::TRT_BarrelElement *current = m_detectorManager->getBarrelElement(e,iABC, iMod, s);
	    if (prev && current) {
	      prev->setNextInPhi(current);
	      current->setPreviousInPhi(prev);
	    }
	    prev=current;
	  }
	  if (m_data->nBarrelModulesUsed==m_data->nBarrelModules) { // Full complement; then, we wrap!:
	    InDetDD::TRT_BarrelElement *first=m_detectorManager->getBarrelElement(e,iABC,0,s);
	    InDetDD::TRT_BarrelElement *last =m_detectorManager->getBarrelElement(e,iABC,m_data->nBarrelModules-1,s);
	    if (first && last) {
	      first->setPreviousInPhi(last);
	      last->setNextInPhi(first);
	    }
	  }
	}
      }
    }
  }//end of if (pBarrelVol)

 

  //-----------------------------------------------------------------------//
  //                                                                       //
  // Endcap Modules                                                        //
  //                                                                       //
  //-----------------------------------------------------------------------//
  
  // TK: This part could really use some cleanup and reordering.
  //     There is no need to repeat the same code for A, B & C endcaps.


  // if none of the endcaps is being built we can return.
  if (!(pEndCapABPlus || pEndCapABMinus || pEndCapCPlus || pEndCapCMinus)){
    return;
  }
  unsigned int firstIndexOfA = 0;
  unsigned int firstIndexOfB = m_data->endcapNumberOfAWheels;
  unsigned int firstIndexOfC = m_data->endcapNumberOfAWheels + m_data->endcapNumberOfBWheels;

  unsigned int indexUpperBound = firstIndexOfA + m_detectorManager->getNumerology()->getNEndcapWheels();

  if (m_data->initialLayout) indexUpperBound = firstIndexOfC; // No wheel C.

  const unsigned int nSides = 2;
  const unsigned int nStrawLayMaxEc = 8;//hardcoded...

  unsigned int iiSide, iiWheel, iiPlane, iiPhi, counter;    //set of counters
  GeoFullPhysVol *childPlane = nullptr;


  double RotationsOfStrawPlanes[nStrawLayMaxEc]; //8 is hardcoded
  double shiftForEachRotation = m_data->endCapShiftForEachRotation; // in units of deltaPhi
  RotationsOfStrawPlanes[0] = 0.;

  bool oldGeometry = true;
  // Temporary way to determine old from new
  if (shiftForEachRotation < 0) oldGeometry = false;

  if (oldGeometry) {
    // For old geometry
    for (counter = 1; counter < nStrawLayMaxEc; counter++)
      {
	RotationsOfStrawPlanes[counter] = RotationsOfStrawPlanes[counter-1] + shiftForEachRotation;
	if (RotationsOfStrawPlanes[counter] >= 1.)
	  RotationsOfStrawPlanes[counter] -= 1.;
      }
  } else {
    // New geometry
    double RotationsOfStrawPlanesTmp[nStrawLayMaxEc] = {0,0,0,0,2,2,2,2}; 
    for (counter = 0; counter < nStrawLayMaxEc; counter++)
      {
	RotationsOfStrawPlanes[counter] = (counter * shiftForEachRotation) +  RotationsOfStrawPlanesTmp[counter];
      }
  }

  // Create and initialize by 0 arrays of descriptors
  std::vector<InDetDD::TRT_EndcapDescriptor*> descriptorsAB[nSides][nStrawLayMaxEc];
  std::vector<InDetDD::TRT_EndcapDescriptor*> descriptorsC[nSides][nStrawLayMaxEc];
  InDetDD::TRT_EndcapDescriptor* pDescriptor = nullptr;
  InDetDD::TRT_EndcapElement* element = nullptr;

  for(iiSide = 0; iiSide<nSides; iiSide++) {
    for(iiPlane = 0; iiPlane < nStrawLayMaxEc; iiPlane++) {
      descriptorsAB[iiSide][iiPlane].resize (m_data->nEndcapPhi);
      descriptorsC[iiSide][iiPlane].resize (m_data->nEndcapPhi);
    }
  }



  // Do Wheels A and B if one of them is present
  if (pEndCapABPlus || pEndCapABMinus) {
    // --------------   Wheel A  -----------------------
    
    
    // Straw plane
    setEndcapTransformField(firstIndexOfA);

    
    // This is the straw pitch.
    double deltaPhiForStrawsA = 360.*GeoModelKernelUnits::deg/m_data->endcapNumberOfStrawsInStrawLayer_AWheels;
    
    
    // In reality the positive and negative endcaps are built identical, both in 
    // geometry and readout. The offline numbering however keeps phi numbering going 
    // in the same direction as global phi (righthanded direction). 
    
    // For the latest version we build +ve and negative endcaps identical.
    // We also build the descriptors identical apart from the setting of startphi.
    //
    // The mapping is fixed (this must be reproduced in the sensitive
    // detector and readout geometry) The mapping is 1-1 for the 
    // +ve endcap, for the -ve endcap it is as follows:
    //
    //   ***************************************************************
    //   *  Negative endcap (Endcap C) mapping.                        *
    //   *                                                             *
    //   *  nSectors = 32                                              *
    //   *  nStraws = num straws in sector                             *
    //   *  sector -> (nSectors + nSectors/2 - sector - 1) % nSectors  *
    //   *  straw  -> nStraws - 1 - straw                              *  
    //   ***************************************************************
    //
    // For compatibility with old (wrong geometry) we rotate the strawlayers 
    // differently for the negative endcap than we do for the positive endcap.
    // This is to allow the sensitive detector and readout geometry to have
    // the same code for both layouts. 
    //
    // Below we refere to online as the physical readout and offline as the offline
    // identifier convetions.
    // iiPhi corresponds to the "online" readout phi sector. This goes
    // right handed in positive endcap and left handed in negative, where handedness 
    // is wrt to global frame.
    // iiPhiOffline is the offline numbering which is always right handed.
    
    for(iiSide=0; iiSide<nSides; iiSide++) {
      // Wheel A
      if (pCommonEndcapAB[iiSide]) {
	
	for(iiWheel=firstIndexOfA; iiWheel < firstIndexOfB; iiWheel++)
	  {
	    //prepair to place wheel
	    
	    GeoFullPhysVol* pWheelA = mapFPV["TRTWheelA-"
					     +std::to_string(iiSide)+"-"
					     +std::to_string(iiWheel)];
	    
	    GeoAlignableTransform * xfAlignableModule = nullptr;
	    
	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelA; iiPlane++)
	      {
		// WheelA is subdivided into 4 alignable objects. (Every 4th straw layer)
		// We create an alignable transform for each alignable module 
		// and multiply this by the transform for every straw layer in the "alignable module" 
		// The tranform is by default Identity.
		if (iiPlane % 4 == 0) {
		  // Register alignable node
		  int barrel_ec = (iiSide) ? -2 : +2;
		  xfAlignableModule = mapAX["TRTWheelA-StrawPlane-"
					    +std::to_string(iiSide)+"-"
					    +std::to_string(iiWheel)+"-"
					    +std::to_string(iiPlane)];
		  
		  Identifier idSubModule = idHelper->layer_id(barrel_ec, 0, iiWheel, iiPlane); 
		  // We pass the parent volume as the local delta for this correction is the same as a local delta
		  // on the transformation of the wheel.
		  m_detectorManager->addAlignableTransform(AlignmentLevelSubWheel, idSubModule, xfAlignableModule, pWheelA); 	    
		}
		
		// phiPlane is phi of straw 0, sector 0 (online numbering)
		double phiPlane = m_data->endCapPhiOfFirstStraw + RotationsOfStrawPlanes[iiPlane%nStrawLayMaxEc]*deltaPhiForStrawsA;
		
		// For compatibility with old geometry we have to shift every eighth wheel by 1 straw pitch.
		if(iiSide && oldGeometry && (iiPlane%8 == 0)) {
		  phiPlane +=  deltaPhiForStrawsA;
		}
		
		Identifier TRT_Identifier;
		int bar_ec = (iiSide) ? -2 : +2;
		TRT_Identifier = idHelper->straw_id(bar_ec, 1, iiWheel, 1, 1);
		
		
		childPlane = mapFPV["TRTWheelA-StrawPlane-"
				    +std::to_string(iiSide)+"-"
				    +std::to_string(iiWheel)+"-"
				    +std::to_string(iiPlane)];
		
		
		// Create descriptors
		// Just do it for the first wheel
		if(iiWheel==firstIndexOfA && iiPlane < nStrawLayMaxEc)
		  for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		    {
		      
		      pDescriptor = new InDetDD::TRT_EndcapDescriptor();
                      m_detectorManager->setEndcapDescriptor(pDescriptor);
		      
		      pDescriptor->nStraws() = m_data->endcapNumberOfStrawsInStrawLayer_AWheels/m_data->nEndcapPhi;
		      pDescriptor->strawPitch() = deltaPhiForStrawsA;
		      
		      double startPhi = phiPlane + iiPhi * pDescriptor->strawPitch() * pDescriptor->nStraws();
		      
		      // For negative endcap the startPhi is the last straw in the physical sector
		      //   phi -> phi + strawPitch*(n-1)
		      // it then gets rotated 180 around y axis 
		      //   phi -> pi - phi
		      if (iiSide) {
			startPhi = GeoModelKernelUnits::pi - (startPhi + pDescriptor->strawPitch() * (pDescriptor->nStraws() - 1));
		      }
		      
		      // Make sure its between -pi and pi.
		      if (startPhi <= -GeoModelKernelUnits::pi) startPhi += 2*GeoModelKernelUnits::pi;
		      if (startPhi > GeoModelKernelUnits::pi) startPhi -= 2*GeoModelKernelUnits::pi;
		      
		      pDescriptor->startPhi() = startPhi;
		      
		      pDescriptor->strawLength() = m_data->endCapOuterRadiusOfSupportA - m_data->endCapRadialThicknessOfOuterSupportA
			- 2*m_data->lengthOfDeadRegion - m_data->endCapRadialThicknessOfInnerSupportA - m_data->endCapInnerRadiusOfSupportA;
		      pDescriptor->innerRadius() = m_data->endCapInnerRadiusOfSupportA + m_data->endCapRadialThicknessOfInnerSupportA
			+ m_data->lengthOfDeadRegion;
		      pDescriptor->setStrawTransformField(m_detectorManager->endcapTransformField(0),iiPhi*pDescriptor->nStraws());
		      
		      descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi] = pDescriptor;
		    }
		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     m_detectorManager->conditions());
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }
	    // Place Inner/Outer supports in the wheel
	    
	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel = mapAX["TRTWheelA-"
						    +std::to_string(iiSide)+"-"
						    +std::to_string(iiWheel)];
	    
	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelA); 	    
	    
	  } // iiWheel loop for Wheel A
      } // if (pCommonEndcapAB[iiSide]) block for Wheel A
    } // iiSide loop for Wheel A
    
    
    
    // ---------------  Wheel B  ----------------------------
    
    // Straw plane
    setEndcapTransformField(firstIndexOfB);
    
    for(iiSide=0; iiSide<nSides; iiSide++) {
      
      // Wheel B
      if (pCommonEndcapAB[iiSide]) {
	for(iiWheel=firstIndexOfB; iiWheel < firstIndexOfC; iiWheel++)
	  {
	    
	    GeoFullPhysVol* pWheelB = mapFPV["TRTWheelB-"
					     +std::to_string(iiSide)+"-"
					     +std::to_string(iiWheel)];

	    GeoAlignableTransform * xfAlignableModule = nullptr;	    
	    
	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelB; iiPlane++)
	      {
		
		// Each wheel in WheelB is subdivided into 2 alignable objects (every 4th straw layer)
		// We create an alignable transform for each alignable module 
		// and multiply this by the transform for every straw layer in the "alignable module" 
		// The tranform is by default Identity.
		if (iiPlane % 4 == 0) {
		  // Register alignable node
		  int barrel_ec = (iiSide) ? -2 : +2;
		  xfAlignableModule = 	    mapAX["TRTWheelB-StrawPlane-"
					    +std::to_string(iiSide)+"-"
					    +std::to_string(iiWheel)+"-"
					    +std::to_string(iiPlane)];



		  Identifier idSubModule = idHelper->layer_id(barrel_ec, 0, iiWheel, iiPlane); 
		  // We pass the parent volume as the local delta for this correction is the same as a local delta
		  // on the transformation of the wheel.
		  m_detectorManager->addAlignableTransform(AlignmentLevelSubWheel, idSubModule, xfAlignableModule, pWheelB); 	    
		}
		
		Identifier TRT_Identifier;
		int bar_ec = (iiSide) ? -2 : +2;
		TRT_Identifier = idHelper->straw_id(bar_ec, 1, iiWheel, 1, 1);
		

		childPlane = mapFPV["TRTWheelB-StrawPlane-"
				    +std::to_string(iiSide)+"-"
				    +std::to_string(iiWheel)+"-"
				    +std::to_string(iiPlane)];
		


		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsAB[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     m_detectorManager->conditions());
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }
	    
	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel = mapAX["TRTWheelB-"
						    +std::to_string(iiSide)+"-"
						    +std::to_string(iiWheel)];
	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelB); 	    
	  }// iiWheel loop  for Wheel B
      } // if (pCommonEndcapAB[iiSide]) block for Wheel B
    } // iiSide loop for Wheel B
    //pStrawPlaneB->unref(); // Get eventual seg fault if unref. Clone doesn't increment ref count of orig, See bug #34074
    
  } // end AB
  
    // ----------------  Wheel C  ---------------------------
    // Not present in initial layout
  if (pEndCapCPlus || pEndCapCMinus) {

    setEndcapTransformField(firstIndexOfC);

    // This is the straw pitch.
    double deltaPhiForStrawsC = 360.*GeoModelKernelUnits::deg/m_data->endcapNumberOfStrawsInStrawLayer_CWheels;
  
    for(iiSide=0; iiSide<nSides; iiSide++) {
      // Wheel C
      if (pCommonEndcapC[iiSide]) {
	for(iiWheel=firstIndexOfC; iiWheel < indexUpperBound; iiWheel++)
	  {
	    GeoFullPhysVol* pWheelC = mapFPV["TRTWheelC-"
					     +std::to_string(iiSide)+"-"
					     +std::to_string(iiWheel)];



	    // Place planes in the wheel
	    for (iiPlane = 0; iiPlane < m_data->endCapNumberOfStrawLayersPerWheelC; iiPlane++)
	      {
		// phiPlane is phi of straw 0, sector 0 (online numbering)
		double phiPlane = m_data->endCapPhiOfFirstStraw +  RotationsOfStrawPlanes[iiPlane%nStrawLayMaxEc]*deltaPhiForStrawsC;

		// For compatibility with old geometry we have to shift every eighth wheel by 1 straw pitch.
		if(iiSide && oldGeometry && (iiPlane%8 == 0)) {
		  phiPlane +=  deltaPhiForStrawsC;
		}



		childPlane = mapFPV["TRTWheelC-StrawPlane-"
				    +std::to_string(iiSide)+"-"
				    +std::to_string(iiWheel)+"-"
				    +std::to_string(iiPlane)];


		// Create descriptors
		// Just do it for the first wheel
		if(iiWheel==firstIndexOfC && iiPlane < nStrawLayMaxEc)
		  for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		    {
		      pDescriptor = new InDetDD::TRT_EndcapDescriptor();
                      m_detectorManager->setEndcapDescriptor(pDescriptor);

		      pDescriptor->nStraws() = m_data->endcapNumberOfStrawsInStrawLayer_CWheels/m_data->nEndcapPhi;
		      pDescriptor->strawPitch() = deltaPhiForStrawsC;


		      double startPhi = phiPlane + iiPhi * pDescriptor->strawPitch() * pDescriptor->nStraws();

		      // For negative endcap the startPhi is the last straw in the physical sector, it then gets
		      // rotated 180 around y axis (phi -> pi - phi)
		      if (iiSide) {
			startPhi = GeoModelKernelUnits::pi - (startPhi + pDescriptor->strawPitch() * (pDescriptor->nStraws() - 1));
		      }
		      
		      // Make sure its between -pi and pi.
		      if (startPhi <= -GeoModelKernelUnits::pi) startPhi += 2*GeoModelKernelUnits::pi;
		      if (startPhi > GeoModelKernelUnits::pi) startPhi -= 2*GeoModelKernelUnits::pi;
		      
		 		      
		      pDescriptor->startPhi() = startPhi;

		      pDescriptor->strawLength() = m_data->endCapOuterRadiusOfSupportC - m_data->endCapRadialThicknessOfOuterSupportC
			- 2*m_data->lengthOfDeadRegion - m_data->endCapRadialThicknessOfInnerSupportC - m_data->endCapInnerRadiusOfSupportC;
		      pDescriptor->innerRadius() = m_data->endCapInnerRadiusOfSupportC + m_data->endCapRadialThicknessOfInnerSupportC + m_data->lengthOfDeadRegion;
		      pDescriptor->setStrawTransformField(m_detectorManager->endcapTransformField(2),iiPhi*pDescriptor->nStraws());


		      descriptorsC[iiSide][iiPlane%nStrawLayMaxEc][iiPhi] = pDescriptor;
		    }

		// Create elements
		for(iiPhi = 0; iiPhi < m_data->nEndcapPhi; iiPhi++)
		  {
		    // m_data->nEndcapPhi assumed to be even.
		    // For positive endcap online == offline. For negative endcap we rotate 180 deg about y axis so 
		    // sector 0 -> 15, 15 -> 0, 16 -> 31, 31 -> 16, etc. This is achieved with 
		    // sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
		    int iiPhiOffline = (iiSide==0) ? iiPhi :  (3*m_data->nEndcapPhi/2 - iiPhi - 1)% m_data->nEndcapPhi;
		    element = new InDetDD::TRT_EndcapElement(childPlane,
							     descriptorsC[iiSide][iiPlane%nStrawLayMaxEc][iiPhi],
							     iiSide==0,
							     iiWheel,
							     iiPlane,
							     iiPhiOffline,
							     idHelper,
							     m_detectorManager->conditions());
		    m_detectorManager->manageEndcapElement(element);
		  }
	      }


	    // Place wheel in the Endcap Volume
	    GeoAlignableTransform * xfWheel = mapAX["TRTWheelC-"
						    +std::to_string(iiSide)+"-"
						    +std::to_string(iiWheel)];

	    // Register alignable node
	    int barrel_ec = (iiSide) ? -2 : +2;
	    Identifier idModule = idHelper->module_id(barrel_ec, 0, iiWheel);
	    m_detectorManager->addAlignableTransform(AlignmentLevelModule, idModule, xfWheel, pWheelC); 	    
	    

	  } // iiWheel loop for Wheel C
      } // if (pCommonEndcapC[iiSide]) block for Wheel C
    } // iiSide loop for Wheel C
    //pStrawPlaneC->unref(); // Get eventual seg fault if unref. Clone doesn't increment ref count of orig, See bug #34074

  } // End Wheel C


    // Set up the nearest neighbor pointers: in Z
  for (iiSide=0; iiSide<2; iiSide++)
    for(iiPhi=0; iiPhi<m_data->nEndcapPhi; iiPhi++)
      {
	InDetDD::TRT_EndcapElement *prev = nullptr;
	for (iiWheel=0; iiWheel<indexUpperBound; iiWheel++)
	  for (iiPlane=0; iiPlane<m_detectorManager->getNumerology()->getNEndcapLayers(iiWheel); iiPlane++)
	    {
	      InDetDD::TRT_EndcapElement *current = m_detectorManager->getEndcapElement(iiSide, iiWheel, iiPlane, iiPhi);
	      if (prev && current)
		{
		  prev->setNextInZ(current);
		  current->setPreviousInZ(prev);
		}
	      prev=current;
	    }
      }
}

//////////////////////////////////////////////////////////////////////////////////




//GeoPhysVol * TRTDetectorFactory_Lite::makeStraw( double& activeGasZPosition, bool hasLargeDeadRegion /*= false*/ ) const {
double TRTDetectorFactory_Lite::activeGasZPosition(bool hasLargeDeadRegion /*= false*/) const {

  double lengthOfInnerDeadRegion= hasLargeDeadRegion ? m_data->barrelLengthOfLargeDeadRegion : m_data->lengthOfDeadRegion ;
  double lengthOfActiveGas = (m_data->barrelLengthOfStraw-m_data->barrelLengthOfTwister)/2.0 - m_data->lengthOfDeadRegion - lengthOfInnerDeadRegion;
  return (lengthOfActiveGas + m_data->barrelLengthOfTwister) / 2. + lengthOfInnerDeadRegion; // middle of lengthOfActiveGas

}
//////////////////////////////////////////////////////////////////////////////////



///////////////////////////////// makeStrawPlane /////////////////////////////////
//
void TRTDetectorFactory_Lite::setEndcapTransformField(size_t w) {

  size_t nstraws=0;

  //A and B wheels have similar straw planes, but the C wheels are different.
  //  const size_t firstIndexOfC = 15; //hardcoded
  const size_t firstIndexOfC = 14; //hardcoded

  double R0, R1;
  if (w >= firstIndexOfC) {
    //C wheels:
    nstraws=m_data->endcapNumberOfStrawsInStrawLayer_CWheels;
    R0     = m_data->endcapOuterRadiusOfInnerSupport_wheelC;
    R1     = m_data->endcapInnerRadiusOfOuterSupport_wheelC;
  } else {
    //A and B wheels:
    R0     = m_data->endcapOuterRadiusOfInnerSupport_wheelAB;
    R1     = m_data->endcapInnerRadiusOfOuterSupport_wheelAB;
    nstraws=m_data->endcapNumberOfStrawsInStrawLayer_AWheels;
  }

  double pos    = 0.5*(R0+R1);

  // Positioning of straws :
  double dphi = 2*M_PI/ nstraws;
  GeoTrf::RotateZ3D    Rz(1.0);// Radians!
  GeoTrf::TranslateX3D Tx(1.0);// MM! TK: actually this doesnt need to be interpreted as mm? Just as a dimensionless 1. (i guess)
  GeoTrf::TranslateY3D Ty(1.0);// MM!
  Variable    i;
  Sin sin;
  Cos cos;
  TRANSFUNCTION tx =  Pow(Tx,pos*cos(dphi*i))*Pow(Ty,pos*sin(dphi*i))*Pow(Rz,dphi*i)*GeoTrf::RotateY3D(-90*GeoModelKernelUnits::deg);

  // Give this parameterization also to the readout geometry:
  if (w<firstIndexOfC) {
    m_detectorManager->setEndcapTransformField(0,tx.clone());
    m_detectorManager->setEndcapTransformField(1,tx.clone());
  }
  else {
    m_detectorManager->setEndcapTransformField(2,tx.clone());
  }
  return;

}


