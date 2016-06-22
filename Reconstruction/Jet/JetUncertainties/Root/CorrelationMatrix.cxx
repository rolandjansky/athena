/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/CorrelationMatrix.h"
#include "JetUncertainties/Helpers.h"

#include "TH2D.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////
 
CorrelationMatrix::CorrelationMatrix(const std::string& name)
    : asg::AsgMessaging(name)
    , m_isInit(false)
    , m_name(name.c_str())
    , m_numBins(-1)
    , m_minVal(1e10)
    , m_maxVal(-1e10)
    , m_fixedVal1(0)
    , m_fixedVal2(0)
    , m_corrMat(NULL)
    , m_jets(NULL)
    , m_eInfos(NULL)
    , m_jet1(NULL)
    , m_jet2(NULL)
    , m_eInfo(NULL)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

CorrelationMatrix::CorrelationMatrix(const TString name, const int numBins, const double minVal, const double maxVal, const double fixedVal1, const double fixedVal2)
    : asg::AsgMessaging(name.Data())
    , m_isInit(false)
    , m_name(name)
    , m_numBins(numBins)
    , m_minVal(minVal)
    , m_maxVal(maxVal)
    , m_fixedVal1(fixedVal1)
    , m_fixedVal2(fixedVal2)
    , m_corrMat(NULL)
    , m_jets(NULL)
    , m_eInfos(NULL)
    , m_jet1(NULL)
    , m_jet2(NULL)
    , m_eInfo(NULL)
{
    ATH_MSG_DEBUG("Creating CorrelationMatrix named " << m_name.Data());
}


CorrelationMatrix::~CorrelationMatrix()
{
    JESUNC_SAFE_DELETE(m_corrMat);
}


StatusCode CorrelationMatrix::initializeForPt(const JetUncertaintiesTool& uncTool)
{
    // Check for initialization and tool status
    if (checkInitialization(uncTool).isFailure())
        return StatusCode::FAILURE;
    m_isInit = true;

    // Determine the center of mass energy for kinematic limits if possible
    // If not possible, then ignores kinematic limits
    // pt * cosh(eta) = sqrtS/2 for kinematic limit
    const double sqrtS = uncTool.getSqrtS();
    const double kinLimit1 = sqrtS > 0 ? (sqrtS/2.) / cosh(m_fixedVal1) : 1e99;
    const double kinLimit2 = sqrtS > 0 ? (sqrtS/2.) / cosh(m_fixedVal2) : 1e99;

    // Build the correlation matrix
    m_corrMat = buildMatrix(utils::getLogBins(m_numBins,m_minVal,m_maxVal));

    // Prepare the objects for use
    if (createStore().isFailure())
        return StatusCode::FAILURE;
    
    // Set default values
    if (setDefaultProperties(uncTool).isFailure())
        return StatusCode::FAILURE;

    // Fill the correlation matrix
    for (int binX = 1; binX <= m_corrMat->GetNbinsX(); ++binX)
    {
        // Set the jet values, (pT, eta, phi, m)
        // note that only pT and eta matter right now
        const double valX = m_corrMat->GetXaxis()->GetBinCenter(binX);
        m_jet1->setJetP4(xAOD::JetFourMom_t(valX,m_fixedVal1,0,0));
        
        for (int binY = 1; binY <= m_corrMat->GetNbinsY(); ++binY)
        {
            const double valY = m_corrMat->GetYaxis()->GetBinCenter(binY);

            // Check for the kinematic limit (simple case)
            if (valX > kinLimit1 || valY > kinLimit2 )
                m_corrMat->SetBinContent(binX,binY,JESUNC_ERROR_CODE);
            else
            {
                // Set the jet values, (pT, eta, phi, m)
                // note that only pT and eta matter right now
                m_jet2->setJetP4(xAOD::JetFourMom_t(valY,m_fixedVal2,0,0));

                // Now determine the correlation for the two jets
                m_corrMat->SetBinContent(binX,binY,getCorrelation(uncTool));
            }
        }
    }

    // Clear up the objects we used
    if (clearStore().isFailure())
        return StatusCode::FAILURE;

    // Done
    return StatusCode::SUCCESS;
}

StatusCode CorrelationMatrix::initializeForEta(const JetUncertaintiesTool& uncTool)
{
    // Check for initialization and tool status
    if (checkInitialization(uncTool).isFailure())
        return StatusCode::FAILURE;
    m_isInit = true;
    
    // Determine the center of mass energy for kinematic limits if possible
    // If not possible, then ignores kinematic limits
    // pt * cosh(eta) = sqrtS/2 for kinematic limit
    const double sqrtS = uncTool.getSqrtS();
    const double kinLimit1 = sqrtS > 0 ? acosh((sqrtS/2.) / m_fixedVal1) : 1e99;
    const double kinLimit2 = sqrtS > 0 ? acosh((sqrtS/2.) / m_fixedVal2) : 1e99;
    
    // Build the correlation matrix
    m_corrMat = buildMatrix(utils::getUniformBins(m_numBins,m_minVal,m_maxVal));

    // Prepare the objects for use
    if (createStore().isFailure())
        return StatusCode::FAILURE;

    // Set default values
    if (setDefaultProperties(uncTool).isFailure())
        return StatusCode::FAILURE;

    // Fill the correlation matrix
    for (int binX = 1; binX <= m_corrMat->GetNbinsX(); ++binX)
    {
        // Set the jet values, (pT, eta, phi, m)
        // note that only pT and eta matter right now
        const double valX = m_corrMat->GetXaxis()->GetBinCenter(binX);
        m_jet1->setJetP4(xAOD::JetFourMom_t(m_fixedVal1,valX,0,0));
        
        for (int binY = 1; binY <= m_corrMat->GetNbinsY(); ++binY)
        {
            const double valY = m_corrMat->GetYaxis()->GetBinCenter(binY);

            // Check for the kinematic limit (simple case)
            if (valX > kinLimit1 || valY > kinLimit2)
                m_corrMat->SetBinContent(binX,binY,JESUNC_ERROR_CODE);
            else
            {
                // Set the jet values, (pT, eta, phi, m)
                // note that only pT and eta matter right now
                m_jet2->setJetP4(xAOD::JetFourMom_t(m_fixedVal2,valY,0,0));

                // Now determine the correlation for the two jets
                m_corrMat->SetBinContent(binX,binY,getCorrelation(uncTool));
            }
        }
    }

    // Clear up the objects we used
    if (clearStore().isFailure())
        return StatusCode::FAILURE;

    // Done
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Helper methods for building the matrices    //
//                                              //
//////////////////////////////////////////////////

StatusCode CorrelationMatrix::checkInitialization(const JetUncertaintiesTool& uncTool) const
{
    // Prevent double-initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("CorrelationMatrix is already initialized: " << m_name.Data());
        return StatusCode::FAILURE;
    }

    // Warn about any non-four-vector terms which will be ignored
    bool scalesFourVec = false;
    for (size_t iComp = 0; iComp < uncTool.getNumComponents(); ++iComp)
        if (!uncTool.getComponentScalesFourVec(iComp))
            ATH_MSG_WARNING("CorrelationMatrix will ignore component which does not scale the jet four-vector: " << uncTool.getComponentName(iComp));
        else
            scalesFourVec = true;
    
    // If no components scale the four-vector, end now
    if (!scalesFourVec)
    {
        ATH_MSG_ERROR("No components scale the four-vector for the CorrelationMatrix: " << m_name.Data());
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode CorrelationMatrix::createStore()
{
    // Build a jet container and a pair of jets for us to manipulate later
    m_jets   = new xAOD::JetContainer();
    m_jets->setStore(new xAOD::JetAuxContainer());
    m_jets->push_back(new xAOD::Jet());
    m_jets->push_back(new xAOD::Jet());
    m_jet1 = m_jets->at(0);
    m_jet2 = m_jets->at(1);

    // Build an EventInfo object for us to manipulate later
    m_eInfos = new xAOD::EventInfoContainer();
    m_eInfos->setStore(new xAOD::EventInfoAuxContainer());
    m_eInfos->push_back(new xAOD::EventInfo());
    m_eInfo = m_eInfos->at(0);

    return StatusCode::SUCCESS;
}

StatusCode CorrelationMatrix::clearStore()
{
    m_jet1  = NULL;
    m_jet2  = NULL;
    m_eInfo = NULL;
    
    //for (size_t iJet = 0; iJet < m_jets->size(); ++iJet)
    //    JESUNC_SAFE_DELETE((*m_jets)[iJet]);
    m_jets->clear();
    JESUNC_SAFE_DELETE(m_jets);

    //for (size_t iInfo = 0; iInfo < m_eInfos->size(); ++iInfo)
    //    JESUNC_SAFE_DELETE((*m_eInfos)[iInfo]);
    m_eInfos->clear();
    JESUNC_SAFE_DELETE(m_eInfos);

    return StatusCode::SUCCESS;
}

StatusCode CorrelationMatrix::setDefaultProperties(const JetUncertaintiesTool& uncTool)
{
    // TODO make this part of a common file
    static SG::AuxElement::Accessor<int> Nsegments("GhostMuonSegmentCount");
    static SG::AuxElement::Accessor<char> IsBjet("IsBjet");
    static SG::AuxElement::Accessor<float> mu("averageInteractionsPerCrossing");
    static SG::AuxElement::Accessor<float> NPV("NPV");   
 
    // 25 segments is about average for jets receiving a correction
    // This is modulated by the probability of punchthrough
    // TODO add punch-through Nsegments/etc dependence on probability
    Nsegments(*m_jet1) = 0; //25;
    Nsegments(*m_jet2) = 0; //25;
    IsBjet(*m_jet1) = false;
    IsBjet(*m_jet2) = false;
    
    float    sigmaMu  = 0;
    float    sigmaNPV = 0;

    const TString config = uncTool.getConfigFile();
    if (config.Contains("_2011/"))
    {
        // Dag, night of Febuary 4/5, 2013
        sigmaMu  = 3.0;
        sigmaNPV = 3.0;
    }
    else if (config.Contains("_2012/") || config.Contains("_2015/Prerec"))
    {
        // Craig Sawyer, Jan 22 2013
        sigmaMu  = 5.593*1.11;
        sigmaNPV = 3.672;
    }
    else if (config.Contains("_2015"))
    {
        // Kate, Jan 31 2016
        sigmaMu = 1.9;
        sigmaNPV = 2.9;
    }
    else
    {
        printf("Unexpected year for setPileupShiftsForYear in config: %s\n",config.Data());
        exit(-1);
    }
    
    mu(*m_eInfo) = uncTool.getRefMu()+sigmaMu;
    NPV(*m_eInfo) = uncTool.getRefNPV()+sigmaNPV;

    return StatusCode::SUCCESS;
}

TH2D* CorrelationMatrix::buildMatrix(const std::vector<double>& bins) const
{
    TH2D* matrix = new TH2D(m_name,m_name,bins.size()-1,&bins[0],bins.size()-1,&bins[0]);
    matrix->GetZaxis()->SetRangeUser(-1,1);
    return matrix;
}

double CorrelationMatrix::getCorrelation(const JetUncertaintiesTool& uncTool) const
{
    return getCovariance(uncTool,m_jet1,m_jet2)/sqrt(getCovariance(uncTool,m_jet1,m_jet1)*getCovariance(uncTool,m_jet2,m_jet2));
}

double CorrelationMatrix::getCovariance(const JetUncertaintiesTool& uncTool, const xAOD::Jet* jet1, const xAOD::Jet* jet2) const
{
    double covariance = 0;
    for (size_t iComp = 0; iComp < uncTool.getNumComponents(); ++iComp)
        covariance += uncTool.getUncertainty(iComp,*jet1,*m_eInfo)*uncTool.getUncertainty(iComp,*jet2,*m_eInfo);

    return covariance;
}

} // end jet namespace
