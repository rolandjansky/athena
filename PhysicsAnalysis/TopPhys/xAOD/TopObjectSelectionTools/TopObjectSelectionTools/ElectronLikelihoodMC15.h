/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONLIKELIHOODMC15_H_
#define ELECTRONLIKELIHOODMC15_H_

#include "TopObjectSelectionTools/ElectronSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include <memory>

namespace top {

  /**
  * @brief To select electrons based on the "likelihood" definition.
  */
  class ElectronLikelihoodMC15 : public top::ElectronSelectionBase {
    public:
      /**
      * @brief Cut on likelihood electrons
      *
      * @param ptcut The minimum pT electrons should have
      * @param vetoCrack Do you want to veto 1.37 < |cluster_eta| < 1.52?
      * @param operatingPoint Likelihood operating point for the main object
      * definition
      * @param operatingPointLoose Likelihood operating point for the loose
      * objects (fakes estimates?)
      * @param isolation The isolation tool the user wants to use.  If you don't 
      * want any isolation cuts to be applied then leave this as a nullptr.
      */
      ElectronLikelihoodMC15(const double ptcut, const bool vetoCrack, const std::string& operatingPoint,
                             const std::string& operatingPointLoose, StandardIsolation* isolation, const bool applyTTVACut=true);
      // this constructor is kept for backward compatibility - isPrimaryxAOD is not needed anymore
      ElectronLikelihoodMC15(const bool,
                             const double ptcut, const bool vetoCrack, const std::string& operatingPoint,
                             const std::string& operatingPointLoose, StandardIsolation* isolation, const bool applyTTVACut=true);
      virtual ~ElectronLikelihoodMC15(){}
      /**
      * @brief Selection for the main analysis (i.e. tight object definitions).
      *
      * @param el The electron in question
      * @return True if it passes the selection, false otherwise.
      */
      virtual bool passSelection(const xAOD::Electron& el) const override;

      /**
      * @brief Selection for the loose object definitions (e.g. in fakes
      * estimates).
      *
      * @param el The electron in question
      * @return True if it passes the selection, false otherwise.
      */
      virtual bool passSelectionLoose(const xAOD::Electron& el) const override;

      /**
       * @brief The BLayer cuts for the loose lepton LH WP.
       *
       * @param el
       * @return True if passes, false otherwise
       */
      virtual bool passBLayerCuts(const xAOD::Electron& el) const;


      /**
       * @brief The track-to-vertex association (TTVA) cuts.
       *
       * @param el
       * @return True if passes, false otherwise
       */
       virtual bool passTTVACuts(const xAOD::Electron& el) const;
    
      /**
       * @brief The charge flip selector cut
       *
       * @param el
       * @return True if passes, false otherwise
       */
       virtual bool passChargeIDCut(const xAOD::Electron& el) const;

      /**
      * @brief Why would you not want to know what you're running?
      *
      * @param os Print details of the cuts to the output stream.
      */
      virtual void print(std::ostream& os) const override;

    protected:
      /**
      * @brief The loose and tight selections are the same, except they have different
      * use isolation and a different likelihood tool.
      *
      * @param el The electron in question.
      * @param likelihoodTool The tool to use (m_likelihood, or m_likelihoodLoose)
      * @return True if it's good, False if it's not
      */
      bool passSelectionNoIsolation(const xAOD::Electron& el, const std::string& operatingPoint_DF, const std::string& operatingPoint) const;
      
      ///Minimum pT that electrons should have
      double m_ptcut;

      ///Veto the crack region?
      bool m_vetoCrack;
    
      std::string m_operatingPoint;
      std::string m_operatingPointLoose;
      std::string m_operatingPoint_DF;
      std::string m_operatingPointLoose_DF;

      ///The isolation tool, or nullptr if we don't want isolation
      std::unique_ptr<top::StandardIsolation> m_isolation;

      /// decide if TTVA cut should be applied at all
      bool m_applyTTVACut;

      /// decide to apply the charge ID selector tool
      bool m_applyChargeIDCut;
  };

}

#endif
