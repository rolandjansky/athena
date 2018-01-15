///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisHelper.h 
// Header file for class AthAnalysisHelper
// Methods that are useful for athena-based analysis will go here
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ATHANALYSISBASECOMPS_ATHANALYSISHELPER_H
#define ATHANALYSISBASECOMPS_ATHANALYSISHELPER_H 1

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Algorithm.h"

#include "AthContainers/AuxElement.h"

#include "GaudiKernel/IAppMgrUI.h"


#include "GaudiKernel/ToolHandle.h"

class AthAnalysisHelper { //thought about being a namespace but went for static methods instead, in case I want private data members in future

public:
    AthAnalysisHelper(); //need a constructor to link the dictionary library with the implementation library

   static const std::string UNDEFINED;  //used in getProperty
   
   ///initGaudi method starts the gaudi ApplicationMgr ready for working with all the components
   static IAppMgrUI* initGaudi(const char* options = "");

   
   ///helper method for adding a property to the JobOptionsSvc
   ///to list all the properties in the catalogue, do: AthAnalysisHelper::dumpJobOptionProperties()
   template<typename W> static StatusCode addPropertyToCatalogue( const std::string& name , const std::string& property, const W& value, bool override=true) {
     ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AthAnalysisHelper");
     if(joSvc.retrieve().isFailure()) return StatusCode::FAILURE;
     if(!override) {
      //check if property already defined. If so, then print a warning and return failure
      std::string pValue = getProperty( name, property );
      if( pValue != UNDEFINED) {
        std::cout << "WARNING: " << name << "." << property << " already defined as " << pValue << " ... ignoring " <<  Gaudi::Utils::toString ( value ) << std::endl;
        return StatusCode::FAILURE;
      }
     }
     StatusCode result = joSvc->addPropertyToCatalogue( name , StringProperty( property , Gaudi::Utils::toString ( value ) ) );
     if(joSvc.release().isFailure()) return StatusCode::FAILURE;
     return result;
   }
   

   //alias for above
   template<typename W> static StatusCode setProperty( const std::string& name , const std::string& property, const W& value, bool override=true) {
    return addPropertyToCatalogue(name,property,value,override);
   }
   
   ///non-template method, for use in joboptions
   ///current usage would be:
   /// ... usual joboptions ...
   /// theApp.setup() #triggers population of property catalogue
   /// import ROOT
   /// ROOT.AAH.setProperty( "ToolSvc.MyTool" , "Property" , str(value) )
   static StatusCode setProperty( const std::string& name , const std::string& property, const std::string& value, bool override=true) { 
    return addPropertyToCatalogue( name, property, value, override );
   } //set a property for a client in the catalogue


   ///helper method for setting a property on a tool before retrieving it
   ///uses the toolhandle to work out what to register the joboptionsvc
   ///Usage examples:
   ///  ToolHandle<IMyTool> tool("MyTool/toolName");
   ///  CHECK( AthAnalysisHelper::setProperty( tool, "IntegerProperty", 5) );
   ///  CHECK( AthAnalysisHelper::setProperty( tool, "StringProperty", "hello") );
   ///  CHECK( AthAnalysisHelper::setProperty( tool, "StringArrayProperty", std::vector<std::string>({"a","b","c"}) ) );
   ///  CHECK( AthAnalysisHelper::setProperty( tool, "PublicToolHandleProperty", anotherToolHandle) );
   ///  CHECK( AthAnalysisHelper::setProperty( tool, "PrivateToolHandleName.SubToolIntegerProperty", 4) );
   ///The last example assumes the 'MyTool' class has declared a ToolHandle("SubTool/PrivateToolHandleName",this)
   ///and 'SubTool' class has an integer property called 'SubToolIntegerProperty' declared

  template<class W, typename = typename std::enable_if<!std::is_base_of<GaudiHandleBase,W>::value && 
						       !std::is_base_of<GaudiHandleArrayBase,W>::value>::type> 
   static StatusCode setProperty(const GaudiHandleBase& toolHandle, const std::string& property, const W& value, bool override=true) {
      if(toolHandle.empty()) {
          std::cout << "ERROR: Empty toolHandle passed to AthAnalysisHelper::setProperty" << std::endl;
          return StatusCode::FAILURE;
      }
      std::string fullName = toolHandle.parentName() + "." + toolHandle.name();
      std::string thePropertyName(property);
      //if the property contains any "." then strip the last bit as the property name
      std::string::size_type dotLocation = thePropertyName.find_last_of('.');
      if(dotLocation != std::string::npos) {
         fullName += "." + thePropertyName.substr(0,dotLocation);
         thePropertyName = thePropertyName.substr(dotLocation+1,thePropertyName.length()-dotLocation);
      }
      //check if the tool already exists, if so then we are too late!!
      if( toolExists( fullName ) ) {
        std::cout << "ERROR: Cannot setProperty on a tool that is already initialized" << std::endl;
        return StatusCode::FAILURE;
      }
      //tool not existing, ok so add property to catalogue
      return addPropertyToCatalogue(fullName, thePropertyName, value, override);
   }
   
   ///Partial template specialization for ToolHandles and ToolHandleArrays ... strips parent name from tool name, for setting private handles on
   static StatusCode setProperty(const GaudiHandleBase& toolHandle, const std::string& property, const GaudiHandleBase& value, bool override=true) {
      std::string subToolName(value.name());
      size_t start_pos = subToolName.find(toolHandle.name()+".");
      if(start_pos!=std::string::npos) { subToolName.replace( start_pos, toolHandle.name().length()+1, "" ); }
      std::string typeAndName = value.type(); if(!subToolName.empty()) typeAndName += "/"+subToolName;
      return setProperty( toolHandle, property, typeAndName, override );
   }
   static StatusCode setProperty(const GaudiHandleBase& toolHandle, const std::string& property, const GaudiHandleArrayBase& value, bool override=true) {
      return setProperty( toolHandle, property, value.typesAndNames(), override );
   }

   ///setProperty on any tool, even when initialized
   template<typename W> static StatusCode setProperty(IAlgTool* tool, const std::string& property, const W& value) {
      AlgTool* algtool = dynamic_cast<AlgTool*>(tool);
      if(!algtool) {
         std::cout << "ERROR: Tool is not an algtool. Cannot AthAnalysisHelper::setProperty on it" << std::endl;
         return StatusCode::FAILURE;
      }
      return algtool->setProperty(property, value);
   }

   ///setProperty on an alg, even when initialized
   template<typename W> static StatusCode setProperty(IAlgorithm* alg, const std::string& property, const W& value) {
      Algorithm* theAlg = dynamic_cast<Algorithm*>(alg);
      if(!theAlg) {
         std::cout << "ERROR: alg is not an Algorithm. Cannot AthAnalysisHelper::setProperty on it" << std::endl;
         return StatusCode::FAILURE;
      }
      return theAlg->setProperty(property, value);
   }

  template<typename T> static std::string toString(const T& value) { return Gaudi::Utils::toString( value ); }
  static std::string toString(const std::string& value) { return value; } //gaudi's toString puts extra quote marks around things like "['b']" .. should probably stop that..
  static std::string toString(const char* value) { return value; } //gaudi's toString puts extra quote marks around things like "['b']" .. should probably stop that..


   ///setProperty for services ... will allow setProperty on already-existing services
   template<typename T, typename W> static StatusCode setProperty(const ServiceHandle<T>& serviceHandle, const std::string& property, const W& value) {
      if(serviceHandle.isSet()) {
         return dynamic_cast<Service&>(*serviceHandle).setProperty(property,toString ( value ));
      }
      std::string fullName = serviceHandle.name();
      std::string thePropertyName(property);
      //if the property contains any "." then strip the last bit as the property name
      std::string::size_type dotLocation = thePropertyName.find_last_of('.');
      if(dotLocation != std::string::npos) {
         fullName += "." + thePropertyName.substr(0,dotLocation);
         thePropertyName = thePropertyName.substr(dotLocation+1,thePropertyName.length()-dotLocation);
      }

      //check if the service already exists
      if(Gaudi::svcLocator()->existsService(serviceHandle.name())) {
         //set property on the service directly
         return dynamic_cast<Service&>(*serviceHandle).setProperty(property,toString ( value ));
      } 

      //service not existing, ok so add property to catalogue
      return addPropertyToCatalogue(fullName, thePropertyName, value);
   }

   ///Create a tool using the gaudi factory methods. This creates tools that wont be known to the ToolSvc
   ///User is responsible for deleting the tool
   ///Parent is optional, allows your created tool to inherit OutputLevel properties from the parent
   ///If not parent specified, the ToolSvc will be used as the parent
   ///Please Note: The preferred method of creating tools is with a ToolHandle
   ///Usage: 
   /// IMyTool* tool = AthAnalysisHelper::createTool<IMyTool>("ToolType/ToolName");
   /// CHECK( AthAnalysisHelper::setProperty( tool, "Property", value ) );
   /// CHECK( tool->initialize() );
   ///Preferred:
   /// ToolHandle<IMyTool> tool("ToolType/ToolName");
   /// CHECK( AthAnalysisHelper::setProperty( tool, "Property", value ) );
   /// CHECK( tool.retrieve() );
   template<typename W> static W* createTool(const std::string& typeAndName, INamedInterface* parent = 0) {
      std::string type = typeAndName; std::string name = typeAndName;
      if(type.find("/")!=std::string::npos) { type = type.substr(0,type.find("/")); name = name.substr(name.find("/")+1,name.length()); }
      if(parent==0) {
	//use ToolSvc as parent
	parent = Gaudi::svcLocator()->service( "ToolSvc" );
      }
      IAlgTool* algtool = AlgTool::Factory::create(type,type,name,parent);
      algtool->addRef(); //important to increment the reference count so that Gaudi Garbage collection wont delete alg ahead of time 
      W* out = dynamic_cast<W*>(algtool);
      if(!out && algtool) {
         std::cout << "ERROR: Tool of type " << type << " does not implement the interface " << System::typeinfoName(typeid(W)) << std::endl;
         delete algtool;
         return 0; 
      }
      return out;
   }
   static IAlgTool* createTool(const std::string& typeAndName, INamedInterface* parent = 0) {
     return createTool<IAlgTool>(typeAndName,parent);
   }


   //equivalent method for creating an algorithm ... always returns an IAlgorithm though, so not templated
   static IAlgorithm* createAlgorithm(const std::string& typeAndName) {
      std::string type = typeAndName; std::string name = typeAndName;
      if(type.find("/")!=std::string::npos) { type = type.substr(0,type.find("/")); name = name.substr(name.find("/")+1,name.length()); }
      IAlgorithm* out = Algorithm::Factory::create(type,name,Gaudi::svcLocator());
      out->addRef(); //important to increment the reference count so that Gaudi Garbage collection wont delete alg ahead of time 
      return out;
   }
   
   
   ///check if tool already exists. FullName = Parent.Name
   static bool toolExists( const std::string& fullName );
   static bool toolExists(const GaudiHandleBase& toolHandle) { return toolExists( toolHandle.parentName() + "." + toolHandle.name() ); }

  
  ///method that always returns as a string
  ///you can use from, e.g, pyROOT with
  ///evt = ROOT.POOL.TEvent()
  ///...
  ///ROOT.AAH.retrieveMetadata("/Folder","key",evt.inputMetaStore())
  static std::string retrieveMetadata(const std::string& folder, const std::string& key, ServiceHandle<StoreGateSvc>& inputMetaStore) throw(std::exception) {
    std::string out("");
    StatusCode result = retrieveMetadata(folder,key,out,inputMetaStore);
    if(result.isFailure()) { std::cout << "ERROR: Could not retrieve IOVMetadata with folder " << folder << " and key " << key << std::endl; }
    return out;
  }


   ///retrieve metadata from the input metadata storegate. Use checkMetaSG.py to see the 'folder' and 'key' values available
   ///always takes the first CondAttrListCollection (aka IOV) and the first channel number present in that IOV
   template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out) throw(std::exception) {
      ServiceHandle<StoreGateSvc> inputMetaStore("StoreGateSvc/InputMetaDataStore", "AthAnalysisHelper");
      if(inputMetaStore.retrieve().isFailure()) return StatusCode::FAILURE; //must remember to release
      StatusCode result = retrieveMetadata(folder,key,out,inputMetaStore);
      if(inputMetaStore.release().isFailure()) return StatusCode::FAILURE;
      return result;
   }
   
   
   
   ///implemenation where you pass it a particular store instead
  template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out, ServiceHandle<StoreGateSvc>& inputMetaStore) throw(std::exception) {
      const IOVMetaDataContainer* cont = 0;
      if( inputMetaStore->retrieve(cont,folder).isFailure()) return StatusCode::FAILURE;
   
      //payload is a collection of condattrlistcollections
      //only look a the first one, assuming it exists, and within that only look at the first channel;
      if(cont->payloadContainer()->size()>0 && cont->payloadContainer()->at(0)->size()>0) {
         //just try to retrieve the requested key from the attributelist - we will let it throw the coral::AttributeListException (inherits from std::exception) if it fails
         //if the typeName is std::string, we will try to use the gaudi parsers to parse it
         //otherwise we try to do a straight assignment 
	 try {
	   const coral::Attribute& attr = cont->payloadContainer()->at(0)->attributeList(cont->payloadContainer()->at(0)->chanNum(0))[key];
	 
	   if(attr.specification().typeName()=="string") {
	     if(Gaudi::Parsers::parse(out,attr.data<std::string>()).isFailure()) return StatusCode::FAILURE;
	   } else { //do a straight conversion, and just hope its ok (FIXME: should probably do a check of typeid(T) vs typeName)
	     out = attr.data<T>();
	   }
	 } catch(...) {
	   //anything that goes wrong is taken to be a failure
	   return StatusCode::FAILURE;
	 }
   
         return StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
   }

  ///retrieve metadata, for a specified IOVTime and a specific channel, unless the channel is -1, in which case we take the first available channel
  ///channels have to be unsigned int, so can use -1 to signal 'take whatever first channel is (it isn't always 0)'
  template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out, ServiceHandle<StoreGateSvc>& inputMetaStore, const IOVTime& time, int channel=-1) throw(std::exception) {
      const IOVMetaDataContainer* cont = 0;
      if( inputMetaStore->retrieve(cont,folder).isFailure()) return StatusCode::FAILURE;
   
      //payload is a collection of condattrlistcollections
      //have to find first one that the time lies in
      auto cond = cont->payloadContainer()->find(time);
      if(cond == cont->payloadContainer()->end()) { return StatusCode::FAILURE; }

      //get the first channel number, if required 
      if(channel<0) channel = (*cond)->chanNum(0);

      //get the channel pair.. checks if it exists...
      auto attrlist = (*cond)->chanAttrListPair(channel);
      if(attrlist == (*cond)->end()) { return StatusCode::FAILURE; }
      
      
      //just try to retrieve the requested key from the attributelist - we will let it throw the coral::AttributeListException (inherits from std::exception) if it fails
      //if the typeName is std::string, we will try to use the gaudi parsers to parse it
      //otherwise we try to do a straight assignment 
      try {
	const coral::Attribute& attr = attrlist->second[key];
	if(attr.specification().typeName()=="string") {
	  if(Gaudi::Parsers::parse(out,attr.data<std::string>()).isFailure()) return StatusCode::FAILURE;
	} else { //do a straight conversion, and just hope its ok (FIXME: should probably do a check of typeid(T) vs typeName)
	  out = attr.data<T>();
	}
      }
      catch(...) {
	return StatusCode::FAILURE;
      }
   
      return StatusCode::SUCCESS;
   }

  template<typename T> static StatusCode retrieveMetadata(const std::string& folder, const std::string& key, T& out, IOVTime time, int channel=-1) throw(std::exception) {
      ServiceHandle<StoreGateSvc> inputMetaStore("StoreGateSvc/InputMetaDataStore", "AthAnalysisHelper");
      if(inputMetaStore.retrieve().isFailure()) return StatusCode::FAILURE; //must remember to release
      StatusCode result = retrieveMetadata(folder,key,out,inputMetaStore,time,channel);
      if(inputMetaStore.release().isFailure()) return StatusCode::FAILURE;
      return result;
   }


   ///Dump the properties from joboptionsvc of clients with names beginning with given string. If string is blank, will print all clients
   ///Example usage:
   ///athena -i myJobOptions.py
   ///athena> theApp.setup()
   ///athena> import ROOT
   ///athena> ROOT.AAH.dumpJobOptionProperties()
   ///This will display all the joboption properties declared at the start of your job
   static void dumpJobOptionProperties(const std::string& client="");
   inline static void dumpProperties(const std::string& client="") { dumpJobOptionProperties(client); } //list properties of client present in catalogue

   ///Check catalogue for property of specified client
   ///returns AAH::UNDEFINED in case of no property value available (i.e. component will take the default property value)
   ///otherwise returns the property value in its string representation
   static std::string getProperty(const std::string& client, const std::string& property); //get property of client present in catalogue, or return AAH::UNDEFINED


   ///Print the aux variables of an xAOD object (aux element)
   ///An alternative to this method is the 'xAOD::dump' method
   static void printAuxElement(const SG::AuxElement& ae);
   static void dumpProperties(const SG::AuxElement& ae) { printAuxElement(ae); } //list properties of an xAOD object


   ///Dump the properties of an IProperty
   //these aren't necessarily the same as what is in the JobOptionsSvc
  static void dumpProperties(const IProperty& component); //list properties of an existing component (may not match what is in the catalogue)
    

   template<typename T> static void dumpProperties(const GaudiHandle<T>& handle) {
    if(!handle.isSet()) {std::cout << "Please retrieve handle before dumping properties" << std::endl; return;}
    return dumpProperties(dynamic_cast<const IProperty&>(*handle));
  }




   ///we keep a static handle to the joboptionsvc, since it's very useful
   ///can e.g. do: AAH::joSvc->readOptions("myJob.opts","$JOBOPTSEARCHPATH")
   static ServiceHandle<IJobOptionsSvc> joSvc; 


}; //AthAnalysisHelper class


class AAH : public AthAnalysisHelper {
  
};



#endif
