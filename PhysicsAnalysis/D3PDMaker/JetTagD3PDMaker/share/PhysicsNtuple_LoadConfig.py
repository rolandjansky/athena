
# same as slim with some modifications
include("JetTagD3PDMaker/SlimNtuple_LoadConfig.py")


JetTagD3PDFlags.METObjects=['MET_RefFinal']
JetTagD3PDFlags.AddMuonMuidObject = True


JetTagD3PDFlags.UseMuonEventFilter=True
JetTagD3PDFlags.UseJetEventFilter=False
