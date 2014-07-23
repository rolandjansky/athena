/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file  read_statm.h
 * @brief returns statm memory statistics for current process (see man proc)
 * @param vm_pages total program size in unit of pages
 * @param rss_pages resident set size in unit of pages
 * @author Sebastien Binet, Paolo Calafiura
 * $Id: read_athena_statm.h 592439 2014-04-10 17:35:29Z calaf $
 */
struct athena_statm {
  unsigned int vm_pages;
  unsigned int rss_pages;
};
#ifdef __cplusplus
extern "C"
#endif
struct athena_statm read_athena_statm();
