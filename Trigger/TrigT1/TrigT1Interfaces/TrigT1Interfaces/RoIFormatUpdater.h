// Dear emacs, this is -*- c++ -*-
#ifndef TRIGT1INTERFACES_ROIFORMATUPDATER_H
#define TRIGT1INTERFACES_ROIFORMATUPDATER_H

// System include(s):
#include <stdint.h>
#include <iostream>

namespace LVL1 {

   /**
    *  @short Tool for changing the format of RoI words
    *
    *         Quick and dirty hack to work around fact that there is no RoI
    *         format version at the moment. Hopefully this is a short-term
    *         fix which will be suppressed soon and never needed again.
    *
    * @author Alan Watson <Alan.Watson@cern.ch> 17-5-06
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class RoIFormatUpdater {

   public:
      /** Method to update EM/Tau RoI word */
      static unsigned int UpdateEMTauRoI( unsigned int RoIWord );
      /** Method to update Jet/Energy RoI word */
      static unsigned int UpdateJetEnergyRoI( unsigned int RoIWord );
    
   private:
      /** Method to identify (old-style) JetEnergy RoI type*/
      static unsigned int OldJetEnergyType( unsigned int RoIWord );
      /** Method to update Jet RoI word */
      static unsigned int UpdateJetRoI( unsigned int RoIWord );
      /** Method to update JetEt RoI word */
      static unsigned int UpdateJetEtRoI( unsigned int RoIWord );
      /** Methods to update EnergySum RoI words */
      static unsigned int UpdateEnergyRoI0( unsigned int RoIWord );
      static unsigned int UpdateEnergyRoI1( unsigned int RoIWord );
      static unsigned int UpdateEnergyRoI2( unsigned int RoIWord );

   }; // class RoIFormatUpdater

} // namespace LVL1

#endif // TRIGT1INTERFACES_ROIFORMATUPDATER_H
