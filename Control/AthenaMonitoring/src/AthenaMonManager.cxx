/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: AthenaMonManager.cxx,v 1.36 2009-05-08 09:50:05 sschaetz Exp $
// **********************************************************************

#include "AthenaMonitoring/AthenaMonManager.h"

#include <cctype>

#include "TDirectory.h"
#include "TH1.h"
#include "TObject.h"
#include "TROOT.h"
#include "LWHists/LWHist.h"
#include "LWHists/LWHistControls.h"
#include "LWHists/LWHistStats.h"
#include "LWHistAthMonWrapper.h"
#include "AthMonBench.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "SGAudCore/ISGAudSvc.h"

#include <limits.h>
#include <vector>

class AthenaMonManager::Imp {
public:
    Imp(AthenaMonManager*man,const std::string& name) :  m_theManager(man),
        m_sgAudSvc(0),
        m_isPrimaryManager(false),
        m_everyNevents(-1),
        m_eventCounter(-1),
        m_fileKeyProp(name),
        m_manualDataTypeSetupProp(false),
        m_dataTypeProp("userDefined"),
        m_environmentProp("user"),
        m_LBsLowStatProp(10),
        m_LBsMedStatProp(30),
        m_LBsHigStatProp(90),
        m_manualRunLBProp(false),
        m_runProp(0),
        m_lumiBlockProp(0),
        m_nMonGroupCopies(0),
        m_nActiveLWHists(0),
	m_forkedProcess(false),
	m_lastPID(0),
	m_doResourceMon(false)
 {}
  
    ~Imp()
    {
        std::map<IMonitorToolBase*,ToolBench*>::iterator it, itE;

        for (it=m_toolbenchmarks.begin(),itE=m_toolbenchmarks.end(); it!=itE; ++it) {
            delete it->second;
        }

        m_toolbenchmarks.clear();
    }

    //These were protected before we moved to a private implementation:
    typedef std::map<std::string,TObject*>  ObjMap_t;
    ObjMap_t     m_objMap;

    typedef std::map<std::string,LWHist*>  ObjMapLW_t;
    ObjMapLW_t   m_objMapLW;

    AthenaMonManager * m_theManager;
    ISGAudSvc * m_sgAudSvc;

    bool         m_isPrimaryManager;

    long         m_everyNevents;
    long         m_eventCounter;

    std::string  m_fileKeyProp;

    bool         m_manualDataTypeSetupProp;
    std::string  m_dataTypeProp;
    std::string  m_environmentProp;
    unsigned int m_LBsLowStatProp, m_LBsMedStatProp, m_LBsHigStatProp;

    bool         m_manualRunLBProp;

    unsigned int m_runProp;
    unsigned int m_lumiBlockProp;

    unsigned m_nMonGroupCopies;
    long m_nActiveLWHists;

    bool m_forkedProcess;
    pid_t m_lastPID;


    //NB: The LW hist leak checker is now also looking for
    //inappropriate usage of MonGroup copy constructors (temporary
    //until we outlaw copy/assignment of MonGroups):
    class LWHistLeakChecker {
        Imp * m_d;
        long m_initVal;
        unsigned m_mgcopies_initval;
    public:
        LWHistLeakChecker(Imp*d) : m_d(d), m_initVal(LWHistStats::nActiveLWHists()),
            m_mgcopies_initval(ManagedMonitorToolBase::MonGroup::ncopies()) {}
        ~LWHistLeakChecker()
        {   m_d->m_nActiveLWHists += LWHistStats::nActiveLWHists()-m_initVal;
            m_d->m_nMonGroupCopies += ManagedMonitorToolBase::MonGroup::ncopies()-m_mgcopies_initval;
        }
    };

    template<class T> void toolAudStart(const T&tool) {
        if (m_sgAudSvc) m_sgAudSvc->setFakeCurrentAlg(m_theManager->name()+"_MT_"+tool->name());
    }
    void toolAudEnd() {
        if (m_sgAudSvc) m_sgAudSvc->clearFakeCurrentAlg();
    }

    bool m_doResourceMon;

    class ToolBench {
    public:
        ToolBench(IMonitorToolBase * t) :  m_theTool(t) {}
        const IMonitorToolBase * m_theTool;
        AthMonBench m_bench_alginit_retrieve;
        AthMonBench m_bench_algexec_fillHists;//NB: includes call to preselector
        AthMonBench m_bench_algexec_checkHists;
        AthMonBench m_bench_algexec_bookHists;
        //NB: So far we don't check the call to runStat()
        AthMonBench m_bench_algfin_finalHists;
        AthMonBench m_bench_algfin_checkHists;
        AthMonBench m_bench_algfin_convertLWHists;
        void report(AthenaMonManager*a) {
            MSG::Level l(AthMonBench::s_resourceMonThreshold);
            a->msg(l)<<"Init ResourceSummary ["<<m_theTool->name()<<"] --> load/create/initialize : "<<m_bench_alginit_retrieve<<endmsg;
            m_bench_algexec_checkHists.setUnitCount();
            m_bench_algexec_bookHists.setUnitCount();
            a->msg(l)<<"Exec ResourceSummary ["<<m_theTool->name()<<"] --> book : "<<m_bench_algexec_bookHists<<endmsg;
            a->msg(l)<<"Exec ResourceSummary ["<<m_theTool->name()<<"] --> check : "<<m_bench_algexec_checkHists<<endmsg;
            a->msg(l)<<"Exec ResourceSummary ["<<m_theTool->name()<<"] --> fill : "<<m_bench_algexec_fillHists<<endmsg;
            a->msg(l)<<"Fin ResourceSummary ["<<m_theTool->name()<<"] --> finalHists : "<<m_bench_algfin_finalHists<<endmsg;
            a->msg(l)<<"Fin ResourceSummary ["<<m_theTool->name()<<"] --> checkHists(fromFinalize==true) : "<<m_bench_algfin_checkHists<<endmsg;
            a->msg(l)<<"Fin ResourceSummary ["<<m_theTool->name()<<"] --> final LWHist conversion+writeout : "<<m_bench_algfin_convertLWHists<<endmsg;
        }

    };
    std::map<IMonitorToolBase*,ToolBench*> m_toolbenchmarks;
    ToolBench* getToolBench(IMonitorToolBase* t) {
        std::map<IMonitorToolBase*,ToolBench*>::iterator it = m_toolbenchmarks.find(t);
        if (it==m_toolbenchmarks.end()) {
            ToolBench*tb =new ToolBench(t);
            m_toolbenchmarks[t] = tb;
            return tb;
        }
        return it->second;
    }

    // The data below should be truly global for a job, and inheriting classes should
    // not have the option of modifying them.
    // The data are set from the corresponding m_* properties of the first AthenaMonManager
    // in the Athena algorithm chain (this manager sets s_staticDataAreInit to true).

    static ISvcLocator*    s_svcLocator;

    static bool            s_staticDataAreInit;

    static std::string     s_dataTypeStr;
    static std::string     s_environmentStr;
    static DataType_t      s_dataType;
    static Environment_t   s_environment;

    static unsigned int s_lumiBlock;
    static unsigned int s_run;
    static unsigned int s_fill;
    static unsigned int s_LBsLowStat;
    static unsigned int s_LBsMedStat;
    static unsigned int s_LBsHigStat;
};

ISvcLocator* AthenaMonManager::Imp::s_svcLocator(0);

bool AthenaMonManager::Imp::s_staticDataAreInit(false);

std::string                      AthenaMonManager::Imp::s_dataTypeStr("userDefined");
std::string                      AthenaMonManager::Imp::s_environmentStr("user");
AthenaMonManager::DataType_t     AthenaMonManager::Imp::s_dataType(userDefined);
AthenaMonManager::Environment_t  AthenaMonManager::Imp::s_environment(user);

unsigned int  AthenaMonManager::Imp::s_lumiBlock(0);
unsigned int  AthenaMonManager::Imp::s_run(0);
unsigned int  AthenaMonManager::Imp::s_fill(0);
unsigned int  AthenaMonManager::Imp::s_LBsLowStat(0);
unsigned int  AthenaMonManager::Imp::s_LBsMedStat(0);
unsigned int  AthenaMonManager::Imp::s_LBsHigStat(0);
namespace {

std::string strToLower( const std::string& str );

} // unnamed namespace

// *********************************************************************
// Public Methods
// *********************************************************************

AthenaMonManager::
AthenaMonManager( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), m_THistSvc(0), m_d(new Imp(this, name))
{
    declareProperty( "AthenaMonTools", m_monTools );
    declareProperty( "CheckEveryNoEvents", m_d->m_everyNevents );
    declareProperty( "FileKey", m_d->m_fileKeyProp );
    declareProperty( "ManualDataTypeSetup", m_d->m_manualDataTypeSetupProp );
    declareProperty( "DataType", m_d->m_dataTypeProp );
    declareProperty( "Environment", m_d->m_environmentProp );
    declareProperty( "LBsInLowStatInterval", m_d->m_LBsLowStatProp );
    declareProperty( "LBsInMediumStatInterval", m_d->m_LBsMedStatProp );
    declareProperty( "LBsInHighStatInterval", m_d->m_LBsHigStatProp );
    declareProperty( "ManualRunLBSetup", m_d->m_manualRunLBProp );
    declareProperty( "Run", m_d->m_runProp );
    declareProperty( "LumiBlock", m_d->m_lumiBlockProp );

    if( Imp::s_svcLocator==0 )
        Imp::s_svcLocator = pSvcLocator;
}


AthenaMonManager::
~AthenaMonManager()
{
    if (m_d->m_nActiveLWHists)
        msg(MSG::WARNING) << m_d->m_nActiveLWHists<< " LW histograms were created but never deleted" << endmsg;
    /*
    if (m_d->m_nMonGroupCopies)
      msg(MSG::WARNING) << "Usage of MonGroup copy constructor or assignment operator detected in associated tools "
    	      << m_d->m_nMonGroupCopies<<" time(s). This usually happens when tools pass MonGroup's around by"
    	      <<" value. This behaviour will be forbidden at the compilation stage soon, so please fix your packages!" << endmsg;
    */
    delete m_d;

    // If it is known that ManagedMonitorToolBase objects are deleted _after_ this object,
    // then one should call setMonManager(0) on each of them.
}


AthenaMonManager::Environment_t
AthenaMonManager::
envStringToEnum( const std::string& str )
{
    std::string lcstr( strToLower(str) );

    if( lcstr == "user" )
        return user;
    else if( lcstr == "nooutput" )
        return noOutput;
    else if( lcstr == "online" )
        return online;
    else if( lcstr == "tier0" )
        return tier0;
    else if( lcstr == "tier0raw" )
        return tier0Raw;
    else if( lcstr == "tier0esd" )
        return tier0ESD;
    else if( lcstr == "aod" )
        return AOD;
    else if( lcstr == "altprod" )
        return altprod;

    if( Imp::s_svcLocator!=0 ) {
        IMessageSvc* ms(0);
        StatusCode sc = Imp::s_svcLocator->service( "MessageSvc", ms, true );
        if( sc.isSuccess() ) {
            MsgStream log( ms, "AthenaMonManager::envStringToEnum()" );
            log << MSG::WARNING << "Unknown AthenaMonManager::Environment_t \""
                << str << "\", returning \"user\"" << endmsg;
        }
    }

    return user;
}


AthenaMonManager::DataType_t
AthenaMonManager::
dataTypeStringToEnum( const std::string& str )
{
    std::string lcstr( strToLower(str) );

    if( lcstr == "userdefined" )
        return userDefined;
    else if( lcstr == "montecarlo" )
        return monteCarlo;
    else if( lcstr == "collisions" )
        return collisions;
    else if( lcstr == "cosmics" )
        return cosmics;
    else if( lcstr == "heavyioncollisions" )
        return heavyIonCollisions;

    if( Imp::s_svcLocator!=0 ) {
        IMessageSvc* ms(0);
        StatusCode sc = Imp::s_svcLocator->service( "MessageSvc", ms, true );
        if( sc.isSuccess() ) {
            MsgStream log( ms, "AthenaMonManager::dataTypeStringToEnum()" );
            log << MSG::WARNING << "Unknown AthenaMonManager::DataType_t \""
                << str << "\", returning \"userDefined\"" << endmsg;
        }
    }

    return userDefined;
}


AthenaMonManager::Environment_t
AthenaMonManager::
environment()
{
    return Imp::s_environment;
}

unsigned int AthenaMonManager::getLBsLowStat() {
    return Imp::s_LBsLowStat;
}
unsigned int AthenaMonManager::getLBsMedStat() {
    return Imp::s_LBsMedStat;
}
unsigned int AthenaMonManager::getLBsHigStat() {
    return Imp::s_LBsHigStat;
}

AthenaMonManager::DataType_t
AthenaMonManager::
dataType()
{
    return Imp::s_dataType;
}

bool 
AthenaMonManager::
forkedProcess() {
  return m_d->m_forkedProcess;
}


unsigned int
AthenaMonManager::
lumiBlockNumber()
{
    return Imp::s_lumiBlock;
}


unsigned int
AthenaMonManager::
runNumber()
{
    return Imp::s_run;
}


unsigned int
AthenaMonManager::
fillNumber()
{
    return Imp::s_fill;
}


StatusCode
AthenaMonManager::
initialize()
{
    Imp::LWHistLeakChecker lc(m_d);

    //typedef std::vector<IMonitorToolBase*> MonList_t;
    //typedef MonList_t::iterator            MonIter_t;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AthenaMonManager::initialize():" << endmsg;

    if (Imp::s_svcLocator->service("SGAudSvc", m_d->m_sgAudSvc, false/*do not create*/).isFailure())
        m_d->m_sgAudSvc=0;


    m_d->m_doResourceMon = msgLvl(AthMonBench::s_resourceMonThreshold);

    StatusCode sc;
    sc.setChecked();

    sc = service( "THistSvc", m_THistSvc, true );
    if( !sc.isSuccess() ) {
        msg(MSG::ERROR) << "!! Unable to locate the THistSvc service !!" << endmsg;
        return sc;
    }

    if( !Imp::s_staticDataAreInit ) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Initializing static data" << endmsg;

        if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "      * \"ManualDataTypeSetup\" = " << m_d->m_manualDataTypeSetupProp << endmsg;
        }

#if 0  // The two branches of the conditional are identical.
        if( m_d->m_manualDataTypeSetupProp ) {
            Imp::s_dataTypeStr = m_d->m_dataTypeProp;
            Imp::s_dataType = dataTypeStringToEnum( m_d->m_dataTypeProp );
        }
        else
#endif
        {
            Imp::s_dataTypeStr = m_d->m_dataTypeProp;
            Imp::s_dataType = dataTypeStringToEnum( m_d->m_dataTypeProp );
        }

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "      * \"DataType\" = " << Imp::s_dataType << endmsg;

        Imp::s_environmentStr = m_d->m_environmentProp;
        Imp::s_environment = envStringToEnum( m_d->m_environmentProp );

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "      * \"Environment\" = " << Imp::s_environment << endmsg;

        Imp::s_LBsLowStat = m_d->m_LBsLowStatProp;
        Imp::s_LBsMedStat = m_d->m_LBsMedStatProp;
        Imp::s_LBsHigStat = m_d->m_LBsHigStatProp;
        if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "      * \"LBsInLowStatInterval\" = " << Imp::s_LBsLowStat << endmsg;
            msg(MSG::DEBUG) << "      * \"LBsInMediumStatInterval\" = " << Imp::s_LBsMedStat << endmsg;
            msg(MSG::DEBUG) << "      * \"LBsInHighStatInterval\" = " << Imp::s_LBsHigStat << endmsg;
        }

        m_d->m_isPrimaryManager = true;
        Imp::s_staticDataAreInit = true;

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "      * \"ManualRunLBSetup\" = " << m_d->m_manualRunLBProp << endmsg;

        if( m_d->m_manualRunLBProp ) {
            Imp::s_run = m_d->m_runProp;
            Imp::s_lumiBlock = m_d->m_lumiBlockProp;
            if (msgLvl(MSG::DEBUG))
                msg(MSG::DEBUG) << "          --> using run = " << Imp::s_run << ", lumiBlock = " << Imp::s_lumiBlock << endmsg;
        } else {
            if (msgLvl(MSG::DEBUG))
                msg(MSG::DEBUG) << "          --> using run = " << Imp::s_run << ", lumiBlock = " << Imp::s_lumiBlock << endmsg;
        }
    }

    // Ensure consistency among all managers
    m_d->m_dataTypeProp    = Imp::s_dataTypeStr;
    m_d->m_environmentProp = Imp::s_environmentStr;

    // Provide a virtual Algorithm with a subset of the properties
    IJobOptionsSvc* joSvc = 0;
    sc = service( "JobOptionsSvc", joSvc );
    if( !sc.isSuccess() ) {
        msg(MSG::ERROR) << "!! Unable to locate the JobOptionsSvc service !!" << endmsg;
        return sc;
    }
    ATH_MSG_DEBUG("  --> Found service \"JobOptionsSvc\"");

    std::string client = name() + std::string("Properties");
    ATH_MSG_DEBUG("  --> Adding properties under name \"" << client << "\"");
    StatusCode sc1 = joSvc->addPropertyToCatalogue( client, StringProperty("FileKey",m_d->m_fileKeyProp) );
    StatusCode sc2 = joSvc->addPropertyToCatalogue( client, StringProperty("DataType",m_d->m_dataTypeProp) );
    StatusCode sc3 = joSvc->addPropertyToCatalogue( client, StringProperty("Environment",m_d->m_environmentProp) );
    joSvc->release();
    if( !(sc1.isSuccess() && sc2.isSuccess() && sc3.isSuccess()) ) {
        msg(MSG::ERROR) << "!! Unable to add properties to JobOptionsSvc service !!" << endmsg;
        return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("  --> Added Properties");

    //Determine (globally for now), whether or not LW histograms use a
    //ROOT backend (necessary for online monitoring, and perhaps for
    //debugging):
    bool root_backend_for_lwhists = (environment()==AthenaMonManager::online);
    if( root_backend_for_lwhists != LWHistControls::hasROOTBackend() )
        LWHistControls::setROOTBackend(root_backend_for_lwhists);

//    if( m_monTools.size() > 0 ) {
//       sc = m_monTools.retrieve();
//       if( !sc.isSuccess() ) {
//          msg(MSG::ERROR) << "!! Unable to retrieve monitoring tool " << m_monTools << endmsg;
//          return sc;
//       }
//       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Retrieved AthenaMonTools" << endmsg;
//    }

    m_d->m_eventCounter = m_d->m_everyNevents;

    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
        ToolHandle<IMonitorToolBase>& tool = *i;
        m_d->toolAudStart(tool);
        AthMonBench bench_tmp;
        if (m_d->m_doResourceMon)
            bench_tmp.startMeasurement();
        StatusCode sc_toolret = tool.retrieve();
        if (m_d->m_doResourceMon)
            bench_tmp.finishMeasurement();
        m_d->toolAudEnd();
        if( sc_toolret.isFailure() ) {
            msg(MSG::ERROR) << "Failed to retrieve monitoring tool " << tool << endmsg;
            return StatusCode::FAILURE;
        }
        else {
            msg(MSG::INFO) << "Retrieved tool " << tool << endmsg;
        }
        IMonitorToolBase* mon = tool.operator->();
        ManagedMonitorToolBase* managed = dynamic_cast<ManagedMonitorToolBase*>( mon );
        if( managed != 0 ) {
            managed->setMonManager( this );
        }
        if (m_d->m_doResourceMon) {
            Imp::ToolBench* tb = m_d->getToolBench(mon);
            tb->m_bench_alginit_retrieve = bench_tmp;
        }

    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Exiting successfully" << endmsg;

    return StatusCode::SUCCESS;
}


StatusCode
AthenaMonManager::
execute()
{
    m_d->m_forkedProcess=false;
    pid_t currPID=getpid();
    //m_lastPID 0 means the execute method was not called yet. 
    if (m_d->m_lastPID!=0 && m_d->m_lastPID!=currPID) {
      m_d->m_forkedProcess=true;
      ATH_MSG_INFO("Forked event discovered!");
    }
    m_d->m_lastPID=currPID;

    Imp::LWHistLeakChecker lc(m_d);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AthenaMonManager::execute():" << endmsg;

    StatusCode sc;
    sc.setChecked();

    if( !m_d->m_manualRunLBProp ) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Calling setEventInfo()" << endmsg;
        setEventInfo();
    }

    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
        ToolHandle<IMonitorToolBase>& tool = *i;
        m_d->toolAudStart(tool);
        AthMonBench bench_tmp;
        if (m_d->m_doResourceMon)
            bench_tmp.startMeasurement();
        if( tool->preSelector() ) {
            //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> The tool is preSelector" << endmsg;

            // Uncomment to test luminosity tools
            // I don't know where exactly it must be placed exactly
            // Yuriy - start
            /*
            IMonitorToolBase* mon = tool.operator->();
            ManagedMonitorToolBase* managed = dynamic_cast<ManagedMonitorToolBase*>( mon );

            float avgmu = managed->lbAverageInteractionsPerCrossing();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Average mu: " << avgmu << endmsg;

            float instmu = managed->lbInteractionsPerCrossing();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Instantaneous mu: " << instmu << endmsg;

            float avglumi = managed->lbAverageLuminosity();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Average lumi: " << avglumi << endmsg;

            float instlumi = managed->lbLuminosityPerBCID();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Instantaneous lumi: " << instlumi << endmsg;

            double duration = managed->lbDuration();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Lumiblock duration: " << duration << endmsg;

            double live = managed->livefractionPerBCID();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, lumiinst livefraction: " << live << endmsg;

            float lumilive = managed->lbAverageLivefraction();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, lumiavg livefraction: " << lumilive << endmsg;

            double lumiweight = managed->lbLumiWeight();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Test, Lumiblock weight: " << lumiweight << endmsg;
            */
            // Yuriy - end

            sc = tool->fillHists();

            if( !sc.isSuccess() ) {
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::fillHists() unsuccessful" << endmsg;
            }
        }


        if (m_d->m_doResourceMon) {
            bench_tmp.finishMeasurement();
            Imp::ToolBench* tb =  m_d->getToolBench(tool.operator->());
            tb->m_bench_algexec_fillHists += bench_tmp;//fixme: count events and subdivide
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> m_doResourceMon is True" << endmsg;
        }
        m_d->toolAudEnd();
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Done calling IMonitorToolBase::fillHists()" << endmsg;

    if( m_d->m_eventCounter > 0 ) {
        m_d->m_eventCounter--;
    }
    else if( m_d->m_eventCounter == 0 ) {
        for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
            ToolHandle<IMonitorToolBase>& tool = *i;
            m_d->toolAudStart(tool);
            AthMonBench bench_tmp;

            if (m_d->m_doResourceMon)
                bench_tmp.startMeasurement();
            sc = tool->checkHists(false);
            if (m_d->m_doResourceMon) {
                bench_tmp.finishMeasurement();
                Imp::ToolBench* tb =  m_d->getToolBench(tool.operator->());
                tb->m_bench_algexec_checkHists += bench_tmp;//fixme: count events and subdivide
            }
            if( !sc.isSuccess() ) {
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::checkHists() unsuccessful" << endmsg;
            }
            m_d->toolAudEnd();
        }
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Done calling IMonitorToolBase::checkHists()" << endmsg;

        m_d->m_eventCounter = m_d->m_everyNevents;
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  --> Exiting successfully" << endmsg;

    return StatusCode::SUCCESS;
}


StatusCode
AthenaMonManager::
finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode
AthenaMonManager::
stop()
{
    Imp::LWHistLeakChecker lc(m_d);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AthenaMonManager::finalize():" << endmsg;

    StatusCode sc;
    sc.setChecked();

    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
        ToolHandle<IMonitorToolBase>& tool = *i;
        m_d->toolAudStart(tool);
        Imp::ToolBench* tb =  m_d->m_doResourceMon ? m_d->getToolBench(tool.operator->()) : 0;
        if (tb)
            tb->m_bench_algfin_finalHists.startMeasurement();
        sc = tool->finalHists();
        if (tb)
            tb->m_bench_algfin_finalHists.finishMeasurement();
        if( !sc.isSuccess() ) {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::finalHists() unsuccessful" << endmsg;
        }
        if (tb)
            tb->m_bench_algfin_checkHists.startMeasurement();
        sc = tool->checkHists(true);
        if (tb)
            tb->m_bench_algfin_checkHists.finishMeasurement();
        if( !sc.isSuccess() ) {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::checkHists() unsuccessful" << endmsg;
        }
        if (tb)
            tb->m_bench_algfin_convertLWHists.startMeasurement();
        sc = tool->convertLWHists();
        if (tb)
            tb->m_bench_algfin_convertLWHists.finishMeasurement();
        if( !sc.isSuccess() ) {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::convertLWHists() unsuccessful" << endmsg;
        }
        if (tb)
            tb->report(this);
        m_d->toolAudEnd();
    }
    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "  --> Done calling IMonitorToolBase::finalHists() and IMonitorToolBase::checkHists()" << endmsg;
        msg(MSG::DEBUG) << "  --> Exiting successfully" << endmsg;
    }
    return StatusCode::SUCCESS;
}


StatusCode
AthenaMonManager::
beginRun()
{
    Imp::LWHistLeakChecker lc(m_d);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AthenaMonManager::beginRun():" << endmsg;

    StatusCode sc;
    sc.setChecked();

    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
        ToolHandle<IMonitorToolBase>& tool = *i;
        m_d->toolAudStart(tool);
        AthMonBench bench_tmp;
        if (m_d->m_doResourceMon)
            bench_tmp.startMeasurement();
        sc = tool->bookHists();
        if (m_d->m_doResourceMon) {
            bench_tmp.finishMeasurement();
            Imp::ToolBench* tb =  m_d->getToolBench(tool.operator->());
            tb->m_bench_algexec_bookHists += bench_tmp;
        }
        if( !sc.isSuccess() ) {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::bookHists() unsuccessful" << endmsg;
        }
        m_d->toolAudEnd();
    }
    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "  --> Done calling IMonitorToolBase::bookHists()" << endmsg;
        msg(MSG::DEBUG) << "  --> Exiting successfully" << endmsg;
    }

    return StatusCode::SUCCESS;
}


StatusCode
AthenaMonManager::
endRun()
{
    Imp::LWHistLeakChecker lc(m_d);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AthenaMonManager::endRun():" << endmsg;

    StatusCode sc;
    sc.setChecked();

    ToolHandleArray<IMonitorToolBase>::iterator monToolsEnd = m_monTools.end();
    for( ToolHandleArray<IMonitorToolBase>::iterator i = m_monTools.begin(); i != monToolsEnd; ++i ) {
        ToolHandle<IMonitorToolBase>& tool = *i;
        m_d->toolAudStart(tool);
        sc = tool->runStat();
        if( !sc.isSuccess() ) {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "IMonitorToolBase::runStat() unsuccessful" << endmsg;
        }
        m_d->toolAudEnd();
    }
    if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "  --> Done calling IMonitorToolBase::runStat()" << endmsg;
        msg(MSG::DEBUG) << "  --> Exiting successfully" << endmsg;
    }
    return StatusCode::SUCCESS;
}


std::string
AthenaMonManager::
fileKey() const
{
    return m_d->m_fileKeyProp;
}


void
AthenaMonManager::
passOwnership( TObject* h, const std::string& key )
{
    Imp::ObjMap_t::value_type valToInsert( key, h );
    m_d->m_objMap.insert( valToInsert );
}


void
AthenaMonManager::
passOwnership( LWHist* h, const std::string& key )
{
    Imp::ObjMapLW_t::value_type valToInsert( key, h );
    m_d->m_objMapLW.insert( valToInsert );
}



LWHist *
AthenaMonManager::
ownedLWHistOfKey(const std::string& key) const
{
    Imp::ObjMapLW_t::iterator iLW = m_d->m_objMapLW.find( key );
    return iLW == m_d->m_objMapLW.end() ? 0 : iLW->second;
}


void
AthenaMonManager::
writeAndDeleteLWHist( LWHist*lwh  )
{
    writeAndDeleteLWHist(LWHistAthMonWrapper::key(lwh),LWHistAthMonWrapper::streamName(lwh));
}


LWHist *
AthenaMonManager::
writeAndDeleteLWHist( const std::string& key, const std::string& streamName )
{
//     std::cout<<"writeAndDeleteLWHist... maybe"<<std::endl;
    //If is owned LWHist we write it in a root file and deletes it + returns it's (now deallocated) address.

    Imp::ObjMapLW_t::iterator iLW = m_d->m_objMapLW.find( key );
    if( iLW != m_d->m_objMapLW.end() )
    {
        //Fixme: add private helper method actualWriteAndDelete(TH1*), to use by both...
        LWHist * lwhist =iLW->second;
        TH1 * h = lwhist->getROOTHistBase();
        if( h != 0 )
        {
            //Get correct dir by doing a quick reg/dereg:
            bool ok(false);
            if (lwhist->usingROOTBackend())
            {
                //Already registered
                ok = true;
            }
            else
            {
                //Delayed registration:
                if (m_THistSvc->regHist( streamName, h ).isSuccess())
                    //    if (m_THistSvc->deReg(  h ).isSuccess())
                    ok = true;
            }

            if (ok)
            {
                bool doRecursiveReferenceDelete = gROOT->MustClean();
                gROOT->SetMustClean(false);
                TDirectory* dir = h->GetDirectory();
                TDirectory* g = gDirectory;
                if(dir)
                    dir->cd();
                h->Write();
                g->cd();

                StatusCode sc = m_THistSvc->deReg( h );
                if( !sc.isSuccess() )
                {
                    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "AthenaMonManager::WriteAndDeleteHist(): Failure to deReg( TObject* )" << endmsg;
                }

                m_d->m_objMapLW.erase( iLW );
                lwhist->setOwnsROOTHisto(true);//Since we deregistered
                LWHistAthMonWrapper::deleteLWHist(lwhist);
                lwhist=0;
                gROOT->SetMustClean(doRecursiveReferenceDelete);//Should be after the deleteLWHist call
            }
            return lwhist;
        }
    }
    return 0;
}

LWHist *
AthenaMonManager::
writeAndResetLWHist( const std::string& key, const std::string& streamName )
{
//     std::cout<<"writeAndDeleteLWHist... maybe"<<std::endl;
    //If is owned LWHist we write it in a root file and deletes it + returns it's (now deallocated) address.

    Imp::ObjMapLW_t::iterator iLW = m_d->m_objMapLW.find( key );
    if( iLW != m_d->m_objMapLW.end() )
    {
        //Fixme: add private helper method actualWriteAndDelete(TH1*), to use by both...
        LWHist * lwhist =iLW->second;
        TH1 * h = lwhist->getROOTHistBase();
        if( h != 0 )
        {
            //Get correct dir by doing a quick reg/dereg:
            bool ok(false);
            if (lwhist->usingROOTBackend())
            {
                //Already registered
                ok = true;
            }
            else
            {
                //Delayed registration:
                if (m_THistSvc->regHist( streamName, h ).isSuccess())
                    //    if (m_THistSvc->deReg(  h ).isSuccess())
                    ok = true;
            }

            if (ok)
            {
                bool doRecursiveReferenceDelete = gROOT->MustClean();
                gROOT->SetMustClean(false);
                TDirectory* dir = h->GetDirectory();
                TDirectory* g = gDirectory;
                if(dir)
                    dir->cd();
                h->Write();
                g->cd();

                StatusCode sc = m_THistSvc->deReg( h );
                if( !sc.isSuccess() )
                {
                    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "AthenaMonManager::WriteAndDeleteHist(): Failure to deReg( TObject* )" << endmsg;
                }

                m_d->m_objMapLW.erase( iLW );
                lwhist->setOwnsROOTHisto(true);//Since we deregistered

                //LWHistAthMonWrapper::deleteLWHist(lwhist);
                // Yuriy: commented out this line to make the code properly place  histograms
                // in the offline environament; It has been a bug after I fixed online environment
                // with a new approach; Basically inherited from writeAndDeletLWHist(...)
                //LWHistAthMonWrapper::removeCustomData(lwhist);

                lwhist->Reset();

                lwhist=0;
                gROOT->SetMustClean(doRecursiveReferenceDelete);//Should be after the deleteLWHist call
            }
            return lwhist;
        }
    }
    return 0;
}


void
AthenaMonManager::
writeAndDelete( const std::string& key )
{
    // If is owned TObject, we deregister from THistSvc, write it in a
    // root file, and delete it.

    Imp::ObjMap_t::iterator i = m_d->m_objMap.find( key );
    if( i != m_d->m_objMap.end() ) {
        TObject* o = i->second;
        TH1* h = dynamic_cast<TH1*>( o );
        if( h != 0 ) {
            // By default, ROOT does its own bookkeeping of references to
            // histograms, and this leads to a significant amount of time
            // spent in the TObject d'tor when there are >= O(1000) TDirectories.
            // So, turn this bookkeeping off when deleting these histograms.
            bool doRecursiveReferenceDelete = gROOT->MustClean();
            gROOT->SetMustClean(false);
            TDirectory* dir = h->GetDirectory();
            TDirectory* g = gDirectory;
            if( dir )
                dir->cd();
            h->Write();
            g->cd();

            StatusCode sc = m_THistSvc->deReg( h );
            if( !sc.isSuccess() ) {
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "AthenaMonManager::WriteAndDeleteHist(): Failure to deReg( TObject* )" << endmsg;
            }
            delete o;
            gROOT->SetMustClean(doRecursiveReferenceDelete);
            m_d->m_objMap.erase( key );
        }
    }
}

// *********************************************************************
// Protected Methods
// *********************************************************************

void
AthenaMonManager::
setEventInfo()
{
    if( m_d->m_isPrimaryManager ) {
        const DataHandle<EventInfo> evt;
        const DataHandle<EventInfo> evtEnd;
        StatusCode sc = evtStore()->retrieve( evt, evtEnd );

        if( !sc.isSuccess() ) {
            msg(MSG::ERROR) << "!! Unable to retrieve Event from StoreGate !!" << endmsg;
            return;
        }

        Imp::s_run = evt->event_ID()->run_number();
        Imp::s_lumiBlock = evt->event_ID()->lumi_block();

        if (msgLvl(MSG::DEBUG))
            msg(MSG::DEBUG) << "          --> setEventInfo: run = " << Imp::s_run << ", lumiBlock = " << Imp::s_lumiBlock << endmsg;

    }
}


// *********************************************************************
// Private Methods
// *********************************************************************
namespace {

std::string strToLower( const std::string& str )
{
    std::string lstr(str);
    std::string::const_iterator  from   = str.begin();
    std::string::const_iterator  strend = str.end();
    std::string::iterator        to     = lstr.begin();
    while( from != strend ) {
        *to++ = tolower(*from++);
    }
    return lstr;
}

} // unnamed namespace
