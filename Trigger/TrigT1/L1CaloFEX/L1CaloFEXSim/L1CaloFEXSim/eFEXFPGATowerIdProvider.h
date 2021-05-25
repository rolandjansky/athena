/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file eFEXFPGATowerIdProvider.h
 * @brief Provide tower-FPGA mapping
 *
 * Load information stored in a .csv file and
 * provide the IDs of all towers in an FPGA or an eFEX module.
 *
 * @author Tong Qiu (tong.qiu@cern.ch)
 * @date 13 May 2021
 */

#ifndef EFEXFPGATOWERIDPROVIDER_H
#define EFEXFPGATOWERIDPROVIDER_H

#include "L1CaloFEXToolInterfaces/IeFEXFPGATowerIdProvider.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>
#include <unordered_map>
#include <vector>
namespace LVL1 {
  /**
   * @class eFEXFPGATowerIdProvider
   * @brief Provide tower-FPGA mapping
   * Load information stored in a .csv file and
   * provide the IDs of all towers in an FPGA or an eFEX module.
   */
  class eFEXFPGATowerIdProvider : public AthAlgTool, virtual public IeFEXFPGATowerIdProvider {

  /**
   * @brief Sturcture defines the id, eta and phi position of a tower
   */
  struct towerinfo {
    int eTowerID; ///< tower ID 
    int eTowerEta; ///< tower Eta index
    int eTowerPhi; ///< tower Phi index
  };


  public:
    /// @brief Constructor
    eFEXFPGATowerIdProvider(const std::string& type,const std::string& name,const IInterface* parent);

    /// @brief Destructor
    ~eFEXFPGATowerIdProvider();

    /// @brief initialize the tool
    StatusCode initialize() override;

    /// @brief set path to the csv file and load
    StatusCode setAddress(std::string inputaddress) override;

    /**
     * @brief obtain ordered tower IDs in an FPGA
     *
     * @param eFEXID Id of the eFEX module (0-23)
     * @param FPGAID Id of the FPGA (0-3)
     * @param towerlist a 10*6 (phi*eta) array to store the output tower Ids
     *
     * @return status code
     *
     */
    StatusCode getRankedTowerIDinFPGA(int eFEXID, int FPGAID, int(&towerlist)[10][6]) const override;

    /**
     * @brief obtain ordered tower IDs in an eFEX
     *
     * @param eFEXID Id of the eFEX module (0-23)
     * @param towerlist a 10*18 (phi*eta) array to store the output tower Ids
     *
     * @return status code
     *
     */
    StatusCode getRankedTowerIDineFEX(int eFEXID, int(&towerlist)[10][18]) const override;
    bool ifhaveinputfile() const override;

  private:
    bool m_hascsvfile; ///< if the csv file is valid

    /**
     * @brief rank the tower ids in an FPGA
     * This function determines the order of towers in an FPGA.
     * Towers are sorted using eta and phi, and 
     * then store the result in m_towerrankingcache object for future use.
     * 
     * @param FPGAindex index of the FPGA
     *
     * @return status code
     * 
     * @see getFPGAIndex
     * 
     */
    StatusCode rankTowerinFPGA(int FPGAindex) override;

    /// Ordered tower Ids in each FPGAs. The index is the index of the FPGA. @see getFPGAIndex
    std::vector<std::vector<int>*> m_towerrankingcache;

    /// Unordered tower Ids in each FPGAs. The index is the index of the FPGA. @see getFPGAIndex
    std::unordered_map<int, std::vector<towerinfo>* > m_alltowers;

    /// @brief check if an FPGA exists in the csv file
    bool hasFPGA(int) const override;

    /// @brief load the mapping info in the csv file
    StatusCode loadcsv() override;

    /**
     * @brief determine the index of an FPGA
     * 
     * @param eFEXID Id of the eFEX module (0-23)
     * @param FPGAID Id of the FPGA (0-3)
     *
     * @return FPGA index
     * 
     */
    int getFPGAIndex(int eFEXid, int FPGAid) const override;

    std::string m_csvaddress; ///< path to the input csv file
    
    // Gaudi::Property<std::string> m_csvaddress_setting { this, "path", "xxx", "path to csv" };
  };
}

#endif
