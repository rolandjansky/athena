#include "AGILEPack/parser.h"
#include "AGILEPack/graphbuilder.h"
#include "AGILEPack/graphbuilderadapter.h"

namespace YAML
{
  void *BuildGraphOfNextDocument(Parser& parser, GraphBuilderInterface& graphBuilder)
  {
    GraphBuilderAdapter eventHandler(graphBuilder);
    if (parser.HandleNextDocument(eventHandler)) {
      return eventHandler.RootNode();
    } else {
      return NULL;
    }
  }
}
