/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgWriter.cxx 796122 2017-02-08 03:23:15Z ssnyder $

#ifdef STANDALONE
#include <iostream>
#else
#include "AthenaKernel/getMessageSvc.h"
#endif // STANDALONE

// Local include(s):
#include "MsgWriter.h"

//***********************************************************************
//
//       Version : $Revision: 796122 $
//
//   Description : Implementation of the MsgWriterSingleton class
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2017-02-08 04:23:15 +0100 (Wed, 08 Feb 2017) $
//
//***********************************************************************

namespace LVL1MUCTPI {

  MsgWriter* MsgWriter::m_instance = 0;

  MsgWriter* MsgWriter::instance() {

    if( ! m_instance ) {

      m_instance = new MsgWriter();

    }

    return m_instance;

  }

  MsgWriter::~MsgWriter() {

#ifndef STANDALONE
    delete m_stream;
#endif // STANDALONE

  }

  void MsgWriter::write( MsgType type, std::string line ) {

#ifdef STANDALONE
    if( type < m_min_type ) return;
    std::cout << "  (" << m_type_map[ type ] << ")  " << line << std::endl;
#else
    *m_stream << m_type_map[ type ] << line << endmsg;
#endif // STANDALONE

  }

#ifdef STANDALONE
  void MsgWriter::setMinType( MsgType type ) {
    m_min_type = type;
  }
  bool MsgWriter::msgLvl(MsgType type) const {
    return type >= m_min_type;
  }
  bool MsgWriter::msgLvl( unsigned int type ) const {
    return msgLvl(MsgType(type));
  }
#else
  void MsgWriter::setMinType( MSG::Level type ) {
    m_stream->setLevel( type );
  }
  bool MsgWriter::msgLvl( MSG::Level type ) const {
    return type >= m_stream->level();
  }
  bool MsgWriter::msgLvl( unsigned int type ) const {
    return msgLvl(MSG::Level(type));
  }
  void MsgWriter::setSource( const std::string& source ) {
    delete m_stream;
    m_stream = new MsgStream( Athena::getMessageSvc(), source );
  }
#endif // STANDALONE

  MsgWriter::MsgWriter() {

#ifdef STANDALONE
    m_type_map[ VERBOSE ] = "VERBOSE";
    m_type_map[ DEBUG ]   = " DEBUG ";
    m_type_map[ INFO ]    = " INFO  ";
    m_type_map[ WARNING ] = "WARNING";
    m_type_map[ ERROR ]   = " ERROR ";
    m_type_map[ FATAL ]   = " FATAL ";
    m_type_map[ ALWAYS ]  = "ALWAYS ";

    m_min_type = INFO;
#else
    m_stream = new MsgStream( Athena::getMessageSvc(), "L1Muctpi" );
    m_type_map[ VERBOSE ] = MSG::VERBOSE;
    m_type_map[ DEBUG ]   = MSG::DEBUG;
    m_type_map[ INFO ]    = MSG::INFO;
    m_type_map[ WARNING ] = MSG::WARNING;
    m_type_map[ ERROR ]   = MSG::ERROR;
    m_type_map[ FATAL ]   = MSG::FATAL;
    m_type_map[ ALWAYS ]  = MSG::ALWAYS;
#endif // STANDALONE

  }

} // namespace LVL1MUCTPI
