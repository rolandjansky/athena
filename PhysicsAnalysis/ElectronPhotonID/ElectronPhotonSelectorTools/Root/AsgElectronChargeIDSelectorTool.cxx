/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/**
 @class AsgElectronChargeIDSelectorTool
 @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.
 
 @author Karsten Koeneke
 @date   October 2012
 
 09-APR-2014, convert to ASGTool (Jovan Mitrevski)
 22-AUG-2016, copied from AsgElectronLikelihoodTool (Kazuya Mochizuki)
 
 */

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
//#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
//#include "EGSelectorConfigurationMapping.h"

// STL includes
#include <string>
#include <cstdint>
#include <cmath>

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"
#include "TEnv.h"
#include "TFile.h"
#include "TObjString.h"
#include "TObjArray.h"


#include "PathResolver/PathResolver.h"


//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronChargeIDSelectorTool::AsgElectronChargeIDSelectorTool(std::string myname) :
AsgTool(myname) //,m_cutOnBDT(0)//,m_configFile("")//,m_rootTool(0)
{
    // Declare the needed properties
    declareProperty("WorkingPoint",m_WorkingPoint="","The Working Point");
    //declareProperty("ConfigFile",m_configFile="","The config file to use");
    declareProperty("usePVContainer", m_usePVCont=true, "Whether to use the PV container");
    declareProperty("nPVdefault", m_nPVdefault = 0, "The default number of PVs if not counted");
    declareProperty("primaryVertexContainer", m_primVtxContName="PrimaryVertices", "The primary vertex container name" );
    
    declareProperty("TrainingFile",  m_trainingFile="", "The input ROOT file name holding training" );
    declareProperty("CutOnBDT",m_cutOnBDT=0,"Cut on BDT discriminant");
    m_pid_name=myname.data();
}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronChargeIDSelectorTool::~AsgElectronChargeIDSelectorTool()
{
    if(finalize().isFailure()){
        ATH_MSG_ERROR ( "Failure in AsgElectronChargeIDSelectorTool finalize()");
    }
    //delete m_rootTool;
    for (auto bdt: m_v_bdts) if (bdt) delete bdt;
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronChargeIDSelectorTool::initialize()
{
    m_pid_name.ToLower(); //KM: List of 97% OPs with different PIDs below
    std::string op_name="loose";
    bool op_isUserSpecified=false;
    if (m_cutOnBDT==0) {  //when cutOnBDT is unmodified, adjust it to the 97% OP in each PID menu
        if      (m_pid_name.Contains("tight") ) op_name="tight" , m_cutOnBDT=-0.109249;//Tight  (with data): -0.109249
        else if (m_pid_name.Contains("medium")) op_name="medium", m_cutOnBDT=-0.257081;//Medium (with data): -0.257081
        else                                                      m_cutOnBDT=-0.337671;//Loose  (with data): -0.337671
    }
    else op_isUserSpecified=true;
    m_pid_name="loose";//Now only one training is provided, using loose PID but OP varies for differnt PID
    
    std::string display= op_isUserSpecified ? "user specified":"97% signal-eff";
    ATH_MSG_INFO("OP to use: " << op_name <<", with cut on BDT: "<<m_cutOnBDT<<", which corresponds to "<<display<<" working point.");
    
    std::string TrainingFile;
    if (!m_trainingFile.empty()) {  //If the property was set by the user, take that.
        
        //KM: below should be uncommented after the file is in the official place!!
        TrainingFile= PathResolverFindCalibFile( m_trainingFile );
        if(TrainingFile==""){//Error if it cant find the conf
            ATH_MSG_ERROR("Could not locate " << m_trainingFile );
            return StatusCode::FAILURE;
        }
        else ATH_MSG_INFO("trainingfile to use  " << TrainingFile );
        
    }
    else {
        ATH_MSG_ERROR("Could not find configuration file: \""<< m_trainingFile<<"\"");
        return StatusCode::FAILURE;
    }
    
    unsigned nfold=1;
    TFile* bdtfile = TFile::Open(TrainingFile.data());
    if (!bdtfile) {
        ATH_MSG_ERROR("Input file found to be empty!! "<< TrainingFile);
        return StatusCode::FAILURE;
    }
    else {
        TIter next(bdtfile->GetListOfKeys());
        TKey *key;
        while ((key = (TKey*)next())) {
            TClass *clas = gROOT->GetClass(key->GetClassName());
            if (!clas->InheritsFrom("TDirectoryFile")) continue;
            TDirectory *td = (TDirectoryFile*)key->ReadObj();
            std::string dirName =td->GetName();
            if (dirName.find(m_pid_name)!=std::string::npos) {
                std::string foldconf=dirName.substr(dirName.rfind("_")+1,-1);
                // std::string f_index=foldconf.substr(0,foldconf.find("o"));
                std::string s_nfold=foldconf.substr(foldconf.find("o")+1,-1);
                nfold=atoi(s_nfold.data());
                break;
            }
        }
    }
    
    ATH_MSG_INFO("ECIDS nfold configuration: "<<nfold);
    
    TObjArray* toa= (TObjArray*) bdtfile->Get("/ECIDS_"+m_pid_name+TString::Format("_0o%d",nfold)+"/variables");
    std::string commaSepVars="";
    if (toa) {
        TObjString *tos= 0;
        if (toa->GetEntries()>0) tos= (TObjString*) toa->At(0);
        commaSepVars=tos->GetString().Data();
        ATH_MSG_INFO("Variables for ECIDS= "<<commaSepVars);
    }
    else ATH_MSG_FATAL("Cannot access the list of input variables @"<<bdtfile->GetName()<<":/ECIDS_"+m_pid_name+TString::Format("_0o%d",nfold)+"/variables");
    
    //prepare m_inputVars
    m_inputVars.clear();
    while (commaSepVars.find(",")!=std::string::npos) {
        m_inputVars.push_back(commaSepVars.substr(0,commaSepVars.find(",")));
        commaSepVars.erase(0,commaSepVars.find(",")+1);
    }
    m_inputVars.push_back(commaSepVars.substr(0,-1));//push back the last element
    
    std::vector<float*> inputPointers;  //KM: set pointers
    for (unsigned i_fold=0; i_fold<nfold; i_fold++) {
        TString treename="/ECIDS_"+m_pid_name+TString::Format("_%do%d",i_fold,nfold)+"/BDT";
        //std::cout<<"Trying to access a ttree with name: "<<treename<<std::endl;
        TTree* tree = (TTree*)bdtfile->Get(treename);
        m_v_bdts.push_back(new MVAUtils::BDT(tree));
        bool noBadVariableFound = true;
        if (i_fold==0) noBadVariableFound = SetVariableRefs(m_inputVars,inputPointers);
        if (!noBadVariableFound) {
            std::cerr<<"\tUnknown input variable given, disabling the algorithm...."<<std::endl;
            exit(1);
            //KM: disabling feature to be implemented.
        }
        m_v_bdts.back()->SetPointers(inputPointers);
    }
    
    ///-----------End of text config----------------------------
    
    // Get the name of the current operating point, and massage the other strings accordingly
    //ATH_MSG_VERBOSE( "Going to massage the labels based on the provided operating point..." );
    
    m_cutPosition_bdt = m_acceptBDT.addCut( "bdt", "pass bdt" );
    m_resultPosition_bdt = m_resultBDT.addResult( "bdt", "ECIDS bdt" );
    
    return StatusCode::SUCCESS ;
}


//=============================================================================
// Asgena finalize method (now called by destructor)
//=============================================================================
StatusCode AsgElectronChargeIDSelectorTool::finalize()
{
    return StatusCode::SUCCESS;
}


//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
const Root::TAccept& AsgElectronChargeIDSelectorTool::accept( const xAOD::Electron* eg, double mu ) const
{
    
    double bdt=calculate(eg,mu);
    
    ATH_MSG_VERBOSE("\t accept( const xAOD::Electron* eg, double mu ), bdt="<<bdt);
    
    m_acceptBDT.clear();
    
    m_acceptBDT.setCutResult(m_cutPosition_bdt,bdt>m_cutOnBDT);
    
    return m_acceptBDT;
}

//=============================================================================
// Accept method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TAccept& AsgElectronChargeIDSelectorTool::accept( const xAOD::Egamma* eg, double mu) const
{
    double bdt=calculate(eg,mu);
    
    ATH_MSG_VERBOSE("\t accept( const xAOD::Egamma* eg, double mu ), bdt="<<bdt);
    
    m_acceptBDT.clear();
    
    m_acceptBDT.setCutResult(m_cutPosition_bdt,bdt>m_cutOnBDT);
    
    return m_acceptBDT;
}

//=============================================================================
// The main result method: the actual likelihood is calculated here
//=============================================================================
const Root::TResult& AsgElectronChargeIDSelectorTool::calculate( const xAOD::Electron* eg, double mu ) const
{
    
    ATH_MSG_VERBOSE("\t AsgElectronChargeIDSelectorTool::calculate( const xAOD::Electron* eg, double mu= "<<mu<<" )");
    
    if ( !eg ) {
        ATH_MSG_ERROR ("Failed, no egamma object.");
        return m_resultDummy;
    }
    
    const xAOD::CaloCluster* cluster = eg->caloCluster();
    if ( !cluster ) {
        ATH_MSG_ERROR ("Failed, no cluster.");
        return m_resultDummy;
    }
    
    const double energy =  cluster->e();
    const float eta = cluster->etaBE(2);
    if ( fabs(eta) > 300.0 ) {
        ATH_MSG_ERROR ("Failed, eta range.");
        return m_resultDummy;
    }
    
    double et = 0.;// transverse energy of the electron (using the track eta)
    if (eg->trackParticle() )
        et     = ( cosh(eg->trackParticle()->eta()) != 0.) ? energy/cosh(eg->trackParticle()->eta()) : 0.;
    else et  = ( cosh(eta) != 0.) ? energy/cosh(eta) : 0.;
    
    
    // number of track hits and other track quantities
    uint8_t nSCT(0);
    // uint8_t nTRThigh(0);
    // uint8_t nTRThighOutliers(0);
    // uint8_t nTRT(0);
    // uint8_t nTRTOutliers(0);
    // uint8_t nTRTXenon(0);
    float trackqoverp(0.0);
    float trackqoverpsig(0.0);
    int   charge(0.0);
    int   lifeSign(0.0);
    float trackchi2(0.0);
    float avgCharge_SCTw(0.0);
    float d0(0.0);
    float z0(0.0);
    float phi0(0.0);
    float theta(0.0);
    float EoverP(0.0);
    float d0sigma(0.0);
    double dpOverp(0.0);
    float TRT_PID(0.0);
    //double trans_TRT_PID(0.0);
    float deltaPhi1=0, deltaPhi2=0;
    float deltaPhiFromLM=0;
    float deltaPhiRescaled2=0;//deltaEta=0,
    //double rTRT(0.0);
    
    TVector2 el_cluster; el_cluster.SetMagPhi(cluster->energyBE(2)/cosh(eta),cluster->phiBE(2));
    
    bool allFound = true;
    // retrieve associated TrackParticle
    const xAOD::TrackParticle* t = eg->trackParticle();
    if (t) {
        trackqoverp = t->qOverP();
        charge= t->charge();
        d0 = t->d0();
        if(m_map_inputs.find("z0sinTheta"             )!= m_map_inputs.end()) {
            z0 = t->z0();
            theta = t->theta();
        }
        if(m_map_inputs.find("chi2oftrackfit"         )!= m_map_inputs.end())
            trackchi2 = t->chiSquared();
        
        phi0 = t->phi() + (d0>=0? M_PI/2 : -M_PI/2);
        TVector2 d0_direction;    d0_direction.SetMagPhi(fabs(d0),phi0);
        float inner_product = el_cluster.X()*d0_direction.X() + el_cluster.Y()*d0_direction.Y();
        lifeSign = inner_product>=0? 1 : -1;
        
        EoverP   = energy * fabs(t->qOverP());
        if(m_map_inputs.find("d0Err"                  )!= m_map_inputs.end() or
           m_map_inputs.find("d0Sig"                  )!= m_map_inputs.end() ) {
            float vard0 = t->definingParametersCovMatrix()(0,0);
            if (vard0 > 0) {
                d0sigma=sqrtf(vard0);
            }
        }
        
        //KM: calculation of SCT-weighted charge
        float charge = 0, SCT = 0;
        for (unsigned TPit = 0; TPit < eg->nTrackParticles(); TPit++) {
            uint8_t temp_NSCTHits;
            eg->trackParticle(TPit)->summaryValue(temp_NSCTHits, xAOD::numberOfSCTHits);
            //ATH_MSG_INFO ("kmdebug: trk#"<<TPit<<", #SCT-hits="<<unsigned(temp_NSCTHits)<<", charge="<<eg->trackParticle(TPit)->charge() );
            SCT += temp_NSCTHits;
            charge += temp_NSCTHits*eg->trackParticle(TPit)->charge();
        }
        avgCharge_SCTw= eg->charge()*charge/SCT;
        
        const std::vector<float>&cov= t->definingParametersCovMatrixVec();
        trackqoverpsig= cov[14];
        
        if(m_map_inputs.find("nSctHits"               )!= m_map_inputs.end())
            allFound = allFound && t->summaryValue(nSCT, xAOD::numberOfSCTHits);
        // allFound = allFound && t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
        // allFound = allFound && t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
        // allFound = allFound && t->summaryValue(nTRT, xAOD::numberOfTRTHits);
        // allFound = allFound && t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);
        // allFound = allFound && t->summaryValue(nTRTXenon, xAOD::numberOfTRTXenonHits);
        // allFound = allFound && t->summaryValue(TRT_PID, xAOD::eProbabilityHT);
        
        //Transform the TRT PID output for use in the LH tool.
        double fEpsilon = 1.0e-30;  // to avoid zero division
        double pid_tmp = TRT_PID;
        if (pid_tmp >= 1.0) pid_tmp = 1.0 - 1.0e-15;  //this number comes from TMVA
        else if (pid_tmp <= fEpsilon) pid_tmp = fEpsilon;
        // double tau = 15.0;
        //trans_TRT_PID = - log(1.0/pid_tmp - 1.0)*(1./double(tau));
        
        if(m_map_inputs.find("deltaPoverP"            )!= m_map_inputs.end()) {
            unsigned int index;
            if( t->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {
                
                double refittedTrack_LMqoverp  =
                t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
                                   std::pow(t->parameterPY(index), 2) +
                                   std::pow(t->parameterPZ(index), 2));
                
                dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
            }
        }
        
    }
    else {
        allFound=false;
        ATH_MSG_WARNING ( "Failed, no track particle: et= " << et << "eta= " << eta );
    }
    
    float Rphi(0);//float Reta(0), Rphi(0),  Rhad1(0), Rhad(0), ws3(0), w2(0), f1(0), Eratio(0), f3(0);
    allFound = allFound && eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi);// rphi e233/e237
    // allFound = allFound && eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);
    
    // difference between the cluster phi (sampling 2) and the eta of the track extrapolated from the last measurement point.
    allFound = allFound && eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);
    
    if(m_map_inputs.find("deltaphi1"              )!= m_map_inputs.end())
        allFound = allFound && eg->trackCaloMatchValue(deltaPhi1, xAOD::EgammaParameters::deltaPhi1);
    if(m_map_inputs.find("deltaphi2"              )!= m_map_inputs.end() or
       m_map_inputs.find("deltaDeltaPhiFirstAndLM")!= m_map_inputs.end())
        allFound = allFound && eg->trackCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::deltaPhi2);
    if(m_map_inputs.find("deltaDeltaPhiFirstAndLM")!= m_map_inputs.end())
        allFound = allFound && eg->trackCaloMatchValue(deltaPhiFromLM, xAOD::EgammaParameters::deltaPhiFromLastMeasurement);
    
    // TRT high-to-low threshold hits ratio
    // if ( nTRTXenon > 0 )
    //     rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRTXenon)) ;
    // else if ( nTRTXenon < 0 && (nTRT+nTRTOutliers) > 0 ) // this is always false as given
    //     rTRT = ((double)(nTRThigh+nTRThighOutliers)) / ((double)(nTRT+nTRTOutliers)) ;
    
    // Get the number of primary vertices in this event
    // double ip = static_cast<double>(m_nPVdefault);
    // if(mu < 0) // use npv if mu is negative (not given)
    //   ip = static_cast<double>(m_usePVCont ? this->getNPrimVertices() : m_nPVdefault);
    // else ip = mu;
    
    ATH_MSG_VERBOSE("xAOD variables: pt                           = "<< et                       <<",\t isRequested= "<<(m_map_inputs.find("pt"                     )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: eta                     = "<< eta                      <<",\t isRequested= "<<(m_map_inputs.find("eta"                    )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: abs_eta                 = "<< fabs(eta)                <<",\t isRequested= "<<(m_map_inputs.find("abs_eta"                )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: avgCharge_SCTw          = "<< avgCharge_SCTw           <<",\t isRequested= "<<(m_map_inputs.find("avgCharge_SCTw"         )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: d0                      = "<< d0                       <<",\t isRequested= "<<(m_map_inputs.find("d0"                     )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: ld0                     = "<< lifeSign*d0              <<",\t isRequested= "<<(m_map_inputs.find("ld0"                    )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: cd0                     = "<< charge*d0                <<",\t isRequested= "<<(m_map_inputs.find("cd0"                    )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: EoverP                  = "<< EoverP                   <<",\t isRequested= "<<(m_map_inputs.find("EoverP"                 )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: deltaphi1               = "<< deltaPhi1                <<",\t isRequested= "<<(m_map_inputs.find("deltaphi1"              )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: deltaphiRes             = "<< deltaPhiRescaled2        <<",\t isRequested= "<<(m_map_inputs.find("deltaphiRes"            )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: Rphi                    = "<< Rphi                     <<",\t isRequested= "<<(m_map_inputs.find("Rphi"                   )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: qoverpSig               = "<< trackqoverpsig           <<",\t isRequested= "<<(m_map_inputs.find("qoverpSig"              )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: nSctHits                = "<< unsigned(nSCT)           <<",\t isRequested= "<<(m_map_inputs.find("nSctHits"               )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: z0sinTheta              = "<< z0*sin(theta)            <<",\t isRequested= "<<(m_map_inputs.find("z0sinTheta"             )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: d0Err                   = "<< d0sigma                  <<",\t isRequested= "<<(m_map_inputs.find("d0Err"                  )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: d0Sig                   = "<< d0/d0sigma               <<",\t isRequested= "<<(m_map_inputs.find("d0Sig"                  )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: deltaphi2               = "<< deltaPhi2                <<",\t isRequested= "<<(m_map_inputs.find("deltaphi2"              )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: chi2oftrackfit          = "<< trackchi2                <<",\t isRequested= "<<(m_map_inputs.find("chi2oftrackfit"         )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: deltaPoverP             = "<< dpOverp                  <<",\t isRequested= "<<(m_map_inputs.find("deltaPoverP"            )!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: deltaDeltaPhiFirstandLM = "<< deltaPhi2-deltaPhiFromLM <<",\t isRequested= "<<(m_map_inputs.find("deltaDeltaPhiFirstAndLM")!=m_map_inputs.end())<<std::endl<<
                    "\t\t xAOD variables: AllFound                = "<<allFound            );
    
    if(m_map_inputs.find("pt"                     )!= m_map_inputs.end()) m_map_inputs["pt"                     ] = et               ;
    if(m_map_inputs.find("eta"                    )!= m_map_inputs.end()) m_map_inputs["eta"                    ] = eta              ;
    if(m_map_inputs.find("abs_eta"                )!= m_map_inputs.end()) m_map_inputs["abs_eta"                ] = fabs(eta)        ;
    if(m_map_inputs.find("avgCharge_SCTw"         )!= m_map_inputs.end()) m_map_inputs["avgCharge_SCTw"         ] = avgCharge_SCTw   ;
    if(m_map_inputs.find("d0"                     )!= m_map_inputs.end()) m_map_inputs["d0"                     ] = d0               ;
    if(m_map_inputs.find("ld0"                    )!= m_map_inputs.end()) m_map_inputs["ld0"                    ] = lifeSign*d0      ;
    if(m_map_inputs.find("cd0"                    )!= m_map_inputs.end()) m_map_inputs["cd0"                    ] = charge*d0        ;
    if(m_map_inputs.find("EoverP"                 )!= m_map_inputs.end()) m_map_inputs["EoverP"                 ] = EoverP           ;
    if(m_map_inputs.find("deltaphi1"              )!= m_map_inputs.end()) m_map_inputs["deltaphi1"              ] = deltaPhi1        ;
    if(m_map_inputs.find("deltaphiRes"            )!= m_map_inputs.end()) m_map_inputs["deltaphiRes"            ] = deltaPhiRescaled2;
    if(m_map_inputs.find("Rphi"                   )!= m_map_inputs.end()) m_map_inputs["Rphi"                   ] = Rphi             ;
    if(m_map_inputs.find("qoverpSig"              )!= m_map_inputs.end()) m_map_inputs["qoverpSig"              ] = trackqoverpsig   ;
    if(m_map_inputs.find("nSctHits"               )!= m_map_inputs.end()) m_map_inputs["nSctHits"               ] = nSCT             ;
    if(m_map_inputs.find("z0sinTheta"             )!= m_map_inputs.end()) m_map_inputs["z0sinTheta"             ] = z0*sin(theta)    ;
    if(m_map_inputs.find("d0Err"                  )!= m_map_inputs.end()) m_map_inputs["d0Err"                  ] = d0sigma          ;
    if(m_map_inputs.find("d0Sig"                  )!= m_map_inputs.end()) m_map_inputs["d0Sig"                  ] = d0/d0sigma       ;
    if(m_map_inputs.find("deltaphi2"              )!= m_map_inputs.end()) m_map_inputs["deltaphi2"              ] = deltaPhi2        ;
    if(m_map_inputs.find("chi2oftrackfit"         )!= m_map_inputs.end()) m_map_inputs["chi2oftrackfit"         ] = trackchi2        ;
    if(m_map_inputs.find("deltaPoverP"            )!= m_map_inputs.end()) m_map_inputs["deltaPoverP"            ] = dpOverp          ;
    if(m_map_inputs.find("deltaDeltaPhiFirstAndLM")!= m_map_inputs.end()) m_map_inputs["deltaDeltaPhiFirstAndLM"] = deltaPhi2-deltaPhiFromLM;
    //m_map_inputs["PtErr"                  ] = ;//to be implemented
    
    if (!allFound) ATH_MSG_FATAL("Missing input variable for ECIDS BDT calculation");
    
    const xAOD::EventInfo* eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo).isFailure()) ATH_MSG_WARNING ( " Cannot access to event info " );
    // lumiBlock = eventInfo->lumiBlock(), runNumber = eventInfo->runNumber(), eventNumber=eventInfo->eventNumber();
    //ATH_MSG_DEBUG("event_num%bdt_size="<<eventInfo->eventNumber()<<"%"<<unsigned(m_v_bdts.size())<<"= "<<eventInfo->eventNumber()%unsigned(m_v_bdts.size()));
    unsigned bdt_index=eventInfo->eventNumber()%unsigned(m_v_bdts.size());
    
    ////KM: dumping variables
    for (auto input: m_map_inputs)
        ATH_MSG_DEBUG("\t input: "<<input.first<<"\t= "<<input.second);
    
    ////KM: dumping variables, only variables used by BDT
    // unsigned i=0;
    // for (auto pointer: m_v_bdts.at(bdt_index)->GetPointers()) {
    //   std::cout<<"\t kmdebug: "<<m_inputVars.at(i)<<"\t("<<pointer<<")\t = "<<*pointer<<std::endl; i++;
    // }
    
    double bdt_output = m_v_bdts.at(bdt_index)->GetGradBoostMVA(m_v_bdts.at(bdt_index)->GetPointers());
    ATH_MSG_DEBUG("ECIDS-BDT= "<<bdt_output);
    
    m_resultBDT.setResult(m_resultPosition_bdt,bdt_output);
    return m_resultBDT;
}

//=============================================================================
// Calculate method for EFCaloLH in the trigger; do full LH if !CaloCutsOnly
//=============================================================================
const Root::TResult& AsgElectronChargeIDSelectorTool::calculate( const xAOD::Egamma* eg, double mu ) const
{
    ATH_MSG_VERBOSE("AsgElectronChargeIDSelectorTool::calculate( const xAOD::Egamma* "<<eg<<", double mu= "<<mu<< " ) const");
    ATH_MSG_WARNING("Method not implemented for egamma object! Reurning -1!!");
    
    m_resultBDT.setResult(m_resultPosition_bdt,-1);
    return m_resultBDT;
}

//=============================================================================
const Root::TAccept& AsgElectronChargeIDSelectorTool::accept(const xAOD::IParticle* part) const
{
    ATH_MSG_VERBOSE("Entering accept( const IParticle* part )");
    const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
    if(eg)
    {
        return accept(eg);
    }
    else{
        ATH_MSG_ERROR("AsgElectronChargeIDSelectorTool::could not cast to const Electron");
        return m_acceptDummy;
    }
}

const Root::TResult& AsgElectronChargeIDSelectorTool::calculate(const xAOD::IParticle* part) const
{
    const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
    if (eg)
    {
        return calculate(eg);
    }
    else
    {
        ATH_MSG_ERROR ( " Could not cast to const Electron " );
        return m_resultDummy;
    }
}




//=============================================================================
// Helper method to get the number of primary vertices
// ( This is horrible! We don't want to iterate over all vertices in the event for each electron!!!
//   This is slow!)
//=============================================================================
unsigned int AsgElectronChargeIDSelectorTool::getNPrimVertices() const
{
    static bool PVExists = true;
    unsigned int nVtx(0);
    const xAOD::VertexContainer* vxContainer(0);
    if(PVExists)
    {
        if ( StatusCode::SUCCESS != evtStore()->retrieve( vxContainer, m_primVtxContName ) )
        {
            ATH_MSG_WARNING ( "Vertex container not found with name: " << m_primVtxContName );
            PVExists = false; // if retrieve failed, don't try to retrieve again
            return nVtx;
        }
        for ( unsigned int i=0; i<vxContainer->size(); i++ )
        {
            const xAOD::Vertex* vxcand = vxContainer->at(i);
            if ( vxcand->nTrackParticles() >= 2 ) nVtx++;
        }
    }
    return nVtx;
}


bool AsgElectronChargeIDSelectorTool::SetVariableRefs(std::vector<std::string> inputs, std::vector<float*> &inputPointers) {
    
    bool unknownVarFound = false;
    unsigned nVars = 0;
    
    for (auto inputName: inputs) {
        //KM:                                           create elements by assigning default values.
        if      (inputName=="pt"                     ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="eta"                    ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="abs_eta"                ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="avgCharge_SCTw"         ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="d0"                     ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="ld0"                    ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="cd0"                    ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="EoverP"                 ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="deltaphi1"              ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="deltaphiRes"            ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="Rphi"                   ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="qoverpSig"              ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="nSctHits"               ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="z0sinTheta"             ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="d0Err"                  ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="d0Sig"                  ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="deltaphi2"              ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="chi2oftrackfit"         ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="deltaPoverP"            ) {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else if (inputName=="deltaDeltaPhiFirstAndLM") {ATH_MSG_VERBOSE("\t Setting up BDT using variable: "<<inputName); m_map_inputs[inputName]=0; inputPointers.push_back(&m_map_inputs[inputName]); nVars++;}
        else {
            ATH_MSG_WARNING("Unknown variables found with name: "<<inputName);
            unknownVarFound = true;
        }
    }
    
    return inputs.size()==nVars and !unknownVarFound;
    
}

