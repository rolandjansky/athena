/**
 * @file  IOVDbMetaDataTool_entries.cxx
 * 
 * @brief Declaration of entry points for Gaudi tool
 * 
 * @author Antoine Pérus <perus@lal.in2p3.fr>
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbMetaDataTools/src/components/IOVDbMetaDataTool_entries.cxx,v 1.1 2007-06-18 09:52:00 schaffer Exp $
 */
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../IOVDbMetaDataTool.h"

DECLARE_TOOL_FACTORY( IOVDbMetaDataTool )

DECLARE_FACTORY_ENTRIES(IOVDbMetaDataTool) {
  DECLARE_TOOL( IOVDbMetaDataTool )
}
