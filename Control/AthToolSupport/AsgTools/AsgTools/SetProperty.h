///////////////////////// -*- C++ -*- /////////////////////////////
// SetProperty.h 
// Methods that are useful for working with AsgTools
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ASGTOOLS_SETPROPERTY_H
#define ASGTOOLS_SETPROPERTY_H

namespace asg {

  /// Helper method for calling setProperty on an interface class
  /// Usage: asg::setProperty( myTool , "PropertyName" , value )
  /// Returns: StatusCode
  template<typename W> static StatusCode setProperty(IAsgTool* tool, const std::string& property, W&& value) {
          AsgTool* asgtool = dynamic_cast<AsgTool*>(tool);
          if(!asgtool) return StatusCode::FAILURE;
          return asgtool->setProperty(property, value);
  }
  template<typename W> static StatusCode setProperty(IAsgTool& tool, const std::string& property, W&& value) {
       return asg::setProperty( &tool , property, value );
  }
  ///specializations for unique_ptr
  template<typename T,typename W> static StatusCode setProperty(std::unique_ptr<T>& tool, const std::string& property, W&& value) {
       return asg::setProperty( tool.get() , property, value );
  }
  
  ///specialization for setting a toolhandle property ... don't set the pointer directly, set the typeAndName instead
  ///strips out parent tool name so that this will allow setting of private tools
  template<typename W> static StatusCode setProperty(IAsgTool* tool, const std::string& property, ToolHandle<W>&& value) {
          if(!tool) return StatusCode::FAILURE;
          std::string subToolName(value.name());
          size_t start_pos = subToolName.find(tool->name()+".");
          if(start_pos!=std::string::npos) { subToolName.replace( start_pos, tool->name().length()+1, "" ); }
          return asg::setProperty(tool, property, value.type()+"/"+subToolName);
  }
  

} //asg namespace

#endif



