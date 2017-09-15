/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonAmbiTrackSelectionTool_page MuonAmbiTrackSelectionTool Package

@author Niels van Eldik <niels.van.eldik@cern.ch>

@section MuonAmbiTrackSelectionTool_MuonAmbiTrackSelectionToolIntro Introduction

This tool is a helper tool to the ambiguity processing. 
It decides which hits remain on a track and which hits have to be removed, because they are already used on other tracks. 
Shared hits are allowed within certain constraints. For the book keeping which hits are alreasy used, a PRD_AssociationTool
is used. Some inline methods of the selection tool allow the client to use some functionality of the association tool via the
selection tool.




*/
