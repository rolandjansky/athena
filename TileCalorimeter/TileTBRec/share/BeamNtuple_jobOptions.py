#-------------------------------
# Add beam elements to combined ntuple
#-------------------------------

# if not loaded - load these DLLs
theApp.Dlls += [ "TileRecUtils", "TileTBRec" ]

theApp.TopAlg += ["TileTBNtuple/BeamCrate"]
BeamCrate = Algorithm( "BeamCrate" )

BeamCrate.NTupleLoc = "/FILE1/TB"
BeamCrate.NTupleID = "tree"

BeamCrate.drawerList = [ ]

BeamCrate.beamFragList = ["0x001","0x002","0x003","0x007",
                          "0x010","0x011","0x012","0x013","0x014" ]

BeamCrate.CompleteNtuple = FALSE
BeamCrate.CommitNtuple = FALSE

BeamCrate.BN2X1 =  0.522
BeamCrate.BN2X2 =  0.175 
BeamCrate.BN2Y1 =  1.44  
BeamCrate.BN2Y2 =  0.173
BeamCrate.BN2Z  =  1.0  

BeamCrate.BN1X1 = -0.309
BeamCrate.BN1X2 =  0.173 
BeamCrate.BN1Y1 = -0.561
BeamCrate.BN1Y2 =  0.175 
BeamCrate.BN1Z  =  1.0  

BeamCrate.BC0X1 = -0.111
BeamCrate.BC0X2 =  0.185
BeamCrate.BC0Y1 =  0.028
BeamCrate.BC0Y2 =  0.181
BeamCrate.BC0Z  =  1.0  

BeamCrate.BC1X1 = -5.56 
BeamCrate.BC1X2 =  0.183
BeamCrate.BC1Y1 = -3.04 
BeamCrate.BC1Y2 =  0.180 
BeamCrate.BC1Z  =  15530.65  

BeamCrate.BC2X1 = -2.22 
BeamCrate.BC2X2 =  0.195  
BeamCrate.BC2Y1 =  1.91 
BeamCrate.BC2Y2 =  0.188  
BeamCrate.BC2Z  =  11227.9  

