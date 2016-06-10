/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TrigT1CTP/MsgWriter.h"

namespace LVL1CTP {

   MsgWriter* MsgWriter::m_instance = 0;

   MsgWriter* MsgWriter::instance() {

      if( !m_instance ) m_instance = new MsgWriter();

      return m_instance;

   }

   MsgWriter::~MsgWriter() {

      delete m_stream;

   }

   bool
   MsgWriter::msgLvl (const MSG::Level lvl) const
   {
      if (m_stream->level() <= lvl) {
         *m_stream << lvl;
         return true;
      } else {
         return false;
      }
   }

   void MsgWriter::write( MSG::Level type, const std::string& line ) {

      if(m_stream->level() <= type){
         *m_stream << type << line << endreq;
         return;
      }
      return;
   }

   void MsgWriter::setName( const std::string& name ) {

      if( m_stream ) delete m_stream;
      m_stream = new MsgStream( Athena::getMessageSvc(), name );
      return;

   }

   MsgWriter::MsgWriter() {

      m_stream = new MsgStream( Athena::getMessageSvc(), "CTPSimulation" );

   }

} // namespace LVL1CTP
