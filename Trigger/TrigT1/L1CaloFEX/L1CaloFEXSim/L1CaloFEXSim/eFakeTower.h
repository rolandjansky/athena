/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file eFakeTower.h
 * @brief Load Et of the test vector
 *
 * Replace the Et in all tower slot with the one store in the test vector
 * file for the validation of the L1Calo simulation.
 *
 * @author Tong Qiu (tong.qiu@cern.ch)
 * @date 13 May 2021
 */

#ifndef EFAKETOWER_H
#define EFAKETOWER_H
#include<string>
#include<unordered_map>
#include<vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXSysSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eFEXFPGATowerIdProvider.h"
#include "L1CaloFEXToolInterfaces/IeFakeTower.h"
#include "L1CaloFEXSim/eFEXCompression.h"


namespace LVL1 {
  /**
   * @class eFakeTower
   * @brief Load Et of the test vector
   *
   * Replace the Et in all towers with the ones store in test vector
   * files for the validation of the L1Calo simulation.
   *
   * The test vector files should be keep in one single directory. 
   * The Et of each FPGA should be stored in separate files.
   * The file name should be ***.csv where
   * The first two digits are the index of the eFEX (0-23) while the third digit is the 
   * index of the FPGA (0-3).
   * 
   * The Et will be set to zero if the file of an FPGA cannot be found.
   * 
   * The index of the Et should be defined in the first block of the test vector and the other blocks 
   * define Et of each event.
   * An example of the test vector can be found in /athena/Trigger/TrigT1/L1CaloFEX/L1CaloFEXSim/share/testvectorexample.txt.
   * 
   */
  class eFakeTower: public AthAlgTool, virtual public IeFakeTower
  {
  public:
    /// @brief Constructor
    eFakeTower(const std::string& type,const std::string& name,const IInterface* parent);

    /// @brief initiate with the path to the test vector directory 
    virtual StatusCode init(std::string) override;

    /// @brief Destructor
    ~eFakeTower();

    /**
     * @brief obtain the Et of a tower slot
     *
     * @param FPGAid FPGA id
     * @param eta eta index of the tower (0-5)
     * @param phi phi index of the tower (0-9)
     * @param layer layer (0-4)
     * @param cell tower slot. (pre-sample, layer-4 and had: 0, layer-2 and layer-3: 0-3)
     *
     * @return Et
     *
     */
    virtual int getET(int FPGAid, int eta, int phi, int layer, int cell) override;

    /// @brief Load the test vector of the next event
    virtual StatusCode loadnext() override;

    /// @brief replace the Tower Et with the ones stored in the test vector.
    virtual StatusCode execute() override;

    /// @brief Define the eTowerContainer object for which the Et will be replaced
    virtual StatusCode seteTowers(eTowerContainer*) override;

  private:
    /**
     * @brief Replace the Et in an FOGA by the ones in the test vector
     * 
     * @param tmp_eTowersIDs_subset Array of tower IDs in the FPGA
     * @param FPGAnumber FPGA index to locate the FPGA
     * @param eFEXnumber eFEX index to locate the FPGA
     *
     * @return status code
     * 
     */
    StatusCode changeFPGAET(int tmp_eTowersIDs_subset[][6], int FPGAnumber, int eFEXnumber);

    /// @brief load the Et in an FPGA
    StatusCode loadFPGA(int);
    
    /// @brief load index of Et
    StatusCode loaddic(int);

    /**
     * @brief Replace the Et in a tower by the ones in the test vector
     * 
     * @param inputtower eTower object to be processed
     * @param eta eta index to locate the SuperCell
     * @param phi phi index to locate the SuperCell
     * @param FPGAid FPGA Id to locate the SuperCell
     *
     * @return status code
     * 
     */
    StatusCode changeTowerET(LVL1::eTower* inputtower, int eta, int phi, int FPGAid);

    /**
     * @brief determine the index of an FPGA
     * 
     * @param iefex Id of the eFEX module (0-23)
     * @param ifpga Id of the FPGA (0-3)
     *
     * @return FPGA index
     * 
     */
    int getFPGAnumber(int iefex, int ifpga);

    eTowerContainer* m_eTowerContainer; //the eTowerContainer object for which the Et will be replaced

    /// @brief Load the Et or index in a block
    std::vector<int>* loadBlock(std::string, int);

    int m_numberofevents; ///< number of events

    /// tool needed for tower-FPGA mapping
    ToolHandle<IeFEXFPGATowerIdProvider> m_eFEXFPGATowerIdProviderTool {this, "eFEXFPGATowerIdProviderTool", "LVL1::eFEXFPGATowerIdProvider", "Tool that provides tower-FPGA mapping"};

    std::string m_inputfile; ///< path to the input directory
    
    /**
    * map of all supercell ETs of FPGAs
    * m_alltowers[FPGAid] = (supercell id,ET)
    * supercell id = eta * 1000 + phi * 100 + layer * 10 + cell
    */
    std::unordered_map<int, std::unordered_map<int, unsigned int>*> m_alltowers;
    
    /// map for mapping infomation. m_dict[FPGAid] = [ list of supercell id in order ]
    std::unordered_map<int, std::vector<int>*> m_dict;
  };
}

#endif
