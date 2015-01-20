/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GLOBALEVENTTAGTOOL_H 
#define GLOBALEVENTTAGTOOL_H 

/*****************************************************************************
Name    : GlobalEventTagTool.h
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : Tool to buid the Global Event Tags
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"

#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

//#include "CoolKernel/IDatabase.h"

#include "EventTagUtils/EventInfoTagTool.h"

#include <map>
#include <vector>
#include <string>

/** Interface ID for GlobalEventTagTool */  
static const InterfaceID IID_GlobalEventTagTool("GlobalEventTagTool", 1, 0);

//class GlobalEventTagTool : public AlgTool {
//class GlobalEventTagTool : public EventInfoTagTool, virtual public IIncidentListener {
class GlobalEventTagTool : public EventInfoTagTool {

public:
  
  /** Standard Constructor */
  GlobalEventTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_GlobalEventTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::vector<std::pair<std::string,AthenaAttributeType> >& attrMap);
  virtual StatusCode attributeSpecification(std::vector<AthenaAttributeSpecification>& attrMap);
  virtual StatusCode execute(TagFragmentCollection& eventTagCol);  
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~GlobalEventTagTool( );

private:

   /** the various components to build their own fragments of tag */
   StatusCode caloTag    (TagFragmentCollection& eventTagCol);
   StatusCode trackTag   (TagFragmentCollection& eventTagCol);
   StatusCode vertexTag  (TagFragmentCollection& eventTagCol);

private:

  /** Properties */
  std::string m_caloClusterContainerName;
  std::string m_trackContainerName;
  std::string m_vertexContainerName;

  bool m_mc;
  bool m_includeVertexFlag;
  int  m_vertexTrackCut;
  
  bool m_includeRecoTime;
  bool m_recoInclPers;
  
};

#endif // GLOBALEVENTTAGTOOL_H



