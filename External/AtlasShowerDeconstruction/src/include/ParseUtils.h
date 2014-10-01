#ifndef PARSEUTILS_H
#define PARSEUTILS_H

#include <string>
#include <getopt.h>

struct extendedOption {
  const char *name;
  int has_arg; // no_argument, required_argument, optional_argument
  int *flag;
  int val;
  const char *description;
  void *pointerToValue;
  enum extOptType { eOTFloat = 0, eOTString, eOTInt } type;
};

void buildOptions(struct extendedOption *extOpt, struct option *&opt, std::string &shortOpts);
void dumpHelp(const std::string &nameOfProgram, struct extendedOption *extOpt, const std::string &description = "");
void dumpOptions(struct extendedOption *extOpt);
bool parseArguments(int argc, char **argv, struct extendedOption *extOpt);

#endif

