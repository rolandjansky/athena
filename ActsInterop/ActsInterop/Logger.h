#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "GaudiKernel/MsgStream.h"

#include <memory>


#define ACTS_ATH_LOGGER(_name) Acts::makeAthenaLogger(msgSvc().get(), name(), _name)

namespace Acts {

class AthenaPrintPolicy : public Logging::OutputPrintPolicy
{
public:

  AthenaPrintPolicy(std::shared_ptr<MsgStream> msg) : p_msg(msg) {}

  void
  flush(const Logging::Level& lvl, const std::ostringstream& input);

private:
  std::shared_ptr<MsgStream> p_msg;
};

class AthenaFilterPolicy : public Logging::OutputFilterPolicy {
public:
  AthenaFilterPolicy(std::shared_ptr<MsgStream> msg) : p_msg(msg) {}

  bool doPrint(const Acts::Logging::Level& lvl) const;

private:
  std::shared_ptr<MsgStream> p_msg;
  MSG::Level m_currentLevel;
};

                                      
std::unique_ptr<const Logger>
getDefaultLogger(const std::string&    name,
                 const Logging::Level& lvl,
                 std::ostream*         log_stream);

std::unique_ptr<const Logger>
makeAthenaLogger(MsgStream& msg);

std::unique_ptr<const Logger>
makeAthenaLogger(IMessageSvc *svc, const std::string& parent_name, const std::string& name);


}  // end of namespace Acts
