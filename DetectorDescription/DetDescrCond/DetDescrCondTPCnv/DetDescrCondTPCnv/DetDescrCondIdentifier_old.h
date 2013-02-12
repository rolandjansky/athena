/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_H 
#define DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_H 
//
// DetDescrCondIdentifier_old.h - class used to make a
// TVirtualConverter to read in an Identifier object with an unsigned
// int and convert it to an Identifier with an unsigned long long
//
// RD Schaffer, started 23/02/12
//

class DetDescrCondIdentifier_old {
public:
    unsigned int  m_id;
};

#endif // DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_H 
