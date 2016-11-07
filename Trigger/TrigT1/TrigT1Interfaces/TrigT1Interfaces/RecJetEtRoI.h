// Dear emacs, this is -*- c++ -*-
// $Id: RecJetEtRoI.h 782811 2016-11-07 17:20:40Z smh $
/***************************************************************************
                         RecJetEtRoI.h  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   copyright            : (C) 2001 by moyse
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TRIGT1INTERFACES_RECJETETROI_H
#define TRIGT1INTERFACES_RECJETETROI_H

// Gaudi/Athena include(s):
#include "AthContainers/DataVector.h"

namespace LVL1 {

   /** This class defines the reconstructed jetEt ROI. It is generated from the
       Slink output of TrigT1Calo

       @author Edward Moyse
   */
   class RecJetEtRoI {

   public:
      // constructor
      RecJetEtRoI() {}
      // constructor
      RecJetEtRoI( unsigned int RoIWord );
      // destructor
      ~RecJetEtRoI();

      // constructor
      void construct( unsigned int RoIWord );

      /** returns roi word*/
      unsigned int roiWord() const;

   private:
      /**roiWord - defined as
         0010 | 24*0 | 4b Jet ET*/
      unsigned int m_roiWord { 0 };
      /** 4b word that contains JetET energy (in GeV)*/
      unsigned int m_energy { 0 };

   }; // class RecJetEtRoI

} // namespace LVL1

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( DataVector< LVL1::RecJetEtRoI >, 6260, 1 )
CLASS_DEF( LVL1::RecJetEtRoI, 6261, 1 )

#endif // TRIGT1INTERFACES_RECJETETROI_H
