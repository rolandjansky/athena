// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_PortMap.h
 * @brief Header file for the SCT_PortMap class
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 */

#ifndef SCT_ConditionData_SCT_PortMap_h
#define SCT_ConditionData_SCT_PortMap_h

/** Enums for chip type*/
namespace SCT_Parameters {
  enum ChipType {Chip0=0, Chip1, Chip2, Chip3, Chip4, Chip5, Chip6, Chip7, Chip8, Chip9, Chip10, Chip11, NChips, None=-999};
}

/**
 * @class SCT_PortMap
 * @brief Helper class to map the input/output ports of a chip
 **/

class SCT_PortMap {

public:

  /** Constuctor from input and output chip types*/
  SCT_PortMap(SCT_Parameters::ChipType in0, SCT_Parameters::ChipType in1, SCT_Parameters::ChipType out0, SCT_Parameters::ChipType out1) {
    m_input[0]  = in0;
    m_input[1]  = in1;
    m_output[0] = out0;
    m_output[1] = out1;
  }

  ~SCT_PortMap() = default;
  
  /** Functions to retrieve the chip mapped to the input and output ports*/
  inline SCT_Parameters::ChipType input(int port) const  { return m_input[port]; }
  inline SCT_Parameters::ChipType output(int port) const { return m_output[port]; }

private:

  SCT_Parameters::ChipType m_input[2];   //!<  Port type for the chip's two inputs
  SCT_Parameters::ChipType m_output[2];  //!<  Port type for the chip's two outputs
};

#endif // SCT_ConditionData_SCT_PortMap_h
