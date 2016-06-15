/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ReadoutTool.h
 * Header file for the SCT_ReadoutTool class
 * @author Carl Gwilliam <gwilliam@mail.cern.ch>
 */

#ifndef SCT_ConditionServices_SCT_ReadoutTool_h
#define SCT_ConditionServices_SCT_ReadoutTool_h

// C++
#include <string> 
#include <vector> 
#include <bitset>
//#include <array>

// Gaudi
#include "GaudiKernel/ServiceHandle.h"

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsServices/ISCT_ReadoutTool.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

#include "SCT_Chip.h"

// Forward declarations
class SCT_ID;

/** Enums for module and chip type*/
namespace SCT_Parameters {
  enum ModuleType {BARREL, MODIFIED_0, MODIFIED_1, ENDCAP};
  enum ChipType   {Chip0=0, Chip1, Chip2, Chip3, Chip4, Chip5, Chip6, Chip7, Chip8, Chip9, Chip10, Chip11, NChips, None=-999};
}

/**
 * @class SCT_PortMap
 * Helper class to map the input/output ports of a chip
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

  ~SCT_PortMap() {}
  
  /** Functions to retrieve the chip mapped to the input and output ports*/
  inline SCT_Parameters::ChipType input(int port) const  { return m_input[port];}
  inline SCT_Parameters::ChipType output(int port) const { return m_output[port];}

private:

  SCT_Parameters::ChipType m_input[2];   //!<  Port type for the chip's two inputs
  SCT_Parameters::ChipType m_output[2];  //!<  Port type for the chip's two outputs
};

/**
 * @class SCT_ReadoutTool
 * Class to represent the SCT module readout
**/

class SCT_ReadoutTool : public AthAlgTool, virtual public ISCT_ReadoutTool{

public:

  /** Usual framework methods for an AlgTool*/
  SCT_ReadoutTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~SCT_ReadoutTool() {}
  StatusCode initialize();
  StatusCode finalize();
  
  /** Determine which chips are in the readout for a module of a particular type by Identifier*/
  StatusCode determineReadout(const Identifier& moduleId, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok);
  /** Determine which chips are in the readout for a module of a particular type by truncated serial number*/
  StatusCode determineReadout(const int truncatedSerialNumber, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok);
  //unsigned int chipsInReadout() {return static_cast<unsigned int>(m_chipInReadout.to_ulong());}

private:

  /** Private data*/
  const SCT_ID*                       m_sctId;               //!< ID helper for SCT
  ServiceHandle<ISCT_CablingSvc>      m_cablingSvc;          //!< Service handle for SCT Cabling
  std::vector<SCT_Chip*>              m_chips;               //!< Vector of actual SCT Chips for that module
  std::vector<SCT_PortMap>            m_chipMap;             //!< Vector of port mapping from the chips in an SCT module 
  bool                                m_linkActive[2];       //!< Links status for link 0 and 1
  std::bitset<SCT_Parameters::NChips> m_chipInReadout;       //!< Bitset indicating whether a chip is readout or not
  SCT_Parameters::ModuleType          m_type;                //!< The type of this module (Barrel, Modified Barrel (0 or 1), Endcap)
  std::vector<int>                    m_chipsOnLink0;        //!< The chips read out on link 0
  std::vector<int>                    m_chipsOnLink1;        //! <The chips read out on link 1

  /** Find the ID of the input chip for chip*/ 
  inline SCT_Parameters::ChipType inputChip(const SCT_Chip& chip) const {
    return m_chipMap[chip.id()].input(chip.inPort());
  }

  /** Find the ID of the output chip for chip*/
  inline SCT_Parameters::ChipType outputChip(const SCT_Chip& chip) const {
    return m_chipMap[chip.id()].output(chip.outPort());
  }

  /** Set chip in readout and which link it is on*/
  inline void setChipIn(const SCT_Chip& chip, int link) {
    m_chipInReadout.set(chip.id());
    if (link==0) m_chipsOnLink0.push_back(chip.id());
    if (link==1) m_chipsOnLink1.push_back(chip.id());
  }

  /** Set chip out of readout and reset link*/
  inline void                         setChipOut(const SCT_Chip& chip) {m_chipInReadout.reset(chip.id());}
  /** Test if chip is in readout or not*/
  inline bool                         isChipReadOut(const SCT_Chip& chip) const {return m_chipInReadout.test(chip.id());}
  /** Set the module type */
  void                                setModuleType(Identifier moduleId);
  /** Fill the chip mapping*/
  void                                setChipMap(SCT_Parameters::ModuleType type);
  /** Chip is an end but is being talked to*/
  bool                                isEndBeingTalkedTo(const SCT_Chip& chip) const;
  /** Chip has a correctly connected input*/
  bool                                hasConnectedInput(const SCT_Chip& chip) const;
  /** Check which chips are in the readout for a particular link and if the readout is sane*/
  void                                checkLink(int link);
  /** Follow the readout to the input side*/
  bool                                followReadoutUpstream(int link, const SCT_Chip& chip, int remainingDepth = 12);
  /** is the readout for a particular link sane*/
  bool                                isLinkStandard(int link);
  /** Mask the chips that are not in the readout*/
  void                                maskChipsNotInReadout();
  /** Print readout status*/
  void                                printStatus(const Identifier& moduleId);

};

#endif
