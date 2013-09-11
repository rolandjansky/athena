/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPCONSTANTS_H
#define TRIGT1CTP_CTPCONSTANTS_H

#include <string>

namespace LVL1CTP {

  /**
   *
   *   @short Commonly used constants of the CTP simulation
   *
   *          It seemed reasonable to put all the contants in one place, so that it would be
   *          easier to change them later on.
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version $Id: CTPConstants.h,v 1.5 2008-02-01 17:20:18 efeld Exp $
   *
   */
  
  class CTPConstants {

  public:
    static const int MAXIMUM_ITEM_COUNT;                     //!< maximum number of items
    static const int MAXIMUM_THRESHOLD_COUNT;                //!< maximum number of thresholds

    static const std::string DEFAULT_MuonConfigLocation;     //!< location of legacy muon configuration in StoreGate
    static const std::string DEFAULT_CaloConfigLocation;     //!< location of legacy calo configuration in StoreGate
    static const std::string DEFAULT_JEConfigLocation;       //!< location of legacy jet energy configuration in StoreGate

  }; // class CTPConstants

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPCONSTANTS_H
