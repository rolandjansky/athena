/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiProfileSvc.h"

LumiProfileSvc::LumiProfileSvc(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_runlumilist()
  , m_scalefactorlist()
  , m_scaleFactorMap()
  , m_currentSF(1.0)
  , m_now(0)
{
  declareProperty("RunLumiList", m_runlumilist,  "List of all IOVTimes ((run << 32) + (0xFFFFFFFF & lumiblock)). Same length as ScaleFactorList." );
  declareProperty("ScaleFactorList", m_scalefactorlist, "List of scale factors for the luminosity. Same length as RunLumiList.");
}

LumiProfileSvc::~LumiProfileSvc()
{
}

StatusCode LumiProfileSvc::initialize()
{
  m_scaleFactorMap.clear();
  const std::vector<uint64_t>& rProp(m_runlumilist.value());
  std::vector<uint64_t>::const_iterator iRL(rProp.begin());
  std::vector<uint64_t>::const_iterator prEnd(rProp.end());
  const std::vector<float>& sProp(m_scalefactorlist.value());
  std::vector<float>::const_iterator iSF(sProp.begin());
  std::vector<float>::const_iterator psEnd(sProp.end());
  if (rProp.size() != sProp.size())
    {
      ATH_MSG_ERROR("Length of RunLumiList differs from length of ScaleFactorList!");
      return StatusCode::FAILURE;
    }
  for( ; iRL != prEnd && iSF != psEnd; ++iRL, ++iSF )
    {
      m_scaleFactorMap.insert(std::map<uint64_t,float>::value_type(*iRL, *iSF));
    }
 return StatusCode::SUCCESS;
}

float LumiProfileSvc::scaleFactor(unsigned int run, unsigned int lumi, bool & updated)
{
  uint64_t now = (( (uint64_t) run << 32) + lumi);
  if (m_now != now)
    {
      updated = true;
      m_now = now;
      std::map<uint64_t,float>::const_iterator iSF = m_scaleFactorMap.find(now);
      if (iSF == m_scaleFactorMap.end())
        {
          ATH_MSG_WARNING("Did not find lumi scale factor for run " << run << " lb " << lumi << "; using cached value.");
        }
      else
        {
          m_currentSF = iSF->second;
        }
      ATH_MSG_DEBUG("Using SF=" << m_currentSF <<" for run=" << run << " lb=" << lumi << ".");
    }
  else
    {
      updated = false;
    }
  return m_currentSF;
}
