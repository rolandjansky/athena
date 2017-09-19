
//abstract interface

#ifndef __IFTK_DUPLICATETRACKREMOVAL_TOOL_H__
#define __IFTK_DUPLICATETRACKREMOVAL_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

static const InterfaceID IID_IFTK_DuplicateTrackRemovalTool("IFTK_DuplicateTrackRemovalTool",1,0);

class IFTK_DuplicateTrackRemovalTool : virtual public IAlgTool { 

 public:
  /** other standard AlgTool methods */
  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_IFTK_DuplicateTrackRemovalTool; }  	

  virtual FTK_RawTrackContainer* removeDuplicates(const FTK_RawTrackContainer* trks) = 0;

 private:

};

#endif
