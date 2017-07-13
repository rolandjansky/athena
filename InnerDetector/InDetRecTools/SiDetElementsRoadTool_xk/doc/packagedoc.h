/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiDetElementsRoadTool_xk_page SiDetElementsRoadTool_xk
@author Igor.Gavrilenko@cern.ch
@section SiDetElementsRoadTool_xk_SiDetElementsRoadTool_xkIntro Introduction

This tool is concrete implementation for ISiDetElementsRoadMaker
and is devoted for list<const InDetDD::SiDetectorElement*>&) production. 

During initialization step this tool produce a map all Pixels and SCT detetector elements ordered
in set logical layers sorted in radius order for barrel and Z coordinate order for endcap. In each layer
all detector elements are sorted in azimuthal angle order.

The input information for this tool  is track parameters. Using globalPositions of IPropagator tool 
algorithm get list global positions of the posible trajectory. This list is input for fast comparison 
trajectory with map of detector elements.
Algorith iterate through this list using straight line extrapolation between neighbouring points
and compare position of the trajectory with position detector elements in the map. If trajectory can cross some detector
element with some tolerance algorithm save this element to the list. All detector elments in the list sorted
in propagation order and first detector element is closest to beam axis.    
     
This algorithm uses IMagneticFieldTool and IPropagator  tools.


*/
