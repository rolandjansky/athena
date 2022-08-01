#include <iostream>
#include <string>
#include "channelMappings/EfexHardwareInfo.h"
EfexHardwareInfo::EfexHardwareInfo()
:m_valid(false), m_efexlabel("invalid") , m_fibre(-1), m_inputconnector(-1),
m_mpodlabel("invalid")
{    
}
EfexHardwareInfo::EfexHardwareInfo(std::string efexlabel,
                                int fibre,
                                int inputconnector,
                                std::string mpod)
:m_valid(true), m_efexlabel(efexlabel) , m_fibre(fibre), m_inputconnector(inputconnector),
m_mpodlabel(mpod),m_overlap(-99)
{    
}
std::string EfexHardwareInfo::getEFEXLabel() const
{
    return m_efexlabel;
}
int         EfexHardwareInfo::getFibreNumber() const
{
    return m_fibre;
}
int         EfexHardwareInfo::getRibbonFibreNumber() const
{    
    return 11-(m_fibre)%12; // Ribbon fibre # flip (hopefully right!)
}
int         EfexHardwareInfo::getInputConnector() const
{
    return m_inputconnector;
}
int         EfexHardwareInfo::getMpodNumber() const
{
    int mpodnum = int(m_mpodlabel.at(1)) -48;
    if (m_mpodlabel.at(0) == 'H') mpodnum += 10;
    return mpodnum;
}
std::string EfexHardwareInfo::getMpodLabel() const
{
    return m_mpodlabel;
}
bool        EfexHardwareInfo::getValidity() const
{
    return m_valid;
}
void        EfexHardwareInfo::setOverlap(int overlap)
{
    m_overlap = overlap;
}
int         EfexHardwareInfo::getOverlap() const
{
    return m_overlap;
}
void        EfexHardwareInfo::printInfo() const
{    
    std::cout << "EfexHardwareInfo Object:" << std::endl;
    if(m_valid){
        std::cout << "EFEX: " << this->getEFEXLabel()  << std::endl;
        std::cout << "Fibre: " << std::to_string(this->getFibreNumber())  << std::endl;
        std::cout << "Input Connector: " << std::to_string(this->getInputConnector())  << std::endl;
        std::cout << "MpodLabel: " << this->getMpodLabel()  << std::endl;
    }
    else{
        std::cout << "Invalid/Unused Connection" << std::endl;
    }
    std::cout << std::endl;
}
