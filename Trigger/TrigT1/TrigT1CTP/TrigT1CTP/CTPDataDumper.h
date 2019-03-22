/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPDATADUMPER_H
#define TRIGT1CTP_CTPDATADUMPER_H

// STL include(s):
#include <string>

// Basic includes:
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Namespace for the CTP related classes
namespace LVL1CTP {

  /**
   *
   *   @short CTP Data Dumper for CTP releated objects
   *
   *          Dumps object content of CTP related object:
   *          CTPSLink, CTP_RDO, EventInfo
   *
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: CTPDataDumper.h,v 1.5 2009-01-29 21:14:24 efeld Exp $
   *
   */

  class CTPDataDumper : public AthAlgorithm {

  public:

    CTPDataDumper( const std::string& name, ISvcLocator* pSvcLocator );
    ~CTPDataDumper();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    // dumper methods
    StatusCode dumpCTP_RDO();                               //!< dump content of CTP_RDO object
    StatusCode dumpCTPSLink();                              //!< dump content of CTPSLink object
    StatusCode dumpEventInfo();                             //!< dump content of EventInfo object

    // Properties: stearing flags
    bool        m_rawFormat;                                //!< property, see @link CTPDataDumper::CTPDataDumper @endlink
    bool        m_prettyFormat;                             //!< property, see @link CTPDataDumper::CTPDataDumper @endlink

    // Properties: StoreGate location of output
    std::string m_roiLoc;                                   //!< property, see @link CTPDataDumper::CTPDataDumper @endlink
    std::string m_rdoLoc;                                   //!< property, see @link CTPDataDumper::CTPDataDumper @endlink

  }; // class CTPDataDumper

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPDATADUMPER_H
