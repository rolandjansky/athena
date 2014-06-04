/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

      There are two types of identifiers for the ATLAS calorimeters:<br>
      offline identifiers (type= Identifier), and online identifiers (type= HWIdentifier). <br>
 <br>
      And there are two 'families' of identifiers: <br>
      detector specific identifiers (for EM, HEC, FCAL and TILES) <br>
      calorimeter-wide identifiers (for Trigger Towers, for Dead Material and for EM/HEC/FCAL/Tile wrapped together in CaloCell) <br>

      All identifiers are decoded by specific classes called "Helpers", all deriving from base class AtlasDetectorID. <br>
      The field content of the identifiers is described in xlm files called dictionaries which can be found in the data folder 
      of the DetectorDescription/IdDictParser package.  <br>



      The offline identifiers are decoded by the following helper classes:  LArEM_ID ,  LArHEC_ID ,  LArFCAL_ID ,  TileID
      and CaloLVL1_ID , CaloDM_ID , CaloCell_ID of the present package. <br>
      The corresponding dictionaries are IdDictLArCalorimeter_xxx.xml (LArEM_ID, LArHEC_ID, LArFCAL_ID), IdDictTileCalorimeter_yyy.xml (TileID), 
      and IdDictCalorimeter_zzz.xml (CaloLVL1_ID, CaloDM_ID); there is no dictionary for CaloCell_ID. <br>

      The online identifiers are decoded by the following helper classes (release 13 onwards): <br>
      TTOnlineID of the present package <br>
      LArOnlineID , LArReadoutModuleID of package LArCalorimeter/LArIdentifier <br>
      and TileHWID of package TileCalorimeter/TileIdentifier. <br>
      The corresponding dictionaries are IdDictLArCalorimeter_xxx.xml (LArOnline_ID,), IdDictTileCalorimeter_yyy.xml (TileHWID) 
      and IdDictCalorimeter_zzz.xml (TTOnline_ID); there is no dictionary for LArReadoutModuleID. <br>
      There are also identifiers/helpers for calibration lines and High Voltages, see package LArCalorimeter/LArIdentifier. <br>

@htmlinclude used_packages.html

@include requirements


*/
