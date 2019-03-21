// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         iRecCoordRoi.h  -  description
                            -------------------
   begin                : Tue Aug 12 2003
   email                : edward.moyse@cern.ch
***************************************************************************/

#ifndef TRIGT1INTERFACES_IRECCOORDROI_H
#define TRIGT1INTERFACES_IRECCOORDROI_H

/**
 *  @short Interface for all reconstructed RoI classes
 *
 *         This interface is implemented by all reconstructed RoI classes which
 *         specify an eta-phi position in the detector.
 *
 * @author E. Moyse
 *
 * $Revision: 187728 $
 * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
 */
class iRecCoordRoI {

public:
   iRecCoordRoI();
   virtual ~iRecCoordRoI() = default;
   /** override this method & return eta coord */
   virtual double eta() const = 0;
   /** override this method & return phi coord */
   virtual double phi() const = 0;
   /** override this method & return 32-bit RoI word */
   virtual unsigned int roiWord() const = 0;

}; // class iRecCoordRoI

#endif // TRIGT1INTERFACES_IRECCOORDROI_H
