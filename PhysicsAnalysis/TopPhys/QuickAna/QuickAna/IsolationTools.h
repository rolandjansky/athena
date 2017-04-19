/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ISOLATIONTOOLS_H_
#define QUICK_ANA__ISOLATIONTOOLS_H_

#include <QuickAna/Global.h>

#include <IsolationSelection/IsolationSelectionTool.h>

// Note: just a copy of the class in TopObjectSelectionTools, with minimal edits
//  -- Used for now to conveniently replicate the top group selections
//  -- We can of course design something better later
// Note: copied from TopObjectSelection-00-00- and then updated independently
//  -- Will probably get rid of this pretty soon

namespace ana {

/**
 * @brief A common base for implementing isolation cuts.  Probably these
 * isolation tools should be in individual files?  Time will tell.
 */
class IsolationBase {
public:
    ///Create
    IsolationBase();

    ///Destroy!
    virtual ~IsolationBase();

    /**
     * @brief For applying isolation cuts to the analysis (tight) lepton.
     *
     * Note that you must implement this otherwise it wont compile. Ha.
     *
     * @param p The lepton
     * @return True if it passes the isolation cuts, false otherwise.
     */
    virtual bool passSelection(const xAOD::IParticle& p) const = 0;

    /**
     * @brief For applying loose isolation cuts to the loose lepton.
     *
     * Note that you must implement this otherwise it wont compile. Ha.
     *
     * @param p The lepton
     * @return True if it passes the isolation cuts, false otherwise.
     */
    virtual bool passSelectionLoose(const xAOD::IParticle& p) const = 0;

    /**
     * @brief Because people like to know what isolation cuts they're applying.
     *
     * @param os Where to print things to, e.g. cout.
     */
    virtual void print(std::ostream& os) const = 0;
};

/**
 * @brief These should give you access to xAOD::Iso::topoetcone20, 30, 40
 * and ptcone20, 30, 40
 *
 * They are only correct in the derivations, they will be wrong in the primary
 * xAOD.
 *
 * It performs an absolute cut on the quantity you specify.  If you need more
 * fancy things, we should implement them.  Don't you want to look at using
 * RelativePTVarCone instead though?
 */
class AbsoluteIsolationDC14 : public IsolationBase {
public:
    /**
     * @brief Absolute cut on topoetcone or ptcone.
     *
     * Construct with something like:
     *
     * @code
     * new IsolationDC14(xAOD::Iso::topoetcone20, 5000.)
     * @endcode
     *
     * We didn't bother implementing the loose selection yet.  I left that as
     * an exercise for the reader.
     *
     * @param type I think xAOD::Iso::topoetcone20, 30, 40 and ptcone20, 30, 40
     * are allowed.
     * @param value A number in MeV.
     */
    AbsoluteIsolationDC14(const xAOD::Iso::IsolationType type, double value);
    bool passSelection(const xAOD::IParticle& p) const override final;
    bool passSelectionLoose(const xAOD::IParticle& p) const override final;
    void print(std::ostream& os) const override final;

private:
    ///What to cut on (topoetcone20, 30, 40 or ptcone20, 30, 40).
    xAOD::Iso::IsolationType m_type;

    ///The value, in MeV, to cut.
    double m_cutvalue;
};

/**
 * @brief Yay DC14 now has isolation! This should allow you to cut on the
 * isolation (ptvarcone) as a fraction of the pt of the lepton.
 */
class RelativePTVarCone : public IsolationBase {
public:
    /**
     * @brief Cut on the mini-isolation (aka ptvarcone) as a fraction of the
     * pt of the lepton.
     *
     * @param size Cone size (*10). i.e. 20, 30, 40 for the analysis
     * @param fraction For the tight (analysis) leptons
     * @param sizeLoose Cone size for the loose lepton definition
     * @param fractionLoose Fraction for the loose lepton definition
     */
    RelativePTVarCone(unsigned int size, double fraction, unsigned int sizeLoose, double fractionLoose);

    /**
     * @brief Does this particle pass the isolation cuts for the main analysis
     * definitions?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelection(const xAOD::IParticle& p) const override final;

    /**
     * @brief Does this particle pass the isolation cuts for the loose analysis
     * definitions (useful for background estimates)?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelectionLoose(const xAOD::IParticle& p) const override final;

    ///Come on, you really need me to tell you what this does?
    void print(std::ostream& os) const override final;

private:
    ///The name of the variable to access, e.g. ptvarcone20 formed in the constructor.
    std::string m_size;

    ///For the main analysis object selection
    double m_fraction;

    ///Name of the variable to access for the loose object definition
    std::string m_sizeLoose;

    ///For the objects used in the looser object selection (e.g. matrix method fakes)
    double m_fractionLoose;
};

/**
 * @brief Danilo's approximation of mini-isolation which has been calculated
 * in AnalysisTop and applied as a decoration to the objects.
 *
 * This gave you something to work with before the DC14 derivations included
 * isolation (which they should by the time you read this).  Probably other
 * options will be better, and almost certainly faster if we don't need to load
 * all the tracks!
 */
class ApproxPTVarCone : public IsolationBase {
public:
    /**
     * @brief An approximation of mini-isolation.
     *
     * This applies some sort of fractional cut to mini-iso / particle pT.
     *
     * @param fraction For the tight (analysis) leptons
     * @param fractionLoose For leptons that will be used in the background
     * estimate.
     */
    ApproxPTVarCone(double fraction, double fractionLoose);

    /**
     * @brief Does this particle pass the isolation cuts for the main analysis
     * definitions?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelection(const xAOD::IParticle& p) const override final;

    /**
     * @brief Does this particle pass the isolation cuts for the loose analysis
     * definitions (useful for background estimates)?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelectionLoose(const xAOD::IParticle& p) const override final;

    ///Come on, you really need me to tell you what this does?
    void print(std::ostream& os) const override final;

private:
    ///For the main analysis object selection
    double m_fraction;

    ///For the objects used in the looser object selection (e.g. matrix method fakes)
    double m_fractionLoose;
};

/**
 * @brief Apply the cuts to etcone and ptcone like we used to in Run-I
 */
class StandardIsolation : public IsolationBase {
public:
    /**
     * @brief Applies the cuts to etcone and ptcone like we used to in Run-I
     *
     * This sets up the isolation tool and configures it with the cuts.
     */
    StandardIsolation(const std::string& obj_type_str);

    /**
     * @brief Does this particle pass the isolation cuts for the main analysis
     * definitions?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelection(const xAOD::IParticle& p) const override final;

    /**
     * @brief Does this particle pass the isolation cuts for the loose analysis
     * definitions (useful for background estimates)?
     *
     * @param p Particle that we're worried about.
     * @return True if it passes the isolation cuts.
     */
    bool passSelectionLoose(const xAOD::IParticle& p) const override final;

    ///Come on, you really need me to tell you what this does?
    virtual void print(std::ostream& os) const override final;

private:
    ///Official isolation tool
    CP::IsolationSelectionTool m_isolationTool;
    CP::IsolationSelectionTool m_isolationToolLoose;
};

}

#endif
