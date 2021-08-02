/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTSBASE_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTSBASE_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODBase/IParticle.h"
#include "AthenaKernel/MsgStreamMember.h"

class IHistogramDefinitionSvc;
class SingleHistogramDefinition;

class TCCPlotsBase:public PlotBase {
    public:
      TCCPlotsBase(PlotBase* pParent, const std::string& folder);
      
      /// Retrieve a single histogram definition, given the unique string identifier
      SingleHistogramDefinition retrieveDefinition(const std::string& histoIdentifier,
						   const std::string& folder = "default");
      
      /// Book a TH1 histogram
      void book(TH1*& pHisto, const SingleHistogramDefinition& hd);
      /// Book a TH1 histogram with optional folder name
      void book(TH1*& pHisto, const std::string& histoIdentifier, const std::string& folder = "default");
            
      /// Book a 2D histogram (TH2)
      void book(TH2*& pHisto, const SingleHistogramDefinition& hd);
      /// Book a 2D histogram (TH2) with optional folder name
      void book(TH2*& pHisto, const std::string& histoIdentifier, const std::string& folder = "default");
      
      /// Book a 3D histogram (TH3)
      void book(TH3*& pHisto, const SingleHistogramDefinition& hd);
      /// Book a 3D histogram (TH3) with optional folder name
      void book(TH3*& pHisto, const std::string& histoIdentifier, const std::string& folder = "default");

      //
      static void fillHisto(TH1* pTh1, const float value);
      static void fillHisto(TH1* pTh1, const float value, const float weight);
      static void fillHisto(TH2* pTh2, const float xval, const float yval);
      static void fillHisto(TH2* pTh2, const float xval, const float yval, const float weight);
      static void fillHisto(TH3* pTh3, const float xval, const float yval, const float zval);
      static void fillHisto(TH3* pTh3, const float xval, const float yval, const float zval, const float weight);

      std::string m_folder;
            
    protected:
      /// Log a message using the Athena controlled logging system
      MsgStream&
      msg(MSG::Level lvl) const {
        return m_msg.get() << lvl;
      }
    
      /// Check whether the logging system is active at the provided verbosity level
      bool
      msgLvl(MSG::Level lvl) {
        return m_msg.get().level() <= lvl;
      }
    
      /// Private message stream member
      mutable Athena::MsgStreamMember m_msg;
      
    private:
      IHistogramDefinitionSvc* m_histoDefSvc;

};

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTSBASE_H
