/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

namespace Trk {

int pgraphm_(long int *weit, long int *edges, long int *nodes, 
	long int *set, long int *nptr, long int *nth)
{

    /* Builtin functions */
//  void s_stop(char *, ftnlen);

    /* Local variables */
    static short int teit[999000]	/* was [2][499500] */, sett[499500];
    static long int vset[1000], wset[1000];
    static long int tabnr;
    static long int lteit[1000], lweit, lsett[1000];
    static long int lvset, lwset,choice;

    long int node=0, maxl,i__1;
    long int k1,k2,ind,nlink[1000];
    long int i__, k, l, ndmap[1000];

    void trevni_(long int *, long int *, long int *, long int *, long int *, long int *);


#define teit_ref(a_1,a_2) teit[(a_2)*2 + a_1 - 3]
#define weit_ref(a_1,a_2) weit[(a_2)*2 + a_1]


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
	switch (choice) {
	    case 1:  goto L10;
	    case 2:  goto L20;
	    case 3:  goto L999;
	}
    }
    if (*nodes == 1 || *edges == 0) {
	goto L900;
    }
    lweit = *edges;
    tabnr = 1;
    lwset = 0;
    lvset = 0;
    lteit[0] = 0;
    lsett[0] = 0;

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
    i__1 = lweit;
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

    ++lvset;
    vset[lvset - 1] = node;

/* ***   STEP 3 + 4 */
/* WSET() - list of nodes connected with selected NODE. */
/* TEIT() - list of edges which DO NOT end on selected NODE. */
/* If no such edges can be found then both VSET and WSET are */
/* complements of the solutions. */
/* K1 - incremental length of TEIT, recalculated from index for speed. */
/* K2 - incremental length of WSET, --"-- */

/*      K1 = 0 */
/*      K2 = 0 */
    ind = lteit[tabnr - 1];
    i__1 = lweit;
    for (i__ = 1; i__ <= i__1; ++i__) {
	k = weit_ref(1, i__);
	l = weit_ref(2, i__);
	if (k == node) {
	    ++lwset;
	    wset[lwset - 1] = l;
/*         K2 = K2 + 1 */
	} else if (l == node) {
	    ++lwset;
	    wset[lwset - 1] = k;
/*         K2 = K2 + 1 */
	} else {
	    ++ind;
	    teit_ref(1, ind) = (short int) k;
	    teit_ref(2, ind) = (short int) l;
/*         K1 = K1 + 1 */
	}
/* L2: */
    }
    k1 = ind - lteit[tabnr - 1];
    k2 = lweit - k1;
/* --   Note that IND.GT.EDGES is possible. */
    if (ind > 499500) {
	return 1;
//	s_stop("PGRAPH: TEIT stack overflow", 27L);
    }
    if (k1 == 0) {
	goto L300;
    }
/* -- */
    ind = lsett[tabnr - 1];
    i__1 = lvset;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++ind;
	sett[ind - 1] = (short int) vset[i__ - 1];
/* L51: */
    }
    if (ind > 499500) {
	return 2;
//	s_stop("PGRAPH: SETT stack overflow", 27L);
    }

/* ***   STEP 5 */

    ++tabnr;
    if (tabnr > *nodes) {
	return 3;
//	s_stop("PGRAPH: LSETT stack overflow", 28L);
    }
    lsett[tabnr - 1] = lsett[tabnr - 2] + lvset;
    lteit[tabnr - 1] = lteit[tabnr - 2] + k1;

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
    i__1 = lwset;
    for (i__ = lwset - k2 + 1; i__ <= i__1; ++i__) {
/* L27: */
	ndmap[wset[i__ - 1] - 1] = 1;
    }
    k1 = 0;
    i__1 = lteit[tabnr - 1];
    for (i__ = lteit[tabnr - 2] + 1; i__ <= i__1; ++i__) {
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
    i__1 = lwset;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L50: */
	vset[i__ - 1] = wset[i__ - 1];
    }
    lvset = lwset;
    lweit = k1;
    goto L888;

/*  THE STATEMENTS 300 ... 20 RETURN THE SOLUTIONS IN V AND W. */
/*  BEFORE RETURNING, HOWEVER, THE 'COMPLEMENT' OF THE SOLUTION IS */
/*  COMPUTED (= ALL NODES OF THE GRAPH NOT CONTAINED IN THE SOLUTION) */
/*  AND STORED INTO 'SET', FOLLOWED BY THE ACTUAL (CONFER ALGORITHM OF */
/*  S.R. DAS) SOLUTION. */
/** Do not returt set complement; do not return solutions shorter than NTH.*/

L300:
    if (*nodes - lvset < *nth) {
	goto L10;
    }
    trevni_(vset, &lvset, &set[1], nodes, nptr, ndmap);
/*     DO 41 I = 1,LVSET ; NPTR = NPTR + 1 ; 41 SET(NPTR) = VSET(I) */
    choice = 1;
    return 0;

L10:
    if (*nodes - lwset < *nth) {
	goto L20;
    }
    trevni_(wset, &lwset, &set[1], nodes, nptr, ndmap);
/*     DO 40 I = 1,LWSET ; NPTR = NPTR + 1 ; 40 SET(NPTR) = WSET(I) */
    choice = 2;
    return 0;

/* ***   STEP 6. Go back one level, exit if we are at the first level. */

L20:
    if (tabnr == 1) {
	goto L999;
    }
    lweit = lteit[tabnr - 1] - lteit[tabnr - 2];
    lwset = lsett[tabnr - 1] - lsett[tabnr - 2];
    lvset = lwset;
    --tabnr;
    ind = lteit[tabnr - 1];
    i__1 = lweit;
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
    ind = lsett[tabnr - 1];
    i__1 = lwset;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++ind;
	l = sett[ind - 1];
	wset[i__ - 1] = l;
	vset[i__ - 1] = l;
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
    choice = 3;
    return 0;
/* ---- */
L999:
    *nptr = 0;
    return 0;
} /* pgraphm_ */

#undef weit_ref
#undef teit_ref


 void trevni_(long int *from, long int *length, long int *to, 
	long int *maxv, long int *newlng, long int *work)
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
    return;
}

} /* End of VKalVrtCore namespace */

