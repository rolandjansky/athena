/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
   */
#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/TestLHE.h"
#include "GaudiKernel/DataSvc.h"
#include "TruthUtils/PIDHelpers.h"
#include "AGDDControl/XercesParser.h"

// For find
#include <algorithm>

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

    declareProperty("InfoStructureTest",        m_infoStructureTest = true);
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
    m_unknownPDGID_tab.clear();
    m_SusyPdgID_tab.clear();

    // =========== Found File 1 ===============
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

    // =========== Found File 2 ===============
    std::ifstream susyFile;
    susyFile.open("susyParticlePdgid.txt");
    int susyPdgID;
    if (!susyFile.fail())
    {
        while(getline(susyFile, line))
        {
            std::stringstream ss1(line);
            ss1>>susyPdgID;
            m_SusyPdgID_tab.push_back(susyPdgID);
        }
        susyFile.close();
    }
    else
    {
        ATH_MSG_INFO("susyParticlePdgid list for PDG IDs not provided");
    }

    m_invalidEventformatCheckRate = 0;
    m_invalidParticleFormatCheckRate = 0;
    m_NegativeParticleCheckRate = 0;
    m_EnergyBalanceCheckRate = 0;
    m_MomentumBalanceCheckRate = 0;
    m_particleNanCheckRate = 0;
    m_particleInfiniteCheckRate = 0;
    m_tachyonParticleCheckRate = 0;
    m_unknownPDGIDCheckRate = 0;
    m_idDuplicateCheckRate = 0;
    m_nameDuplicateCheckRate = 0;
    m_idInitDuplicateCheckRate = 0;
    m_nameSpecialLetterCheckRate = 0;
    m_isAscendOrderCheckRate = 0;
    m_isInitAscendOrderCheckRate = 0;
    m_nameForId0CheckRate = 0;
    m_particleNumberCheckRate = 0;
    m_weightSameValueCheckRate = 0;
    m_weightConsistentCheckRate = 0;

    m_unknownPDGID_tab.clear();
    m_G4pdgID_tab.clear();
    m_SusyPdgID_tab.clear();
    m_GroupName.clear();
    m_InitID.clear();
    m_WeightInitID.clear();
    m_WeightInitName.clear();
    m_WeightID.clear();
    m_WeightValue.clear();
    modify_weight.clear();
    m_storeRwgtLine.clear();

    m_storeEventWeight.clear();
    m_storeListWeight.clear();
    m_EventIntInfo_tab.clear();
    m_EventFloatInfo_tab.clear();
    m_ParticleIntInfo_tab.clear();
    m_ParticleFloatInfo_tab.clear();
    m_InitIntInfo_tab.clear();
    m_InitFloatInfo_tab.clear();
    m_particleTempInt_tab.clear();
    m_particleTempFloat_tab.clear();

    n_events = 0;
    n_particles = 0;
    version = 0;

    XercesParser XParser;
    if ( XParser.ParseFile(m_eventsLheFile) ) { ATH_MSG_WARNING("Some Errors are detected when first reading LHE files"); }
    else  { ATH_MSG_INFO("No Errors are detected when first reading LHE files"); }

    LHEfile.open(m_eventsLheFile.c_str());
    if(LHEfile.fail()) ATH_MSG_WARNING("Can not find events.lhe files in this directory");

    if (m_ifCreateNewLheFile) { ModifiedFile.open(m_modifiedLheFile.c_str()); }

    return StatusCode::SUCCESS;
}


StatusCode TestLHE::execute()
{
    std::string currentLine; 
    int nline = 0;                               //to record line number
    int eventline = 0;                           //looking for keyword <event>, check <event> block 
    int initline = 0;                            //looking for keywork <init> , check <init> initial beam information
    int commentLine = 0;

    int beginInitRwgtline = 0;
    int beginRwgtline = 0;                        //find <rwgt>

    int intTypeInfo;
    float floatTypeInfo;

    bool readingInit     = false;
    bool readingEvent    = false;
    bool readingInitRwgt = false;
    bool readingRwgt     = false;
    std::string initialInfo;
    std::string eventInfo;
    std::string particleInfo;

    if (LHEfile.good()) 
    {
        while(std::getline(LHEfile, currentLine))
        {
            ++nline;

            // ======== to make sure that we are reading a LHE file ==========
            if (nline == 1)
            {
                version = check_lheSymbol(currentLine);
                if (version == 3 || version == 2)
                {
                    ATH_MSG_DEBUG("Reading a LHE file with 'LesHouchesEvents' tag");
                }
            }

            // ======== to find the <header> of a LHE File ==========
            if ( currentFind("<header>", currentLine) ) 
            {
                ATH_MSG_INFO("TestLHE tool begin to check LHE file");
            }

            // ======== checking initial block in LHE file ==========
            if (currentFind("<initrwgt>", currentLine) )
            {
                readingInitRwgt = true;
                beginInitRwgtline = nline;
            }
            if (readingInitRwgt && nline > beginInitRwgtline )
            {
                if ( currentFind("<weightgroup", currentLine) )
                {
                    std::string name = GetGroupName(currentLine, "<weightgroup");
                    m_GroupName.push_back(name);
                }

                if ( currentFind("</weight>", currentLine) )
                {
                    int id = -1;
                    std::string name;
                    id = GetWeightId(currentLine, "</weight>");
                    m_InitID.push_back(id);
                    m_WeightInitID.push_back(id);
                    name = GetWeightName(currentLine, "</weight>");
                    m_WeightInitName.push_back(name);
                }

                if ( currentFind("</weightgroup>", currentLine) ) 
                {
                    int NumberOfGroup = 0;
                    NumberOfGroup = m_GroupName.size();

                    if (NumberOfGroup != 0)
                    {
                        initList = check_InitBlock(m_GroupName, m_WeightInitName, m_InitID);
                        m_nameDuplicateCheckRate     += std::get<0>(initList);
                        m_nameSpecialLetterCheckRate += std::get<1>(initList);
                        m_idInitDuplicateCheckRate   += std::get<2>(initList);
                        m_isInitAscendOrderCheckRate += std::get<3>(initList);
                        m_nameForId0CheckRate        += std::get<4>(initList);
                    }
                    m_GroupName.clear();
                    m_InitID.clear();
                    m_WeightInitName.clear();
                }
                if ( currentFind("</initrwgt>", currentLine))
                    readingInitRwgt = false;
            }

            // ======== checking <init> line in each <event> block =======
            if ( currentFind("<init>", currentLine) )
            {
                readingInit = true;
                initline = nline;
            }
            if (readingInit && (nline>initline) && (nline<initline+3))
            {
                std::stringstream ss1(currentLine);
                std::stringstream ss2(currentLine);

                while (ss1>>initialInfo)
                {
                    if (  !(currentFind("E", initialInfo)) && !(currentFind(".", initialInfo)) )
                    {
                        ss2 >> intTypeInfo;
                        m_InitIntInfo_tab.push_back(intTypeInfo);
                    }
                    else
                    {
                        ss2 >> floatTypeInfo;
                        m_InitFloatInfo_tab.push_back(floatTypeInfo);
                    }
                }
            }
            if (currentFind("</init>", currentLine))
            {
                readingInit = false;
                if (m_InitIntInfo_tab.size() == 9 && m_InitFloatInfo_tab.size() == 5)
                {
                    IDBMUP.first  = m_InitIntInfo_tab[0];
                    IDBMUP.second = m_InitIntInfo_tab[1];
                    PDFGUP.first  = m_InitIntInfo_tab[2];
                    PDFGUP.second = m_InitIntInfo_tab[3];
                    PDFSUP.first  = m_InitIntInfo_tab[4];
                    PDFSUP.first  = m_InitIntInfo_tab[5];
                    IDWTUP        = m_InitIntInfo_tab[6];
                    NPRUP         = m_InitIntInfo_tab[7];
                    LPRUP         = m_InitIntInfo_tab[8];

                    EBMUP.first = m_InitFloatInfo_tab[0];
                    EBMUP.second = m_InitFloatInfo_tab[1];
                    XSECUP = m_InitFloatInfo_tab[2];
                    XERRUP = m_InitFloatInfo_tab[3];
                    XMAXUP = m_InitFloatInfo_tab[4];
                }
                else
                {
                    throw std::runtime_error
                        ("Wrong Initial Information format in LHE file");
                }
                m_cm_energy = EBMUP.first+EBMUP.second;
                m_InitIntInfo_tab.clear();
                m_InitFloatInfo_tab.clear();
            }

            // ======= checking <event> block =========
            if ( currentFind("<event", currentLine) )
            {
                readingEvent = true;
                eventline = nline;
                n_events += 1;
            }
            if (readingEvent)
            {
                if ( (nline == eventline+1))
                {
                    m_EventIntInfo_tab = GetEventIntInfo(currentLine);
                    m_EventFloatInfo_tab = GetEventFloatInfo(currentLine);
                }
                if ( (nline > eventline+1))
                {
                    m_particleTempInt_tab = GetEventIntInfo(currentLine);
                    m_particleTempFloat_tab = GetEventFloatInfo(currentLine);
                    m_ParticleIntInfo_tab.push_back(m_particleTempInt_tab);
                    m_ParticleFloatInfo_tab.push_back(m_particleTempFloat_tab);

                    m_particleTempInt_tab.clear();
                    m_particleTempFloat_tab.clear();
                    if ( (currentFind("</event>", currentLine)) || (currentFind("#pdf", currentLine)) || (currentFind("<mgrwt>", currentLine)) || currentFind("</mgrwt>", currentLine) || currentFind("<rscale>", currentLine) || currentFind("<asrwt>", currentLine) || currentFind("<pdfrwt>", currentLine) || currentFind("#pdf", currentLine) || (currentFind("#rwgt", currentLine)) || (currentFind("<rwgt>", currentLine)))
                    {
                        readingEvent = false;

                        m_invalidEventformatCheckRate += check_eventInfoFormat(m_EventIntInfo_tab, m_EventFloatInfo_tab);
                        if (m_invalidEventformatCheckRate == 0)
                        {
                            NUP = m_EventIntInfo_tab[0];
                            IDPRUP = m_EventIntInfo_tab[1];

                            XWGTUP = m_EventFloatInfo_tab[0];
                            SCALUP = m_EventFloatInfo_tab[1];
                            AQEDUP = m_EventFloatInfo_tab[2];
                            AQCDUP = m_EventFloatInfo_tab[3];
                            m_storeEventWeight.push_back(XWGTUP);
                        }

                        m_EventIntInfo_tab.clear();
                        m_EventFloatInfo_tab.clear();

                        n_particles += NUP;

                        for (int i = 0 ; i < NUP ; i++)
                        {
                            m_invalidParticleFormatCheckRate += check_particleInfoFormat(m_ParticleIntInfo_tab[i], m_ParticleFloatInfo_tab[i]);
                            if (m_invalidParticleFormatCheckRate == 0)
                            {
                                IDUP.push_back(m_ParticleIntInfo_tab[i][0]);
                                ISTUP.push_back(m_ParticleIntInfo_tab[i][1]);
                                MOTHUP1.push_back(m_ParticleIntInfo_tab[i][2]);
                                MOTHUP2.push_back(m_ParticleIntInfo_tab[i][3]);
                                ICOLUP1.push_back(m_ParticleIntInfo_tab[i][4]);
                                ICOLUP2.push_back(m_ParticleIntInfo_tab[i][5]);

                                mom particle{0,0,0,0,0};
                                particle = {m_ParticleFloatInfo_tab[i][0], m_ParticleFloatInfo_tab[i][1], m_ParticleFloatInfo_tab[i][2], m_ParticleFloatInfo_tab[i][3], m_ParticleFloatInfo_tab[i][4]};
                                FourMomentum.push_back(particle);
                                VTIMUP.push_back(m_ParticleFloatInfo_tab[i][5]);
                                SPINUP.push_back(m_ParticleFloatInfo_tab[i][6]);
                            }
                        }

                        // ========= check particle number consistence==================
                        commentLine = nline;
                        particleList = check_particleInfo(commentLine-eventline, NUP, m_energy_diff, m_momentumBalance, m_unknownPDGIDTest, IDUP, m_unknownPDGID_tab, ISTUP, FourMomentum);
                        m_particleNumberCheckRate   += std::get<0>(particleList); 
                        m_unknownPDGIDCheckRate     += std::get<1>(particleList);
                        m_NegativeParticleCheckRate += std::get<2>(particleList);
                        m_particleNanCheckRate      += std::get<3>(particleList);
                        m_particleInfiniteCheckRate += std::get<4>(particleList);
                        m_tachyonParticleCheckRate  += std::get<5>(particleList);
                        m_EnergyBalanceCheckRate    += std::get<6>(particleList);
                        m_MomentumBalanceCheckRate  += std::get<7>(particleList);
                        int par_pdgid = std::get<8>(particleList);

                        if (m_unknownPDGIDCheckRate != 0)
                        {
                            ATH_MSG_WARNING("Invalid PDG ID Found: "<< par_pdgid);
                        }

                        m_unknownPDGID_tab.clear();
                        m_ParticleIntInfo_tab.clear();
                        m_ParticleFloatInfo_tab.clear();
                        IDUP.clear();
                        ISTUP.clear();
                        MOTHUP1.clear();
                        MOTHUP2.clear();
                        ICOLUP1.clear();
                        ICOLUP2.clear();
                        FourMomentum.clear();
                        VTIMUP.clear();
                        SPINUP.clear();
                    }
                }
            }

            // ======= checking rwgt block ==========
            if ( currentFind("<rwgt>", currentLine) )
            {
                readingRwgt = true;
                beginRwgtline = nline;
                m_storeRwgtLine.push_back(beginRwgtline);
            }

            if ( (nline > beginRwgtline) && readingRwgt )
            {
                if (!currentFind("</rwgt>", currentLine))
                {
                    int id = 0;
                    double weight = 0.;
                    id = GetWeightId(currentLine, "<wgt id");
                    m_WeightID.push_back(id);
                    weight = GetWeight(currentLine);
                    m_WeightValue.push_back(weight);
                    int p1 = currentLine.find_first_of(">");
                    int p2 = currentLine.find_first_of("<", p1+1);
                    std::string str = currentLine.substr(p1+1, p2-p1-1);
                    if (id == 0)    modify_weight.push_back(str);
                }
                else
                {
                    readingRwgt = false;
                    m_storeListWeight.push_back(m_WeightValue[0]);
                    rwgtList = check_rwgtBlock(m_WeightInitID, m_WeightID);
                    m_weightSameValueCheckRate = std::get<0>(rwgtList);
                    m_idDuplicateCheckRate     = std::get<1>(rwgtList);
                    m_isAscendOrderCheckRate   = std::get<2>(rwgtList);
                    m_WeightID.clear();
                    m_WeightValue.clear();
                    m_WeightInitID.clear();
                }
            }
        }
        LHEfile.close();
    }

    //========here create a new copy of events.lhe file but removed some useless charactors========
    if (m_ifCreateNewLheFile) 
    {
        int nn_line = 0;
        std::string line;
        if (!ModifiedFile.fail())
        {
            ATH_MSG_INFO("modified events.lhe are created");
            LHEfile.open(m_eventsLheFile.c_str());
            if (!LHEfile.fail()){
                while(std::getline(LHEfile,line, '\n'))
                {
                    nn_line++;
                    for (int i = 0 ; i < n_events ; i++)
                    {
                        line = weight_Modification(nn_line, line, modify_weight[i], m_storeEventWeight[i], m_storeRwgtLine[i]);
                    }
                    ModifiedFile<<line<<std::endl;
                }
                modify_weight.clear();
                m_storeRwgtLine.clear();
            }
            LHEfile.close();
        }
        ModifiedFile.close();
    }

    return StatusCode::SUCCESS;
}

StatusCode TestLHE::finalize()
{
    ATH_MSG_INFO("Beam Energy = " << m_cm_energy );
    ATH_MSG_INFO("Total events = " << n_events );
    ATH_MSG_INFO("Total particles = " << n_particles );

    //============check the weight spread in log scale (XWGTUP)==================
    float maxXwgtup = 0;
    float minXwgtup = 0;
    float diffXwgtup = 0;

    maxXwgtup = *std::max_element(m_storeEventWeight.begin(), m_storeEventWeight.end());
    minXwgtup = *std::min_element(m_storeEventWeight.begin(), m_storeEventWeight.end());

    diffXwgtup = std::log10(maxXwgtup) - std::log10(minXwgtup);
    if (diffXwgtup < m_abs_XWGTUP) { 
        m_isLargeWeight = true;
        ATH_MSG_INFO("now the weight is in proper range");
    }
    else { ATH_MSG_WARNING("Weight spread in log scale is larger than expected value");  }

    //=======Check each event if the first weight of the list is the same weight indicated in the XWGTUP field of the first line of the <event> block
    int Lsize = m_storeListWeight.size();
    if (Lsize == 0)
        ATH_MSG_INFO("LHE file does not contain weight information, consistence check will not executed");
    else
    {
        m_weightConsistentCheckRate = check_weightConsistent(m_storeEventWeight, m_storeListWeight, m_weight_diff);
    }
    m_storeEventWeight.clear();
    m_storeListWeight.clear();

    if (m_invalidEventformatCheckRate==0)    ATH_MSG_INFO("Event information has right information structure");
    if (m_invalidParticleFormatCheckRate==0) ATH_MSG_INFO("Particle information has right information structure");
    if (m_invalidEventformatCheckRate!=0 || m_invalidParticleFormatCheckRate!=0) ATH_MSG_WARNING("Wrong information format structure and need to check");
    ATH_MSG_INFO("Event rate with invalid event information format = " << m_invalidEventformatCheckRate*100.0/n_events << "%" );
    ATH_MSG_INFO("Event rate with invalid particle information format = " << m_invalidParticleFormatCheckRate*100.0/n_particles << "%" );

    if (m_idInitDuplicateCheckRate==0)       {ATH_MSG_INFO("No Duplication occurs in Initial weight ID");}
    ATH_MSG_INFO("Event rate with initial duplicate id = " << m_idInitDuplicateCheckRate*100.0/n_events << "%");
    if (m_isInitAscendOrderCheckRate==0)     {ATH_MSG_INFO("Initial Weight id spreads in ascending order");}
    ATH_MSG_INFO("Event rate with initial weight id not in ascending order = " << m_isInitAscendOrderCheckRate*100.0/n_events << "%");

    if (m_idDuplicateCheckRate==0)       {ATH_MSG_INFO("No Duplication occurs in weight ID");}
    ATH_MSG_INFO("Event rate with duplicate id = " << m_idDuplicateCheckRate*100.0/n_events << "%");
    if (m_nameDuplicateCheckRate==0)     {ATH_MSG_INFO("No Duplication occurs in weight name");}
    ATH_MSG_INFO("Event rate with duplicate name = " << m_nameDuplicateCheckRate*100.0/n_events << "%");
    if (m_nameSpecialLetterCheckRate==0) {ATH_MSG_INFO("No Special letter occurs in weight name");}
    ATH_MSG_INFO("Event rate with special letter in weight name = " << m_nameSpecialLetterCheckRate*100.0/n_events << "%");
    if (m_isAscendOrderCheckRate==0)     {ATH_MSG_INFO("Weight id spreads in ascending order");}
    ATH_MSG_INFO("Event rate with weight id not in ascending order = " << m_isAscendOrderCheckRate*100.0/n_events << "%");
    if (m_nameForId0CheckRate==0)        {ATH_MSG_INFO("Format for weight id 0 is correct");}
    ATH_MSG_INFO("Event rate with wrong format for weight id 0 = " << m_nameForId0CheckRate*100.0/n_events << "%" );
    if (m_weightSameValueCheckRate==0)   {ATH_MSG_INFO("Weight Number Consistent in header and event block");}
    ATH_MSG_INFO("Event rate with different weight numbers in header and event block = " << m_weightSameValueCheckRate*100.0/n_events << "%");
    if (m_weightConsistentCheckRate==0)  {ATH_MSG_INFO("Weight Consistent in event line and list line");}
    ATH_MSG_INFO("Event rate with different weight value in event line and list line = " << m_weightConsistentCheckRate*100.0/n_events << "%");

    if (m_particleNumberCheckRate == 0)  {ATH_MSG_INFO("Particle Number is Consistent with the value in event line");}
    ATH_MSG_INFO("Event rate with correct particle number = "<<m_particleNumberCheckRate*100.0/n_events << "%");

    if (m_EnergyBalanceCheckRate==0) { ATH_MSG_INFO("Energy is balanced for all events");}
    ATH_MSG_INFO("Event rate with no energy balance = " << m_EnergyBalanceCheckRate*100.0/n_events << "%" ); 

    if (m_MomentumBalanceCheckRate==0) { ATH_MSG_INFO("Momentum is balanced for all events");}
    ATH_MSG_INFO("Event rate with no momentum balance = " << m_MomentumBalanceCheckRate*100.0/n_events << "%" );

    if ( m_NegativeParticleCheckRate==0 ) ATH_MSG_INFO("No particle with negative energy");
    else ATH_MSG_WARNING("Particle with negative energy detected");
    ATH_MSG_INFO("Event rate with negative energy = " << m_NegativeParticleCheckRate*100.0/n_particles << "%" );

    if ( m_particleNanCheckRate==0 ) ATH_MSG_INFO("No nan variables detected");
    else ATH_MSG_WARNING("Paritlce with nan variables detected"); 
    ATH_MSG_INFO("Event rate with Nan value = " << m_particleNanCheckRate*100.0/n_particles << "%" );
    if ( m_particleInfiniteCheckRate==0 ) ATH_MSG_INFO("No infinite variables detected");
    else ATH_MSG_WARNING("Paritlce with infinite variables detected"); 
    ATH_MSG_INFO("Event rate with Infinite value = " << m_particleInfiniteCheckRate*100.0/n_particles << "%" );

    if ( m_tachyonParticleCheckRate==0 )  ATH_MSG_INFO("no possible tachyon in this production mode");
    ATH_MSG_INFO("Event rate with tachyons = " << m_tachyonParticleCheckRate*100.0/n_particles << "%" ); 

    ATH_MSG_INFO("Event rate with unknown PDG IDs " << m_unknownPDGIDCheckRate*100.0/n_particles << "%" );
    if (m_isLargeWeight) ATH_MSG_INFO("The weight spread in log scale is pretty large");
    if (!m_unknownPDGIDTest) ATH_MSG_INFO("The check for unknown PDG IDs is switched off, so it is not included in the final TestLHE efficiency"); 

    return StatusCode::SUCCESS;
}

int TestLHE::check_lheSymbol(const std::string& Tagline)
{
    int lheTag = 0;
    if ( !currentFind("<LesHouchesEvents", Tagline) )
    {
        throw std::runtime_error
            ("Tried to read a file which does not start with the 'LesHouchesEvents' tag.");
        lheTag = 1;
    }
    if ( currentFind("version=\"3.0", Tagline)) lheTag = 3;
    else if (currentFind("version=\"2.0", Tagline)) lheTag = 2;
    else if ( !currentFind("version=\"1.0", Tagline)) 
    {
        throw std::runtime_error
            ("Tried to read a LesHouchesEvents file which is above version 3.");
    }

    return lheTag;
}

std::string TestLHE::GetGroupName(const std::string& Groupline, const std::string& symbol)
{
    std::string name;
    if ( currentFind(symbol, Groupline))
    {
        int p1 = Groupline.find_first_of("'");
        int p2 = Groupline.find_first_of("\"");

        if (p1 != -1)
        {
            int p3 = Groupline.find_first_of("'", p1+1);
            name = Groupline.substr(p1+1, p3-p1-1);
        }
        else if (p2 != -1)
        {
            int p4 = Groupline.find_first_of("\"", p2+1);
            name = Groupline.substr(p2+1, p4-p2-1);
        }
    }

    return name;
}

int TestLHE::GetWeightId(const std::string& IDline, const std::string& symbol)
{
    int id = 0;
    if ( currentFind(symbol, IDline) )
    {
        int p1 = IDline.find_first_of("'");
        int p2 = IDline.find_first_of("\"");
        if (p1 != -1)
        {
            int p3 = IDline.find_first_of("'", p1+1);
            std::string str = IDline.substr(p1+1, p3-p1-1);
            std::stringstream ss(str);
            ss>>id;
        }
        else if (p2 != -1)
        {
            int p4 = IDline.find_first_of("\"", p2+1);
            std::string str = IDline.substr(p2+1, p4-p2-1);
            std::stringstream ss(str);
            ss>>id;
        }
    }
    return id;
}

std::string TestLHE::GetWeightName(const std::string& wline, const std::string& symbol)
{
    std::string name;
    if ( currentFind(symbol, wline) )
    {
        int p1 = wline.find_first_of(">");
        int p2 = wline.find_first_of("<", p1+1);
        name = wline.substr(p1+1, p2-p1-1);
    }
    return name;
}

double TestLHE::GetWeight(const std::string& wline)
{
    double weight = 0;
    int p1 = wline.find_first_of(">");
    int p2 = wline.find_first_of("<", p1+1);
    std::string str = wline.substr(p1+1, p2-p1-1);
    std::stringstream ss(str);
    ss>>weight;
    return weight;
}

std::vector<int> TestLHE::GetEventIntInfo(const std::string& eventline)
{
    std::vector<int> Intvec;
    std::stringstream ss1(eventline);
    std::stringstream ss2(eventline);
    int intInfo;
    std::string tempInfo;
    while (ss1 >> tempInfo)
    {
        if (!currentFind("E", tempInfo) && !currentFind(".", tempInfo))
        {
            ss2 >> intInfo;
            Intvec.push_back(intInfo);
        }
    }
    return Intvec;
}

std::vector<float> TestLHE::GetEventFloatInfo(const std::string& eventline)
{
    std::vector<float> Floatvec;
    std::stringstream ss1(eventline);
    std::stringstream ss2(eventline);
    float floatInfo;
    int intInfo;
    std::string tempInfo;
    while (ss1 >> tempInfo)
    {
        if ( currentFind("E", tempInfo) || currentFind(".", tempInfo))
        {
            ss2 >> floatInfo;
            Floatvec.push_back(floatInfo);
        }
        else ss2 >> intInfo;
    }
    return Floatvec;
}

initInfo TestLHE::check_InitBlock(const std::vector<std::string>& groupname, const std::vector<std::string>& weightname, const std::vector<int>& weightid)
{
    int n_nameDup = 0;
    int n_idDup = 0;
    int n_nameSep = 0;
    int n_idAscend = 0;
    int n_id0name = 0;

    if ( nameDuplicate(groupname) || nameDuplicate(weightname) ) {n_nameDup += 1; } 
    if ( idDuplicate(weightid) ) {n_idDup += 1; }
    if ( !std::is_sorted(std::begin(weightid), std::end(weightid)) ) {n_idAscend += 1; }
    if ( CheckTextNumberOrLetter(groupname) ) {n_nameSep +=1; }
    if ( weightid[0] == 0 )
    {
        if ( weightname[0] != "nominal" && weightname[0] != "Default" ) {n_id0name += 1;}
    }

    std::array<int, 5> InitialArray;
    InitialArray[0] = n_nameDup;
    InitialArray[1] = n_nameSep;
    InitialArray[2] = n_idDup;
    InitialArray[3] = n_idAscend;
    InitialArray[4] = n_id0name;

    return InitialArray;
}

int TestLHE::check_eventInfoFormat(const std::vector<int>& intInfo, const std::vector<float>& floatInfo)
{
    int isWrong = 0;
    if (intInfo.size()!=2 || floatInfo.size()!=4) isWrong = 1;
    return isWrong;
}

int TestLHE::check_particleInfoFormat(const std::vector<int>& intInfo, const std::vector<float>& floatInfo)
{
    int isWrong = 0;
    if (intInfo.size()!=6 || floatInfo.size()!=7) isWrong = 1;
    return isWrong;
}

parInfo TestLHE::check_particleInfo(int numline, int nPar, float energyDiff, float momDiff, bool doTest, const std::vector<int>& pdgid, const std::vector<int>& unknownID, const std::vector<int>& Parstatus, const std::vector<mom>& FourMom)
{
    int n_parNum = 0;
    int n_unknownPDG = 0;
    int n_negPar = 0;
    int n_nanPar = 0;
    int n_infPar = 0;
    int n_tachyonPar = 0;
    int n_engBal = 0;
    int n_momBal = 0;
    int id = -1;

    float par_diff_energy = 0;
    float par_final_energy = 0;
    float par_incoming_energy = 0;
    float par_E_Xmomentum = 0;
    float par_E_Ymomentum = 0;
    float par_E_Zmomentum = 0;
    float momentum = 0;

    if (numline != (nPar+2)) { n_parNum = 1; }  
    for (int i = 0 ; i < nPar ; i++)
    {
        int par_pdgid = pdgid[i];
        if (!MC::PID::isValid(par_pdgid))
        {
            id = par_pdgid;
            if (doTest && std::find(unknownID.begin(), unknownID.end(), par_pdgid) == unknownID.end()) { n_unknownPDG += 1; }
        }

        int par_status = 0;
        int par_particle_energy = 0;

        par_status = Parstatus[i];
        par_particle_energy = FourMom[i].e;
        if (par_status == -1)
        {
            par_incoming_energy += FourMom[i].e;
        }
        else if (par_status == 1)
        {
            par_final_energy += FourMom[i].e;
            par_E_Xmomentum  += FourMom[i].px;
            par_E_Ymomentum  += FourMom[i].py;
            par_E_Zmomentum  += FourMom[i].pz;
        }

        if (par_particle_energy<0) n_negPar += 1;
        if (std::isnan(FourMom[i].px) || std::isnan(FourMom[i].py) || std::isnan(FourMom[i].pz) || std::isnan(FourMom[i].e) || std::isnan(FourMom[i].mass)) { n_nanPar += 1; }
        if (std::isinf(FourMom[i].px) || std::isinf(FourMom[i].py) || std::isinf(FourMom[i].pz) || std::isinf(FourMom[i].e) || std::isinf(FourMom[i].mass)) { n_infPar += 1; }
        if ( (FourMom[i].e<FourMom[i].px) || (FourMom[i].e<FourMom[i].py) || (FourMom[i].e<FourMom[i].pz) ) { n_tachyonPar += 1; }
    }

    par_diff_energy = par_incoming_energy - par_final_energy;
    if (par_diff_energy > energyDiff) { n_engBal += 1; }

    momentum = std::sqrt(par_E_Xmomentum*par_E_Xmomentum + par_E_Ymomentum*par_E_Ymomentum);
    if (momentum > momDiff) { n_momBal += 1; } 

    std::array<int, 9> particleArray;
    particleArray[0] = n_parNum;
    particleArray[1] = n_unknownPDG;
    particleArray[2] = n_negPar;
    particleArray[3] = n_nanPar;
    particleArray[4] = n_infPar;
    particleArray[5] = n_tachyonPar;
    particleArray[6] = n_negPar;
    particleArray[7] = n_momBal;
    particleArray[8] = id;

    return particleArray;
}

rwgtInfo TestLHE::check_rwgtBlock(const std::vector<int>& initWeightId, const std::vector<int>& rwgtId)
{
    int n_sameWeight = 0;
    int n_idDup = 0;
    int n_idAcend = 0;
    int Isize = initWeightId.size();
    int Rsize = rwgtId.size();
    if (Isize == Rsize)
    {
        for (int index = 0 ; index < Isize ; index++)
        {
            if (initWeightId[index] != rwgtId[index]) { n_sameWeight += 1;}
        }
    }

    if ( idDuplicate(rwgtId) )                                   { n_idDup += 1;}
    if ( !std::is_sorted(std::begin(rwgtId), std::end(rwgtId)) ) { n_idAcend += 1; }

    std::array<int, 3> rwgtArray;
    rwgtArray[0] = n_sameWeight;
    rwgtArray[1] = n_idDup;
    rwgtArray[2] = n_idAcend;

    return rwgtArray;
}

int TestLHE::check_weightConsistent(const std::vector<float>& eventWeight, const std::vector<float>& listWeight, float weightDiff)
{
    int Lsize = listWeight.size();
    int Esize = eventWeight.size();
    int n_notConsistent = 0;

    if (Lsize == Esize)
    {
        float evtWeight = 0;
        float lisWeight = 0;
        float absWeightDiff = 0;

        for (int i = 0 ; i < Esize ; i++)
        {
            evtWeight = eventWeight[i];
            lisWeight = listWeight[i];
            absWeightDiff = std::fabs(lisWeight - evtWeight);
            if ( std::fabs(absWeightDiff-weightDiff)>1e-4 )
            {
                lisWeight = evtWeight;
                n_notConsistent += 1;
            }
        }
    }
    else n_notConsistent = 1;

    return n_notConsistent;
}

std::string TestLHE::weight_Modification(int numLine, std::string& input, const std::string& modifyWeight, float eventWeight, int eventLine)
{
    if (input.length()!=0)
    {
        float targetW = eventWeight;
        std::string replacedS = modifyWeight;
        std::string targetS = std::to_string(targetW);
        if (numLine == eventLine+1)
        {
            int pos = -1;
            pos = input.find(replacedS);
            input.replace(pos, replacedS.length(), targetS);
        }
    }
    return input;
}

#endif
