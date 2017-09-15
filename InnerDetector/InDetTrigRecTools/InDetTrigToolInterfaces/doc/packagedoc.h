/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetTrigToolInterfaces_page InDetTrigToolInterfaces Package
@author Jiri.Masik@cern.ch

@section InDetTrigToolInterfaces_InDetTrigToolInterfacesIntro Introduction

This package is an interface package defining API of the tools listed
below. It allows separation of the interface and implementation as
foreseen by the Gaudi ramework. Clients should refer to these
interface classes rather than using their full implementation in the
ToolHandle etc. This policy reduces dependencies between packages,
risk of cascaded build failures and results in a cleaner and faster
build. It also allows smaller libraries of the component type.

@section InDetTrigToolInterfaces_InDetTrigToolInterfacesOverview Overview

Currently the package defines two interfaces

<ul>

<li> InDet::ITrigRawDataProviderTool

The interface for the raw data decoding tools of the Pixel, SCT and
TRT. It defines the form of the InDet::ITrigRawDataProviderTool::decode method for decoding data in a
region specified by the TrigRoiDescriptor and RoI halfwidth in eta and
phi. The implementation can be found in the package
InDetTrigRawDataProvider.

</li>

<li> InDet::ITrigSCT_SpacePointTool

Clients use InDet::ITrigSCT_SpacePointTool::addSCT_SpacePoints method
which fills the output SpacePointCollection from the collection of SCT
clusters. The implementation can be found in the package
SiTrigSpacePointFormation.

</li>

*/
