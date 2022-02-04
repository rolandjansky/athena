/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***********************************************************************
//                                 eFEXFormTOBs.h
//                                 --------------
//     begin                       : 30 04 2021
//     email                       : nicholas.andrew.luongo@cern.ch
//***********************************************************************

#ifndef eFEXFORMTOBS_H
#define eFEXFORMTOBS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXFormTOBs.h"

namespace LVL1 {

  //Doxygen class description below:
  /** The eFEXFormTOBs class provides functions for creating TOBs for eFEX objects
  */

  class eFEXFormTOBs : public AthAlgTool, virtual public IeFEXFormTOBs {

  public:
    /** Constructors */
    eFEXFormTOBs(const std::string& type, const std::string& name, const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** Destructor */
    virtual ~eFEXFormTOBs();

    virtual uint32_t formTauTOBWord(int &, int &, int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &) override;

    virtual uint32_t formEmTOBWord(int &, int &, int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &, unsigned int &) override;

    /** Internal data */
  private:
    const unsigned int m_eFexStep = 25;
    const unsigned int m_eFexTobStep = 100;
  };

} // end of namespace

CLASS_DEF( LVL1::eFEXFormTOBs , 261506707 , 1 )

#endif
