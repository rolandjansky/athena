/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BLM_GeoModel/BLM_Builder.h"
#include "BLM_GeoModel/BLM_GeometryManager.h"
#include "BLM_GeoModel/BLM_Module.h"

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"

#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GaudiKernel/SystemOfUnits.h"

//================ Constructor =================================================

InDetDD::BLM_Builder::BLM_Builder(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  //declareInterface<IBLMBuilder>(this);
  declareInterface<IGeoSubDetTool>(this);

  //default settings
  //          Z                              R                               PHI                        ROT_X                      ROT_Y                        ROT_Z                  DIAM_TRANS_X               DIAM_TRANS_Y              DIAM_TRANS_Z
  m_module0.push_back(3383.5);     m_module0.push_back(62.825);    m_module0.push_back(17);    m_module0.push_back(180);   m_module0.push_back(0);    m_module0.push_back(287);    m_module0.push_back(0);    m_module0.push_back(0);    m_module0.push_back(0);
  m_moduleI.push_back(3383.5);     m_moduleI.push_back(62.825);    m_moduleI.push_back(45);    m_moduleI.push_back(180);   m_moduleI.push_back(0);    m_moduleI.push_back(315);    m_moduleI.push_back(0);    m_moduleI.push_back(0);    m_moduleI.push_back(0);
  m_moduleII.push_back(3383.5);    m_moduleII.push_back(62.825);   m_moduleII.push_back(73);   m_moduleII.push_back(180);  m_moduleII.push_back(0);   m_moduleII.push_back(343);   m_moduleII.push_back(0);   m_moduleII.push_back(0);   m_moduleII.push_back(0);
  m_moduleIII.push_back(3383.5);   m_moduleIII.push_back(62.825);  m_moduleIII.push_back(197); m_moduleIII.push_back(180); m_moduleIII.push_back(0);  m_moduleIII.push_back(107);  m_moduleIII.push_back(0);  m_moduleIII.push_back(0);  m_moduleIII.push_back(0);
  m_moduleIV.push_back(3383.5);    m_moduleIV.push_back(62.825);   m_moduleIV.push_back(225);  m_moduleIV.push_back(180);  m_moduleIV.push_back(0);   m_moduleIV.push_back(135);   m_moduleIV.push_back(0);   m_moduleIV.push_back(0);   m_moduleIV.push_back(0);
  m_moduleV.push_back(3383.5);     m_moduleV.push_back(62.825);    m_moduleV.push_back(253);   m_moduleV.push_back(180);   m_moduleV.push_back(0);    m_moduleV.push_back(163);    m_moduleV.push_back(0);    m_moduleV.push_back(0);    m_moduleV.push_back(0);
  m_moduleVI.push_back(-3383.5);   m_moduleVI.push_back(62.825);   m_moduleVI.push_back(17);   m_moduleVI.push_back(0);    m_moduleVI.push_back(0);   m_moduleVI.push_back(107);   m_moduleVI.push_back(0);   m_moduleVI.push_back(0);   m_moduleVI.push_back(0);
  m_moduleVII.push_back(-3383.5);  m_moduleVII.push_back(62.825);  m_moduleVII.push_back(45);  m_moduleVII.push_back(0);   m_moduleVII.push_back(0);  m_moduleVII.push_back(135);  m_moduleVII.push_back(0);  m_moduleVII.push_back(0);  m_moduleVII.push_back(0);
  m_moduleVIII.push_back(-3383.5); m_moduleVIII.push_back(62.825); m_moduleVIII.push_back(73); m_moduleVIII.push_back(0);  m_moduleVIII.push_back(0); m_moduleVIII.push_back(163); m_moduleVIII.push_back(0); m_moduleVIII.push_back(0); m_moduleVIII.push_back(0);
  m_moduleIX.push_back(-3383.5);   m_moduleIX.push_back(62.825);   m_moduleIX.push_back(197);  m_moduleIX.push_back(0);    m_moduleIX.push_back(0);   m_moduleIX.push_back(287);   m_moduleIX.push_back(0);   m_moduleIX.push_back(0);   m_moduleIX.push_back(0);
  m_moduleX.push_back(-3383.5);    m_moduleX.push_back(62.825);    m_moduleX.push_back(225);   m_moduleX.push_back(0);     m_moduleX.push_back(0);    m_moduleX.push_back(315);    m_moduleX.push_back(0);    m_moduleX.push_back(0);    m_moduleX.push_back(0);
  m_moduleXI.push_back(-3383.5);   m_moduleXI.push_back(62.825);   m_moduleXI.push_back(253);  m_moduleXI.push_back(0);    m_moduleXI.push_back(0);   m_moduleXI.push_back(343);   m_moduleXI.push_back(0);   m_moduleXI.push_back(0);   m_moduleXI.push_back(0);

  //  template for property decalration
  declareProperty("Modul0", m_module0);
  declareProperty("ModulI", m_moduleI);
  declareProperty("ModulII", m_moduleII);
  declareProperty("ModulIII", m_moduleIII);
  declareProperty("ModulIV", m_moduleIV);
  declareProperty("ModulV", m_moduleV);
  declareProperty("ModulVI", m_moduleVI);
  declareProperty("ModulVII", m_moduleVII);
  declareProperty("ModulVIII", m_moduleVIII);
  declareProperty("ModulIX", m_moduleIX);
  declareProperty("ModulX", m_moduleX);
  declareProperty("ModulXI", m_moduleXI);
  declareProperty("ModulesOn", m_moduleon=0xFFFF);
  declareProperty("BLMon", m_blmon=true);
  declareProperty("DBparameters", m_BDparameters=true);
}

//================ Destructor =================================================

InDetDD::BLM_Builder::~BLM_Builder()
{}


//================ Initialisation =================================================

StatusCode InDetDD::BLM_Builder::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  ATH_MSG_INFO("BLMBuilder initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDetDD::BLM_Builder::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//================ Actual Work =================================================

StatusCode InDetDD::BLM_Builder::build(GeoVPhysVol* pv)
{
  if(!m_blmon) {
    ATH_MSG_INFO("BLM disabled.");  
    return StatusCode::SUCCESS;
  }

  // Unfortunately add is not part of the abstract interface of GeoVPhysVol so we have to dynamic cast
  // to the concrete class.
  GeoFullPhysVol* Phys = dynamic_cast<GeoFullPhysVol *>(pv);
  if (!Phys) {
    ATH_MSG_ERROR("Parent volume is not a GeoFullPhysVol as expected.");  
    return StatusCode::FAILURE;
  }

//  ATH_MSG_INFO("BLMBuilder building..."); //commented out by D.Dobos on request by M.Elsing
 
  const StoredMaterialManager * materialManager;
  const AbsMaterialManager * mat_mgr = 0;
  if (StatusCode::SUCCESS == detStore()->retrieve(materialManager, std::string("MATERIALS"))) {
    mat_mgr = materialManager;
  }

  //create geometry manager
  BLM_GeometryManager* manager = new BLM_GeometryManager;

  StatusCode sc;
  if(m_BDparameters)
    {
      DecodeVersionKey versionKey("InnerDetector");

      // Issue error if AUTO.
      if (versionKey.tag() == "AUTO")
	{
	  ATH_MSG_ERROR("AUTO Atlas version. Please select a version.");
	}

      ATH_MSG_INFO("Building BLM with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node());

      IRDBAccessSvc *accessSvc;
      sc = service("RDBAccessSvc",accessSvc);
      if (sc.isFailure())
	{
	  ATH_MSG_FATAL("Could not locate RDBAccessSvc");
	  delete manager;
	  return StatusCode::FAILURE;
	}

      // Print the BLM version tag:
      std::string BLMVersionTag;
      BLMVersionTag = accessSvc->getChildTag("BLM", versionKey.tag(), versionKey.node());
      ATH_MSG_INFO("BLM Version: " << BLMVersionTag <<  "  Package Version: " << PACKAGE_VERSION);

      // Check if version is empty. If so, then the BLM cannot be built. This may or may not be intentional. We just issue an INFO message.
      if (BLMVersionTag.empty())
  	{
	  ATH_MSG_INFO("No BLM Version. BLM will not be built.");
	  delete manager;
	  return StatusCode::SUCCESS;
     	}

      IRDBRecordset_ptr DBmodul = accessSvc->getRecordsetPtr("BLMModule", versionKey.tag(), versionKey.node());
      //DBmodul = accessSvc->getRecordset("BLMModule", "InnerDetector-DC3-Dev", "InnerDetector");

      ATH_MSG_DEBUG(" --> Number of records fetched = " << DBmodul->size());

      //loop over all the records and putting them in module_property

      unsigned int ind;
      long moduleNo;
      //std::vector<double>* module_property = NULL;
      for(ind = 0; ind < DBmodul->size(); ind++)
      {
	  const IRDBRecord* rec = (*DBmodul)[ind];
		//temp
 	  moduleNo = rec->getLong("MODULE_ID");
 	  //check if this module is suposed to be builded

 	  unsigned int mask = (1 << moduleNo);
 	  if((mask & m_moduleon) != mask)
 	    {
 	      //this module is not sopoused to be built
 	      continue;
 	    }

          std::vector<double> module_property;
 	  module_property.push_back(rec->getFloat("Z"));
 	  module_property.push_back(rec->getFloat("R"));
 	  module_property.push_back(rec->getFloat("PHI"));
 	  module_property.push_back(rec->getFloat("ROT_X"));
 	  module_property.push_back(rec->getFloat("ROT_Y"));
 	  module_property.push_back(rec->getFloat("ROT_Z"));
 	  module_property.push_back(rec->getFloat("DIAM_X"));
 	  module_property.push_back(rec->getFloat("DIAM_Y"));
 	  module_property.push_back(rec->getFloat("DIAM_Z"));

 	  //set the BCM_GeometryManeger
 	  manager->ModuleOn(moduleNo);
 	  manager->Module(moduleNo)->Set(moduleNo, &module_property);
 	}
      ATH_MSG_DEBUG(" --> Number succesfully read from DB");
    }
  else
    {
      ATH_MSG_DEBUG(" --> BLM parameters via jobOptions or default");
      //parameters via jobOptions
      int moduleNo;
      const std::vector<double>* module_property = NULL;
      for(moduleNo = 0; moduleNo < 12; moduleNo++)
	{
	  unsigned int mask = (1 << moduleNo);
	  if((mask & m_moduleon) != mask)
	    {
	      //this module is not sopoused to be built
	      continue;
	    }
	  if(moduleNo == 0)      module_property = &m_module0;
	  else if(moduleNo == 1) module_property = &m_moduleI;
	  else if(moduleNo == 2) module_property = &m_moduleII;
	  else if(moduleNo == 3) module_property = &m_moduleIII;
	  else if(moduleNo == 4) module_property = &m_moduleIV;
	  else if(moduleNo == 5) module_property = &m_moduleV;
	  else if(moduleNo == 6) module_property = &m_moduleVI;
	  else if(moduleNo == 7) module_property = &m_moduleVII;
	  else if(moduleNo == 8) module_property = &m_moduleVIII;
	  else if(moduleNo == 9) module_property = &m_moduleIX;
	  else if(moduleNo == 10) module_property = &m_moduleX;
	  else if(moduleNo == 11) module_property = &m_moduleXI;

	  //set the BLM_GeometryManeger
	  manager->ModuleOn(moduleNo);
	  manager->Module(moduleNo)->Set(moduleNo, module_property);
	}
    }

  //we are now adding eight BLM modules
  BLM_Module blm;

  for(int i=0; i<12; i++)
    {
      //see if this module is to be built
      if(!(manager->IsModuleOn(i)))
	continue;

      //the module should be build
      BLM_ModuleParameters* parameters = manager->Module(i);

      //setting transformation
      GeoTrf::Translation3D pos(parameters->R()*cos(parameters->Phi()*Gaudi::Units::deg), parameters->R()*sin(parameters->Phi()*Gaudi::Units::deg), parameters->Z());
      GeoTrf::Transform3D rm = GeoTrf::RotateZ3D(parameters->Rotation_Z()*Gaudi::Units::deg)
	* GeoTrf::RotateY3D(parameters->Rotation_Y()*Gaudi::Units::deg)
	* GeoTrf::RotateX3D(parameters->Rotation_X()*Gaudi::Units::deg);
      GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      xform->ref();
      //building module
      int k=i+222;

      GeoNameTag* tag = new GeoNameTag("BLM Module");
      tag->ref();
      if (mat_mgr){
        GeoVPhysVol* blmModPhys = blm.Build(mat_mgr, parameters, (msgLvl(MSG::INFO) ? &msg(MSG::INFO) : NULL));
        Phys->add(tag);
        Phys->add(new GeoIdentifierTag(k));
        Phys->add(xform);
        Phys->add(blmModPhys);
        ATH_MSG_DEBUG(" --> BUILD MODULE: " << i);
      }
      tag->unref();
      xform->unref();
    }

  //save Geometry_manager in storegate
  ATH_MSG_DEBUG("Registering BLM_GeometryManager.");
  sc = detStore()->record(manager, "BLMParameters");

  if (sc.isFailure())
    {
      ATH_MSG_INFO("Could not register BLM_GeometryManager");
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

// Register callback function on ConDB object
// Empty for now
StatusCode InDetDD::BLM_Builder::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

// Callback function itself
// Empty for now
StatusCode InDetDD::BLM_Builder::align(IOVSVC_CALLBACK_ARGS)
{
  // Return Failure since no function has been registered
  return StatusCode::FAILURE;
}

//============================================================================================

