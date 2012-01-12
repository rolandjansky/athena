/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUDETECTORSTATUS_H_
#define  _TRIGTAUDETECTORSTATUS_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauDetectorStatus
/// Package : TauTrigPerformance
/// Author  : Soshi Tsuno (KEK)
/// Created : June 2009
///
/// DESCRIPTION:
///         Store detector status
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TObject.h"
#include <vector>

class TrigTauDetectorStatus: public TObject
{
 public:
  TrigTauDetectorStatus();
  TrigTauDetectorStatus(const TrigTauDetectorStatus & other);
  virtual ~TrigTauDetectorStatus();
  virtual void reset();

  enum DetectorStatusFlag { 
    BLACK  = 0, // Disabled
    RED    = 1, // Bad
    YELLOW = 2, // Use with caution
    GREEN  = 3, // Good
    WHITE  = 4, // Enabled
    BLUE   = 5  // Undefined
  };

  // Based on 15.2.0
  enum DetectorElement { 
    PIXB = 101, PIX0 = 102, PIXEA = 104, PIXEC = 105,
    SCTB = 111, SCTEA = 114, SCTEC = 115,
    TRTB = 121, TRTEA = 124, TRTEC = 125,
    IDGL = 130, IDGB = 131, IDGEA = 134, IDGEC = 135,
    IDAL = 140, IDAB = 141, IDAEA = 144, IDAEC = 145,
    IDBS = 150,
    IDPF = 160,
    IDBCM = 170,
    EMBA = 202, EMBC = 203, EMECA = 204, EMECC = 205,
    HECA = 214, HECC = 215, FCALA = 224, FCALC = 225,
    TIGB = 230,
    TILBA = 232, TILBC = 233, TIEBA = 234, TIEBC = 235,
    CALBA = 251, CALEA = 254, CALEC = 255,
    MDTBA = 302, MDTBC = 303, MDTEA = 304, MDTEC = 305,
    RPCBA = 312, RPCBC = 313,
    TGCEA = 324, TGCEC = 325,
    CSCEA = 334, CSCEC = 335,
    LCD = 350, LCDA = 353, LCDC = 354,
    ALFA = 360, ZDC = 370,
    L1CAL = 401, L1MUB = 402, L1MUE = 403, L1CTP = 404,
    TRCAL = 411,
    TRBJT = 421, TRBPH = 422, TRCOS = 423, TRELE = 424,
    TRGAM = 425, TRJET = 426, TRMET = 427, TRMBI = 428,
    TRMUO = 429, TRTAU = 430, TRIDT = 431,
    LUMI = 450,
    EIDB = 501, EIDEA = 504, EIDEC = 505,
    MIDB = 511, MIDEA = 514, MIDEC = 515,
    JETB = 521, JETEA = 524, JETEC = 525,
    MET = 530,
    BTGB = 541, BTGEA = 544, BTGEC = 545,
    TAUB = 551, TAUEA = 554, TAUEC = 555
  };

  std::vector<int> m_DetectorStatus;
  std::vector<int> m_DetectorElement;

//  TrigTauDetectorStatus::DetectorStatusFlag GetDetectorStatus(int LumiBlock, TrigTauDetectorStatus::DetectorElement detectorID);

 private:
  ClassDef(TrigTauDetectorStatus, 7);
};

#endif //TRIGTAUDETECTORSTATUS_H
