/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_METADATATOOLEXAMPLE_H
#define CPANALYSISEXAMPLES_METADATATOOLEXAMPLE_H 1

///
///This is an example of a tool that configures
///itself using in-file metadata
///If your tool will behave differently depending on metadata (like AFII vs full sum)
///Then you should write your tool like this
///



#include "AsgTools/AsgMetadataTool.h"

#include "CPAnalysisExamples/IMetadataToolExample.h"

namespace CP {

class MetadataToolExample: public asg::AsgMetadataTool, public virtual IMetadataToolExample { 
 public: 
   //constructor for athena can be created using special macro
   //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
   ASG_TOOL_CLASS( MetadataToolExample , IMetadataToolExample )
   //add another constructor for non-athena use cases
   MetadataToolExample( const std::string& name );

   /// Initialize is required by AsgTool base class
   virtual StatusCode  initialize() override;

   /// This method is called at the start of each input file
   virtual StatusCode beginInputFile() override; 

   /// This method called at end of each input file
   virtual StatusCode endInputFile() override;

   /// This method is called at beginning of each event
   virtual StatusCode beginEvent() override;


   virtual void print() const override;

 private: 
   bool m_isConfigured = false;
   bool m_isMC = false;
   bool m_isFastSim = false;
}; 

}

#endif //> !CPANALYSISEXAMPLES_METADATATOOLEXAMPLE_H
