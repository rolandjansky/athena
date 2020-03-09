/* Dear emacs, this is -*-c++-*- */
#ifndef _IPRDtoTrackMapExchangeTool_H_
#define _IPRDtoTrackMapExchangeTool_H_

#include "GaudiKernel/IAlgTool.h"
#include "CxxUtils/checker_macros.h"

static const InterfaceID IID_IPRDtoTrackMapExchangeTool("Trk::IPRDtoTrackMapExchangeTool", 1, 0);
namespace Trk {
   class PRDtoTrackMap;

   /** Tool to emulate PRD Association behaviour of the PRD_AssociationTool.
    * Should not be used!
   */
   class IPRDtoTrackMapExchangeTool : virtual public IAlgTool
   {
   public:
      DeclareInterfaceID(IPRDtoTrackMapExchangeTool, 1, 0);

      /** Set a new map.
       * Ownership over the map is transferred to this tool.
       */
      virtual void setPRDtoTrackMap ATLAS_NOT_THREAD_SAFE (Trk::PRDtoTrackMap *a_map) = 0;

      /** Get the currently set map or nullptr.
       */
      virtual const Trk::PRDtoTrackMap *getPRDtoTrackMap() const = 0;
   };
}
#endif
