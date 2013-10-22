/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNMODULELIST_H
#define TRKALIGNEVENT_ALIGNMODULELIST_H

/**
   @file AlignModuleList.h
   @typedef AlignModuleList

   @brief AlignModuleList is a list of AlignModules.  An AlignModule is a grouping of TrkDetElementBase objects, grouped according to the type of alignment, groupings being specific to detector type.  
   For example, for MDT alignment:
   1) L1 alignment uses an AlignModule for each endcap and one for the barrel for a total of 3 AlignModule elements in the AlignModuleList.
   2) L3 alignment uses a single MDT chamber for each AlignModule.
   3) L2 and other types not yet implemented for MDT alignment.
   See detector-specific implementation of IGeometryManagerTool::createAlignModuleList() to see how implemented.

   There should be one instance of AlignModuleList for each detector type, held by
   AlignModuleTool.  Specific detectors can create their own AlignModuleLists, inheriting
   from AlignModuleList, but it is not necessary.

   @author Robert Harrington <roberth@bu.edu>
   @date 10/14/2008
*/

namespace Trk {

  class AlignModule;

  /** Type of AlignModuleList (see description above). */
  enum AlignModuleListType { L0=0,L1,L2,L3,
			     L1_5,L2_5,
			     ModulesOnTrack
  };
  
  typedef std::vector<AlignModule*> AlignModuleList;
  
}

#endif // TRKALIGNEVENT_ALIGNMODULELIST_H
