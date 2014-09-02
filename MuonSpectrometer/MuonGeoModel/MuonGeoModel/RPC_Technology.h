/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_H
#define RPC_H

#include "MuonGeoModel/Technology.h"

#define RPCprint false
namespace MuonGM {

class RPC: public Technology {
public:

    // the internal RPC geometry parameters{
    // the external support panel is the pre-bent panel: the honeycomb material can depend of the
    // station type !!!!
    double externalSupPanelThickness; double externalAlSupPanelThickness; //given by AMDB  W RPC
    //for Lay.P lower=external and upper does not exist
    //for Lay.M lower and upper are given separatly by AMDB
    double    lowerSupPanelThickness; double    lowerAlSupPanelThickness;
    double    upperSupPanelThickness; double    upperAlSupPanelThickness;
    // the central support panel
    double  centralSupPanelThickness; double  centralAlSupPanelThickness; //given by AMDB  W RPC

    // A rpcLayer consists of a bakelite gas volume,
    // a pet foil (on top[bottom] of the bakelite gas volume for the lower[upper] gas gap in a doublet),
    // two strip panel layers, one on each side of the bakelite gas gap
    double rpcLayerThickness;  // given by AMDB
    // TotalThickness = 2*rpcLayerThickness + centralSupPanelThickness + externalSupPanelThickness
    // for layout P and following
    // TotalThickness = 2*rpcLayerThickness + centralSupPanelThickness + lower + upperSupPanelThickness
    // for layout M
    double TotalThickness;        // given by AMDB

    
    // GasGap consists of a bakelite gas volume,
    // a pet foil (on top[bottom] of the bakelite gas volume for the lower[upper] gas gap in a doublet)
    int    NGasGaps_in_s;        // from AMDB
    int    NGasGaps_in_z;        // from AMDB
    double GasGapThickness;      // must be computed, if needed, from the following data
    double bakeliteThickness;    // from AMDB P
    double bakeliteframesize;    // from where ?????????????
    double bakelitePetThickness; // from amdb
    double gasThickness;         // from AMDB 
//    double petThickness;         // from AMDB P
    double totalAirThickness;         // from AMDB P

    // The spacers reinforcing the gasgap structure
    double spacerDiameter; double spacerPitch;   // from AMDB P
    
    // extra pet foil
    double petFoilThickness;

    // StripPanels
    int    NstripPanels_in_s;          int    NstripPanels_in_z;         // from AMDB
    double MidChamberDeadRegion_in_s;  double MidChamberDeadRegion_in_z; // from AMDB
    double stripPitchS; double stripPitchZ; double stripSeparation; //from AMDB
    double stripPanelThickness;  // to be computed from the following quantities
    double stripPanelFoamThickness; // from AMDB
    double stripPanelCopperSkinThickness; // from AMDB
    double stripPanelStripSidePetThickness; double stripPanelGroundSidePetThickness; // from AMDB
    double frontendBoardWidth; double backendBoardWidth;
    

    double maxThickness; // dictated by sup clearance !!!!
    
    // end of the internal RPC geometry parameters}

    double supportThickness; double supportThicknessWidth;

    double upperHoneycombLayerThickness;
    double alThickness;
    
    double RPCLayerThickness;
    double foamSpacerThickness; double xx;
    double lowerHoneycombLayerThickness;double yy;
    double totalThickness;
    double pitchs; double pitchz; double deadSeparation;
    //double bakeliteThickness;
    double gazGapThickness;
    double stripsSupportThickness; double stripsThickness; 
    double internalMidchamberDeadRegion;
    //    double spacerDiameter;   double spacerPitch;
    double firstStripsOffset;  double secondStripsOffset;
    double numberOfStripsReadouts;





    double centralalThickness;
    //double petThickness;
    //double airThickness;
    double petUpperThickness;
    double petLowerThickness; 
    double sInternalMidchamberDeadRegion;
    double zInternalMidchamberDeadRegion;
    double sStripsOffset, zFirstStripsOffset;  double zSecondStripsOffset;
    double sNumberOfStripsReadouts; double zNumberOfStripsReadouts;
    double sGasGapsNumber;




    RPC(std::string s):Technology(s) {}
    

    double GetCentralalThickness(){return centralalThickness;};
    double GetRPCLayerThickness(){return RPCLayerThickness;};
    double GetFoamSpacerThickness(){return foamSpacerThickness;};
    double GetUpperHoneycombLayerThickness(){return upperHoneycombLayerThickness;};
    double GetAlThickness(){return alThickness;};
    double GetPitchs(){return pitchs;};
    double GetPitchz(){return pitchz;};
    double GetDeadSeparation(){return deadSeparation;};
    double GetBakeliteThickness(){return bakeliteThickness;};
    double GetGazGapThickness(){return gazGapThickness;};
    //    double GetPetThickness(){return petThickness;};
    double GetAirThickness(){return totalAirThickness;};
    double GetStripsSupportThickness(){return stripsSupportThickness;};
    double GetStripsThickness(){return stripsThickness;};
    double GetPetUpperThickness(){return petUpperThickness;};
    double GetPetLowerThickness(){return petLowerThickness;}; 
    double GetSInternalMidchamberDeadRegion(){return sInternalMidchamberDeadRegion;};
    double GetZInternalMidchamberDeadRegion(){return zInternalMidchamberDeadRegion;};
    double GetSpacerDiameter(){return spacerDiameter;};
    double GetSpacerPitch(){return spacerPitch;};
    double GetSStripsOffset(){return sStripsOffset;};
    double GetZFirstStripsOffset(){return zFirstStripsOffset;};
    double GetZSecondStripsOffset(){return zSecondStripsOffset;};
    double GetSNumberOfStripsReadouts(){return sNumberOfStripsReadouts;};
    double GetZNumberOfStripsReadouts(){return zNumberOfStripsReadouts;};
    double GetSGasGapsNumber(){return sGasGapsNumber;};
	
    // set methods

    void SetCentralalThickness(double d){centralalThickness=d;};
    void SetRPCLayerThickness(double d){RPCLayerThickness=d;};
    void SetFoamSpacerThickness(double d){foamSpacerThickness=d;};
    void SetUpperHoneycombLayerThickness(double d){upperHoneycombLayerThickness=d;};
    void SetAlThickness(double d){alThickness=d;};
    void SetPitchs(double d){pitchs=d;};
    void SetPitchz(double d){pitchz=d;};
    void SetDeadSeparation(double d){deadSeparation=d;};
    void SetBakeliteThickness(double d){bakeliteThickness=d;};
    void SetGazGapThickness(double d){gazGapThickness=d;};
    //    void SetPetThickness(double d){petThickness=d;};
    void SetAirThickness(double d){totalAirThickness=d;};
    void SetStripsSupportThickness(double d){stripsSupportThickness=d;};
    void SetStripsThickness(double d){stripsThickness=d;};
    void SetPetUpperThickness(double d){petUpperThickness=d;};
    void SetPetLowerThickness(double d){petLowerThickness=d;}; 
    void SetSInternalMidchamberDeadRegion(double d){sInternalMidchamberDeadRegion=d;};
    void SetZInternalMidchamberDeadRegion(double d){zInternalMidchamberDeadRegion=d;};
    void SetSpacerDiameter(double d){spacerDiameter=d;};
    void SetSpacerPitch(double d){spacerPitch=d;};
    void SetSStripsOffset(double d){sStripsOffset=d;};
    void SetZFirstStripsOffset(double d){zFirstStripsOffset=d;};
    void SetZSecondStripsOffset(double d){zSecondStripsOffset=d;};
    void SetSNumberOfStripsReadouts(double d){sNumberOfStripsReadouts=d;};
    void SetZNumberOfStripsReadouts(double d){zNumberOfStripsReadouts=d;};
    void SetSGasGapsNumber(double d){sGasGapsNumber=d;};


};
} // namespace MuonGM

#endif
