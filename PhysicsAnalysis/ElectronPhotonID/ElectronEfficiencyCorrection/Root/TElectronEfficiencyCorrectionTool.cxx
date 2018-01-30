/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/**
  @class TElectronEfficiencyCorrectionTool
  @brief Calculate the egamma scale factors in pure ROOT
  @author Karsten Koeneke, Felix Buehrer, Kristin Lohwasser
  @date   July 2012
  */


// This class header
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"
// STL includes
#include <iostream>
#include <cmath>
#include "CxxUtils/make_unique.h"
#include <memory>
// ROOT includes
#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TObjString.h"
#include "TMD5.h"

namespace{
    template <class T>
        inline std::string toString(const T& in){
            std::stringstream stream;
            stream << in;
            return stream.str();
        }
    const std::string LowPt_string("LowPt");
}

namespace mapkey{
    enum key{ sf =1,
        stat=2,
        eig=3,
        uncorr=4,	      
        sys=5,
        end=6
    };
    const char* keytostring (int input){
        switch(input){
        case(sf) : 
            return "sf";
        case(stat) : 
            return "stat";
        case(eig) : 
            return "eig";
        case(uncorr) : 
            return "uncorr";
        case(sys) : 
            return "sys";
        }
        return "";
    }
}

Root::TElectronEfficiencyCorrectionTool::TElectronEfficiencyCorrectionTool(const char *name) :
    asg::AsgMessaging(std::string(name)),
    m_doToyMC(false),
    m_doCombToyMC(false),
    m_detailLevel(2),
    m_nToyMC(0),
    m_seed(0),
    m_nSysMax(0),
    m_Rndm()
{
    //Setup the keys
    m_keys.push_back(mapkey::sf);
    m_keys.push_back(mapkey::stat);
    m_keys.push_back(mapkey::eig);
    m_keys.push_back(mapkey::uncorr);
}

Root::TElectronEfficiencyCorrectionTool::~TElectronEfficiencyCorrectionTool() {

    /*
     * Need some gymnastic to make sure that the 
     * TObjArray elements are owned and deleted ...
     */
    for (auto  &tempit : m_histList) {
        for (unsigned int i = 0; i < tempit.second.size(); ++i) {
            tempit.second.at(i).SetOwner(kTRUE);
        }
    }
    for (auto  &tempit : m_fastHistList) {
        for (unsigned int i = 0; i < tempit.second.size(); ++i) {
            tempit.second.at(i).SetOwner(kTRUE);
        }
    }
    for (auto  &tempit : m_sysList) {
        for (auto  &i : tempit) {
            i.SetOwner(kTRUE);
        }
    }
    for (auto  &tempit :  m_fastSysList) {
        for (auto  &i : tempit) {
            i.SetOwner(kTRUE);
        }
    }
    for (auto  &tempit : m_uncorrToyMCSystFull) {
        for (auto & i : tempit) {
            i.SetOwner(kTRUE);
        }
    }  
    for (auto  &tempit : m_uncorrToyMCSystFast) {
        for (auto & i : tempit) {
            i.SetOwner(kTRUE);
        }
    }
}

// =============================================================================
// Initialize this correction tool
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::initialize() {
    // use an int as a StatusCode
    int sc(1);

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: "
            << __LINE__ << ") " << "Debug flag set. Printing verbose output!");

    //Check if files are present
    if (m_corrFileNameList.size() == 0) {
        ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                << " No file added!");
        return 0;
    }
    ATH_MSG_DEBUG("Initializing tool with " << m_corrFileNameList.size() 
            << " configuration file(s)");

    // Check if the first file can be opened (needed for auto-setting of the seed based on the md5-sum of the file)
    const std::unique_ptr<char> fname(gSystem->ExpandPathName(m_corrFileNameList[0].c_str()));
    std::unique_ptr<TFile> rootFile_tmp = CxxUtils::make_unique<TFile> (fname.get(), "READ");
    if (!rootFile_tmp) {
        ATH_MSG_ERROR(
                " (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                << "No ROOT file found here: " << m_corrFileNameList[0]);
        return 0;
    }
    rootFile_tmp->Close();
    // close it back again
    //
    //invalid input requested
    if (m_doToyMC && m_doCombToyMC) {
        ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                << " Both regular and combined toy MCs booked!" << " Only use one!");
        return 0;
    }
    /*
     * initialize the random number generator if toyMC propagation booked
     * Use the 1st 4 bytes of the CheckSum of the reccomendation file
     * as seed
     */
    if (m_doToyMC || m_doCombToyMC) {
        if (m_seed == 0) {
            std::unique_ptr<TMD5> tmd=CxxUtils::make_unique<TMD5>();
            const char* tmd_as_string=tmd->FileChecksum(fname.get())->AsString();
            m_seed = *(reinterpret_cast<const unsigned long int*>(tmd_as_string));
            ATH_MSG_DEBUG("Seed (automatically) set to " << m_seed);
        }else {
            ATH_MSG_DEBUG("Seed set to " << m_seed);
        }
        m_Rndm= TRandom3(m_seed);
    }
    // Load the needed histograms
    if (0 == getHistograms()) {
        ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "! Problem when calling getHistograms()");
        return 0;
    }
    const unsigned int nRunNumbersFull = m_begRunNumberList.size();
    const unsigned int nRunNumbersFast = m_begRunNumberListFastSim.size();

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
            << "Found " << nRunNumbersFast << " run number ranges for fast sim with a total of " <<
            m_fastHistList[mapkey::sf].size() << " scale factor histograms.");

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
            << "Found " << nRunNumbersFull << " run number ranges for full sim with a total of " <<
            m_histList[mapkey::sf].size() << " scale factor histograms.");

    /* 
     * Set up the vector of the position of the corr syst 
     * At this stage we should have the m_nSysMax and we know
     * the beginning
     * */
    const size_t index_of_corr=static_cast<size_t> (Position::End);  
    m_position_corrSys.resize(m_nSysMax);
    for (int sys = 0; sys < m_nSysMax; ++sys) { 
        m_position_corrSys[sys] = (index_of_corr + sys);
    }  
    /* 
     * The same as above by now for the toys if applicable
     */
    const size_t index_of_toys=static_cast<size_t> (Position::End)+m_nSysMax;
    m_position_uncorrToyMCSF.resize(m_nToyMC); 
    for (int toy=0; toy < m_nToyMC; ++toy) {
        m_position_uncorrToyMCSF[toy]=(index_of_toys+toy);
    }

    ATH_MSG_DEBUG("Tool succesfully initialized!");

    return sc;
}

const std::vector<double>
Root::TElectronEfficiencyCorrectionTool::calculate(const PATCore::ParticleDataType::DataType dataType,
        const unsigned int runnumber,
        const double cluster_eta,
        const double et, /* in MeV */
        size_t& index_of_corr,
        size_t& index_of_toys) const {

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "  entering function calculate");
    /* 
     * At this point , since this is after initialize,
     * we know the size of the vector we want to construct.
     * it is :
     * Position::End + m_nSysMax + m_nToyMC
     * The starting index of the sys is Position::End 
     * The starting point of the toys is Position::End+m_nSysMax
     */
    std::vector<double> result(
            (static_cast<size_t> (Position::End)+m_nSysMax+m_nToyMC),0
            );
    //Set up the non-0 defaults
    result[static_cast<size_t> (Position::SF)]=-999;
    result[static_cast<size_t> (Position::Total)]=1;

    /*
     * Set the known indices
     */
    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "Set the indices"); 
    if (m_nSysMax) {
        index_of_corr=m_position_corrSys.at(0);
    }
    if (m_nToyMC) {
        index_of_toys=m_position_uncorrToyMCSF.at(0);
    } 
    /* 
     * Determine Simulation flavour
     * And find the run period
     */
    const bool isFastSim=(dataType == PATCore::ParticleDataType::Fast) ? true: false;
    int runnumberIndex = -1;
    if (isFastSim) {
        for (unsigned int i = 0; i < m_begRunNumberListFastSim.size(); ++i) {
            if (m_begRunNumberListFastSim[i] <= runnumber && runnumber <= m_endRunNumberListFastSim[i]) {
                runnumberIndex=i;
                break;
            }
        }  
    }else {
        for (unsigned int i = 0; i < m_begRunNumberList.size(); ++i) {
            if (m_begRunNumberList[i] <= runnumber && runnumber <= m_endRunNumberList[i]) {
                runnumberIndex=i;
                break;
            }
        }
    }
    if(runnumberIndex <0 ){  
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No valid run number period  found for the current run number: " 
                << runnumber <<" for simulation type: " << dataType); 
        return result;
    }
    /* What we have is a map key:std::vector<TObjArray> 
     * Key: sf,stat,eigen, uncorr
     * The vector<TObArray> has as many entries as supported run periods
     * The TobjArray has 2D histos for  high, low et, or forward electrons
     * The 2D Histo then has the number we want.
     * What follows is the logic to get to this number
     */
    const std::map<int, std::vector< TObjArray > >& currentmap = (isFastSim)? m_fastHistList : m_histList;
    std::map<int, std::vector< TObjArray > >::const_iterator currentVector_itr = currentmap.find(mapkey::sf); //find the vector
    //See if we can find a SF vector in the map and the corresponding TobjArray for this run period
    if (currentVector_itr == currentmap.end()) {
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No valid vector of sf ObjArray found for the current run number " 
                << runnumber<<" for simulation type: " << dataType);  
        return result;
    }
    //Get a reference (synonym) to this vector 
    const std::vector<TObjArray>& currentVector=currentVector_itr->second;
    if (currentVector.size()<=0 || runnumberIndex>= static_cast <int> (currentVector.size())) {
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No valid  sf ObjArray found for the current run number " 
                << runnumber<<" for simulation type: " << dataType);  
        return result;
    }
    /* 
     * At this stage we have found the relevant TobjArray
     * So we need to locate the right histogram
     */
    const TObjArray& currentObjectArray = currentVector.at(runnumberIndex);  
    const int entries = currentObjectArray.GetEntries();
    /* 
     * Now the logic of finding the histogram
     * Perhaps one the points of the code that
     * could be improved given some "feedback"
     */
    double xValue(et);
    double yValue(cluster_eta);
    int smallEt(0), etaCov(0), nSF(0);
    bool block = false;
    bool changed = false;
    int index = -1;
    TH2 *tmpHist(0);
    for (int i = 0; i < entries ; ++i) {
        block = kFALSE;
        tmpHist = (TH2 *) (currentObjectArray.At(i));
        //block if we are below minimum et 
        if (et < tmpHist->GetXaxis()->GetXmin()) {
            smallEt++;
            block = kTRUE;
        }
        //block if we are above max eta 
        if (std::abs(yValue) >= tmpHist->GetYaxis()->GetXmax()) {
            etaCov++;
            block = kTRUE;
        }
        // Block if we are less than minimum (fwd electrons)
        if (std::abs(yValue) < tmpHist->GetYaxis()->GetXmin()) {
            etaCov++;
            block = kTRUE;
        }
        //Block if above max et and is the low Et histo
        if (et > tmpHist->GetXaxis()->GetXmax()) {
            if (TString(tmpHist->GetName()).Contains(LowPt_string)) {
                block = kTRUE;
            } else {
                xValue = tmpHist->GetXaxis()->GetBinCenter(tmpHist->GetNbinsX());
                changed = kTRUE;
            }
        }
        if (!block) {
            index = i;
            if (!changed) {
                nSF++;
            }
        }
    }
    //We are out of bounds 
    if (smallEt == entries) {
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No correction factor provided for et " << xValue);  
        return result;
    }
    if (etaCov == entries) {
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No correction factor provided for eta " << yValue);  
        return result;
    }
    if (nSF > 1) {
        ATH_MSG_WARNING("More than 1 SF found for eta=" << yValue << " , et = " 
                << et << " , run number = " << runnumber << ". Please check your input files!");
    }
    //Now we have the index of the histogram for this region in the TObjectarray 
    TH2* currentHist(0);
    if (index >= 0) {
        currentHist = static_cast<TH2*> (currentObjectArray.At(index));
    }
    else {
        ATH_MSG_DEBUG("(file: " << __FILE__ << ", line: " << __LINE__ << ") "<<
                "No correction factor provided because of an invalid index" << yValue);
        return result;
    }
    // If SF is only given in Abs(eta) convert eta input to std::abs()
    constexpr double epsilon = 1e-6;
    if (currentHist->GetYaxis()->GetBinLowEdge(1) >= 0 - epsilon) {
        if (yValue < 0) {
            ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") "
                    << "Scale factor only measured in Abs(eta) changing eta from " 
                    << yValue << " to " << std::abs(yValue));
        }
        yValue = std::abs(yValue);
    }
    const int globalBinNumber = currentHist->FindFixBin(xValue, yValue);
    const double scaleFactor = currentHist->GetBinContent(globalBinNumber);
    const double scaleFactorErr = currentHist->GetBinError(globalBinNumber);
    //
    // Write the retrieved values into the return object
    result[static_cast<size_t> (Position::SF)]=scaleFactor;
    result[static_cast<size_t> (Position::Total)]=scaleFactorErr;
    //DONE WITH THE SF
    //
    //DO the stat error using the available info from above i.e index etc
    double statErr = -999;
    currentVector_itr = currentmap.find(mapkey::stat); //find the vector
    if (currentVector_itr != currentmap.end()) {
        //itr at the location of the vector, .second get the vector, at(runnumberIndex is the TObjectArray 
        // for the period , finaly get the hist at index (from above).
        const TH1 *stat = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
        statErr = stat->GetBinContent(globalBinNumber);
        result[static_cast<size_t> (Position::Stat)]=statErr;
    }
    /*
     * Check if the correlated reduced
     * due to the detail level
     */ 
    std::array<int,detailLevelEnd> sLevel{};    
    currentVector_itr = currentmap.find(mapkey::eig); //find the vector
    if (currentVector_itr != currentmap.end()) {
        //Check on the ObjArray
        if (currentVector_itr->second.at(runnumberIndex).GetEntriesFast()> 0) {
            //itr at the location of the vector, .second get the vector, at(runnumberIndex is the TObjectArray 
            // for the period , finaly get the hist at index (from above).
            const TH1D *eig = static_cast<const TH1D*>(currentVector_itr->second.at(runnumberIndex).At(index)); 
            int nSys{};
            calcDetailLevels(eig, sLevel,nSys);
            nSys-=sLevel[m_detailLevel];
            result[static_cast<size_t> (Position::NSys)]=nSys;
        }
    }
    /*The issue now is that the previous setup is becoming cumbersome for the 10 systematic
     * So we keep them in a vector of vector of TObjectArray
     * The first vector index being the runnumber
     * The second the systematic
     * And the obj array for high low etc
     * We need to see if we can do something better here ...
     */
    const std::vector< std::vector< TObjArray > > &sysList = (isFastSim) ? m_fastSysList : m_sysList;
    std::vector<double> corrSys; 
    corrSys.reserve(10); 
    corrSys.clear();
    if (sysList.size() > static_cast<unsigned int> (index)) {
        if (sysList.at(index).size() > static_cast<unsigned int> (runnumberIndex)) {
            const int sys_entries = sysList.at(index).at( runnumberIndex).GetEntries();
            for (int sys = 0; sys < sys_entries; ++sys) {
                tmpHist = (TH2 *) sysList.at(index).at(runnumberIndex).At(sys_entries - 1 - sys);
                corrSys.push_back(tmpHist->GetBinContent(globalBinNumber));
                result[m_position_corrSys[(sys_entries - 1 - sys)]] =corrSys[sys];
            }
            if (m_position_corrSys.size() > 0 && sys_entries<=1) {
                if (result[m_position_corrSys[0]] == 0) {
                    result[m_position_corrSys[0]]=scaleFactorErr;
                }
            }
        }
    }
    //Do the uncorr error using the available info from above i.e index etc
    //Get the statErr from above
    double val = statErr;
    currentVector_itr = currentmap.find(mapkey::uncorr); //find the vector
    if (currentVector_itr != currentmap.end()) {
        //Check on the ObjArray
        if (currentVector_itr->second.at(runnumberIndex).GetEntriesFast()>0) {
            TH1 *uncorr = static_cast<TH1*>(currentVector_itr->second.at(runnumberIndex).At(index));
            const double valAdd = uncorr->GetBinContent(globalBinNumber);
            val = sqrt(val * val + valAdd * valAdd);
            for (int i = 0; i < sLevel[m_detailLevel]; ++i) {
                const double valAdd1 = corrSys.at(corrSys.size() - 1 - i);
                val = sqrt(val * val + valAdd1 * valAdd1);
            }
        }
    }
    if (val == -999) {
        val = 0;
    }
    result[static_cast<size_t> (Position::UnCorr)]=val;
    /* 
     * Do the toys
     */
    if (m_doToyMC || m_doCombToyMC) {
        const std::vector<std::vector<TObjArray > >& toyMCList = ((isFastSim) ? m_uncorrToyMCSystFast : m_uncorrToyMCSystFull);
        if (toyMCList.size() > (unsigned int) runnumberIndex) {
            for (int toy = 0; toy < m_nToyMC; toy++) {
                if (toyMCList.at(runnumberIndex).at(toy).GetLast() >= index) {
                    result[m_position_uncorrToyMCSF.at(toy)]=
                        ((TH2 *) toyMCList.at(runnumberIndex).at(toy).At(index))->GetBinContent(globalBinNumber);
                }
            }
        }
    }
    result[static_cast<size_t> (Position::GlobalBinNumber)]=globalBinNumber;
    return result;
}

/*
 *Calculate the detail levels for a given eigenvector histogram
 * Since the syst are in practice eigenvalues 
 * the smaller ones could be disgarded added to uncorrelated
 */
void
Root::TElectronEfficiencyCorrectionTool::calcDetailLevels(
        const TH1D *eig, 
        std::array<int,detailLevelEnd>& sLevel,
        int& nSys) const {

    sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = 0;
    sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = 0;
    sLevel[Root::TElectronEfficiencyCorrectionTool::detailed] = 0;
    nSys = eig->GetNbinsX() - 1;
    double sign = 0;
    // Calculate detail level
    for (int i = nSys + 1; i >= 2; i--) {
        sign += eig->GetBinContent(i);
        if (sign > 0.8 && sLevel[Root::TElectronEfficiencyCorrectionTool::simple] == 0) {
            sLevel[Root::TElectronEfficiencyCorrectionTool::simple] = i - 2;
        }
        if (sign > 0.95 && sLevel[Root::TElectronEfficiencyCorrectionTool::medium] == 0) {
            sLevel[Root::TElectronEfficiencyCorrectionTool::medium] = i - 2;
        }
    }
}
/*
 * Build the toyMC tables from inputs
 * Ownership should be tranfered to the map of the tables
 * and the proper delete happens in the dtor
 */
std::vector<TH2D *>
Root::TElectronEfficiencyCorrectionTool::buildSingleToyMC(
        const TH2D *sf, 
        const TH2D *stat, 
        const TH2D *uncorr, 
        const TObjArray& corr,
        const std::array<int,detailLevelEnd> sLevel,
        int& randomCounter) {

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ")! " 
            << "entering function buildSingleToyMC");
    std::vector<TH2D*> tmpHists;
    int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
    for (int toy = 0; toy < m_nToyMC; toy++) {
        tmpHists.push_back((TH2D *) corr.At(0)->Clone());
    }
    // Loop over all bins
    for (int bin = 0; bin < nBins; bin++) {
        double val = stat->GetBinContent(bin);

        // Add uncorrelated systematics
        if (uncorr != 0) {
            double valAdd = uncorr->GetBinContent(bin);
            val = sqrt(val * val + valAdd * valAdd);
        }
        // Add smaller correlated systematics as uncorrelated
        for (int i = 0; i < sLevel[m_detailLevel]; ++i) {
            if (corr.At(i) != 0) {
                double valAdd = ((TH2D *) corr.At(i))->GetBinContent(bin);
                val = sqrt(val * val + valAdd * valAdd);
            }
        }
        for (int toy = 0; toy < m_nToyMC; toy++) {
            tmpHists.at(toy)->SetBinContent(bin, (val * m_Rndm.Gaus(0, 1)) + sf->GetBinContent(bin));
            randomCounter++;
            tmpHists.at(toy)->SetDirectory(0);
        }
    }
    return tmpHists;
}
/*
 * Build the combined toyMC tables from inputs
 * Ownership should be tranfered to the map of the tables
 * and the proper delete happens in the dtor
 */
TH2D*
Root::TElectronEfficiencyCorrectionTool::buildSingleCombToyMC(
        const TH2D *sf, 
        const TH2D *stat, 
        const TH2D *uncorr, 
        const TObjArray& corr,
        const std::array<int,detailLevelEnd> sLevel,
        const int nSys,
        int& randomCounter){

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "entering function buildSingleCombToyMC");

    TH2D *tmpHist;
    const int nBins = (stat->GetNbinsX() + 2) * (stat->GetNbinsY() + 2);
    tmpHist = (TH2D *) corr.At(0)->Clone();
    // Create random numbers for the corr. uncertainties
    std::vector<double> rnd (nSys,0);
    for (int s = 0; s < nSys; ++s) {
        rnd[s] = m_Rndm.Gaus(0, 1);
        randomCounter++;
    }
    // Loop over all bins
    for (int bin = 0; bin < nBins; ++bin) {
        double val = stat->GetBinContent(bin);

        // Add uncorrelated systematics
        if (uncorr != 0) {
            double valAdd = uncorr->GetBinContent(bin);
            val = sqrt(val * val + valAdd * valAdd);
        }
        // Add smaller correlated systematics as uncorrelated
        for (int s = 0; s < sLevel[m_detailLevel]; ++s) {
            if (corr.At(s) != 0) {
                double valAdd = ((TH2D *) corr.At(s))->GetBinContent(bin);
                val = sqrt(val * val + valAdd * valAdd);
            }
        }
        val = val * m_Rndm.Gaus(0,1);
        randomCounter++;
        // Add larger correlated systematics
        for (int s = sLevel[m_detailLevel]; s < nSys; ++s) {
            if (corr.At(s) != 0) {
                val += ((TH2D *) corr.At(s))->GetBinContent(bin) * rnd[s];
            }
        }
        tmpHist->SetBinContent(bin, val + sf->GetBinContent(bin));
    }
    tmpHist->SetDirectory(0);
    return tmpHist;
}
/*
 * Build the toyMC tables from inputs
 */
std::vector<TObjArray>
Root::TElectronEfficiencyCorrectionTool::buildToyMCTable(const TObjArray& sf, 
        const TObjArray& eig, 
        const TObjArray& stat,
        const TObjArray& uncorr, 
        const std::vector<TObjArray>& corr) {

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "entering function buildToyMCTable");
    
    std::array<int,detailLevelEnd> sLevel{}; 
    int nSys{};
    int randomCounter(0);
    std::vector<TObjArray> tmpVec;
    const int stat_entries = stat.GetEntries();

    if (m_doCombToyMC) {
        for (int toyMC = 0; toyMC < m_nToyMC; toyMC++) {
            TObjArray tmpArray;
            for (int i = 0; i < stat_entries; ++i) {
                if (eig.GetEntriesFast()>0 && uncorr.GetEntriesFast()>0) {
                    std::array<int,detailLevelEnd> sLevel{};    
                    int nSys{};
                    calcDetailLevels((TH1D *) eig.At(i), sLevel,nSys);
                    tmpArray.Add(buildSingleCombToyMC((TH2D *) sf.At(i), 
                                (TH2D *) stat.At(i), 
                                (TH2D *) uncorr.At(i), 
                                corr.at(i),
                                sLevel,
                                nSys,
                                randomCounter));
                }else {
                    tmpArray.Add(buildSingleCombToyMC((TH2D *) sf.At(i), 
                                (TH2D *) stat.At(i), 
                                0, 
                                corr.at(i) ,
                                sLevel,
                                nSys,
                                randomCounter));
                }
            }
            tmpVec.push_back(tmpArray);
        }
    }else {
        std::vector< std::vector<TH2D*> > tmpVec2 ;
        for (int i = 0; i < stat_entries; ++i) {
            calcDetailLevels((TH1D *) eig.At(i),sLevel,nSys);
            tmpVec2.push_back(buildSingleToyMC((TH2D *) sf.At(i), 
                        (TH2D *) stat.At(i), 
                        (TH2D *) uncorr.At(i),
                        corr.at(i),
                        sLevel,
                        randomCounter));
        }
        for (int toy = 0; toy < m_nToyMC; toy++) {
            TObjArray tmpArray;
            for (unsigned int i = 0; i < tmpVec2.size(); ++i) {
                tmpArray.Add(tmpVec2.at(i).at(toy));
            }
            tmpVec.push_back(tmpArray);
        }
    }
    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "m_Rndm->Uniform(0, 1) after throwing " << randomCounter
            << " random numbers: " << m_Rndm.Uniform(0,1));

    return tmpVec;
}
// =============================================================================
// Helper function to retrieve number of uncorrelated bins
// =============================================================================
int
Root::TElectronEfficiencyCorrectionTool::getNbins(std::map<float, std::vector<float> > &pt_eta1) const {
    //Get sf histograms 
    const std::vector<TObjArray >& tmpVec = m_histList.at(mapkey::sf);
    int nbinsTotal = 0;
    pt_eta1.clear();
    std::vector<float>eta1;
    eta1.clear();

    //Loop over the different run ranges (one TObjeArray for each)
    for (unsigned int ikey = 0; ikey < tmpVec.size(); ++ikey) {
        //Loop over the histograms for a given run numbers
        for (int entries = 0; entries < (tmpVec.at(ikey)).GetEntries(); ++entries) {
            eta1.clear();
            //Get number of bins
            TH2D *h_tmp = ((TH2D * ) (tmpVec.at(ikey)).At(entries));
            int nbinsX = h_tmp->GetNbinsX();
            int nbinsY = h_tmp->GetNbinsY();
            //fill in the eta pushing back
            for (int biny = 1; biny <= nbinsY; ++biny) {
                eta1.push_back(h_tmp->GetYaxis()->GetBinLowEdge(biny));
            }
            //associate each pt (bin) with the corresponding/available eta ones
            for (int binx = 1; binx <=nbinsX; ++binx) {
                pt_eta1[h_tmp->GetXaxis()->GetBinLowEdge(binx)] = eta1;
            }
        }
    }
    for (auto &i : pt_eta1) {
        nbinsTotal += i.second.size();
    }
    return nbinsTotal;
}
// =============================================================================
// Get the input histograms from the input files
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::getHistograms() {

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "entering function getHistograms");
    // Cache the current directory in the root file
    TDirectory *origDir = gDirectory;
    // -------------------------------------------------------
    // Get the name of the first input ROOT file and
    // interpret from that what we have:
    // efficiency vs. efficiencySF; offline vs. trigger; medium, loose,...
    // -------------------------------------------------------
    if (!m_corrFileNameList.empty()) {
        TString firstFileNameAndPath = m_corrFileNameList[0].c_str();
        std::unique_ptr<TObjArray> myStringList(firstFileNameAndPath.Tokenize("/"));
        int lastIdx = myStringList->GetLast();
        TString fileName = ((TObjString *) myStringList->At(lastIdx))->GetString();
        std::unique_ptr<TObjArray> myFileNameTokensList(fileName.Tokenize("."));

        if (myFileNameTokensList->GetLast() < 3) {
            ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                    << "input file name has wrong format!");
            return 0;
        }

    }
    // -------------------------------------------------------
    // Get all ROOT files and histograms
    // -------------------------------------------------------
    for (unsigned int i = 0; i < m_corrFileNameList.size(); ++i) {
        // Load the ROOT file
        const std::unique_ptr<char> fname (gSystem->ExpandPathName(m_corrFileNameList[i].c_str()));
        std::unique_ptr<TFile> rootFile = CxxUtils::make_unique<TFile> (fname.get(), "READ");
        if (!rootFile) {
            ATH_MSG_ERROR(
                    " (file: " << __FILE__ << ", line: " << __LINE__ << ") " << "No ROOT file found here: " <<
                    m_corrFileNameList[i]);
            return 0;
        }
        // Loop over all directories inside the root file (correspond to the run number ranges
        TIter nextdir(rootFile->GetListOfKeys());
        TKey *dir;
        TObject *obj;
        while ((dir = (TKey *) nextdir())) {
            obj = dir->ReadObj();
            if (obj->IsA()->InheritsFrom("TDirectory")) {
                // splits string by delimiter --> e.g RunNumber1_RunNumber2
                //std::unique_ptr<TObjArray> dirNameArray(TString(obj->GetName()).Tokenize("_"));
                TObjArray dirNameArray = *(TString(obj->GetName()).Tokenize("_"));
                //// returns index of last string --> if one, the directory name does not contain any run numbers
                int lastIdx = dirNameArray.GetLast();
                if (lastIdx != 1) {
                    ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                            << "The folder name seems to have the wrong format! Directory name:"
                            << obj->GetName());
                    return 0;
                }
                rootFile->cd(obj->GetName());
                if (0 == this->setupHistogramsInFolder(dirNameArray, lastIdx)) {
                    ATH_MSG_ERROR(
                            " (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                            << "unable to setup the histograms in directory " << dir->GetName()
                            << "in file " << m_corrFileNameList[i]);
                    return 0;
                }
            }else {
                ATH_MSG_ERROR(
                        " (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                        << "Wrong file content! Expected only Directories " <<
                        gDirectory->cd());
                return 0;
            }
            // Return to the original ROOT directory
            gDirectory = origDir;
        } // End: directory loop
    } // End: file loop
    return 1;
}
// =============================================================================
// Get the input histograms from a given folder/run number range
// =============================================================================
int Root::TElectronEfficiencyCorrectionTool::setupHistogramsInFolder(const TObjArray& dirNameArray, int lastIdx) {

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "entering funtion setupHistogramsInFolder");

    int runNumBegin(-1);
    TString myBegRunNumString = ((TObjString *) dirNameArray.At(lastIdx - 1))->GetString();
    if (myBegRunNumString.IsDigit()) {
        runNumBegin = myBegRunNumString.Atoi();
    }
    int runNumEnd(-1);
    TString myEndRunNumString = ((TObjString *) dirNameArray.At(lastIdx))->GetString();
    if (myEndRunNumString.IsDigit()) {
        runNumEnd = myEndRunNumString.Atoi();
    }
    if (runNumBegin < 0 || runNumEnd < 0 || runNumEnd < runNumBegin) {
        ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                << "Could NOT interpret the run number range: " << runNumBegin
                << " - " << runNumEnd);
        return 0;
    }
    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << runNumBegin << "  " << runNumEnd);
    //
    /// setup pairs of obj arrays and keys --> e.g. "sf", new Array to take all SF Histos
    std::map<int, TObjArray> objsFull;
    std::map<int, TObjArray > objsFast;
    for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
        TObjArray dummyFull;
        objsFull.insert(std::make_pair(m_keys.at(ikey),dummyFull));
        TObjArray dummyFast;
        objsFast.insert(std::make_pair(m_keys.at(ikey),dummyFast));
    }  
    TObjArray dummyFull;    
    objsFull.insert(std::make_pair(mapkey::sys, dummyFull));
    TObjArray dummyFast;
    objsFast.insert(std::make_pair(mapkey::sys, dummyFast));
    //
    std::vector<TObjArray > sysObjsFull;
    std::vector<TObjArray > sysObjsFast;
    //
    TIter nextkey(gDirectory->GetListOfKeys());
    TKey *key;
    TObject *obj(0);
    TString tmpName = "";
    int tmpCounter = 0;
    //Loop of the keys 
    while ((key = (TKey *) nextkey())) {
        obj = key->ReadObj();
        if (obj->IsA()->InheritsFrom("TH1")) {
            // The histogram containing the scale factors need to end with _sf and need to contain either the string "FullSim"
            // or "AtlFast2"!
            if (TString(obj->GetName()).Contains("FullSim")) {
                //Add all except the correlated 
                for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
                    if (TString(obj->GetName()).EndsWith("_" +  TString(mapkey::keytostring(m_keys.at(ikey))))) {
                        objsFull.find(m_keys.at(ikey))->second.Add(obj);
                    }
                }
                //
                tmpName = TString(obj->GetName());
                //Special treatment , this is only for photons 
                if (tmpName.EndsWith("_sys")) {
                    objsFull.find(mapkey::sys)->second.Add(obj);
                    TObjArray tmpArrayFull;
                    tmpArrayFull.Add(obj);
                    sysObjsFull.push_back(tmpArrayFull);
                    tmpCounter++;
                }
                //
                //See if we are dealing with correlated
                if (tmpName.Contains("_corr")) {
                    if (tmpName.EndsWith("corr0")) {
                        //This is the worse part ...
                        //corr0 triggers a few thing
                        //
                        //1st create a TObjectArray
                        //For high or low Pt (one for each ...)
                        TObjArray tmpArrayFull;
                        //Resgister it to the vector (so the vector has size 1 or 2)
                        sysObjsFull.push_back(tmpArrayFull);
                        //Reset the counter
                        tmpCounter=0;
                    }
                    //Add to the current last element of the sysObject full
                    //This will be Low Pt once and high Pt the other time
                    sysObjsFull.back().Add(obj);
                    //Now increase the counter
                    tmpCounter++;
                }
                if (tmpCounter > m_nSysMax) {
                    m_nSysMax = tmpCounter;
                }
            }
            else if (TString(obj->GetName()).Contains("AtlFast2")) {
                for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
                    if (TString(obj->GetName()).EndsWith("_" +  TString(mapkey::keytostring(m_keys.at(ikey))))) {
                        objsFast.find(m_keys.at(ikey))->second.Add(obj);
                    }
                }
                //See if we are dealing with correlated
                tmpName = TString(obj->GetName());
                //Special treatment , this is only for photons 
                if (tmpName.EndsWith("_sys")) {
                    objsFast.find(mapkey::sys)->second.Add(obj);
                    TObjArray tmpArrayFast;
                    tmpArrayFast.Add(obj);
                    sysObjsFast.push_back(tmpArrayFast);
                    tmpCounter++;
                }
                //
                //See if we are dealing with correlated
                if (tmpName.Contains("_corr")) {
                    if (tmpName.EndsWith("corr0")) {
                        //This is the worse part ...
                        //corr0 triggers a few thing
                        //1st create a TObjectArray
                        TObjArray tmpArrayFast;
                        //Resgister it
                        sysObjsFast.push_back(tmpArrayFast);
                        //Reset the counter
                        tmpCounter=0;
                    }
                    //Add to the current last element of the sysObject full
                    //This will be Low Pt once and high Pt the other time
                    sysObjsFast.back().Add(obj);
                    //Now increase the counter
                    tmpCounter++;
                }
                if (tmpCounter > m_nSysMax) {
                    m_nSysMax = tmpCounter;
                }
            } else {
                ATH_MSG_ERROR(
                        " (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                        << "Could NOT interpret if the histogram: " << obj->GetName()
                        << " is full or fast simulation!");
                return 0;
            }
        }
    }

    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "setting up histograms for run ranges  " <<
            runNumEnd);
    //
    //The setup here copies from the temporaties created in this function , 
    //to the actual class member variables.
    for (unsigned int ikey = 0; ikey < m_keys.size(); ++ikey) {
        if (objsFull.find(m_keys.at(ikey))->second.GetEntries() != 0) {
            if (0 == setup(objsFull.find(m_keys.at(ikey))->second, m_histList[m_keys.at(ikey)], 
                        m_begRunNumberList,m_endRunNumberList,runNumBegin,runNumEnd)) {
                ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                        << "! Could NOT setup histogram " 
                        << m_keys.at(ikey)<< " for full sim!");
                return 0;
            }
        }
        if (objsFast.find(m_keys.at(ikey))->second.GetEntries() != 0) {
            if (0 == setup(objsFast.find(m_keys.at(ikey))->second, m_fastHistList[m_keys.at(ikey)],
                        m_begRunNumberListFastSim, m_endRunNumberListFastSim,runNumBegin,runNumEnd)) {
                ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                        << "! Could NOT setup histogram " << m_keys.at(ikey)
                        << " for fast sim");
                return 0;
            }
        }
    }
    for (unsigned int sys = 0; sys < sysObjsFast.size(); sys++) {
        m_fastSysList.resize(sysObjsFast.size());
        if (0 == setup(sysObjsFast.at(sys), m_fastSysList[sys], m_begRunNumberListFastSim, 
                    m_endRunNumberListFastSim,runNumBegin,runNumEnd)) {
            ATH_MSG_ERROR(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " <<
                    "! Could NOT setup systematic histograms for fast sim");
            return 0;
        }
    }
    for (unsigned int sys = 0; sys < sysObjsFull.size(); sys++) {
        m_sysList.resize(sysObjsFull.size());
        if (0 == setup(sysObjsFull.at(sys), m_sysList[sys], m_begRunNumberList, 
                    m_endRunNumberList,runNumBegin,runNumEnd)) {
            ATH_MSG_ERROR(
                    " (file: " << __FILE__ << ", line: " << __LINE__ << ") " <<
                    "! Could NOT setup systematic histograms for fast sim");
            return 0;
        }
    }
    //
    //Toys
    ATH_MSG_DEBUG(" (file: " << __FILE__ << ", line: " << __LINE__ << ") " 
            << "Checking for (m_doToyMC || m_doCombToyMC)");

    if (m_doToyMC || m_doCombToyMC) {
        bool fullToysBooked = kFALSE;
        bool fastToysBooked = kFALSE;
        TObjArray dummy;

        if (m_histList[mapkey::sf].size() > 0) {
            if (objsFull.find(mapkey::eig)->second.GetEntries() < 1 || objsFull.find(mapkey::stat)->second.GetEntries() < 1 ||
                    objsFull.find(mapkey::uncorr)->second.GetEntries() < 1) {
                if (objsFull.find(mapkey::stat)->second.GetEntries() > 1 || objsFull.find(mapkey::sys)->second.GetEntries() > 1) {
                    m_uncorrToyMCSystFull.push_back(buildToyMCTable(objsFull.find(mapkey::sf)->second, dummy,
                                objsFull.find(mapkey::stat)->second,
                                dummy, sysObjsFull));
                    fullToysBooked = kTRUE;
                }else {
                    ATH_MSG_DEBUG("! Toy MC error propagation booked, but not all needed" 
                            <<"histograms found in the output (For full sim). Skipping toy creation!");
                }
            }else {
                m_uncorrToyMCSystFull.push_back(buildToyMCTable(objsFull.find(mapkey::sf)->second, objsFull.find(mapkey::eig)->second,
                            objsFull.find(mapkey::stat)->second,
                            objsFull.find(mapkey::uncorr)->second, sysObjsFull));
                fullToysBooked = kTRUE;
            }
        }
        ///// AF2
        if (m_fastHistList[mapkey::sf].size() > 0) {
            if (objsFast.find(mapkey::eig)->second.GetEntries() < 1 || objsFast.find(mapkey::stat)->second.GetEntries() < 1 ||
                    objsFast.find(mapkey::uncorr)->second.GetEntries() < 1) {
                if (objsFast.find(mapkey::stat)->second.GetEntries() > 1 || objsFast.find(mapkey::sys)->second.GetEntries() > 1) {
                    m_uncorrToyMCSystFast.push_back(buildToyMCTable(objsFast.find(mapkey::sf)->second, dummy,
                                objsFast.find(mapkey::stat)->second,
                                dummy, sysObjsFast));
                    fastToysBooked = kTRUE;
                }else {
                    ATH_MSG_DEBUG("! Toy MC error propagation booked, but not all needed"
                            << "histograms found in the output (For fast sim). Skipping toy creation!");
                }
            }else {
                m_uncorrToyMCSystFast.push_back(buildToyMCTable(objsFast.find(mapkey::sf)->second, objsFast.find(mapkey::eig)->second,
                            objsFast.find(mapkey::stat)->second,
                            objsFast.find(mapkey::uncorr)->second, sysObjsFast));
                fastToysBooked = kTRUE;
            }
        }

        if (fullToysBooked || fastToysBooked) {
            if (m_doToyMC) {
                ATH_MSG_DEBUG("Created tables for " << m_nToyMC << " ToyMC systematics ");
            }
            if (m_doCombToyMC) {
                ATH_MSG_DEBUG("Created tables for " << m_nToyMC << " combined ToyMC systematics ");
            }
        }
    }
    return 1;
}
/*
 * Fill and interpret the setup, depending 
 * on which histograms are found in the input file(s)
 */
int
Root::TElectronEfficiencyCorrectionTool::setup(const TObjArray& hists,
        std::vector< TObjArray> &histList,
        std::vector< unsigned int > &beginRunNumberList,
        std::vector< unsigned int > &endRunNumberList,
        const int runNumBegin,
        const int runNumEnd) const {
    if (hists.GetEntriesFast()==0) {
        ATH_MSG_ERROR(
                "(file: " << __FILE__ << ", line: " << __LINE__ << ") " 
                << "! Could NOT find histogram with name *_sf in folder");
        return 0;
    }
    TH2 *tmpHist(0);
    for (int i = 0; i < hists.GetEntries(); ++i) {
        tmpHist = (TH2 *) hists.At(i);
        tmpHist->SetDirectory(0);
    }
    // Now, we have all the needed info. Fill the vectors accordingly
    histList.push_back(hists);
    if (beginRunNumberList.size() > 0) {
        if (runNumBegin != (int) beginRunNumberList.back()) {
            beginRunNumberList.push_back(runNumBegin);
        }
    }else {
        beginRunNumberList.push_back(runNumBegin);
    }
    if (endRunNumberList.size() > 0) {
        if (runNumEnd != (int) endRunNumberList.back()) {
            endRunNumberList.push_back(runNumEnd);
        }
    }else {
        endRunNumberList.push_back(runNumEnd);
    }
    return 1;
}

