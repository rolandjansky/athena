# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# D3PD object saving information about TrigRoiDescriptor objects.
# By default it saves the "initial RoIs", the RoIs that are given
# to the LVL2 algorithms from LVL1.

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TriggerD3PDMaker

TrigRoiDescD3PDObject = make_SGDataVector_D3PDObject( "TrigRoiDescriptorCollection",
                                                      "HLT_initialRoI",
                                                      "trig_l2_roi_",
                                                      "TrigRoiDescD3PDObject" )

TrigRoiDescD3PDObject.defineBlock( 0, "Position",
                                   TriggerD3PDMaker.TrigRoiDescriptorFillerTool,
                                   SavePosition = True )
TrigRoiDescD3PDObject.defineBlock( 1, "Size",
                                   TriggerD3PDMaker.TrigRoiDescriptorFillerTool,
                                   SaveSize = True )
TrigRoiDescD3PDObject.defineBlock( 2, "RoIInfo",
                                   TriggerD3PDMaker.TrigRoiDescriptorFillerTool,
                                   SaveRoIWord = True )
