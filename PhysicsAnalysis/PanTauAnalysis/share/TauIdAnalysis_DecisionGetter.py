from tauRec.tauRecFlags import jobproperties as tauRecJP

if tauRecJP.tauRecFlags.doPanTau():
    
    
    #Inform the other PanTau JOs that we're using the tauRec seeds as input
    from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
    PSB_Flags.Config_UseTauRecAsSeeds.set_Value_and_Lock(True)
    
    include("PanTauAnalysis/JobOptions_Main_PanTau.py")
# end check for PanTau execution


#end of file
