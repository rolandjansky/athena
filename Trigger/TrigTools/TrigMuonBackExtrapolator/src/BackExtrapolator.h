/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BACKEXTRAPOLATOR_H
#define BACKEXTRAPOLATOR_H


namespace MuonBackExtrapolator {

    bool give_eta_phi_at_vertex( double, double, double, bool,bool,bool,bool,
                                 double&, double&, double&, double&,double, 
                                 const double data_Barrel_Param[2][2][2],
                                 const double data_Barrel_Sigmas[2][2][2][2],
                                 const double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
                                 const double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
                                 const double data_Endcap_InnerST_Param[4][12][2][2][2],
                                 const double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
                                 const double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]);

    bool give_eta_phi_at_tuned_vertex( double, double, double, double, double, 
                                       bool, bool, bool, bool, double&, double&, 
				       double&, double&, double,
                                 const double data_Barrel_Param[2][2][2],
                                 const double data_Barrel_Sigmas[2][2][2][2],
                                 const double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
                                 const double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
                                 const double data_Endcap_InnerST_Param[4][12][2][2][2],
                                 const double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
                                 const double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]);
		       
    bool idTrack_in_loose_window ( double, int, double, double, double, 
                                   double, double, bool, bool, double, double,
                                 double data_Barrel_Param[2][2][2],
                                 double data_Barrel_Sigmas[2][2][2][2],
                                 double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
                                 double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
                                 double data_Endcap_InnerST_Param[4][12][2][2][2],
                                 double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
                                 double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]);
			       
    bool idTrack_in_tight_window ( double, int, double, double, double, double, 
                                   double, double, double, bool, bool, double, double,
                                 double data_Barrel_Param[2][2][2],
                                 double data_Barrel_Sigmas[2][2][2][2],
                                 double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
                                 double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
                                 double data_Endcap_InnerST_Param[4][12][2][2][2],
                                 double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
                                 double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]);

    double retune_vtx(double, double, double);
    
    double retune_pt(double, bool dataset=false);

}

#endif
