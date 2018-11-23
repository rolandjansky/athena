[example0:electron_selection] ##################################################
            /// lepton must be above trigger threshold:
            if(pt < (runNumber>290000? 27e3f : 25e3f)) continue;

[example0:muon_selection] ######################################################
            /// lepton must be above trigger threshold:
            if(pt < (runNumber>290000? 27.3e3f : 21e3f)) continue;

[example0:trigger_matching] ####################################################
        /// Events must contain at least one lepton above trigger threshold
        if(myTriggeringElectrons.size()+myTriggeringMuons.size() < 1) continue;

[example1and2:trigcounters] ####################################################
        unsigned nTrig1L = 0, nTrig2mu = 0;

[example1and2:electron_selection] ##############################################
            /// lepton must be above softest trigger threshold:
            if((runNumber>320000 && pt<25e3f) /// 2017: 2e24
				|| (runNumber>290000 && pt<18e3f) /// 2016: 2e17
				|| (pt<13e3f)) continue; /// 2015: 2e12
            /// also count leptons above single-lepton trigger threshold
            if(pt >= (runNumber>290000? 27e3f : 25e3f)) ++nTrig1L;

[example1:muon_selection] ######################################################
            /// lepton must be above softest trigger threshold (mu8noL1 here):
            if(pt < 10e3f) continue;
            /// also count leptons above single-lepton trigger threshold
            if(pt >= (runNumber>290000? 27.3e3f : 21e3f)) ++nTrig1L;
            // and count muons suitable for the hard leg of the dimuon trigger
            if(pt >= (runNumber>290000? 23e3f : 19e3f)) ++nTrig2mu;
        
[example2:muon_selection] ######################################################
            /// lepton must be above softest trigger threshold (mu8noL1 here):
            if(pt < 10e3f) continue;
            /// also count leptons above single-lepton trigger threshold
            if(pt >= (runNumber>290000? 27.3e3f : 21e3f)) ++nTrig1L;
            /// and count muons suitable for the hard leg of the dimuon trigger
            if(pt >= 25e3f) ++nTrig2mu;

[example1:trigger_matching] ####################################################
        /// Events must contain enough leptons to trigger
        if(nTrig1L==0 /// single-lepton trigger
            && myTriggeringElectrons.size()<2 /// dielectron
            && (nTrig2mu==0 || myTriggeringMuons.size()<2)) /// dimuon
        {
            continue;
        }

[example2:trigger_matching] ####################################################
        /// Events must contain enough leptons to trigger
        if((nTrig1L==0 || (runNumber>=303638 && runNumber<320000)) /// single-lepton trigger (when used)
            && myTriggeringElectrons.size()<2 /// dielectron
            && (nTrig2mu==0 || myTriggeringMuons.size()<2)) /// dimuon
        {
            continue;
        }

[example3d3e:trigcounters] #####################################################
        unsigned nTrig_e26 = 0, nTrig_e7 = 0, nTrig_e17 = 0;

[example3a3b3c:electron_selection] #############################################
            /// electron must be above softest trigger threshold (e12 here)
            if(pt < 13e3f) continue;
        
[example3d3e:electron_selection] ###############################################
            /// electron must be above softest trigger threshold (e7 here
            if(pt < 7e3f) continue;
            if(pt >= 18e3f) ++nTrig_e17;
        
[example3d3e:muon_selection] ###################################################
            /// muon must be above softest trigger threshold (mu24 here)
            if(pt < 25.2e3f) continue;
        
[example3a3b:tagging] ##########################################################
        /// Let's pretend that the leading electron passes TightLH+isolation,
		/// thus the event passes the selection; now we tag the electrons:
        auto compareByPt = [](const xAOD::Electron*e1, const xAOD::Electron*e2)
            { return e1->pt() < e2->pt(); };
        auto leadingElectron = *std::max_element(myTriggeringElectrons.begin(),
            myTriggeringElectrons.end(), compareByPt);
		for(auto electron : myTriggeringElectrons)
        {
            /// Leading electron tagged as 'Signal' -> decorated value set to 1
            /// Subleading electron(s) not tagged -> decorated value set to 0
            dec_signal(*electron) = (electron==leadingElectron)? 1 : 0;
        }
        
[example3a:trigger_matching] ###################################################
        /// Events must contain at least two lepton above trigger threshold
        if(myTriggeringElectrons.size() < 2) continue;

+[example3a3b:tagging]
        
[example3b:trigger_matching] ###################################################
        if(myTriggeringElectrons.size() < 1) continue;
        
+[example3a3b:tagging]
        
        /// Events must contain enough leptons to trigger
        if(leadingElectron->pt() < 25e3f /// single-electron trigger
            && myTriggeringElectrons.size() < 2) /// dielectron
        {
            continue;
        }
        
[example3c:trigger_matching] ###################################################
        /// Add 'Signal' decorations to random electrons
        /// also count 'Signal' electrons above e24_xxx threshold
        unsigned nTrig1L = 0;
		for(auto electron : myTriggeringElectrons)
        {
            bool signal = bernoulliPdf(randomEngine);
            dec_signal(*electron) = signal? 1 : 0;
            if(signal && electron->pt()>25e3f) ++nTrig1L;
        }
        
        /// Events must contain enough leptons to trigger
        if(nTrig1L < 1 /// single-electron trigger
            && myTriggeringElectrons.size() < 2) /// dielectron
        {
            continue;
        }
        
[example3d:trigger_matching] ###################################################
        /// Add 'MyMedium' & 'MyTight' decorations to random electrons
        /// also count tight electrons above e26_xxx threshold
        /// and medium electrons above e7_xxx threshold
		for(auto electron : myTriggeringElectrons)
        {
            bool medium = bernoulliPdf(randomEngine);
            dec_medium(*electron) = medium? 1 : 0;
            if(medium && electron->pt()>8e3f) ++nTrig_e7;
            bool tight = medium && bernoulliPdf(randomEngine);
            dec_tight(*electron) = tight? 1 : 0;
            if(tight && electron->pt()>27e3f) ++nTrig_e26;
        }
        
        /// Events must contain enough leptons to trigger
        if(nTrig_e26 < 1 /// single-electron trigger
            && (nTrig_e7==0 || myTriggeringMuons.size()==0) /// electron-muon
            && nTrig_e17 < 2) /// dielectron
        {
            continue;
        }
        
[example3e:trigger_matching] ###################################################
        /// Add 'PID' decorations to random electrons
        /// also count 'Tight' electrons above e26_xxx threshold
        /// and 'Medium' electrons above e7_xxx threshold
		for(auto electron : myTriggeringElectrons)
        {
            bool medium = bernoulliPdf(randomEngine);
            bool tight = medium && bernoulliPdf(randomEngine);
            /// Set decorated value to 2 for TightLH+iso or 1 for MediumLH
            dec_pid(*electron) = tight? 2 : medium? 1 : 0;
            if(medium && electron->pt()>8e3f) ++nTrig_e7;
            if(tight && electron->pt()>27e3f) ++nTrig_e26;
        }
        
        /// Events must contain enough leptons to trigger
        if(nTrig_e26 < 1 /// single-electron trigger
            && (nTrig_e7==0 || myTriggeringMuons.size()==0) /// electron-muon
            && nTrig_e17 < 2) /// dielectron
        {
            continue;
        }
