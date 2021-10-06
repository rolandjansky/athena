/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCORFUNCSET_H
#define MDTCORFUNCSET_H

namespace MuonCalib {

    class IMdtSlewCorFunc;
    class IMdtBFieldCorFunc;
    class IMdtTempCorFunc;
    class IMdtBackgroundCorFunc;
    class IMdtWireSagCorFunc;

    /** Class which holds all correction functions for a given region.
        The segmentation can differ from the rt calibration region */
    class MdtCorFuncSet {
    public:
        MdtCorFuncSet();

        MdtCorFuncSet(IMdtSlewCorFunc* s, IMdtBFieldCorFunc* bf, IMdtTempCorFunc* t, IMdtBackgroundCorFunc* bg, IMdtWireSagCorFunc* w);

        ~MdtCorFuncSet();

        const IMdtSlewCorFunc* slewing() const { return m_slewing; }
        const IMdtBFieldCorFunc* bField() const { return m_bField; }
        const IMdtTempCorFunc* temperature() const { return m_temperature; }
        const IMdtBackgroundCorFunc* background() const { return m_background; }
        const IMdtWireSagCorFunc* wireSag() const { return m_wireSag; }

        void setSlewing(IMdtSlewCorFunc* slew);
        void setBField(IMdtBFieldCorFunc* bField);
        void setTemperature(IMdtTempCorFunc* temperature);
        void background(IMdtBackgroundCorFunc* background);
        void wireSag(IMdtWireSagCorFunc* wireSag);

    private:
        /** no assignment */
        MdtCorFuncSet& operator=(const MdtCorFuncSet&);
        /** no copy constructor */
        MdtCorFuncSet(const MdtCorFuncSet&);

        IMdtSlewCorFunc* m_slewing;
        IMdtBFieldCorFunc* m_bField;
        IMdtTempCorFunc* m_temperature;
        IMdtBackgroundCorFunc* m_background;
        IMdtWireSagCorFunc* m_wireSag;
    };

}  // namespace MuonCalib

#endif
