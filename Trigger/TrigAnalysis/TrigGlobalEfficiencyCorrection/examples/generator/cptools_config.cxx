[example0:electron_properties_set] #############################################
        t->setProperty("TriggerKey", string(j?"":"Eff_")
			+ "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0").ignore();
        t->setProperty("IdKey", "Tight").ignore();
        t->setProperty("IsoKey", "Tight").ignore();
	
[example1and2:electron_extraproperties_declare] ################################
    /// For property 'ListOfLegsPerTool':
    std::map<string,string> legsPerTool;

[example1and2:electron_toolconfigs] ############################################
    enum{ cLEGS, cKEY };
    vector<std::array<string,2>> toolConfigs = {
         /// {<list of trigger legs>, <key in map file>}
         /// Single-electron trigger (same tool instance for 2015 and 2016):
        {"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose, e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0",
            "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"}, 
        /// Dielectron trigger (same tool instance for 2015 and 2016):
        {"e12_lhloose_L1EM10VH, e17_lhvloose_nod0", 
            "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0"}
     };

[example1and2:electron_properties_set] #########################################
        t->setProperty("TriggerKey", string(j?"":"Eff_") + cfg[cKEY]).ignore();
        t->setProperty("IdKey", "Tight").ignore();
        t->setProperty("IsoKey", "Tight").ignore();

[example1and2:electron_extraproperties_fill] ###################################
        /// Safer to retrieve the name from the final ToolHandle, it might be
        /// prefixed (by the parent tool name) when the handle is copied
		name = handles[handles.size()-1].name();
		legsPerTool[name] = cfg[cLEGS];

[example3com:electron_extraproperties_declare] #################################
    /// For property 'ListOfLegsPerTool':
	std::map<std::string,std::string> legsPerTool;
    /// For property 'ListOfTagsPerTool':
	std::map<std::string,std::string> tagsPerTool;
    /// For property 'ElectronLegsPerTag':
	std::map<std::string,std::string> legsPerTag;
  
[example3a3b:electron_extraproperties_declare] #################################
+[example3com:electron_extraproperties_declare]
    /// To tag the leading electron as 'Signal'
    SG::AuxElement::Decorator<char> dec_signal("Signal");

[example3c:electron_extraproperties_declare] ###################################
+[example3com:electron_extraproperties_declare]
    /// To tag electron(s) as 'Signal'
    SG::AuxElement::Decorator<char> dec_signal("Signal");
    /// To emulate PID selection (90% loose-to-medium efficiency)
    std::bernoulli_distribution bernoulliPdf(0.9);

[example3d:electron_extraproperties_declare] ###################################
+[example3com:electron_extraproperties_declare]
    /// To tag electron(s) as 'MyMedium' and 'MyTight'
    SG::AuxElement::Decorator<char> dec_medium("MyMedium");
    SG::AuxElement::Decorator<char> dec_tight("MyTight");
    /// To emulate PID selection (90% loose-to-medium/medium-to-tight eff.)
    std::bernoulli_distribution bernoulliPdf(0.9);
 
[example3e:electron_extraproperties_declare] ###################################
+[example3com:electron_extraproperties_declare]
    /// To tag electrons according to the PID criteria they fulfil
    SG::AuxElement::Decorator<char> dec_pid("PID");
    /// To emulate PID selection (90% loose-to-medium/medium-to-tight eff.)
    std::bernoulli_distribution bernoulliPdf(0.9);
 
[example3a:electron_toolconfigs] ###############################################
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
        /// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
        /// For leading electron:
        {"e12_lhloose_L1EM10VH", "Signal", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "Tight", "Tight"},
        /// For subleading electron(s):
        {"e12_lhloose_L1EM10VH", "*", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "LooseBLayer", ""}
    };
        
[example3b:electron_toolconfigs] ###############################################
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
		/// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
        /// Single electron trigger, only for the leading electron ("Signal")
		{"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", "Signal", "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "Tight", "Tight"}, 
		/// Dielectron trigger, for the leading electron ("Signal")
		{"e12_lhloose_L1EM10VH", "Signal", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "Tight", "Tight"}, 
		/// Dielectron trigger, for subleading electron(s) (not tagged => "*")
		{"e12_lhloose_L1EM10VH", "*", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "LooseBLayer", ""}
     };
        
[example3c:electron_toolconfigs] ###############################################
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
		/// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
        /// Single electron trigger: electrons tagged 'Signal'
		{"e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", "Signal", "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "Medium", ""}, 
		/// Dielectron trigger: all electrons (tagged or not)
		{"e12_lhloose_L1EM10VH", "*,Signal", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "LooseBLayer", ""}
     };
        
[example3d:electron_toolconfigs] ###############################################
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
		/// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
		/// Single-electron trigger: electrons tagged 'MyTight'
		{"e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "MyTight", 
			"SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "Tight", "GradientLoose"},
		/// Electron-muon trigger: electrons tagged 'MyTight' or 'MyMedium'
		{"e7_lhmedium_nod0", "MyMedium,MyTight", "MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0", "Medium", ""},
		/// Dielectron trigger: all electrons (tagged or not)
		{"e17_lhvloose_nod0", "*,MyMedium,MyTight", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "LooseBLayer", ""},
     };
        
[example3e:electron_toolconfigs] ###############################################
    enum{ cLEGS, cTAG, cKEY, cPID, cISO };
    std::vector<std::array<std::string,5> > toolConfigs = {
		/// <list of legs>, <list of tags>, <key in map file>, <PID WP>, <iso WP>
		/// Single-electron trigger: only electrons tagged 'PID2' (TightLH+iso) 
		{"e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "PID2", 
			"SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0", "Tight", "GradientLoose"},
		/// Electron-muon trigger: electrons tagged 'PID2' or 'PID1' (MediumLH)
		{"e7_lhmedium_nod0", "PID1,PID2", "MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0", "Medium", ""},
		/// Dielectron trigger: all electrons (tagged or not)
		{"e17_lhvloose_nod0", "*,PID1,PID2", "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0", "LooseBLayer", ""},
     };
        
[example3:electron_properties_set] #############################################
        t->setProperty("TriggerKey", string(j?"":"Eff_") + cfg[cKEY]).ignore();
        t->setProperty("IdKey", cfg[cPID]).ignore();
        t->setProperty("IsoKey", cfg[cISO]).ignore();
        
[example3:electron_extraproperties_fill] #######################################
        /// Safer to retrieve the name from the final ToolHandle, it might be
        /// prefixed (by the parent tool name) when the handle is copied    
        name = handles[handles.size()-1].name();
        legsPerTool[name] = cfg[cLEGS];
        tagsPerTool[name] = cfg[cTAG];
        if(!j)
        {
            for(auto& tag : ::split_comma_delimited(cfg[cTAG]))
            {
                if(legsPerTag[tag]=="") legsPerTag[tag] = cfg[cLEGS];
                else legsPerTag[tag] += "," + cfg[cLEGS];
            }
        }
