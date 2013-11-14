/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Analysis_h
#define Analysis_h
#include "HasEvt.h"
#include "TString.h"
#include "AANTMisc.h"

#include "Assoc.h"

class Analysis : public HasEvt {
public: 
  Analysis();
  virtual ~Analysis( ); 

public:
///////////////////////////////////
    virtual void initialize(char* option) = 0 ;
    virtual void execute()                = 0 ;
    virtual void finalize()               = 0 ;

    void setAssoCuts(int nsigAng, int nsigMom);
    bool isInsideAcceptance(Assoc* asso, float eta);

    static void setComment(char* comm);
    static void doAssoc();
    static void closeAssoc();

    float m_PI ;
    int   m_LARGPS ;
    int   m_HAUTPS ;

    static char*  m_comment;
    static Assoc* p_assocMB;
    static Assoc* p_assocMS;
    static Assoc* p_assocST;
    static Assoc* p_assocMU;
    static Assoc* p_assocID;
    static Assoc* p_assocMI;
    static int    m_Nassoc;

};
#endif // 
