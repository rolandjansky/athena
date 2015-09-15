/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* checks differences in chains
 between two sets of expert monitoring files
 
 Original version from N. Sinev
 
 Upated by Strom,       05-10-09
 Upated by czodrows     08-05-14 - to ensure merged HLT compatibility
 Upated by czodrows     12-12-14 - add the L1 Chains added by Joerg
 
 
 Compare both total chain acceptance and results
 of individual TEs
 
 #to test "offline" do:
 tol=10
 
 ref=/path/to/a/refence/expert/monitoring/file.root
 #in my local case
 ref=r.root
 level="HLT"
 echo -e ".x TrigTest_CheckCounts.C($tol,\042${ref}\042,\042${level}\042) \n .q " | root -b 
 */



#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1.h>
#include <iostream> 
#include <iomanip>
#include <Riostream.h> 
#include <RVersion.h>

void  TrigTest_CheckCounts(Int_t toler, const std::string& reffile, std::string level)
{
    Bool_t debug = 0;  
    Int_t ncheck = 0;
    TString *hnames[5];
    TString *hanames[5];
    TString *fnames[5];
    Int_t EXT_BNS = 500;
    
    if(!(level=="L2"||level=="EF"||level=="BOTH"||level=="HLT")){
        cout << "checkcounts: ERROR : Invalid trigger level given as input: " << level << endl;
        cout << "Next time please choose between: L2,EF,HLT and BOTH - goodbye! "<< endl;
        exit(-1);
    }
    
    TFile *rf = new TFile("expert-monitoring.root");
    if(rf->IsZombie()){
        cout << "checkcounts: ERROR : Unable to open expert-monitoring.root" << endl;
        exit(-1);
    }
    
    cout << "using reference file: " << reffile << endl;
    TFile *ref = new TFile(reffile.c_str());
    if(ref->IsZombie()){
        cout << "checkcounts: ERROR : Unable to open reference file: " << reffile << endl;
        exit(-1);
    }
    
    TDirectory *l1_dir_cur =  rf->GetDirectory("../CTPSimulation");///L1ItemsAV");
    TDirectory *l1_dir_ref = ref->GetDirectory("../CTPSimulation");///L1ItemsAV");
    bool run_l1=0;
    if((l1_dir_cur)&&(l1_dir_ref)) run_l1=1;
    
    
    
    ///this is option for possible direct calls of levels
    if(level=="L2"||level=="EF"||level=="HLT"){
        hnames[0] =new TString("ChainAcceptance");
        // look for alternate names : hanames
        hanames[0]=new TString("SignatureAcceptance_runsummary");
        //hanames[0]=new TString("ChainAcceptance_runsummary");
        fnames[0] =new TString("TrigSteer_"+level);
        
        hnames[1] =new TString("NumberOfActiveTEs");
        // look for alternate names : hanames
        hanames[1]=new TString("NumberOfActiveTEsPerEvent");
        //hanames[1]=new TString("NumberOfActiveTEs_runsummary");
        fnames[1] =new TString("TrigSteer_"+level);
        ncheck =2;
        
        if(run_l1) {
            hnames[2] =new TString("L1ItemsAV");
            hanames[2]=new TString("L1ItemsAV");
            fnames[2] =new TString("CTPSimulation");
            ncheck =3;
        }
        cout << "Checking " << level << " trigger counts" << endl;
    }
    
    if(level=="BOTH"){   // "BOTH"
        TDirectory *hlt_dir = rf->GetDirectory("TrigSteer_HLT");
        if(hlt_dir){
            level = "HLT";
            //cout << "LEVEL: " << level << endl;
            hnames[0] =new TString("ChainAcceptance");
            // look for alternate names : hanames
            hanames[0]=new TString("SignatureAcceptance_runsummary");
            fnames[0] =new TString("TrigSteer_HLT");
            
            // look at TE details
            hnames[1] =new TString("NumberOfActiveTEs");
            // look for alternate names : hanames
            hanames[1]=new TString("NumberOfActiveTEsPerEvent");
            fnames[1] =new TString("TrigSteer_HLT");
            ncheck =2;
            if(run_l1) {
                hnames[2] =new TString("L1ItemsAV");
                hanames[2]=new TString("L1ItemsAV");
                fnames[2] =new TString("CTPSimulation");
                ncheck =3;
            }
            cout << "Checking HLT trigger counts" << endl;
        } else {
            //cout << "TrigLevel BOTH was given -- please ignore the following info line if this is a release < 18 "<< endl; 
            //cout << "TrigSteer_HLT does not exist in expert-monitoring.root --> falling back to L2 and EF check" << endl;
            hnames[0] =new TString("ChainAcceptance");
            hanames[0]=new TString("SignatureAcceptance_runsummary");
            //hanames[0]=new TString("ChainAcceptance_runsummary");
            fnames[0] =new TString("TrigSteer_L2");
            
            hnames[1] =new TString("ChainAcceptance");
            // look for alternate names : hanames
            hanames[1]=new TString("SignatureAcceptance_runsummary");
            //hanames[1]=new TString("ChainAcceptance_runsummary");
            fnames[1] =new TString("TrigSteer_EF");
            
            // look at TE details
            hnames[2] =new TString("NumberOfActiveTEs");
            // look for alternate names : hanames
            hanames[2]=new TString("NumberOfActiveTEsPerEvent");
            //hanames[2]=new TString("NumberOfActiveTEs_runsummary");
            fnames[2] =new TString("TrigSteer_L2");
            
            hnames[3] =new TString("NumberOfActiveTEs");
            // look for alternate names : hanames
            hanames[3]=new TString("NumberOfActiveTEsPerEvent");
            //hanames[3]=new TString("NumberOfActiveTEs_runsummary");
            fnames[3] =new TString("TrigSteer_EF");
            ncheck =4;
            if(run_l1) {
                hnames[4] =new TString("L1ItemsAV");
                hanames[4]=new TString("L1ItemsAV");
                fnames[4] =new TString("CTPSimulation");
                ncheck =5;
            }
            cout << "Checking L2 and EF trigger counts " << endl;
        }
    } //end if(level=="BOTH")
    
    if(ncheck==0){
        cout << "checkcounts: ERROR : This should never happen some config error involving ncheck and *level - please contact/assign a TrigValTools developer - exiting CheckCounts!" << endl;
        exit(-1);
    }
    
    Bool_t DIRERROR=0;
    //check if any TrigSteer directory exists - and also if they are the correct one(s) 
    TDirectory *hlt_dir_ref = ref->GetDirectory("TrigSteer_HLT");
    TDirectory *hlt_dir_cur =  rf->GetDirectory("TrigSteer_HLT");
    
    TDirectory *ef_dir_ref = ref->GetDirectory("TrigSteer_EF");
    TDirectory *ef_dir_cur =  rf->GetDirectory("TrigSteer_EF");
    TDirectory *l2_dir_ref = ref->GetDirectory("TrigSteer_L2");
    TDirectory *l2_dir_cur =  rf->GetDirectory("TrigSteer_L2");
    //        TDirectory *l1_dir_cur =  rf->GetDirectory("../CTPSimulation");///L1ItemsAV");
    //        TDirectory *l1_dir_ref = ref->GetDirectory("../CTPSimulation");///L1ItemsAV");
    
    if(level=="L2"){ 
        if(!(l2_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_L2 " << endl; 
        if(!(l2_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_L2 " << endl;
        if(!(l2_dir_ref)||!(l2_dir_cur))  DIRERROR=1;
    }
    if(level=="EF"){     
        if(!(ef_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_EF " << endl;
        if(!(ef_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_EF " << endl;
        if(!(ef_dir_ref)||!(ef_dir_cur)) DIRERROR=1;
    }
    if(level=="HLT"){
        //            if(!(l1_dir_cur)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: CTPSimulation " << endl; 
        //            if((l1_dir_cur)) cout << "good reference file: " << reffile << " : Does  contain the directory: CTPSimulation " << endl; 
        if(!(hlt_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_HLT " << endl;
        if(!(hlt_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_HLT " << endl;
        if(!(hlt_dir_ref)||!(hlt_dir_cur)) DIRERROR=1;
    }
    if(level=="BOTH"){
        //            if(!(l1_dir_cur)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: CTPSimulation " << endl; 
        //            if((l1_dir_cur)) cout << "good reference file: " << reffile << " : Does  contain the directory: CTPSimulation " << endl; 
        //check for existence of TrigSteer_HLT in either of the 2 files
        if(!(hlt_dir_ref)||!(hlt_dir_cur)){ //not found -> check for L2 and EF TrigSteer_ directories
            if(!(l2_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_L2 " << endl; 
            if(!(l2_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_L2 " << endl;
            if(!(l2_dir_ref)||!(l2_dir_cur))  DIRERROR=1;
            if(!(ef_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_EF " << endl;
            if(!(ef_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_EF " << endl;
            if(!(ef_dir_ref)||!(ef_dir_cur)) DIRERROR=1;
        }
        //check for existence of L2 and EF TrigSteer_ directories in either of the 2 files
        if(!(ef_dir_ref)||!(ef_dir_cur)||!(l2_dir_ref)||!(l2_dir_cur)){ //not found -> check for TrigSteer_HLT dirs
            if(!(hlt_dir_ref)) cout << "Bad reference file: " << reffile << " : Does not contain the directory: TrigSteer_HLT " << endl;
            if(!(hlt_dir_cur)) cout << "Bad expert-monitoring.root file: Does not contain the directory: TrigSteer_HLT " << endl;
            if(!(hlt_dir_ref)||!(hlt_dir_cur)) DIRERROR=1;
        }
    }
    
    if(DIRERROR){
        cout << "checkcounts: ERROR : Inconsitency/Problem with the TrigSteer_ directories in the reference and/or expert-monitoring.root file detected - exiting CheckCounts!" << endl;
        exit(-1);
    }
    // all seems fine with the 2 files lets go on then ... 
    
    
    TString **xlabels;  
    Double_t *newbc;
    Double_t *oldbc;
    Double_t *newbct;
    Double_t *oldbct;
    Bool_t *manam;
    Int_t nbins = 0;
    Double_t nrnnew = 0.;
    Double_t nrnold = 0.;
    for(Int_t index=0; index<ncheck; index++)
    {
        
        rf = new TFile("expert-monitoring.root");
        if( rf->IsZombie()){
            cout << "checkcounts test warning: Unable to open expert-monitoring.root file" << endl;
            exit(-1);
        }
        TString *fname = fnames[index];  
        rf->cd(*fname);
        TH1 *tchain;
        TString* hname  = hnames[index];
        if(debug){
            cout << "looking for " << *hname << endl;
        }
        gDirectory->GetObject(*hname,tchain);
        if(tchain==0)
        {
            //look for alternate names
            hname = hanames[index];
            if(debug){
                cout << "looking for alternate " << *hname << endl;
            }
            gDirectory->GetObject(*hname,tchain);
            if(tchain==0)
            {
                cout << "checkcounts test warning : no " << *hname 
                << " in expert-monitoring.root" << endl;      
                continue;
            }   
        }
        cout << endl << *hname << " in directory " << *fname << endl << endl;
        nbins = tchain->GetNbinsX()+1;
        
        if(debug) 
        {
            cout << "bins in test: " << nbins << endl;
        }
        newbc = new Double_t[nbins+EXT_BNS];
        oldbc = new Double_t[nbins+EXT_BNS];
        
        #if ROOT_VERSION_CODE >= ROOT_VERSION(5,99,0)
        //new code for root 6                                                                                                                                                                             
        TString* xlabels[nbins+EXT_BNS];
        #else
        //old code for root 5                                                                                                                                                                       
        TString* xlabels= new TString[nbins+EXT_BNS];
        #endif
        
        manam = new Bool_t[nbins];
        for(Int_t i=0; i<nbins+EXT_BNS; i++)
        {    
            if(i<nbins)
            {     
                xlabels[i] = new TString(tchain->GetXaxis()->GetBinLabel(i));
                while (xlabels[i]->Contains(" ")) xlabels[i]->Chop(); 
                manam[i]=false;
            }
        }
        for(Int_t i=0; i<nbins+EXT_BNS; i++)
        {    
            
            newbc[i]=0.;
            oldbc[i]=0.;
        }
        
        for(Int_t i=0; i<nbins; i++) newbc[i]=tchain->GetBinContent(i);
        TH1 *echist;
        gDirectory->GetObject("NInitialRoIsPerEvent",echist);  
        if (*fname=="CTPSimulation"){
            const char * olddir ;
            olddir = gDirectory->GetPath() ;
            gDirectory->GetObject("../"+(*fnames[0])+"/NInitialRoIsPerEvent",echist);   
            gDirectory->cd(olddir);
        }
        // look for old sytle names if new one is not there
        //        if(echist == 0 ){
        //            `    if(fname->Contains("_EF"))
        //                `   {
        //                    gDirectory->GetObject("N_Initial_RoI_in_Event_EF",echist);   
        //                }            
        //            if(fname->Contains("_L2"))
        //            {
        //                gDirectory->GetObject("N_Initial_RoI_in_Event_L2",echist);   
        //            }
        if(echist==0)
        {    
            cout << "checkcounts FAILURE : can't determine number of events processed" << endl;      
            continue;
        }
        //        } 
        nrnnew=echist->GetEntries();
        cout << "Number of events processed in test: " << nrnnew << endl;               
        rf->Close();
        TFile *orf = new TFile(reffile.c_str());
        if( orf->IsZombie()){
            cout << "checkcounts test warning: Unable to open reference file" 
            << reffile << endl;
            exit(-1);
        }
        
        //  allow for different names in old root file
        orf->cd(*fname);
        TH1 *otchain;
        TString *honame = hnames[index];
        if(debug){cout << "looking for " << *honame << endl; }
        gDirectory->GetObject(*honame,otchain);
        if(otchain==0)
        {    
            // look for alternate names
            honame = hanames[index];
            if(debug){ cout << "looking for alternate " << *honame << endl; }
            gDirectory->GetObject(*honame,otchain);
            if(otchain==0)
            {
                cout << "checkcounts test warning : no " << *honame << " in " 
                << reffile << endl;      
                continue;
            }
        }     
        Int_t onbins = otchain->GetNbinsX()+1;
        if(debug){ cout << "bins in reference: " << onbins << endl; }
        if(nbins != onbins)
        {     
            cout << "Different number of bins in histos! Test: " << nbins << " ref: " << onbins << endl;
        }
        Int_t nexlb=0;
        for(Int_t i=0; i<onbins; i++)
        {
            Int_t bi = -1;
            TString *obna = new TString(otchain->GetXaxis()->GetBinLabel(i));
            while (obna->Contains(" ")) obna->Chop();
            if( debug ){ cout << i << "reference bin label: " << *obna << endl; }
            for(Int_t si=0; si<nbins+nexlb; si++)
            {
                if(xlabels[si]->CompareTo(*obna) == 0) bi=si;   
            }
            if(bi!=-1)
            {               
                oldbc[bi]=otchain->GetBinContent(i);
                if(bi < nbins) manam[bi]=true;
            }
            if(bi==-1)
            {     
                cout << "ref chain name " << *obna << "  does not exist in test" << endl;
                xlabels[nbins+nexlb] = obna;
                oldbc[nbins+nexlb]=otchain->GetBinContent(i);
                nexlb++;
                if(nexlb > EXT_BNS-1)
                {    
                    nexlb = EXT_BNS-1;
                    cout << "need more bins for new labels" << endl;
                }
            }
        }
        gDirectory->GetObject("NInitialRoIsPerEvent",echist); 
        if (*fname=="CTPSimulation"){
            const char * olddir ;
            olddir = gDirectory->GetPath() ;
            gDirectory->GetObject("../"+(*fnames[0])+"/NInitialRoIsPerEvent",echist);    
            gDirectory->cd(olddir);
        }
        
        // look for old sytle names if new one is not there
        //        if(echist == 0 ){
        //            if(fname->Contains("_EF"))
        //            {
        //                gDirectory->GetObject("N_Initial_RoI_in_Event_EF",echist);   
        //            }            
        //            if(fname->Contains("_L2"))
        //            {
        //                gDirectory->GetObject("N_Initial_RoI_in_Event_L2",echist);   
        //            }
        if(echist==0){
            cout << "failed to find number of events in reference" << endl;
            continue;
        }
        //        }
        nrnold=echist->GetEntries();
        cout << "Number of events in reference: " << nrnold << endl;               
        orf->Close();
        if(abs(nrnnew-nrnold) < toler + 1)
        {       
            Int_t ngood=0;
            Int_t nbad=0;
            for(Int_t i=0; i<nbins; i++)
            {
                Int_t cndif = 0;
                if(debug){ cout << i << "reference: " << oldbc[i] << "test: " << newbc[i] << endl; }  
                cndif =  newbc[i]-oldbc[i];      
                if((cndif < -toler) || (cndif > toler)) nbad++;
                else ngood++;
            }
            if(nbad !=0)
            { 
                cout << "checkcounts test warning : trigger counts outside tolerance: " << endl;     
                // set label width to 50 -- a compromise between reading the differences and allowing the full name
                cout << "Label" << setw(50) << 
                "Ref n" << setw(10) << "Test" << setw(15) << "test-ref" << endl;  
                for(Int_t i=1; i<nbins+nexlb; i++)
                {
                    TString *label = xlabels[i];
                    label->Resize(50);
                    Int_t cndif = 0;
                    if(i<nbins)
                    {
                        cndif =  newbc[i]-oldbc[i];
                        if(debug){cout << *label << "reference:  " << oldbc[i]  << "test: " << newbc[i] << endl; }
                        if((cndif < -toler) || (cndif > toler)) 
                        {
                            if(manam[i])
                            {          
                                cout << *label<< setw(10) << oldbc[i] << setw(10) << newbc[i] << setw(15) << cndif << endl;
                            }
                            else
                            {
                                cout << *label << setw(10) << "no_ref" << setw(10) << newbc[i]  << endl;  
                            }
                        }        
                    }
                    else   
                    {
                        cout << *label<< setw(10) << oldbc[i] << setw(10) << "no data"  << endl;
                    }       
                } // end for(Int_t i=1; i<nbins+nexlb; i++)
                //if(toler > 0 ){
                if( hname->Contains("TE") ){
                    cout << "TE (trigger element)" << endl;
                    cout << "Fraction inside tolerance: " 
                    << " "  <<  ngood << "/" 
                    <<  ngood+nbad << endl;
                } else {
                    cout << "RAW counts" << endl;
                    cout << "Fraction inside tolerance: " 
                    << " "  <<  ngood << "/" 
                    <<  ngood+nbad << endl;
                }
                cout << "----------------------------------------------------------------" << endl;
                //}
            }// end if(nbad !=0)
            else
            {
                cout << " checkcounts: SUCCESS : all "<< nbins << " trigger counts within tolerance" << endl;   
            }        
        } else {
            cout<< " checkcounts: number of events in reference and test different by more than tolerance" << endl;
        } // end if(abs(nrnnew-nrnold) < 2.)
    } // end for(Int_t index=0; index<2; index++)                           
} // end of function      
