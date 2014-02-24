{
//Change as need. Settings in this file will take precedence over
//those set in the jobOption.py file.

Setup::decay_particle=15; //look at tau decays

Setup::debug_mode=false; //verbose output from MC-Tester?

// Setup histograms
int n_bins=60;
double default_min_bin=0.0;
double default_max_bin=2000.0; //Max in MeV

Setup::SetHistogramDefaults(n_bins,default_min_bin,default_max_bin);

Setup::gen1_desc_1="My description goes here";
Setup::gen1_desc_2="We can use \\LaTeX symbols too";
Setup::gen1_desc_3="eg. $\\pi$ are stable in this example";

//The UserTreeAnalysis variable allows a user defined method to 
//run which modifies the list of stable daughter particles before
//histograming. See MC-Tester documentation for a description.
Setup::UserTreeAnalysis="UserTreeAnalysis";

//If generation step of MC-Tester (1st step)
if (Setup::stage!=0){
  printf("Setup loaded from SETUP.C, GENERATION stage %i.\n",Setup::stage);
  //Setup::SuppressDecay(111); // suppress pi0 decays
}
else { //If analysis step (2nd step)
  printf("Setup loaded from SETUP.C, ANALYSIS stage.\n");
  Setup::use_log_y=true; //plot with log scale?
  Setup::user_analysis=MCTest01; //choice of SDP calculator
}


};
