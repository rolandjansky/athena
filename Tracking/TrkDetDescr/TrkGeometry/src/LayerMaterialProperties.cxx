/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGeometry/LayerMaterialProperties.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream& Trk::operator<<(MsgStream& sl,
                           const Trk::LayerMaterialProperties& lmp) {
  return lmp.dump(sl);
}

std::ostream& Trk::operator<<(std::ostream& sl,
                              const Trk::LayerMaterialProperties& lmp) {
  return lmp.dump(sl);
}

