/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_KEYMIXIN_H
#define ATHEXJOBOPTIONS_KEYMIXIN_H 1

class AthAlgorithm;

#include <string>


/////////////////////////////////////////////////////////////////////////////

class KeyMixin {
public:
   KeyMixin( AthAlgorithm* );

protected:
   std::string getInputKey() const { return m_inputKey; }
   std::string getOutputKey() const { return m_outputKey; }

private:
   std::string m_inputKey;
   std::string m_outputKey;
};

#endif // !ATHEXJOBOPTIONS_KEYMIXIN_H
