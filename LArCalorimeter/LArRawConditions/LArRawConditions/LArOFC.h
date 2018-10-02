/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFC
#define LARRAWCONDITIONS_LAROFC

#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

#include "LArElecCalib/ILArOFC.h"

#include <vector>
#include <array>

class LArOFC : public ILArOFC {

    public:
        LArOFC(const LArOnlineID_Base* onlineID,
               const LArOnOffIdMapping* cabling,
	           const size_t nGains);

        virtual ~LArOFC();

        bool setOFC(const IdentifierHash& hid, const int gain, std::pair<std::vector<float>,std::vector<float>> ofcab);

        virtual ILArOFC::OFCRef_t OFC_a(const HWIdentifier& id,
                                        int gain,
                                        int tbin=0) const;

        virtual ILArOFC::OFCRef_t OFC_b(const HWIdentifier& id,
                                        int gain,
                                        int tbin=0) const;

        virtual ILArOFC::OFCRef_t OFC_a(const Identifier& id,
                                        int gain,
                                        int tbin=0) const;
        virtual ILArOFC::OFCRef_t OFC_b(const Identifier& id,
                                        int gain,
                                        int tbin=0) const;
        //FIXME: is used, for data
        virtual float timeOffset(const HWIdentifier& id, int gain) const;
        virtual float timeOffset(const Identifier&  CellID, int gain) const;
        virtual unsigned nTimeBins(const HWIdentifier& id, int gain) const;
        virtual unsigned nTimeBins(const Identifier&  CellID, int gain) const;
        virtual float timeBinWidth(const HWIdentifier& id, int gain) const;
        virtual float timeBinWidth(const Identifier&  CellID, int gain) const;

    private:
        // dimensions n x m
        // n: number of IDs
        // m: number of OFCs
        typedef std::vector<std::vector<float> > vecOFCa;
        typedef std::vector<std::vector<float> > vecOFCb;

        // dimensions k x (n x m)
        // k: 3 (value of enum LARNGAIN)
        std::array<vecOFCa, CaloGain::LARNGAIN> m_OFCa;
        std::array<vecOFCa, CaloGain::LARNGAIN> m_OFCb;

        const LArOnlineID_Base* m_onlineID;
        const LArOnOffIdMapping* m_cabling;

};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArOFC, 71011311, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (LArOFC, 50867161, ILArOFC);

#endif
