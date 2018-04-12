/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#undef PACKAGE_VERSION

#include "TrigConfStorage/TrigConfCoolWriter.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/MCKLoader.h"

#include "CoolKernel/Exception.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include "TrigConf2COOLOnline/OnlTrigC2CWriter.h"

#include "TTCInfo/TrigConfSmKeyNamed.h"
#include "TTCInfo/TrigConfL1PsKeyNamed.h"
#include "TTCInfo/TrigConfHltPsKeyNamed.h"
#include "TTCInfo/TrigConfReleaseNamed.h"

#include "config/Configuration.h"

#include "dal/Partition.h"
#include "dal/TriggerConfiguration.h"
#include "dal/TriggerDBConnection.h"
#include "dal/L1TriggerConfiguration.h"
#include "HLTPUDal/HLTImplementationDB.h"

#include "RunControl/Common/Exceptions.h"
#include "RunControl/Common/RunControlCommands.h"
#include "RunControl/Common/OnlineServices.h"

#include "ipc/partition.h"
#include "is/inforeceiver.h"
#include "is/infoany.h"
#include "is/info.h"
#include "is/infodictionary.h"

#include "ers/ers.h"

#include "rc/RunParams.h"
#include "rc/RunParamsNamed.h"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include <algorithm>
#include <iostream>

ERS_DECLARE_ISSUE( TrigConf,
                   BadCoolDB,
                   "Archiving to COOL failed because " << explanation,
                   ((const char*) explanation)
                   )

ERS_DECLARE_ISSUE( TrigConf,
                   CoolWriting,
                   "Archiving to COOL failed because " << explanation,
                   ((const char*) explanation)
                   )

ERS_DECLARE_ISSUE( TrigConf,
                   InvalidConfiguration,
                   "Configuration error - " << explanation,
                   ((const char*) explanation)
                   )

ERS_DECLARE_ISSUE( TrigConf,
                   TriggerDBReadError,
                   "Error when reading TriggerDB: " << explanation,
                   ((const char*) explanation)
                   )

ERS_DECLARE_ISSUE( TrigConf,
                   ISPublishing,
                   "IS publishing error: " << explanation,
                   ((const char*) explanation)
                   )

ERS_DECLARE_ISSUE( TrigConf,
                   InvalidUserCommand,
                   "Invalid user command: " << explanation,
                   ((const char*) explanation)
                   )

using namespace std;
using namespace TrigConf;

OnlTrigC2CWriter::OnlTrigC2CWriter(const std::string & partitionName, const std::string& cooldb, const std::string& triggerdb, bool l1is, bool l1cool, bool mckcool ) :
   Controllable()
   , fConf2Cool( new TrigConfCoolWriter(cooldb) )
   , fTriggerDB(triggerdb)
   , fInfoRec(0)
   , fIPCPartition(0)
   , fHLTFrame(0)
   , fCTPConfig(0)
{
    conf().CoolDb()        = cooldb;
    conf().L1toIS()        = l1is;
    conf().L1toCOOL()      = l1cool;
    conf().PartitionName() = partitionName;
    conf().MCKtoCOOL()     = mckcool;

    fIPCPartition = new IPCPartition( conf().PartitionName() );
}

TrigConf::OnlTrigC2CWriter::~OnlTrigC2CWriter() noexcept {
   delete fIPCPartition;
   delete fHLTFrame;
   delete fCTPConfig;
   delete fConf2Cool;
   delete fInfoRec;
}


bool
OnlTrigC2CWriter::readOKS() {

    ERS_INFO("Reading configuration for partition " << conf().PartitionName());

    try {
        Configuration& oks = daq::rc::OnlineServices::instance().getConfiguration();

        // get partition
        const daq::core::Partition& partition = daq::rc::OnlineServices::instance().getPartition();

        // get trigger configuration
        const daq::core::TriggerConfiguration* triggerConfig = partition.get_TriggerConfiguration();
        if( triggerConfig == 0)
            throw daq::rc::ConfigurationIssue(ERS_HERE, "No trigger configuration object in partition!");


        // LVL1 configuration object (to get L1 PSK)
        const daq::core::L1TriggerConfiguration * l1Config = triggerConfig->get_l1();
        if(l1Config!=0) {
            conf().DBL1PSK() = l1Config->get_Lvl1PrescaleKey();
            ERS_INFO("Read L1 PSK " << conf().DBL1PSK() );
        } else {
            conf().DBL1PSK() = 0;
            ERS_INFO("No LVL1 configuration.");
        }


        // get the HLT configuration
        const daq::core::HLTImplementation * hlt = triggerConfig->get_hlt();
        if(hlt==0) {
            ERS_INFO("No HLTImplementation configured");
            return false;
        } else {
            ERS_INFO("Found HLT Implementation" << *hlt << " ... " << hlt->UID());
        }

        const daq::hlt::HLTImplementationDB* hltDB = oks.get<daq::hlt::HLTImplementationDB>( hlt->UID() );
        if(hltDB==0)
            ERS_INFO("HLT implementation '" << hlt->UID() << "' is not for running from TriggerDB.");

        // setting the HLT prescale key
        conf().DBHLTPSK() = hltDB!=0 ? boost::lexical_cast<unsigned long,std::string>( hltDB->get_hltPrescaleKey() ) : 0;

        // get the DB connection
        const daq::core::TriggerDBConnection * trigDb = triggerConfig->get_TriggerDBConnection();
        conf().UseTriggerDB() = (trigDb!=0);
        if( conf().UseTriggerDB() ) {
            conf().DBSMK()    = trigDb->get_SuperMasterKey();
            if (trigDb->get_Alias()!="") {
                conf().DBType()   = "dblookup";
                conf().DBServer() = trigDb->get_Alias();
            }
            else {
                conf().DBType()   = trigDb->get_Type();
                conf().DBServer() = trigDb->get_Server();
                conf().DBUser()   = trigDb->get_User();
                conf().DBPasswd() = trigDb->get_Password();
                conf().DBName()   = trigDb->get_Name();
            }
        } else {
            ERS_INFO("TriggerDBConnection not set in '" << triggerConfig->full_name() << "'");
        }
    }
    catch (const daq::rc::ConfigurationIssue & e) {
        InvalidConfiguration issue(ERS_HERE, e.what());
        ers::fatal(issue);
        return false;
    }
    catch (const daq::config::Generic & e) {
        InvalidConfiguration issue(ERS_HERE, e.what());
        ers::fatal(issue);
        return false;
    }

    //ERS_LOG("" << "\nRead configuration needed for " << getName() << " from oks\n");  // this is like cout but with time and line info

    return true;
}

bool
TrigConf::OnlTrigC2CWriter::subscribeToIS() {

    ERS_INFO("subscribing to IS (RunParams).");

    if(!fInfoRec) fInfoRec = new ISInfoReceiver(*fIPCPartition);

    try {
        fInfoRec->subscribe("RunParams.SOR_RunParams", &OnlTrigC2CWriter::runNumberCallback, this);
    }
    catch (const daq::is::Exception & ex ) {
        ers::fatal(ex);
        return false;
    }
    return true;
}


void
TrigConf::OnlTrigC2CWriter::readRunNumberFromIS() {

    ERS_INFO("readRunNumberFromIS entered");

    RunParamsNamed rp(*fIPCPartition,"RunParams.SOR_RunParams");

    rp.checkout();

    conf().CurrentRunNumber() = rp.run_number;

    ERS_INFO("Read new run number: " << conf().CurrentRunNumber());

}

void
TrigConf::OnlTrigC2CWriter::runNumberCallback(ISCallbackInfo *isc) {
    if (isc->reason() == is::Deleted)
        return;

    RunParams sor;
    isc->value(sor);

    conf().CurrentRunNumber() = sor.run_number;

    ERS_INFO("Received new run number: " << conf().CurrentRunNumber());

    return;
}

void
TrigConf::OnlTrigC2CWriter::publishToIS() {
    ERS_INFO("Publish to IS");

    TrigConfSmKeyNamed sm(*fIPCPartition,"RunParams.TrigConfSmKey");
    sm.SuperMasterKey     = conf().DBSMK();
    sm.SuperMasterComment = conf().DBSMcomment();

    TrigConfL1PsKeyNamed l1ps(*fIPCPartition,"RunParams.TrigConfL1PsKey");
    l1ps.L1PrescaleKey     = conf().DBL1PSK();
    l1ps.L1PrescaleComment = conf().DBL1PScomment();

    TrigConfHltPsKeyNamed hltps(*fIPCPartition,"RunParams.TrigConfHltPsKey");
    hltps.HltPrescaleKey     = conf().DBHLTPSK();
    hltps.HltPrescaleComment = conf().DBHLTPScomment();

    TrigConfReleaseNamed rel(*fIPCPartition,"RunParams.TrigConfRelease");
    rel.HLTReleaseVersion = getenv("AtlasVersion")?getenv("AtlasVersion"):"Unknown Version";

    if(getenv("AtlasProject")) {
        rel.HLTPatchProject = getenv("AtlasProject");
    } else {
        rel.HLTPatchProject = "Unknown Project";
    }
    rel.HLTExtraPatch = "";

    sm.print(std::cout);
    l1ps.print(std::cout);
    hltps.print(std::cout);
    rel.print(std::cout);

    try {
        sm.checkin();
        if(conf().L1toIS()) l1ps.checkin();
        hltps.checkin();
        rel.checkin();
    }
    catch (daq::is::Exception& ex) {
        TrigConf::ISPublishing issue(ERS_HERE, ex.what());
        ers::fatal(issue);
    }
    catch (std::exception & ex) {
        TrigConf::ISPublishing issue(ERS_HERE, ex.what());
        ers::fatal(issue);
    }
    catch (CORBA::SystemException& ex) {
        TrigConf::ISPublishing issue(ERS_HERE, ex._name());
        ers::fatal(issue);
    }
    catch (...) {
        TrigConf::ISPublishing issue(ERS_HERE,"unknown exception");
        ers::fatal(issue);
        //throw;
    }

}

void
TrigConf::OnlTrigC2CWriter::readTriggerDb() {

    if(!conf().UseTriggerDB()) return;

    m_forceTriggerDBReadAtPrepareForRun = false;

    // clear the menu
    delete fHLTFrame; fHLTFrame = 0;


    // get the TriggerDB connection
    std::string conn, user, pw;
    conf().TriggerDbConnectionParams(conn,user,pw);
    if(fTriggerDB!="") {
        conn = fTriggerDB;
    }
    cout << "Using triggerdb connection : " << conn << endl;
    //    cout << "user : " << user << endl;
    //    cout << "pw   : " << pw << endl;

    std::unique_ptr< StorageMgr > sm( new StorageMgr(conn,user,pw, cout) );

    sm->sessionMgr();

    conf().DBConnection() = sm->sessionMgr().connection();

    if(conf().DBSMK()!=0 && conf().DBHLTPSK()!=0) {

        fHLTFrame = new HLTFrame();

        ERS_INFO("Reading HLT menu from the TriggerDB with SMK " << conf().DBSMK() << " and HLT PSK " << conf().DBHLTPSK() << ".");

        fHLTFrame->setSMK(conf().DBSMK());
        fHLTFrame->thePrescaleSetCollection().set_prescale_key_to_load( conf().DBHLTPSK() );
        try {
            sm->hltFrameLoader().load( *fHLTFrame );
        }
        catch(const std::exception & e) {
            TrigConf::TriggerDBReadError issue(ERS_HERE, e.what());
            ers::fatal(issue);
        }
        //fHLTPrescaleSet->fillFromFrame(fHLTFrame);
        conf().DBSMcomment() = fHLTFrame->name();
        conf().DBHLTPScomment() = fHLTFrame->getPrescaleSetCollection().prescaleSet()->name();

        ERS_INFO("Done reading HLT menu");

    } else {
        ERS_INFO("Will not read SMK since one of these keys is 0: SMK = " << conf().DBSMK() << ", HLT PSK = " << conf().DBHLTPSK() << ".");
        conf().DBHLTPScomment() = "No HLT Prescale key specified";
        conf().DBSMcomment() = "Not read since HLT prescale key was not specified";
        conf().DBHLTPSK()=0;
    }

    if ( conf().MCKtoCOOL() ) {
        if ( conf().DBSMK()!=0 ) {
            ERS_INFO("Reading Menu-aware Monitoring MCK from the TriggerDB with SMK " << conf().DBSMK() << ".");

            try {
                // get the mck
                uint mck {0};
                std::string release {""};
                auto fMCKLoader = new MCKLoader(*sm);
                fMCKLoader->loadMCKlinkedToSMK(conf().DBSMK(), mck);
                conf().DBMCK() = mck;
                if ( conf().DBMCK() !=0 ) {
                    fMCKLoader->loadReleaseLinkedToMCK(conf().DBMCK(), release);
                    conf().DBMCKrelease() = release;
                }
            }
            catch(const std::exception & e) {
                TrigConf::TriggerDBReadError issue(ERS_HERE, e.what());
                ers::error(issue);
            }

            ERS_INFO("Done reading MCK " << conf().DBMCK() << " for release " << conf().DBMCKrelease() << ".");
        } else {
            ERS_INFO("Will not read MCK since SMK is 0: SMK = " << conf().DBSMK() << ".");
            conf().DBMCKinfo() = "Not read since SMK not specified";
        }
    } else {
        ERS_INFO("Writing of MCK in HLT archiver is disabled");
    }

    if( conf().L1toCOOL() ) {
        if( conf().DBSMK()!=0 ) {
            ERS_INFO("Reading LVL1 menu from the TriggerDB");

            // write also the L1 stuff to COOL (key+menu+prescales)
            // should only happen in rare circumstances
            delete fCTPConfig;
            fCTPConfig = new CTPConfig();

            fCTPConfig->setSuperMasterTableId(conf().DBSMK());
            try {
                sm->masterTableLoader().load(*fCTPConfig);
            }
            catch(std::exception e) {
                TrigConf::TriggerDBReadError issue(ERS_HERE, e.what());
                ers::fatal(issue);
            }
            ERS_INFO("LVL1 menu read");

            // prescales
            PrescaleSet l1pss;
            l1pss.setId(conf().DBL1PSK());
            try {
                sm->prescaleSetLoader().load(l1pss);
            }
            catch(std::exception e) {
                TrigConf::TriggerDBReadError issue(ERS_HERE, e.what());
                ers::fatal(issue);
            }
            ERS_INFO("LVL1 prescales read");

            fCTPConfig->setPrescaleSet( l1pss );

        }
    } else {
        ERS_INFO("Writing of LVL1 menu in HLT archiver is disabled");
    }
}

void
TrigConf::OnlTrigC2CWriter::configure(const daq::rc::TransitionCmd& cmd) {
    ERS_INFO("CONFIGURE(" << cmd.toString() << ") -> read OKS, subscribe to IS for run number, read hlt config from TriggerDB.");
    readOKS();
    ERS_INFO(conf());
    subscribeToIS();
    readTriggerDb();
}


void
TrigConf::OnlTrigC2CWriter::connect(const daq::rc::TransitionCmd& cmd) {
    if(conf().UseTriggerDB()) {
        ERS_INFO("CONNECT(" << cmd.toString() << ") -> check COOL connection, publish to IS");
    } else {
        ERS_INFO("CONNECT(" << cmd.toString() << ") -> nothing to do since we are not running from the TriggerDB");
        return;
    }
    // first thing to do is to check the connection to COOL. We quickly open and close.
    ERS_INFO("Checking COOL DB connection.");
    try {
        fConf2Cool->checkDbConnection(false);
    }
    catch(cool::Exception& e) {
        TrigConf::BadCoolDB issue(ERS_HERE, e.what());
        ers::fatal(issue);
        return;
    }

    publishToIS();
}


void
TrigConf::OnlTrigC2CWriter::prepareForRun(const daq::rc::TransitionCmd& cmd) {

    ERS_INFO("Entered prepareForRun");

    if(m_forceTriggerDBReadAtPrepareForRun) {
        ERS_INFO("m_forceTriggerDBReadAtPrepareForRun is true, rereading from triggerDB with this configuration.");
        ERS_INFO(conf());
        readTriggerDb();
    }



    readRunNumberFromIS();

    if(conf().UseTriggerDB()) {
        ERS_INFO("PREPAREFORRUN(" << cmd.toString() << ") -> write hlt config to COOL. Current run number " << conf().CurrentRunNumber());
    } else {
        ERS_INFO("PREPAREFORRUN(" << cmd.toString() << ") -> nothing to do since we are not running from the TriggerDB");
        return;
    }

    TrigConf::ThresholdConfig thrcfg;
    TrigConf::CaloInfo ci;

    std::string configurationSource = conf().DBConnection() + ",";
    if(getenv("AtlasVersion")) {
        configurationSource += std::string(getenv("AtlasVersion")) + std::string(",");
    } else {
        configurationSource += std::string("Unknown Version,");
    }
    if(getenv("AtlasProject")) {
        configurationSource += std::string(getenv("AtlasProject"));
    } else {
        configurationSource += std::string("Unknown Project");
    }

    // write the payload
    std::cout << "Configuration source: " << configurationSource << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << fCTPConfig << std::endl;
    if(fCTPConfig)
        std::cout << *fCTPConfig << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << fHLTFrame << std::endl;
    if(fHLTFrame)
        std::cout << *fHLTFrame << std::endl;
    std::cout << "=====================================================" << std::endl;


    if(fHLTFrame) {
        if(0!=fHLTFrame && 0!=fCTPConfig) {
            fConf2Cool->writeRunPayload(conf().CurrentRunNumber(),
                                        (unsigned int) conf().DBSMK(),
                                        (unsigned int) conf().DBHLTPSK(), // if 0 then no menu will be saved
                                        thrcfg,  // ignored
                                        *fCTPConfig,
                                        ci, // ignored
                                        *fHLTFrame,
                                        configurationSource);
        } else if (0!=fHLTFrame && 0==fCTPConfig) {
            fConf2Cool->writeHLTPayload( ValidityRange( conf().CurrentRunNumber() ), *fHLTFrame, configurationSource);
            ERS_INFO("Prescale Set ptr = " << (void*)fHLTFrame->getPrescaleSetCollection().prescaleSet());
            fConf2Cool->writeHltPrescalePayload( conf().CurrentRunNumber(), 1, *fHLTFrame->getPrescaleSetCollection().prescaleSet());
        }
    } else {
        ERS_INFO("Menu will not be written to COOL, since it was not read from the TriggerDB");
    }

    if(conf().MCKtoCOOL()) {
        if (conf().DBMCK() != 0){
            fConf2Cool->writeMCKPayload( conf().CurrentRunNumber(), conf().DBMCK(), conf().DBMCKrelease(), conf().DBMCKinfo());
            ERS_INFO("Done writing MCK " << conf().DBMCK() << " for release " << conf().DBMCKrelease() << " to COOL.");
        } else {
            ERS_INFO("MCK not written to COOL since it is 0");
        }
    }
}


void
TrigConf::OnlTrigC2CWriter::unconfigure(const daq::rc::TransitionCmd& cmd) {
    ERS_INFO("UNCONFIGURE(" << cmd.toString() << ") -> unsubscribe from IS");

    // unsubscribe from IS
    try {
        fInfoRec->unsubscribe(".*");
    }
    catch(ers::Issue & e) {
    }
    delete fInfoRec; fInfoRec = 0;
}



void
TrigConf::OnlTrigC2CWriter::writeHLTPrescaleSetToCool(unsigned int lb, unsigned int pskey, std::string& name) {

    ERS_INFO("Entered writeHLTPrescaleSetToCool with LB = " << lb << " and HLT psk " << pskey);

    if(!conf().UseTriggerDB()) return;

    if(!fHLTFrame) {
        ERS_INFO("Running without the HLT, can not write HLT prescales to COOL");
        return;
    }

    readRunNumberFromIS();

    std::string conn(""), user(""), pw("");
    conf().TriggerDbConnectionParams(conn,user,pw);
    std::unique_ptr< TrigConf::StorageMgr > sm(new TrigConf::StorageMgr(conn,user,pw));

    HLTPrescaleSet* hltPrescaleSet = new HLTPrescaleSet();

    hltPrescaleSet->setId(pskey);
    sm->hltPrescaleSetLoader().load( *hltPrescaleSet );
    name = hltPrescaleSet->name();

    ERS_INFO("writeHLTPrescaleSetToCool: HLT psk " << pskey << ", name " << name << " for Run " << conf().CurrentRunNumber() << " and LB " << lb);

    fHLTFrame->thePrescaleSetCollection().addPrescaleSet( lb, hltPrescaleSet );

    fConf2Cool->writeHltPrescalePayload( conf().CurrentRunNumber(), lb, *hltPrescaleSet );
}


void
TrigConf::OnlTrigC2CWriter::user(const daq::rc::UserCmd& usrCmd) {

    string commandName = usrCmd.commandName();
    vector<string> arguments = usrCmd.commandParameters();

    ERS_INFO("Received '" << commandName << "'  [" << usrCmd.toString() << "]");

    // this is a hack until the CTP controller sends the correct user command

    ERS_INFO("Received command " << commandName << " (" << usrCmd.toString() << ")");
    if(commandName == "USER") {
        ERS_INFO("User command " << usrCmd.toString() << " not formed correctly, need to apply parser hack.");

        vector<string> splitarg;
        boost::split(splitarg, arguments[0], boost::is_any_of(" "));

        arguments.clear();
        bool first(true);
        for(const string& s : splitarg) {
            if(first) {
                commandName = s;
                first = false;
            } else {
                arguments.push_back(s);
            }
        }
    }

    ERS_INFO("Command " << commandName);
    unsigned int i(0);
    for(const string& s :arguments) {
        ERS_INFO("   arg " << i++ << "  " << s);
    }


    if( commandName == "HLTPRESCALE2COOL" ) {

        if( arguments.size() != 2) {
            TrigConf::InvalidUserCommand issue(ERS_HERE,"'HLTPRESCALE2COOL' needs exactly 2 arguments: LB and HLT PS key");
            ers::warning(issue);
            return;
        }

        unsigned int lb    = boost::lexical_cast<unsigned int, std::string>(arguments[0]);
        unsigned int pskey = boost::lexical_cast<unsigned int, std::string>(arguments[1]);

        std::string pssname("");
        try {
            writeHLTPrescaleSetToCool(lb, pskey, pssname);
        }
        catch(std::exception & ex) {
            TrigConf::CoolWriting issue(ERS_HERE, ex.what());
            ers::error(issue);
        }

        // updating the internally held key
        conf().DBHLTPSK() = pskey;


        // publish to IS RunParams
        TrigConfHltPsKeyNamed hltps(*fIPCPartition,"RunParams.TrigConfHltPsKey");
        hltps.HltPrescaleKey = pskey;
        hltps.HltPrescaleComment = pssname;

        try {
            hltps.checkin();
            ERS_INFO("Published current hlt pskey " << pskey << " to RunParams.TrigConfHltPsKey");

        }
        catch (daq::is::Exception& ex) {
            TrigConf::ISPublishing issue(ERS_HERE, ex.what());
            ers::error(issue);
        }
        catch (std::exception & ex) {
            TrigConf::ISPublishing issue(ERS_HERE, ex.what());
            ers::error(issue);
        }
        catch (CORBA::SystemException& ex) {
            TrigConf::ISPublishing issue(ERS_HERE, ex._name());
            ers::error(issue);
        }
        catch (...) {
            TrigConf::ISPublishing issue(ERS_HERE,"unknown exception");
            ers::error(issue);
        }
    }
}


void
TrigConf::OnlTrigC2CWriter::stopArchiving(const daq::rc::TransitionCmd&) {
    ERS_INFO("stopArchiving");
    ERS_INFO("setting m_forceTriggerDBReadAtPrepareForRun to true");
    m_forceTriggerDBReadAtPrepareForRun = true;

}
