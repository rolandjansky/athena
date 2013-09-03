/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// Author: Marc-Andre Dufour
// Date: 21 Oct 2008
//
// Description: Script that will dump TriggerRateTools' results in an XML file.
////////////////////////////////////////////////////////////////////////////////
{
	gROOT->Reset();
	gROOT->SetStyle("Plain");

        gSystem->Load("$TrigNtDecision_PATH/$CMTCONFIG/libTrigNtDecision.so");
        gSystem->Load("$TriggerRateSlices_PATH/$CMTCONFIG/libTriggerRateSlices.so");

	char temp1[20];
	char temp2[20];
	char temp3[20];
	char temp4[20];
	char num_passed[20];
	char eff[20];
	char eff_err[20];
	char peff_err[20];
	char rate[20];
	char rate_err[20];
	char passthrough[20];
	char prescale[20];
	char prescaled_eff[20];
	char indep_frac[20];
	char unique_frac[20];
	char unique_rate[20];
	char overlap[20];
	char overlap_max[20];
	char xsec[20];
	char lum[20];
	ofstream fout_xml;
	fout_xml.open("TriggerRates.xml");

	Double_t Xsection = 0.0;
	Double_t Luminosity = 0.0;

	Double_t Prescale = 1.0; 
	Double_t Efficiency = 0.0; 
	Double_t Prescaled_Efficiency = 0.0; 

	Double_t max_overlap_value = 0;
	Int_t max_overlap_index = 0;
	Double_t temp_var1 = 0.0;
	Double_t temp_var2 = 0.0;
	Int_t temp_index = 0.0;
	TString *temp_str;
	TChain* s_sigs  = new TChain("SingleSignatures");
	TChain* c_sigs  = new TChain("CplxSignatures");
	TChain* l_sigs  = new TChain("Levels");
	//TChain* streams = new TChain("Streams");
        s_sigs->Add("TriggerRates.root");
        c_sigs->Add("TriggerRates.root");
        l_sigs->Add("TriggerRates.root");
        //streams->Add("TriggerRates.root");
	Long64_t s_sigs_Nentries = s_sigs->GetEntries();
	Long64_t c_sigs_Nentries = c_sigs->GetEntries();
	Long64_t l_sigs_Nentries = l_sigs->GetEntries();
	//Long64_t streams_Nentries = streams->GetEntries();
	Long64_t Nentries = 0;
	//if ((s_sigs_Nentries == c_sigs_Nentries) && (s_sigs_Nentries == l_sigs_Nentries) && (s_sigs_Nentries == streams_Nentries)) {
	if ((s_sigs_Nentries == c_sigs_Nentries) && (s_sigs_Nentries == l_sigs_Nentries)) {
		Nentries = s_sigs_Nentries;
	} else {
		cout << "	[!!] Signature containers are not consistent... [!!]" << endl;
	}
	
        TObjArray *s_sigs_branches=s_sigs->GetListOfBranches();
        const Int_t num_sigs = s_sigs_branches->GetEntries();
        TString s_sig_names[num_sigs];
        for(Int_t i=0; i<num_sigs; i++) {
            s_sig_names[i]=NULL;
            s_sig_names[i]=*(((TD_Signature)s_sigs_branches.At(i))->getName());
        }

        TD_Signature* sigs[num_sigs];
        for (Int_t sig_i = 0; sig_i < num_sigs; sig_i++) {
            sigs[sig_i] = NULL;
	    s_sigs->SetBranchAddress((s_sig_names[sig_i]) , &sigs[sig_i]);
        }
        

        TObjArray *c_sigs_branches=c_sigs->GetListOfBranches();
        const Int_t num_csigs = c_sigs_branches->GetEntries();
        TString c_sig_names[num_csigs];
        for(Int_t i=0; i<num_csigs; i++) {
            c_sig_names[i]=NULL;
            c_sig_names[i]=*(((TD_CplxSignature)c_sigs_branches.At(i))->getName());
        }

        TD_CplxSignature* sigs_c[num_csigs];
        for (Int_t sig_i = 0; sig_i < num_csigs; sig_i++) {
            sigs_c[sig_i] = NULL;
	    c_sigs->SetBranchAddress((c_sig_names[sig_i]) , &sigs_c[sig_i]);
        }

       
        //TObjArray *streams_branches=streams->GetListOfBranches();
        //const Int_t num_streams = streams_branches->GetEntries()-1;
        //TString stream_names[num_streams];
        //for(Int_t i=0; i<num_streams; i++) {
        //    stream_names[i]=NULL;
        //    stream_names[i]=*(((CplxSignature)streams_branches.At(i+1))->getName());
        //}

        //CplxSignature* sigs_streams[num_streams];
        //for (Int_t sig_i = 0; sig_i < num_streams; sig_i++) {
        //    sigs_streams[sig_i] = NULL;
	//    streams->SetBranchAddress((stream_names[sig_i]) , &sigs_streams[sig_i]);
        //}

 
        TObjArray *level_branches=l_sigs->GetListOfBranches();
        const Int_t num_levels = level_branches->GetEntries();
        TString level_names[num_levels];
        for(Int_t i=0; i<num_levels; i++) {
            level_names[i]=NULL;
            level_names[i]=*(((TD_CplxSignature)level_branches.At(i))->getName());
        }

        TD_CplxSignature* levels[num_levels];
        for (Int_t sig_i = 0; sig_i < num_levels; sig_i++) {
            levels[sig_i] = NULL;
	    l_sigs->SetBranchAddress((level_names[sig_i]) , &levels[sig_i]);
        }


	fout_xml << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
	fout_xml << "<?xml-stylesheet type=\"text/xsl\" href=\"trigger_rates.xsl\"?>" << endl;
	fout_xml << "<trigger>" << endl;
	for (Int_t entry = 0; (entry < Nentries) && (num_sigs > 0); entry++) {
		s_sigs->GetEntry(entry);
		c_sigs->GetEntry(entry);
		l_sigs->GetEntry(entry);
		//streams->GetEntry(entry);
		cout << "    >>> Started extracting rates" << endl;
		sprintf(xsec, "%.2e", sigs[0]->getXsec());
                temp_var1 = -1.0;
                for (Int_t j = 0; (j < num_sigs)&&(temp_var1 == -1.0); j++) {
                    if ((sigs[j])->getNumPassed() > 0) {
                        temp_var1 = (sigs[j])->getLumi();
                    }
                }
		sprintf(lum, "%.2e", temp_var1);
		fout_xml << "    <Xsection>" << xsec << "</Xsection>" << endl;
		fout_xml << "    <Luminosity>" << lum << "</Luminosity>" << endl;
		fout_xml << "    <n_evts>" << sigs[0]->getNumEvts() << "</n_evts>" << endl;
		for (Int_t i = 0; i < num_levels; i++) {
			sprintf(eff, "%.2e", (levels[i])->getEff() );
		        sprintf(eff_err, "%.2e", (levels[i])->getEffErr() );
			sprintf(prescaled_eff, "%.2e", (levels[i])->getPeff() );
                        if ((levels[i])->getPrescale() > 0) {
		    	    sprintf(peff_err, "%.2e", ((levels[i])->getEffErr())/((levels[i])->getPrescale()) );
                        } else {
		    	    sprintf(peff_err, "%.2e", 0.0 );
                        }
			sprintf(rate, "%.2e", (levels[i])->getRate());
		        sprintf(rate_err, "%.2e", (levels[i])->getRateErr() );
			temp_str = (levels[i])->getLevel();
			fout_xml << "    <level>" << endl;
			fout_xml << "        <lvl_name>" << *(temp_str) << "</lvl_name>" << endl;
			fout_xml << "        <lvl_eff>" << eff << "</lvl_eff>" << endl;
			fout_xml << "        <lvl_eff_err>" << eff_err << "</lvl_eff_err>" << endl;
			fout_xml << "        <lvl_prescaled_eff>" << prescaled_eff << "</lvl_prescaled_eff>" << endl;
			fout_xml << "        <lvl_prescaled_eff_err>" << peff_err << "</lvl_prescaled_eff_err>" << endl;
			fout_xml << "        <lvl_rate>" << rate << "</lvl_rate>" << endl;
			fout_xml << "        <lvl_rate_err>" << rate_err << "</lvl_rate_err>" << endl;
			for (Int_t j = 0; j < num_sigs; j++) {
				if ( *((sigs[j])->getLevel()) == *temp_str) {
					sprintf(num_passed, "%i", (sigs[j])->getNumPassed() );
					sprintf(eff, "%.2e", (sigs[j])->getEff() );
					sprintf(eff_err, "%.2e", (sigs[j])->getEffErr() );
					sprintf(rate, "%.2e", (sigs[j])->getRate() );
					sprintf(rate_err, "%.2e", (sigs[j])->getRateErr() );
					sprintf(prescale, "%.2e", (sigs[j])->getPrescale() );
					sprintf(passthrough, "%.2e", (sigs[j])->getPassthrough() );
					sprintf(prescaled_eff, "%.2e", (sigs[j])->getPeff() );
                                        if ((sigs[j])->getPrescale() > 0) {
			                    sprintf(peff_err, "%.2e", ((sigs[j])->getEffErr())/((sigs[j])->getPrescale()) );
                                        } else {
			                    sprintf(peff_err, "%.2e", 0.0 );
                                        }
					if ((sigs[j])->getNumPassed() > 0) {
						sprintf(indep_frac, "%.2e", (sigs[j])->getIndepFrac() );
						sprintf(unique_frac, "%.2e", (float)(sigs[j])->getUniqueFrac() );
						sprintf(unique_rate, "%.2e", (float)(sigs[j])->getUniqueRate() );
					} else {
						sprintf(indep_frac, "%.2e", 0.0);
						sprintf(unique_frac, "%.2e", 0.0);
						sprintf(unique_rate, "%.2e", 0.0);
					}
					fout_xml << "        <signature>" << endl;
					fout_xml << "            <sig_name>" << *( (sigs[j])->getName() ) << "</sig_name>" << endl;
					fout_xml << "            <prescale>" << prescale << "</prescale>" << endl;
					fout_xml << "            <passthrough>" << passthrough << "</passthrough>" << endl;
					fout_xml << "            <evts_passed>" << num_passed << "</evts_passed>" << endl;
					fout_xml << "            <efficiency>" << eff << "</efficiency>" << endl;
					fout_xml << "            <efficiency_err>" << eff_err << "</efficiency_err>" << endl;
					fout_xml << "            <prescaled_efficiency>" << prescaled_eff << "</prescaled_efficiency>" << endl;
					fout_xml << "            <prescaled_efficiency_err>" << peff_err << "</prescaled_efficiency_err>" << endl;
					fout_xml << "            <rate>" << rate << "</rate>" << endl;
					fout_xml << "            <rate_err>" << rate_err << "</rate_err>" << endl;
					fout_xml << "            <indep_fraction>" << indep_frac << "</indep_fraction>" << endl;
					fout_xml << "            <unique_fraction>" << unique_frac << "</unique_fraction>" << endl;
					fout_xml << "            <unique_rate>" << unique_rate << "</unique_rate>" << endl;
                                        
/*					fout_xml << "            <overlap>" << endl;
					max_overlap_value = 0.0;
					max_overlap_index = -1;
                                        temp_index = (sigs[j])->getOverlapSize();
					temp_var1 = (sigs[j])->getNumPassed();
					for(Int_t k = 0; k < temp_index; k++) {
						fout_xml << "                <sig>" << endl;
						fout_xml << "                    <sig_name>" << *((sigs[j])->getOverlapNameAt(k)) << "</sig_name>" << endl;
						if (temp_var1 > 0) {
							if (((sigs[j])->getOverlapAt(k) >= max_overlap_value) && (*((sigs[j])->getName()) != *((sigs[j])->getOverlapNameAt(k)))) {
								max_overlap_value = (sigs[j])->getOverlapAt(k);
								max_overlap_index = k;
							}
						        sprintf(overlap, "%.5f", (sigs[j])->getOverlapAt(k) );
						} else {
							sprintf(overlap, "N/A");
						}
						fout_xml << "                    <sig_overlap>" << overlap << "</sig_overlap>" << endl;
						fout_xml << "                </sig>" << endl;
					}
					fout_xml << "                </overlap>" << endl;
					if (max_overlap_index != -1) {
					    fout_xml << "            <largest_overlap_name>" << *((sigs[j])->getOverlapNameAt(max_overlap_index)) << "</largest_overlap_name>" << endl;
					    sprintf(overlap, "%.5f", max_overlap_value);
					    fout_xml << "            <largest_overlap>" << overlap << "</largest_overlap>" << endl;
					} else {
					    fout_xml << "            <largest_overlap_name>N/A</largest_overlap_name>" << endl;
					    fout_xml << "            <largest_overlap>N/A</largest_overlap>" << endl;
					}
*/
					fout_xml << "        </signature>" << endl;
				}
			}
			fout_xml << "    </level>" << endl;
		}
		for (Int_t i = 0; i < num_csigs; i++ ) {
                    fout_xml << "    <cplx_signature>" << endl;
                    fout_xml << "        <type>" << *((sigs_c[i])->getType()) << "</type>" << endl;
                    sprintf(temp1, "%.2e", (sigs_c[i])->getEff() );
		    sprintf(eff_err, "%.2e", (sigs_c[i])->getEffErr() );
                    sprintf(temp2, "%.2e", (sigs_c[i])->getRate() );
		    sprintf(rate_err, "%.2e", (sigs_c[i])->getRateErr() );
                    sprintf(temp3, "%.2e", (sigs_c[i])->getPrescale() );
                    sprintf(temp4, "%.2e", (sigs_c[i])->getPeff() );
                    sprintf(unique_rate, "%.2e", (float)(sigs_c[i])->getUniqueRate() );

                    if ((sigs_c[i])->getPrescale() > 0) {
		        sprintf(peff_err, "%.2e", ((sigs_c[i])->getEffErr())/((sigs_c[i])->getPrescale()) );
                    } else {
		        sprintf(peff_err, "%.2e", 0.0 );
                    }
                    fout_xml << "        <sig_name>" << *((sigs_c[i])->getName()) << "</sig_name>" << endl;
                    fout_xml << "        <prescale>" << temp3 << "</prescale>" << endl;
                    fout_xml << "        <efficiency>" << temp1 << "</efficiency>" << endl;
                    fout_xml << "        <efficiency_err>" << eff_err << "</efficiency_err>" << endl;
                    fout_xml << "        <prescaled_efficiency>" << temp4 << "</prescaled_efficiency>" << endl;
                    fout_xml << "        <prescaled_efficiency_err>" << peff_err << "</prescaled_efficiency_err>" << endl;
                    fout_xml << "        <rate>" << temp2 << "</rate>" << endl;
                    fout_xml << "        <rate_err>" << rate_err << "</rate_err>" << endl;
                    fout_xml << "        <unique_rate>" << unique_rate << "</unique_rate>" << endl;
                    fout_xml << "        <components>" << endl;
                    for(Int_t j = 0; j < (Int_t)(sigs_c[i]->getSigNamesSize()); j++ ) {
                        fout_xml << "            <sig>" << endl;
                        fout_xml << "                <sig_name>" << *(sigs_c[i]->getSigAt(j)) << "</sig_name>" << endl;
                        fout_xml << "            </sig>" << endl;
                    }
                    fout_xml << "        </components>" << endl;
                    
/*                    fout_xml << "            <overlap>" << endl;
                    max_overlap_value = 0.0;
                    max_overlap_index = -1;
                    temp_index = (sigs_c[i])->getOverlapSize();
                    temp_var1 = (sigs_c[i])->getNumPassed();
                    for(Int_t k = 0; k < temp_index; k++) {
                            fout_xml << "                <sig>" << endl;
                            fout_xml << "                    <sig_name>" << *((sigs_c[i])->getOverlapNameAt(k)) << "</sig_name>" << endl;
                            if (temp_var1 > 0) {
                                    if (((sigs_c[i])->getOverlapAt(k) >= max_overlap_value) && (*((sigs_c[i])->getName()) != *((sigs_c[i])->getOverlapNameAt(k)))) {
                                            max_overlap_value = (sigs_c[i])->getOverlapAt(k);
                                            max_overlap_index = k;
                                    }
                                    sprintf(overlap, "%.5f", (sigs_c[i])->getOverlapAt(k) );
                            } else {
                                    sprintf(overlap, "N/A");
                            }
                            fout_xml << "                    <sig_overlap>" << overlap << "</sig_overlap>" << endl;
                            fout_xml << "                </sig>" << endl;
                    }
                    fout_xml << "            </overlap>" << endl;
                    if (max_overlap_index != -1) {
                        fout_xml << "            <largest_overlap_name>" << *((sigs_c[i])->getOverlapNameAt(max_overlap_index)) << "</largest_overlap_name>" << endl;
                        sprintf(overlap, "%.5f", max_overlap_value);
                        fout_xml << "            <largest_overlap>" << overlap << "</largest_overlap>" << endl;
                    } else {
                        fout_xml << "            <largest_overlap_name>N/A</largest_overlap_name>" << endl;
                        fout_xml << "            <largest_overlap>N/A</largest_overlap>" << endl;
                    }
*/

                    fout_xml << "    </cplx_signature>" << endl;
		}
		//for (Int_t i = 0; i < num_streams; i++ ) {
                //    fout_xml << "    <stream>" << endl;
                //    fout_xml << "        <type>" << *((sigs_streams[i])->getType()) << "</type>" << endl;
                //    sprintf(temp1, "%.2e", (sigs_streams[i])->getEff() );
		//    sprintf(eff_err, "%.2e", (sigs_streams[i])->getEffErr() );
                //    sprintf(temp2, "%.2e", (sigs_streams[i])->getRate() );
		//    sprintf(rate_err, "%.2e", (sigs_streams[i])->getRateErr() );
		//    sprintf(unique_rate, "%.2e", (sigs_streams[i])->getUniqueRate() );
                //    sprintf(temp3, "%.2e", (sigs_streams[i])->getPrescale() );
                //    sprintf(temp4, "%.2e", (sigs_streams[i])->getPeff() );
                //    if ((sigs_streams[i])->getPrescale() > 0) {
		//        sprintf(peff_err, "%.2e", ((sigs_streams[i])->getEffErr())/((sigs_streams[i])->getPrescale()) );
                //    } else {
		//        sprintf(peff_err, "%.2e", 0.0 );
                //    }
                //    fout_xml << "        <sig_name>" << *((sigs_streams[i])->getName()) << "</sig_name>" << endl;
                //    fout_xml << "        <prescale>" << temp3 << "</prescale>" << endl;
                //    fout_xml << "        <efficiency>" << temp1 << "</efficiency>" << endl;
                //    fout_xml << "        <efficiency_err>" << eff_err << "</efficiency_err>" << endl;
                //    fout_xml << "        <prescaled_efficiency>" << temp4 << "</prescaled_efficiency>" << endl;
                //    fout_xml << "        <prescaled_efficiency_err>" << peff_err << "</prescaled_efficiency_err>" << endl;
                //    fout_xml << "        <rate>" << temp2 << "</rate>" << endl;
                //    fout_xml << "        <rate_err>" << rate_err << "</rate_err>" << endl;
		//    fout_xml << "        <unique_rate>" << unique_rate << "</unique_rate>" << endl;
                //    fout_xml << "        <components>" << endl;
                //    temp_arrS = (sigs_streams[i])->getSigNames();
                //    for(Int_t j = 0; j < (temp_arrS->getSize()); j++ ) {
                //        fout_xml << "            <sig>" << endl;
                //        fout_xml << "                <sig_name>" << *(temp_arrS->getAt(j)) << "</sig_name>" << endl;
                //        fout_xml << "            </sig>" << endl;
                //    }
                //    fout_xml << "        </components>" << endl;
                //    
                //    fout_xml << "        <overlap>" << endl;
                //    max_overlap_value = 0.0;
                //    max_overlap_index = -1;
                //    temp_index = (sigs_streams[i])->getOverlapSize();
                //    temp_var1 = (sigs_streams[i])->getNumPassed();
                //    for(Int_t k = 0; k < temp_index; k++) {
                //            fout_xml << "            <sig>" << endl;
                //            fout_xml << "                <sig_name>" << *((sigs_streams[i])->getOverlapNameAt(k)) << "</sig_name>" << endl;
                //            if (temp_var1 > 0) {
                //                    if (((sigs_streams[i])->getOverlapAt(k) >= max_overlap_value) && (*((sigs_streams[i])->getName()) != *((sigs_streams[i])->getOverlapNameAt(k)))) {
                //                            max_overlap_value = (sigs_streams[i])->getOverlapAt(k);
                //                            max_overlap_index = k;
                //                    }
                //                    sprintf(overlap, "%.5f", (sigs_streams[i])->getOverlapAt(k) );
                //            } else {
                //                    sprintf(overlap, "N/A");
                //            }
                //            fout_xml << "                <sig_overlap>" << overlap << "</sig_overlap>" << endl;
                //            fout_xml << "            </sig>" << endl;
                //    }
                //    fout_xml << "        </overlap>" << endl;
                //    if (max_overlap_index != -1) {
                //        fout_xml << "        <largest_overlap_name>" << *((sigs_streams[i])->getOverlapNameAt(max_overlap_index)) << "</largest_overlap_name>" << endl;
                //        sprintf(overlap, "%.5f", max_overlap_value);
                //        fout_xml << "        <largest_overlap>" << overlap << "</largest_overlap>" << endl;
                //    } else {
                //        fout_xml << "        <largest_overlap_name>N/A</largest_overlap_name>" << endl;
                //        fout_xml << "        <largest_overlap>N/A</largest_overlap>" << endl;
                //    }

                //    fout_xml << "    </stream>" << endl;
		//}

	}
	fout_xml << "</trigger>" << endl;
        
}
