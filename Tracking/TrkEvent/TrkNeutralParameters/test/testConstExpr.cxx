/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkNeutralParameters/NeutralParameters.h"
#include <iostream>
#include <memory>
#include <vector>
int
main()
{

  std::vector<std::unique_ptr<const Trk::NeutralParameters>> params;
  params.emplace_back(std::make_unique<Trk::NeutralCurvilinearParameters>());
  params.emplace_back(std::make_unique<Trk::NeutralAtaCone>());
  params.emplace_back(std::make_unique<Trk::NeutralAtaCylinder>());
  params.emplace_back(std::make_unique<Trk::NeutralAtaDisc>());
  params.emplace_back(std::make_unique<Trk::NeutralPerigee>());
  params.emplace_back(std::make_unique<Trk::NeutralAtaPlane>());
  params.emplace_back(std::make_unique<Trk::NeutralAtaStraightLine>());

  for (const auto& i : params) {
     std::cout << "Dimensions: " << i->dim << " isCharged: " << i->isCharged()
              << " SurfaceType: " << i->surfaceType() << '\n';
  }
}
