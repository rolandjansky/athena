#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# --------------------------------------------------------------------------------------------- #
# Configuration of secondary vertex finding for charged tracks orginating from the primary      #
# vertex and decaying in inner detector (optmized for decays beyond the 3rd pixel layer         #
# Requires two input track containers                                                           #
# --------------------------------------------------------------------------------------------- #

IDTRKVALIDStream.AddItem("xAOD::VertexContainer#*")
IDTRKVALIDStream.AddItem("xAOD::VertexAuxContainer#*")
