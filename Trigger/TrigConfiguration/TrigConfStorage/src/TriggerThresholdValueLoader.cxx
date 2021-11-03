/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     TriggerThresholdValueLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set data type)
//
//////////////////////////////////////////////////////////////////////

#include "boost/lexical_cast.hpp"

#include "./TriggerThresholdValueLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/HelperFunctions.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;
  
bool
TrigConf::TriggerThresholdValueLoader::load( TriggerThresholdValue& ttvTarget ) {
   TRG_MSG_DEBUG("TriggerThresholdValueLoader loading thresholdvalue with ID = " << ttvTarget.id());
   L1DataDef def;

   string name = "";
   string type = "";
   int version = 0;
   float ptcut = 0;
   float priority=0;
   int phimin = 0;
   int phimax = 0;
   int etamin = 0;
   int etamax = 0;
   string emisolation = "";
   string hadisolation = "";
   string hadveto = "";
   int window = 0;
      
   try {
      //unsigned int schema = triggerDBSchemaVersion();

      startSession();

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_TRIGGER_THRESHOLD_VALUE");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<long>("ttvId");
      std::string cond= "L1TTV_ID = :ttvId";
      bindList[0].data<long>() = ttvTarget.id();	
      query->setCondition( cond, bindList );
        
      //Define the data types
      coral::AttributeList attList;	
      attList.extend<std::string>( "L1TTV_NAME" );
      attList.extend<int>( "L1TTV_VERSION" );
      attList.extend<std::string>( "L1TTV_TYPE" );
      attList.extend<std::string>( "L1TTV_PT_CUT" );
      attList.extend<int>( "L1TTV_ETA_MIN" );
      attList.extend<int>( "L1TTV_ETA_MAX" );
      attList.extend<int>( "L1TTV_PHI_MIN" );
      attList.extend<int>( "L1TTV_PHI_MAX" );
      attList.extend<std::string>( "L1TTV_EM_ISOLATION" );
      attList.extend<std::string>( "L1TTV_HAD_ISOLATION" );
      attList.extend<std::string>( "L1TTV_HAD_VETO" );
      attList.extend<int>( "L1TTV_WINDOW" );
      attList.extend<std::string>( "L1TTV_PRIORITY" );
      query->defineOutput(attList);
      query->addToOutputList( "L1TTV_NAME" );
      query->addToOutputList( "L1TTV_VERSION" ); 
      query->addToOutputList( "L1TTV_TYPE" ); 
      query->addToOutputList( "L1TTV_PT_CUT" ); 
      query->addToOutputList( "L1TTV_ETA_MIN" ); 
      query->addToOutputList( "L1TTV_ETA_MAX" );
      query->addToOutputList( "L1TTV_PHI_MIN" );
      query->addToOutputList( "L1TTV_PHI_MAX" );
      query->addToOutputList( "L1TTV_EM_ISOLATION" ); 
      query->addToOutputList( "L1TTV_HAD_ISOLATION" ); 
      query->addToOutputList( "L1TTV_HAD_VETO" ); 
      query->addToOutputList( "L1TTV_WINDOW" ); 
      query->addToOutputList( "L1TTV_PRIORITY" ); 

      coral::ICursor& cursor = query->execute();
      if ( !cursor.next()) {
         msg() << "TriggerThresholdValueLoader >> No such TriggerThresholdValue exists " 
               << ttvTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "TriggerThresholdValueLoader >> TriggerThresholdValue not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();

      name = row["L1TTV_NAME"].data<std::string>();
      version = row["L1TTV_VERSION"].data<int>();
      type = row["L1TTV_TYPE"].data<std::string>();
      ptcut = boost::lexical_cast<float,std::string>(row["L1TTV_PT_CUT"].data<std::string>());
      etamin = row["L1TTV_ETA_MIN"].data<int>();
      etamax = row["L1TTV_ETA_MAX"].data<int>();
      phimin = row["L1TTV_PHI_MIN"].data<int>();
      phimax = row["L1TTV_PHI_MAX"].data<int>();
      window = row["L1TTV_WINDOW"].data<int>();
      priority = boost::lexical_cast<float,std::string>(row["L1TTV_PRIORITY"].data<std::string>());
      emisolation = row["L1TTV_EM_ISOLATION"].data<std::string>();
      hadisolation = row["L1TTV_HAD_ISOLATION"].data<std::string>();
      hadveto = row["L1TTV_HAD_VETO"].data<std::string>();


      if (cursor.next()) {
         msg() << "TriggerThresholdValueLoader >> More than one TriggerThresholdValue exists " 
               << ttvTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "TriggerThresholdValueLoader >> TriggerThresholdValue not available" );
      }

      // Fill the value common for all threshold_value
      ttvTarget.setName( name );
      ttvTarget.setVersion( version );
      ttvTarget.setType( type );
      ttvTarget.setPtcut( ptcut );
      ttvTarget.setPriority(priority);
      delete query;
      commitSession();
      //msg() << name << std::endl;

   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Caught coral exception: " << e.what() );
      throw; 
   }
   catch( const std::exception& e ) {
      TRG_MSG_ERROR("Caught standard exception: " << e.what() );
      throw; 
   }

   try {
      ClusterThresholdValue& ctvTarget = dynamic_cast<ClusterThresholdValue&>(ttvTarget);
      if ( type != def.emType() && type != def.tauType()) {
         msg() << "TriggerThresholdValueLoader >> No type match for ttv_id = " 
               << ctvTarget.id() << " " << type << std::endl;
         throw std::runtime_error( "TriggerThresholdValueLoader >> ClusterThresholdValue not available" );
      }

      cout << "ISO " << emisolation << "   " << hadisolation << "   " << hadveto << endl;

      if(hadveto=="USEISOBITS" || boost::lexical_cast<int,std::string>(hadveto)==99 ) {
         ctvTarget.setIsolationMask( TrigConf::bin2uint(emisolation) );
         ctvTarget.setUseIsolationMask();
      } else {
         ctvTarget.setEmIsolation( boost::lexical_cast<float,std::string>(emisolation) );
         ctvTarget.setHadIsolation( boost::lexical_cast<float,std::string>(hadisolation) );
         ctvTarget.setHadVeto( boost::lexical_cast<float,std::string>(hadveto) );
         ctvTarget.setUseIsolationMask( false );
      }


      ctvTarget.setPhiMin( phimin);
      ctvTarget.setPhiMax( phimax);
      ctvTarget.setEtaMin( etamin );
      ctvTarget.setEtaMax( etamax );
   } catch (std::bad_cast& ex) {  }
    
   //is it a jet_threshold_value?
   try {
      JetThresholdValue& jtvTarget = dynamic_cast<JetThresholdValue&>(ttvTarget);
      
      if (type!=def.jetType() && type!=def.jbType() && type!=def.jfType()) {
         msg() << "TriggerThresholdValueLoader >> No type match for ttv_id = " 
               << jtvTarget.id() << type << std::endl;
         throw std::runtime_error( "TriggerThresholdValueLoader >> TriggerThresholdValue not available" );
      }
      jtvTarget.setPhiMin( phimin );
      jtvTarget.setPhiMax( phimax );
      jtvTarget.setEtaMin( etamin );
      jtvTarget.setEtaMax( etamax );
      jtvTarget.setWindow( window );
   } catch (std::bad_cast& ex) {}

   return true;
}
