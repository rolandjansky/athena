include.block( "MissingETPerformance/MissingETEventSelector_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     MissingETEventSelector as ConfiguredMissingETEventSelector
MissingETEventSelector = ConfiguredMissingETEventSelector(

    ElectronContainer      = 'ElectronAODCollection',
    MuonContainerKey       = 'StacoMuonCollection',
    JetKey                 = 'AntiKt6LCTopoJets',

    JetPtCut               = 7.*GeV,
    JetElecOverlapDeltaR   = 0.2,
    MinNJets               = 0,
    MaxNJets               = 100,
    ElecPtCut              = 15.*GeV,
    MaxElecIsolCone        = 10.*GeV,
    MaxMuonIsolCone        = 10.*GeV,
    isEM                   = 0,
    MuonPtCut              = 20.*GeV,
    minRefFinalMET         = 20.*GeV,
    minNonIntMET           = 20.*GeV,
    minInvMass             = 71.*GeV,
    maxInvMass             = 111.*GeV,
    minEventEMfraction     = 0.1,
    minPerJetEMfraction    = 0.05,

    #SelectionType         = "hasMinPerJetEMfraction",
    #SelectionType         = "hasMinEventEMfraction",
    #SelectionType         = "hasOneElectron",
    #SelectionType         = "hasOneJet",
    #SelectionType         = "hasNJets",
    #SelectionType         = "hasMinMETRefFinal",
    #SelectionType         = "hasMinNonIntMET",
    #SelectionType         = "hasOneMuon",
    #SelectionType          = "hasZboson_mumu",
    SelectionType          = "hasZboson_ee",
    #SelectionType         = "passesMBTSCut", #need to set flags below as well
    
    #selectedMuonKey only relevant for "hasZboson_mumu" selection
    selectedMuonKey        = "SelectedMuons",
    #selectedElectronKey only relevant for "hasZboson_ee" selection
    selectedElectronKey    = "SelectedElectrons",

    #here you can choose to do an AND of several selections
    #SelectionType         = "ANDedSelection",
    #selectionList only relevant for SelectionType         = "ANDedSelection"
    selectionList         = [  "hasMinMETRefFinal",
                               "hasOneElectron",
                               "hasNJets",
                            ],

    )

if MissingETEventSelector.SelectionType == "passesMBTSCut":
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DetDescrVersion = "ATLAS-GEO-08-00-02"
    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.DataSource.set_data()
    include( "RecExCond/AllDet_detDescr.py")

ToolSvc += MissingETEventSelector
print      MissingETEventSelector
