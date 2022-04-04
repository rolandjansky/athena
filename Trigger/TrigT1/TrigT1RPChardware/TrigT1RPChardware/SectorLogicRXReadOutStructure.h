/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorLogicRXReadOutStructure_H
#define TrigT1RPChardware_SectorLogicRXReadOutStructure_H

#include <vector>

#include "TrigT1RPChardware/Lvl1Def.h"
#include "TrigT1RPChardware/RPCReadOut.h"

class SectorLogicRXReadOutStructure : public RPCReadOut {
public:
    SectorLogicRXReadOutStructure();
    SectorLogicRXReadOutStructure(ubit16 inputData);
    ~SectorLogicRXReadOutStructure() = default;

    void setInit();
    //
    ubit16 makeHeader(ubit16 *inputData);
    ubit16 makeSubHeader() { return 0; };
    ubit16 makeBody(ubit16 *inputData) { return *inputData; };
    ubit16 makeFooter(ubit16 errorCode) { return errorCode; };
    ubit16 makeFooter(ubit16 *errorCode);

    //**************************************************//
    // general user methods to decode ReadOut stream    //
    //**************************************************//
    ubit16 decodeFragment(ubit16 inputWord, char &field);
    //
    bool isHeader();
    bool isBody() { return false; }
    bool isFooter();

    bool isSubHeader();

    bool isInputHeader();
    bool isOutputHeader();
    bool isInputBody();
    bool isOutputBody();
    //
    // header
    ubit16 slid() const { return m_slid; }
    ubit16 fel1id() const { return m_fel1id; }
    // subheader
    ubit16 slbcid() const { return m_slbcid; }
    // input header
    ubit16 inputRowinBcid() const { return m_inputRowinBcid; }
    ubit16 inputPadId() const { return m_inputPadId; }
    // output header
    ubit16 outputRowinBcid() const { return m_outputRowinBcid; }
    // input body
    ubit16 inputTriggerBcid() const { return m_inputTriggerBcid; }
    ubit16 inputThreshold() const { return m_inputThreshold; }
    ubit16 inputRoi() const { return m_inputRoi; }
    ubit16 inputOverlapEta() const { return m_inputOverlapEta; }
    ubit16 inputOverlapPhi() const { return m_inputOverlapPhi; }
    ubit16 inputOuterPlane() const { return m_inputOuterPlane; }
    // output body
    ubit16 nTriggerCand() const { return m_nTriggerCand; }
    ubit16 outputTriggerBcid(int /*nCand*/) const { return m_outputTriggerBcid; }
    ubit16 outputThreshold(int nCand) const { return m_outputThreshold[nCand]; }
    ubit16 outputRoi(int nCand) const { return m_outputRoi[nCand]; }
    ubit16 outputOverlap(int nCand) const { return m_outputOverlap[nCand]; }
    ubit16 hasMoreThan2TriggerCand() const { return m_moreThan2TriggerCand; }
    // footer
    ubit16 status() const { return m_status; }
    ubit16 crc() const { return m_crc; }

    ubit16 nOfTriggerWords() const { return m_outTriggerWord.size(); }
    bool isOutputDecoded() { return (nOfTriggerWords() == 3); }

private:
    //
    // header
    ubit16 m_slid{0};
    ubit16 m_fel1id{0};
    // subheader
    ubit16 m_slbcid{0};
    // input header
    ubit16 m_inputRowinBcid{0};
    ubit16 m_inputPadId{0};
    // output header
    ubit16 m_outputRowinBcid{0};
    //  8 or 12  bits of trigger output word
    std::vector<ubit16> m_outTriggerWord{0};
    // input body
    ubit16 m_inputTriggerBcid{0};
    ubit16 m_inputThreshold{0};
    ubit16 m_inputRoi{0};
    ubit16 m_inputOverlapEta{0};
    ubit16 m_inputOverlapPhi{0};
    ubit16 m_inputOuterPlane{0};
    // output body
    ubit16 m_outputTriggerBcid{0};
    std::array<ubit16, 2> m_outputThreshold{0};
    std::array<ubit16, 2> m_outputOverlap{0};
    std::array<ubit16, 2> m_outputRoi{0};
    ubit16 m_nTriggerCand{0};
    bool m_moreThan2TriggerCand{false};
    // footer
    ubit16 m_status{0};
    ubit16 m_crc{0};

    //
    // internal objects;
    //
    //
    // some static parameter describing the structure
    //
    static constexpr ubit16 s_headerNum = 3;
    static constexpr std::array<ubit16, s_headerNum> s_headerPos{12, 9, 0};
    static constexpr std::array<ubit16, s_headerNum> s_headerLen{4, 3, 9};
    static constexpr ubit16 s_headerVal = 0x000d;

    static constexpr ubit16 s_subHeaderNum = 2;
    static constexpr std::array<ubit16, s_subHeaderNum> s_subHeaderPos{12, 0};
    static constexpr std::array<ubit16, s_subHeaderNum> s_subHeaderLen{4, 12};
    static constexpr ubit16 s_subHeaderVal = 0x0006;

    static constexpr ubit16 s_inputHeaderNum = 3;
    static constexpr std::array<ubit16, s_inputHeaderNum> s_inputHeaderPos{12, 8, 0};
    static constexpr std::array<ubit16, s_inputHeaderNum> s_inputHeaderLen{4, 4, 3};
    static constexpr ubit16 s_inputHeaderVal = 0x0001;

    static constexpr ubit16 s_outputHeaderNum = 3;
    static constexpr std::array<ubit16, s_outputHeaderNum> s_outputHeaderPos{12, 8, 0};
    static constexpr std::array<ubit16, s_outputHeaderNum> s_outputHeaderLen{4, 4, 8};
    static constexpr ubit16 s_outputHeaderVal = 0x0002;

    static constexpr ubit16 s_inputBodyNum = 7;
    static constexpr std::array<ubit16, s_inputBodyNum> s_inputBodyPos{12, 9, 7, 6, 5, 2, 0};
    static constexpr std::array<ubit16, s_inputBodyNum> s_inputBodyLen{4, 3, 1, 1, 1, 3, 2};
    static constexpr ubit16 s_inputBodyVal = 0x0000;

    static constexpr ubit16 s_outputBodyNum = 4;
    static constexpr std::array<ubit16, s_outputBodyNum> s_outputBodyPos{12, 0};
    static constexpr std::array<ubit16, s_outputBodyNum> s_outputBodyLen{4, 12};
    static constexpr ubit16 s_outputBodyVal = 0x0003;

    static constexpr ubit16 s_footerNum = 3;
    static constexpr std::array<ubit16, s_footerNum> s_footerPos{12, 8, 0};
    static constexpr std::array<ubit16, s_footerNum> s_footerLen{4, 4, 8};
    static constexpr ubit16 s_footerVal = 0x000f;
    //
};

#endif
