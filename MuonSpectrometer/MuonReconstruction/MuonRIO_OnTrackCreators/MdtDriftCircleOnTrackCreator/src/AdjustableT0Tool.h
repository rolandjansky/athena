/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AdjustableT0Tool_H
#define AdjustableT0Tool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class AdjustableT0Map;

/////////////////////////////////////////////////////////////////////////////
#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"

  /**
   @class AdjustableT0Tool

   This class is used to adjust the T0 shift. 
   It is highly inspired from MuGirlStau code
   It derives from IAdjustableT0Tool
   
  @author samusog@cern.ch
  
  */

namespace AdjT0
{

class AdjustableT0Tool : public AthAlgTool, virtual public IAdjustableT0Tool{
public:
    AdjustableT0Tool(const std::string&,const std::string&,const IInterface*);
    virtual ~AdjustableT0Tool() ;

public:
///////////////////////////////////

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual void SetVerboset0shift() ;
  
  virtual void DumpT0Shift() ;
  
  virtual void SetDefaultTShift( double TShift ) ;
  virtual void ResetSetTShift( double Tshift = 0. ) ;
  virtual void SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift )  ;

  virtual double GetDefaultTShift() const;
  virtual int    NberOfData() const;
  virtual void   GetData(int Irank, int& stationName, int& stationEta, int& stationPhi, double& TShift ) const;

  virtual void SetEnable();
  virtual void SetDisable();

  virtual double timeOfFlight( const Identifier& id, const Amg::Vector3D& pos ) const;

private:
///////////////////////////////////
  int m_DoTof ;

  double m_inverseSpeedOfLight;
  
  int m_Verbose;
  
  AdjustableT0Map* p_AdjustableT0Map ; //!< Pointer on AdjustableT0Map

};

}

#endif

