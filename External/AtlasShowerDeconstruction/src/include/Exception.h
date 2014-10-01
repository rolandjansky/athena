#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <map>

#define NEW_EXCEPTION(x) Deconstruction::Exception(x, __LINE__, __FILE__)

namespace Deconstruction {

  class Exception {
    public:
      Exception(const std::string &msg, int lineno, const std::string &sourceFile);

      Exception &setParam(const std::string &key, const std::string &value);

      std::string what() const;

    protected:
      std::string m_msg;
      int m_lineno;
      std::string m_sourceFile;

      std::map<std::string, std::string> m_param;

      std::string fixedMsg() const;

      void findAndReplace(std::string msg, const std::string &key, const std::string &value) const;
  };

}

#endif

