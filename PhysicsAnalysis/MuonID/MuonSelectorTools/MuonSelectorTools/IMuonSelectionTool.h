// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMuonSelectionTool.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef MUONSELECTORTOOLS_IMUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_IMUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

namespace CP {

   /// Interface for (a) muon selector tool(s)
   ///
   /// This is an example of how to define object selection in
   /// a tool.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class IMuonSelectionTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IMuonSelectionTool )

   public:
      /// Decide whether the muon in question is a "good muon" or not
      virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

      /// Decide whether the muon in question is a "good muon" or not
      virtual asg::AcceptData accept( const xAOD::Muon& mu ) const = 0;

      /// set the passes ID cuts variable of the muon 
      virtual void setPassesIDCuts( xAOD::Muon& mu ) const = 0;

      /// set the passes high pT cuts variable of the muon 
      virtual void setPassesHighPtCuts( xAOD::Muon& mu ) const = 0;

      /// set the passes low pT cuts variable of the muon
      //virtual void setPassesLowPtEfficiencyCuts( xAOD::Muon& mu ) const = 0;

      /// set the passes quality variable of the muon 
      virtual void setQuality( xAOD::Muon& mu ) const = 0;

      /// Returns true if the muon passes the standard MCP ID cuts. To set the value on the muon, instead call setPassesIDCuts(xAOD::Muon&) const
      virtual bool passedIDCuts(const xAOD::Muon&) const =0;

      /// Returns true if the muon passes a standardized loose preselection.
      virtual bool passedMuonCuts(const xAOD::Muon&) const =0;
      
      /// Returns true if the track particle passes the standard MCP ID cuts.
      virtual bool passedIDCuts(const xAOD::TrackParticle&) const=0;

      /// Returns true if the muon passes the standard MCP high pt cuts. To set the value on the muon, instead call setPassesHighPtCuts(xAOD::Muon&) const
      virtual bool passedHighPtCuts(const xAOD::Muon&) const =0;
     
      /// Returns true if the muon passes the standard MCP low pt cuts. To set the value on the muon, instead call setPassesHighPtCuts(xAOD::Muon&) const
      virtual bool passedLowPtEfficiencyCuts(const xAOD::Muon&) const =0;
      virtual bool passedLowPtEfficiencyCuts(const xAOD::Muon&, xAOD::Muon::Quality thisMu_quality) const =0;
     
      /// Returns true if a CB muon fails a pt- and eta-dependent cut on the relative CB q/p error   
      virtual bool passedErrorCutCB(const xAOD::Muon&) const=0;

      /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks 
      virtual bool isBadMuon(const xAOD::Muon&) const=0;

      /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
      virtual xAOD::Muon::Quality getQuality( const xAOD::Muon& mu ) const =0;

     /// Returns true if the muon passes additional calo-tag quality cuts
     virtual bool passedCaloTagQuality (const xAOD::Muon& mu) const = 0;

   }; // class IMuonSelectionTool

} // namespace CP

#endif // CPTOOLTESTS_IMUONSELECTIONTOOL_H
