// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1MUCTPI_MIOCTL1TOPOCONVERTER_H
#define TRIGT1MUCTPI_MIOCTL1TOPOCONVERTER_H


// Local include(s):
#include "../Mioct/Sector.h"
#include "../Mioct/MioctID.h"
#include "../Logging/MsgLogger.h"
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"
#include "TrigConfMuctpi/MuctpiXMLParser.h"

namespace LVL1MUCTPI {

   /**
    *  @short Convert candidates of a Mioct to L1Topo output via NIM
    *
    *         This is a converter class that takes the candidates within
    *         a Mioct and creates the outputs plus decoding for L1Topo
    *
    * $author Thorsten Wengler 
    *
    */
   class MioctL1TopoConverter {

   public:
     //constructor 
      MioctL1TopoConverter();
      ~MioctL1TopoConverter();

     void setupParser(const std::string & inputfile);

     //Aquire the muon candidates of this mioct after overlap handling and vetos
     LVL1::MuCTPIL1TopoCandidate convertToL1Topo(const MioctID& mioctModuleID, const Sector& sector, bool isFirstCandidate);
     

   private:
     mutable MsgLogger   m_logger;
     MuctpiXMLParser m_xmlParser;

   }; // class MioctL1TopoConverter

} // namespace LVL1MUCTPI

 #endif // TRIGT1MUCTPI_MIOCTL1TOPOCONVERTER_H
