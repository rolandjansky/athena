/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_MomentumConservation_H
#define G4UserActions_MomentumConservation_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

/// User action to check for event-level momentum/energy violation
class MomentumConservation final: public UserActionBase {

  public:

   /// Constructor
   MomentumConservation(const std::string& type, const std::string& name, const IInterface* parent)
     : UserActionBase(type,name,parent), _sum_edep(0), _sum_eesc(0)
   {;}

   /// Check that primary and deposited/escaped energy agree
   virtual void EndOfEvent(const G4Event* anEvent) override;

   /// Sum the deposited/escaped energy at each step
   virtual void Step(const G4Step* aStep) override;

   virtual StatusCode queryInterface(const InterfaceID&, void**) override;


  private:

   /// Sum of energy deposited in the detector
   double _sum_edep;

   /// Sum of energy escaping the detector (volume = 0)
   double _sum_eesc;

};

#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaKernel/MsgStreamMember.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class MomentumConservation : public IEndEventAction, public ISteppingAction
  {
    public:

      MomentumConservation()
        : m_msg("MomentumConservation"), _sum_edep(0), _sum_eesc(0)
      {};

      virtual void endOfEvent(const G4Event*) override;
      virtual void processStep(const G4Step*) override;

    private:

      /// Log a message using the Athena controlled logging system
      MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }

      /// Private message stream member and access to evtstore
      mutable Athena::MsgStreamMember m_msg;

     /// Sum of energy deposited in the detector
     double _sum_edep;

     /// Sum of energy escaping the detector (volume = 0)
     double _sum_eesc;

  }; // class MomentumConservation

} // namespace G4UA

#endif
