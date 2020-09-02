/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParameters/TrackParameters.h"
#include <iostream>
#include <memory>
#include <vector>
int
main()
{

  std::vector<std::unique_ptr<const Trk::TrackParameters>> params;
  params.emplace_back(std::make_unique<Trk::CurvilinearParameters>());
  params.emplace_back(std::make_unique<Trk::AtaCone>());
  params.emplace_back(std::make_unique<Trk::AtaCylinder>());
  params.emplace_back(std::make_unique<Trk::AtaDisc>());
  params.emplace_back(std::make_unique<Trk::Perigee>());
  params.emplace_back(std::make_unique<Trk::AtaPlane>());
  params.emplace_back(std::make_unique<Trk::AtaStraightLine>());

  for (const auto& i : params) {
    std::cout << "Dimensions: " << i->dim << " isCharged: " << i->isCharged()
              << " SurfaceType: " << i->surfaceType() << '\n';
  }
}
