// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsolationCondition.h 635994 2014-12-15 14:37:46Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_ISOLATIONCONDITION_H
#define ELECTRONISOLATIONSELECTION_ISOLATIONCONDITION_H

#include <memory>

#include "PATCore/TAccept.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODEgamma/Electron.h"
#include "PATCore/TAccept.h"


// Forward Declaration(s)
class TF1;
class TH1D;
class TH3D;

namespace CP
{
  class IsolationCondition{
    public:
      IsolationCondition( xAOD::Iso::IsolationType type , const float& isLessThan , const bool& divideByPt );
      IsolationCondition( xAOD::Iso::IsolationType type , const std::string& isolationFunction , const std::shared_ptr<TH3D> efficiencyHisto3D ,
                          const std::shared_ptr<TH1D> etBins, const std::shared_ptr<TH1D> etaBins );

      virtual ~IsolationCondition(){}
      
      // Does the electron pass the isolation condition?
      Root::TAccept& accept( const xAOD::Electron& x ) const;
      
      // Get the cut value used
      float getCutValue( const xAOD::Electron& x ) const;
      
      // check validity of isolation condition
      bool checkValidity() const;
      
      // Get an object describing the "selection steps" of the tool
      inline const Root::TAccept& getTAccept() const {return m_accept;}  
      
      // Get the type of isolation
      inline xAOD::Iso::IsolationType isolationType() const {return m_isolationType;}
      
      // Get the isolation TAccept name 
      inline TString getTAcceptName() const {return m_acceptName;}
      
      // Set m_cutPositionInElectronIsolationSelectionToolTAccept
      inline void setCutPositionInElectronIsolationSelectionToolTAccept(const unsigned int& v){m_cutPositionInElectronIsolationSelectionToolTAccept = v;}
      
      // Get m_cutPositionInElectronIsolationSelectionToolTAccept
      inline unsigned int getCutPositionInElectronIsolationSelectionToolTAccept() const {return m_cutPositionInElectronIsolationSelectionToolTAccept;}
      
    protected:
      void setupTAccept();
      int findEtBin(const float& Et) const;
      int findEtaBin(const float& Eta) const;
      
    private:
      IsolationCondition();
      IsolationCondition(const IsolationCondition& rhs);
      IsolationCondition(const IsolationCondition&& rhs);
      void operator=(const IsolationCondition& rhs);
      
      xAOD::Iso::IsolationType m_isolationType;
      bool m_cutBasedIsolation;
      bool m_divideByPt;
      float m_isLessThanEqualTo;
      
      std::string m_isolationFunctionName;
      std::shared_ptr<TH3D> m_efficiencyHisto3D;
      std::shared_ptr<TH1D> m_etBins;
      std::shared_ptr<TH1D> m_etaBins;       
      std::shared_ptr<TF1>  m_isolationFunction;
      
      TString m_acceptName;
      unsigned int m_cutPositionInLocalTAccept;
      unsigned int m_cutPositionInElectronIsolationSelectionToolTAccept;
      mutable Root::TAccept m_accept;    
  };
}


#endif
