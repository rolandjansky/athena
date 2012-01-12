#!/bin/csh
source setup.csh
root -b <<EOF |& grep -v TVector

  // Not sure why, but I need this here
  TFile tf;

  TrigTauEffTool tr;
  tr.setRecoMatch(false);

  // Pick a selection
  //  tr.setDefaultVersion(TrigTauSelectionManager::MENU13030pre);
  tr.setDefaultVersion(TrigTauSelectionManager::MENU1400);


  tr.defineSelector("tau12", TrigTauFlags::TAU12);
  tr.defineSelector("tau16i", TrigTauFlags::TAU16I);
  tr.defineSelector("tau20i", TrigTauFlags::TAU20I);
  tr.defineSelector("tau29i", TrigTauFlags::TAU29I);
  tr.defineSelector("tau38", TrigTauFlags::TAU38);
  tr.defineSelector("tau38i", TrigTauFlags::TAU38I);
  tr.defineSelector("tau50",  TrigTauFlags::TAU50);
  tr.defineSelector("tau84",  TrigTauFlags::TAU84);
  tr.defineSelector("tau125",  TrigTauFlags::TAU125);

  tr.defineSelector("tau12_medium", TrigTauFlags::TAU12_MEDIUM);
  tr.defineSelector("tau16i_medium", TrigTauFlags::TAU16I_MEDIUM);
  tr.defineSelector("tau20i_medium", TrigTauFlags::TAU20I_MEDIUM);
  tr.defineSelector("tau29i_medium", TrigTauFlags::TAU29I_MEDIUM);
  tr.defineSelector("tau38_medium", TrigTauFlags::TAU38_MEDIUM);
  tr.defineSelector("tau38i_medium", TrigTauFlags::TAU38I_MEDIUM);
  tr.defineSelector("tau50_medium",  TrigTauFlags::TAU50_MEDIUM);
  tr.defineSelector("tau84_medium",  TrigTauFlags::TAU84_MEDIUM);
  tr.defineSelector("tau125_medium",  TrigTauFlags::TAU125_MEDIUM);

  tr.defineSelector("tau12_tight", TrigTauFlags::TAU12_TIGHT);
  tr.defineSelector("tau16i_tight", TrigTauFlags::TAU16I_TIGHT);
  tr.defineSelector("tau20i_tight", TrigTauFlags::TAU20I_TIGHT);
  tr.defineSelector("tau29i_tight", TrigTauFlags::TAU29I_TIGHT);
  tr.defineSelector("tau38_tight", TrigTauFlags::TAU38_TIGHT);
  tr.defineSelector("tau38i_tight", TrigTauFlags::TAU38I_TIGHT);
  tr.defineSelector("tau50_tight",  TrigTauFlags::TAU50_TIGHT);
  tr.defineSelector("tau84_tight",  TrigTauFlags::TAU84_TIGHT);
  tr.defineSelector("tau125_tight",  TrigTauFlags::TAU125_TIGHT);

  
//  tr.fillSample("5107",  "ntuples/5107/trig1_misal1_csc11.005107.pythia_Wtauhad.recon.AOD.v13003003.TTP06.merged.0.root");
//  tr.fillSample("misal", "ntuples/5107_misal1day/misal_1day_5107.AOD.v13040.TTP06.2.root");
//  tr.fillSample("misal", "ntuples/5107_misal1day/misal_1day_5107.AOD.v13040.TTP06.3.root");
//  tr.fillSample("misal", "ntuples/5107_misal1day/misal_1day_5107.AOD.v13040.TTP06.4.root");
//  tr.fillSample("misal", "ntuples/5107_misal1day/misal_1day_5107.AOD.v13040.TTP06.5.root");

  tr.fillSample("5200", "ntuples/trigtau.5200.TTP07.RDO.rel3.root");




  //  tr.print();



  tr.printSelector("tau12" , true);
  tr.printSelector("tau16i", true);
  tr.printSelector("tau20i", true);
  tr.printSelector("tau29i", true);
  tr.printSelector("tau38" , true);
  tr.printSelector("tau38i", true);
  tr.printSelector("tau50" , true);
  tr.printSelector("tau84" , true); 
  tr.printSelector("tau125", true);
			   

  tr.printSelector("tau12_medium" , true);
  tr.printSelector("tau16i_medium", true);
  tr.printSelector("tau20i_medium", true);
  tr.printSelector("tau29i_medium", true);
  tr.printSelector("tau38_medium" , true);
  tr.printSelector("tau38i_medium", true);
  tr.printSelector("tau50_medium" , true);
  tr.printSelector("tau84_medium" , true); 
  tr.printSelector("tau125_medium", true);
			   

  tr.printSelector("tau12_tight" , true);
  tr.printSelector("tau16i_tight", true);
  tr.printSelector("tau20i_tight", true);
  tr.printSelector("tau29i_tight", true);
  tr.printSelector("tau38_tight" , true);
  tr.printSelector("tau38i_tight", true);
  tr.printSelector("tau50_tight" , true);
  tr.printSelector("tau84_tight" , true); 
  tr.printSelector("tau125_tight", true);
			   

			   

  tr.printSummary();

  tr.printCutFlow("tau20i");
  tr.printCutFlow("tau60");

  //  tr.printL1Summary();

EOF
