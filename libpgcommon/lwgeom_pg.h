/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 *
 * Copyright (C) 2011      Sandro Santilli <strk@keybit.net>
 * Copyright (C) 2009-2010 Paul Ramsey <pramsey@cleverelephant.ca>
 * Copyright (C) 2008      Mark Cave-Ayland <mark.cave-ayland@siriusit.co.uk>
 * Copyright (C) 2004-2007 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

#ifndef _LWGEOM_PG_H
#define _LWGEOM_PG_H 1

#include "postgres.h"
#include "utils/geo_decls.h"
#include "fmgr.h"

#include "../postgis_config.h"

#include "liblwgeom.h"
#include "pgsql_compat.h"

void *pg_alloc(size_t size);
void *pg_realloc(void *ptr, size_t size);
void pg_free(void *ptr);
void pg_error(const char *msg, va_list vp);
void pg_notice(const char *msg, va_list vp);


/* Debugging macros */
#if POSTGIS_DEBUG_LEVEL > 0

/* Display a simple message at NOTICE level */
#define POSTGIS_DEBUG(level, msg) \
        do { \
                if (POSTGIS_DEBUG_LEVEL >= level) \
                        ereport(NOTICE, (errmsg_internal("[%s:%s:%d] " msg, __FILE__, __func__, __LINE__))); \
        } while (0);

/* Display a formatted message at NOTICE level (like printf, with variadic arguments) */
#define POSTGIS_DEBUGF(level, msg, ...) \
        do { \
                if (POSTGIS_DEBUG_LEVEL >= level) \
                        ereport(NOTICE, (errmsg_internal("[%s:%s:%d] " msg, __FILE__, __func__, __LINE__, __VA_ARGS__))); \
        } while (0);

#else

/* Empty prototype that can be optimised away by the compiler for non-debug builds */
#define POSTGIS_DEBUG(level, msg) \
        ((void) 0)

/* Empty prototype that can be optimised away by the compiler for non-debug builds */
#define POSTGIS_DEBUGF(level, msg, ...) \
        ((void) 0)

#endif


/*
 * Standard macro for reporting parser errors to PostgreSQL
 */

extern void pg_parser_errhint(LWGEOM_PARSER_RESULT *lwg_parser_result);
extern void pg_unparser_errhint(LWGEOM_UNPARSER_RESULT *lwg_unparser_result);

#define PG_PARSER_ERROR(lwg_parser_result) \
        do { \
                pg_parser_errhint(&lwg_parser_result); \
        } while(0);

/*
 * Standard macro for reporting unparser errors to PostgreSQL
 */
#define PG_UNPARSER_ERROR(lwg_unparser_result) \
        do { \
                pg_unparser_errhint(&lwg_unparser_result); \
        } while(0);


/*
** GSERIALIED prototypes used outside the index functions
*/

/* Remove the embedded bounding box */
GSERIALIZED* gserialized_drop_gidx(GSERIALIZED *g);




/* Serialize/deserialize a GSERIALIZED (postgis datatype) */
extern GSERIALIZED *pglwgeom_serialize(LWGEOM *lwgeom);
extern LWGEOM *pglwgeom_deserialize(GSERIALIZED *pglwgeom);


/* GSERIALIZED SRID get/set */
extern GSERIALIZED *pglwgeom_set_srid(GSERIALIZED *pglwgeom, int32 newSRID);
extern int pglwgeom_get_srid(GSERIALIZED *pglwgeom);
extern int pglwgeom_get_type(const GSERIALIZED *lwgeom);
extern int pglwgeom_get_zm(const GSERIALIZED *lwgeom);
extern GSERIALIZED* pglwgeom_drop_bbox(GSERIALIZED *geom);
extern size_t pglwgeom_size(const GSERIALIZED *geom);
extern int pglwgeom_ndims(const GSERIALIZED *geom);
extern bool pglwgeom_has_bbox(const GSERIALIZED *lwgeom);
extern bool pglwgeom_has_z(const GSERIALIZED *lwgeom);
extern bool pglwgeom_has_m(const GSERIALIZED *lwgeom);
extern int pglwgeom_is_empty(const GSERIALIZED *geom);
/*
 * Get the 2d bounding box of the given geometry, in FLOAT4 format.
 * Use a cached bbox if available, compute it otherwise.
 * Return LW_FALSE if the geometry has no bounding box (ie: is empty).
 */
extern int pglwgeom_getbox2d_p(const GSERIALIZED *geom, GBOX *box);

/* PG-dependant */

/**
* Utility to convert cstrings to textp pointers 
*/
text* cstring2text(const char *cstring);
char* text2cstring(const text *textptr);

/*
 * Use this macro to extract the char * required
 * by most functions from an GSERIALIZED struct.
 * (which is an GSERIALIZED w/out int32 size casted to char *)
 */
#define SERIALIZED_FORM(x) ((uint8_t *)VARDATA((x)))

/* 
 * For PostgreSQL >= 8.5 redefine the STATRELATT macro to its
 * new value of STATRELATTINH 
 */
#if POSTGIS_PGSQL_VERSION >= 85
	#define STATRELATT STATRELATTINH
#endif

/* PG-exposed */
Datum BOX2D_same(PG_FUNCTION_ARGS);
Datum BOX2D_overlap(PG_FUNCTION_ARGS);
Datum BOX2D_overleft(PG_FUNCTION_ARGS);
Datum BOX2D_left(PG_FUNCTION_ARGS);
Datum BOX2D_right(PG_FUNCTION_ARGS);
Datum BOX2D_overright(PG_FUNCTION_ARGS);
Datum BOX2D_overbelow(PG_FUNCTION_ARGS);
Datum BOX2D_below(PG_FUNCTION_ARGS);
Datum BOX2D_above(PG_FUNCTION_ARGS);
Datum BOX2D_overabove(PG_FUNCTION_ARGS);
Datum BOX2D_contained(PG_FUNCTION_ARGS);
Datum BOX2D_contain(PG_FUNCTION_ARGS);
Datum BOX2D_intersects(PG_FUNCTION_ARGS);
Datum BOX2D_union(PG_FUNCTION_ARGS);

Datum LWGEOM_same(PG_FUNCTION_ARGS);
Datum BOX3D_construct(PG_FUNCTION_ARGS);

Datum LWGEOM_force_2d(PG_FUNCTION_ARGS);
Datum LWGEOM_force_3dm(PG_FUNCTION_ARGS);
Datum LWGEOM_force_3dz(PG_FUNCTION_ARGS);
Datum LWGEOM_force_4d(PG_FUNCTION_ARGS);
Datum LWGEOM_force_collection(PG_FUNCTION_ARGS);
Datum LWGEOM_force_multi(PG_FUNCTION_ARGS);

Datum LWGEOMFromWKB(PG_FUNCTION_ARGS);
Datum WKBFromLWGEOM(PG_FUNCTION_ARGS);

Datum LWGEOM_getBBOX(PG_FUNCTION_ARGS);
Datum LWGEOM_addBBOX(PG_FUNCTION_ARGS);
Datum LWGEOM_dropBBOX(PG_FUNCTION_ARGS);

#endif /* !defined _LWGEOM_PG_H 1 */
