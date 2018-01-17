/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalMotherMonTool.h
 * Implementation of inner detector global monitoring mother tool
 *
 * @author Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalMotherMonTool.h,v 1.49 2009/04/23 16:55:20 kastanas Exp $
 *
 ****************************************************************************/

#ifndef InDetGlobalMotherTrigMonTool_H
#define InDetGlobalMotherTrigMonTool_H

//Local includes
#include "InDetGlobalMotherMonTool.h"

//Standard C++
#include <vector>
#include <string>


//Predeclarations
class IInterface;
class StatusCode;
class TH1I;
class TH1I_LW;
class TH2I;
class TH2I_LW;
class TProfile;


/// This tool derives from InDetGlobalMonitorToolBase
/// The Mother tool interfaces to all subtools
class InDetGlobalMotherTrigMonTool : public InDetGlobalMotherMonTool
{

 public:
  ///Constructor    
  InDetGlobalMotherTrigMonTool( const std::string & type, 
				const std::string & name,
				const IInterface* parent); 
  
  ///Destructor
  virtual ~InDetGlobalMotherTrigMonTool() {}
  
  virtual StatusCode initialize();  
  
 protected:
    
    StatusCode CheckTriggers();
    
    bool hasTriggerFired(int trigger);
    StatusCode fillTrigAwareHisto(TH1I_LW*, int);
    StatusCode fillTrigAwareHisto(TH2I_LW*, int);
    std::string m_triggerNames[8];  
    StatusCode PrintTriggers();
    bool m_doTrigger;
    
 private:
    int m_firedTriggers[8];
    std::vector<unsigned int> m_activeMenuItems;
};

#endif
