/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBRPCSECTORLOGIC_H
#define MUONCALIBRPCSECTORLOGIC_H

#include <string>
#include <vector>

namespace MuonCalib {

    /**
       Simplified class designed to store information from RpcSectorLogic. It has :
   */
    class MuonCalibRpcSectorLogic {
    public:
        struct defineParams {
            int sectorId{0};
            int fel1Id{0};
            int bcid{0};
            int errorCode{0};
            int crc{0};
            int hasMoreThan2TriggerCand{0};
            int slIndex{0};
            float triggerRates{0};
            int counters{0};
            int slIndex2{0};
            int rowinBcid{0};
            int padid{0};
            int ptid{0};
            int roi{0};
            int outerPlane{0};
            int overlapPhi{0};
            int overlapEta{0};
            int triggerBcid{0};
            int isInput{0};
        };
        MuonCalibRpcSectorLogic() = default;

        MuonCalibRpcSectorLogic(defineParams pars);  //!< constructor setting all members
        ~MuonCalibRpcSectorLogic() = default;        //!< destructor

        int sectorId() const;
        int fel1Id() const;
        int bcid() const;
        int errorCode() const;
        int crc() const;
        int hasMoreThan2TriggerCand() const;
        int slIndex() const;
        float triggerRates() const;
        int counters() const;
        int slIndex2() const;
        int rowinBcid() const;
        int padid() const;
        int ptid() const;
        int roi() const;
        int outerPlane() const;
        int overlapPhi() const;
        int overlapEta() const;
        int triggerBcid() const;
        int isInput() const;

    private:
        defineParams m_params;
    };

}  // namespace MuonCalib

#endif
