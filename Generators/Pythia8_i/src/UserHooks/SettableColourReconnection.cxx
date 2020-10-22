/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/ColourReconnectionHooks.h"
#include "UserSetting.h"

namespace Pythia8{

  class SettableColourReconnectionBase : public UserHooks{
    
    public:

    SettableColourReconnectionBase() : m_crHook(0){}
    
    virtual ~SettableColourReconnectionBase(){
      if(m_crHook != 0) delete m_crHook;
    }
    
    // Allow colour reconnection after resonance decays (early or late)...
    bool canReconnectResonanceSystems() {return true;}
    
    bool doReconnectResonanceSystems(int oldSizeEvent, Event& event) {
      
      if(m_crHook == 0){
        _init();
        PYTHIA8_INITUSERHOOK_WRAP(m_crHook);
      }
      return m_crHook->doReconnectResonanceSystems(oldSizeEvent, event);
    }
    
    protected:
    
    virtual void _init() = 0;
    
    UserHooks *m_crHook;
    
    
  };
  //////////////////////////////////////////////////////////////////////
  // Minimum bias colour reconnection hook
  class SettableMBColourReconnection : public SettableColourReconnectionBase{
    
    public:
    SettableMBColourReconnection() :  SettableColourReconnectionBase(),
                                      m_modeIn("MBReconnection:Mode", 0),
                                      m_flipIn("MBReconnection:Flip", 0),
                                      m_dLamCutIn("MBReconnection:DeltaLambda", 0.),
                                      m_fracGluonIn("MBReconnection:GluonFraction", 1.){}
    
    protected:
    
    void _init(){
      if(m_crHook != 0) return;
      m_crHook = new MBReconUserHooks(m_modeIn(settingsPtr), m_flipIn(settingsPtr), m_dLamCutIn(settingsPtr), m_fracGluonIn(settingsPtr));
      return;
    }
    
    private:
    
    Pythia8_UserHooks::UserSetting<int> m_modeIn;
    Pythia8_UserHooks::UserSetting<int> m_flipIn;
    Pythia8_UserHooks::UserSetting<double> m_dLamCutIn;
    Pythia8_UserHooks::UserSetting<double> m_fracGluonIn;
    
  };
  
  //////////////////////////////////////////////////////////////////////
  // Top colour reconnection hook
  class SettableTopColourReconnection : public SettableColourReconnectionBase{
    
  public:
    SettableTopColourReconnection() :  SettableColourReconnectionBase(),
                                      m_modeIn("TopReconnection:Mode", 0),
                                      m_strengthIn("TopReconnection:Strength", 1){}
    
  protected:
    
    void _init(){
      if(m_crHook != 0) return;
      m_crHook = new TopReconUserHooks(m_modeIn(settingsPtr), m_strengthIn(settingsPtr));
      return;
    }
    
  private:
    
    Pythia8_UserHooks::UserSetting<int> m_modeIn;
    Pythia8_UserHooks::UserSetting<int> m_strengthIn;
    
  };
  
  
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::SettableMBColourReconnection> settableMBColourReconnectionCreator("MBReconUserHooks_Unvalidated");
Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::SettableTopColourReconnection> settableTopColourReconnectionCreator("TopReconUserHooks_Unvalidated");
