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
	 m_ETA_LOW_MID_DELTA(0.075),
	 m_ETA_HIGH_MID_DELTA(0.035),
	 m_PHI_MID_DELTA(0.035),
	 m_ETA_LOW_INN_DELTA(0.15),
	 m_ETA_HIGH_INN_DELTA(0.10),
	 m_PHI_INN_DELTA(0.10)
      {};

      ~TgcDataPreparatorRoadParameters() {};

   public:
      inline double deltaEtaAtMiddleForLowPt()  const { return m_ETA_LOW_MID_DELTA; };
      inline double deltaEtaAtMiddleForHighPt() const { return m_ETA_HIGH_MID_DELTA; };
      inline double deltaPhiAtMiddle()          const { return m_PHI_MID_DELTA; };

      inline double deltaEtaAtInnerForLowPt()   const { return m_ETA_LOW_INN_DELTA; };
      inline double deltaEtaAtInnerForHighPt()  const { return m_ETA_HIGH_INN_DELTA; };
      inline double deltaPhiAtInner()           const { return m_PHI_INN_DELTA; };

   private:
      double m_ETA_LOW_MID_DELTA;
      double m_ETA_HIGH_MID_DELTA;
      double m_PHI_MID_DELTA;
      double m_ETA_LOW_INN_DELTA;
      double m_ETA_HIGH_INN_DELTA;
      double m_PHI_INN_DELTA;
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
