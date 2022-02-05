/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* HLTMET Post Processing Method: Peform bin-wise division for each Phi.vs.Eta maps of MET, SumEt, SumE
 *                                with Phi.vs.Eta map of N to compute <MET>(eta,phi), <SumEt>(eta,phi)
 *                                and <SumE>(eta,phi)
 * Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 * Date     : Feb 2010
 */

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"

namespace dqutils {

  void
  MonitoringFile::HLTMETAveragePhivsEtaMaps(TFile* f, TString& run_dir) {

    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> HLTMETAveragePhivsEtaMaps: <Quantity(eta,phi)>  = Quantity(eta,phi)/N(eta,phi) " << std::endl;

    f->cd("/");
    TIter next_run ( f->GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )  { continue;}


      std::string run_dir2 = run_dir.Data();

      // all merged root files have the structure "rootfile.root:/run_NNNNNNN"
      // use that to extract run number
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      // begin HLTMET
      // note 1: prefix all dirs and hists with '/'
      // note 2: missing dir => return
      // note 3: missing hist => continue
      TString hlt_top = run_dir + "/HLT"; // toplevel
      TString met_efdir = "/EFMissingET_Fex"; // EF dir
      
      std::vector<TString> met_fexs, hist_numr; 
     
      // expect the following fex dirs
      met_fexs.push_back("/METMon");
      //met_fexs.push_back("/METMon_FEB");
      //met_fexs.push_back("/METMon_allCells");

      // check if fex dirs are in hlt 
      for( std::vector<TString>::iterator it = met_fexs.begin(); it != met_fexs.end(); ++it ) {
        TString theHistDir = hlt_top + *it;
        TDirectory* dir = f->GetDirectory(theHistDir);
        if(!dir){
          std::cerr << "--> HLTMETAveragePhivsEtaMaps: directory " << theHistDir << " not found" << std::endl;
          return;
        }
        // expect the EF dir inside each fex dir
        theHistDir += met_efdir;
        dir = f->GetDirectory(theHistDir);
        if(!dir){
          std::cerr << "--> HLTMETAveragePhivsEtaMaps: directory " << theHistDir << " not found" << std::endl;
          return;
        }
      }

      // MEt, SumEt, SumE of components (numerator)
      // compSumEt_lin_EtaPhi_00 etc..
      hist_numr.push_back("/compEt_lin_");
      hist_numr.push_back("/compSumEt_lin_");
      hist_numr.push_back("/compSumE_lin_"); // need to plot SumE on linear scale (todo)
      
      // components N (denominator)
      TString hist_denr = "/compN_" ;
      
      // type (eta,phi map)
      TString hist_suffix = "EtaPhi_"; // phi vs. eta map

      // each component a 2d hist. get all components
      unsigned int comp_num = 25; // 25 components
      
      // we have all dirs, get the component histograms
      for( std::vector<TString>::iterator itFex = met_fexs.begin(); itFex != met_fexs.end(); ++itFex ) {
        for( std::vector<TString>::iterator itNum = hist_numr.begin(); itNum != hist_numr.end(); ++itNum ) {
          for(unsigned int icomp = 0; icomp < comp_num; icomp++ ) {
            TH2F *hnum(0), *hden(0);
            
            // prepend histogram name with path and append with suffix [_00 .., _24 for each component]
            TString thePath = hlt_top + (*itFex) + met_efdir;
            TString numHist = (*itNum) + hist_suffix + TString(Form("%02u",icomp));
            TString denHist = hist_denr + hist_suffix + TString(Form("%02u",icomp));
            TString numPath = thePath + numHist;
            TString denPath = thePath + denHist;

            // test if histograms are present
            if(!f->Get(numPath)) { if(dbgLevel) std::cerr << "--> HLTMETAveragePhivsEtaMaps: no histogram " << numPath << std::endl; continue;}
            if(!f->Get(denPath)) { if(dbgLevel) std::cerr << "--> HLTMETAveragePhivsEtaMaps: no histogram " << denPath << std::endl; continue;}
            
            // get histograms
            hnum = (TH2F *) (f->Get(numPath));
            hden = (TH2F *) (f->Get(denPath));

            // get directory of histograms
            TDirectory* dir = f->GetDirectory(thePath);

            // these are disabled, because we have to worry about updating metadata
            // clone numerator histogram in the same directory; prepend with "avg_" 
            // use numerator to do the job
            //TString avgHist = TString("avg_") + (*itNum) + hist_suffix + TString(Form("%02u",icomp));
            //havg = (TH2F *) (hnum->Clone(avgHist));
            //havg->SetDirectory(dir);

            // divide num by den to get average quantity.
            hnum->Divide(hnum,hden);

            // fix histogram titles
            TString title = hnum->GetTitle();
            title.ReplaceAll(": ",": #LT");
            title.ReplaceAll("(#eta","#GT(#eta");
            hnum->SetTitle(title);
            
            //title = hden->GetTitle();
            ////std::cout << title << std::endl;
            //title.ReplaceAll(" #phi VS #eta",": N(#eta, #phi)");
            //hden->SetTitle(title);
            
            //std::cout << numHist << " div " << denHist << std::endl;
            //std::cout << hnum->GetZaxis()->GetTitle() << "\t";
            //std::cout << hden->GetZaxis()->GetTitle() << std::endl; 
            //std::cout << hnum->GetTitle() << "\t"; 
            //std::cout << hden->GetTitle() << std::endl;
            //std::cout << "-------------------------------------------------------------" << std::endl;
            //getchar();
            // cd() into that directory and save the avg histogram in file
            dir->cd();
            hnum->Write("",TObject::kOverwrite);

          } // done looping over components
        } // done looping over quantities
      } // done looping over directories
      
    } // end while loop over all keys
  } // end method MonitoringFile::HLTMETAveragePhivsEtaMaps

} // end namespace
