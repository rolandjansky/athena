/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// Author: Marc-Andre Dufour
// Date: 26 Nov 2008
//
// Description: Script that will dump TriggerRateTools' results in an HTML file.
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
	ofstream fout_html;
	fout_html.open("TriggerRates.html");

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


	fout_html << "<html>" << endl;
	fout_html << "<head>" << endl;
	fout_html << "<TITLE>Trigger Rates</TITLE>" << endl;
        fout_html << "<body bgcolor=\"#FFFFFF\" text=\"#000000\" link=\"#0000FF\" vlink=\"#000080\" alink=\"#800000\">" << endl;
	fout_html << "<div align=\"center\">" << endl;
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
		fout_html << "<p>Xsection: " << xsec << " barns</p>" << endl;
		fout_html << "<p>Luminosity: " << lum << " barns^-1 sec^-1</p>" << endl;
		fout_html << "<p>Number of events: " << sigs[0]->getNumEvts() << "</p>" << endl;
		for (Int_t i = 0; i < num_levels; i++) {
			sprintf(eff, "%.2f", (levels[i])->getEff() );
		        sprintf(eff_err, "%.2f", (levels[i])->getEffErr() );
			sprintf(prescaled_eff, "%.2f", (levels[i])->getPeff() );
                        if ((levels[i])->getPrescale() > 0) {
		    	    sprintf(peff_err, "%.2f", ((levels[i])->getEffErr())/((levels[i])->getPrescale()) );
                        } else {
		    	    sprintf(peff_err, "%.2f", 0.0 );
                        }
                        if ( (levels[i])->getRate() < 0.005 ) {
			    sprintf(rate, "%s", "< 0.005");
			    sprintf(rate_err, "%s", "");
                        } else {
			    sprintf(rate, "%.2f", (levels[i])->getRate());
		            sprintf(rate_err, "%.2f", (levels[i])->getRateErr() );
                        }
			temp_str = (levels[i])->getLevel();
                        fout_html << "<p><b>" << *(temp_str) << " Rates</b></p>" << endl;
			fout_html << "<table border=\"1\">" << endl;
			fout_html << "    <tr>" << endl;
			fout_html << "        <td align=\"CENTER\"><b>Trigger Name</b></td>" << endl; //Name
			fout_html << "        <td align=\"CENTER\"><b>Cumulative Prescale</b></td>" << endl; //Prescale
			fout_html << "        <td align=\"CENTER\"><b>Pass-Through</b></td>" << endl; //Pass-through
			fout_html << "        <td align=\"CENTER\"><b>Raw # events accepted </b></td>" << endl; //Num passed
			fout_html << "        <td align=\"CENTER\"><b>Rate (Hz)</b></td>" << endl; //Rate
			fout_html << "        <td align=\"CENTER\"><b>Rate error (Hz)</b></td>" << endl; //Rate error
			fout_html << "    </tr>" << endl;
			fout_html << "    <tr>" << endl;
			fout_html << "        <td align=\"LEFT\">Overall "<< *(temp_str) <<"</td>" << endl; //Name
			fout_html << "        <td align=\"CENTER\">---</td>" << endl; //Prescale
			fout_html << "        <td align=\"CENTER\">---</td>" << endl; //Pass-through
			fout_html << "        <td align=\"CENTER\">---</td>" << endl; //Num passed
        		fout_html << "        <td align=\"RIGHT\">" << rate << "</td>" << endl; //Rate
			fout_html << "        <td align=\"RIGHT\">" << rate_err << "</td>" << endl; //Rate error
			fout_html << "    </tr>" << endl;

			for (Int_t j = 0; j < num_sigs; j++) {
				if ( *((sigs[j])->getLevel()) == *temp_str) {
					sprintf(num_passed, "%i", (sigs[j])->getNumPassed() );
					sprintf(eff, "%.2f", (sigs[j])->getEff() );
					sprintf(eff_err, "%.2f", (sigs[j])->getEffErr() );
                                        if ( (sigs[j])->getRate() < 0.005 ) {
			                    sprintf(rate, "%s", "< 0.005");
			                    sprintf(rate_err, "%s", "");
                                        } else {
			                    sprintf(rate, "%.2f", (sigs[j])->getRate());
		                            sprintf(rate_err, "%.2f", (sigs[j])->getRateErr() );
                                        }
					sprintf(prescale, "%i", (sigs[j])->getPrescale() );
					sprintf(passthrough, "%i", (sigs[j])->getPassthrough() );
					sprintf(prescaled_eff, "%.2f", (sigs[j])->getPeff() );
                                        if ((sigs[j])->getPrescale() > 0) {
			                    sprintf(peff_err, "%.2f", ((sigs[j])->getEffErr())/((sigs[j])->getPrescale()) );
                                        } else {
			                    sprintf(peff_err, "%.2f", 0.0 );
                                        }
					if ((sigs[j])->getNumPassed() > 0) {
						sprintf(indep_frac, "%.2f", (sigs[j])->getIndepFrac() );
						sprintf(unique_frac, "%.2f", (sigs[j])->getUniqueFrac() );
						sprintf(unique_rate, "%.2f", (sigs[j])->getUniqueRate() );
					} else {
						sprintf(indep_frac, "%.2f", 0.0);
						sprintf(unique_frac, "%.2f", 0.0);
						sprintf(unique_rate, "%.2f", 0.0);
					}
                			fout_html << "    <tr>" << endl;
                			fout_html << "        <td align=\"LEFT\">"<< *( (sigs[j])->getName() ) <<"</td>" << endl; //Name
                			fout_html << "        <td align=\"RIGHT\">"<< prescale <<"</td>" << endl; //Prescale
                			fout_html << "        <td align=\"RIGHT\">"<< passthrough <<"</td>" << endl; //Passthrough
                			fout_html << "        <td align=\"RIGHT\">"<< num_passed <<"</td>" << endl; //Num passed
                                        fout_html << "        <td align=\"RIGHT\">" << rate << "</td>" << endl; //Rate
                                        fout_html << "        <td align=\"RIGHT\">" << rate_err << "</td>" << endl; //Rate error
                                        fout_html << "    </tr>" << endl;
				}
			}
			fout_html << "</table>" << endl;
		}
	}
	fout_html << "</div>" << endl;
	fout_html << "</body>" << endl;
        
}
