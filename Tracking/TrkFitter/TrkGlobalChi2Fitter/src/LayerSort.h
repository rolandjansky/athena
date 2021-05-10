/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>
namespace Trk{
  class Layer;
}

namespace GXF{
  class LayerSort{
  public:
    typedef std::pair<const Trk::Layer *, const Trk::Layer *> LayerPtrPair;
    bool operator() (const LayerPtrPair & one, const LayerPtrPair & two) const;
  };
  
  //
  class LayerSort2 {
  public:
    bool operator() (const Trk::Layer * one, const Trk::Layer * two) const;
  };
}
  
  
  