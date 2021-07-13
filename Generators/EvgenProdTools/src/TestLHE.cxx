/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/TestLHE.h"
#include "GaudiKernel/DataSvc.h"
#include "TruthUtils/PIDHelpers.h"
#include "AGDDControl/XercesParser.h"

// For find
#include <algorithm>

using namespace std;


TestLHE::TestLHE(const std::string& name, ISvcLocator* pSvcLocator)
    : GenBase(name, pSvcLocator),
    m_thistSvc("THistSvc", name)
{
    declareProperty("PdgToSearch",              m_pdg = 15);              //to do this test is a bit weirdly specific to taus
    declareProperty("CmEnergy",                 m_cm_energy = -1);        // in GeV, -1 = get from event
    declareProperty("CmeDifference",            m_energy_diff = 0.1);     // GeV
    declareProperty("absXWGTUP",                m_abs_XWGTUP = 5.);
    declareProperty("WeightDifference",         m_weight_diff = 0.005);
    declareProperty("ParticleMomentumBalance",  m_momentumBalance = 0.01);  //GeV

    declareProperty("UnknownPDGIDFile",         m_unknownPDGIDFile = "pdgid_extras.txt");
    declareProperty("LHEFile",                  m_eventsLheFile = "events.lhe");
    declareProperty("ModifiedLHEFile",          m_modifiedLheFile = "events_modified.lhe");

    declareProperty("IsLargeSpreadWeight",      m_isLargeWeight = false);
    declareProperty("BeamEnergyTest",           m_beamEnergyTest = true);    //switching off inactive
    declareProperty("UnknowPDGIDTest",          m_unknownPDGIDTest = true);
    declareProperty("IfCreateNewLHEFile",       m_ifCreateNewLheFile = false);

    declareProperty("THistSvc", m_thistSvc);

    declareProperty("DoHist", m_doHist=false); //histograming yes/no true/false

    m_nPass = 0;
    m_nFail = 0;
    m_nparticle = 0;

    //check Counters
    m_invalidEventformatCheckRate = 0;
    m_invalidParticleFormatCheckRate = 0;
    m_NegativeParticleCheckRate = 0;
    m_MomentumBalanceCheckRate = 0;       //momentum balance check
    m_EnergyBalanceCheckRate = 0;             //energy balance check
    m_particleNanCheckRate = 0;               //Nan value for each parameter check
    m_particleInfiniteCheckRate = 0;          //check Infinite particle value
    m_tachyonParticleCheckRate = 0;           //tychon particle check
    m_unknownPDGIDCheckRate = 0;              //unknown PDG ID check

}

StatusCode TestLHE::initialize()
{
    CHECK(GenBase::initialize());

    if(m_doHist)
    {
        CHECK(m_thistSvc.retrieve());
    }

    std::string line;

    std::ifstream pdgFile;
    pdgFile.open(m_unknownPDGIDFile.c_str());
    int pdgID;
    if (pdgFile.good()){
        ATH_MSG_INFO("extra white list for PDG IDs found "<< m_unknownPDGIDFile.c_str());
        while (std::getline(pdgFile,line)) {
            std::stringstream ss(line);
            ss >> pdgID;
            m_unknownPDGID_tab.push_back(pdgID);
        }
        pdgFile.close();
    }
    else {
        ATH_MSG_INFO("extra white list for PDG IDs not provided");
    }

    int nline = 0;                               //to record line number
    int eventline = 0;                           //looking for keyword <event>, check <event> block 
    int initline = 0;                            //looking for keywork <init> , check <init> initial beam information
    int addline = 0;
    int beginInitRwgtline = 0;
    int beginRwgtline = 0;                        //find <rwgt>
    int intTypeInfo;
    float floatTypeInfo;
    float rwgtWeight;
    bool ifFoundInit = false;
    bool ifFoundEvent = false;
    bool ifFoundInitRwgt = false;
    bool ifFoundRwgt = false;
    std::string initialInfo;
    std::string eventInfo;
    std::string particleInfo;

    //========keywords in weights information==========
    std::string initialRwgt = "<initrwgt>";           //this keyword represent the beginning block of weights information
    std::string endInitRwgt = "</initrwgt>";       //this keyword represent the ending block of weights information in initial part
    std::string beginRwgt = "<rwgt>";              //each line has one weight value after <event> block
    std::string endRwgt = "</rwgt>";
    std::string endGroupName = "</weightgroup>";   //end of one block of weights
    std::string symbol1 = "</weight>";             //in <initrwgt> block, each line has </weight> for each weight id
    std::string symbol2 = "</wgt>";
    std::string weightGroupSymbol = "combine";               //looking for weightgroup name
    std::string::size_type idx1;                   //find weight id so that to keep id number
    std::string::size_type idx2;                   //find wgt id 
    std::string::size_type idx31;                  //find weightgroup name 
    std::string::size_type idx32;                  //find /weightgroup name 
    std::string::size_type idx33;                  //find /initrwgt 
    //=========Symbols for weight information=======

    //========keywords to find <event> block and record information========
    std::string keyword1 = "<init>";
    std::string keyword2 = "<event>";
    std::string floatSymbol = "E";                    //determine integar or float type
    std::string intSymbol = ".";                    //determine integar or float type
    std::string CommentSymbol1 = "#pdf";           //end of <event> block   (multiple keywords in ending this block)
    std::string CommentSymbol2 = "<mgrwt>";        //end of <event> block   (multiple keywords in ending this block)
    std::string CommentSymbol3 = "<rwgt>";         //end of <event> block   (multiple keywords in ending this block)
    std::string CommentSymbol4 = "</event>";       //end of <event> block   (multiple keywords in ending this block)
    std::string CommentSymbol5 = "#rwgt";          //end of <event> block   (multiple keywords in ending this block)
    std::string::size_type idxIsFloat;                   //find character symbol'E' to determine int or float
    std::string::size_type idxIsInt;                     //find character symbol'.' to determine int or float
    std::string::size_type idxComment1;
    std::string::size_type idxComment2;
    std::string::size_type idxComment3;
    std::string::size_type idxComment4;
    std::string::size_type idxComment5;
    //========Symbols for beam and event information=======================
    

    //========for Powheg Weight Checking=======
    //========need to check weight ID (no duplicate, in an increasing order)=======
    //========need to check weight value=======

    XercesParser XParser;

    if ( XParser.ParseFile(m_eventsLheFile) )
    {
        ATH_MSG_WARNING("Some Errors are detected when first reading LHE files");
    }
    else {
        ATH_MSG_INFO("No Errors are detected when first reading LHE files");
    }

    std::ifstream LHEfile;
    LHEfile.open(m_eventsLheFile.c_str());
    if(LHEfile.fail()) ATH_MSG_INFO("Can not find events.lhe files in this directory");
    else {
        ATH_MSG_INFO("events.lhe file are found");
        while(std::getline(LHEfile,line)) {
            ++nline;
            if (line == initialRwgt) {
                ifFoundInitRwgt = true;
                beginInitRwgtline = nline;
            }
            else if (line == keyword1) {
                initline = nline;
                ifFoundInit = true;
            }
            else if (line == keyword2) { 
                eventline = nline;
                m_storeEventLine.push_back(eventline);
                ifFoundEvent = true;
            }
            else if (line == beginRwgt) {
                beginRwgtline = nline;
                ifFoundRwgt = true;
                ifFoundInitRwgt = false;
            }
            if (nline>beginInitRwgtline && ifFoundInitRwgt && !ifFoundRwgt){
                idx1  = line.find(symbol1);
                idx31 = line.find(weightGroupSymbol);
                idx32 = line.find(endGroupName);
                idx33 = line.find(endInitRwgt);
                if (! (idx31 == std::string::npos)) {
                    int p1 = line.find_first_of("'");
                    int p3 = line.find_first_of("\"");
                    if (p1 != -1) {
                        int p2 = line.find_first_of("'",p1+1);
                        std::string group_name = line.substr(p1+1, p2-p1-1); 
                        m_GroupName.push_back(group_name);
                    }
                    else if (p3 != -1) {
                        int p4 = line.find_first_of("\"",p3+1);
                        std::string group_name = line.substr(p3+1, p4-p3-1); 
                        m_GroupName.push_back(group_name);
                    }
                }
                else if (! (idx1 == std::string::npos) ){
                    int p1 = line.find_first_of(">");
                    int p2 = line.find_last_of("<");
                    std::string Str = line.substr(p1+1, p2-p1-1);
                    WeightInitTemp.push_back(Str);         //get weight name in each block

                    int p3 = line.find_first_of("'");
                    int p5 = line.find_first_of("\"");
                    int id;
                    if (p3 != -1){
                        int p4 = line.find_first_of("'",p3+1);
                        std::string temp = line.substr(p3+1, p4-p3-1);
                        std::stringstream ss(temp);
                        ss>>id;
                        weightTemp.push_back(id);
                    }
                    else if (p5 != -1){
                        int p6 = line.find_first_of("\"",p5+1);
                        std::string temp = line.substr(p5+1, p6-p5-1);
                        std::stringstream ss(temp);
                        ss>>id;
                        weightTemp.push_back(id);       //get weight ID in each block
                    }
                }
                if ( !(idx32 == std::string::npos) || !(idx33 == std::string::npos) ) 
                {
                    m_WeightInitID.push_back(weightTemp);
                    m_WeightInitName.push_back(WeightInitTemp);
                    weightTemp.clear();
                    WeightInitTemp.clear();
                }
            }
            if( nline>initline && nline<initline+3 && ifFoundInit){
                std::stringstream ss(line);
                std::stringstream ss2(line);
                while (ss>>initialInfo) {
                    idxIsFloat = initialInfo.find(floatSymbol);
                    idxIsInt = initialInfo.find(intSymbol);
                    if ((idxIsFloat == std::string::npos) && (idxIsInt == std::string::npos)){
                        ss2 >> intTypeInfo;
                        m_InitIntInfo_tab.push_back(intTypeInfo);
                    }
                    else {
                        ss2 >> floatTypeInfo;
                        m_InitFloatInfo_tab.push_back(floatTypeInfo);
                    }
                }
            }
            if (ifFoundEvent){
                if (nline == eventline+1) {
                    std::string tempInfo;
                    std::stringstream ss(line);
                    std::stringstream ss2(line);
                    std::stringstream ss3(line);
                    while (ss3 >> tempInfo){
                        InformationTemp.push_back(tempInfo);
                    }
                    while (ss >> eventInfo){
                        idxIsFloat = eventInfo.find(floatSymbol);
                        idxIsInt = eventInfo.find(intSymbol);
                        if ((idxIsFloat == std::string::npos) && (idxIsInt == std::string::npos)){
                            ss2 >> intTypeInfo;
                            m_tempInt_tab.push_back(intTypeInfo);
                        }
                        else {
                            ss2 >> floatTypeInfo;
                            m_tempFloat_tab.push_back(floatTypeInfo);
                        }
                    }
                    m_EventIntInfo_tab.push_back(m_tempInt_tab);
                    m_EventFloatInfo_tab.push_back(m_tempFloat_tab);
                    target_information.push_back(InformationTemp);
                }
                m_tempInt_tab.clear();
                m_tempFloat_tab.clear();
                InformationTemp.clear();
                idxComment1 = line.find(CommentSymbol1);
                idxComment2 = line.find(CommentSymbol2);
                idxComment3 = line.find(CommentSymbol3);
                idxComment4 = line.find(CommentSymbol4);
                idxComment5 = line.find(CommentSymbol5);
                if( (idxComment1 == std::string::npos) && (idxComment2 == std::string::npos) && (idxComment3 == std::string::npos) && (idxComment4 == std::string::npos) && (idxComment5 == std::string::npos) )
                {
                    if ( (nline > eventline+1) ) {
                        std::stringstream ss(line);
                        std::stringstream ss2(line);
                        while (ss >> particleInfo){
                            idxIsFloat = particleInfo.find(floatSymbol);
                            idxIsInt = particleInfo.find(intSymbol);
                            if ((idxIsFloat == std::string::npos) && (idxIsInt == std::string::npos)){
                                ss2 >> intTypeInfo;
                                m_tempInt_tab.push_back(intTypeInfo);
                            }
                            else {
                                ss2 >> floatTypeInfo;
                                m_tempFloat_tab.push_back(floatTypeInfo);
                            }
                        }
                        m_particleTempInt_tab.push_back(m_tempInt_tab);
                        m_particleTempFloat_tab.push_back(m_tempFloat_tab);
                    }
                }
                else { m_tempInt_tab.clear();  m_tempFloat_tab.clear();  }
                if ( (!(idxComment1 == std::string::npos) || !(idxComment2 == std::string::npos) || !(idxComment3 == std::string::npos) || !(idxComment4 == std::string::npos) || !(idxComment5 == std::string::npos)) && m_particleTempInt_tab.size()!=0 ) { 
                    addline = nline;
                    storeCommentline.push_back(addline);
                    ifFoundEvent=false; 
                    m_ParticleIntInfo_tab.push_back(m_particleTempInt_tab);
                    m_ParticleFloatInfo_tab.push_back(m_particleTempFloat_tab);
                    m_particleTempFloat_tab.clear(); 
                    m_particleTempInt_tab.clear(); 
                }
            }
            if (nline>beginRwgtline && !ifFoundInitRwgt && ifFoundRwgt) {
                idx2 = line.find(symbol2);
                if (! (idx2 == std::string::npos) ){
                    int p1 = line.find_first_of(">");
                    int p2 = line.find_last_of("<");
                    std::string Str = line.substr(p1+1, p2-p1-1);
                    modifyweightTemp.push_back(Str);
                    std::stringstream ss(Str); 
                    ss>>rwgtWeight;
                    weightValueTemp.push_back(rwgtWeight);
                    int p3 = line.find_first_of("'");
                    int p5 = line.find_first_of("\"");
                    int id;
                    if (p3 != -1){
                        int p4 = line.find_first_of("'",p3+1);
                        std::string temp = line.substr(p3+1, p4-p3-1);
                        std::stringstream ss(temp);
                        ss>>id;
                        weightBlockTemp.push_back(id);
                    }
                    else if (p5 != -1){
                        int p6 = line.find_first_of("\"",p5+1);
                        std::string temp = line.substr(p5+1, p6-p5-1);
                        std::stringstream ss(temp);
                        ss>>id;
                        weightBlockTemp.push_back(id);       //get weight ID in each block
                    }
                }
            }
            if (line == endRwgt) {
                ifFoundRwgt = false;
                m_WeightID.push_back(weightBlockTemp);
                m_WeightValue.push_back(weightValueTemp);
                modify_weight.push_back(modifyweightTemp);
                weightBlockTemp.clear();
                weightValueTemp.clear();
                modifyweightTemp.clear();
            }
        }
        LHEfile.close();
    }
    ATH_MSG_INFO("This file contains "<<m_ParticleIntInfo_tab.size()<<" events in total");

    //===========before doing event check by looping all events block, we need to check the format of weight ID and Name====================
    //===========Check 1 : No duplicate for weight ID or weightName====================
    bool isInitAscendOrder = false;                  //is initial weight id in ascending order
    bool isIdDuplicate = false;                      //is weight id duplicated
    bool isAscendOrder = false;                      //is weight id in ascending order
    bool isInitDuplicate = false;                    //is initial weight id duplicated
    bool isInitNameDuplicate = false;                //is initial weight name duplicated

    int NumberOfGroup = m_GroupName.size();
    int NumberOfInitialWeight = m_WeightInitID.size();

    if (NumberOfGroup == 0){
        ATH_MSG_INFO("Will not apply weight checks due to no weight information");
    }
    else {
        //=========check duplications in weightGroup name=========
        if ( !nameDuplicate(m_GroupName) )  ATH_MSG_INFO("No Duplicate for weight group name "); 
        else ATH_MSG_WARNING("Duplication occurs in WeightGroup Name");

        //=========check Special Symbol in Weight-groupName========
        if ( !CheckTextNumberOrLetter(m_GroupName) ) ATH_MSG_INFO("No Special Letters Or Symbols in weightgroup Name item");
        else ATH_MSG_WARNING("Some Special Letters Or Symbols detected in weightgroup Name item");

        for (int i = 0 ; i < NumberOfInitialWeight ; i++){
            isInitDuplicate = isInitDuplicate || idDuplicate(m_WeightInitID[i]);
            isInitNameDuplicate  = isInitNameDuplicate || nameDuplicate(m_WeightInitName[i]);
            isInitAscendOrder = isInitAscendOrder || std::is_sorted(std::begin(m_WeightInitID[i]), std::end(m_WeightInitID[i]));
        }

        int eventWeight_length = m_WeightID.size();      //two dimension vector, get the number of events
        for(int i = 0 ; i < eventWeight_length ; i++) {
            isIdDuplicate   = isIdDuplicate || idDuplicate(m_WeightID[i]);
            isAscendOrder = isAscendOrder || std::is_sorted(std::begin(m_WeightID[i]), std::end(m_WeightID[i]));
        }
        if (isInitAscendOrder && isAscendOrder) { ATH_MSG_INFO("Weight IDs are in Ascending order"); }
        else {ATH_MSG_WARNING("Weight IDs are not in Ascending order"); }

        if (!isInitNameDuplicate) ATH_MSG_INFO("No Duplicate for Initial weight name ");
        else ATH_MSG_WARNING("Duplication occurs in Initial weight name");
        if (!isInitDuplicate && !isIdDuplicate) { ATH_MSG_INFO("No Duplicate for weight IDs"); }
        else ATH_MSG_WARNING("Duplication occurs in weight IDs");

        std::string weight0_1 = "nominal";
        std::string weight0_2 = "Default";
        for (int i = 0 ; i < NumberOfInitialWeight ; i++)
        {
            int weightInit_length = m_WeightInitID[i].size();
            for (int j = 0 ; j < weightInit_length ; j++) {
                int temp_id = -99;
                std::string temp_name;
                temp_id = m_WeightInitID[i].at(j);
                if (temp_id == 0) {
                    temp_name = m_WeightInitName[i].at(j);
                    if (temp_name == weight0_1 || temp_name == weight0_2) { ATH_MSG_INFO("the weight with ID 0 has right name and it is "<<temp_name); }
                    else { ATH_MSG_WARNING("the weight with ID 0 has wrong name and it is "<<temp_name); }
                }
            }
        }

        //=======Check each event if the first weight of the list is the same weight indicated in the XWGTUP field of the first line of the <event> block
        float evtWeight = 0;
        float listWeight = 0;
        float absWeightDiff = 0;
        bool isConsistent = true;
        if (m_WeightValue.size()!=0){
            for (int i = 0 ; i < eventWeight_length ; i++)
            {
                evtWeight = m_EventFloatInfo_tab[i].at(0);
                listWeight = m_WeightValue[i].at(0);
                absWeightDiff = std::abs(listWeight - evtWeight);
                if (absWeightDiff > m_weight_diff) { 
                    isConsistent = false; 
                    m_WeightValue[i].at(0) = m_EventFloatInfo_tab[i].at(0);
                    ATH_MSG_WARNING("Not Consistent: the first weight of the list and XWGTUP field of the first line of the <event> block");
                    break;
                }
            }
        }
        if (isConsistent) ATH_MSG_INFO("Consistent: the first weight of the list and XWGTUP field of the first line of the <event> block");

        //=============From now on, we need to check the consistence between each events and the header=========
        std::vector<int> tempWeight;
        for (int i = 0 ; i < NumberOfInitialWeight ; i++) {
            int parInitLength = m_WeightInitID[i].size();
            for (int j = 0 ; j < parInitLength ; j++) {
                int initId = m_WeightInitID[i].at(j);
                tempWeight.push_back(initId);
            }
        }
        bool isSameValue = true;
        for (int i = 0 ; i < eventWeight_length ; i++){
            int InitialWeightNumber = tempWeight.size();
            int IEveWeightIDNumber = m_WeightID[i].size();
            if (InitialWeightNumber == IEveWeightIDNumber) { 
                for (int j = 0 ; j < IEveWeightIDNumber ; j++){
                    if ( tempWeight.at(j) != m_WeightID[i].at(j) ) 
                    { 
                        isSameValue = false;
                        ATH_MSG_WARNING("Weight NOT Consistent in header and in list");
                        break;
                    }
                }
            }
        }
        if (isSameValue) { ATH_MSG_INFO("Weight Consistent in header and in list"); }
    }

    //===========check particle number consistence============================
    int lineNumber = storeCommentline.size();
    bool numParticleConsistance = true;
    for (int i = 0 ; i < lineNumber ; i++) {
        int tempComment = storeCommentline[i];
        int tempEvent = m_storeEventLine[i];
        int No_particles = m_EventIntInfo_tab[i][0];
        if ( (tempComment-tempEvent) !=  (No_particles+2) ) { 
            numParticleConsistance = false;
            ATH_MSG_WARNING("The number of particles is NOT corresponds to the first integar of the first line");
            break;
        }
    }
    if (numParticleConsistance) { ATH_MSG_INFO("The number of particles is corresponds to the first integar of the first line"); }


    // Holder for filter outcome; allows us to check all filters on each event and diagnose multiple problems at once
    n_events = m_EventIntInfo_tab.size();
    n_particles = m_ParticleIntInfo_tab.size();

    //===========Initial Information About Beam========================
    if (m_InitFloatInfo_tab.size()==5) {
        m_cm_energy = m_InitFloatInfo_tab[0] + m_InitFloatInfo_tab[1];
        ATH_MSG_INFO("Beam Energy is " << m_cm_energy);
    }

    //===========loop by <event> block============================
    int evtNumber = m_EventIntInfo_tab.size();
    for (int i = 0 ; i < evtNumber ; i++){

        int par_length = m_ParticleIntInfo_tab[i].size();
        int   par_status = 0;
        float par_final_energy;
        float par_incoming_energy;
        float par_particle_energy = 0;   //used to check negative energy
        float par_diff_energy = 0;
        float par_E_Xmomentum=0;         //check energe & momentum balance 
        float par_E_Ymomentum=0;         //check energe & momentum balance 
        float par_E_Zmomentum=0;         //check energe & momentum balance 
        float par_Xmomentum=0;           //check tachyons 
        float par_Ymomentum=0;           //check tachyons 
        float par_Zmomentum=0;           //check tachyons 
        float momentum=0;          //Transverse momentum = sqrt(Px^2+Py^2)

        //=========particle and event information structure===========
        if ( m_EventIntInfo_tab[i].size()%2 != 0 || m_EventFloatInfo_tab[i].size()%4 != 0 ) { m_invalidEventformatCheckRate++; }

        for (int j = 0 ; j < par_length ; j++){
            if ( m_ParticleIntInfo_tab[i][j].size()%6 != 0 || m_ParticleFloatInfo_tab[i][j].size()%7 != 0 ) { m_invalidParticleFormatCheckRate++; }

            //==========check for bad PDG IDs===========
            int  par_pdgid = m_ParticleIntInfo_tab[i][j].at(0); 
            if (!MC::PID::isValid(par_pdgid)) {
                ATH_MSG_WARNING("Invalid PDG ID found: "<<par_pdgid);
                if (m_unknownPDGIDTest && std::find(m_unknownPDGID_tab.begin(), m_unknownPDGID_tab.end(), par_pdgid)==m_unknownPDGID_tab.end()){
                    m_unknownPDGIDCheckRate++;
                }
            }

            //========calculate total energy for all particles for each event=======
            par_particle_energy = m_ParticleFloatInfo_tab[i][j].at(3);
            par_status = m_ParticleIntInfo_tab[i][j].at(1);
            if (par_status == -1) {
                par_incoming_energy = m_ParticleFloatInfo_tab[i][j].at(3);
            }
            else if (par_status == 1) {
                par_final_energy += m_ParticleFloatInfo_tab[i][j].at(3);
                par_E_Xmomentum += m_ParticleFloatInfo_tab[i][j].at(0);
                par_E_Ymomentum += m_ParticleFloatInfo_tab[i][j].at(1);
                par_E_Zmomentum += m_ParticleFloatInfo_tab[i][j].at(2);
            }

            //============check negative energy ==========
            if (par_particle_energy < 0) { m_NegativeParticleCheckRate++;}

            //===============check nan and negative value===============
            float kinematicVar = 0;;                 // px,py,pz,E,m
            for(int z = 0 ; z < 5 ; z++){
                kinematicVar = m_ParticleFloatInfo_tab[i][j].at(z);
                if (std::isnan(kinematicVar)) { m_particleNanCheckRate++; }
                if (std::isinf(kinematicVar)) { m_particleInfiniteCheckRate++; }
            }

            //=============check tachyons ===============
            par_Xmomentum = std::abs(m_ParticleFloatInfo_tab[i][j].at(0));
            par_Ymomentum = std::abs(m_ParticleFloatInfo_tab[i][j].at(1));
            par_Zmomentum = std::abs(m_ParticleFloatInfo_tab[i][j].at(2));
            if ((par_particle_energy<par_Xmomentum) || (par_particle_energy<par_Ymomentum) || (par_particle_energy<par_Zmomentum))
            { m_tachyonParticleCheckRate++; }

        }

        //========check energy balance  ====================
        par_diff_energy = par_incoming_energy - par_final_energy;
        if (par_diff_energy >= m_energy_diff){
            ATH_MSG_WARNING("Difference between final particles energy and incoming particles energy is larger than expected."<<" And energy for incoming particles is "<<par_incoming_energy<<", energy for final particles is "<<par_final_energy);
            m_EnergyBalanceCheckRate++;
        }

        //=========calculate Momentum Balance================
        momentum = calculateMomentum(par_E_Xmomentum, par_E_Ymomentum);
        if (momentum >= m_momentumBalance)
        {
            m_MomentumBalanceCheckRate++;
            ATH_MSG_WARNING("Transverse momentun is "<<momentum<<" , NOT balanced");
        }

    }
    //===========event and particle structure check=============================
    if (m_invalidEventformatCheckRate==0) ATH_MSG_INFO("Event information has right information structure");
    if (m_invalidParticleFormatCheckRate==0) ATH_MSG_INFO("Particle information has right information structure");
    if (m_invalidEventformatCheckRate!=0 || m_invalidParticleFormatCheckRate!=0) ATH_MSG_WARNING("Wrong information format structure and need to check");

    //===========here we do energe balance check=======================================
    if (m_EnergyBalanceCheckRate==0) { ATH_MSG_INFO("Energy is balanced for all events");}
    if (m_MomentumBalanceCheckRate==0) { ATH_MSG_INFO("Momentum is balanced for all events");}

    //============check negative energy ==========
    if ( m_NegativeParticleCheckRate==0 ) ATH_MSG_INFO("No particle with negative energy");
    else ATH_MSG_WARNING("Particle with negative energy detected");

    //===============check nan and negative value===============
    if ( m_particleNanCheckRate==0 ) ATH_MSG_INFO("No nan variables detected");
    else ATH_MSG_WARNING("Paritlce with nan variables detected"); 
    if ( m_particleInfiniteCheckRate==0 ) ATH_MSG_INFO("No infinite variables detected");
    else ATH_MSG_WARNING("Paritlce with infinite variables detected"); 

    //=========Check tachyons and give information===========
    if ( m_tachyonParticleCheckRate==0 )  ATH_MSG_INFO("no possible tachyon in this production mode");

    //============check the weight spread in log scale (XWGTUP)==================
    float xwgtup = 0;
    float maxXwgtup = 0;
    float minXwgtup = 0;
    float diffXwgtup = 0;

    maxXwgtup = m_EventFloatInfo_tab[0].at(0);
    minXwgtup = m_EventFloatInfo_tab[0].at(0);
    for (int i = 0 ; i < evtNumber ; i++){
        xwgtup = std::abs(m_EventFloatInfo_tab[i].at(0));
        maxXwgtup = std::max(xwgtup, maxXwgtup);
        minXwgtup = std::min(xwgtup, minXwgtup);
    }
    diffXwgtup = std::log10(maxXwgtup) - std::log10(minXwgtup);
    if (diffXwgtup < m_abs_XWGTUP) { 
        m_isLargeWeight = true;
        ATH_MSG_INFO("now the weight is in proper range");
    }
    else { ATH_MSG_WARNING("Weight spread in log scale is larger than expexted value");  }

    //========here create a new copy of events.lhe file but removed some useless charactors========
    if (m_ifCreateNewLheFile) {
        int nn_line = 0;
        std::ofstream ModifiedFile;
        ModifiedFile.open(m_modifiedLheFile.c_str());
        if (!ModifiedFile.fail()){
            ATH_MSG_INFO("modified events.lhe are created");
            LHEfile.open(m_eventsLheFile.c_str());
            if (!LHEfile.fail()){
                while(std::getline(LHEfile,line, '\n')){
                    nn_line++;
                    if(line.length()!=0){
                        if (modify_weight.size()!=0){
                            for (int i = 0 ; i < lineNumber ; i++)
                            {
                                int temp_evtline = m_storeEventLine[i];
                                std::string target_character = target_information[i].at(2);
                                std::string replaced_weight = modify_weight[i].at(0);
                                int s_length = target_character.length();
                                if (nn_line == temp_evtline+1) {
                                    int pos = line.find(target_character);
                                    line.replace(pos, s_length, replaced_weight); 
                                }
                            }
                        }
                        ModifiedFile<<line<<std::endl;
                    }
                }
            }
            LHEfile.close();
        }
        ModifiedFile.close();
    }

    return StatusCode::SUCCESS;
}


StatusCode TestLHE::execute()
{
    return StatusCode::SUCCESS;
}

StatusCode TestLHE::finalize()
{
    ATH_MSG_INFO("Total events = " << n_events );
    ATH_MSG_INFO("Event rate with invalid event information format = " << m_invalidEventformatCheckRate*100.0/n_events << "%" );
    ATH_MSG_INFO("Event rate with invalid particle information format = " << m_invalidParticleFormatCheckRate*100.0/n_particles << "%" );
    ATH_MSG_INFO("Event rate with no energy balance = " << m_EnergyBalanceCheckRate*100.0/n_particles << "%" ); 
    ATH_MSG_INFO("Event rate with no momentum balance = " << m_MomentumBalanceCheckRate*100.0/n_particles << "%" );
    ATH_MSG_INFO("Event rate with Nan value = " << m_particleNanCheckRate*100.0/n_particles << "%" );
    ATH_MSG_INFO("Event rate with Infinite value = " << m_particleInfiniteCheckRate*100.0/n_particles << "%" );
    ATH_MSG_INFO("Event rate with negative energy = " << m_NegativeParticleCheckRate*100.0/n_particles << "%" );
    ATH_MSG_INFO("Event rate with tachyons = " << m_tachyonParticleCheckRate*100.0/n_particles << "%" ); 
    ATH_MSG_INFO("Event rate with unknown PDG IDs " << m_unknownPDGIDCheckRate*100.0/n_particles << "%" );
    if (m_isLargeWeight) ATH_MSG_INFO("The weight spread in log scale is pretty large");
    if (!m_unknownPDGIDTest) ATH_MSG_INFO("The check for unknown PDG IDs is switched off, so it is not included in the final TestLHE efficiency"); 

    return StatusCode::SUCCESS;

}

#endif
