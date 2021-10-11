/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLING_LARHVCABLINGSIMTOOL_H
#define LARCABLING_LARHVCABLINGSIMTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"

#include <vector>
#include <map>

class LArHVLineID;


/**
 * @brief Mapping from HV lines to electrodes.
 *
 * For use only in simulation, where we don't want to use conditions data.
 * Otherwise, use the LArHVIdMapping conditions object.
 *
 * Derived from the original LArHVCablingTool:
 *   date of creation : 07/09/2004 
 *   @author : lucotte@in2p3.fr 
 */
class LArHVCablingSimTool : public AthAlgTool
{
public:
  // constructor
  LArHVCablingSimTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent ) ;
  
  // Destructor
  virtual ~LArHVCablingSimTool() = default;
  
  virtual StatusCode initialize( ) override;
  

  /** Return a vector of LArElectrodeID corresponding to a given LArHVLineID
  */
  const std::vector<HWIdentifier>& getLArElectrodeIDvec(const HWIdentifier& hvlineId) const;



private:
  std::string m_mappingFolder;

  const LArHVLineID *    m_hvlineHelper ;
  std::map<HWIdentifier,std::vector<HWIdentifier> > m_hvline_electrode_map;
};
  	  		  		 
#endif // LARCABLING_LARHVCABLINGSIMTOOL_H
