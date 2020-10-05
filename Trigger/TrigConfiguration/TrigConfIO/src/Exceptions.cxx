#include "TrigConfIO/Exceptions.h"

TrigConf::IOException::IOException(std::string msg) : m_msg(std::move(msg))
{}

const char*
TrigConf::IOException::what() const noexcept
{
   return m_msg.c_str();
}
