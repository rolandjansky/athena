#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <vector>

namespace Deconstruction {

  class Parameters {

    public:
      Parameters();
      Parameters(const std::string &input);
      virtual ~Parameters();

      int read(const std::string &input);

      int insert(const std::string &key, const double value);

      double &operator[](const std::string &key);
      double &operator[](const int key);

    protected:
      std::vector<double> m_param;
      std::vector<std::string> m_keys;

  };
}

#endif

