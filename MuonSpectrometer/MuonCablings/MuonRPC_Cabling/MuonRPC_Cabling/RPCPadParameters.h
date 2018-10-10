/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************

 -----------------------------------------
 ***************************************************************************/

#ifndef MUONRPC_CABLING_RPCPADPARAMETERS_H
# define MUONRPC_CABLING_RPCPADPARAMETERS_H
#include <iostream>

// This class contains the trigger parameters for a given Pad (tower)

class RPCPadParameters
{
private:
    bool m_feet_on;   // special treatment for feet towers: true=on false=off
    bool m_eta_and_phi; // true=eta-AND-phi    false=eta-OR-phi
    unsigned short int  m_feet_thresholds; //thresholds for feet towers : th1+ 8*th2 + 64*th3
    unsigned short int  m_cma_mask;  // cma mask

public:
    RPCPadParameters();
    RPCPadParameters(const RPCPadParameters&);
    ~RPCPadParameters();
    RPCPadParameters& operator=(const RPCPadParameters&);

    bool feet_on(void) const {return m_feet_on;};
    bool eta_and_phi(void) const { return m_eta_and_phi;};
    unsigned short int  feet_thresholds(void) const {return m_feet_thresholds;};
    unsigned short int  cma_mask(void) const {return m_cma_mask;};
    
    void set_feet_on(bool feet_on) {m_feet_on=feet_on;};
    void set_eta_and_phi(bool eta_and_phi) {m_eta_and_phi=eta_and_phi;};
    void set_cma_mask(unsigned int mask) {m_cma_mask=mask;};
    
    void set_phi_only() { m_cma_mask=0x33; m_eta_and_phi=false; };
    void set_eta_only() { m_cma_mask=0xcc; m_eta_and_phi=false; };
    
    unsigned short int feet_threshold(unsigned short int it) const;
    bool set_feet_threshold(unsigned short int it, unsigned short int th);

    void Print(std::ostream& stream);
    
};



#endif // MUONRPC_CABLING_RPCPADPARAMETERS_H
