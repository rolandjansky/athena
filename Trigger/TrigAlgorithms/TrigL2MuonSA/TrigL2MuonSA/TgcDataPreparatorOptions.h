/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOROPTIONS_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOROPTIONS_H

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcDataPreparatorRoadParameters
{
   public:
      TgcDataPreparatorRoadParameters() :
	 ETA_LOW_MID_DELTA(0.075),
	 ETA_HIGH_MID_DELTA(0.035),
	 PHI_MID_DELTA(0.035),
	 ETA_LOW_INN_DELTA(0.15),
	 ETA_HIGH_INN_DELTA(0.10),
	 PHI_INN_DELTA(0.10)
      {};

      ~TgcDataPreparatorRoadParameters() {};

   public:
      inline double deltaEtaAtMiddleForLowPt()  const { return ETA_LOW_MID_DELTA; };
      inline double deltaEtaAtMiddleForHighPt() const { return ETA_HIGH_MID_DELTA; };
      inline double deltaPhiAtMiddle()          const { return PHI_MID_DELTA; };

      inline double deltaEtaAtInnerForLowPt()   const { return ETA_LOW_INN_DELTA; };
      inline double deltaEtaAtInnerForHighPt()  const { return ETA_HIGH_INN_DELTA; };
      inline double deltaPhiAtInner()           const { return PHI_INN_DELTA; };

   private:
      double ETA_LOW_MID_DELTA;
      double ETA_HIGH_MID_DELTA;
      double PHI_MID_DELTA;
      double ETA_LOW_INN_DELTA;
      double ETA_HIGH_INN_DELTA;
      double PHI_INN_DELTA;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcDataPreparatorOptions
{
   public:
      TgcDataPreparatorOptions() : m_isOnTimeOnly(true), m_roadParameters() {};
      ~TgcDataPreparatorOptions() {};

      inline bool isOnTimeOnly() const { return m_isOnTimeOnly; };
      inline const TgcDataPreparatorRoadParameters& roadParameters() const { return m_roadParameters; };

   private:
      bool  m_isOnTimeOnly;
      TgcDataPreparatorRoadParameters m_roadParameters;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOROPTIONS_H
