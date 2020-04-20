/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigOnlineSpacePointTool_page TrigOnlineSpacePointTool Package

This package contains implementation of a spacepoint conversion tool
The TrigSpacePointConversionTool has the following functionality:

   - it converts Pixel and SCT spacepoints to a common format
   - it contains logic to reject spacepoints outside the RoI boundary
   - it can adjust the spacepoint position to account for beam shift and tilt

  main method:
  getSpacePoints(const IRoiDescriptor&, std::vector<TrigSiSpacePointBase>&, int&, int&);

@author Dmitry.Emeliyanov@cern.ch

*/  
