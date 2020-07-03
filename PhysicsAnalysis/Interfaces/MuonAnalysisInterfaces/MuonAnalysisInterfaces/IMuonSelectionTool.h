/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

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
   class IMuonSelectionTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IMuonSelectionTool )

   public:

      ///Enum for resolution categories, the "merged" categories Zero to Four group together categories with similar resolution
      enum ResolutionCategory {
	unclassified=-1, missingInner=1, missingMiddle=1<<1, missingOuter=1<<2, highPt2station=1<<3, highPt=1<<4,
	spoiledCSC=1<<5, BEoverlap=1<<6, BIS78=1<<7, missingBEE=1<<8, oneStation=1<<9,
	CategoryZero = missingInner | spoiledCSC | BEoverlap | BIS78 | missingBEE, 
	CategoryOne = missingMiddle | oneStation, 
	CategoryTwo = missingOuter, 
	CategoryThree = highPt2station, 
	CategoryFour = highPt
      };

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
     
      /// Returns true if a CB muon passes a pt- and eta-dependent cut on the relative CB q/p error   
      virtual bool passedErrorCutCB(const xAOD::Muon&) const=0;

      /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks 
      virtual bool isBadMuon(const xAOD::Muon&) const=0;

      /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
      virtual xAOD::Muon::Quality getQuality( const xAOD::Muon& mu ) const =0;

     /// Returns true if the muon passes additional calo-tag quality cuts
     virtual bool passedCaloTagQuality (const xAOD::Muon& mu) const = 0;

     /// Returns an integer corresponding to categorization of muons with different resolutions
     virtual int getResolutionCategory(const xAOD::Muon&) const=0;

   }; // class IMuonSelectionTool

} // namespace CP

#endif // CPTOOLTESTS_IMUONSELECTIONTOOL_H
