# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: EnergySumROID3PDObject.py 504811 2012-06-10 13:09:39Z krasznaa $
#
# D3PD object saving the LVL1 EnergySum RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TriggerD3PDMaker

EnergySumROID3PDObject = \
      make_SG_D3PDObject( "LVL1_ROI", "LVL1_ROI", "trig_L1_esum_",
                          "EnergySumROID3PDObject",
                          default_getterClass = \
                          TriggerD3PDMaker.EnergySum_ROIGetterTool,
                          default_allowMissing = True )


EnergySumROID3PDObject.defineBlock( 0, "ThrInfo",
                                    TriggerD3PDMaker.EnergySum_ROIFillerTool,
                                    WriteThrInfo = True )

EnergySumROID3PDObject.defineBlock( 1, "DecodedInfo",
                                    TriggerD3PDMaker.EnergySum_ROIFillerTool,
                                    WriteDecodedInfo = True )

EnergySumROID3PDObject.defineBlock( 2, "RoIWord",
                                    TriggerD3PDMaker.EnergySum_ROIFillerTool,
                                    WriteRoIWord = True )
