/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/CLinksAOD.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of AOD objects.
 */


#include "DataModelTestDataCommon/CLinksAOD.h"


namespace DMTest {


const std::vector<ElementLink<CVec> >& CLinksAOD::vel() const
{
  return m_vel;
}


void CLinksAOD::setVel (const std::vector<ElementLink<CVec> >& vel)
{
  m_vel = vel;
}


void CLinksAOD::setVel (std::vector<ElementLink<CVec> >&& vel)
{
  m_vel = std::move(vel);
}


const ElementLinkVector<CVec>& CLinksAOD::elv() const
{
  return m_elv;
}


ElementLinkVector<CVec>& CLinksAOD::elv()
{
  return m_elv;
}


void CLinksAOD::setElv (const ElementLinkVector<CVec>& elv)
{
  m_elv = elv;
}


} // namespace DMTest
