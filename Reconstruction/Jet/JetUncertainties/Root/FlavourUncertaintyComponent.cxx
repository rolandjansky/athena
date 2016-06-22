/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/FlavourUncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"
#include "TList.h"
#include "TKey.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

FlavourUncertaintyComponent::FlavourUncertaintyComponent(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name),0)
    , m_flavourType(FlavourComp::UNKNOWN)
    , m_jetType("")
    , m_analysisFileName("")
    , m_absEta(false)
    , m_secondUncName("")
    , m_secondUncHist(NULL)
    , m_respType(FlavourResp_UNKNOWN)
    , m_secondRespType(FlavourResp_UNKNOWN)
    , m_BjetAccessor("IsBjet")
    , m_NjetAccessor("Njet")
    , m_gluonFractionHists()
    , m_gluonFractionErrorHists()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

FlavourUncertaintyComponent::FlavourUncertaintyComponent(   const ComponentHelper& component,
                                                            const TString jetType,
                                                            const TString analysisRootFileName,
                                                            const TString path
                                                            )
    : UncertaintyComponent(component,component.flavourType == FlavourComp::Composition ? 2 : 1)
    , m_flavourType(component.flavourType)
    , m_jetType(jetType)
    , m_analysisFileName(analysisRootFileName)
    , m_path(path)
    , m_absEta(CompParametrization::isAbsEta(component.parametrization))
    , m_secondUncName(component.uncNames.size()>1 ? component.uncNames.at(1) : "")
    , m_secondUncHist(NULL)
    , m_respType(FlavourResp_UNKNOWN)
    , m_secondRespType(FlavourResp_UNKNOWN)
    , m_BjetAccessor("IsBjet")
    , m_NjetAccessor("Njet")
    , m_gluonFractionHists()
    , m_gluonFractionErrorHists()
{
    ATH_MSG_DEBUG("Created FlavourUncertaintyComponent named" << m_uncHistName.Data());
    
    // Ensure that the flavour type and ref values are sensible
    if (m_flavourType == FlavourComp::UNKNOWN)
        ATH_MSG_FATAL("Flavour type is UNKNOWN: " << m_uncHistName.Data());
}

FlavourUncertaintyComponent::FlavourUncertaintyComponent(const FlavourUncertaintyComponent& toCopy)
    : UncertaintyComponent(toCopy)
    , m_flavourType(toCopy.m_flavourType)
    , m_jetType(toCopy.m_jetType)
    , m_analysisFileName(toCopy.m_analysisFileName)
    , m_path(toCopy.m_path)
    , m_absEta(toCopy.m_absEta)
    , m_secondUncName(toCopy.m_secondUncName)
    , m_secondUncHist(NULL)
    , m_respType(toCopy.m_respType)
    , m_secondRespType(toCopy.m_secondRespType)
    , m_BjetAccessor(toCopy.m_BjetAccessor)
    , m_NjetAccessor(toCopy.m_NjetAccessor)
    , m_gluonFractionHists()
    , m_gluonFractionErrorHists()
{
    ATH_MSG_DEBUG(Form("Creating copy of FlavourUncertaintyComponent named %s",m_uncHistName.Data()));
    
    if (toCopy.m_secondUncHist)
        m_secondUncHist = new UncertaintyHistogram(*toCopy.m_secondUncHist);

    for (size_t iHist = 0; iHist < toCopy.m_gluonFractionHists.size(); ++iHist)
        if (toCopy.m_gluonFractionHists.at(iHist))
            m_gluonFractionHists.push_back(new UncertaintyHistogram(*toCopy.m_gluonFractionHists.at(iHist)));

    for (size_t iHist = 0; iHist < toCopy.m_gluonFractionErrorHists.size(); ++iHist)
        if (toCopy.m_gluonFractionErrorHists.at(iHist))
            m_gluonFractionErrorHists.push_back(new UncertaintyHistogram(*toCopy.m_gluonFractionErrorHists.at(iHist)));
}

FlavourUncertaintyComponent* FlavourUncertaintyComponent::clone() const
{
    return new FlavourUncertaintyComponent(*this);
}

FlavourUncertaintyComponent::~FlavourUncertaintyComponent()
{
    JESUNC_SAFE_DELETE(m_secondUncHist);

    for (size_t iHisto = 0; iHisto < m_gluonFractionHists.size(); ++iHisto)
    {
        JESUNC_SAFE_DELETE(m_gluonFractionHists.at(iHisto));
        JESUNC_SAFE_DELETE(m_gluonFractionErrorHists.at(iHisto));
    }
    m_gluonFractionHists.clear();
    m_gluonFractionErrorHists.clear();
}

StatusCode FlavourUncertaintyComponent::initialize(TFile* histFile)
{
    // Call the base class first
    if (UncertaintyComponent::initialize(histFile).isFailure())
        return StatusCode::FAILURE;
    
    // Ensure that the number of histograms matches what is expected for Flavour components
    if (m_flavourType == FlavourComp::Response && m_secondUncName != "")
    {
        ATH_MSG_ERROR("Expected one histogram for FlavourResponse: " << getName().Data());
        return StatusCode::FAILURE;
    }
    else if (m_flavourType == FlavourComp::Composition && m_secondUncName == "")
    {
        ATH_MSG_ERROR("Expected two histograms for FlavourComposition: " << getName().Data());
        return StatusCode::FAILURE;
    }
    else if (m_flavourType == FlavourComp::bJES && m_secondUncName != "")
    {
        ATH_MSG_ERROR("Expected one histogram for bJES uncertainty: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Get the flavour response types if applicable
    if (m_flavourType == FlavourComp::Composition)
    {
        if      ( m_uncHistName.Contains("glu",TString::kIgnoreCase) && (m_secondUncName.Contains("light",TString::kIgnoreCase) || m_secondUncName.Contains("quark",TString::kIgnoreCase)) )
        {
            m_respType = FlavourResp_GLUON;
            m_secondRespType = FlavourResp_QUARK;
        }
        else if ( (m_uncHistName.Contains("light",TString::kIgnoreCase) || m_uncHistName.Contains("quark",TString::kIgnoreCase)) && m_secondUncName.Contains("glu",TString::kIgnoreCase) )
        {
            m_respType = FlavourResp_QUARK;
            m_secondRespType = FlavourResp_GLUON;
        }
        else
        {
            // Unexpected inputs
            ATH_MSG_ERROR("Component is FlavourComposition, but histogram names are unexpected (need to discriminate gluon vs quark response histograms): "  << m_uncHistName.Data() << " and " << m_secondUncName.Data());
            return StatusCode::FAILURE;
        }

    }

    // Create the second histogram if applicable
    if (m_flavourType == FlavourComp::Composition)
    {
        m_secondUncHist = new UncertaintyHistogram(m_secondUncName,m_interpolate);
        if (!m_secondUncHist)
        {
            ATH_MSG_ERROR("Failed to create second uncertainty histogram for component: " << getName().Data());
            return StatusCode::FAILURE;
        }
        if (m_secondUncHist->initialize(histFile).isFailure()) return StatusCode::FAILURE;
    }

    // Now read the analysis input histograms if this is not a bJES component
    if (m_flavourType != FlavourComp::bJES)
    {
        TFile* analysisFile = utils::readRootFile(m_analysisFileName,m_path);
        if (!analysisFile || analysisFile->IsZombie())
        {
            ATH_MSG_ERROR("Cannot open analysis histogram file: " << m_analysisFileName.Data());
            return StatusCode::FAILURE;
        }

        // Retrieve the gluon fraction(s) and gluon fraction uncertainty(ies)
        // May be a single histogram (default) or specified by nJet bins
        // Store all of the key names for now, retrieval will happen later
        std::vector<TString> gluonFractionKeys;
        std::vector<TString> gluonFractionErrorKeys;
        TList* keys = analysisFile->GetListOfKeys();
        TIter nextkey(keys);
        while (TKey* key = dynamic_cast<TKey*>(nextkey()))
        {
            if (!key) continue;
            const TString keyName = key->GetName();
            if (keyName.Contains(m_jetType) && !keyName.Contains("valid"))
            {
                if (keyName.Contains("gluonFractionError"))
                    gluonFractionErrorKeys.push_back(keyName);
                else if (keyName.Contains("gluonFraction"))
                    gluonFractionKeys.push_back(keyName);
            }
        }

        // Ensure we found histograms...
        if (!gluonFractionKeys.size() || !gluonFractionErrorKeys.size())
        {
            ATH_MSG_ERROR(Form("Failed to find gluon fraction histogram(s), found %zu nominal and %zu error hists in file %s",gluonFractionKeys.size(),gluonFractionErrorKeys.size(),m_analysisFileName.Data()));
            return StatusCode::FAILURE;
        }

        // Determine the max nJet value
        int nJetsMax = -1;
        for (size_t iKey = 0; iKey < gluonFractionKeys.size(); ++iKey)
        {
            int nJets = -1;
            if (getNjetFromKey(gluonFractionKeys.at(iKey),nJets).isFailure())
                return StatusCode::FAILURE;
            if (nJets > nJetsMax)
                nJetsMax = nJets;
        }

        // If there is only one histogram and it's not an nJets histogram, this is trivial
        if (nJetsMax < 0 && gluonFractionKeys.size() == 1 && gluonFractionErrorKeys.size() == 1)
        {
            m_gluonFractionHists.push_back(new UncertaintyHistogram(gluonFractionKeys.at(0),m_interpolate));
            m_gluonFractionErrorHists.push_back(new UncertaintyHistogram(gluonFractionErrorKeys.at(0),m_interpolate));
        }
        // If there is more than one histogram and they are not nJets histograms, this is a problem
        else if (nJetsMax < 0 && gluonFractionKeys.size() > 1)
        {
            ATH_MSG_ERROR(Form("Found %zu gluon fraction histograms, but they do not appear to be binned by nJets:",gluonFractionKeys.size()));
            for (size_t iKey = 0; iKey < gluonFractionKeys.size(); ++iKey)
                ATH_MSG_ERROR(Form("\tKey %zu: %s",iKey,gluonFractionKeys.at(iKey).Data()));
            return StatusCode::FAILURE;
        }
        else
        {
            // Fill the actual vectors now, in proper order
            for (int nJets = 0; nJets < nJetsMax; ++nJets)
            {
                m_gluonFractionHists.push_back(NULL);
                m_gluonFractionErrorHists.push_back(NULL);
            }
            if (readNjetsHistograms(m_gluonFractionHists,gluonFractionKeys).isFailure())
                return StatusCode::FAILURE;
            if (readNjetsHistograms(m_gluonFractionErrorHists,gluonFractionErrorKeys).isFailure())
                return StatusCode::FAILURE;
            
            // Ensure that every non-NULL gluon fraction has a non-NULL gluon fraction error
            // (Note that not all indices must be filled, as maybe an analyis is only 4 and 5 jets)
            for (size_t iJet = 0; iJet < m_gluonFractionHists.size(); ++iJet)
            {
                if (m_gluonFractionHists.at(iJet) && !m_gluonFractionErrorHists.at(iJet))
                {
                    ATH_MSG_ERROR(Form("nJets = %zu was specified for the gluon fraction, but not the error",iJet));
                    return StatusCode::FAILURE;
                }
                else if (!m_gluonFractionHists.at(iJet) && m_gluonFractionErrorHists.at(iJet))
                {
                    ATH_MSG_ERROR(Form("nJets = %zu was specified for the error, but not the gluon fraction",iJet));
                    return StatusCode::FAILURE;
                }
            }
        }
        
        // Initialize the histograms
        for (size_t iHist = 0; iHist < m_gluonFractionHists.size(); ++iHist)
        {
            if (m_gluonFractionHists.at(iHist)->initialize(analysisFile).isFailure())
                return StatusCode::FAILURE;
            if (m_gluonFractionErrorHists.at(iHist)->initialize(analysisFile).isFailure())
                return StatusCode::FAILURE;
        }

        // We're finally done reading that file...
        analysisFile->Close();
    }

    return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool FlavourUncertaintyComponent::getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // Currently only one validity histogram exists
    // Might need to expand in the future
    
    return !m_validHist ? true : getValidBool(m_validHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()));

//    // Valid only if all histogram(s) are valid
//    // Histograms to consider varies by flavour type
//    // Start with the standard histograms
//    for (size_t iHisto = 0; iHisto < m_histos.size(); ++iHisto)
//        if (!m_histos.at(iHisto)->getValidity(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()))
//            return false;
//
//    // Now do analysis histograms
//    for (size_t iHisto = 0; iHisto < m_gluonFractionHists.size(); ++iHisto)
//        if (!m_gluonFractionHists.at(iHisto)->getValidity(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()) || 
//            !m_gluonFractionErrorHists.at(iHisto)->getValidity(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta()) )
//            return false;
//    
//    return true;
}

double FlavourUncertaintyComponent::getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    double unc = JESUNC_ERROR_CODE;
    if (m_flavourType == FlavourComp::Response)
        unc = getFlavourResponseUncertainty(jet,eInfo);
    else if (m_flavourType == FlavourComp::Composition)
        unc = getFlavourCompositionUncertainty(jet,eInfo);
    else if (m_flavourType == FlavourComp::bJES)
        unc = getBJESUncertainty(jet,eInfo);
    else
    {
        ATH_MSG_ERROR("Unknown flavour type for " << getName().Data());
        return unc;
    }
    
    return unc;
}

double FlavourUncertaintyComponent::getFlavourResponseUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    //  Assumption:
    //      dR(q) = JES uncertainty (measured in gamma/Z+jet)
    //      dR(g) = JES uncertainty (+) additional gluon response component
    //  component to be added to JES uncertainty: 
    //      fg*dR(gluon response modelling uncertainty)
    //      where gluon response modelling uncertainty is taken as difference between gluon response in Pythia and Herwig++
    
    // Check if this is a b-jet
    if (isBjet(jet))
        return 0;
    
    // Get the number of jets
    int nJets = 0;
    if (m_gluonFractionHists.size() > 1)
    {
        if (m_NjetAccessor.isAvailable(eInfo))
            nJets = m_NjetAccessor(eInfo);
        else
        {
            ATH_MSG_ERROR("Specified Njets treatment, but did not decorate EventInfo object");
            return JESUNC_ERROR_CODE;
        }
    }
    
    if (checkNjetsInput(nJets).isFailure())
        return JESUNC_ERROR_CODE;

    const double pT  = jet.pt()*m_energyScale;
    const double eta = m_absEta ? fabs(jet.eta()) : jet.eta();
    
    // return the uncertainty
    return getGluonResponseDifference(pT,eta) * getGluonFraction(pT,eta,nJets);
}

double FlavourUncertaintyComponent::getFlavourCompositionUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    //  Returns the flavor composition uncertainty using the formula:
    //    
    //  Uncertainty = df * | Rq - Rg | / ( fg * Rg + (1 - fg) * Rq )
    //  with
    //      Rs = fg * Rg + (1 - fg) * Rq as heavy q uncertainties accounted separately
    //      df = error on fg
    //      fg = fraction of gluons
    //      Rq = light quark response
    //      Rg = gluon response

    // Check if this is a b-jet
    if (isBjet(jet))
        return 0;

    // Get the number of jets
    int nJets = 0;
    if (m_gluonFractionHists.size() > 1)
    {
        if (m_NjetAccessor.isAvailable(eInfo))
            nJets = m_NjetAccessor(eInfo);
        else
        {
            ATH_MSG_ERROR("Specified Njets treatment, but did not decorate EventInfo object");
            return JESUNC_ERROR_CODE;
        }
    }


    if (checkNjetsInput(nJets).isFailure())
        return JESUNC_ERROR_CODE;

    const double pT  = jet.pt()*m_energyScale;
    const double eta = m_absEta ? fabs(jet.eta()) : jet.eta();
    
    //calculating the sample response:
    //fg*Rg + (1-fg)*Rq
    const double gluonFrac = getGluonFraction(pT,eta,nJets);
    const double Rg = getGluonResponseBaseline(pT,eta);
    const double Rq = getQuarkResponseBaseline(pT,eta);
     
    const double Rsample = gluonFrac * Rg + (1-gluonFrac) * Rq;
    
    //this should never happen (it means the Rg == Rq == 0), but checking anyway
    if (Rsample==0)
    {
        ATH_MSG_ERROR(Form("R(sample) = 0 for pT=%.1f, eta=%.2f",pT,eta));
        return JESUNC_ERROR_CODE;
    }
    
    //calculating the uncertainty
    const double gluonFracError = getGluonFractionError(pT,eta,nJets);
    const double flavorCompUnc  = gluonFracError*fabs(Rq-Rg)/Rsample;
    
    return flavorCompUnc;

}

double FlavourUncertaintyComponent::getBJESUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo&) const
{
    // Ensure this is a b-jet
    if (!isBjet(jet))
        return 0;
    return m_uncHist->getValue(jet.pt()*m_energyScale,m_absEta ? fabs(jet.eta()) : jet.eta());
}


//////////////////////////////////////////////////
//                                              //
//  Wrapper functions for special flavour hists //
//                                              //
//////////////////////////////////////////////////

double FlavourUncertaintyComponent::getGluonFraction(const double pT, const double eta, const int nJets) const
{
    return m_gluonFractionHists.at(nJets)->getValue(pT,fabs(eta));
}

double FlavourUncertaintyComponent::getGluonFractionError(const double pT, const double eta, const int nJets) const
{
    return m_gluonFractionErrorHists.at(nJets)->getValue(pT,fabs(eta));
}

double FlavourUncertaintyComponent::getGluonResponseDifference(const double pT, const double eta) const
{
    if (m_flavourType != FlavourComp::Response)
    {
        ATH_MSG_ERROR("This method is only useable for FlavourResponse uncertainties, not " << getName().Data());
        return JESUNC_ERROR_CODE;
    }
    return m_uncHist->getValue(pT,eta);
}

double FlavourUncertaintyComponent::getGluonResponseBaseline(const double pT, const double eta) const
{
    if (m_flavourType != FlavourComp::Composition)
    {
        ATH_MSG_ERROR("This method is only useable for FlavourComposition uncertainties, not " << getName().Data());
        return JESUNC_ERROR_CODE;
    }

    if (m_respType == FlavourResp_GLUON)
        return m_uncHist->getValue(pT,eta);
    else if (m_secondRespType == FlavourResp_GLUON)
        return m_secondUncHist->getValue(pT,eta);
    ATH_MSG_ERROR("Unexpected flavour response parametrization: " << getName().Data());
    return JESUNC_ERROR_CODE;
}

double FlavourUncertaintyComponent::getQuarkResponseBaseline(const double pT, const double eta) const
{
    if (m_flavourType != FlavourComp::Composition)
    {
        ATH_MSG_ERROR("This method is only useable for FlavourComposition uncertainties, not " << getName().Data());
        return JESUNC_ERROR_CODE;
    }
    if (m_respType == FlavourResp_QUARK)
        return m_uncHist->getValue(pT,eta);
    else if (m_secondRespType == FlavourResp_QUARK)
        return m_secondUncHist->getValue(pT,eta);
    ATH_MSG_ERROR("Unexpected flavour response parametrization: " << getName().Data());
    return JESUNC_ERROR_CODE;
}


//////////////////////////////////////////////////
//                                              //
//  Private helper functions                    //
//                                              //
//////////////////////////////////////////////////

StatusCode FlavourUncertaintyComponent::readNjetsHistograms(std::vector<UncertaintyHistogram*>& hists, const std::vector<TString>& histKeys)
{
    for (size_t iKey = 0; iKey < histKeys.size(); ++iKey)
    {
        const TString histName = histKeys.at(iKey);
        int nJets = -1;
        if (getNjetFromKey(histName,nJets).isFailure())
            return StatusCode::FAILURE;
        if (nJets < 0 || nJets > static_cast<int>(histKeys.size())-1)
        {
            ATH_MSG_ERROR(Form("Unexpected gluon fraction nJet %d of index %zu: %s",nJets,iKey,histName.Data()));
            return StatusCode::FAILURE;
        }
        if (hists[nJets])
        {
            ATH_MSG_ERROR(Form("A histo for nJets of %d was already found, blocking double-creation of %s",nJets,histName.Data()));
            return StatusCode::FAILURE;
        }
        hists[nJets] = new UncertaintyHistogram(histName,m_interpolate);
    }
    return StatusCode::SUCCESS;
}

StatusCode FlavourUncertaintyComponent::getNjetFromKey(const TString& key, int& nJets) const
{
    std::vector<TString> tokens = utils::vectorize<TString>(key,"_");
    if (tokens.size() > 2 && tokens.at(tokens.size()-1).Contains("nJet",TString::kIgnoreCase))
    {
        TString nJetStr = tokens.at(tokens.size()-1).ReplaceAll("nJet","");
        nJetStr = nJetStr.ReplaceAll("njet","");
        nJetStr = nJetStr.ReplaceAll("Njet","");
        if (!utils::getTypeObjFromString<int>(nJetStr,nJets))
        {
            ATH_MSG_ERROR("Found nJets histogram, but failed to parse the index: " << key.Data());
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode FlavourUncertaintyComponent::checkNjetsInput(int& nJets) const
{
    // nJets = 0 is the inclusive composition
    // nJets = # is the composition for # jets
    
    // Case of no histograms is checked in initialization, no need to repeat here
    // Initialization also ensures gluonFractionHists and gluonFractionErrorHists are consistent

    // Take overflow from the highest bin
    // Note that this means the nJets=0 inclusive composition is used for ALL nJets values if the user did not provide nJets histograms (intended functionality)
    if (nJets > 0 && static_cast<size_t>(nJets) >= m_gluonFractionHists.size())
        nJets = m_gluonFractionHists.size() - 1;

    // Watch for missing histograms (users don't need to specify every nJet bin if they don't use them)
    if (!m_gluonFractionHists.at(nJets))
    {
        ATH_MSG_ERROR(Form("nJets of %d is NULL for %s",nJets,getName().Data()));
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

bool FlavourUncertaintyComponent::isBjet(const xAOD::Jet& jet) const
{
    // If not specified, assume it's not a b-jet
    if (!m_BjetAccessor.isAvailable(jet)) return false;

    // If it's available, return the value (now a char, so check non-equality with 0)
    return m_BjetAccessor(jet) != 0;
}

} // end jet namespace

