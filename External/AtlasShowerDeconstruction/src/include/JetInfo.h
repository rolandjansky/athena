#ifndef JETINFO_H
#define JETINFO_H

#include <fastjet/PseudoJet.hh>

namespace Deconstruction {

  class JetInfo : public fastjet::PseudoJet::UserInfoBase {
    public:
      JetInfo();
      JetInfo(unsigned int i, int user_index);
      virtual ~JetInfo();

      unsigned int &i();
      int &user_index();

      const unsigned int &i() const;
      const int &user_index() const;

    private:
      unsigned int m_i;
      int m_user_index;
  };

  bool lessThanIndex(const fastjet::PseudoJet &i, const fastjet::PseudoJet &j);

}

#endif

