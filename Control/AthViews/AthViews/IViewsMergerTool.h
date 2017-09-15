#ifndef AthViews_IViewsMergerTool_h
#define AthViews_IViewsMergerTool_h

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "AthViews/View.h"

static const InterfaceID IID_IViewsMergerTool( "IViewsMergerTool", 1, 0 ); 

class IViewsMergerTool : public virtual IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IViewsMergerTool; }
  
  /**
   * @brief processes views to create merged collection collection of the content
   * 
   **/
  virtual StatusCode merge( const EventContext& context, const std::vector<SG::View*>& ) const = 0;
  
  virtual ~IViewsMergerTool() {}
};

#endif
