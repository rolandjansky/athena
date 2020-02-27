


#include "TrigEgammaMatchingTool/TrigEgammaMatchingUtils.h"


using namespace TrigEgammaMatchingUtils;

 
Element::Element(): m_name(""), m_te(nullptr), m_data(nullptr)
{}

Element::Element(std::string name): m_name(name), m_te(nullptr), m_data(nullptr)
{}

Element::Element(std::string name, const xAOD::TrigComposite *data): m_name(name), m_te(nullptr), m_data(data)
{}

Element::Element(std::string name, const HLT::TriggerElement *te): m_name(name), m_te(te), m_data(nullptr)
{}
  

