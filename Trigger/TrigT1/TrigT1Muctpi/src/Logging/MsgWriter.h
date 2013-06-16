// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgWriter.h 472835 2011-12-06 21:05:16Z stelzer $
#ifndef TRIGT1MUCTPI_MSGWRITER_H
#define TRIGT1MUCTPI_MSGWRITER_H

// STL include(s):
#include <string>
#include <map>

#ifndef STANDALONE
// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#endif // STANDALONE

// Local include(s):
#include "MsgType.h"

namespace LVL1MUCTPI {

  /**
   * @short   Singleton class responsible for showing messages
   *
   *          The MuCTPI simulation has to work in two environments at
   *          this time. As part of the ATLAS digitization in Athena,
   *          and in a stand-alone mode in the MuCTPI DVS tests. In
   *          Athena all message logging should be done via the MsgStream
   *          class, while in the tests the standard output is used.
   *
   *          This class is used to make sure the output is redirected to
   *          the correct place.
   *
   * @see     MsgLogger
   * @author  $Author: krasznaa $
   * @version $Revision: 472835 $
   */
  class MsgWriter {

  public:
    /// Singleton method for requesting the one and only object.
    static MsgWriter* instance();
    ~MsgWriter();

    /// Write the message given in 'line' having a priority defined by 'type'.
    void write( MsgType type, std::string line );

    /// Set the minimal type of the messages to display.
#ifdef STANDALONE
    void setMinType( MsgType type );
    bool msgLvl(MsgType type) const;
#else
    void setMinType( MSG::Level type );
    void setSource( const std::string& source );
    bool msgLvl( MSG::Level type ) const;
#endif // STANDALONE

    bool msgLvl( unsigned int type ) const;

  protected:
    /// Singleton design, the constructor is protected.
    MsgWriter();

  private:
    /// The pointer to the only instance of the object.
    static MsgWriter* m_instance;
    // The two different environments need different variables:
#ifdef STANDALONE
    std::map< MsgType, std::string > m_type_map;
    MsgType m_min_type;
#else
    MsgStream* m_stream;
    std::map< MsgType, MSG::Level > m_type_map;
#endif // STANDALONE

  }; // class MsgWriter

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MSGWRITERSINGLETON_H
