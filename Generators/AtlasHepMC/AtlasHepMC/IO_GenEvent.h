/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_IOGENEVENT_H
#define ATLASHEPMC_IOGENEVENT_H
#ifdef HEPMC3
#include "HepMC3/Version.h"
#include "HepMC3/Reader.h"
#include "HepMC3/Writer.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/WriterAsciiHepMC2.h"
namespace HepMC
{
typedef HepMC3::WriterAsciiHepMC2   WriterAsciiHepMC2;
typedef HepMC3::ReaderAsciiHepMC2   ReaderAsciiHepMC2;
class IO_GenEvent {
public:
    IO_GenEvent( const std::string& filename="IO_GenEvent.dat",
                 std::ios::openmode mode=std::ios::out ) {
        m_writer=new  HepMC3::WriterAsciiHepMC2(filename);
    }
    IO_GenEvent( std::ostream & os ) {
        m_writer=new  HepMC3::WriterAsciiHepMC2(os);
    }
    ~IO_GenEvent() {
        if(m_writer) {
            m_writer->close();
            delete m_writer;
        }
    }
    void write_event( const HepMC3::GenEvent* evt ) {
        m_writer->write_event(*evt);
    }
private:
    HepMC3::WriterAsciiHepMC2 *m_writer;
};
}
#else
#include "HepMC/IO_GenEvent.h"
#endif
#endif
