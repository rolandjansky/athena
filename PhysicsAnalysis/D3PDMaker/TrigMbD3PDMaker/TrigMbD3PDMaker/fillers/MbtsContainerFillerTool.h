/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_MBTSCONTAINERFILLERTOOL_H
#define TRIGMBD3PDMAKER_MBTSCONTAINERFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

/* for the getter tool **/
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TileEvent/TileContainer.h"
/************************/

class TileCell;
class TileTBID;

namespace TrigMbD3PD {

/* *************************
 * Getter class            *
 * Not needed in principle * 
 ***************************/
class MbtsCollectionGetterTool
  : public D3PD::SGCollectionGetterTool<TileCellContainer>
{
public:
  MbtsCollectionGetterTool (const std::string& type,
			    const std::string& name,
			    const IInterface* parent)
    : D3PD::SGCollectionGetterTool<TileCellContainer> 
    (type, name, parent){}
};
/***************************/

typedef TileCell MbtsContainerFillerObjectType;
typedef D3PD::BlockFillerTool<MbtsContainerFillerObjectType> MbtsContainerFillerBase;

class MbtsContainerFillerTool
  : public MbtsContainerFillerBase
{
public:
  MbtsContainerFillerTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const MbtsContainerFillerObjectType& p);

private:

  void clearData();

  /** For access to the tile test beam identifiers */
  const TileTBID* m_tileTBID;

  /* NTuple variables **/
  float *m_energy;
  float *m_time;
  float *m_quality;
  int *m_type;
  int *m_channel;
  int *m_module;

}; // class MbtsContainerFillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_MBTSCONTAINERFILLERTOOL_H
