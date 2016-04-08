/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Persistent representation of TBEvent/TBMWPCCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBMWPCCONT_P1_H
#define TBMWPCCONT_P1_H


class TBMWPCCont_p1
{

public:

  // reconstructed (c,y)
  std::vector < std::vector<float> > m_cPos;
  std::vector < std::vector<float> > m_cErr;

  std::vector < bool > m_isX;

  // variables used in 2002 HEC+EMEC from LArTBBeamChambers :
  std::vector < std::vector<float> > m_clusterSize_c;     // < half with of cluster (c)  (cm)

  // overflow indicators
  // ------------------------------------------------------------------------------------------------
  // for some reason a stright forward definition of m_cPosOverflow as
  // std::vector < std::vector<bool> > m_cPosOverflow;
  // does not work. perhaps due to the fact that std::vector<bool> is nonconforming (not
  // really a vector of bool). instead we defined
  // std::vector < std::vector<unsigned> > m_cPosOverflow;
  // and will need to convert between the bool and unsigned constituents of the vectors later on
  // ------------------------------------------------------------------------------------------------
  std::vector < std::vector<unsigned> > m_cPosOverflow;
  // ------------------------------------------------------------------------------------------------

  std::vector < std::string > m_tbDetectorName;  // from TBBeamDetector.h
  std::vector < bool > m_overflow;               // from TBBeamDetector.h

};


#endif
