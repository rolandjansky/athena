/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** -*- C -*-
 * AthDsoCbk.h  
 * Header file for the dso callbacks api 
 * Author: S.Binet<binet@cern.ch> 
 */

#ifndef CXXUTILS_ATHDSOCBK_H
#define CXXUTILS_ATHDSOCBK_H 1

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* forward declares */

/* type of events the dso-callback framework will dispatch */
struct ath_dso_event
{
  const char* fname; /* the name of the dso being loaded */
  int step;          /* 0: before loading the dso
                        1: after  loading the dso */
};

/* type of the callback function the dso logger will execute 
 *  NOTE: userdata pointer may be NULL
 */
typedef int (*ath_dso_event_cbk_t)(const struct ath_dso_event*, void *userdata);

/* register a callback function with the dso-cbk framework 
 * @return 0 on success
 *        -1 on failure
 */
int ath_dso_cbk_register(ath_dso_event_cbk_t cbk, void *userdata);

/* unregister a callback function with the dso-cbk framework
 * @return 0 on success
 *        -1 on failure
 */
int ath_dso_cbk_unregister(ath_dso_event_cbk_t cbk);

#ifdef __cplusplus
} /* extern C */
#endif /*__cplusplus */

#endif /* !CXXUTILS_ATHDSOCBK_H */

