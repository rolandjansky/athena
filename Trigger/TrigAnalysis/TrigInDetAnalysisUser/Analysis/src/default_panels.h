/* emacs: this is -*- c++ -*- */
/**
 **   @file    default_panels.h        
 **                    
 **   @author mark sutton
 **   @date   Sat 12 May 2018 23:58:57 BST
 **
 **   $Id: default_panels.h, v0.0   Fri 11 Jan 2019 07:06:38 CET sutt $
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef   DEFAULT_PANELS_H
#define   DEFAULT_PANELS_H

#include <string>

const int Nhistos = 48;

std::string histos_default[Nhistos][6] = { 

    /// distributions - 4
    { "pT",      "p_{T}",     "xaxis:lin:auto:1:100",   "Offline p_{T} [GeV]", "yaxis:log:auto",  ""  },
    { "pT_rec",  "p_{T} rec", "xaxis:lin:auto:1:100",   "Trigger p_{T} [GeV]", "yaxis:log:auto",  ""  },
    { "a0",      "a0",        "xaxis:lin:-3:3",         "Offline a_{0} [mm]",  "yaxis:log:auto",  ""  },
    { "a0_rec",  "a0 rec",    "xaxis:lin:-3:3",         "Trigger a_{0} [mm]",  "yaxis:log:auto",  ""  },
    { "z0",      "z0",        "xaxis:lin:-250:250",     "z_{0} [mm]",          "yaxis:log:auto",  ""  },

    /// efficiencies - 10 
    { "pT_eff",       "Efficiency p_{T}", "xaxis:log:auto:1:100",  "Offline track p_{T} [GeV]",    "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
    { "eta_eff",      "Efficiency #eta",  "xaxis:lin",             "Offline track #eta",           "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
    { "phi_eff",      "Efficiency #phi",  "xaxis:lin",             "Offline track #phi",           "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
    { "d0_eff",       "Efficiency d0",    "xaxis:lin:autosym",     "Offline track d_{0} [mm]",     "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
    { "a0_eff",       "Efficiency a0",    "xaxis:lin:autosym",     "Offline track d_{0} [mm]",     "yaxis:lin:auto:90:102",  "Efficiency [%]" },      
    { "z0_eff",       "Efficiency z0",    "xaxis:lin:-250:250",    "Offline track z_{0} [mm]",     "yaxis:lin:auto:90:102",  "Efficiency [%]" },        
 
    { "eff_vs_mu",    "Efficiency <#mu>",            "xaxis:lin:auto",       "<#mu>",              "yaxis:lin:90:102",       "Efficiency [%]" },       
    { "roi_dphi_eff", "Efficiency #Delta#phi(RoI)",  "xaxis:lin:-0.6:0.6",   "#Delta#phi (RoI)",   "yaxis:lin:90:102",       "Efficiency [%]" },
    { "roi_deta_eff", "Efficiency #Delta#eta(RoI)",  "xaxis:lin:-0.6:0.6",   "#Delta#eta (RoI)",   "yaxis:lin:90:102",       "Efficiency [%]" },       
    { "roi_dR_eff",   "Efficiency #DeltaR(RoI)",     "xaxis:lin:0:0.6",      "#Delta R (RoI)",     "yaxis:lin:90:102",       "Efficiency [%]" },       

    /// standard residuals - 5 
    { "ipT_res",    "Residual 1/p_{T}",  "xaxis:lin:-0.15:0.2",     "#Delta 1/p_{T} [GeV^{-1}]",    "yaxis:log:auto",    "Normalised entries" },       
    { "eta_res",    "Residual #eta",     "xaxis:lin:-0.05:0.05",    "#Delta#eta",                   "yaxis:log:auto",    "Normalised entries" },       
    { "phi_res",    "Residual #phi",     "xaxis:lin:-0.05:0.05",    "#Delta#phi",                   "yaxis:log:auto",    "Normalised entries" },
    { "z0_res",     "Residual z0",       "xaxis:lin:-10:10",        "#Delta z_{0} [mm]",            "yaxis:log:auto",    "Normalised entries" },
    { "a0_res",     "Residual a0",       "xaxis:lin:-1:1",          "#Delta d_{0} [mm]",            "yaxis:log:auto",    "Normalised entries" },       
 
    /// residuals vs track parameters - 17
    { "rd0_vs_pt/sigma",          "Residual d vs p_{T}",          "xaxis:lin:auto",      "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "d_{0} resolution [mm]" },
    { "rd0_vs_signed_pt/sigma",   "Residual d vs signed p_{T}",   "xaxis:lin:-100:100",  "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "d_{0} resolution [mm]" },    
    { "rd0_vs_eta/sigma",         "Residual d vs #eta",           "xaxis:lin",           "Offline #eta",          "yaxis:lin:auto",  "d_{0} resolution [mm]" },                    
    { "rd0_vs_ipt/sigma",         "Residual d vs 1/p_{T}",        "xaxis:lin",           "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "d_{0} resolution [mm]" },                       

    { "ript_vs_pt/sigma",         "Residual 1/p_{T} vs p_{T}",    "xaxis:lin:auto",     "Offline p_{T} [GeV]",   "yaxis:log:auto",  "1/p_{T} resolution [GeV^{-1}]" },
    { "ript_vs_eta/sigma",        "Residual 1/p_{T} vs #eta",     "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto",  "1/p_{T} resolution [GeV^{-1}]" },
    { "ript_vs_ipt/sigma",        "Residual 1/p_{T} vs 1/p_{T}",  "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "1/p_{T} resolution [GeV^{-1}]" },

    { "reta_vs_pt/sigma",         "Residual #eta p_{T}",          "xaxis:log:auto",     "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "#eta resolution" },            
    { "reta_vs_eta/sigma",        "Residual #eta vs #eta",        "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto",  "#eta resolution" },            
    { "reta_vs_ipt/sigma",        "Residual #eta vs 1/p_{T}",     "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "#eta resolution" },            

    { "rphi_vs_pt/sigma",         "Residual #phi vs p_{T}",       "xaxis:lin:1:100",    "p_{T} [GeV]",           "yaxis:lin:auto", "#phi resolution" },
    { "rphi_vs_ipt/sigma",        "Residual #phi vs 1/p_{T}",     "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto", "#phi resolution" },

    { "rzed_vs_eta/sigma",        "Residual z vs #eta",           "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto", "z_{0} resolution [mm]" },
    { "rzed_vs_pt/sigma",         "Residual z vs p_{T}",          "xaxis:log:auto",     "Offline p_{T} [GeV]",   "yaxis:lin:auto", "z_{0} resolution [mm]" },
    { "rzed_vs_signed_pt/sigma",  "Residual z vs signed p_{T}",   "xaxis:lin:-100:100", "Offline p_{T} [GeV]",   "yaxis:lin:auto", "z_{0} resolution [mm]" },
    { "rzed_vs_zed/sigma",        "Residual z vs z",              "xaxis:lin:-250:250", "Offline z [mm]",        "yaxis:lin:auto", "z_{0} resolution [mm]" },
    { "rzed_vs_ipt/sigma",        "Residual z vs 1/p_{T}",        "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto", "z_{0} resolution [mm]" },

    /// track multiplicity - 1
    { "ntracks_rec",             "number of reconstructed tracks", "xaxis:lin:auto",   "N trigger tracks",     "yaxis:log:auto", "Entries"  },

    /// hit multiplicity - 6
    { "npix_eta/mean",           "mean number of pixel hits",  "xaxis:lin",   "Offline #eta",   "yaxis:lin:3:6",  "Pixel hits"    },
    { "nsct_eta/mean",           "mean number of SCT hits",    "xaxis:lin",   "Offline #eta",   "yaxis:lin:7:10", "SCT clusters"  },

    { "npix_pt/mean",           "mean number of pixel hits",  "xaxis:lin:auto",   "Offline p_{T} [GeV]",   "yaxis:lin:3:6",  "Pixel hits"    },
    { "nsct_pt/mean",           "mean number of SCT hits",    "xaxis:lin:auto",   "Offline p_{T} [GeV]",   "yaxis:lin:7:10", "SCT clusters"  },

    { "npixh_pt/mean",           "mean number of pixel holes",  "xaxis:lin:auto",   "Offline p_{T} [GeV]",   "yaxis:lin:-1:6",  "Pixel holes"    },
    { "nscth_pt/mean",           "mean number of SCT holes",    "xaxis:lin:auto",   "Offline p_{T} [GeV]",   "yaxis:lin:-1:10", "SCT holes"  },


    /// chi2 and chi2 probability - 3
    { "Chi2prob/1d",           "Chi2 probability",       "xaxis:lin",         "track #chi^{2} Probability",   "yaxis:lin:auto",  "Entries"  },
    { "Chi2dof/1d",            "Chi2 per dof",           "xaxis:lin",         "track #chi^{2} per dof",       "yaxis:log:auto",  "Entries"  },
    { "Chi2/1d",               "Chi2",                   "xaxis:lin",         "Offline track #chi^{2}",       "yaxis:lin",       "Entries"  },

    { "Chi2prob/mean",         "Chi2 probability vs pT", "xaxis:log:auto",    "Offline p_{T} [GeV]",          "yaxis:lin",  "mean track #Chi^{2} Probability"  }

};
  


std::string eff_panel[4][6] = { 

  { "eta_eff",      "Efficiency #eta",  "xaxis:lin",             "Offline track #eta",           "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
  { "pT_eff",       "Efficiency p_{T}", "xaxis:log:auto:1:100",  "Offline track p_{T} [GeV]",    "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
  { "phi_eff",      "Efficiency #phi",  "xaxis:lin",             "Offline track #phi",           "yaxis:lin:auto:90:102",  "Efficiency [%]" },       
  { "a0_eff",       "Efficiency a0",    "xaxis:lin:autosym",     "Offline track d_{0} [mm]",     "yaxis:lin:auto:90:102",  "Efficiency [%]" }      

};     


std::string res_panel[4][6] = { 

  { "eta_res",    "Residual #eta",     "xaxis:lin:-0.05:0.05",    "#Delta#eta",                   "yaxis:log:auto",    "Normalised entries" },        
  { "ipT_res",    "Residual 1/p_{T}",  "xaxis:lin:-0.15:0.2",     "#Delta 1/p_{T} [GeV^{-1}]",    "yaxis:log:auto",    "Normalised entries" },       
  { "phi_res",    "Residual #phi",     "xaxis:lin:-0.05:0.05",    "#Delta#phi",                   "yaxis:log:auto",    "Normalised entries" },
  { "z0_res",     "Residual z0",       "xaxis:lin:-10:10",        "#Delta z_{0} [mm]",            "yaxis:log:auto",    "Normalised entries" } 

};


std::string diff_panel[10][6] = { 

    { "reta_vs_eta/sigma",        "Residual #eta vs #eta",        "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto",  "#eta resolution" },            
    { "reta_vs_pt/sigma",         "Residual #eta p_{T}",          "xaxis:log:auto",     "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "#eta resolution" },            

    { "ript_vs_eta/sigma",        "Residual 1/p_{T} vs #eta",     "xaxis:lin:auto",     "Offline #eta",         "yaxis:log:auto",  "1/p_{T} resolution [GeV^{-1}]" },
    { "ript_vs_pt/sigma",         "Residual 1/p_{T} vs p_{T}",    "xaxis:log:auto",     "Offline p_{T} [GeV]",   "yaxis:lin:auto", "1/p_{T} resolution [GeV^{-1}]" },

    { "rzed_vs_eta/sigma",        "Residual z vs #eta",           "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto", "z_{0} resolution [mm]" },
    { "rzed_vs_pt/sigma",         "Residual z vs p_{T}",          "xaxis:log:auto",     "Offline p_{T} [GeV]",   "yaxis:lin:auto", "z_{0} resolution [mm]" },

    { "rd0_vs_eta/sigma",         "Residual d vs #eta",           "xaxis:lin",           "Offline #eta",          "yaxis:lin:auto",  "d_{0} resolution [mm]" },                    
    { "rd0_vs_pt/sigma",          "Residual d vs p_{T}",          "xaxis:log:auto",      "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "d_{0} resolution [mm]" },

    { "rd0_vs_signed_pt/sigma",   "Residual d vs signed p_{T}",   "xaxis:lin:-100:100",  "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "d_{0} resolution [mm]" },    
    { "rzed_vs_signed_pt/sigma",  "Residual z vs signed p_{T}",   "xaxis:lin:-100:100",  "Offline p_{T} [GeV]",   "yaxis:lin:auto",  "z_{0} resolution [mm]" }    

};




// std::string vxeff[][6] = { 
//  
// };


std::string unwanted[8][6] = { 

    { "ript_vs_eta/sigma",        "Residual 1/p_{T} vs #eta",     "xaxis:lin",          "Offline #eta",          "yaxis:lin:auto",  "1/p_{T} resolution [GeV^{-1}]" },
    { "rphi_vs_pt/sigma",         "Residual #phi vs p_{T}",       "xaxis:lin:1:100",    "p_{T} [GeV]",           "yaxis:lin:auto", "#phi resolution" },
    { "rzed_vs_zed/sigma",        "Residual z vs z",              "xaxis:lin:-250:250", "Offline z [mm]",        "yaxis:lin:auto", "z_{0} resolution [mm]" },

    { "rd0_vs_ipt/sigma",         "Residual d vs 1/p_{T}",        "xaxis:lin",           "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "d_{0} resolution [mm]" },                       
    { "ript_vs_ipt/sigma",        "Residual 1/p_{T} vs 1/p_{T}",  "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "1/p_{T} resolution [GeV^{-1}]" },
    { "reta_vs_ipt/sigma",        "Residual #eta vs 1/p_{T}",     "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto",  "#eta resolution" },            
    { "rphi_vs_ipt/sigma",        "Residual #phi vs 1/p_{T}",     "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto", "#phi resolution" },
    { "rzed_vs_ipt/sigma",        "Residual z vs 1/p_{T}",        "xaxis:lin",          "1/p_{T} [GeV^{-1}]",    "yaxis:lin:auto", "z_{0} resolution [mm]" }

};


#endif  /* DEFAULT_PANELS_H */










