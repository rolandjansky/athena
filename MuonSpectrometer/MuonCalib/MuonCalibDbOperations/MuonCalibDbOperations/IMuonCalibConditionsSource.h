/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_IMuonCalibConditionsSource
#define MuonCalib_IMuonCalibConditionsSource

// Gaudi
#include "GaudiKernel/AlgTool.h"

namespace MuonCalib {

    class CoolInserter;

    static const InterfaceID IID_IMuonCalibConditionsSource("MuonCalib::IMuonCalibConditionsSource", 1, 0);

    class IMuonCalibConditionsSource : virtual public IAlgTool {
    public:
        /** destructor */
        inline virtual ~IMuonCalibConditionsSource() {}
        /** interface method */
        static const InterfaceID& interfaceID() { return IID_IMuonCalibConditionsSource; }
        /** insert calibration */
        inline bool InstertCalibration(CoolInserter* inserter, bool store_t0, bool store_rt) {
            m_inserter = inserter;
            return insert_calibration(store_t0, store_rt);
        }
        inline virtual void SetIOV(int& /*start*/, int& /*end*/) const {}

    protected:
        CoolInserter* m_inserter;
        virtual bool insert_calibration(bool store_t0, bool store_rt) = 0;
    };

}  // namespace MuonCalib

#endif
