/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * LumiWeightHandler.cxx
 *
 *  Created on: Jul 10, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/LumiWeightHandler.h"
#include <cstdlib>

namespace CP {

LumiWeightHandler::LumiWeightHandler():
                                m_user_lumi_weights(){
}
LumiWeightHandler::LumiWeightHandler(LumiWeightHandler::lumimap user_weights):
                                m_user_lumi_weights(user_weights){

}

LumiWeightHandler::~LumiWeightHandler() {
}

void LumiWeightHandler::SetLumiWeights (LumiWeightHandler::lumimap user_weights){
    m_user_lumi_weights = user_weights;
}
bool LumiWeightHandler::LoadLumiFromInput(LumiWeightHandler::lumimap & map, std::string file){

    // start by clearing anything in the weight map so far
    // in case we initialize multiple times for some reason...
    map.clear();


    // read what is provided in the file
    TFile* fin = TFile::Open(file.c_str(),"READ");
    if (!fin) {
        Error("MuonEfficiencyScaleFactors","unable to open %s for lumi input",file.c_str());
        return false;
    }
    TTree* intree=0;
    fin->GetObject("LumiData",intree);

    // if no Lumi tree is found, we assume that the SF are not binned in time
    if (!intree){
        map["All"] = 1;
        //            Error("MuonEfficiencyScaleFactors","unable to read lumi data tree from %s",file.c_str());
        fin->Close();
        return true;
    }
    std::string *period = 0;
    float lumi = 0;
    intree->SetBranchAddress("Period",&period);
    intree->SetBranchAddress("IntLumi",&lumi);

    lumimap input_lumi_weights;
    for (int t = 0; intree->GetEntry(t);t++){
        //          input_lumi_weights[PeriodName(*period)] += lumi;
        input_lumi_weights[*period] += lumi;
    }

    // if no further user specification, we use this information as it is
    // also cater for the case that the SF are not binned in time
    if (m_user_lumi_weights.empty() ||
            (input_lumi_weights.size() == 1 && input_lumi_weights.find("All") != input_lumi_weights.end())
    ){
        map = input_lumi_weights;
    }
    // in case of user specified inputs, we use the keys in the map read from the file
    // to 'translate' the user input into a format corresponding to the input file histograms
    else {
        // this avoids repeated warnings inside TranslateLumiElement
        bool warned = false;

        for (ilumimap iu = m_user_lumi_weights.begin(); iu != m_user_lumi_weights.end();iu++){
            lumimap iu_add_map = TranslateLumiElement(iu,input_lumi_weights,warned,file);
            for (ilumimap iuel = iu_add_map.begin(); iuel != iu_add_map.end();iuel++){
                map[iuel->first]+=iuel->second;
            }
        }
    }
    fin->Close();
    return true;
}

LumiWeightHandler::lumimap LumiWeightHandler::TranslateLumiElement(
        LumiWeightHandler::ilumimap & iel,
        LumiWeightHandler::lumimap & imap,
        bool & warned_the_user, std::string filename){

    // this is a bit complicated. The user may provide per period or per run lumis,
    // and the SF may also be binned either way. We want to cater for all cases.
    // especially for early 2015, per run SF might be interesting.

    std::map<std::string,double> out;
    std::string arg = iel->first;

    // check if our input is per run or per period
    bool input_per_period = (PeriodName(imap.begin()->first) == imap.begin()->first);
    bool user_per_period = (PeriodName(arg) == arg);

    // user format and format on file match: replace the lumis we have by the user ones
    // only need to look for unsupported runs/periods
    if ((input_per_period && user_per_period ) || (!input_per_period && !user_per_period )){
        if (imap.find(arg)!= imap.end()){
            out[arg]=iel->second;
        }
        else Warning("MuonEfficiencyScaleFactors","%s\n%s",
                Form("Run/period number %s encountered in user specified lumi - not covered by input file %s",arg.c_str(),filename.c_str()),
                "Will ignore this lumi contribution when averaging over the luminosities for this file");
    }

    // per run lumis by the user, per period in the SF histos
    // -> replace each period by the sum of corresponding user runs
    // also tell the user that the SF use different time bins
    else if (input_per_period && !user_per_period){
        if (!warned_the_user){
            Warning("MuonEfficiencyScaleFactors",
                    "You provided per run lumis, but input file %s provides per period histos. \n %s",filename.c_str(),
                    "For this file, will translate your inputs to per period lumis for you");
            warned_the_user = true;
        }
        if (PeriodName(arg) == "Unknown"){
            Warning("MuonEfficiencyScaleFactors","%s\n%s",
                    Form("run number %s encountered in user specified lumi - not covered by input file %s",arg.c_str(),filename.c_str()),
                    "For this file, will ignore this lumi contribution when averaging over the luminosities");
        }
        else out[PeriodName(arg)] = iel->second;
    }
    // Per period lumis by the user, per run in the SF histos.
    // Here, we scale all our runs by the ratio of period lumis.
    // This is not entirely precise, so we also throw a warning.
    else if (!input_per_period && user_per_period ){
        if (!warned_the_user){
            Warning("MuonEfficiencyScaleFactors",
                    "You provided period lumis, but the input file %s is per run.\n%s", filename.c_str(),
                    "For this file, will scale all the runs in one period by the same fraction, based on your input.");
            warned_the_user = true;
        }
        // first, add up the period lumi in our per run file
        double my_period_lumi=0;
        for (ilumimap iin = imap.begin();iin != imap.end();++iin){
            if (PeriodName(iin->first)==arg) my_period_lumi += iin->second;
        }
        // then scale all our runs by the corresponding ratio
        if (my_period_lumi == 0){
            Warning("MuonEfficiencyScaleFactors",
                    "No runs from period %s supported by the SF in the file %s.\n %s",arg.c_str(),filename.c_str(),
                    "Will ignore this lumi contribution when averaging over the luminosities for this file");
            return out;
        }
        else {
            for (ilumimap iin = imap.begin();iin != imap.end();++iin){
                if (PeriodName(iin->first)==arg) out[iin->first] = iin->second * iel->second / my_period_lumi;
            }
        }
    }
    return out;

}

std::string LumiWeightHandler::PeriodName (std::string run){
    // also cater for period numbers
    if (std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZAll").find(run.c_str())!= std::string::npos) return run;

    // otherwise, assume the user is providing per-run lumis
    int runno = atoi(run.c_str());
    // 2012
    if (200804 <= runno && runno <=201556) return "A";
    else if (202660 <= runno && runno <=205113) return "B";
    else if (206248 <= runno && runno <=207397) return "C";
    else if (207447 <= runno && runno <=209025) return "D";
    else if (209074 <= runno && runno <=210308) return "E";
    else if (211522 <= runno && runno <=212272) return "G";
    else if (212619 <= runno && runno <=213359) return "H";
    else if (213431 <= runno && runno <=213819) return "I";
    else if (213900 <= runno && runno <=215091) return "J";
    else if (215414 <= runno && runno <=215643) return "L";
    else if (216399 <= runno && runno <=216432) return "M";

    // 2011
    else if (177531 <= runno && runno <=177965) return "A";
    else if (177986 <= runno && runno <=178109) return "B";
    else if (179710 <= runno && runno <=180481) return "D";
    else if (180614 <= runno && runno <=180776) return "E";
    else if (182013 <= runno && runno <=182519) return "F";
    else if (182726 <= runno && runno <=183462) return "G";
    else if (183544 <= runno && runno <=184169) return "H";
    else if (185353 <= runno && runno <=186493) return "I";
    else if (186516 <= runno && runno <=186755) return "J";
    else if (186873 <= runno && runno <=187815) return "K";
    else if (188902 <= runno && runno <=190343) return "L";
    else if (190503 <= runno && runno <=191933) return "M";

    else return "Unknown";

}


} /* namespace CP */
