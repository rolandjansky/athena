###############################################################
#
# jobOptions file to test the TBCaloCoordinate tool
#
#  cannot run standalone, because needs TB conditions
# 
#          ==> has to be included in RecExTB
#==============================================================

# read Table positions
include ("TBCaloGeometry/TBCaloGeometry_H8_joboptions.py")

# abc : just for tests !!!!!! This is realy bad !!!!!
ToolSvc = Service( "ToolSvc" )
#ToolSvc.LArDetDescrTool.emb_phi_shift= 0.
#ToolSvc.LArDetDescrTool.emb_theta_shift= 0.
#ToolSvc.LArDetDescrTool.emb_psi_shift= 0.
#ToolSvc.LArDetDescrTool.emb_x_shift= 1276.
#ToolSvc.LArDetDescrTool.emb_y_shift= 0.
#ToolSvc.LArDetDescrTool.emb_z_shift= 0.

theApp.Dlls += [ 
"TestLArDetDescr"
]
theApp.TopAlg += [
"TestCaloGeom" 
]
