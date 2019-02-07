/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_GeoModel/BCM_Builder.h"
#include "BCM_GeoModel/BCM_GeometryManager.h"
#include "BCM_GeoModel/BCM_Module.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GaudiKernel/SystemOfUnits.h"

//================ Constructor =================================================

InDetDD::BCM_Builder::BCM_Builder(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p)
{
  //declareInterface<IBCMBuilder>(this);
  declareInterface<IGeoSubDetTool>(this);

  //default settings
  //                 TRANS_X                        TRANS_Y                        TRANS_Z                          ROT_X                       ROT_Y                      ROT_Z                   DIMENSION_Z               DIAM_TRANS_Y                   DIAM_TRANS_Z             DIAM_DIAM_X                 DIAM_DIAM_Y                  DIAM_DIAM_Z                LAMEL_OFFSET_H                   LAMEL_OFFSET_I                 LAMEL_OFFSET_J
  m_module0.push_back(95.744);   m_module0.push_back(0);         m_module0.push_back(1882.4);    m_module0.push_back(0);     m_module0.push_back(45);    m_module0.push_back(0);     m_module0.push_back(0.4);   m_module0.push_back(0);   m_module0.push_back(-59.785);   m_module0.push_back(0.2);   m_module0.push_back(0);   m_module0.push_back(-0.4);   m_module0.push_back(34.860);   m_module0.push_back(29.509);   m_module0.push_back(20.205);   //F410
  m_moduleI.push_back(0);        m_moduleI.push_back(95.744);    m_moduleI.push_back(1882.4);    m_moduleI.push_back(0);     m_moduleI.push_back(45);    m_moduleI.push_back(90);    m_moduleI.push_back(0.4);   m_moduleI.push_back(0);   m_moduleI.push_back(-59.785);   m_moduleI.push_back(0.2);   m_moduleI.push_back(0);   m_moduleI.push_back(-0.4);   m_moduleI.push_back(34.860);   m_moduleI.push_back(29.509);   m_moduleI.push_back(20.205);   //F405
  m_moduleII.push_back(-95.744); m_moduleII.push_back(0);        m_moduleII.push_back(1882.4);   m_moduleII.push_back(0);    m_moduleII.push_back(45);   m_moduleII.push_back(180);  m_moduleII.push_back(0.4);  m_moduleII.push_back(0);  m_moduleII.push_back(-59.785);  m_moduleII.push_back(0.2);  m_moduleII.push_back(0);  m_moduleII.push_back(-0.4);  m_moduleII.push_back(34.860);  m_moduleII.push_back(29.509);  m_moduleII.push_back(20.205);  //F413
  m_moduleIII.push_back(0);      m_moduleIII.push_back(-95.744); m_moduleIII.push_back(1882.4);  m_moduleIII.push_back(0);   m_moduleIII.push_back(45);  m_moduleIII.push_back(270); m_moduleIII.push_back(0.4); m_moduleIII.push_back(0); m_moduleIII.push_back(-59.785); m_moduleIII.push_back(0.2); m_moduleIII.push_back(0); m_moduleIII.push_back(-0.4); m_moduleIII.push_back(34.860); m_moduleIII.push_back(29.509); m_moduleIII.push_back(20.205); //F404
  m_moduleIV.push_back(95.744);  m_moduleIV.push_back(0);        m_moduleIV.push_back(-1882.4);  m_moduleIV.push_back(180);  m_moduleIV.push_back(-45);  m_moduleIV.push_back(0);    m_moduleIV.push_back(1.4);  m_moduleIV.push_back(0);  m_moduleIV.push_back(-59.785);  m_moduleIV.push_back(0.2);  m_moduleIV.push_back(0);  m_moduleIV.push_back(-0.4);  m_moduleIV.push_back(34.860);  m_moduleIV.push_back(29.509);  m_moduleIV.push_back(20.205);  //F424
  m_moduleV.push_back(0);        m_moduleV.push_back(95.744);    m_moduleV.push_back(-1882.4);   m_moduleV.push_back(180);   m_moduleV.push_back(-45);   m_moduleV.push_back(90);    m_moduleV.push_back(1.4);   m_moduleV.push_back(0);   m_moduleV.push_back(-59.785);   m_moduleV.push_back(0.2);   m_moduleV.push_back(0);   m_moduleV.push_back(-0.4);   m_moduleV.push_back(34.860);   m_moduleV.push_back(29.509);   m_moduleV.push_back(20.205);   //F420
  m_moduleVI.push_back(-95.744); m_moduleVI.push_back(0);        m_moduleVI.push_back(-1882.4);  m_moduleVI.push_back(180);  m_moduleVI.push_back(-45);  m_moduleVI.push_back(180);  m_moduleVI.push_back(1.4);  m_moduleVI.push_back(0);  m_moduleVI.push_back(-59.785);  m_moduleVI.push_back(0.2);  m_moduleVI.push_back(0);  m_moduleVI.push_back(-0.4);  m_moduleVI.push_back(34.860);  m_moduleVI.push_back(29.509);  m_moduleVI.push_back(20.205);  //F422
  m_moduleVII.push_back(0);      m_moduleVII.push_back(-95.744); m_moduleVII.push_back(-1882.4); m_moduleVII.push_back(180); m_moduleVII.push_back(-45); m_moduleVII.push_back(270); m_moduleVII.push_back(0.4); m_moduleVII.push_back(0); m_moduleVII.push_back(-59.785); m_moduleVII.push_back(0.2); m_moduleVII.push_back(0); m_moduleVII.push_back(-0.4); m_moduleVII.push_back(34.860); m_moduleVII.push_back(29.509); m_moduleVII.push_back(20.205); //F408

  //  template for property decalration
  declareProperty("Modul0", m_module0);
  declareProperty("ModulI", m_moduleI);
  declareProperty("ModulII", m_moduleII);
  declareProperty("ModulIII", m_moduleIII);
  declareProperty("ModulIV", m_moduleIV);
  declareProperty("ModulV", m_moduleV);
  declareProperty("ModulVI", m_moduleVI);
  declareProperty("ModulVII", m_moduleVII);
  declareProperty("ModulesOn", m_moduleon=255);
  declareProperty("BCMon", m_bcmon=true);
  declareProperty("DBparameters", m_BDparameters=true);
}

//================ Destructor =================================================

InDetDD::BCM_Builder::~BCM_Builder()
{}


//================ Initialisation =================================================

StatusCode InDetDD::BCM_Builder::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  ATH_MSG_INFO("BCMBuilder initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDetDD::BCM_Builder::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//================ Actual Work =================================================

StatusCode InDetDD::BCM_Builder::build(GeoVPhysVol* pv)
{
  if(!m_bcmon) {
    ATH_MSG_INFO("BCM disabled.");
    return StatusCode::SUCCESS;
  }

  // Unfortunately add is not part of the abstract interface of GeoVPhysVol so we have to dynamic cast
  // to the concrete class.
  GeoFullPhysVol* Phys = dynamic_cast<GeoFullPhysVol *>(pv);
  if (!Phys) {
    ATH_MSG_ERROR("Parent volume is not a GeoFullPhysVol as expected.");  
    return StatusCode::FAILURE;
  }

//  ATH_MSG_INFO("BCMBuilder building...");  //commented out by D.Dobos on request by M.Elsing
  
  const StoredMaterialManager * materialManager;
  const AbsMaterialManager * mat_mgr = 0;
  if (StatusCode::SUCCESS == detStore()->retrieve(materialManager, std::string("MATERIALS"))) {
    mat_mgr = materialManager;
  }

  //create geometry manager
  BCM_GeometryManager* manager = new BCM_GeometryManager;

  StatusCode sc;

  if(m_BDparameters)
    {
      DecodeVersionKey versionKey("InnerDetector");
      
      // Issue error if AUTO.
      if (versionKey.tag() == "AUTO")
	{
	  ATH_MSG_ERROR("AUTO Atlas version. Please select a version.");
	}
      
      ATH_MSG_INFO("Building BCM with Version Tag: " << versionKey.tag() << " at Node: " << versionKey.node());
      
      IRDBAccessSvc *accessSvc;
      sc = service("RDBAccessSvc",accessSvc);
      if (sc.isFailure())
	{
	  ATH_MSG_FATAL("Could not locate RDBAccessSvc");
	  delete manager;
	  return StatusCode::FAILURE; 
	}  
      
      // Print the BCM version tag:
      std::string BCMVersionTag;
      BCMVersionTag = accessSvc->getChildTag("BCM", versionKey.tag(), versionKey.node());
      ATH_MSG_INFO("BCM Version: " << BCMVersionTag <<  "  Package Version: " << PACKAGE_VERSION);
      
      // Check if version is empty. If so, then the BCM cannot be built. This may or may not be intentional. We just issue an INFO message. 
      if (BCMVersionTag.empty())
  	{ 
	  ATH_MSG_INFO("No BCM Version. BCM will not be built.");
	  delete manager;
	  return StatusCode::SUCCESS; 
     	}
      
      IRDBRecordset_ptr DBmodul = accessSvc->getRecordsetPtr("BCMModule", versionKey.tag(), versionKey.node());
      //DBmodul = accessSvc->getRecordset("BCMModule", "InnerDetector-DC3-Dev", "InnerDetector");
      
      ATH_MSG_DEBUG(" --> Number of records fetched = " << DBmodul->size());
      
      unsigned int ind;
      long moduleNo;
      //std::vector<double>* module_property = NULL;
      for(ind = 0; ind < DBmodul->size(); ind++)
	{
	  const IRDBRecord* rec = (*DBmodul)[ind];
	  
	  moduleNo = rec->getLong("MODULE_ID");
	  //check if this module is suposed to be builded

	  unsigned int mask = (1 << moduleNo);
	  if((mask & m_moduleon) != mask)
	    {
	      //this module is not sopoused to be built
	      continue;
	    }

	  std::vector<double> module_property;
	  module_property.push_back(rec->getFloat("TRANS_X"));
	  module_property.push_back(rec->getFloat("TRANS_Y"));
	  module_property.push_back(rec->getFloat("TRANS_Z"));
	  module_property.push_back(rec->getFloat("ROT_X"));
	  module_property.push_back(rec->getFloat("ROT_Y"));
	  module_property.push_back(rec->getFloat("ROT_Z"));
	  module_property.push_back(rec->getFloat("DIMESION_Z"));
	  module_property.push_back(rec->getFloat("DIAM_TRANS_Y"));
	  module_property.push_back(rec->getFloat("DIAM_TRANS_Z"));
	  module_property.push_back(rec->getFloat("DIAM_DIAM_X"));
	  module_property.push_back(rec->getFloat("DIAM_DAIM_Y"));
	  module_property.push_back(rec->getFloat("DIAM_DIAM_Z"));
	  module_property.push_back(rec->getFloat("OFF_H"));
	  module_property.push_back(rec->getFloat("OFF_I"));
	  module_property.push_back(rec->getFloat("OFF_J"));/**/
	  
	  //set the BCM_GeometryManeger
	  manager->ModuleOn(moduleNo);
	  manager->Module(moduleNo)->Set(moduleNo, &module_property);
	}
      ATH_MSG_DEBUG(" --> Number succesfully read from DB");
    } 
  else
    {
      //parameters via jobOptions
      int moduleNo;
      const std::vector<double>* module_property = NULL;
      for(moduleNo = 0; moduleNo < 8; moduleNo++)
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
	  	  
	  //set the BCM_GeometryManeger
	  manager->ModuleOn(moduleNo);
	  manager->Module(moduleNo)->Set(moduleNo, module_property);
	}
    }
  
  //we are now adding eight BCM modules
  BCM_Module bcm;
  
  for(int i=0; i<8; i++)
    {
      //see if this module is to be built
      if(!(manager->IsModuleOn(i)))
		continue;
      
      //the module should be build
      BCM_ModuleParameters* parameters = manager->Module(i);
      
      //setting transformation
      GeoTrf::Translation3D pos(parameters->Position_X(), parameters->Position_Y(), parameters->Position_Z());
      GeoTrf::Transform3D rm = GeoTrf::RotateZ3D(parameters->Rotation_Z()*Gaudi::Units::deg)
	* GeoTrf::RotateY3D(parameters->Rotation_Y()*Gaudi::Units::deg)
	* GeoTrf::RotateX3D(parameters->Rotation_X()*Gaudi::Units::deg)
	* GeoTrf::RotateZ3D(-90.*Gaudi::Units::deg)
	* GeoTrf::RotateY3D(-90.*Gaudi::Units::deg);
      GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(pos*rm));
      xform->ref();
      ATH_MSG_DEBUG(" --> Module " << i << " build!");

      //building module
      int k=i+951;
      
      GeoNameTag* tag = new GeoNameTag("BCM Module"); 
      tag->ref();
      if (mat_mgr){
        GeoVPhysVol* bcmModPhys = bcm.Build(mat_mgr, parameters, (msgLvl(MSG::INFO) ? &msg(MSG::INFO) : NULL));
        Phys->add(tag);
        Phys->add(new GeoIdentifierTag(k));
        Phys->add(xform);
        Phys->add(bcmModPhys);	
      }
      tag->unref();
      xform->unref();  
    }
  
  ATH_MSG_DEBUG("Registering BCM_GeometryManager."); 
  sc = detStore()->record(manager, "BCMParameters");
  
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Could not register BCM_GeometryManager");
      return StatusCode::FAILURE ;
    }
  return StatusCode::SUCCESS; 
  
}


// Register callback function on ConDB object
// Empty for now
StatusCode InDetDD::BCM_Builder::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}

// Callback function itself
// Empty for now
StatusCode InDetDD::BCM_Builder::align(IOVSVC_CALLBACK_ARGS)
{
  // Return Failure since no function has been registered
  return StatusCode::FAILURE;
}



//============================================================================================

