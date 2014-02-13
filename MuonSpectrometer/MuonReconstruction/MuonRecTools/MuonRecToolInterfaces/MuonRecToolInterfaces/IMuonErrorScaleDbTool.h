/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONERRORSCALEDBTOOL_H
#define MUON_IMUONERRORSCALEDBTOOL_H

#include "Identifier/Identifier.h"

static const InterfaceID IID_IMuonErrorScaleDbTool
    ("Muon::IMuonErrorScaleDbTool",1,0);


namespace Muon {

  /** 
      @file IMuonErrorScaleDbTool.h
      @class IMuonErrorScaleDbTool

      @brief Interface for tools used to retrieve constants from conditions database used for error scaling of muon hits.

      @author Robert Harrington <roberth@bu.edu>
      @date 20 May 2010
   */

  class IMuonErrorScaleDbTool : virtual public IAlgTool {
    
  public:
    
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    /** methods to indicate whether constants are available for error scaling of subsystems. */
    virtual bool doMdt() const = 0;
    virtual bool doTgc() const = 0;
    virtual bool doRpc() const = 0;
    virtual bool doCsc() const = 0;

    virtual std::vector<double> errScale(const Identifier id) const = 0;

    /** dump the scaling parameters to MsgStream output. */
    virtual MsgStream& dump( MsgStream& ) const = 0;
  };
  
  inline const InterfaceID& IMuonErrorScaleDbTool::interfaceID()
  {
    return IID_IMuonErrorScaleDbTool;
  }

  MsgStream& operator << (MsgStream&, const IMuonErrorScaleDbTool&);

  inline MsgStream& operator << (MsgStream& sl, const IMuonErrorScaleDbTool& se)
  {
    return se.dump(sl);
  }

} // end of name space


#endif // MUON_IMUONERRORSCALEDBTOOL_H
