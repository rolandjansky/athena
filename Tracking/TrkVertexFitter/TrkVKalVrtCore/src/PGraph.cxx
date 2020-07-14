/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkVKalVrtCore/PGraph.h"


namespace Trk {

int PGraph::pgraphm_(long int *weit, long int *edges, long int *nodes, 
	long int *set, long int *nptr, long int *nth) noexcept
{

    /* Builtin functions */
//  void s_stop(char *, ftnlen);

    /* Local variables */
    long int node=0, maxl,i__1;
    long int k1,k2,ind,nlink[1000];
    long int i__, k, l, ndmap[1000];




#define teit_ref(a_1,a_2) m_teit[(a_2)*2 + (a_1) - 3]
#define weit_ref(a_1,a_2) weit[(a_2)*2 + (a_1)]


/* * Modified version of CERNLIB V401 PGRAPH routine. */
/* * Created by Igor kachaev, kachaev@mx.ihep.su, 30-Jun-1998. */
/* * Much faster than initial; */
/* * Does correspond to its description; */
/* * Do not return compliment of the solution; */
/* * long int*2 internal SETT,TEIT arrays; */
/* * Parameter NTH is added, SETPTR is excluded (use NPTR) */
/** Case EDGES=0 or NODES=1 (graph is fully compatible) is handled correctly.*/
/* Returns compatible subgraphs from incompatibility graph. */
/*Can be used instead of GRAPH (CERNLIB V401) if subdivision of graph into*/
/* connected subgraphs is not required. Note: */
/* 1. Total memory used internally and in the calling routine */
/*    is about 7*MXNODE^2 bytes = 2.5 Mb for MXNODE = 600. */
/* 2. This algorithm has small internal error, namely all solutions */
/*   returned are correct (compatible) and all correct solutions are found,*/
/*    but some additional solutions are returned, which are subsets */
/*    of some correct solutions (i.e. corresponding coverings of the */
/*    graph are not minimal). */
/* Parameters: */
/* WEIT(2,EDGES) - modify, on input - list of edges between incompatible */
/*   nodes. Each edge is represented by a pair of nodes. */
/*   Used internally as working array, destroyed on output. */
/* EDGES - input, number of edges in the list. */
/* NODES - input, number of nodes in the graph. */
/*   Max. number of nodes is MXNODE, */
/*   Max. number of edges is MXEDGE = MXNODE*(MXNODE-1)/2 */
/* SET   - output, SET(1:NPTR) contains solution, */
/* SET(NPTR+1:NODES) isn't filled now, but can contain complement of the */
/*solution (all nodes not contained in it, "minimal covering" of the graph)*/
/* NPTR  - I/O, on input: */
/*       = 0 for initialization, */
/*       > 0 to get next solution. */
/*       On output: */
/*       > 0 - length of the solution stored in SET() */
/*       = 0 - no more solutions can be found. */
/* NTH   - input, solutions shorter than NTH aren't returned (ignored) */

/* General structure of algorithm is recursion converted into a */
/* stack-driven loop with temp. returns to user with next solution. */
/* TABNR - stack pointer, SETT  - vertex stack, */
/* TEIT - edges stack, LSETT, LTEIT - pointers to SETT,TEIT arrays. */
/* NLINK(I) - number of links in array WEIT() for node I. */
/* NDMAP(I).NE.0 for all recently added nodes in WSET(). */


    /* Parameter adjustments */
    weit -= 3;
    --set;

    /* Function Body */
    if (*nodes <= 0 || *nodes > 1000) {
	goto L999;
    }
    if (*edges < 0 || *edges > 499500) {
	goto L999;
    }
    if (*nodes < *nth) {
	goto L999;
    }
    if (*nptr > 0) {
	switch (m_choice) {
	    case 1:  goto L10;
	    case 2:  goto L20;
	    case 3:  goto L999;
	}
    }
    if (*nodes == 1 || *edges == 0) {
	goto L900;
    }
    m_lweit = *edges;
    m_tabnr = 1;
    m_lwset = 0;
    m_lvset = 0;
    m_lteit[0] = 0;
    m_lsett[0] = 0;

/* Add a node to the VSET list. Select first NODE with the largest */
/* number of links. This can be time-consuming, time about NODES^3. */
/** So we try to comment selection and use first available NODE. This lead
s*/
/* * to the large number of "small" solutions and TEIT stack overflow. */
/* So we optimize this search using temporary memory NLINK(). */

L888:
    i__1 = *nodes;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L61: */
	nlink[i__ - 1] = 0;
    }
    i__1 = m_lweit;
    for (i__ = 1; i__ <= i__1; ++i__) {
	k = weit_ref(1, i__);
	l = weit_ref(2, i__);
	++nlink[k - 1];
	++nlink[l - 1];
/* L62: */
    }
/* ---- */
/* L889: */
    maxl = 0;
    i__1 = *nodes;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (maxl < nlink[i__ - 1]) {
	    maxl = nlink[i__ - 1];
	    node = i__;
	}
/*     NLINK(I) = 0 */
/* L63: */
    }

/* ***   STEP2 */

    ++m_lvset;
    m_vset[m_lvset - 1] = node;

/* ***   STEP 3 + 4 */
/* WSET() - list of nodes connected with selected NODE. */
/* TEIT() - list of edges which DO NOT end on selected NODE. */
/* If no such edges can be found then both VSET and WSET are */
/* complements of the solutions. */
/* K1 - incremental length of TEIT, recalculated from index for speed. */
/* K2 - incremental length of WSET, --"-- */

/*      K1 = 0 */
/*      K2 = 0 */
    ind = m_lteit[m_tabnr - 1];
    i__1 = m_lweit;
    for (i__ = 1; i__ <= i__1; ++i__) {
	k = weit_ref(1, i__);
	l = weit_ref(2, i__);
	if (k == node) {
	    ++m_lwset;
	    m_wset[m_lwset - 1] = l;
/*         K2 = K2 + 1 */
	} else if (l == node) {
	    ++m_lwset;
	    m_wset[m_lwset - 1] = k;
/*         K2 = K2 + 1 */
	} else {
	    ++ind;
	    teit_ref(1, ind) = (short int) k;
	    teit_ref(2, ind) = (short int) l;
/*         K1 = K1 + 1 */
	}
/* L2: */
    }
    k1 = ind - m_lteit[m_tabnr - 1];
    k2 = m_lweit - k1;
/* --   Note that IND.GT.EDGES is possible. */
    if (ind > 499500) {
	return 1;
//	s_stop("PGRAPH: TEIT stack overflow", 27L);
    }
    if (k1 == 0) {
	goto L300;
    }
/* -- */
    ind = m_lsett[m_tabnr - 1];
    i__1 = m_lvset;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++ind;
	m_sett[ind - 1] = (short int) m_vset[i__ - 1];
/* L51: */
    }
    if (ind > 499500) {
	return 2;
//	s_stop("PGRAPH: SETT stack overflow", 27L);
    }

/* ***   STEP 5 */

    ++m_tabnr;
    if (m_tabnr > *nodes) {
	return 3;
//	s_stop("PGRAPH: LSETT stack overflow", 28L);
    }
    m_lsett[m_tabnr - 1] = m_lsett[m_tabnr - 2] + m_lvset;
    m_lteit[m_tabnr - 1] = m_lteit[m_tabnr - 2] + k1;

/* Replace WEIT by the set of edges which are NOT connected with nodes */
/* in WSET (and in VSET too). If there are no such edges, then WSET is */
/*the compliment of the solution, return it to the user, go one level back,*/
/*replace WSET by VSET and restart; else replace WSET by VSET and restart.*/
/* * This search is also optimized using temp. node map NDMAP. */
/*      IEND = LTEIT(TABNR) */
/*      IANF = LTEIT(TABNR - 1) + 1 */
/*      K1 = 0 */
/*      JANF = LWSET - K2 + 1 */
/*      DO 200 I = IANF, IEND */
/*        DO 25 L = JANF,LWSET */
/*          IF(TEIT(1,I) .EQ. WSET(L)) GO TO 200 */
/*          IF(TEIT(2,I) .EQ. WSET(L)) GO TO 200 */
/*   25   CONTINUE */
/*        K1 = K1 + 1 */
/*        WEIT(1,K1) = TEIT(1,I) */
/*        WEIT(2,K1) = TEIT(2,I) */
/*  200 CONTINUE */

    i__1 = *nodes;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L26: */
	ndmap[i__ - 1] = 0;
    }
    i__1 = m_lwset;
    for (i__ = m_lwset - k2 + 1; i__ <= i__1; ++i__) {
/* L27: */
	ndmap[m_wset[i__ - 1] - 1] = 1;
    }
    k1 = 0;
    i__1 = m_lteit[m_tabnr - 1];
    for (i__ = m_lteit[m_tabnr - 2] + 1; i__ <= i__1; ++i__) {
	k = teit_ref(1, i__);
	l = teit_ref(2, i__);
	if (ndmap[k - 1] + ndmap[l - 1] == 0) {
	    ++k1;
	    weit_ref(1, k1) = k;
	    weit_ref(2, k1) = l;
/*         NLINK(K) = NLINK(K) + 1 */
/*         NLINK(L) = NLINK(L) + 1 */
	}
/* L200: */
    }
/* ---- */
    if (k1 == 0) {
	goto L10;
    }
/* ---- */
    i__1 = m_lwset;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L50: */
	m_vset[i__ - 1] = m_wset[i__ - 1];
    }
    m_lvset = m_lwset;
    m_lweit = k1;
    goto L888;

/*  THE STATEMENTS 300 ... 20 RETURN THE SOLUTIONS IN V AND W. */
/*  BEFORE RETURNING, HOWEVER, THE 'COMPLEMENT' OF THE SOLUTION IS */
/*  COMPUTED (= ALL NODES OF THE GRAPH NOT CONTAINED IN THE SOLUTION) */
/*  AND STORED INTO 'SET', FOLLOWED BY THE ACTUAL (CONFER ALGORITHM OF */
/*  S.R. DAS) SOLUTION. */
/** Do not returt set complement; do not return solutions shorter than NTH.*/

L300:
    if (*nodes - m_lvset < *nth) {
	goto L10;
    }
    trevni_(m_vset, &m_lvset, &set[1], nodes, nptr, ndmap);
/*     DO 41 I = 1,LVSET ; NPTR = NPTR + 1 ; 41 SET(NPTR) = VSET(I) */
    m_choice = 1;
    return 0;

L10:
    if (*nodes - m_lwset < *nth) {
	goto L20;
    }
    trevni_(m_wset, &m_lwset, &set[1], nodes, nptr, ndmap);
/*     DO 40 I = 1,LWSET ; NPTR = NPTR + 1 ; 40 SET(NPTR) = WSET(I) */
    m_choice = 2;
    return 0;

/* ***   STEP 6. Go back one level, exit if we are at the first level. */

L20:
    if (m_tabnr == 1) {
	goto L999;
    }
    m_lweit = m_lteit[m_tabnr - 1] - m_lteit[m_tabnr - 2];
    m_lwset = m_lsett[m_tabnr - 1] - m_lsett[m_tabnr - 2];
    m_lvset = m_lwset;
    --m_tabnr;
    ind = m_lteit[m_tabnr - 1];
    i__1 = m_lweit;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++ind;
	weit_ref(1, i__) = teit_ref(1, ind);
	weit_ref(2, i__) = teit_ref(2, ind);
/*       K = TEIT(1,IND) */
/*       L = TEIT(2,IND) */
/*       WEIT(1,I) = K */
/*       WEIT(2,I) = L */
/*       NLINK(K) = NLINK(K) + 1 */
/*       NLINK(L) = NLINK(L) + 1 */
/* L21: */
    }
    ind = m_lsett[m_tabnr - 1];
    i__1 = m_lwset;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++ind;
	l = m_sett[ind - 1];
	m_wset[i__ - 1] = l;
	m_vset[i__ - 1] = l;
/* L22: */
    }
    goto L888;
/* ---- */
L900:
    i__1 = *nodes;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L910: */
	set[i__] = i__;
    }
    *nptr = *nodes;
    m_choice = 3;
    return 0;
/* ---- */
L999:
    *nptr = 0;
    return 0;
} /* pgraphm_ */

#undef weit_ref
#undef teit_ref


 void PGraph::trevni_(long int *from, long int *length, long int *to, 
	long int *maxv, long int *newlng, long int *work) noexcept
{
     long int i__1, i__, k;


/*  *INVERT* PUTS INTO 'TO' ALL NUMBERS 1 ... MAXV WHICH ARE NOT */
/*  CONTAINED IN 'FROM(1)' ... 'FROM(LENGTH)'. 'NEWLNG' SPECIFIES THE */
/*  NUMBER OF ELEMENTS IN 'TO'.  (NEWLNG = MAXV - LENGTH) */
/*  07-Feb-98 [KIA] Version with working array, speed-optimized. */

    /* Parameter adjustments */
    --from;
    --to;
    --work;

    /* Function Body */
    i__1 = *maxv;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L10: */
	work[i__] = 0;
    }
    i__1 = *length;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L20: */
	work[from[i__]] = 1;
    }
    k = 0;
    i__1 = *maxv;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (work[i__] == 0) {
	    ++k;
	    to[k] = i__;
	}
/* L30: */
    }
    *newlng = k;
}

} /* End of VKalVrtCore namespace */

