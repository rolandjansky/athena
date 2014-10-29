#include "AGILEPack/node.h"
#include "AGILEPack/nodebuilder.h"
#include "AGILEPack/nodeevents.h"

namespace YAML
{
    Node Clone(const Node& node)
    {
		NodeEvents events(node);
        NodeBuilder builder;
        events.Emit(builder);
        return builder.Root();
    }
}
