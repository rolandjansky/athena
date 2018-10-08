[example0] #####################################################################
    const char* triggers2015 = 
        "mu20_iloose_L1MU15_OR_mu50"
        "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose";
    myTool.setProperty("TriggerCombination2015", triggers2015).ignore();
    const char* triggers2016and2017 = 
        "mu26_ivarmedium_OR_mu50"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016and2017).ignore();
    myTool.setProperty("TriggerCombination2017", triggers2016and2017).ignore();

[example1] #####################################################################
    const char* triggers2015 = 
        "mu20_iloose_L1MU15_OR_mu50"
        "|| mu18_mu8noL1"
        "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"
        "|| 2e12_lhloose_L12EM10VH";
    myTool.setProperty("TriggerCombination2015", triggers2015).ignore();
    const char* triggers2016 = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016).ignore();
    const char* triggers2017 = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e24_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2017", triggers2017).ignore();
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
        
[example2] #####################################################################
    /// Fill a map with the trigger combinations
    std::map<string, string> triggers;
    triggers["2015"] = 
        "mu20_iloose_L1MU15_OR_mu50"
        "|| mu18_mu8noL1"
        "|| e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"
        "|| 2e12_lhloose_L12EM10VH";
    triggers["2016:A-2016:D"] = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e17_lhvloose_nod0";
    triggers["303638-311481"] = /// (change triggers for fun in 2016 periods E-L)
        "mu22_mu8noL1"
        "|| 2e17_lhvloose_nod0";
    triggers["2017"] = 
        "mu26_ivarmedium_OR_mu50"
        "|| mu22_mu8noL1"
        "|| e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| 2e24_lhvloose_nod0";
    myTool.setProperty("TriggerCombination", triggers).ignore();
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
    /// Change for fun the pT thresholds used by the tool for mu18 and mu22
    std::map<string, string> thresholds;
    thresholds["mu18"] = "25e3";
    thresholds["mu22"] = "25e3";
    myTool.setProperty("OverrideThresholds", thresholds).ignore();

[example3com] ##################################################################
    myTool.setProperty("ListOfLegsPerTool", legsPerTool).ignore();
    myTool.setProperty("ListOfTagsPerTool", tagsPerTool).ignore();
    myTool.setProperty("ListOfLegsPerTag", legsPerTag).ignore();
        
[example3a] ####################################################################
    const char* triggers2015 = "2e12_lhloose_L12EM10VH";
    myTool.setProperty("TriggerCombination2015", triggers2015).ignore();
    myTool.setProperty("LeptonTagDecorations", "Signal").ignore();
+[example3com]
        
[example3b3c] ##################################################################
    const char* triggers2015 = 
        "e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"
        "|| 2e12_lhloose_L12EM10VH";
    myTool.setProperty("TriggerCombination2015", triggers2015).ignore();
    myTool.setProperty("LeptonTagDecorations", "Signal").ignore();
+[example3com]
 
[example3d] ####################################################################
    const char* triggers2016 = 
        "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| e7_lhmedium_nod0_mu24"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016).ignore();
    /// Listing 'Tight' first as it has higher priority (an electron with both
    /// non-zero 'Tight'+'Medium' decorations will then be tagged as 'Tight')
    myTool.setProperty("LeptonTagDecorations", "MyTight,MyMedium").ignore();
+[example3com]
        
[example3e] ####################################################################
    const char* triggers2016 = 
        "e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
        "|| e7_lhmedium_nod0_mu24"
        "|| 2e17_lhvloose_nod0";
    myTool.setProperty("TriggerCombination2016", triggers2016).ignore();
    /// Special character '?' indicates that the decorated value is to be 
    /// suffixed to the name (=> 'PID1' for medium, 'PID2' for tight)
    myTool.setProperty("LeptonTagDecorations", "PID?").ignore();
+[example3com]