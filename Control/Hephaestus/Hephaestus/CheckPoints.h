/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_CHECKPOINTS_H
#define HEPHAESTUS_CHECKPOINTS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int CheckPointPos_t;

/* set a new check point, which will become current */
int hhh_setCheckPoint( void );

/* consolidate the check point with the general memory tracking */
int hhh_clearCheckPoint( CheckPointPos_t );

/* print out statistics for the given check point */
int hhh_check( CheckPointPos_t );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !HEPHAESTUS_CHECKPOINTS_H */
