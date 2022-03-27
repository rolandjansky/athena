/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCMGM_AMDCDUMPGEOMODEL_H
#define AMDCMGM_AMDCDUMPGEOMODEL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

/////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <string>
#include <vector>

#include "AmdcAth/AmdcsimrecAthenaSvc.h"

namespace MuonGM {
    class MdtReadoutElement;
    class RpcReadoutElement;
    class TgcReadoutElement;
    class CscReadoutElement;
}  // namespace MuonGM

/**
 @class AmdcDumpGeoModel

 This class compares positions of active elements of the
 Muon Spectrometer as computed by Amdcsimrec and the Muon GeoModel

@author samusog@cern.ch

*/

class AmdcDumpGeoModel : public AthAlgorithm {
public:
    AmdcDumpGeoModel(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute() { return StatusCode::SUCCESS; }

private:
    ///////////////////////////////////
    // Functions

    /**Comparison for Mdt */
    void LoopMdtElements(std::ofstream& OutFile);

    /**Comparison for Mdt */
    void MdtCompare(std::ofstream& OutFile, Identifier channelId, const MuonGM::MdtReadoutElement* pReadoutElement);
    void MdtCompare(std::ofstream& OutFile, double EpsLoMdt, int WarningLevel, int SPstationName, std::string SPstationNameString,
                    int SPstationEta, int SPstationPhi, int SPmultilayer, int SPtubeLayer, int SPtube, double GeoModX_RO, double GeoModY_RO,
                    double GeoModZ_RO, double GeoModX, double GeoModY, double GeoModZ, double TubeLength);

    /**Comparison for Rpc */
    void LoopRpcElements(std::ofstream& OutFile);

    /**Comparison for Rpc */
    void RpcCompare(std::ofstream& OutFile, Identifier channelId, const MuonGM::RpcReadoutElement* pReadoutElement);
    void RpcCompare(std::ofstream& OutFile, double EpsLoRpc, int WarningLevel, int SPstationName, std::string SPstationNameString,
                    int SPstationEta, int SPstationPhi, int SPDoubletR, int SPDoubletZ, int SPDoubletPhi, int SPGasGap, int SPMeasuresPhi,
                    int SPStrip, double GeoModX, double GeoModY, double GeoModZ);

    /**Comparison for Tgc */
    void LoopTgcElements(std::ofstream& OutFile);

    /**Comparison for Tgc */
    void TgcCompare(std::ofstream& OutFile, Identifier channelId, const MuonGM::TgcReadoutElement* pReadoutElement);
    void TgcCompare(std::ofstream& OutFile, double EpsLoTgc, int WarningLevel, int SPstationName, std::string SPstationNameString,
                    int SPstationEta, int SPstationPhi, int SPGasGap, int SPIsStrip, int SPChannel, double GeoModX, double GeoModY,
                    double GeoModZ);

    /**Comparison for Csc */
    void LoopCscElements(std::ofstream& OutFile);

    /**Comparison for Csc */
    void CscCompare(std::ofstream& OutFile, Identifier channelId, const MuonGM::CscReadoutElement* pReadoutElement);
    void CscCompare(std::ofstream& OutFile, double EpsLoCsc, int WarningLevel, int SPstationName, std::string SPstationNameString,
                    int SPstationEta, int SPstationPhi, int SPChamberLayer, int SPWireLayer, int SPMeasuresPhi, int SPStrip, double GeoModX,
                    double GeoModY, double GeoModZ);

    /**AntiSelection on Station type */
    int AntiSelectedStation(const std::string& TheStationName);

    /**Selection on Station type */
    int SelectedStation(const std::string& TheStationName);

    /**Selection on chambers */
    int SelectedChamber(const std::string& TheStationName, int AmdcJzz, int AmdcJff);

    // Data

    int m_SwitchOff;     //!< Optional switch off
    int m_WarningLevel;  //!< Warnig Level warning

    int m_CheckMdt;     //!< Control comparison for Mdt
    double m_EpsLoMdt;  //!< Control comparison for Mdt

    int m_CheckRpc;     //!< Control comparison for Rpc
    double m_EpsLoRpc;  //!< Control comparison for Rpc

    int m_CheckTgc;     //!< Control comparison for Tgc
    double m_EpsLoTgc;  //!< Control comparison for Tgc

    int m_CheckCsc;       //!< Control comparison for Csc
    double m_EpsLoCsc;    //!< Control comparison for Csc
    int m_CenterCscMean;  //!< use mean value of strip corners position instead of center for CSC

    int m_AntiStationSelection;                      //!< Control Station anti-selection
    std::vector<std::string> m_AntiStationSelected;  //!< Station anti-Selected

    int m_StationSelection;                      //!< Control Station selection
    std::vector<std::string> m_StationSelected;  //!< Station Selected

    int m_ChamberSelection;                          //!< Control Chamber selection
    std::vector<std::string> m_StationNameSelected;  //!< Chamber Selected
    std::vector<int> m_StationAbsAmdcJzzSelected;    //!< Chamber Selected
    std::vector<int> m_StationAmdcJffSelected;       //!< Chamber Selected

    int m_EmergencyOut;  //!< Optional stop at the end of initialisation

    const MuonGM::MuonDetectorManager* p_MuonDetectorManager;  //!< Pointer On MuonDetectorManager

    double m_Mdt_MaxDiffZ;   //!< Max deviation
    double m_Mdt_MaxDiffS;   //!< Max deviation
    double m_Mdt_MaxDiffT;   //!< Max deviation
    double m_Mdt_MaxDiffZs;  //!< Max deviation
    double m_Mdt_MaxDiffSs;  //!< Max deviation
    double m_Mdt_MaxDiffTs;  //!< Max deviation

    double m_Rpc_MaxDiffZ;  //!< Max deviation
    double m_Rpc_MaxDiffS;  //!< Max deviation
    double m_Rpc_MaxDiffT;  //!< Max deviation

    double m_Tgc_MaxDiffZ;  //!< Max deviation
    double m_Tgc_MaxDiffS;  //!< Max deviation
    double m_Tgc_MaxDiffT;  //!< Max deviation

    double m_Csc_MaxDiffss;  //!< Max deviation
    double m_Csc_MaxDiffzz;  //!< Max deviation
    double m_Csc_MaxDifftt;  //!< Max deviation

    int m_KountCallsDoIt;  //!< Kount calls to DoIt

    ServiceHandle<AmdcsimrecAthenaSvc> p_AmdcsimrecAthenaSvc;  //!< Pointer On AmdcsimrecAthenaSvc
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    StatusCode DoIt();
};

#endif
