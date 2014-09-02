/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetQGPartonLabel.h
//   Header file for class JetQGPartonLabel
///////////////////////////////////////////////////////////////////
/**
 * <b>Purpose:</b> label jets with b or c quarks.<br>
 * <b>Author:</b> Jean-Baptiste de Vivie<br>
 * <b>Email:</b> devivie@cppm.in2p3.fr <br>
 * <b>Created:</b> 2005/02/25 <br>
 */

#ifndef PARTICLEJETTOOLS_JETQGPARTONLABEL_H
#define PARTICLEJETTOOLS_JETQGPARTONLABEL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ParticleJetTools/IJetTruthMatching.h"
#include <string>
#include <map>

class McEventCollection;
class Jet;

namespace Analysis
{

  class JetQGPartonLabel : public AthAlgTool,
                       virtual public IJetTruthMatching {
     public:
       JetQGPartonLabel(const std::string&,const std::string&,const IInterface*);
       virtual ~JetQGPartonLabel();
       StatusCode initialize();
       StatusCode finalize();
         
       /** Method to truth tag a jet.
	   NB: for this particular algorithm, matchJet is TRUE if the jet is matched 
	   to a b-quark OR a c-quark. The jetLabel is set accordingly and is no 
	   longer a job option. */
       virtual bool matchJet(const Jet& myJet);

       virtual void m_printParameterSettings();
       
       /** NEXT METHODS ARE ONLY ACCESSIBLE AFTER CASTING!! */
       
       /** Return barcode */
       int barcode() const;
       
       /** Return pdg to match */
       int pdgCode() const;
       
       /** Return the predefined name to label the jets passing the matching: */
       inline const std::string& jetLabel() const { return m_jetLabel; }

       double deltaRToParton() const;

       /** Get the number of MC Events in the McEventCollection: */
       inline int NEventInCollection() const { return m_NEventInCollection; }
       inline void EventSelection(short s) { m_inTime = s; }
       inline short EventSelection() const { return m_inTime; }

  private:
      std::string m_mcEventCollection; 
      double m_deltaRCut; //!< deltaR cut value of the cone matching (max distance between Jet axis and momentum of truth particel)
      double m_ptCut;     //!< pT cut for partons
      bool   m_noDoc;
      short  m_inTime;
      int    m_pdg;       //!< pdg code of the parton/baryon the jet has been matched to (which was closest)
      int    m_barcode;   //!< barcode of the matched parton (to be able to find the parton in the McEventColl)
      bool m_testJet(const Jet&, const McEventCollection*);
      std::string m_jetLabel; //!< label to use for matching jets
      int m_NEventInCollection;

      double m_maxEnergy;
      double m_matchPdgId;
      double m_matchBarcode;
      double m_matchDeltaR;

  };

  /** Return barcode */
  inline int JetQGPartonLabel::barcode()  const { return m_barcode; }
       
  /** Return pdg to match */
  inline int JetQGPartonLabel::pdgCode()  const { return m_pdg; }
       
}
#endif // TRUTHMATCHTOOLS_JETQGPARTONLABEL_H

