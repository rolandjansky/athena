/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     LutCamLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  16. Dec. 2005   
//                                                     
//PURPOSE: This Loader loads the LutCam Object from realtional DBs
//         using RAL
//
//////////////////////////////////////////////////////////////////////

#include "./LutCamLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/LutCam.h"

#include <stdexcept>
#include <typeinfo>

bool
TrigConf::LutCamLoader::load( LutCam& lTarget) {

   msg() << "LutCamLoader started loading data via ID. ID = " << lTarget.id() << std::endl;
   try {
      startSession();
      msg() << "Loading LutCam " << lTarget.id() << std::endl;

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CTP_FILES");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );

      std::string condition= "L1CF_ID = :l1cfid";

      coral::AttributeList boundvars;
      boundvars.extend<int>("l1cfid");
      boundvars[0].data<int>() = lTarget.id();

      query->setCondition( condition, boundvars );      

      query->addToOutputList( "L1CF_NAME" );
      query->addToOutputList( "L1CF_VERSION" );
      query->addToOutputList( "L1CF_LUT" );
      query->addToOutputList( "L1CF_CAM" );

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         msg() << "LutCamLoader >> No such L1_CTP_Files exists " << lTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "LutCamLoader >> LutCam not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();

      std::string name = "";
      name = row["L1CF_NAME"].data<std::string>();
      int version = 0;
      version = row["L1CF_VERSION"].data<int>();
      std::string lut_str ="";
      lut_str = row["L1CF_LUT"].data<std::string>();
      std::string cam_str ="";
      cam_str = row["L1CF_CAM"].data<std::string>();

      if ( cursor.next() ) {
         msg() << "LutCamLoader >> More than one LutCam exists "   << lTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "LutCamLoader >>  LutCam not available" );
      }

      // transform strings to arrays of integers

      u_int lut_int[LutCam::ALL_LUT_SIZE];
      u_int significantBits = 8; // eight hex digits
      u_int preFix = 2;          // 0x
      u_int postFix = 1;         // termination
      u_int wordsize = (preFix + significantBits + postFix);
      for(u_int i=0; i<LutCam::ALL_LUT_SIZE; i++) {

         u_int tmp_data;

         // 	// this loop is extremely slow!!
         // 	std::sscanf(lut_str.substr(i*wordsize,wordsize).c_str(),"%x",&data);
         // 	lut_int[i]=data;

         // this loop is even slower (about 20%), but at least type safe!
         if(!convert_hex_string<u_int>(tmp_data, lut_str.substr(i*wordsize+preFix,significantBits))) {
            msg() << "LutCamLoader>> Conversion of LUT no" << i << " failed! Read string " << lut_str.substr(i*wordsize+preFix,significantBits) << std::endl;
            msg() << "TEST: substr(0,24): " << lut_str.substr(0,24) << " - substr(14,8): " << lut_str.substr(14,8) << " cc : " << (i*wordsize+preFix) << std::endl;
            commitSession();
            throw std::runtime_error( "LutCamLoader >> LUT conversion failed." );	  
         }
         lut_int[i] = tmp_data;
      }
      u_int cam_int[LutCam::ALL_CAM_SIZE] = {90, 90, 90, 90, 90, 90};
      for(u_int i=0; i<LutCam::ALL_CAM_SIZE; i++) {
	
         u_int tmp_data;

         // 	std::sscanf(cam_str.substr(i*wordsize,wordsize).c_str(),"%x",&data);
	
         if(!convert_hex_string<u_int>(tmp_data, cam_str.substr(i*wordsize+preFix,significantBits))) {
            msg() << "LutCamLoader>> Conversion of CAM no" << i << " failed!" << std::endl;
            commitSession();
            throw std::runtime_error( "LutCamLoader >> CAM conversion failed." );	  
         }
         cam_int[i]=tmp_data;
      }

      // Fill the object with data
      lTarget.setName( name );
      lTarget.setVersion( version );
      lTarget.setLut(lut_int, LutCam::ALL_LUT_SIZE);
      lTarget.setCam(cam_int, LutCam:: ALL_CAM_SIZE);

      delete query;
      commitSession();
      return true;
   }
   catch( const coral::SchemaException& e )
      {
         msg() << "LutCamLoader >> SchemaException: " 
               << e.what() << std::endl;
         commitSession();
         return false;
      }
   catch( const std::exception& e )
      {
         msg() << "LutCamLoader >> Standard C++ exception: " << e.what() << std::endl;
         commitSession();
         return false; 
      }
   catch( ... )
      {
         msg() << "LutCamLoader >> unknown C++ exception" << std::endl;
         commitSession();
         return false; 
      }
}
