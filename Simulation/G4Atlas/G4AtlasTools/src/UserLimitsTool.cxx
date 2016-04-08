/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "UserLimitsTool.h"

// Geant4 includes used in functions
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"

// STL library
#include <boost/tokenizer.hpp>
#include <limits>
UserLimitsTool::UserLimitsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_MaxStep(-1.)
  , m_MinEkine(-1.)
  , m_MaxTrackLength(-1.)
  , m_MaxTime(-1.)
  , m_MinRange(-1.)
{
  declareProperty("MaxStep", m_MaxStep, "Maximum step length");
  declareProperty("MinEkine", m_MinEkine, "Minimum remaining kinetic energy for a track");
  declareProperty("MaxTrackLength", m_MaxTrackLength, "Maximum total track length");
  declareProperty("MaxTime", m_MaxTime, "Maximum global time for a track");
  declareProperty("MinRange", m_MinRange, "Minimum remaining range for a track");
  declareProperty("VolumeList" , m_logicalVolumes , "List of Logical volume to which these limits should be applied" );
}

// Athena method, called at initialization time
StatusCode UserLimitsTool::initialize()
{
  ATH_MSG_INFO(" initializing UserLimitsTool "<<name() );
  G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());

  //Call Limit setting methods here:
  std::vector<std::string>::const_iterator volumeItr(m_logicalVolumes.begin());
  const std::vector<std::string>::const_iterator endOfVolumesItr(m_logicalVolumes.end());
  while(volumeItr!=endOfVolumesItr)
    {
      const std::string& volName(*volumeItr);
      unsigned int ndone(0);
      for (unsigned int i=0;i<lvs.size();i++)
        {
          G4LogicalVolume *lv=lvs[i];
          if (isMatch(volName,lv->GetName()))
            {
              G4UserLimits *ul=lv->GetUserLimits();
              if (!ul) ul=new G4UserLimits;
              if (-0.5 < m_MaxStep)        { ul->SetMaxAllowedStep(m_MaxStep); }
              if (-0.5 < m_MinEkine)       { ul->SetUserMinEkine(m_MinEkine); }
              if (-0.5 < m_MaxTrackLength) { ul->SetUserMaxTrackLength(m_MaxTrackLength); }
              if (-0.5 < m_MaxTime)        { ul->SetUserMaxTime(m_MaxTime); }
              if (-0.5 < m_MinRange)       { ul->SetUserMinRange(m_MinRange); }
              lv->SetUserLimits(ul);
              ++ndone;
            }
        }
      if (-0.5 < m_MaxStep) { ATH_MSG_DEBUG ( "MaxStep set to "<<m_MaxStep<<" to "<<ndone<<" copies of "<<volName ); }
      if (-0.5 < m_MinEkine){ ATH_MSG_DEBUG ( "MinEkine set to "<<m_MinEkine<<" to "<<ndone<<" copies of "<<volName );}
      if (-0.5 < m_MaxTrackLength) { ATH_MSG_DEBUG ( "MaxTrackLength set to "<<m_MaxTrackLength<<" to "<<ndone<<" copies of "<<volName ); }
      if (-0.5 < m_MaxTime)        { ATH_MSG_DEBUG ( "MaxTime set to "<<m_MaxTime<<" to "<<ndone<<" copies of "<<volName ); }
      if (-0.5 < m_MinRange)       { ATH_MSG_DEBUG ( "MinRange set to "<<m_MinRange<<" to "<<ndone<<" copies of "<<volName ); }

      ++volumeItr;
    }
  // TODO would probably be more CPU efficient to loop over the geometry and compare with each limit volume.
  return StatusCode::SUCCESS;
}

bool UserLimitsTool::isMatch(const std::string& a,const std::string b) const
{
  // straightforward cases
  if (a=="*") return true;
  if (a==b) return true;
  // wildcards
  boost::char_separator<char> sep{"*"};
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
  tokenizer tok{a, sep};
  bool returnValue=true;
  std::string temp=b;
  //FIXME This next bit is a bit hacky
  std::vector<std::string> tokens;
  for (const auto& token : tok)
    {
      tokens.push_back(token);
    }
   for (unsigned int i=0;i<tokens.size();i++)
     {
       if (tokens[i].empty()) continue;
       std::string::size_type npos=temp.find(tokens[i]);
       if (i==0 && npos)
         return false;
       if (npos!=std::string::npos)
         {
           temp=temp.substr(npos, temp.size()-npos);
         }
       else
         {
           returnValue=false;
           break;
         }
     }
   if (returnValue && !tokens[tokens.size()-1].empty())
     {
       std::string temp=tokens[tokens.size()-1];
       std::string temp2=b.substr(b.size()-temp.size(),temp.size());
       if (temp!=temp2)
         return false;
     }
  return returnValue;
}

StatusCode
UserLimitsTool::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if ( riid == IUserLimitsTool::interfaceID() )
    {
      *ppvIf = (IUserLimitsTool*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  return AlgTool::queryInterface( riid, ppvIf );
}

