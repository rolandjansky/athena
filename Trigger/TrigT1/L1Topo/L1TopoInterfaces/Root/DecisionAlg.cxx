//  DecisionAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/15/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoInterfaces/DecisionAlg.h"

TCS::StatusCode
TCS::DecisionAlg::doReset() {
   return StatusCode::SUCCESS;
}

TCS::DecisionAlg::~DecisionAlg() {}

TCS::StatusCode
TCS::DecisionAlg::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                          const std::vector<TCS::TOBArray *> & output,
                                          Decision & decision )
{
   return process(input,output,decision);
}

TCS::DecisionAlg& TCS::DecisionAlg::setHardwareBits(const unsigned int &bitNumber,
                                                    const bool &valueDecision,
                                                    const bool &valueOverflow)
{
    if(bitNumber<m_numberOutputBits){
        if(valueDecision)
            m_hardwareDecisionBits |= (1 << bitNumber);
        if(valueOverflow)
            m_hardwareOverflowBits |= (1 << bitNumber);
    } else if(bitNumber<32) {
        TRG_MSG_WARNING("Cannot set hardware bits for bit number "<<bitNumber
                        <<" > "<<m_numberOutputBits<<" N output bits "
                        <<" for algo "<<name());
    } else {
        TRG_MSG_WARNING("Cannot set hardware bits for bit number "<<bitNumber
                        <<" > 31 N output bits for algo "<<name());
    }
    return *this;
}
//----------------------------------------------------------
TCS::DecisionAlg& TCS::DecisionAlg::resetHardwareBits()
{
    m_hardwareDecisionBits = 0;
    m_hardwareOverflowBits = 0;
    return *this;
}
//----------------------------------------------------------
bool TCS::DecisionAlg::getDecisionHardwareBit(const unsigned int &bitNumber) const
{
    bool value = false;
    if(bitNumber<m_numberOutputBits){
        value = (m_hardwareDecisionBits >> bitNumber) & 1;
    } else {
        TRG_MSG_WARNING("Cannot get hardware decision bit "<<bitNumber
                        <<" > "<<m_numberOutputBits<<" N output bits "
                        <<" for algo "<<name());
    }
    return value;
}
//----------------------------------------------------------
bool TCS::DecisionAlg::getOverflowHardwareBit(const unsigned int &bitNumber) const
{
    bool value = false;
    if(bitNumber<m_numberOutputBits){
        value = (m_hardwareOverflowBits >> bitNumber) & 1;
    } else {
        TRG_MSG_WARNING("Cannot get hardware overflow bit "<<bitNumber
                        <<" > "<<m_numberOutputBits<<" N output bits "
                        <<" for algo "<<name());
    }
    return value;
}
//----------------------------------------------------------
TCS::DecisionAlg& TCS::DecisionAlg::setFillHistosBasedOnHardware(const bool &value)
{
    m_fillHistosBasedOnHardware = value;
    return *this;
}
//----------------------------------------------------------
bool TCS::DecisionAlg::fillHistosBasedOnHardware() const
{
    return m_fillHistosBasedOnHardware;
}
//----------------------------------------------------------
TCS::DecisionAlg& TCS::DecisionAlg::setSkipHistos(const bool &value)
{
    m_skipHistos = value;
    return *this;
}
//----------------------------------------------------------
bool TCS::DecisionAlg::skipHistos() const
{
    return m_skipHistos;
}
//----------------------------------------------------------
bool TCS::DecisionAlg::fillHistos() const
{
    return (not m_fillHistosBasedOnHardware or not m_skipHistos);
}
//----------------------------------------------------------
