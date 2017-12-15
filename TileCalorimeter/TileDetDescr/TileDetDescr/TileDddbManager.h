/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDddbManager_H
#define TileDddbManager_H

#include <string>
#include <vector>
#include "RDBAccessSvc/IRDBAccessSvc.h"

class IRDBRecord;
class MsgStream;

/**
  This class provides access to constants in the Geometry DB.
  We have following tables:
  @see TILE     
  @see GLOBALS  
  @see MODULES  Modules
  @see TILB     Modules     
  @see SCNT     Scintillator
  @see TIGR     Girder     
  @see TIFG     Finger
  @see CUTS     Extended Barrel cutouts
  @see TICG     
*/

class TileDddbManager
{
 public:
  enum TileSections
  {
    TILE_BARREL=1,
    TILE_EBARREL=2,
    TILE_PLUG1=3,
    TILE_PLUG2=4,
    TILE_PLUG3=5,
    TILE_PLUG4=6
  };

  TileDddbManager(IRDBAccessSvc* access,
		  std::string    version_tag,
		  std::string    version_node,
                  MsgStream *log);

  virtual ~TileDddbManager() {}
    
  // ---------------- T I L E ---------------
  //fields of TILE structure
  int TILEnmodul() const;
  double TILErmim() const;
  double TILErmam() const;
  double TILEzmam() const;
  double TILErmin() const;
  double TILErmax() const;
  double TILEzshift() const;

  // -------------- TILE GLOBALS ---------------
  int GetNumberOfEnv() const;
  int GetEnvType() const;
  int SetCurrentEnvByType(unsigned int envelope);
  int SetCurrentEnvByIndex(unsigned int envelope);
  int GetEnvSide() const;
  int GetEnvNModules() const;
  double GetEnvRin() const;
  double GetEnvRout() const;
  double GetEnvZLength() const;
  double GetEnvDPhi() const;
  double GetEnvDZ() const;
  double GetEnvDX() const;
  double GetEnvDY() const;
  double GetEnvDPsi() const;
  double GetEnvDTheta() const;
  double GetEnvZShift() const;

  // -------------- TILE MODULES ---------------
  int SetCurrentModuleByIndex(unsigned int Id);
  int SetCurrentModuleByNumber(unsigned int Number);
  int GetModNumber() const;
  int GetModType() const;
  double GetModDX() const;
  double GetModDY() const;
  double GetModDZ() const;
  double GetModDPhi() const;
  double GetModDPsi() const;
  double GetModDTheta() const;

  // -------------- TILE MODULE TYPES ---------------
  int SetCurrentTypeByIndex(unsigned int Id);
  int SetCurrentTypeByNumber(unsigned int Number);
  unsigned int GetModType(unsigned int Id) const;
  unsigned int GetModTypeSize() const;

  // -------------- S E C T I O N --------------
  unsigned int GetNumTilb() const;

  int SetCurrentSectionByIndex(unsigned int IdSection);
  int SetCurrentSectionByNumber(unsigned int section);
  int SetCurrentSection(unsigned int section, bool print=true);

  //fields of TILB structure
  int TILBsection() const;
  int TILBnperiod() const;
  int TILBnmodul() const;
  double TILBrmin() const;
  double TILBrmax() const;
  double TILBrminimal() const;
  double TILBrmaximal() const;
  double TILBdzperio() const;
  double TILBdrfront() const;
  double TILBdzend() const;
  double TILBflangex() const;
  double TILBflangey() const;
  double TILBzoffset() const;
  double TILBphigap() const;
  double TILBisci_st() const;
  double TILBdzmodul() const;
  double TILBdzmast() const;
  double TILBdzspac() const;
  double TILBdzend1() const;
  double TILBdzend2() const;
  int TILBngirder() const;
  int TILBnscin() const;
  int TILBfingpattern() const;
  int TILBnpercutpos() const;
  int TILBnpercutneg() const;
  int TILBcurscint() const;
  double TILBdzgir() const;


  // -------- S C I N T I L L A T O R ----------
  int SetCurrentScin(int item);

  /** Fields of SCNT structure (see also Oracle comments):
      @param SCNTitem  Scintillator number:1-bar,2-ext,3-ITC1(7,8 special ITC),4-ITC2,5-Gap,6-Crack
      @param SCNTdr    Dimension along R  
      @param SCNTrc    R position with respect to the lower edge of module
      @param SCNTzp    Z position
      @param SCNTdrw   Radial space for wrapping
      @param SCNTdt    Scintillator thickness in Z 
      @param SCNTdtw   Wrapping thickness in Z
      @param SCNTdphi  Distance between scintillator and absorber edge in Phi
  */
  int SCNTitem() const;
  double SCNTdr() const;
  double SCNTrc() const;
  double SCNTzp() const;
  double SCNTdrw() const;
  double SCNTdt() const;
  double SCNTdtw() const;
  double SCNTdphi() const;

  // -------- Extended Barrel Cuts ----------
  /** Fields of CUTS structure (see also Oracle comments):
      @param CUTSName  Cuts volume name
      @param CutsDX1   TRD shape, dX1/2  
      @param CutsDX2   TRD shape, dX2/2
      @param CutsDY1   TRD shape, dY1/2
      @param CutsDY2   TRD shape, dY2/2
      @param CutsDZ1   TRD shape, dZ1/2
      @param CutsXpos  X Position
      @param CutsYpos  Y Position
  */

  int SetCurrentCuts(std::string input);
  bool BoolCuts();
  std::string CutsName() const;
  double CutsDX1() const;
  double CutsDX2() const;
  double CutsDY1() const;
  double CutsDY2() const;
  double CutsDZ1() const;
  double CutsXpos() const;
  double CutsYpos() const;

  // ------------- S a d d l e  -----------------
  /** Fields of SADDLE structure (see also Oracle comments):
      @param DzSaddleSupport  Width of Saddle Support [cm]  
      @param RadiusSaddle     Radius of Saddle Support [cm]
  */

  int SetCurrentSaddle(unsigned int index);
  bool BoolSaddle();
  double DzSaddleSupport() const;
  double RadiusSaddle() const;

  // ------------- G I R D E R -----------------
  int SetCurrentGird(int item);

  // fields of TIGR structure
  int TIGRitem() const;
  int TIGRmaterial() const;
  double TIGRrc() const;
  double TIGRdr() const;
  double TIGRdw() const;
  double TIGRoff() const;

  // -------------- T I F G --------------------
  int SetCurrentTifg(int section);

  // fields of TIFG structure
  int TIFGsection() const;
  int TIFGnelem() const;
  double TIFGdz() const;

  // -------------- T I C G --------------------
  int SetCurrentTicg(int item);

  // fields of TICG structure
  int TICGitem() const;
  int TICGmaterial() const;
  int TICGshape() const;
  double TICGrc() const;
  double TICGdr() const;
  double TICGdx1() const;
  double TICGdx2() const;
  double TICGoff() const;
  double TICGdz() const;
  double TICGzc() const;

  // -------------- T I C L --------------------
  unsigned int GetNumTicl() const; 

  int SetCurrentTicl(int detector, double sample, double tower);
  int SetCurrentTicl(unsigned int index);

  // Find first TICL for given detector
  // 1 - success; 0 - doesn't exist any TICL for given detector
  int SetFirstTiclInDet(int detector);   

  // Find first TICL for given detector and sample
  // 1 - success; 0 - doesn't exist any such TICL
  int SetFirstTiclInDetSamp(int detector, double sample);

  // Move to the next TICL with the same detector field
  // 1 - success; 0 - the current is last one, it remains unchanged
  int SetNextTiclInDet();

  // Move to the next TICL with the same detector and sample fields
  // 1 - success; 0 - the current is last one, it remains unchanged
  int SetNextTiclInDetSamp();

  // fields of TICL structure
  int TICLdetector() const;
  double TICLncell() const;
  double TICLtower() const;
  double TICLsample() const;
  double TICLeta() const;
  double TICLdeta() const;
  double TICLfirstrow() const;
  double TICLlastrow() const;
  double TICLntilesrow(unsigned int ind) const;
  double TICLnpmt() const;
  double TICLholes(unsigned int ind) const;

  // fields of tile switches
  bool addPlatesToCell() const;
  int  Ushape()  const;
  int  glue() const;
 private:

  IRDBRecordset_ptr m_tiglob;
  IRDBRecordset_ptr m_timod;
  IRDBRecordset_ptr m_tile;
  IRDBRecordset_ptr m_tilb;
  IRDBRecordset_ptr m_tigr;
  IRDBRecordset_ptr m_scnt;
  IRDBRecordset_ptr m_tifg;
  IRDBRecordset_ptr m_ticg;
  IRDBRecordset_ptr m_ticl;
  IRDBRecordset_ptr m_cuts;
  IRDBRecordset_ptr m_saddle;
  IRDBRecordset_ptr m_tileSwitches; 
 

  unsigned int m_n_tiglob;
  unsigned int m_n_timod;
  unsigned int m_n_tilb;
  unsigned int m_n_tigr;
  unsigned int m_n_scnt;
  unsigned int m_n_tifg;
  unsigned int m_n_ticg;
  unsigned int m_n_ticl;
  unsigned int m_EnvNum;
  unsigned int m_EnvSize;
  unsigned int m_EnvBegin;
  unsigned int m_n_cuts;
  unsigned int m_n_saddle;
  unsigned int m_n_tileSwitches;
 
  const IRDBRecord* m_currentTileGlob;
  const IRDBRecord* m_currentTileMod;
  const IRDBRecord* m_currentSection;
  const IRDBRecord* m_currentGird;
  const IRDBRecord* m_currentScin;
  const IRDBRecord* m_currentTifg;
  const IRDBRecord* m_currentTicg;
  const IRDBRecord* m_currentTicl;
  const IRDBRecord* m_currentCuts;
  const IRDBRecord* m_currentSaddle;

  int m_currentTiclInd;   // Index of current TICL structure

  std::string m_tag;
  std::string m_node;
  std::vector<unsigned int> m_modTypes;
  void FillModTypes();

  bool m_verbose;
  bool m_buildCuts;
  bool m_buildSaddle;
};

#endif
