// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//             Code produced by D3PDMakerReader
//
//  author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
// -------------------------------------------------------------

#include "../TrigCostD3PD/NTUP_TRIGCOST.h"
//#include "../TrigCostD3PD/D3PDPerfStats.h"

ClassImp( D3PDReader::NTUP_TRIGCOST )

namespace D3PDReader {

   NTUP_TRIGCOST::NTUP_TRIGCOST()
      : D3PDObjectBase(),
        TrigCostHLT( fEntry, "TrigCostHLT_" ),
        trig_DB( fEntry, "trig_DB_" ),
        fEntry( 0 ),
        fFromInput( kTRUE ) {

   }

   NTUP_TRIGCOST::NTUP_TRIGCOST( Bool_t )
      : D3PDObjectBase(),
        TrigCostHLT( "TrigCostHLT_" ),
        trig_DB( "trig_DB_" ),
        fEntry( 0 ),
        fFromInput( kFALSE ) {

   }

   const char* NTUP_TRIGCOST::GetPrefix() const {

      Warning( "GetPrefix", "A prefix is not defined for this object" );
      return "";
   }

   void NTUP_TRIGCOST::SetPrefix( const char* ) {

      Warning( "SetPrefix", "A prefix is not defined for this object" );
      return;
   }

   void NTUP_TRIGCOST::ReadFrom( ::TTree* tree ) {

      if( ! fFromInput ) {
         Error( "ReadFrom", "Object can't read a D3PD. Use a different constructor!" );
         return;
      }

      TrigCostHLT.ReadFrom( tree );
      trig_DB.ReadFrom( tree );

      return;
   }

   void NTUP_TRIGCOST::WriteTo( ::TTree* tree ) {

      TrigCostHLT.WriteTo( tree );
      trig_DB.WriteTo( tree );

      return;
   }

   void NTUP_TRIGCOST::SetActive( ::Bool_t active, const ::TString& pattern ) {

      TrigCostHLT.SetActive( active, pattern );
      trig_DB.SetActive( active, pattern );

      return;
   }

   void NTUP_TRIGCOST::ReadAllActive() {

      TrigCostHLT.ReadAllActive();
      trig_DB.ReadAllActive();

      return;
   }

   // D3PDReadStats NTUP_TRIGCOST::GetStatistics() const {

   //    // The result object:
   //    D3PDReadStats result( D3PDPerfStats::Instance()->GetStats() );

   //    // Collect the statistics from all constituents:
   //    result += TrigCostHLT.GetStatistics();
   //    result += trig_DB.GetStatistics();

   //    return result;
   // }

   void NTUP_TRIGCOST::GetEntry( ::Long64_t entry ) {

      fEntry = entry;
      return;
   }

} // namespace D3PDReader
