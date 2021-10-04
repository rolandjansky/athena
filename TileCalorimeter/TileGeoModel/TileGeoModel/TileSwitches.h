/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESWITCHES_h
#define TILESWITCHES_h

/* This is a very simple struct for holding parameters
   which are stored in TileSwitches table in GeoModel DB
   and which can be overwritten from jobOptions
   Default value -1 means that value will be taken from GeoModel DB later on
*/

class TileSwitches
{
public:
  /** Setup defaults */
  TileSwitches(bool tb=false, bool pl=true) :
          testBeam(tb),
          addPlatesToCell(pl),
          uShape(-1),
          glue(-1),
          pvt(-1),
          steel(-1),
          csTube(-1)
      {}

  /** setting up testbeam geometry or ATLAS geometry **/
  bool testBeam;

  /** calculate cell volumes with or without front-plates and end-plates **/
  bool addPlatesToCell;

  /** 0: simulation without U-shape
      1: simulation with U-shape **/
  int  uShape;

  /** 0: glue layer is removed and replaced by iron,
      1: simulation with glue,
      2: glue is replaced by iron + width of iron is modified in order to get the same sampling fraction      */
  int glue;

  /** 0: all scintillators are polystyrene
      1: crack scrintillators are PVT, others - polystyrene **/
  int pvt;

  /** 0: Absorber is pure Iron
      1: Absorber is tile::Steel defined in DB **/
  int steel;

  /** 0: without Cesium tubes
      1: with cesium tubes **/
  int csTube;
};

#endif
