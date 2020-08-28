/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGEFMISSINGET_PUSPLITGRID_H
#define TRIGEFMISSINGET_PUSPLITGRID_H

#include "TrigEFMissingET/PufitMultiGrid.h"

namespace HLT
{
  namespace MET
  {
    class PUSplitGrid : public PufitMultiGrid<3>
    {
    public:
      /**
       * @brief Create a new tower grid
       * @param maxEta The maximum eta range for the grid
       * @param nEtaTowers The number of eta towers
       * @param nPhiTowers The number of phi towers
       * @param displaceEta Whether to displace eta
       * @param displacePhi Whether to displace phi
       */
      PUSplitGrid(
          double maxEta,
          std::size_t nEtaTowers,
          std::size_t nPhiTowers,
          bool displaceEta = false,
          bool displacePhi = false);

      /// Construct a grid from the provided parameters
      PUSplitGrid(const GridParameters &parameters);

      /// Copy constructor
      PUSplitGrid(const PUSplitGrid &other);
    }; //> end class PUSplitGrid

    using PUSplitGridSet = PufitMultiGridSet<PUSplitGrid>;
  } // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_PUSPLITGRID_H