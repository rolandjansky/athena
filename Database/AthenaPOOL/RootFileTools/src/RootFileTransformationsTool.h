/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTFILETOOLS_ROOTFILETRANSFORMATIONSTOOL_H
#define ROOTFILETOOLS_ROOTFILETRANSFORMATIONSTOOL_H
/** @file RootFileTransformationsTool.cxx
 *  @brief Tool to call RootFileTransformations to optimize output files
 *  @author Gerhard Brandt <gbrandt@cern.ch>
 *  $Id:  $
 **/

#include "RootFileTools/IRootFileTransformationsTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

/** @class RootFileTransformationsTool
 *  @brief Tool to call RootFileTransformations to optimize output files
 **/
class RootFileTransformationsTool : virtual public IRootFileTransformationsTool, 
                                    virtual public AthAlgTool {

public:
   /// Constructor
   RootFileTransformationsTool(const std::string& type,
	   const std::string& name,
	   const IInterface* parent);
   /// Destructor
   virtual ~RootFileTransformationsTool();

   /// Initialize
   virtual StatusCode initialize();
   /// Finalize
   virtual StatusCode finalize();

   // Reorder Baskets
   virtual StatusCode reorderBaskets(std::string, std::string);
   
   // Resize Baskets ( filename, algorithm, names of trees to exlude, memory per tree, memory (in kilobytes!), min zip level)
   virtual StatusCode resizeBaskets(std::string, std::string, std::string, std::string, int, float);

private:
};

#endif
